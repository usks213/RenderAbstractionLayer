/*****************************************************************//**
 * \file   Core_RenderContext.h
 * \brief  �����_�[�R���e�L�X�g�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#ifndef _CORE_RENDER_CONTEXT_
#define _CORE_RENDER_CONTEXT_

#include "Core_ShaderResource.h"
#include "Core_Material.h"
#include "Core_RenderBuffer.h"
#include "Core_RenderTarget.h"
#include "Core_DepthStencil.h"


namespace core
{
	/// @class CoreRenderContext
	/// @brief �����_�[�R���e�L�X�g
	class CoreRenderContext
	{
		friend class CoreRenderer;
	public:

		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		explicit CoreRenderContext() :
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

		/// @brief �f�X�g���N�^
		virtual ~CoreRenderContext() noexcept = default;


		//----- ���\�[�X�w�薽�� -----

		/// @brief �}�e���A���w��
		/// @param materialID �}�e���A��ID
		virtual void setMaterial(const MaterialID& materialID) = 0;

		/// @brief �����_�[�o�b�t�@�w��
		/// @param renderBufferID �����_�[�o�b�t�@ID
		virtual void setRenderBuffer(const RenderBufferID& renderBufferID) = 0;

		/// @brief �e�N�X�`���w��
		/// @param slot �X���b�g
		/// @param textureID �e�N�X�`��ID
		/// @param stage �V�F�[�_�[�X�e�[�W
		virtual void setTexture(std::uint32_t slot, const TextureID& textureID, ShaderStage stage) = 0;

		/// @brief �T���v���[�w��
		/// @param slot �X���b�g
		/// @param state �T���v���[�X�e�[�g
		/// @param stage �V�F�[�_�[�X�e�[�W
		virtual void setSampler(std::uint32_t slot, SamplerState state, ShaderStage stage) = 0;

		/// @brief �v���~�e�B�u�w��
		/// @param topology �v���~�e�B�u�g�|���W�[
		virtual void setPrimitiveTopology(PrimitiveTopology topology) = 0;

		//----- �o�b�t�@�w�薽�� -----

		/// @brief �V�X�e���o�b�t�@���M
		/// @param systemBuffer �V�X�e���o�b�t�@
		virtual void sendSystemBuffer(const SHADER::SystemBuffer& systemBuffer) = 0;

		/// @brief �g�����X�t�H�[���o�b�t�@���M
		/// @param mtxWorld ���[���h�}�g���b�N�X
		virtual void sendTransformBuffer(const Matrix& mtxWorld) = 0;

		/// @brief �A�j���[�V�����o�b�t�@���M
		/// @param mtxBones �{�[���}�g���b�N�X���X�g
		virtual void sendAnimationBuffer(std::vector<Matrix>& mtxBones) = 0;

		/// @brief ���C�g�o�b�t�@���M
		/// @param pointLights �|�C���g���C�g���X�g
		/// @param spotLights �X�|�b�g���C�g���X�g
		virtual void sendLightBuffer(std::vector<CorePointLight>& pointLights, 
			std::vector<CoreSpotLight>& spotLights) = 0;

		//----- �`�施��

		/// @brief �����`�施��
		/// @param renderBufferID �����_�[�o�b�t�@ID
		virtual void render(const RenderBufferID& renderBufferID) = 0;

	protected:
		//------------------------------------------------------------------------------
		// protected variables
		//------------------------------------------------------------------------------

		BlendState			m_curBlendState;		///< �w�蒆�̃u�����h�X�e�C�g
		RasterizeState		m_curRasterizeState;	///< �w�蒆�̃��X�^���C�U�[�X�e�C�g
		DepthStencilState	m_curDepthStencilState;	///< �w�蒆�̃f�v�X�X�e���V���X�e�C�g
		PrimitiveTopology	m_curPrimitiveTopology;	///< �w�蒆�̃v���~�e�B�u�g�|���W�[

		SamplerState		m_curSamplerState[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_SS_SLOT_COUNT];
		TextureID			m_curTexture[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_SRV_SLOT_COUNT];

		ShaderID			m_curShader;			///< �w�蒆�̃V�F�[�_�[
		MaterialID			m_curMaterial;			///< �w�蒆�̃}�e���A��
		RenderBufferID		m_curRenderBuffer;		///< �w�蒆�̃����_�[�o�b�t�@
		RenderTargetID		m_curRenderTarget;		///< �w�蒆�̃����_�[�^�[�Q�b�g
		DepthStencilID		m_curDepthStencil;		///< �w�蒆�̃f�v�X�X�e���V��

	private:

	};
}

#endif // !_CORE_RENDER_CONTEXT_
