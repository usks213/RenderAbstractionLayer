/*****************************************************************//**
 * \file   D3D12_RenderContext.h
 * \brief  DiectX12レンダーコンテキスト
 *
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/

#include "D3D12_RenderContext.h"
#include "D3D12_Renderer.h"
#include "D3D12_RenderDevice.h"

#include "D3D12_Buffer.h"
#include "D3D12_RenderBuffer.h"
#include "D3D12_Texture.h"
//#include "D3D12_RenderTarget.h"
//#include "D3D12_DepthStencil.h"

#include <Renderer/Core/Core_SubResource.h>

#include <functional>

using namespace core;
using namespace d3d12;


//------------------------------------------------------------------------------
// public methods 
//------------------------------------------------------------------------------

/// @brief コンストラクタ
D3D12RenderContext::D3D12RenderContext() :
	m_pRenderer(nullptr),
	m_pDevice(nullptr)
{
}

/// @brief 初期化処理
/// @param pRenderer D3D12レンダラーポインタ
/// @param pDevice D3D12デバイスポインタ
/// @return 初期化: 成功 true | 失敗 false
HRESULT D3D12RenderContext::initialize(D3D12Renderer* pRenderer, D3D12RenderDevice* pDevice)
{
	//--- 初期化
	m_pRenderer = pRenderer;
	m_pDevice = pDevice;

	return S_OK;
}

//----- リソース指定命令 -----

void D3D12RenderContext::setMaterial(const core::MaterialID& materialID)
{
	// マテリアルの取得
	auto* d3dMat = static_cast<D3D12Material*>(m_pDevice->getMaterial(materialID));
	if (d3dMat == nullptr) return;

	// シェーダーの取得
	auto* d3dShader = static_cast<D3D12Shader*>(m_pDevice->getShader(d3dMat->m_shaderID));
	if (d3dShader == nullptr) return;

	// パイプラインステートの指定
	setPipelineState(*d3dShader, *d3dMat);

	// ステージごと
	UINT rootIndex = 0;
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(d3dShader->m_desc.m_stages, stage)) continue;

		auto stageIndex = static_cast<std::size_t>(stage);

		// コンスタントバッファ更新
		for (auto& cb : d3dMat->m_d3dCbuffer[stageIndex])
		{
			auto& cbData = d3dMat->m_cbufferData[stageIndex][cb.first];
			if (cbData.isUpdate)
			{
				void* pData = nullptr;
				CHECK_FAILED(cb.second->Map(0, nullptr, &pData));
				if (!pData) continue;
				std::memcpy(pData, cbData.data.get(), cbData.size);
				cb.second->Unmap(0, nullptr);
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
			setSamplerResource(sam.first, sam.second.state);
		}
	}
}

void D3D12RenderContext::setRenderBuffer(const core::RenderBufferID& renderBufferID)
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

//----- バインド命令 -----

void D3D12RenderContext::setBuffer(std::string_view bindName, const core::ShaderID& shaderID, const core::BufferID bufferID)
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

void D3D12RenderContext::setTexture(std::string_view bindName, const core::ShaderID& shaderID, const core::TextureID textureID)
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

void D3D12RenderContext::setSampler(std::string_view bindName, const core::ShaderID& shaderID, const core::SamplerState sampler)
{
	//constexpr auto type = static_cast<std::size_t>(SHADER::BindType::SAMPLER);
	//auto* pShader = static_cast<D3D12Shader*>(m_pDevice->getShader(shaderID));
	//auto* pTexture = static_cast<D3D12Texture*>(m_pDevice->getTexture(textureID));

	//for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	//{
	//	if (!hasStaderStage(pShader->m_desc.m_stages, stage)) continue;
	//	auto stageIndex = static_cast<std::size_t>(stage);

	//	auto itr = pShader->m_staticBindData[stageIndex][type].find(bindName.data());
	//	if (pShader->m_staticBindData[stageIndex][type].end() != itr)
	//	{
	//		// ヒープ指定
	//		m_pCmdList->SetDescriptorHeaps(1, pTexture->m_pTexHeap.GetAddressOf());
	//		// Table
	//		m_pCmdList->SetGraphicsRootDescriptorTable(
	//			itr->second.rootIndex,
	//			pTexture->m_pTexHeap->GetGPUDescriptorHandleForHeapStart());
	//		break;
	//	}
	//}
}

//----- 描画命令

void D3D12RenderContext::render(const core::RenderBufferID& renderBufferID)
{
	// データの取得
	auto* renderBuffer = static_cast<D3D12RenderBuffer*>(m_pDevice->getRenderBuffer(renderBufferID));

	// ポリゴンの描画
	if (renderBuffer->m_indexData.count > 0)
	{
		m_pCmdList->DrawIndexedInstanced(renderBuffer->m_indexData.count, 1, 0, 0, 0);
	}
	else
	{
		m_pCmdList->DrawInstanced(renderBuffer->m_vertexData.count, 1, 0, 0);
	}
}


//------------------------------------------------------------------------------
// private methods 
//------------------------------------------------------------------------------

void D3D12RenderContext::setPipelineState(D3D12Shader& d3d12Shader, D3D12Material& d3d12Mat)
{
	// グラフィクスパイプラインを検索
	auto pipelineID = d3d12Shader.m_id;
	auto itr = m_pPipelineState.find(pipelineID);

	// パイプラインステートを新規作成
	if (m_pPipelineState.end() == itr)
	{
		// グラフィクスパイプラインの作成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
		// シグネチャー
		gpipeline.pRootSignature = d3d12Shader.m_pRootSignature.Get();
		// 各シェーダー
		gpipeline.VS.pShaderBytecode = d3d12Shader.m_pShaderBlob[0]->GetBufferPointer();
		gpipeline.VS.BytecodeLength = d3d12Shader.m_pShaderBlob[0]->GetBufferSize();
		gpipeline.PS.pShaderBytecode = d3d12Shader.m_pShaderBlob[4]->GetBufferPointer();
		gpipeline.PS.BytecodeLength = d3d12Shader.m_pShaderBlob[4]->GetBufferSize();
		// ストリームアウトプット
		gpipeline.StreamOutput;
		// ブレンドステイト
		gpipeline.BlendState = m_pDevice->m_blendStates[static_cast<std::size_t>(d3d12Mat.m_blendState)];
		// サンプルマスク
		gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		// ラスタライザーステート
		gpipeline.RasterizerState = m_pDevice->m_rasterizeStates[static_cast<std::size_t>(d3d12Mat.m_rasterizeState)];
		// デプスステンシルステート
		gpipeline.DepthStencilState = m_pDevice->m_depthStencilStates[static_cast<std::size_t>(d3d12Mat.m_depthStencilState)];
		// インプットレイアウト
		gpipeline.InputLayout.pInputElementDescs = d3d12Shader.m_inputElementDesc.data();
		gpipeline.InputLayout.NumElements = d3d12Shader.m_inputElementDesc.size();
		// ストリップカット
		gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		// プリミティブトポロジータイプ
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// レンダーターゲット数
		gpipeline.NumRenderTargets = 1;
		// 各レンダーターゲットフォーマット
		gpipeline.RTVFormats[0] = m_pDevice->m_backBufferFormat;
		// デプスステンシルフォーマット
		gpipeline.DSVFormat = m_pDevice->m_depthStencilFormat;
		// サンプルDesc
		gpipeline.SampleDesc.Count = m_pDevice->m_sampleDesc.count;
		gpipeline.SampleDesc.Quality = m_pDevice->m_sampleDesc.quality;
		// ノードマスク
		gpipeline.NodeMask;
		// キャッシュPSO
		gpipeline.CachedPSO;
		// パイプラインフラグ
		gpipeline.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		//gpipeline.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;

		// パイプラインステート作成
		ID3D12PipelineState* pPipelinestate = nullptr;
		CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pPipelinestate)));
		if (pPipelinestate)
			m_pPipelineState.emplace(pipelineID, pPipelinestate);
	}

	// パイプラインステートのセット
	m_pCmdList->SetPipelineState(m_pPipelineState[pipelineID].Get());
	// ルートシグネチャーのセット
	m_pCmdList->SetGraphicsRootSignature(d3d12Shader.m_pRootSignature.Get());

}

void D3D12RenderContext::setCBufferResource(std::uint32_t rootIndex, const core::BufferID& bufferID)
{

}

void D3D12RenderContext::setTextureResource(std::uint32_t rootIndex, const core::TextureID& textureID)
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
		// デフォルトテクスチャ指定

	}
}

void D3D12RenderContext::setSamplerResource(std::uint32_t slot, core::SamplerState state)
{

}

