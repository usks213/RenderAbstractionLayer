/*****************************************************************//**
 * \file   Core_RenderContext.h
 * \brief  �����_�[�R���e�L�X�g�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#ifndef _CORE_RENDER_CONTEXT_
#define _CORE_RENDER_CONTEXT_

#include "Core_Buffer.h"
#include "Core_Material.h"
#include "Core_RenderBuffer.h"
#include "Core_RenderTarget.h"
#include "Core_DepthStencil.h"
#include "Core_ShaderResource.h"


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
		explicit CoreRenderContext()
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


		//----- �o�C���h���� -----

		virtual void setBuffer(const std::string& bindName, const core::ShaderID& shaderID, const core::BufferID bufferID) = 0;

		virtual void setTexture(const std::string& bindName, const core::ShaderID& shaderID, const core::TextureID textureID) = 0;

		virtual void setSampler(const std::string& bindName, const core::ShaderID& shaderID, const core::SamplerState sampler) = 0;


		//----- �`�施��

		/// @brief �����`�施��
		/// @param renderBufferID �����_�[�o�b�t�@ID
		virtual void render(const RenderBufferID& renderBufferID, std::uint32_t instanceCount = 1) = 0;

	protected:
		//------------------------------------------------------------------------------
		// protected variables
		//------------------------------------------------------------------------------


	private:

	};
}

#endif // !_CORE_RENDER_CONTEXT_
