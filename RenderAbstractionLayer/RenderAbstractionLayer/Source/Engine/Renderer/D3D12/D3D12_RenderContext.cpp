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

//#include "D3D12_Buffer.h"
#include "D3D12_RenderBuffer.h"
//#include "D3D12_Texture.h"
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
	//m_pD3DContext(nullptr),
	//m_pD3DDeffered(nullptr),

	//m_systemBuffer(nullptr),
	//m_transformBuffer(nullptr),
	//m_animationBuffer(nullptr),
	//m_pointLightBuffer(nullptr),
	//m_spotLightBuffer(nullptr),
	//m_pointLightSRV(nullptr),
	//m_spotLightSRV(nullptr)
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

	//--- バッファの生成

	//// 共通CBufferの初期化
	//D3D12_BUFFER_DESC d3dDesc = {};
	//d3dDesc.BindFlags = D3D12_BIND_CONSTANT_BUFFER;
	//d3dDesc.Usage = D3D12_USAGE_DEFAULT;
	//d3dDesc.CPUAccessFlags = 0;
	//d3dDesc.MiscFlags = 0;

	//// System
	//d3dDesc.ByteWidth = sizeof(SHADER::SystemBuffer);
	//CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateBuffer(&d3dDesc, nullptr, m_systemBuffer.ReleaseAndGetAddressOf()));
	//// Transform
	//d3dDesc.ByteWidth = sizeof(SHADER::TransformBuffer);
	//CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateBuffer(&d3dDesc, nullptr, m_transformBuffer.ReleaseAndGetAddressOf()));

	//// Map/Unmap Param
	//d3dDesc.Usage = D3D12_USAGE_DYNAMIC;
	//d3dDesc.CPUAccessFlags = D3D12_CPU_ACCESS_WRITE;
	//// Animation
	//d3dDesc.ByteWidth = sizeof(SHADER::AnimationBuffer);
	//CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateBuffer(&d3dDesc, nullptr, m_animationBuffer.ReleaseAndGetAddressOf()));

	//// Light
	//d3dDesc.BindFlags = D3D12_BIND_SHADER_RESOURCE;
	//d3dDesc.MiscFlags = D3D12_RESOURCE_MISC_BUFFER_STRUCTURED;
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	//srvDesc.Buffer.FirstElement = 0;

	//// ポイントライト
	//d3dDesc.ByteWidth = sizeof(CorePointLight) * SHADER::MAX_POINT_LIGHT_COUNT;
	//d3dDesc.StructureByteStride = sizeof(CorePointLight);
	//CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateBuffer(&d3dDesc, nullptr, m_pointLightBuffer.ReleaseAndGetAddressOf()));
	//srvDesc.Buffer.NumElements = SHADER::MAX_POINT_LIGHT_COUNT;
	//CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateShaderResourceView(m_pointLightBuffer.Get(), nullptr, m_pointLightSRV.ReleaseAndGetAddressOf()));
	//// スポットライト
	//d3dDesc.ByteWidth = sizeof(CoreSpotLight) * SHADER::MAX_SPOT_LIGHT_COUNT;
	//d3dDesc.StructureByteStride = sizeof(CoreSpotLight);
	//CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateBuffer(&d3dDesc, nullptr, m_spotLightBuffer.ReleaseAndGetAddressOf()));
	//srvDesc.Buffer.NumElements = SHADER::MAX_SPOT_LIGHT_COUNT;
	//CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateShaderResourceView(m_spotLightBuffer.Get(), nullptr, m_spotLightSRV.ReleaseAndGetAddressOf()));

	// Sampler
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		setSampler(static_cast<std::uint32_t>(core::SHADER::SS_SLOT::Main), SamplerState::LINEAR_WRAP, stage);
		setSampler(static_cast<std::uint32_t>(core::SHADER::SS_SLOT::Shadow), SamplerState::SHADOW, stage);
		setSampler(static_cast<std::uint32_t>(core::SHADER::SS_SLOT::Sky), SamplerState::ANISOTROPIC_WRAP, stage);
	}

	return S_OK;
}

//----- リソース指定命令 -----

void D3D12RenderContext::setPipelineState(const core::MaterialID& materialID, const core::RenderBufferID& renderBufferID)
{
	// パイプラインステート

	// マテリアルの取得
	auto* d3dMat = static_cast<D3D12Material*>(m_pDevice->getMaterial(materialID));
	if (d3dMat == nullptr) return;

	// シェーダーの取得
	auto* d3dShader = static_cast<D3D12Shader*>(m_pDevice->getShader(d3dMat->m_shaderID));
	if (d3dShader == nullptr) return;

	// レンダーバッファの取得
	auto* d3dRenderBuffer = static_cast<D3D12RenderBuffer*>(m_pDevice->getRenderBuffer(renderBufferID));
	if (d3dRenderBuffer == nullptr) return;

	// グラフィクスパイプラインを検索
	auto itr = m_pPipelineState.find(std::make_tuple(materialID, renderBufferID));

	// パイプラインステートを新規作成
	if (m_pPipelineState.end() == itr)
	{
		// グラフィクスパイプラインの作成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
		// シグネチャー
		gpipeline.pRootSignature = d3dShader->m_pRootSignature.Get();
		// 各シェーダー
		gpipeline.VS.pShaderBytecode = d3dShader->m_pShaderBlob[0]->GetBufferPointer();
		gpipeline.VS.BytecodeLength = d3dShader->m_pShaderBlob[0]->GetBufferSize();
		gpipeline.PS.pShaderBytecode = d3dShader->m_pShaderBlob[4]->GetBufferPointer();
		gpipeline.PS.BytecodeLength = d3dShader->m_pShaderBlob[4]->GetBufferSize();
		// ストリームアウトプット
		gpipeline.StreamOutput;
		// ブレンドステイト
		gpipeline.BlendState = m_pDevice->m_blendStates[static_cast<std::size_t>(d3dMat->m_blendState)];
		// サンプルマスク
		gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		// ラスタライザーステート
		gpipeline.RasterizerState = m_pDevice->m_rasterizeStates[static_cast<std::size_t>(d3dMat->m_rasterizeState)];
		// デプスステンシルステート
		gpipeline.DepthStencilState = m_pDevice->m_depthStencilStates[static_cast<std::size_t>(d3dMat->m_depthStencilState)];
		// インプットレイアウト
		gpipeline.InputLayout.pInputElementDescs = d3dShader->m_inputElementDesc.data();
		gpipeline.InputLayout.NumElements = d3dShader->m_inputElementDesc.size();
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
			m_pPipelineState.emplace(std::make_tuple(materialID, renderBufferID), pPipelinestate);
	}

	// パイプラインステートのセット
	m_pCmdList->SetPipelineState(m_pPipelineState[std::make_tuple(materialID, renderBufferID)].Get());
	// ルートシグネチャーのセット
	m_pCmdList->SetGraphicsRootSignature(d3dShader->m_pRootSignature.Get());


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

	// レンダーターゲットのクリア
	static float a = 0;
	//a += 0.1f;
	FLOAT clearColor[] = { sinf(a) + 0.13f , 0.2f, 0.2f, 1.0f };
	m_pCmdList->ClearRenderTargetView(handlRTV, clearColor, 0, nullptr);
	// デプスステンシルのクリア
	m_pCmdList->ClearDepthStencilView(handlDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ビューポートのセット
	m_pCmdList->RSSetViewports(1, &m_pRenderer->m_viewport);

	// シザーのセット
	m_pCmdList->RSSetScissorRects(1, &m_pRenderer->m_scissorrect);



	// レンダーバッファのセット
	setRenderBuffer(renderBufferID);

	// マテリアルのセット
	setMaterial(materialID);

}

void D3D12RenderContext::setMaterial(const core::MaterialID& materialID)
{
	// マテリアルの取得
	auto* d3dMat = static_cast<D3D12Material*>(m_pDevice->getMaterial(materialID));
	if (d3dMat == nullptr) return;

	// シェーダーの取得
	auto* d3dShader = static_cast<D3D12Shader*>(m_pDevice->getShader(d3dMat->m_shaderID));
	if (d3dShader == nullptr) return;

	// マテリアルリソース指定・更新
	setMaterialResource(*d3dMat, *d3dShader);
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
	setPrimitiveTopology(renderBuffer->m_topology);
}

void D3D12RenderContext::setTexture(std::uint32_t slot, const core::TextureID& textureID, core::ShaderStage stage)
{
	auto stageIndex = static_cast<std::size_t>(stage);
	if (m_curTexture[stageIndex][slot] == textureID) return;


	//if (textureID == NONE_TEXTURE_ID)
	//{
	//	ID3D12ShaderResourceView* nullView = nullptr;
	//	setShaderResource[stageIndex](m_pD3DContext, slot, 1, &nullView);
	//	m_curTexture[stageIndex][slot] = NONE_TEXTURE_ID;
	//}
	//else
	//{
	//	D3D12Texture* pD3DTex = static_cast<D3D12Texture*>(m_pDevice->getTexture(textureID));
	//	ID3D12ShaderResourceView* pTex = pD3DTex ? pD3DTex->m_srv.Get() : nullptr;
	//	setShaderResource[stageIndex](m_pD3DContext, slot, 1, &pTex);
	//	m_curTexture[stageIndex][slot] = textureID;
	//}
}

void D3D12RenderContext::setSampler(std::uint32_t slot, core::SamplerState state, core::ShaderStage stage)
{
	auto stageIndex = static_cast<size_t>(stage);
	if (m_curSamplerState[stageIndex][slot] == state) {
		return;
	}

	//setSamplers[stageIndex](m_pD3DContext, slot, 1, m_pDevice->m_samplerStates[static_cast<size_t>(state)].GetAddressOf());
	//m_curSamplerState[stageIndex][slot] = state;
}

void D3D12RenderContext::setPrimitiveTopology(core::PrimitiveTopology topology)
{
	//if (m_curPrimitiveTopology == topology) return;

	m_pCmdList->IASetPrimitiveTopology(getD3D12PrimitiveTopology(topology));
	m_curPrimitiveTopology = topology;
}

//----- バッファ指定命令 -----

void D3D12RenderContext::sendSystemBuffer(const core::SHADER::SystemBuffer& systemBuffer)
{
	/*for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<std::size_t>(stage);
		setCBuffer[stageIndex](m_pD3DContext, static_cast<std::uint32_t>(SHADER::CB_SLOT::System)
			, 1, m_systemBuffer.GetAddressOf());
		m_pD3DContext->UpdateSubresource(m_systemBuffer.Get(), 0, nullptr, &systemBuffer, 0, 0);
	}*/
}

void D3D12RenderContext::sendTransformBuffer(const Matrix& mtxWorld)
{
	SHADER::TransformBuffer transform;
	transform._mWorld = mtxWorld.Transpose();

	//for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	//{
	//	auto stageIndex = static_cast<std::size_t>(stage);
	//	setCBuffer[stageIndex](m_pD3DContext, static_cast<std::uint32_t>(SHADER::CB_SLOT::Transform)
	//		, 1, m_transformBuffer.GetAddressOf());
	//	m_pD3DContext->UpdateSubresource(m_transformBuffer.Get(), 0, nullptr, &transform, 0, 0);
	//}
}

void D3D12RenderContext::sendAnimationBuffer(std::vector<Matrix>& mtxBones)
{
	//// マトリックスは倒置前提
	//SubResource resource;
	//std::size_t size = mtxBones.size() < SHADER::MAX_ANIMATION_BONE_COUNT ?
	//	mtxBones.size() * sizeof(Matrix) : SHADER::MAX_ANIMATION_BONE_COUNT * sizeof(Matrix);
	//// 更新
	//d3dMap(m_animationBuffer.Get(), D3D12_MAP::D3D12_MAP_WRITE_DISCARD, true, resource);
	//std::memcpy(resource.pData, mtxBones.data(), size);
	//d3dUnmap(m_animationBuffer.Get());
	//// 指定
	//setCBuffer[static_cast<std::size_t>(ShaderStage::VS)](m_pD3DContext.Get(),
	//	SHADER::SHADER_CB_SLOT_ANIMATION, 1, m_animationBuffer.GetAddressOf());
}

void D3D12RenderContext::sendLightBuffer(std::vector<core::CorePointLight>& pointLights,
	std::vector<core::CoreSpotLight>& spotLights)
{
	//SubResource resource;
	//// ポイントライト
	//std::size_t size = pointLights.size() < SHADER::MAX_POINT_LIGHT_COUNT ?
	//	pointLights.size() * sizeof(CorePointLight) :
	//	SHADER::MAX_POINT_LIGHT_COUNT * sizeof(CorePointLight);
	//d3dMap(m_pointLightBuffer.Get(), D3D12_MAP::D3D12_MAP_WRITE_DISCARD, true, resource);
	//std::memcpy(resource.pData, pointLights.data(), size);
	//d3dUnmap(m_pointLightBuffer.Get());
	//setShaderResource[static_cast<std::size_t>(ShaderStage::PS)](m_pD3DContext, SHADER::SHADER_SRV_SLOT_POINTLIGHT, 1, m_pointLightSRV.GetAddressOf());
	//// スポットライト
	//size = spotLights.size() < SHADER::MAX_SPOT_LIGHT_COUNT ?
	//	spotLights.size() * sizeof(CoreSpotLight) :
	//	SHADER::MAX_SPOT_LIGHT_COUNT * sizeof(CoreSpotLight);
	//d3dMap(m_spotLightBuffer.Get(), D3D12_MAP::D3D12_MAP_WRITE_DISCARD, true, resource);
	//std::memcpy(resource.pData, spotLights.data(), spotLights.size() * sizeof(CoreSpotLight));
	//d3dUnmap(m_spotLightBuffer.Get());
	//setShaderResource[static_cast<std::size_t>(ShaderStage::PS)](m_pD3DContext, SHADER::SHADER_SRV_SLOT_SPOTLIGHT, 1, m_spotLightSRV.GetAddressOf());
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

void D3D12RenderContext::setMaterialResource(const D3D12Material& d3dMaterial, const D3D12Shader& d3dShader)
{
	auto& d3dMat = const_cast<D3D12Material&>(d3dMaterial);
	UINT rootIndex = 0;

	// ステージごと
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(d3dShader.m_desc.m_stages, stage)) continue;

		auto stageIndex = static_cast<std::size_t>(stage);

		// コンスタントバッファ更新
		for (auto& cb : d3dMat.m_d3dCbuffer[stageIndex])
		{
			auto& cbData = d3dMat.m_cbufferData[stageIndex][cb.first];
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
		if(d3dMaterial.m_pCBufferHeap[stageIndex])
		{
			// ヒープ指定
			ID3D12DescriptorHeap* pHeap[] = { d3dMaterial.m_pCBufferHeap[stageIndex].Get() };
			m_pCmdList->SetDescriptorHeaps(1, pHeap);
			// テーブル指定
			//m_pCmdList->SetGraphicsRootConstantBufferView(rootIndex, d3dMat.m_d3dCbuffer[stageIndex][0]->GetGPUVirtualAddress());
			m_pCmdList->SetGraphicsRootDescriptorTable(rootIndex, 
				d3dMaterial.m_pCBufferHeap[stageIndex]->GetGPUDescriptorHandleForHeapStart());
		}

		// テクスチャ更新
		for (const auto& tex : d3dMat.m_textureData[stageIndex])
		{
			setTexture(tex.first, tex.second.id, stage);
		}

		// サンプラ更新
		for (const auto& sam : d3dMat.m_samplerData[stageIndex])
		{
			setSampler(sam.first, sam.second.state, stage);
		}

		++rootIndex;
	}
}
