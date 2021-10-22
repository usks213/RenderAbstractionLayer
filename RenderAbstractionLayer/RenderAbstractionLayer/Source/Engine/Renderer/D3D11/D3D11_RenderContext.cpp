/*****************************************************************//**
 * \file   D3D11_RenderContext.h
 * \brief  DirectX11�����_�[�R���e�L�X�g�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#include "D3D11_RenderContext.h"
#include "D3D11_Renderer.h"

#include "D3D11_Buffer.h"
#include "D3D11_RenderBuffer.h"
#include "D3D11_Texture.h"
#include "D3D11_RenderTarget.h"
#include "D3D11_DepthStencil.h"

#include <Renderer/Core/Core_SubResource.h>

#include <functional>

using namespace core;
using namespace d3d11;


//------------------------------------------------------------------------------
// local methods 
//------------------------------------------------------------------------------
namespace {
	// CBuffer
	static std::function<void(ID3D11DeviceContext1*, UINT, UINT, ID3D11Buffer* const*)>
		setCBuffer[static_cast<std::size_t>(ShaderStage::MAX)] = {
		&ID3D11DeviceContext1::VSSetConstantBuffers,
		&ID3D11DeviceContext1::GSSetConstantBuffers,
		&ID3D11DeviceContext1::DSSetConstantBuffers,
		&ID3D11DeviceContext1::HSSetConstantBuffers,
		&ID3D11DeviceContext1::PSSetConstantBuffers,
		&ID3D11DeviceContext1::CSSetConstantBuffers, };
	// SRV
	static std::function<void(ID3D11DeviceContext1*, UINT, UINT, ID3D11ShaderResourceView* const*)>
		setShaderResource[static_cast<std::size_t>(ShaderStage::MAX)] = {
		&ID3D11DeviceContext1::VSSetShaderResources,
		&ID3D11DeviceContext1::GSSetShaderResources,
		&ID3D11DeviceContext1::DSSetShaderResources,
		&ID3D11DeviceContext1::HSSetShaderResources,
		&ID3D11DeviceContext1::PSSetShaderResources,
		&ID3D11DeviceContext1::CSSetShaderResources, };
	// Sampler
	static std::function<void(ID3D11DeviceContext1*, UINT, UINT, ID3D11SamplerState* const*)>
		setSamplers[static_cast<std::size_t>(ShaderStage::MAX)] = {
		&ID3D11DeviceContext1::VSSetSamplers,
		&ID3D11DeviceContext1::GSSetSamplers,
		&ID3D11DeviceContext1::DSSetSamplers,
		&ID3D11DeviceContext1::HSSetSamplers,
		&ID3D11DeviceContext1::PSSetSamplers,
		&ID3D11DeviceContext1::CSSetSamplers, };
}

//------------------------------------------------------------------------------
// public methods 
//------------------------------------------------------------------------------

/// @brief �R���X�g���N�^
D3D11RenderContext::D3D11RenderContext() :
	m_pRenderer(nullptr),
	m_pDevice(nullptr),
	m_pD3DContext(nullptr),
	m_pD3DDeffered(nullptr),

	m_systemBuffer(nullptr),
	m_transformBuffer(nullptr),
	m_animationBuffer(nullptr),
	m_pointLightBuffer(nullptr),
	m_spotLightBuffer(nullptr),
	m_pointLightSRV(nullptr),
	m_spotLightSRV(nullptr),

	m_curBlendState(BlendState::NONE),
	m_curRasterizeState(RasterizeState::CULL_NONE),
	m_curDepthStencilState(DepthStencilState::UNKNOWN),
	m_curPrimitiveTopology(PrimitiveTopology::UNKNOWN),

	m_curSamplerState(),
	m_curTexture(),

	m_curShader(NONE_SHADER_ID),
	m_curMaterial(NONE_MATERIAL_ID),
	m_curRenderBuffer(NONE_RENDERBUFFER_ID),
	m_curRenderTarget(NONE_RENDER_TARGET_ID),
	m_curDepthStencil(NONE_DEPTH_STENCIL_ID)
{
}

/// @brief ����������
/// @param pRenderer D3D11�����_���[�|�C���^
/// @param pDevice D3D11�f�o�C�X�|�C���^
/// @return ������: ���� true | ���s false
HRESULT D3D11RenderContext::initialize(D3D11Renderer* pRenderer, D3D11RenderDevice* pDevice)
{
	//--- ������
	m_pRenderer = pRenderer;
	m_pDevice = pDevice;

	//--- �o�b�t�@�̐���

	// ����CBuffer�̏�����
	D3D11_BUFFER_DESC d3dDesc = {};
	d3dDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDesc.CPUAccessFlags = 0;
	d3dDesc.MiscFlags = 0;

	// System
	d3dDesc.ByteWidth = sizeof(SHADER::SystemBuffer);
	CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateBuffer(&d3dDesc, nullptr, m_systemBuffer.ReleaseAndGetAddressOf()));
	// Transform
	d3dDesc.ByteWidth = sizeof(SHADER::TransformBuffer);
	CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateBuffer(&d3dDesc, nullptr, m_transformBuffer.ReleaseAndGetAddressOf()));

	// Map/Unmap Param
	d3dDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// Animation
	d3dDesc.ByteWidth = sizeof(SHADER::AnimationBuffer);
	CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateBuffer(&d3dDesc, nullptr, m_animationBuffer.ReleaseAndGetAddressOf()));

	// Light
	d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	d3dDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;

	// �|�C���g���C�g
	d3dDesc.ByteWidth = sizeof(CorePointLight) * SHADER::MAX_POINT_LIGHT_COUNT;
	d3dDesc.StructureByteStride = sizeof(CorePointLight);
	CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateBuffer(&d3dDesc, nullptr, m_pointLightBuffer.ReleaseAndGetAddressOf()));
	srvDesc.Buffer.NumElements = SHADER::MAX_POINT_LIGHT_COUNT;
	CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateShaderResourceView(m_pointLightBuffer.Get(), nullptr, m_pointLightSRV.ReleaseAndGetAddressOf()));
	// �X�|�b�g���C�g
	d3dDesc.ByteWidth = sizeof(CoreSpotLight) * SHADER::MAX_SPOT_LIGHT_COUNT;
	d3dDesc.StructureByteStride = sizeof(CoreSpotLight);
	CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateBuffer(&d3dDesc, nullptr, m_spotLightBuffer.ReleaseAndGetAddressOf()));
	srvDesc.Buffer.NumElements = SHADER::MAX_SPOT_LIGHT_COUNT;
	CHECK_FAILED(m_pDevice->m_pD3DDevice->CreateShaderResourceView(m_spotLightBuffer.Get(), nullptr, m_spotLightSRV.ReleaseAndGetAddressOf()));

	//// Sampler
	//for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	//{
	//	setSamplerResource(static_cast<std::uint32_t>(core::SHADER::GetSlotByName(
	//		core::BindType::SAMPLER, "Main")), SamplerState::LINEAR_WRAP, stage);
	//	setSamplerResource(static_cast<std::uint32_t>(core::SHADER::GetSlotByName(
	//		core::BindType::SAMPLER, "Shadow")), SamplerState::SHADOW, stage);
	//	setSamplerResource(static_cast<std::uint32_t>(core::SHADER::GetSlotByName(
	//		core::BindType::SAMPLER, "Sky")), SamplerState::ANISOTROPIC_WRAP, stage);
	//}

	return S_OK;
}

//----- ���\�[�X�w�薽�� -----

void D3D11RenderContext::setMaterial(const core::MaterialID& materialID)
{
	// �}�e���A���̎擾
	auto* d3dMat = static_cast<D3D11Material*>(m_pDevice->getMaterial(materialID));
	if (d3dMat == nullptr) return;

	// �}�e���A��������
	//if (m_curMaterial == materialID) return;
	//m_curMaterial = materialID;

	// �V�F�[�_�[�̎擾
	auto* d3dShader = static_cast<D3D11Shader*>(m_pDevice->getShader(d3dMat->m_shaderID));
	if (d3dShader == nullptr) return;

	// �V�F�[�_�[�̎w��
	if (m_curShader != d3dMat->m_shaderID)
	{
		if (d3dShader->vs) m_pD3DContext->VSSetShader(d3dShader->vs, nullptr, 0);
		if (d3dShader->gs) m_pD3DContext->GSSetShader(d3dShader->gs, nullptr, 0);
		if (d3dShader->ds) m_pD3DContext->DSSetShader(d3dShader->ds, nullptr, 0);
		if (d3dShader->hs) m_pD3DContext->HSSetShader(d3dShader->hs, nullptr, 0);
		if (d3dShader->ps) m_pD3DContext->PSSetShader(d3dShader->ps, nullptr, 0);
		if (d3dShader->cs) m_pD3DContext->CSSetShader(d3dShader->cs, nullptr, 0);
		// ���̓��C�A�E�g�w��
		m_pD3DContext->IASetInputLayout(d3dShader->m_inputLayout.Get());
		m_curShader = d3dMat->m_shaderID;
	}

	// �u�����h�X�e�[�g
	if (m_curBlendState != d3dMat->m_blendState)
	{
		constexpr float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_pD3DContext->OMSetBlendState(m_pDevice->m_blendStates[
			static_cast<std::size_t>(d3dMat->m_blendState)].Get(), blendFactor, 0xffffffff);
		m_curBlendState = d3dMat->m_blendState;
	}
	// ���X�^���C�U�X�e�[�g
	if (m_curRasterizeState != d3dMat->m_rasterizeState)
	{
		m_pD3DContext->RSSetState(m_pDevice->m_rasterizeStates[
			static_cast<std::size_t>(d3dMat->m_rasterizeState)].Get());
		m_curRasterizeState = d3dMat->m_rasterizeState;
	}
	// �[�x�X�e���V���X�e�[�g
	if (m_curDepthStencilState != d3dMat->m_depthStencilState)
	{
		if (d3dMat->m_isTransparent)
		{
			m_pD3DContext->OMSetDepthStencilState(m_pDevice->m_depthStencilStates[
				static_cast<std::size_t>(DepthStencilState::ENABLE_TEST_AND_DISABLE_WRITE)].Get(), 0);
			m_curDepthStencilState = DepthStencilState::ENABLE_TEST_AND_DISABLE_WRITE;
		}
		else
		{
			m_pD3DContext->OMSetDepthStencilState(m_pDevice->m_depthStencilStates[
				static_cast<std::size_t>(d3dMat->m_depthStencilState)].Get(), 0);
			m_curDepthStencilState = d3dMat->m_depthStencilState;
		}
	}

	// �}�e���A�����\�[�X�w��E�X�V
	setMaterialResource(*d3dMat, *d3dShader);
}

void D3D11RenderContext::setRenderBuffer(const core::RenderBufferID& renderBufferID)
{
	auto* context = m_pD3DContext;

	// �f�[�^�̎擾
	auto* renderBuffer = static_cast<D3D11RenderBuffer*>(m_pDevice->getRenderBuffer(renderBufferID));

	// ���_�o�b�t�@���Z�b�g
	UINT stride = static_cast<UINT>(renderBuffer->m_vertexData.size);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, renderBuffer->m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	if (renderBuffer->m_indexData.count > 0) {
		context->IASetIndexBuffer(renderBuffer->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	// �v���~�e�B�u�w��
	if (m_curPrimitiveTopology == renderBuffer->m_topology) return;
	m_pD3DContext->IASetPrimitiveTopology(getD3D11PrimitiveTopology(renderBuffer->m_topology));
	m_curPrimitiveTopology = renderBuffer->m_topology;

}

//----- �o�C���h���� -----

void D3D11RenderContext::bindBuffer(const std::string& bindName, const core::ShaderID& shaderID, const core::BufferID bufferID)
{
	auto pShader = static_cast<D3D11Shader*>(m_pDevice->getShader(shaderID));
	auto* pBuffer = static_cast<D3D11Buffer*>(m_pDevice->getBuffer(bufferID));
	auto type = static_cast<std::size_t>(pBuffer->m_type);

	// �X�e�[�W����
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(pShader->m_desc.m_stages, stage)) continue;
		auto stageIndex = static_cast<std::size_t>(stage);

		auto itr = pShader->m_staticBindData[stageIndex][type].find(bindName.data());
		if (pShader->m_staticBindData[stageIndex][type].end() != itr)
		{
			// GPU�o�b�t�@�X�V
			if (pBuffer->m_isUpdate)
			{
				if (pBuffer->m_desc.usage == core::Usage::DYNAMIC || 
					pBuffer->m_desc.usage == core::Usage::STAGING)
				{
					D3D11_MAPPED_SUBRESOURCE subData = {};
					m_pD3DContext->Map(pBuffer->m_pBuffer.Get(), 0,
						D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &subData);
					std::memcpy(subData.pData, pBuffer->m_aData.data(), pBuffer->m_aData.size());
					m_pD3DContext->Unmap(pBuffer->m_pBuffer.Get(), 0);
				}
				else if(pBuffer->m_desc.usage == core::Usage::DEFAULT)
				{
					m_pD3DContext->UpdateSubresource(pBuffer->m_pBuffer.Get(), 0, nullptr,
						pBuffer->m_aData.data(), 0, 0);
				}
				pBuffer->m_isUpdate = false;
			}

			// CBV,SRV,UAV
			if (pBuffer->m_type == CoreBuffer::BufferType::CBV)
			{
				setCBuffer[stageIndex](m_pD3DContext, itr->second.slot,
					1, pBuffer->m_pBuffer.GetAddressOf());
			}
			else if (pBuffer->m_type == CoreBuffer::BufferType::SRV)
			{
				setShaderResource[stageIndex](m_pD3DContext, itr->second.slot,
					1, pBuffer->m_pSRV.GetAddressOf());
			}
			else if (pBuffer->m_type == CoreBuffer::BufferType::UAV)
			{
				if (stage == ShaderStage::CS)
				{
					m_pD3DContext->CSGetUnorderedAccessViews(itr->second.slot,
						1, pBuffer->m_pUAV.GetAddressOf());
				}
			}
			break;
		}
	}
}

void D3D11RenderContext::bindTexture(const std::string& bindName, const core::ShaderID& shaderID, const core::TextureID textureID)
{

}

void D3D11RenderContext::bindSampler(const std::string& bindName, const core::ShaderID& shaderID, const core::SamplerState sampler)
{

}


//----- �`�施��

void D3D11RenderContext::render(const core::RenderBufferID& renderBufferID, std::uint32_t instanceCount)
{
	auto* context = m_pD3DContext;

	// �f�[�^�̎擾
	auto* renderBuffer = static_cast<D3D11RenderBuffer*>(m_pDevice->getRenderBuffer(renderBufferID));

	// �|���S���̕`��
	if (renderBuffer->m_indexData.count > 0)
	{
		context->DrawIndexedInstanced(renderBuffer->m_indexData.count, instanceCount, 0, 0, 0);
	}
	else
	{
		context->DrawInstanced(renderBuffer->m_vertexData.count, instanceCount, 0, 0);
	}
}


//------------------------------------------------------------------------------
// private methods 
//------------------------------------------------------------------------------

void D3D11RenderContext::setMaterialResource(const D3D11Material& d3dMaterial, const D3D11Shader& d3dShader)
{
	auto& d3dMat = const_cast<D3D11Material&>(d3dMaterial);

	// �X�e�[�W����
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(d3dShader.m_desc.m_stages, stage)) continue;

		auto stageIndex = static_cast<std::size_t>(stage);

		// �R���X�^���g�o�b�t�@
		for (const auto& cbuffer : d3dMaterial.m_d3dCbuffer[stageIndex])
		{
			// �w��
			setCBuffer[stageIndex](m_pD3DContext, cbuffer.first, 1, cbuffer.second.GetAddressOf());
			// �X�V
			if (d3dMat.m_cbufferData[stageIndex][cbuffer.first].isUpdate)
			{
				m_pD3DContext->UpdateSubresource(cbuffer.second.Get(), 0, nullptr,
					d3dMat.m_cbufferData[stageIndex][cbuffer.first].data.get(), 0, 0);
				d3dMat.m_cbufferData[stageIndex][cbuffer.first].isUpdate = false;
			}
		}

		// �e�N�X�`���X�V
		for (const auto& tex : d3dMat.m_textureData[stageIndex])
		{
			setTextureResource(tex.first, tex.second.id, stage);
		}

		// �T���v���X�V
		for (const auto& sam : d3dMat.m_samplerData[stageIndex])
		{
			setSamplerResource(sam.first, sam.second.state, stage);
		}
	}
}

void D3D11RenderContext::setTextureResource(std::uint32_t slot, const core::TextureID& textureID, core::ShaderStage stage)
{
	auto stageIndex = static_cast<std::size_t>(stage);
	if (m_curTexture[stageIndex][slot] == textureID) return;


	if (textureID == NONE_TEXTURE_ID)
	{
		ID3D11ShaderResourceView* nullView = nullptr;
		setShaderResource[stageIndex](m_pD3DContext, slot, 1, &nullView);
		m_curTexture[stageIndex][slot] = NONE_TEXTURE_ID;
	}
	else
	{
		D3D11Texture* pD3DTex = static_cast<D3D11Texture*>(m_pDevice->getTexture(textureID));
		ID3D11ShaderResourceView* pTex = pD3DTex ? pD3DTex->m_srv.Get() : nullptr;
		setShaderResource[stageIndex](m_pD3DContext, slot, 1, &pTex);
		m_curTexture[stageIndex][slot] = textureID;
	}
}

void D3D11RenderContext::setSamplerResource(std::uint32_t slot, core::SamplerState state, core::ShaderStage stage)
{
	auto stageIndex = static_cast<size_t>(stage);
	if (m_curSamplerState[stageIndex][slot] == state) {
		return;
	}

	setSamplers[stageIndex](m_pD3DContext, slot, 1, m_pDevice->m_samplerStates[static_cast<size_t>(state)].GetAddressOf());
	m_curSamplerState[stageIndex][slot] = state;
}