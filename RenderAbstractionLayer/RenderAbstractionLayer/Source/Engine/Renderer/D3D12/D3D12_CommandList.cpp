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
#include "D3D12_RenderTarget.h"
#include "D3D12_DepthStencil.h"

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

	CHECK_FAILED(m_pCmdAllocator->Reset());
	CHECK_FAILED(m_pCmdList->Reset(m_pCmdAllocator.Get(), nullptr));

	return S_OK;
}

//----- ���\�[�X�w�薽�� -----

void D3D12CommandList::setMaterial(const core::MaterialID& materialID)
{
	// �}�e���A���̎擾
	auto* d3dMat = static_cast<D3D12Material*>(m_pDevice->getMaterial(materialID));
	if (d3dMat == nullptr) return;

	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̎w��
	setGraphicsPipelineState(d3dMat->m_shaderID, 
		d3dMat->m_blendState, d3dMat->m_rasterizeState, d3dMat->m_depthStencilState);

	// �X�e�[�W����
	UINT rootIndex = 0;
	for (auto stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
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
			setSamplerResource(rootIndex++, sam.second.state);
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


//----- �Z�b�g���� -----

void D3D12CommandList::setBackBuffer()
{
	// �����_�[�^�[�Q�b�g�n���h���̎擾
	auto handlRTV = m_pRenderer->m_pBackBufferHeap->GetCPUDescriptorHandleForHeapStart();
	UINT backBufferIndex = m_pRenderer->m_pSwapChain->GetCurrentBackBufferIndex();
	handlRTV.ptr += backBufferIndex * m_pRenderer->m_nBackBufferSize;
	auto handlDSV = m_pRenderer->m_pDepthStencilHeap->GetCPUDescriptorHandleForHeapStart();

	// �����_�[�^�[�Q�b�g�̃o���A�w��
	D3D12_RESOURCE_BARRIER barrierDesc = {};
	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;					// �o���A���(�J��)
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;						// �o���A�����p
	barrierDesc.Transition.pResource = m_pRenderer->m_pBackBuffer[backBufferIndex].Get();	// ���\�[�X�|�C���^
	barrierDesc.Transition.Subresource = 										// �T�u���\�[�X�̐�
		D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;								// ���\�[�X���̂��ׂẴT�u���\�[�X�𓯎��Ɉڍs
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;			// �J�ڑO�̃��\�[�X���
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;		// �J�ڌ�̃��\�[�X���
	m_pCmdList->ResourceBarrier(1, &barrierDesc);

	// �����_�[�^�[�Q�b�g�̃Z�b�g
	m_pCmdList->OMSetRenderTargets(1, &handlRTV, FALSE, &handlDSV);

	//// �����_�[�^�[�Q�b�g�̃o���A�w��
	//barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;					// �o���A���(�J��)
	//barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;						// �o���A�����p
	//barrierDesc.Transition.pResource = m_pRenderer->m_pBackBuffer[backBufferIndex].Get();	// ���\�[�X�|�C���^
	//barrierDesc.Transition.Subresource = 										// �T�u���\�[�X�̐�
	//	D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;								// ���\�[�X���̂��ׂẴT�u���\�[�X�𓯎��Ɉڍs
	//barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// �J�ڑO�̃��\�[�X���
	//barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// �J�ڌ�̃��\�[�X���
	//m_pCmdList->ResourceBarrier(1, &barrierDesc);
}

void D3D12CommandList::setGraphicsPipelineState(const ShaderID& shaderID, const BlendState& bs,
	const RasterizeState& rs, const DepthStencilState& ds)
{
	// �V�F�[�_�[�̎擾
	auto* d3d12Shader = static_cast<D3D12Shader*>(m_pDevice->getShader(shaderID));
	if (d3d12Shader == nullptr) return;

	// �p�C�v���C���X�e�[�g�̎擾�E�w��
	auto pipelineState = m_pDevice->createGraphicsPipelineState(*d3d12Shader, bs, rs, ds);
	m_pCmdList->SetPipelineState(pipelineState);

	// ���[�g�V�O�l�`���[�̃Z�b�g
	m_pCmdList->SetGraphicsRootSignature(d3d12Shader->m_pRootSignature.Get());
}

void D3D12CommandList::setRenderTarget(const RenderTargetID& rtID)
{
	RenderTargetID rtIDs[] = { rtID };
	setRenderTarget(1, rtIDs);
}

void D3D12CommandList::setRenderTarget(const std::uint32_t num, const RenderTargetID rtIDs[])
{
	// ���S����
	if (num >= MAX_RENDER_TARGET || num <= 0) return;

	// �����_�[�^�[�Q�b�g�擾
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

	// ���݂̃f�v�X�X�e���V���擾
	auto* pDS = static_cast<D3D12DepthStencil*>(m_pDevice->getDepthStencil(m_curDepthStencilID));
	if (pDS)
	{
		// �����_�[�^�[�Q�b�g�w��
		auto handle = pDS->m_pHeapDSV->GetCPUDescriptorHandleForHeapStart();
		m_pCmdList->OMSetRenderTargets(num, rtvs.data(), FALSE, &handle);
	}
	else
	{
		// �����_�[�^�[�Q�b�g�w��
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
	// ���S����
	if (num >= MAX_RENDER_TARGET || num <= 0) return;

	// �����_�[�^�[�Q�b�g�擾
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

	// ���݂̃f�v�X�X�e���V���擾
	auto* pDS = static_cast<D3D12DepthStencil*>(m_pDevice->getDepthStencil(dsID));
	m_curDepthStencilID = dsID;
	if (pDS)
	{
		// �����_�[�^�[�Q�b�g�w��
		auto handle = pDS->m_pHeapDSV->GetCPUDescriptorHandleForHeapStart();
		m_pCmdList->OMSetRenderTargets(num, rtvs.data(), FALSE, &handle);
	}
	else
	{
		// �����_�[�^�[�Q�b�g�w��
		m_pCmdList->OMSetRenderTargets(num, rtvs.data(), FALSE, nullptr);
	}
}

void D3D12CommandList::setViewport(const Rect& rect)
{
	D3D12_VIEWPORT d3d11View = {
		rect.left, rect.top, rect.right, rect.bottom, 0.0f, 1.0f
	};
	m_pCmdList->RSSetViewports(1, &d3d11View);
}

void D3D12CommandList::setViewport(const Viewport& viewport)
{
	D3D12_VIEWPORT d3d11View = { viewport.left, viewport.top,
		viewport.right, viewport.bottom, viewport.minDepth, viewport.maxDepth
	};
	m_pCmdList->RSSetViewports(1, &d3d11View);
}

//----- �Q�b�g���� -----

//----- �o�C���h���� -----

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
			// GPU�X�V
			if (pBuffer->m_isUpdate)
			{
				void* pData = nullptr;
				pBuffer->m_pBuffer->Map(0, nullptr, &pData);
				std::memcpy(pData, pBuffer->m_aData.data(), pBuffer->m_aData.size());
				pBuffer->m_pBuffer->Unmap(0, nullptr);
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

/// @brief 
/// @param destID �Ώۂ̃����_�[�^�[�Q�b�g
/// @param sourceID 
/// @param matID 
void D3D12CommandList::blit(const RenderBufferID& destID, const TextureID& sourceID, const MaterialID& matID)
{

}

//----- ���̑� -----

void D3D12CommandList::clearCommand()
{
	// �R�}���h�̃N���A
}

void D3D12CommandList::clearRederTarget(const RenderTargetID& rtID, const Color& color)
{
	// �����_�[�^�[�Q�b�g�擾
	auto* pRT = static_cast<D3D12RenderTarget*>(m_pDevice->getRenderTarget(rtID));
	if (pRT == nullptr) return;

	FLOAT ColorRGBA[4];
	std::memcpy(ColorRGBA, &color, sizeof(Color));

	// �N���A�R�}���h
	m_pCmdList->ClearRenderTargetView(pRT->m_pHeapRTV->GetCPUDescriptorHandleForHeapStart(), 
		ColorRGBA, 0, nullptr);
}

void D3D12CommandList::clearDepthStencil(const DepthStencilID& dsID, float depth, std::uint8_t stencil)
{
	// ���݂̃f�v�X�X�e���V���擾
	auto* pDS = static_cast<D3D12DepthStencil*>(m_pDevice->getDepthStencil(dsID));
	if (pDS == nullptr) return;

	// �N���A�R�}���h
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
		// �q�[�v�w��
		ID3D12DescriptorHeap* pHeap[] = { pD3DTex->m_pTexHeap.Get() };
		m_pCmdList->SetDescriptorHeaps(_countof(pHeap), pHeap);
		// �e�[�u���w��
		m_pCmdList->SetGraphicsRootDescriptorTable(rootIndex,
			pD3DTex->m_pTexHeap->GetGPUDescriptorHandleForHeapStart());
	}
	else
	{
		// ��̃e�N�X�`��

	}
}

void D3D12CommandList::setSamplerResource(std::uint32_t rootIndex, core::SamplerState state)
{
	// �q�[�v�w��
	ID3D12DescriptorHeap* pHeap[] = { m_pDevice->m_pSamplerHeap.Get() };
	m_pCmdList->SetDescriptorHeaps(_countof(pHeap), pHeap);
	// �e�[�u���w��
	m_pCmdList->SetGraphicsRootDescriptorTable(rootIndex,
		m_pDevice->m_dynamicSamplers[static_cast<std::size_t>(state)]);
}
