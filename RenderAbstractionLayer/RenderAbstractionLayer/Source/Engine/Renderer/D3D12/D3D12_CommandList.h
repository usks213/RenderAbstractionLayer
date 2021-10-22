/*****************************************************************//**
 * \file   D3D12_CommandList.h
 * \brief  DiectX12�����_�[�R�}���h���X�g
 * 
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/

#ifndef _D3D12_RENDER_CONTEXT_
#define _D3D12_RENDER_CONTEXT_

#include <Renderer/Core/Core_CommandList.h>
#include "D3D12_Shader.h"
#include "D3D12_Material.h"

#include <tuple>
#include <map>

namespace d3d12
{
	// �O��`
	class D3D12Renderer;
	class D3D12Device;

	/// @class D3D12CommandList
	/// @brief DirectX11�����_�[�R�}���h���X�g
	class D3D12CommandList final : public core::CoreCommandList
	{
		friend class D3D12Renderer;
	public:
		//------------------------------------------------------------------------------
		// public methods 
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		explicit D3D12CommandList();

		/// @brief �f�X�g���N�^(�f�t�H���g)
		~D3D12CommandList() noexcept = default;

		/// @brief ����������
		/// @param pRenderer D3D12�����_���[�|�C���^
		/// @param pDevice D3D12�f�o�C�X�|�C���^
		/// @return ������: ���� true | ���s false
		HRESULT initialize(D3D12Renderer* pRenderer, D3D12Device* pDevice);


		//----- ���\�[�X�w�薽�� -----

		void setMaterial(const core::MaterialID& materialID) override;

		void setRenderBuffer(const core::RenderBufferID& renderBufferID)  override;

		//----- �o�C���h���� -----

		void bindBuffer(const std::string& bindName, const core::ShaderID& shaderID, const core::BufferID bufferID) override;

		void bindTexture(const std::string& bindName, const core::ShaderID& shaderID, const core::TextureID textureID) override;

		void bindSampler(const std::string& bindName, const core::ShaderID& shaderID, const core::SamplerState sampler) override;

		//----- �`�施��

		void render(const core::RenderBufferID& renderBufferID, std::uint32_t instanceCount = 1)  override;

	private:
		//------------------------------------------------------------------------------
		// private methods 
		//------------------------------------------------------------------------------

		void setCBufferResource(std::uint32_t rootIndex, const core::BufferID& bufferID);

		void setTextureResource(std::uint32_t rootIndex, const core::TextureID& textureID);

		void setSamplerResource(std::uint32_t rootIndex, core::SamplerState state);

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		D3D12Renderer*					m_pRenderer;			///< �����_���[
		D3D12Device*						m_pDevice;			///< �f�o�C�X

		ComPtr<ID3D12CommandAllocator>		m_pCmdAllocator;		///< �R�}���h�A���P�[�^�[
		ComPtr<ID3D12GraphicsCommandList>	m_pCmdList;			///< �R�}���h���X�g

	};
}

#endif // !_D3D12_RENDER_CONTEXT_
