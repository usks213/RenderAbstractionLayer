/*****************************************************************//**
 * \file   D3D12_RenderDevice.h
 * \brief  DirectX12�����_�[�f�o�C�X
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

/// @brief �R���X�g���N�^
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

/// @brief ����������
/// @param hWnd �E�B���h�E�n���h��
/// @param width �E�B���h�E�̕�
/// @param height �E�B���h�E�̍���
/// @return ������: ���� true | ���s false
HRESULT D3D12RenderDevice::initialize(ID3D12Device* pDevice, IDXGIFactory2* pFactory2,
	HWND hWnd, UINT width, UINT height)
{
	// ������
	m_pD3DDevice = pDevice;
	m_hWnd = hWnd;
	m_nOutputWidth = width;
	m_nOutputHeight = height;

	// ���ʃX�e�[�g�̏�����
	CHECK_FAILED(createCommonState());

	return S_OK;
}

//----- ���\�[�X���� -----

core::BufferID D3D12RenderDevice::createBuffer(core::BufferDesc& desc, core::BufferData* pData)
{
	// ID�̎擾
	BufferID id = static_cast<BufferID>(hash::stringHash(desc.name));

	// ���ɐ����ς�
	const auto& itr = m_bufferPool.find(id);
	if (m_bufferPool.end() != itr) return id;

	// �V�K����
	//m_bufferPool[id] = std::make_unique<D3D12Buffer>(m_pD3DDevice, id, desc, pData);

	return id;
}
core::DepthStencilID D3D12RenderDevice::createDepthStencil(std::string name)
{
	// ID�̎擾
	DepthStencilID id = static_cast<DepthStencilID>(hash::stringHash(name));

	// ���ɐ����ς�
	const auto& itr = m_depthStencilPool.find(id);
	if (m_depthStencilPool.end() != itr) return id;

	// �e�N�X�`������
	TextureDesc texDesc;
	texDesc.bindFlags = BindFlags::DEPTH_STENCIL | BindFlags::SHADER_RESOURCE;

	// �f�v�X�X�e���V������

	return id;
}
core::MaterialID D3D12RenderDevice::createMaterial(std::string name, core::ShaderID& shaderID)
{
	// ID�̎擾
	MaterialID id = static_cast<MaterialID>(hash::stringHash(name));

	// ���ɐ����ς�
	const auto& itr = m_materialPool.find(id);
	if (m_materialPool.end() != itr) return id;

	// �V�F�[�_�[�擾
	auto* shader = getShader(shaderID);
	if (shader == nullptr) return NONE_MATERIAL_ID;

	// �V�K����
	m_materialPool[id] = std::make_unique<D3D12Material>(
		m_pD3DDevice, id, name, *shader);

	return id;
}
core::MeshID D3D12RenderDevice::createMesh(std::string name)
{
	// ID�̎擾
	MeshID id = static_cast<MeshID>(hash::stringHash(name));

	// ���ɐ����ς�
	const auto& itr = m_meshPool.find(id);
	if (m_meshPool.end() != itr) return id;

	// �V�K����
	m_meshPool[id] = std::make_unique<CoreMesh>(id, name);

	return id;
}
core::RenderBufferID D3D12RenderDevice::createRenderBuffer(core::ShaderID& shaderID, core::MeshID& meshID)
{
	// �V�F�[�_�[�擾
	auto* shader = getShader(shaderID);
	if (shader == nullptr) return NONE_RENDERBUFFER_ID;
	// ���b�V���擾
	auto* mesh = getMesh(meshID);
	if (mesh == nullptr) return NONE_RENDERBUFFER_ID;
	
	// ID�̐���
	RenderBufferID id = static_cast<RenderBufferID>(
		hash::stringHash(shader->m_desc.m_name + ":" + mesh->m_name));

	// ���ɂ���
	const auto& itr = m_renderBufferPool.find(id);
	if (m_renderBufferPool.end() != itr) return id;

	// �V�K����
	m_renderBufferPool[id] = std::make_unique<D3D12RenderBuffer>(
		m_pD3DDevice, id, *shader, *mesh);

	return id;
}
core::RenderTargetID D3D12RenderDevice::createRenderTarget(std::string name)
{
	// ID�̎擾
	RenderTargetID id = static_cast<RenderTargetID>(hash::stringHash(name));

	// ���ɐ����ς�
	const auto& itr = m_renderTargetPool.find(id);
	if (m_renderTargetPool.end() != itr) return id;

	// �e�N�X�`������
	TextureDesc texDesc;
	texDesc.bindFlags = BindFlags::RENDER_TARGET | BindFlags::SHADER_RESOURCE;

	// �����_�[�^�[�Q�b�g����

	return id;
}
core::ShaderID D3D12RenderDevice::createShader(core::ShaderDesc& desc)
{
	// ID�̎擾
	ShaderID id = static_cast<ShaderID>(hash::stringHash(desc.m_name));

	// ���ɐ����ς�
	const auto& itr = m_shaderPool.find(id);
	if (m_shaderPool.end() != itr) return id;

	// �V�K����
	m_shaderPool[id] = std::make_unique<D3D12Shader>(m_pD3DDevice, desc, id);

	return id;
}
core::TextureID D3D12RenderDevice::createTexture(std::string filePath)
{
	// ID�̎擾
	TextureID id = static_cast<TextureID>(hash::stringHash(filePath));

	// ���ɐ����ς�
	const auto& itr = m_texturePool.find(id);
	if (m_texturePool.end() != itr) return id;

	// �V�K����
	//m_texturePool[id] = std::make_unique<D3D12Texture>(m_pD3DDevice, id, filePath);

	return id;
}
core::TextureID D3D12RenderDevice::createTexture(core::TextureDesc& desc, core::TextureData* pData)
{
	// ID�̎擾
	TextureID id = static_cast<TextureID>(hash::stringHash(desc.name));

	// ���ɐ����ς�
	const auto& itr = m_texturePool.find(id);
	if (m_texturePool.end() != itr) return id;

	// �V�K����
	//m_texturePool[id] = std::make_unique<D3D12Texture>(m_pD3DDevice, id, desc, pData);

	return id;
}


 //------------------------------------------------------------------------------
 // private methods
 //------------------------------------------------------------------------------


/// @brief ���ʃX�e�[�g�̐���
/// @return HRESULT
HRESULT D3D12RenderDevice::createCommonState()
{
	// ���X�^���C�U�X�e�[�g�쐬
	{
		D3D12_RASTERIZER_DESC rasterizerDesc = {};

		// �J�����O�Ȃ� �h��Ԃ�
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.CullMode =D3D12_CULL_MODE_NONE;
		m_rasterizeStates[(size_t)RasterizeState::CULL_NONE] = rasterizerDesc;

		// �\�ʃJ�����O �h��Ԃ�
		rasterizerDesc.CullMode =D3D12_CULL_MODE_FRONT;
		m_rasterizeStates[(size_t)RasterizeState::CULL_FRONT] = rasterizerDesc;

		// ���ʃJ�����O �h��Ԃ�
		rasterizerDesc.CullMode =D3D12_CULL_MODE_BACK;
		m_rasterizeStates[(size_t)RasterizeState::CULL_BACK] = rasterizerDesc;

		// �J�����O�Ȃ� ���C���[�t���[��
		rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
		rasterizerDesc.CullMode =D3D12_CULL_MODE_NONE;
		m_rasterizeStates[(size_t)RasterizeState::CULL_NONE_WIREFRAME] = rasterizerDesc;

		// �\�ʃJ�����O ���C���[�t���[��
		rasterizerDesc.CullMode =D3D12_CULL_MODE_FRONT;
		m_rasterizeStates[(size_t)RasterizeState::CULL_FRONT_WIREFRAME] = rasterizerDesc;

		// ���ʃJ�����O ���C���[�t���[��
		rasterizerDesc.CullMode =D3D12_CULL_MODE_BACK;
		m_rasterizeStates[(size_t)RasterizeState::CULL_BACK_WIREFRAME] = rasterizerDesc;

		// �V���h�E�p
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.CullMode =D3D12_CULL_MODE_BACK;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.DepthBias = 100000;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.SlopeScaledDepthBias = 2.0f;
		m_rasterizeStates[(size_t)RasterizeState::SHADOW] = rasterizerDesc;
	}

	// �T���v���X�e�[�g�쐬
	{
		D3D12_STATIC_SAMPLER_DESC samplerDesc = {};

		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.MaxAnisotropy = D3D12_MAX_MAXANISOTROPY;

		// ���j�A�N�����v
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		m_samplerStates[static_cast<size_t>(SamplerState::LINEAR_CLAMP)] = samplerDesc;

		// �|�C���g�N�����v
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		m_samplerStates[static_cast<size_t>(SamplerState::POINT_CLAMP)] = samplerDesc;

		// �ٕ����N�����v
		samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
		m_samplerStates[static_cast<size_t>(SamplerState::ANISOTROPIC_CLAMP)] = samplerDesc;

		// ���j�A���b�v
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		m_samplerStates[static_cast<size_t>(SamplerState::LINEAR_WRAP)] = samplerDesc;

		// �|�C���g���b�v
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		m_samplerStates[static_cast<size_t>(SamplerState::POINT_WRAP)] = samplerDesc;

		// �ٕ������b�v
		samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
		m_samplerStates[static_cast<size_t>(SamplerState::ANISOTROPIC_WRAP)] = samplerDesc;

		// �V���h�E
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

	// �u�����h�X�e�[�g�쐬
	{
		D3D12_BLEND_DESC blendDesc = {};

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		// ����������
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		m_blendStates[static_cast<size_t>(BlendState::ALPHA)] = blendDesc;

		// ���Z����
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		m_blendStates[static_cast<size_t>(BlendState::ADD)] = blendDesc;

		// ���Z����
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		m_blendStates[static_cast<size_t>(BlendState::SUB)] = blendDesc;

		// ��Z����
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		m_blendStates[static_cast<size_t>(BlendState::MUL)] = blendDesc;

		// ���]����
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		m_blendStates[static_cast<size_t>(BlendState::INV)] = blendDesc;
	}

	// �[�x�X�e���V���X�e�[�g�쐬
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
