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
#include <Renderer/D3D11/D3D11_Shader.h>
#include <Renderer/D3D11/D3D11_Material.h>


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


		//----- リソース指定命令 -----

		void setMaterial(const core::MaterialID& materialID) override;

		void setRenderBuffer(const core::RenderBufferID& renderBufferID)  override;

		//----- 描画命令

		void render(const core::RenderBufferID& renderBufferID)  override;

	private:
		//------------------------------------------------------------------------------
		// private methods 
		//------------------------------------------------------------------------------

		void setMaterialResource(const D3D11Material& d3dMaterial, const D3D11Shader& d3dShader);

		void setTexture(std::uint32_t slot, const core::TextureID& textureID, core::ShaderStage stage)  override;

		void setSampler(std::uint32_t slot, core::SamplerState state, core::ShaderStage stage)  override;

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		D3D11Renderer*						m_pRenderer;			///< レンダラー
		D3D11RenderDevice*					m_pDevice;				///< デバイス

		ID3D11DeviceContext1*				m_pD3DContext;			///< デバイスコンテキスト
		ID3D11DeviceContext1*				m_pD3DDeffered;			///< 遅延コンテキスト

		//----- シェーダーバッファ
		ComPtr<ID3D11Buffer>				m_systemBuffer;
		ComPtr<ID3D11Buffer>				m_transformBuffer;
		ComPtr<ID3D11Buffer>				m_animationBuffer;

		ComPtr<ID3D11Buffer>				m_pointLightBuffer;
		ComPtr<ID3D11Buffer>				m_spotLightBuffer;
		ComPtr<ID3D11ShaderResourceView>	m_pointLightSRV;
		ComPtr<ID3D11ShaderResourceView>	m_spotLightSRV;


		BlendState			m_curBlendState;			///< 指定中のブレンドステイト
		RasterizeState		m_curRasterizeState;		///< 指定中のラスタライザーステイト
		DepthStencilState		m_curDepthStencilState;	///< 指定中のデプスステンシルステイト
		PrimitiveTopology		m_curPrimitiveTopology;	///< 指定中のプリミティブトポロジー

		SamplerState			m_curSamplerState[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_SLOT_COUNT];
		TextureID			m_curTexture[static_cast<size_t>(ShaderStage::MAX)][SHADER::MAX_SLOT_COUNT];

		ShaderID				m_curShader;				///< 指定中のシェーダー
		MaterialID			m_curMaterial;			///< 指定中のマテリアル
		RenderBufferID		m_curRenderBuffer;		///< 指定中のレンダーバッファ
		RenderTargetID		m_curRenderTarget;		///< 指定中のレンダーターゲット
		DepthStencilID		m_curDepthStencil;		///< 指定中のデプスステンシル
	};
}

#endif // !_D3D11_RENDER_CONTEXT_
