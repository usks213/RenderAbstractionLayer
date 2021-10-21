/*****************************************************************//**
 * \file   D3D12_RenderContext.h
 * \brief  DiectX12�����_�[�R���e�L�X�g
 * 
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/

#ifndef _D3D12_RENDER_CONTEXT_
#define _D3D12_RENDER_CONTEXT_

#include <Renderer/Core/Core_RenderContext.h>
#include "D3D12_Shader.h"
#include "D3D12_Material.h"

#include <tuple>
#include <map>

namespace d3d12
{
	// �O��`
	class D3D12Renderer;
	class D3D12RenderDevice;

	/// @class D3D12RenderContext
	/// @brief DirectX11�����_�[�R���e�L�X�g
	class D3D12RenderContext final : public core::CoreRenderContext
	{
		friend class D3D12Renderer;
	public:
		//------------------------------------------------------------------------------
		// public methods 
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		explicit D3D12RenderContext();

		/// @brief �f�X�g���N�^(�f�t�H���g)
		~D3D12RenderContext() noexcept = default;

		/// @brief ����������
		/// @param pRenderer D3D12�����_���[�|�C���^
		/// @param pDevice D3D12�f�o�C�X�|�C���^
		/// @return ������: ���� true | ���s false
		HRESULT initialize(D3D12Renderer* pRenderer, D3D12RenderDevice* pDevice);


		//----- ���\�[�X�w�薽�� -----

		void setMaterial(const core::MaterialID& materialID) override;

		void setRenderBuffer(const core::RenderBufferID& renderBufferID)  override;

		//----- �o�C���h���� -----

		void setBuffer(std::string_view bindName, const core::ShaderID& shaderID, const core::BufferID bufferID) override;

		void setTexture(std::string_view bindName, const core::ShaderID& shaderID, const core::TextureID textureID) override;

		void setSampler(std::string_view bindName, const core::ShaderID& shaderID, const core::SamplerState sampler) override;

		//----- �`�施��

		void render(const core::RenderBufferID& renderBufferID)  override;

	private:
		//------------------------------------------------------------------------------
		// private methods 
		//------------------------------------------------------------------------------

		void setPipelineState(D3D12Shader& d3d12Shader, D3D12Material& d3d12Mat);

		void setCBufferResource(std::uint32_t rootIndex, const core::BufferID& bufferID);

		void setTextureResource(std::uint32_t rootIndex, const core::TextureID& textureID);

		void setSamplerResource(std::uint32_t rootIndex, core::SamplerState state);

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		D3D12Renderer*						m_pRenderer;			///< �����_���[
		D3D12RenderDevice*					m_pDevice;				///< �f�o�C�X

		// �R�}���h���X�g
		ID3D12GraphicsCommandList*			m_pCmdList;
		// �R�}���h�A���P�[�^�[

		// �O���t�B�N�X�p�C�v���C���X�e�[�g
		std::unordered_map<core::ShaderID, ComPtr<ID3D12PipelineState>>	m_pPipelineState;

		//----- �V�F�[�_�[�o�b�t�@
		//ComPtr<ID3D12Buffer>				m_systemBuffer;
		//ComPtr<ID3D12Buffer>				m_transformBuffer;
		//ComPtr<ID3D12Buffer>				m_animationBuffer;

		//ComPtr<ID3D12Buffer>				m_pointLightBuffer;
		//ComPtr<ID3D12Buffer>				m_spotLightBuffer;
		//ComPtr<ID3D12ShaderResourceView>	m_pointLightSRV;
		//ComPtr<ID3D12ShaderResourceView>	m_spotLightSRV;
	};
}

#endif // !_D3D12_RENDER_CONTEXT_
