/*****************************************************************//**
 * \file   D3D12_RenderDevice.h
 * \brief  DirectX12レンダーデバイス
 *
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/

#include "D3D12_RenderDevice.h"
#include <Engine/Utility/Util_Hash.h>

//#include <Renderer/D3D12/D3D12_Buffer.h>
//#include <Renderer/D3D12/D3D12_DepthStencil.h>
#include <Renderer/D3D12/D3D12_Material.h>
#include <Renderer/D3D12/D3D12_RenderBuffer.h>
//#include <Renderer/D3D12/D3D12_RenderTarget.h>
#include <Renderer/D3D12/D3D12_Shader.h>
//#include <Renderer/D3D12/D3D12_Texture.h>

using namespace core;
using namespace d3d12;

 //------------------------------------------------------------------------------
 // public methods
 //------------------------------------------------------------------------------

/// @brief コンストラクタ
D3D12RenderDevice::D3D12RenderDevice() :
	m_pD3DDevice(nullptr),
	//m_backBufferRT(nullptr),
	//m_backBufferRTV(nullptr),
	m_backBufferFormat(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
	//m_depthStencilTexture(nullptr),
	//m_depthStencilView(nullptr),
	m_depthStencilFormat(DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT),

	m_hWnd(nullptr),
	m_viewport(),
	m_backBufferCount(2u),
	m_nOutputWidth(1u),
	m_nOutputHeight(1u)
{

}

/// @brief 初期化処理
/// @param hWnd ウィンドウハンドル
/// @param width ウィンドウの幅
/// @param height ウィンドウの高さ
/// @return 初期化: 成功 true | 失敗 false
HRESULT D3D12RenderDevice::initialize(ID3D12Device* pDevice, IDXGIFactory2* pFactory2,
	HWND hWnd, UINT width, UINT height)
{
	// 初期化
	m_pD3DDevice = pDevice;
	m_hWnd = hWnd;
	m_nOutputWidth = width;
	m_nOutputHeight = height;

	// 共通ステートの初期化
	CHECK_FAILED(createCommonState());

	return S_OK;
}

//----- リソース生成 -----

core::BufferID D3D12RenderDevice::createBuffer(core::BufferDesc& desc, core::BufferData* pData)
{
	// IDの取得
	BufferID id = static_cast<BufferID>(hash::stringHash(desc.name));

	// 既に生成済み
	const auto& itr = m_bufferPool.find(id);
	if (m_bufferPool.end() != itr) return id;

	// 新規生成
	//m_bufferPool[id] = std::make_unique<D3D12Buffer>(m_pD3DDevice, id, desc, pData);

	return id;
}
core::DepthStencilID D3D12RenderDevice::createDepthStencil(std::string name)
{
	// IDの取得
	DepthStencilID id = static_cast<DepthStencilID>(hash::stringHash(name));

	// 既に生成済み
	const auto& itr = m_depthStencilPool.find(id);
	if (m_depthStencilPool.end() != itr) return id;

	// テクスチャ生成
	TextureDesc texDesc;
	texDesc.bindFlags = BindFlags::DEPTH_STENCIL | BindFlags::SHADER_RESOURCE;

	// デプスステンシル生成

	return id;
}
core::MaterialID D3D12RenderDevice::createMaterial(std::string name, core::ShaderID& shaderID)
{
	// IDの取得
	MaterialID id = static_cast<MaterialID>(hash::stringHash(name));

	// 既に生成済み
	const auto& itr = m_materialPool.find(id);
	if (m_materialPool.end() != itr) return id;

	// シェーダー取得
	auto* shader = getShader(shaderID);
	if (shader == nullptr) return NONE_MATERIAL_ID;

	// 新規生成
	m_materialPool[id] = std::make_unique<D3D12Material>(
		m_pD3DDevice, id, name, *shader);

	return id;
}
core::MeshID D3D12RenderDevice::createMesh(std::string name)
{
	// IDの取得
	MeshID id = static_cast<MeshID>(hash::stringHash(name));

	// 既に生成済み
	const auto& itr = m_meshPool.find(id);
	if (m_meshPool.end() != itr) return id;

	// 新規生成
	m_meshPool[id] = std::make_unique<CoreMesh>(id, name);

	return id;
}
core::RenderBufferID D3D12RenderDevice::createRenderBuffer(core::ShaderID& shaderID, core::MeshID& meshID)
{
	// シェーダー取得
	auto* shader = getShader(shaderID);
	if (shader == nullptr) return NONE_RENDERBUFFER_ID;
	// メッシュ取得
	auto* mesh = getMesh(meshID);
	if (mesh == nullptr) return NONE_RENDERBUFFER_ID;
	
	// IDの生成
	RenderBufferID id = static_cast<RenderBufferID>(
		hash::stringHash(shader->m_desc.m_name + ":" + mesh->m_name));

	// 既にある
	const auto& itr = m_renderBufferPool.find(id);
	if (m_renderBufferPool.end() != itr) return id;

	// 新規生成
	m_renderBufferPool[id] = std::make_unique<D3D12RenderBuffer>(
		m_pD3DDevice, id, *shader, *mesh);

	return id;
}
core::RenderTargetID D3D12RenderDevice::createRenderTarget(std::string name)
{
	// IDの取得
	RenderTargetID id = static_cast<RenderTargetID>(hash::stringHash(name));

	// 既に生成済み
	const auto& itr = m_renderTargetPool.find(id);
	if (m_renderTargetPool.end() != itr) return id;

	// テクスチャ生成
	TextureDesc texDesc;
	texDesc.bindFlags = BindFlags::RENDER_TARGET | BindFlags::SHADER_RESOURCE;

	// レンダーターゲット生成

	return id;
}
core::ShaderID D3D12RenderDevice::createShader(core::ShaderDesc& desc)
{
	// IDの取得
	ShaderID id = static_cast<ShaderID>(hash::stringHash(desc.m_name));

	// 既に生成済み
	const auto& itr = m_shaderPool.find(id);
	if (m_shaderPool.end() != itr) return id;

	// 新規生成
	m_shaderPool[id] = std::make_unique<D3D12Shader>(m_pD3DDevice, desc, id);

	return id;
}
core::TextureID D3D12RenderDevice::createTexture(std::string filePath)
{
	// IDの取得
	TextureID id = static_cast<TextureID>(hash::stringHash(filePath));

	// 既に生成済み
	const auto& itr = m_texturePool.find(id);
	if (m_texturePool.end() != itr) return id;

	// 新規生成
	//m_texturePool[id] = std::make_unique<D3D12Texture>(m_pD3DDevice, id, filePath);

	return id;
}
core::TextureID D3D12RenderDevice::createTexture(core::TextureDesc& desc, core::TextureData* pData)
{
	// IDの取得
	TextureID id = static_cast<TextureID>(hash::stringHash(desc.name));

	// 既に生成済み
	const auto& itr = m_texturePool.find(id);
	if (m_texturePool.end() != itr) return id;

	// 新規生成
	//m_texturePool[id] = std::make_unique<D3D12Texture>(m_pD3DDevice, id, desc, pData);

	return id;
}


 //------------------------------------------------------------------------------
 // private methods
 //------------------------------------------------------------------------------


/// @brief 共通ステートの生成
/// @return HRESULT
HRESULT D3D12RenderDevice::createCommonState()
{
	// ラスタライザステート作成
	{
		D3D12_RASTERIZER_DESC rasterizerDesc = {};

		// カリングなし 塗りつぶし
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.CullMode =D3D12_CULL_MODE_NONE;
		m_rasterizeStates[(size_t)RasterizeState::CULL_NONE] = rasterizerDesc;

		// 表面カリング 塗りつぶし
		rasterizerDesc.CullMode =D3D12_CULL_MODE_FRONT;
		m_rasterizeStates[(size_t)RasterizeState::CULL_FRONT] = rasterizerDesc;

		// 裏面カリング 塗りつぶし
		rasterizerDesc.CullMode =D3D12_CULL_MODE_BACK;
		m_rasterizeStates[(size_t)RasterizeState::CULL_BACK] = rasterizerDesc;

		// カリングなし ワイヤーフレーム
		rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
		rasterizerDesc.CullMode =D3D12_CULL_MODE_NONE;
		m_rasterizeStates[(size_t)RasterizeState::CULL_NONE_WIREFRAME] = rasterizerDesc;

		// 表面カリング ワイヤーフレーム
		rasterizerDesc.CullMode =D3D12_CULL_MODE_FRONT;
		m_rasterizeStates[(size_t)RasterizeState::CULL_FRONT_WIREFRAME] = rasterizerDesc;

		// 裏面カリング ワイヤーフレーム
		rasterizerDesc.CullMode =D3D12_CULL_MODE_BACK;
		m_rasterizeStates[(size_t)RasterizeState::CULL_BACK_WIREFRAME] = rasterizerDesc;

		// シャドウ用
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.CullMode =D3D12_CULL_MODE_BACK;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.DepthBias = 100000;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.SlopeScaledDepthBias = 2.0f;
		m_rasterizeStates[(size_t)RasterizeState::SHADOW] = rasterizerDesc;
	}

	// サンプラステート作成
	{
		D3D12_STATIC_SAMPLER_DESC samplerDesc = {};

		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.MaxAnisotropy = D3D12_MAX_MAXANISOTROPY;

		// リニアクランプ
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		m_samplerStates[static_cast<size_t>(SamplerState::LINEAR_CLAMP)] = samplerDesc;

		// ポイントクランプ
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		m_samplerStates[static_cast<size_t>(SamplerState::POINT_CLAMP)] = samplerDesc;

		// 異方性クランプ
		samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
		m_samplerStates[static_cast<size_t>(SamplerState::ANISOTROPIC_CLAMP)] = samplerDesc;

		// リニアラップ
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		m_samplerStates[static_cast<size_t>(SamplerState::LINEAR_WRAP)] = samplerDesc;

		// ポイントラップ
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		m_samplerStates[static_cast<size_t>(SamplerState::POINT_WRAP)] = samplerDesc;

		// 異方性ラップ
		samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
		m_samplerStates[static_cast<size_t>(SamplerState::ANISOTROPIC_WRAP)] = samplerDesc;

		// シャドウ
		samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		m_samplerStates[static_cast<size_t>(SamplerState::SHADOW)] = samplerDesc;
	}

	// ブレンドステート作成
	{
		D3D12_BLEND_DESC blendDesc = {};

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		// 半透明合成
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		m_blendStates[static_cast<size_t>(BlendState::ALPHA)] = blendDesc;

		// 加算合成
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		m_blendStates[static_cast<size_t>(BlendState::ADD)] = blendDesc;

		// 減算合成
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		m_blendStates[static_cast<size_t>(BlendState::SUB)] = blendDesc;

		// 乗算合成
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		m_blendStates[static_cast<size_t>(BlendState::MUL)] = blendDesc;

		// 反転合成
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		m_blendStates[static_cast<size_t>(BlendState::INV)] = blendDesc;
	}

	// 深度ステンシルステート作成
	{
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};

		m_depthStencilStates[static_cast<size_t>(DepthStencilState::UNKNOWN)] = depthStencilDesc;

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		m_depthStencilStates[static_cast<size_t>(DepthStencilState::ENABLE_TEST_AND_ENABLE_WRITE)] = depthStencilDesc;

		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		m_depthStencilStates[static_cast<size_t>(DepthStencilState::ENABLE_TEST_AND_DISABLE_WRITE)] = depthStencilDesc;

		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		m_depthStencilStates[static_cast<size_t>(DepthStencilState::DISABLE_TEST_AND_ENABLE_WRITE)] = depthStencilDesc;

		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		m_depthStencilStates[static_cast<size_t>(DepthStencilState::DISABLE_TEST_AND_DISABLE_WRITE)] = depthStencilDesc;
	}

	return S_OK;
}
