/*****************************************************************//**
 * \file   D3D11_RenderContext.h
 * \brief  DirectX11レンダーコンテキストクラス
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
	// 前定義
	class D3D11Renderer;
	class D3D11RenderDevice;

	/// @class D3D11RenderContext
	/// @brief DirectX11レンダーコンテキスト
	class D3D11RenderContext final : public core::CoreRenderContext
	{
		friend class D3D11Renderer;
	public:
		//------------------------------------------------------------------------------
		// public methods 
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		explicit D3D11RenderContext();

		/// @brief デストラクタ(デフォルト)
		~D3D11RenderContext() noexcept = default;

		/// @brief 初期化処理
		/// @param pRenderer D3D11レンダラーポインタ
		/// @param pDevice D3D11デバイスポインタ
		/// @return 初期化: 成功 true | 失敗 false
		HRESULT initialize(D3D11Renderer* pRenderer, D3D11RenderDevice* pDevice);

	protected:

	private:

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		D3D11Renderer*						m_pRenderer;			///< レンダラー
		D3D11RenderDevice*					m_pDevice;				///< デバイス

		ID3D11DeviceContext1*				m_pD3DContext;			///< デバイスコンテキスト
		ID3D11DeviceContext1*				m_pD3DDeffered;			///< 遅延コンテキスト

		core::BlendState					m_curBlendState;		///< 指定中のブレンドステイト
		core::RasterizeState				m_curRasterizeState;	///< 指定中のラスタライザーステイト
		core::DepthStencilState				m_curDepthStencilState;	///< 指定中のデプスステンシルステイト
		core::PrimitiveTopology				m_curPrimitiveTopology;	///< 指定中のプリミティブトポロジー

		/*D3D11Shader*		m_curD3DShader;
		MaterialID			m_curMaterial;
		SamplerState		m_curSamplerState[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_SS_SLOT_COUNT];
		TextureID			m_curTexture[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_SRV_SLOT_COUNT];*/
	};
}

#endif // !_D3D11_RENDER_CONTEXT_
