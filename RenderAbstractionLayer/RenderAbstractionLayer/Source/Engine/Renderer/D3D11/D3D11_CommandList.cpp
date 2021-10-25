/*****************************************************************//**
 * \file   D3D11_CommandList.h
 * \brief  DirectX11�����_�[�R�}���h���X�g�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#include "D3D11_CommandList.h"
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
D3D11CommandList::D3D11CommandList() :
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
	m_curRenderTargets{ NONE_RENDER_TARGET_ID, NONE_RENDER_TARGET_ID, NONE_RENDER_TARGET_ID, NONE_RENDER_TARGET_ID, 
		NONE_RENDER_TARGET_ID, NONE_RENDER_TARGET_ID, NONE_RENDER_TARGET_ID, NONE_RENDER_TARGET_ID, },
	m_curDepthStencil(NONE_DEPTH_STENCIL_ID)
{
}

/// @brief ����������
/// @param pRenderer D3D11�����_���[�|�C���^
/// @param pDevice D3D11�f�o�C�X�|�C���^
/// @return ������: ���� true | ���s false
HRESULT D3D11CommandList::initialize(D3D11Renderer* pRenderer, D3D11Device* pDevice)
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

void D3D11CommandList::setMaterial(const core::MaterialID& materialID)
{
	// �����}�e���A��
	if (m_curMaterial == materialID) return;
	m_curMaterial = materialID;

	// �}�e���A���̎擾
	auto* d3dMat = static_cast<D3D11Material*>(m_pDevice->getMaterial(materialID));
	if (d3dMat == nullptr) return;

	// �p�C�v���C���X�e�[�g�̐ݒ�
	setGraphicsPipelineState(d3dMat->m_shaderID, d3dMat->m_blendState, 
		d3dMat->m_rasterizeState, d3dMat->m_depthStencilState);

	// �}�e���A�����\�[�X�w��E�X�V
		// �X�e�[�W����
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		auto stageIndex = static_cast<std::size_t>(stage);

		// �R���X�^���g�o�b�t�@
		for (const auto& cbuffer : d3dMat->m_d3dCbuffer[stageIndex])
		{
			// �X�V
			if (d3dMat->m_cbufferData[stageIndex][cbuffer.first].isUpdate)
			{
				m_pD3DContext->UpdateSubresource(cbuffer.second.Get(), 0, nullptr,
					d3dMat->m_cbufferData[stageIndex][cbuffer.first].data.get(), 0, 0);
				d3dMat->m_cbufferData[stageIndex][cbuffer.first].isUpdate = false;
			}
			// �w��
			setCBuffer[stageIndex](m_pD3DContext, cbuffer.first, 1, cbuffer.second.GetAddressOf());
		}

		// �e�N�X�`���X�V
		for (const auto& tex : d3dMat->m_textureData[stageIndex])
		{
			setTextureResource(tex.first, tex.second.id, stage);
		}

		// �T���v���X�V
		for (const auto& sam : d3dMat->m_samplerData[stageIndex])
		{
			setSamplerResource(sam.first, sam.second.state, stage);
		}
	}
}

void D3D11CommandList::setRenderBuffer(const core::RenderBufferID& renderBufferID)
{
	auto* cmdList = m_pD3DContext;

	// �f�[�^�̎擾
	auto* renderBuffer = static_cast<D3D11RenderBuffer*>(m_pDevice->getRenderBuffer(renderBufferID));

	// ���_�o�b�t�@���Z�b�g
	UINT stride = static_cast<UINT>(renderBuffer->m_vertexData.size);
	UINT offset = 0;
	cmdList->IASetVertexBuffers(0, 1, renderBuffer->m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	if (renderBuffer->m_indexData.count > 0) {
		cmdList->IASetIndexBuffer(renderBuffer->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	// �v���~�e�B�u�w��
	if (m_curPrimitiveTopology == renderBuffer->m_topology) return;
	m_pD3DContext->IASetPrimitiveTopology(getD3D11PrimitiveTopology(renderBuffer->m_topology));
	m_curPrimitiveTopology = renderBuffer->m_topology;

}

//----- �Z�b�g���� -----

void D3D11CommandList::setBackBuffer()
{
	m_pD3DContext->OMSetRenderTargets(1, m_pDevice->m_backBufferRTV.GetAddressOf(),
		m_pDevice->m_depthStencilView.Get());
}

void D3D11CommandList::setGraphicsPipelineState(const ShaderID& shaderID, const BlendState& bs,
	const RasterizeState& rs, const DepthStencilState& ds)
{
	// �V�F�[�_�[�̎w��
	if (m_curShader != shaderID)
	{
		m_curShader = shaderID;
		// �V�F�[�_�[�̎擾
		auto* d3dShader = static_cast<D3D11Shader*>(m_pDevice->getShader(shaderID));
		if (d3dShader)
		{
			// �V�F�[�_�[�f�[�^�w��
			if (d3dShader->vs) m_pD3DContext->VSSetShader(d3dShader->vs, nullptr, 0);
			if (d3dShader->gs) m_pD3DContext->GSSetShader(d3dShader->gs, nullptr, 0);
			if (d3dShader->ds) m_pD3DContext->DSSetShader(d3dShader->ds, nullptr, 0);
			if (d3dShader->hs) m_pD3DContext->HSSetShader(d3dShader->hs, nullptr, 0);
			if (d3dShader->ps) m_pD3DContext->PSSetShader(d3dShader->ps, nullptr, 0);
			if (d3dShader->cs) m_pD3DContext->CSSetShader(d3dShader->cs, nullptr, 0);
			// ���̓��C�A�E�g�w��
			m_pD3DContext->IASetInputLayout(d3dShader->m_inputLayout.Get());
		}
	}

	// �u�����h�X�e�[�g
	if (m_curBlendState != bs)
	{
		constexpr float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_pD3DContext->OMSetBlendState(m_pDevice->m_blendStates[
			static_cast<std::size_t>(bs)].Get(), blendFactor, 0xffffffff);
		m_curBlendState = bs;
	}
	// ���X�^���C�U�X�e�[�g
	if (m_curRasterizeState != rs)
	{
		m_pD3DContext->RSSetState(m_pDevice->m_rasterizeStates[
			static_cast<std::size_t>(rs)].Get());
		m_curRasterizeState = rs;
	}
	// �[�x�X�e���V���X�e�[�g
	if (m_curDepthStencilState != ds)
	{
		m_pD3DContext->OMSetDepthStencilState(m_pDevice->m_depthStencilStates[
			static_cast<std::size_t>(ds)].Get(), 0);
		m_curDepthStencilState = ds;
	}
}

void D3D11CommandList::setRenderTarget(const RenderTargetID& rtID)
{
	RenderTargetID rtIDs[] = { rtID };
	setRenderTarget(1, rtIDs);
}

void D3D11CommandList::setRenderTarget(const std::uint32_t num, const RenderTargetID rtIDs[])
{
	// ���S����
	if (num >= MAX_RENDER_TARGET || num <= 0) return;

	// �����_�[�^�[�Q�b�g�擾
	std::vector<ID3D11RenderTargetView*> rtvs(num);
	for (int i = 0; i < num; ++i)
	{
		auto* pRT = static_cast<D3D11RenderTarget*>(m_pDevice->getRenderTarget(rtIDs[i]));
		if (pRT) rtvs.push_back(pRT->m_rtv.Get());
		else rtvs.push_back(nullptr);
		m_curRenderTargets[i] = rtIDs[i];
	}

	// ���݂̃f�v�X�X�e���V���擾
	ID3D11DepthStencilView* pDSV = nullptr;
	auto* pDS = static_cast<D3D11DepthStencil*>(m_pDevice->getDepthStencil(m_curDepthStencil));
	if (pDS) pDSV = pDS->m_dsv.Get();

	// �����_�[�^�[�Q�b�g�w��
	m_pD3DContext->OMSetRenderTargets(num, rtvs.data(), pDSV);

}

void D3D11CommandList::setRenderTarget(const RenderTargetID& rtID, const DepthStencilID& dsID)
{
	RenderTargetID rtIDs[] = { rtID };
;	setRenderTarget(1, rtIDs, dsID);
}

void D3D11CommandList::setRenderTarget(std::uint32_t num, const RenderTargetID rtIDs[], const DepthStencilID& dsID)
{
	// ���S����
	if (num >= MAX_RENDER_TARGET || num <= 0) return;

	// �����_�[�^�[�Q�b�g�擾
	std::vector<ID3D11RenderTargetView*> rtvs(num);
	for (int i = 0; i < num; ++i)
	{
		auto* pRT = static_cast<D3D11RenderTarget*>(m_pDevice->getRenderTarget(rtIDs[i]));
		if (pRT) rtvs.push_back(pRT->m_rtv.Get());
		else rtvs.push_back(nullptr);
		m_curRenderTargets[i] = rtIDs[i];
	}

	// ���݂̃f�v�X�X�e���V���擾
	ID3D11DepthStencilView* pDSV = nullptr;
	auto* pDS = static_cast<D3D11DepthStencil*>(m_pDevice->getDepthStencil(dsID));
	if (pDS) pDSV = pDS->m_dsv.Get();
	m_curDepthStencil = dsID;

	// �����_�[�^�[�Q�b�g�w��
	m_pD3DContext->OMSetRenderTargets(num, rtvs.data(), pDSV);
}

void D3D11CommandList::setViewport(const Rect& rect)
{
	D3D11_VIEWPORT d3d11View = {
		rect.left, rect.top, rect.right, rect.bottom, 0.0f, 1.0f
	};
	m_pD3DContext->RSSetViewports(1, &d3d11View);
}

void D3D11CommandList::setViewport(const Viewport& viewport)
{
	D3D11_VIEWPORT d3d11View = { viewport.left, viewport.top, 
		viewport.right, viewport.bottom, viewport.minDepth, viewport.maxDepth 
	};
	m_pD3DContext->RSSetViewports(1, &d3d11View);
}

//----- �Q�b�g���� -----


//----- �o�C���h���� -----

void D3D11CommandList::bindGlobalBuffer(const core::ShaderID& shaderID, const std::string& bindName, const core::BufferID& bufferID)
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

void D3D11CommandList::bindGlobalTexture(const core::ShaderID& shaderID, const std::string& bindName, const core::TextureID& textureID)
{
	constexpr auto type = static_cast<std::size_t>(BindType::TEXTURE);
	auto* pShader = static_cast<D3D11Shader*>(m_pDevice->getShader(shaderID));
	auto* pTexture = static_cast<D3D11Texture*>(m_pDevice->getTexture(textureID));

	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(pShader->m_desc.m_stages, stage)) continue;
		auto stageIndex = static_cast<std::size_t>(stage);

		auto itr = pShader->m_staticBindData[stageIndex][type].find(bindName.data());
		if (pShader->m_staticBindData[stageIndex][type].end() != itr)
		{
			setTextureResource(itr->second.slot, textureID, stage);
			break;
		}
	}
}

void D3D11CommandList::bindGlobalSampler(const core::ShaderID& shaderID, const std::string& bindName, const core::SamplerState& sampler)
{
	constexpr auto type = static_cast<std::size_t>(BindType::SAMPLER);
	auto* pShader = static_cast<D3D11Shader*>(m_pDevice->getShader(shaderID));
	const auto& samplerState = m_pDevice->m_samplerStates[static_cast<size_t>(sampler)];

	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(pShader->m_desc.m_stages, stage)) continue;
		auto stageIndex = static_cast<std::size_t>(stage);

		auto itr = pShader->m_staticBindData[stageIndex][type].find(bindName.data());
		if (pShader->m_staticBindData[stageIndex][type].end() != itr)
		{
			setSamplerResource(itr->second.slot, sampler, stage);
			break;
		}
	}
}


//----- �`�施��

void D3D11CommandList::render(const core::RenderBufferID& renderBufferID, std::uint32_t instanceCount)
{
	auto* cmdList = m_pD3DContext;

	// �f�[�^�̎擾
	auto* renderBuffer = static_cast<D3D11RenderBuffer*>(m_pDevice->getRenderBuffer(renderBufferID));

	// �|���S���̕`��
	if (renderBuffer->m_indexData.count > 0)
	{
		cmdList->DrawIndexedInstanced(renderBuffer->m_indexData.count, instanceCount, 0, 0, 0);
	}
	else
	{
		cmdList->DrawInstanced(renderBuffer->m_vertexData.count, instanceCount, 0, 0);
	}
}

/// @brief 
/// @param destID �Ώۂ̃����_�[�^�[�Q�b�g
/// @param sourceID 
/// @param matID 
void D3D11CommandList::blit(const RenderBufferID& destID, const TextureID& sourceID, const MaterialID& matID)
{

}

//----- ���̑� -----

void D3D11CommandList::clearCommand()
{
	// �R�}���h�̃N���A
}

void D3D11CommandList::clearRederTarget(const RenderTargetID& rtID, const Color& color)
{
	// �����_�[�^�[�Q�b�g�擾
	auto* pRT = static_cast<D3D11RenderTarget*>(m_pDevice->getRenderTarget(rtID));
	if (pRT == nullptr) return;

	FLOAT ColorRGBA[4];
	std::memcpy(ColorRGBA, &color, sizeof(Color));

	// �N���A�R�}���h
	m_pD3DContext->ClearRenderTargetView(pRT->m_rtv.Get(), ColorRGBA);
}

void D3D11CommandList::clearDepthStencil(const DepthStencilID& dsID, float depth, std::uint8_t stencil)
{
	// ���݂̃f�v�X�X�e���V���擾
	auto* pDS = static_cast<D3D11DepthStencil*>(m_pDevice->getDepthStencil(dsID));
	if (pDS == nullptr) return;

	// �N���A�R�}���h
	m_pD3DContext->ClearDepthStencilView(pDS->m_dsv.Get(), 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
}

//------------------------------------------------------------------------------
// private methods 
//------------------------------------------------------------------------------

void D3D11CommandList::setCBufferResource(std::uint32_t slot, const core::BufferID& bufferID, core::ShaderStage stage)
{

}

void D3D11CommandList::setTextureResource(std::uint32_t slot, const core::TextureID& textureID, core::ShaderStage stage)
{
	auto stageIndex = static_cast<std::size_t>(stage);
	if (m_curTexture[stageIndex][slot] == textureID) return;


	if (textureID == NONE_TEXTURE_ID)
	{
		// �e�N�X�`���Ȃ�
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

void D3D11CommandList::setSamplerResource(std::uint32_t slot, core::SamplerState state, core::ShaderStage stage)
{
	auto stageIndex = static_cast<size_t>(stage);
	if (m_curSamplerState[stageIndex][slot] == state) {
		return;
	}

	setSamplers[stageIndex](m_pD3DContext, slot, 1, m_pDevice->m_samplerStates[static_cast<size_t>(state)].GetAddressOf());
	m_curSamplerState[stageIndex][slot] = state;
}