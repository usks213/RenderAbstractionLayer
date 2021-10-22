/*****************************************************************//**
 * \file   Core_CommandList.h
 * \brief  レンダーコマンドリストクラス
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
	/// @class CoreCommandList
	/// @brief レンダーコマンドリスト
	class CoreCommandList
	{
		friend class CoreRenderer;
	public:

		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		explicit CoreCommandList()
		{
		}

		/// @brief デストラクタ
		virtual ~CoreCommandList() noexcept = default;


		//----- リソース指定命令 -----

		/// @brief マテリアル指定
		/// @param materialID マテリアルID
		virtual void setMaterial(const MaterialID& materialID) = 0;

		/// @brief レンダーバッファ指定
		/// @param renderBufferID レンダーバッファID
		virtual void setRenderBuffer(const RenderBufferID& renderBufferID) = 0;


		//----- バインド命令 -----

		virtual void bindBuffer(const std::string& bindName, const core::ShaderID& shaderID, const core::BufferID bufferID) = 0;

		virtual void bindTexture(const std::string& bindName, const core::ShaderID& shaderID, const core::TextureID textureID) = 0;

		virtual void bindSampler(const std::string& bindName, const core::ShaderID& shaderID, const core::SamplerState sampler) = 0;


		//----- 描画命令

		/// @brief 即時描画命令
		/// @param renderBufferID レンダーバッファID
		virtual void render(const RenderBufferID& renderBufferID, std::uint32_t instanceCount = 1) = 0;

	protected:
		//------------------------------------------------------------------------------
		// protected variables
		//------------------------------------------------------------------------------


	private:

	};
}

#endif // !_CORE_RENDER_CONTEXT_
