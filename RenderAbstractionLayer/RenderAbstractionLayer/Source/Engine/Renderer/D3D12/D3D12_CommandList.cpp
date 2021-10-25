/*****************************************************************//**
 * \file   D3D12_CommandList.h
 * \brief  DiectX12�����_�[�R�}���h���X�g
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
//#include "D3D12_RenderTarget.h"
//#include "D3D12_DepthStencil.h"

#include <Renderer/Core/Core_SubResource.h>

#include <functional>

using namespace core;
using namespace d3d12;


//------------------------------------------------------------------------------
// public methods 
//------------------------------------------------------------------------------

/// @brief �R���X�g���N�^
D3D12CommandList::D3D12CommandList() :
	m_pRenderer(nullptr),
	m_pDevice(nullptr)
{
}

/// @brief ����������
/// @param pRenderer D3D12�����_���[�|�C���^
/// @param pDevice D3D12�f�o�C�X�|�C���^
/// @return ������: ���� true | ���s false
HRESULT D3D12CommandList::initialize(D3D12Renderer* pRenderer, D3D12Device* pDevice)
{
	//--- ������
	m_pRenderer = pRenderer;
	m_pDevice = pDevice;

	//----- �R�}���h�L���[�E�R�}���h�A���P�[�^�[�E�R�}���h���X�g -----
	HRESULT hr = S_OK;
	D3D12_COMMAND_LIST_TYPE cmdType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	UINT nodeMask = 0;

	// �R�}���h�A���P�[�^�[�̐���
	{
		hr = pDevice->m_pD3DDevice->CreateCommandAllocator(cmdType,
			IID_PPV_ARGS(m_pCmdAllocator.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
	}

	// �R�}���h���X�g�̐���
	{
		hr = pDevice->m_pD3DDevice->CreateCommandList(nodeMask, cmdType, m_pCmdAllocator.Get(),
			nullptr, IID_PPV_ARGS(m_pCmdList.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
		m_pCmdList->Close();
	}

	return S_OK;
}

//----- ���\�[�X�w�薽�� -----

void D3D12CommandList::setMaterial(const core::MaterialID& materialID)
{
	// �}�e���A���̎擾
	auto* d3dMat = static_cast<D3D12Material*>(m_pDevice->getMaterial(materialID));
	if (d3dMat == nullptr) return;

	// �V�F�[�_�[�̎擾
	auto* d3dShader = static_cast<D3D12Shader*>(m_pDevice->getShader(d3dMat->m_shaderID));
	if (d3dShader == nullptr) return;

	// �p�C�v���C���X�e�[�g�̎w��
	auto pipelineState = m_pDevice->createPipelineState(*d3dShader, *d3dMat);
	m_pCmdList->SetPipelineState(pipelineState);
	// ���[�g�V�O�l�`���[�̃Z�b�g
	m_pCmdList->SetGraphicsRootSignature(d3dShader->m_pRootSignature.Get());

	// �X�e�[�W����
	UINT rootIndex = 0;
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		if (!hasStaderStage(d3dShader->m_desc.m_stages, stage)) continue;

		auto stageIndex = static_cast<std::size_t>(stage);

		// �R���X�^���g�o�b�t�@�X�V
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

		// �R���X�^���g�o�b�t�@�w��
		if (d3dMat->m_pCBufferHeap[stageIndex])
		{
			// �q�[�v�w��
			ID3D12DescriptorHeap* pHeap[] = { d3dMat->m_pCBufferHeap[stageIndex].Get() };
			m_pCmdList->SetDescriptorHeaps(_countof(pHeap), pHeap);
			// �e�[�u���w��
			m_pCmdList->SetGraphicsRootDescriptorTable(rootIndex++,
				pHeap[0]->GetGPUDescriptorHandleForHeapStart());
		}

		// �e�N�X�`���X�V
		for (const auto& tex : d3dMat->m_textureData[stageIndex])
		{
			setTextureResource(rootIndex++, tex.second.id);
		}

		// �T���v���X�V
		for (const auto& sam : d3dMat->m_samplerData[stageIndex])
		{
			setSamplerResource(sam.first, sam.second.state);
		}
	}
}

void D3D12CommandList::setRenderBuffer(const core::RenderBufferID& renderBufferID)
{
	// �f�[�^�̎擾
	auto* renderBuffer = static_cast<D3D12RenderBuffer*>(m_pDevice->getRenderBuffer(renderBufferID));

	// ���_�o�b�t�@���Z�b�g
	UINT stride = static_cast<UINT>(renderBuffer->m_vertexData.size);
	UINT offset = 0;
	m_pCmdList->IASetVertexBuffers(0, 1, &renderBuffer->m_vertexBufferView);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	if (renderBuffer->m_indexData.count > 0) {
		m_pCmdList->IASetIndexBuffer(&renderBuffer->m_indexBufferVew);
	}

	// �v���~�e�B�u�w��
	m_pCmdList->IASetPrimitiveTopology(getD3D12PrimitiveTopology(renderBuffer->m_topology));

}

//----- �o�C���h���� -----

void D3D12CommandList::bindGlobalBuffer(const core::ShaderID& shaderID, const std::string& bindName, const core::BufferID bufferID)
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
			// GPU�X�V
			if (pBuffer->m_isUpdate)
			{
				void* pData = nullptr;
				pBuffer->m_pBuffer->Map(0, nullptr, &pData);
				std::memcpy(pData, pBuffer->m_aData.data(), pBuffer->m_aData.size());
				pBuffer->m_isUpdate = false;
			}

			// �q�[�v�w��
			m_pCmdList->SetDescriptorHeaps(1, pBuffer->m_pHeap.GetAddressOf());
			// �r���[�w��
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

void D3D12CommandList::bindGlobalTexture(const core::ShaderID& shaderID, const std::string& bindName, const core::TextureID textureID)
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
			// �q�[�v�w��
			m_pCmdList->SetDescriptorHeaps(1, pTexture->m_pTexHeap.GetAddressOf());
			// Table
			m_pCmdList->SetGraphicsRootDescriptorTable(
				itr->second.rootIndex,
				pTexture->m_pTexHeap->GetGPUDescriptorHandleForHeapStart());
			break;
		}
	}
}

void D3D12CommandList::bindGlobalSampler(const core::ShaderID& shaderID, const std::string& bindName, const core::SamplerState sampler)
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
			// �q�[�v�w��
			m_pCmdList->SetDescriptorHeaps(1, m_pDevice->m_pSamplerHeap.GetAddressOf());
			// Table
			m_pCmdList->SetGraphicsRootDescriptorTable(
				itr->second.rootIndex,
				samplerHandle);
			break;
		}
	}
}

//----- �`�施��

void D3D12CommandList::render(const core::RenderBufferID& renderBufferID, std::uint32_t instanceCount)
{
	// �f�[�^�̎擾
	auto* renderBuffer = static_cast<D3D12RenderBuffer*>(m_pDevice->getRenderBuffer(renderBufferID));

	// �|���S���̕`��
	if (renderBuffer->m_indexData.count > 0)
	{
		m_pCmdList->DrawIndexedInstanced(renderBuffer->m_indexData.count, instanceCount, 0, 0, 0);
	}
	else
	{
		m_pCmdList->DrawInstanced(renderBuffer->m_vertexData.count, instanceCount, 0, 0);
	}
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
		// �q�[�v�w��
		ID3D12DescriptorHeap* pHeap[] = { pD3DTex->m_pTexHeap.Get() };
		m_pCmdList->SetDescriptorHeaps(_countof(pHeap), pHeap);
		// �e�[�u���w��
		m_pCmdList->SetGraphicsRootDescriptorTable(rootIndex,
			pD3DTex->m_pTexHeap->GetGPUDescriptorHandleForHeapStart());
	}
	else
	{
		// �f�t�H���g�e�N�X�`���w��

	}
}

void D3D12CommandList::setSamplerResource(std::uint32_t slot, core::SamplerState state)
{

}

