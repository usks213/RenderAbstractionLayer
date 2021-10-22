/*****************************************************************//**
 * \file   D3D11_CommandList.h
 * \brief  DirectX11�����_�[�R�}���h���X�g�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#ifndef _D3D11_RENDER_CONTEXT_
#define _D3D11_RENDER_CONTEXT_

#include <Renderer/Core/Core_CommandList.h>
#include <Renderer/D3D11/D3D11_Shader.h>
#include <Renderer/D3D11/D3D11_Material.h>


namespace d3d11
{
	// �O��`
	class D3D11Renderer;
	class D3D11Device;

	/// @class D3D11CommandList
	/// @brief DirectX11�����_�[�R�}���h���X�g
	class D3D11CommandList final : public core::CoreCommandList
	{
		friend class D3D11Renderer;
	public:
		//------------------------------------------------------------------------------
		// public methods 
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		explicit D3D11CommandList();

		/// @brief �f�X�g���N�^(�f�t�H���g)
		~D3D11CommandList() noexcept = default;

		/// @brief ����������
		/// @param pRenderer D3D11�����_���[�|�C���^
		/// @param pDevice D3D11�f�o�C�X�|�C���^
		/// @return ������: ���� true | ���s false
		HRESULT initialize(D3D11Renderer* pRenderer, D3D11Device* pDevice);


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

		void setMaterialResource(const D3D11Material& d3dMaterial, const D3D11Shader& d3dShader);

		void setTextureResource(std::uint32_t slot, const core::TextureID& textureID, core::ShaderStage stage);

		void setSamplerResource(std::uint32_t slot, core::SamplerState state, core::ShaderStage stage);

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		D3D11Renderer*						m_pRenderer;			///< �����_���[
		D3D11Device*					m_pDevice;				///< �f�o�C�X

		ID3D11DeviceContext1*				m_pD3DContext;			///< �f�o�C�X�R�}���h���X�g
		ID3D11DeviceContext1*				m_pD3DDeffered;			///< �x���R�}���h���X�g

		//----- �V�F�[�_�[�o�b�t�@
		ComPtr<ID3D11Buffer>				m_systemBuffer;
		ComPtr<ID3D11Buffer>				m_transformBuffer;
		ComPtr<ID3D11Buffer>				m_animationBuffer;

		ComPtr<ID3D11Buffer>				m_pointLightBuffer;
		ComPtr<ID3D11Buffer>				m_spotLightBuffer;
		ComPtr<ID3D11ShaderResourceView>	m_pointLightSRV;
		ComPtr<ID3D11ShaderResourceView>	m_spotLightSRV;


		core::BlendState			m_curBlendState;			///< �w�蒆�̃u�����h�X�e�C�g
		core::RasterizeState		m_curRasterizeState;		///< �w�蒆�̃��X�^���C�U�[�X�e�C�g
		core::DepthStencilState	m_curDepthStencilState;	///< �w�蒆�̃f�v�X�X�e���V���X�e�C�g
		core::PrimitiveTopology	m_curPrimitiveTopology;	///< �w�蒆�̃v���~�e�B�u�g�|���W�[

		core::SamplerState		m_curSamplerState[static_cast<size_t>(core::ShaderStage::MAX)][core::SHADER::MAX_SLOT_COUNT];
		core::TextureID			m_curTexture[static_cast<size_t>(core::ShaderStage::MAX)][core::SHADER::MAX_SLOT_COUNT];

		core::ShaderID				m_curShader;				///< �w�蒆�̃V�F�[�_�[
		core::MaterialID			m_curMaterial;			///< �w�蒆�̃}�e���A��
		core::RenderBufferID		m_curRenderBuffer;		///< �w�蒆�̃����_�[�o�b�t�@
		core::RenderTargetID		m_curRenderTarget;		///< �w�蒆�̃����_�[�^�[�Q�b�g
		core::DepthStencilID		m_curDepthStencil;		///< �w�蒆�̃f�v�X�X�e���V��
	};
}

#endif // !_D3D11_RENDER_CONTEXT_
