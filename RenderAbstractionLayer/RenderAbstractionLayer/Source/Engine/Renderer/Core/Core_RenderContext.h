/*****************************************************************//**
 * \file   Core_RenderContext.h
 * \brief  レンダーコンテキストクラス
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
	/// @brief レンダーコンテキスト
	class CoreRenderContext
	{
		friend class CoreRenderer;
	public:

		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
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

		/// @brief デストラクタ
		virtual ~CoreRenderContext() noexcept = default;


		//----- リソース指定命令 -----

		/// @brief マテリアル指定
		/// @param materialID マテリアルID
		virtual void setMaterial(const MaterialID& materialID) = 0;

		/// @brief レンダーバッファ指定
		/// @param renderBufferID レンダーバッファID
		virtual void setRenderBuffer(const RenderBufferID& renderBufferID) = 0;

		/// @brief テクスチャ指定
		/// @param slot スロット
		/// @param textureID テクスチャID
		/// @param stage シェーダーステージ
		virtual void setTexture(std::uint32_t slot, const TextureID& textureID, ShaderStage stage) = 0;

		/// @brief サンプラー指定
		/// @param slot スロット
		/// @param state サンプラーステート
		/// @param stage シェーダーステージ
		virtual void setSampler(std::uint32_t slot, SamplerState state, ShaderStage stage) = 0;

		/// @brief プリミティブ指定
		/// @param topology プリミティブトポロジー
		virtual void setPrimitiveTopology(PrimitiveTopology topology) = 0;

		//----- バッファ指定命令 -----

		/// @brief システムバッファ送信
		/// @param systemBuffer システムバッファ
		virtual void sendSystemBuffer(const SHADER::SystemBuffer& systemBuffer) = 0;

		/// @brief トランスフォームバッファ送信
		/// @param mtxWorld ワールドマトリックス
		virtual void sendTransformBuffer(const Matrix& mtxWorld) = 0;

		/// @brief アニメーションバッファ送信
		/// @param mtxBones ボーンマトリックスリスト
		virtual void sendAnimationBuffer(std::vector<Matrix>& mtxBones) = 0;

		/// @brief ライトバッファ送信
		/// @param pointLights ポイントライトリスト
		/// @param spotLights スポットライトリスト
		virtual void sendLightBuffer(std::vector<CorePointLight>& pointLights, 
			std::vector<CoreSpotLight>& spotLights) = 0;

		//----- 描画命令

		/// @brief 即時描画命令
		/// @param renderBufferID レンダーバッファID
		virtual void render(const RenderBufferID& renderBufferID) = 0;

	protected:
		//------------------------------------------------------------------------------
		// protected variables
		//------------------------------------------------------------------------------

		BlendState			m_curBlendState;		///< 指定中のブレンドステイト
		RasterizeState		m_curRasterizeState;	///< 指定中のラスタライザーステイト
		DepthStencilState	m_curDepthStencilState;	///< 指定中のデプスステンシルステイト
		PrimitiveTopology	m_curPrimitiveTopology;	///< 指定中のプリミティブトポロジー

		SamplerState		m_curSamplerState[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_SS_SLOT_COUNT];
		TextureID			m_curTexture[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_SRV_SLOT_COUNT];

		ShaderID			m_curShader;			///< 指定中のシェーダー
		MaterialID			m_curMaterial;			///< 指定中のマテリアル
		RenderBufferID		m_curRenderBuffer;		///< 指定中のレンダーバッファ
		RenderTargetID		m_curRenderTarget;		///< 指定中のレンダーターゲット
		DepthStencilID		m_curDepthStencil;		///< 指定中のデプスステンシル

	private:

	};
}

#endif // !_CORE_RENDER_CONTEXT_
