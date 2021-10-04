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
#include <Renderer/D3D11/D3D11_CommonState.h>

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

	protected:

	private:

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		D3D11Renderer*						m_pRenderer;			///< �����_���[
		D3D11RenderDevice*					m_pDevice;				///< �f�o�C�X

		ID3D11DeviceContext1*				m_pD3DContext;			///< �f�o�C�X�R���e�L�X�g
		ID3D11DeviceContext1*				m_pD3DDeffered;			///< �x���R���e�L�X�g

		core::BlendState					m_curBlendState;		///< �w�蒆�̃u�����h�X�e�C�g
		core::RasterizeState				m_curRasterizeState;	///< �w�蒆�̃��X�^���C�U�[�X�e�C�g
		core::DepthStencilState				m_curDepthStencilState;	///< �w�蒆�̃f�v�X�X�e���V���X�e�C�g
		core::PrimitiveTopology				m_curPrimitiveTopology;	///< �w�蒆�̃v���~�e�B�u�g�|���W�[

		/*D3D11Shader*		m_curD3DShader;
		MaterialID			m_curMaterial;
		SamplerState		m_curSamplerState[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_SS_SLOT_COUNT];
		TextureID			m_curTexture[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_SRV_SLOT_COUNT];*/
	};
}

#endif // !_D3D11_RENDER_CONTEXT_
