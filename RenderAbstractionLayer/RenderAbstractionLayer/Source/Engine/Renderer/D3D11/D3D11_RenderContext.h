/*****************************************************************//**
 * \file   D3D11_RenderContext.h
 * \brief  DirectX11�����_�[�R���e�L�X�g�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#ifndef _D3D11_RENDER_CONTEXT_
#define _D3D11_RENDER_CONTEXT_

#include <Renderer/Core/Core_RenderContext.h>
#include <Renderer/D3D11/D3D11_Shader.h>
#include <Renderer/D3D11/D3D11_Material.h>


namespace d3d11
{
	// �O��`
	class D3D11Renderer;
	class D3D11RenderDevice;

	/// @class D3D11RenderContext
	/// @brief DirectX11�����_�[�R���e�L�X�g
	class D3D11RenderContext final : public core::CoreRenderContext
	{
		friend class D3D11Renderer;
	public:
		//------------------------------------------------------------------------------
		// public methods 
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		explicit D3D11RenderContext();

		/// @brief �f�X�g���N�^(�f�t�H���g)
		~D3D11RenderContext() noexcept = default;

		/// @brief ����������
		/// @param pRenderer D3D11�����_���[�|�C���^
		/// @param pDevice D3D11�f�o�C�X�|�C���^
		/// @return ������: ���� true | ���s false
		HRESULT initialize(D3D11Renderer* pRenderer, D3D11RenderDevice* pDevice);


		//----- ���\�[�X�w�薽�� -----

		void setMaterial(const core::MaterialID& materialID) override;

		void setRenderBuffer(const core::RenderBufferID& renderBufferID)  override;

		void setTexture(std::uint32_t slot, const core::TextureID& textureID, core::ShaderStage stage)  override;

		void setSampler(std::uint32_t slot, core::SamplerState state, core::ShaderStage stage)  override;

		void setPrimitiveTopology(core::PrimitiveTopology topology)  override;

		//----- �o�b�t�@�w�薽�� -----

		void sendSystemBuffer(const core::SHADER::SystemBuffer& systemBuffer)  override;

		void sendTransformBuffer(const Matrix& mtxWorld)  override;

		void sendAnimationBuffer(std::vector<Matrix>& mtxBones)  override;

		void sendLightBuffer(std::vector<core::CorePointLight>& pointLights,
			std::vector<core::CoreSpotLight>& spotLights)  override;

		//----- �`�施��

		void render(const core::RenderBufferID& renderBufferID)  override;

	private:
		//------------------------------------------------------------------------------
		// private methods 
		//------------------------------------------------------------------------------

		void setMaterialResource(const D3D11Material& d3dMaterial, const D3D11Shader& d3dShader);

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		D3D11Renderer*						m_pRenderer;			///< �����_���[
		D3D11RenderDevice*					m_pDevice;				///< �f�o�C�X

		ID3D11DeviceContext1*				m_pD3DContext;			///< �f�o�C�X�R���e�L�X�g
		ID3D11DeviceContext1*				m_pD3DDeffered;			///< �x���R���e�L�X�g

		//----- �V�F�[�_�[�o�b�t�@
		ComPtr<ID3D11Buffer>				m_systemBuffer;
		ComPtr<ID3D11Buffer>				m_transformBuffer;
		ComPtr<ID3D11Buffer>				m_animationBuffer;

		ComPtr<ID3D11Buffer>				m_pointLightBuffer;
		ComPtr<ID3D11Buffer>				m_spotLightBuffer;
		ComPtr<ID3D11ShaderResourceView>	m_pointLightSRV;
		ComPtr<ID3D11ShaderResourceView>	m_spotLightSRV;
	};
}

#endif // !_D3D11_RENDER_CONTEXT_
