/*****************************************************************//**
 * \file   D3D12_CommandList.h
 * \brief  DiectX12レンダーコマンドリスト
 *
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/

#include "D3D12_CommandList.h"
#include "D3D12_Renderer.h"
#include "D3D12_Device.h"

#include "D3D12_Buffer.h"
#include "D3D12_RenderBuffer.h"
#include "D3D12_Texture.h"
#include "D3D12_RenderTarget.h"
#include "D3D12_DepthStencil.h"

#include <Renderer/Core/Core_SubResource.h>

#include <functional>

using namespace core;
using namespace d3d12;


//------------------------------------------------------------------------------
// public methods 
//------------------------------------------------------------------------------

/// @brief コンストラクタ
D3D12CommandList::D3D12CommandList() :
	m_pRenderer(nullptr),
	m_pDevice(nullptr)
{
}

/// @brief 初期化処理
/// @param pRenderer D3D12レンダラーポインタ
/// @param pDevice D3D12デバイスポインタ
/// @return 初期化: 成功 true | 失敗 false
HRESULT D3D12CommandList::initialize(D3D12Renderer* pRenderer, D3D12Device* pDevice)
{
	//--- 初期化
	m_pRenderer = pRenderer;
	m_pDevice = pDevice;

	//----- コマンドキュー・コマンドアロケーター・コマンドリスト -----
	HRESULT hr = S_OK;
	D3D12_COMMAND_LIST_TYPE cmdType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	UINT nodeMask = 0;

	// コマンドアロケーターの生成
	{
		hr = pDevice->m_pD3DDevice->CreateCommandAllocator(cmdType,
			IID_PPV_ARGS(m_pCmdAllocator.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
	}

	// コマンドリストの生成
	{
		hr = pDevice->m_pD3DDevice->CreateCommandList(nodeMask, cmdType, m_pCmdAllocator.Get(),
			nullptr, IID_PPV_ARGS(m_pCmdList.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
		m_pCmdList->Close();
	}

	CHECK_FAILED(m_pCmdAllocator->Reset());
	CHECK_FAILED(m_pCmdList->Reset(m_pCmdAllocator.Get(), nullptr));

	return S_OK;
}

//----- リソース指定命令 -----

void D3D12CommandList::setMaterial(const core::MaterialID& materialID)
{
	// マテリアルの取得
	auto* d3dMat = static_cast<D3D12Material*>(m_pDevice->getMaterial(materialID));
	if (d3dMat == nullptr) return;

	// グラフィックスパイプラインステートの指定
	setGraphicsPipelineState(d3dMat->m_shaderID, 
		d3dMat->m_blendState, d3dMat->m_rasterizeState, d3dMat->m_depthStencilState);

	// ステージごと
	UINT rootIndex = 0;
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<std::size_t>(stage);

		// コンスタントバッファ更新
		for (auto& cb : d3dMat->m_d3dCbuffer[stageIndex])
		{
			auto& cbData = d3dMat->m_cbufferData[stageIndex][cb.first];
			if (cbData.isUpdate)
			{
				m_pDevice->AddUpdateResource(cb.second.Get(), cbData.data.get(), cbData.size);
				//void* pData = nullptr;
				//CHECK_FAILED(cb.second->Map(0, nullptr, &pData));
				//if (!pData) continue;
				//std::memcpy(pData, cbData.data.get(), cbData.size);
				//cb.second->Unmap(0, nullptr);
				cbData.isUpdate = false;
			}
		}

		// コンスタントバッファ指定
		if (d3dMat->m_pCBufferHeap[stageIndex])
		{
			// ヒープ指定
			ID3D12DescriptorHeap* pHeap[] = { d3dMat->m_pCBufferHeap[stageIndex].Get() };
			m_pCmdList->SetDescriptorHeaps(_countof(pHeap), pHeap);
			// テーブル指定
			m_pCmdList->SetGraphicsRootDescriptorTable(rootIndex++,
				pHeap[0]->GetGPUDescriptorHandleForHeapStart());
		}

		// テクスチャ更新
		for (const auto& tex : d3dMat->m_textureData[stageIndex])
		{
			setTextureResource(rootIndex++, tex.second.id);
		}

		// サンプラ更新
		for (const auto& sam : d3dMat->m_samplerData[stageIndex])
		{
			setSamplerResource(rootIndex++, sam.second.state);
		}
	}
}

void D3D12CommandList::setRenderBuffer(const core::RenderBufferID& renderBufferID)
{
	// データの取得
	auto* renderBuffer = static_cast<D3D12RenderBuffer*>(m_pDevice->getRenderBuffer(renderBufferID));

	// 頂点バッファをセット
	UINT stride = static_cast<UINT>(renderBuffer->m_vertexData.size);
	UINT offset = 0;
	m_pCmdList->IASetVertexBuffers(0, 1, &renderBuffer->m_vertexBufferView);
	// インデックスバッファをセット
	if (renderBuffer->m_indexData.count > 0) {
		m_pCmdList->IASetIndexBuffer(&renderBuffer->m_indexBufferVew);
	}

	// プリミティブ指定
	m_pCmdList->IASetPrimitiveTopology(getD3D12PrimitiveTopology(renderBuffer->m_topology));

}


//----- セット命令 -----

void D3D12CommandList::setBackBuffer()
{
	// レンダーターゲットハンドルの取得
	auto handlRTV = m_pRenderer->m_pBackBufferHeap->GetCPUDescriptorHandleForHeapStart();
	UINT backBufferIndex = m_pRenderer->m_pSwapChain->GetCurrentBackBufferIndex();
	handlRTV.ptr += backBufferIndex * m_pRenderer->m_nBackBufferSize;
	auto handlDSV = m_pRenderer->m_pDepthStencilHeap->GetCPUDescriptorHandleForHeapStart();

	// レンダーターゲットのバリア指定
	D3D12_RESOURCE_BARRIER barrierDesc = {};
	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;					// バリア種別(遷移)
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;						// バリア分割用
	barrierDesc.Transition.pResource = m_pRenderer->m_pBackBuffer[backBufferIndex].Get();	// リソースポインタ
	barrierDesc.Transition.Subresource = 										// サブリソースの数
		D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;								// リソース内のすべてのサブリソースを同時に移行
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;			// 遷移前のリソース状態
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;		// 遷移後のリソース状態
	m_pCmdList->ResourceBarrier(1, &barrierDesc);

	// レンダーターゲットのセット
	m_pCmdList->OMSetRenderTargets(1, &handlRTV, FALSE, &handlDSV);

}

void D3D12CommandList::setGraphicsPipelineState(const ShaderID& shaderID, const BlendState& bs,
	const RasterizeState& rs, const DepthStencilState& ds)
{
	// シェーダーの取得
	auto* d3d12Shader = static_cast<D3D12Shader*>(m_pDevice->getShader(shaderID));
	if (d3d12Shader == nullptr) return;

	// パイプラインステートの取得・指定
	auto pipelineState = m_pDevice->createGraphicsPipelineState(*d3d12Shader, bs, rs, ds);
	m_pCmdList->SetPipelineState(pipelineState);

	// ルートシグネチャーのセット
	m_pCmdList->SetGraphicsRootSignature(d3d12Shader->m_pRootSignature.Get());
}

void D3D12CommandList::setRenderTarget(const RenderTargetID& rtID)
{
	RenderTargetID rtIDs[] = { rtID };
	setRenderTarget(1, rtIDs);
}

void D3D12CommandList::setRenderTarget(const std::uint32_t num, const RenderTargetID rtIDs[])
{
	// 安全処理
	if (num >= MAX_RENDER_TARGET || num <= 0) return;

	// レンダーターゲット取得
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvs;
	rtvs.resize(num);
	for (int i = 0; i < num; ++i)
	{
		auto* pRT = static_cast<D3D12RenderTarget*>(m_pDevice->getRenderTarget(rtIDs[i]));
		if (pRT)
		{
			auto handle = pRT->m_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
			rtvs[i] = handle;
		}
	}

	// 現在のデプスステンシル取得
	auto* pDS = static_cast<D3D12DepthStencil*>(m_pDevice->getDepthStencil(m_curDepthStencilID));
	if (pDS)
	{
		// レンダーターゲット指定
		auto handle = pDS->m_pHeapDSV->GetCPUDescriptorHandleForHeapStart();
		m_pCmdList->OMSetRenderTargets(num, rtvs.data(), FALSE, &handle);
	}
	else
	{
		// レンダーターゲット指定
		m_pCmdList->OMSetRenderTargets(num, rtvs.data(), FALSE, nullptr);
	}
}

void D3D12CommandList::setRenderTarget(const RenderTargetID& rtID, const DepthStencilID& dsID)
{
	RenderTargetID rtIDs[] = { rtID };
	;	setRenderTarget(1, rtIDs, dsID);
}

void D3D12CommandList::setRenderTarget(std::uint32_t num, const RenderTargetID rtIDs[], const DepthStencilID& dsID)
{
	// 安全処理
	if (num >= MAX_RENDER_TARGET || num <= 0) return;

	// レンダーターゲット取得
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvs;
	rtvs.resize(num);
	for (int i = 0; i < num; ++i)
	{
		auto* pRT = static_cast<D3D12RenderTarget*>(m_pDevice->getRenderTarget(rtIDs[i]));
		if (pRT)
		{
			auto handle = pRT->m_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
			rtvs[i] = handle;
		}
	}

	// 現在のデプスステンシル取得
	auto* pDS = static_cast<D3D12DepthStencil*>(m_pDevice->getDepthStencil(dsID));
	m_curDepthStencilID = dsID;
	if (pDS)
	{
		// レンダーターゲット指定
		auto handle = pDS->m_pHeapDSV->GetCPUDescriptorHandleForHeapStart();
		m_pCmdList->OMSetRenderTargets(num, rtvs.data(), FALSE, &handle);
	}
	else
	{
		// レンダーターゲット指定
		m_pCmdList->OMSetRenderTargets(num, rtvs.data(), FALSE, nullptr);
	}
}

void D3D12CommandList::setViewport(const Rect& rect)
{
	// ビューポートの指定
	D3D12_VIEWPORT d3d12View = {
		rect.left, rect.top, rect.right, rect.bottom, 0.0f, 1.0f
	};
	m_pCmdList->RSSetViewports(1, &d3d12View);

	// シザーの指定
	D3D12_RECT d3d12Rect = {};
	d3d12Rect.top = rect.top;//切り抜き上座標
	d3d12Rect.left = rect.left;//切り抜き左座標
	d3d12Rect.right = rect.right;//切り抜き右座標
	d3d12Rect.bottom = rect.bottom;//切り抜き下座標
	m_pCmdList->RSSetScissorRects(1, &d3d12Rect);
}

void D3D12CommandList::setViewport(const Viewport& viewport)
{
	// ビューポートの指定
	D3D12_VIEWPORT d3d12View = { viewport.left, viewport.top,
		viewport.right, viewport.bottom, viewport.minDepth, viewport.maxDepth
	};
	m_pCmdList->RSSetViewports(1, &d3d12View);

	// シザーの指定
	D3D12_RECT d3d12Rect = {};
	d3d12Rect.top = viewport.top;//切り抜き上座標
	d3d12Rect.left = viewport.left;//切り抜き左座標
	d3d12Rect.right = viewport.right;//切り抜き右座標
	d3d12Rect.bottom = viewport.bottom;//切り抜き下座標
	m_pCmdList->RSSetScissorRects(1, &d3d12Rect);
}

//----- ゲット命令 -----

//----- バインド命令 -----

void D3D12CommandList::bindGlobalBuffer(const core::ShaderID& shaderID, const std::string& bindName, const core::BufferID& bufferID)
{
	auto* pShader = static_cast<D3D12Shader*>(m_pDevice->getShader(shaderID));
	auto* pBuffer = static_cast<D3D12Buffer*>(m_pDevice->getBuffer(bufferID));
	auto type = static_cast<std::size_t>(pBuffer->m_type);

	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(pShader->m_desc.m_stages, stage)) continue;
		auto stageIndex = static_cast<std::size_t>(stage);

		auto itr = pShader->m_staticBindData[stageIndex][type].find(bindName.data());
		if (pShader->m_staticBindData[stageIndex][type].end() != itr)
		{
			// GPU更新
			if (pBuffer->m_isUpdate)
			{
				m_pDevice->AddUpdateResource(pBuffer->m_pBuffer.Get(), 
					pBuffer->m_aData.data(), pBuffer->m_aData.size());
				//void* pData = nullptr;
				//pBuffer->m_pBuffer->Map(0, nullptr, &pData);
				//std::memcpy(pData, pBuffer->m_aData.data(), pBuffer->m_aData.size());
				//pBuffer->m_pBuffer->Unmap(0, nullptr);
				pBuffer->m_isUpdate = false;
			}

			// ヒープ指定
			m_pCmdList->SetDescriptorHeaps(1, pBuffer->m_pHeap.GetAddressOf());
			// ビュー指定
			if (stage != ShaderStage::CS)
			{
				if (pBuffer->m_type == CoreBuffer::BufferType::CBV)
				{
					m_pCmdList->SetGraphicsRootConstantBufferView(
						itr->second.rootIndex,
						pBuffer->m_pBuffer->GetGPUVirtualAddress());
				}
				else if (pBuffer->m_type == CoreBuffer::BufferType::SRV)
				{
					m_pCmdList->SetGraphicsRootShaderResourceView(
						itr->second.rootIndex,
						pBuffer->m_pBuffer->GetGPUVirtualAddress());
				}
				else if (pBuffer->m_type == CoreBuffer::BufferType::UAV)
				{
					m_pCmdList->SetGraphicsRootUnorderedAccessView(
						itr->second.rootIndex,
						pBuffer->m_pBuffer->GetGPUVirtualAddress());
				}
			}
			else
			{
				if (pBuffer->m_type == CoreBuffer::BufferType::CBV)
				{
					m_pCmdList->SetComputeRootConstantBufferView(
						itr->second.rootIndex,
						pBuffer->m_pBuffer->GetGPUVirtualAddress());
				}
				else if (pBuffer->m_type == CoreBuffer::BufferType::SRV)
				{
					m_pCmdList->SetComputeRootShaderResourceView(
						itr->second.rootIndex,
						pBuffer->m_pBuffer->GetGPUVirtualAddress());
				}
				else if (pBuffer->m_type == CoreBuffer::BufferType::UAV)
				{
					m_pCmdList->SetComputeRootUnorderedAccessView(
						itr->second.rootIndex,
						pBuffer->m_pBuffer->GetGPUVirtualAddress());
				}
			}
			break;
		}
	}
}

void D3D12CommandList::bindGlobalTexture(const core::ShaderID& shaderID, const std::string& bindName, const core::TextureID& textureID)
{
	constexpr auto type = static_cast<std::size_t>(BindType::TEXTURE);
	auto* pShader = static_cast<D3D12Shader*>(m_pDevice->getShader(shaderID));
	auto* pTexture = static_cast<D3D12Texture*>(m_pDevice->getTexture(textureID));

	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(pShader->m_desc.m_stages, stage)) continue;
		auto stageIndex = static_cast<std::size_t>(stage);

		auto itr = pShader->m_staticBindData[stageIndex][type].find(bindName.data());
		if (pShader->m_staticBindData[stageIndex][type].end() != itr)
		{
			// ヒープ指定
			m_pCmdList->SetDescriptorHeaps(1, pTexture->m_pTexHeap.GetAddressOf());
			// Table
			m_pCmdList->SetGraphicsRootDescriptorTable(
				itr->second.rootIndex,
				pTexture->m_pTexHeap->GetGPUDescriptorHandleForHeapStart());
			break;
		}
	}
}

void D3D12CommandList::bindGlobalSampler(const core::ShaderID& shaderID, const std::string& bindName, const core::SamplerState& sampler)
{
	constexpr auto type = static_cast<std::size_t>(BindType::SAMPLER);
	auto* pShader = static_cast<D3D12Shader*>(m_pDevice->getShader(shaderID));
	const auto& samplerHandle = m_pDevice->m_dynamicSamplers[static_cast<size_t>(sampler)];

	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(pShader->m_desc.m_stages, stage)) continue;
		auto stageIndex = static_cast<std::size_t>(stage);

		auto itr = pShader->m_staticBindData[stageIndex][type].find(bindName.data());
		if (pShader->m_staticBindData[stageIndex][type].end() != itr)
		{
			// ヒープ指定
			m_pCmdList->SetDescriptorHeaps(1, m_pDevice->m_pSamplerHeap.GetAddressOf());
			// Table
			m_pCmdList->SetGraphicsRootDescriptorTable(
				itr->second.rootIndex,
				samplerHandle);
			break;
		}
	}
}

//----- 描画命令

void D3D12CommandList::render(const core::RenderBufferID& renderBufferID, std::uint32_t instanceCount)
{
	// データの取得
	auto* renderBuffer = static_cast<D3D12RenderBuffer*>(m_pDevice->getRenderBuffer(renderBufferID));

	// ポリゴンの描画
	if (renderBuffer->m_indexData.count > 0)
	{
		m_pCmdList->DrawIndexedInstanced(renderBuffer->m_indexData.count, instanceCount, 0, 0, 0);
	}
	else
	{
		m_pCmdList->DrawInstanced(renderBuffer->m_vertexData.count, instanceCount, 0, 0);
	}
}

/// @brief 
/// @param destID 対象のレンダーターゲット
/// @param sourceID 
/// @param matID 
void D3D12CommandList::blit(const RenderBufferID& destID, const TextureID& sourceID, const MaterialID& matID)
{

}

//----- その他 -----

void D3D12CommandList::clearCommand()
{
	// コマンドのクリア
}

void D3D12CommandList::clearBackBuffer(const Color& color)
{
	// レンダーターゲットハンドルの取得
	auto handlRTV = m_pRenderer->m_pBackBufferHeap->GetCPUDescriptorHandleForHeapStart();
	UINT backBufferIndex = m_pRenderer->m_pSwapChain->GetCurrentBackBufferIndex();
	handlRTV.ptr += backBufferIndex * m_pRenderer->m_nBackBufferSize;
	auto handlDSV = m_pRenderer->m_pDepthStencilHeap->GetCPUDescriptorHandleForHeapStart();

	FLOAT clearColor[4] = {};
	std::memcpy(clearColor, &color, sizeof(Color));

	m_pCmdList->ClearRenderTargetView(handlRTV, clearColor, 0, nullptr);
	// デプスステンシルのクリア
	m_pCmdList->ClearDepthStencilView(handlDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void D3D12CommandList::clearRederTarget(const RenderTargetID& rtID, const Color& color)
{
	// レンダーターゲット取得
	auto* pRT = static_cast<D3D12RenderTarget*>(m_pDevice->getRenderTarget(rtID));
	if (pRT == nullptr) return;

	FLOAT ColorRGBA[4];
	std::memcpy(ColorRGBA, &color, sizeof(Color));

	// クリアコマンド
	m_pCmdList->ClearRenderTargetView(pRT->m_pHeapRTV->GetCPUDescriptorHandleForHeapStart(), 
		ColorRGBA, 0, nullptr);
}

void D3D12CommandList::clearDepthStencil(const DepthStencilID& dsID, float depth, std::uint8_t stencil)
{
	// 現在のデプスステンシル取得
	auto* pDS = static_cast<D3D12DepthStencil*>(m_pDevice->getDepthStencil(dsID));
	if (pDS == nullptr) return;

	// クリアコマンド
	m_pCmdList->ClearDepthStencilView(pDS->m_pHeapDSV->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
}

//------------------------------------------------------------------------------
// private methods 
//------------------------------------------------------------------------------

void D3D12CommandList::setCBufferResource(std::uint32_t rootIndex, const core::BufferID& bufferID)
{
}

void D3D12CommandList::setTextureResource(std::uint32_t rootIndex, const core::TextureID& textureID)
{
	D3D12Texture* pD3DTex = static_cast<D3D12Texture*>(m_pDevice->getTexture(textureID));

	if (pD3DTex)
	{
		// ヒープ指定
		ID3D12DescriptorHeap* pHeap[] = { pD3DTex->m_pTexHeap.Get() };
		m_pCmdList->SetDescriptorHeaps(_countof(pHeap), pHeap);
		// テーブル指定
		m_pCmdList->SetGraphicsRootDescriptorTable(rootIndex,
			pD3DTex->m_pTexHeap->GetGPUDescriptorHandleForHeapStart());
	}
	else
	{
		// 空のテクスチャ

	}
}

void D3D12CommandList::setSamplerResource(std::uint32_t rootIndex, core::SamplerState state)
{
	// ヒープ指定
	ID3D12DescriptorHeap* pHeap[] = { m_pDevice->m_pSamplerHeap.Get() };
	m_pCmdList->SetDescriptorHeaps(_countof(pHeap), pHeap);
	// テーブル指定
	m_pCmdList->SetGraphicsRootDescriptorTable(rootIndex,
		m_pDevice->m_dynamicSamplers[static_cast<std::size_t>(state)]);
}

