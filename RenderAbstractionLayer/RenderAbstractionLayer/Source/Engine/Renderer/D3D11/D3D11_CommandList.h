/*****************************************************************//**
 * \file   D3D11_CommandList.h
 * \brief  DirectX11レンダーコマンドリストクラス
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
	// 前定義
	class D3D11Renderer;
	class D3D11Device;

	/// @class D3D11CommandList
	/// @brief DirectX11レンダーコマンドリスト
	class D3D11CommandList final : public core::CoreCommandList
	{
		friend class D3D11Renderer;
	public:
		//------------------------------------------------------------------------------
		// public methods 
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		explicit D3D11CommandList();

		/// @brief デストラクタ(デフォルト)
		~D3D11CommandList() noexcept = default;

		/// @brief 初期化処理
		/// @param pRenderer D3D11レンダラーポインタ
		/// @param pDevice D3D11デバイスポインタ
		/// @return 初期化: 成功 true | 失敗 false
		HRESULT initialize(D3D11Renderer* pRenderer, D3D11Device* pDevice);


		//----- リソース指定命令 -----

		void setMaterial(const core::MaterialID& materialID) override;

		void setRenderBuffer(const core::RenderBufferID& renderBufferID)  override;

		//----- セット命令 -----

		void setBackBuffer() override;

		void setGraphicsPipelineState(const core::ShaderID& shaderID, const core::BlendState& bs,
			const core::RasterizeState& rs, const core::DepthStencilState& ds) override;

		void setRenderTarget(const core::RenderTargetID& rtID) override;
		void setRenderTarget(const std::uint32_t num, const core::RenderTargetID rtIDs[]) override;
		void setRenderTarget(const core::RenderTargetID& rtID, const core::DepthStencilID& dsID) override;
		void setRenderTarget(const std::uint32_t num, const core::RenderTargetID rtIDs[], const core::DepthStencilID& dsID) override;

		void setViewport(const Rect& rect) override;
		void setViewport(const Viewport& viewport) override;

		//----- ゲット命令 -----


		//----- バインド命令 -----

		void bindGlobalBuffer(const core::ShaderID& shaderID, const std::string& bindName, const core::BufferID& bufferID) override;

		void bindGlobalTexture(const core::ShaderID& shaderID, const std::string& bindName, const core::TextureID& textureID) override;

		void bindGlobalSampler(const core::ShaderID& shaderID, const std::string& bindName, const core::SamplerState& sampler) override;

		//----- 描画命令

		void render(const core::RenderBufferID& renderBufferID, std::uint32_t instanceCount = 1)  override;

		/// @brief 
		/// @param destID 対象のレンダーターゲット
		/// @param sourceID 
		/// @param matID 
		void blit(const core::RenderBufferID& destID, const core::TextureID& sourceID, const core::MaterialID& matID) override;


		//----- その他 -----

		void clearCommand() override;		///< コマンドのクリア

		void clearRederTarget(const core::RenderTargetID& rtID, const Color& color) override;

		void clearDepthStencil(const core::DepthStencilID& dsID, float depth, std::uint8_t stencil) override;

	private:
		//------------------------------------------------------------------------------
		// private methods 
		//------------------------------------------------------------------------------

		void setCBufferResource(std::uint32_t slot, const core::BufferID& bufferID, core::ShaderStage stage);

		void setTextureResource(std::uint32_t slot, const core::TextureID& textureID, core::ShaderStage stage);

		void setSamplerResource(std::uint32_t slot, core::SamplerState state, core::ShaderStage stage);

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		static constexpr std::uint32_t MAX_RENDER_TARGET = 8;

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		D3D11Renderer*					m_pRenderer;				///< レンダラー
		D3D11Device*						m_pDevice;				///< デバイス

		ID3D11DeviceContext1*				m_pD3DContext;			///< デバイスコマンドリスト
		ID3D11DeviceContext1*				m_pD3DDeffered;			///< 遅延コマンドリスト

		//----- シェーダーバッファ
		ComPtr<ID3D11Buffer>				m_systemBuffer;
		ComPtr<ID3D11Buffer>				m_transformBuffer;
		ComPtr<ID3D11Buffer>				m_animationBuffer;

		ComPtr<ID3D11Buffer>				m_pointLightBuffer;
		ComPtr<ID3D11Buffer>				m_spotLightBuffer;
		ComPtr<ID3D11ShaderResourceView>	m_pointLightSRV;
		ComPtr<ID3D11ShaderResourceView>	m_spotLightSRV;


		core::BlendState			m_curBlendState;			///< 指定中のブレンドステイト
		core::RasterizeState		m_curRasterizeState;		///< 指定中のラスタライザーステイト
		core::DepthStencilState	m_curDepthStencilState;	///< 指定中のデプスステンシルステイト
		core::PrimitiveTopology	m_curPrimitiveTopology;	///< 指定中のプリミティブトポロジー

		core::SamplerState		m_curSamplerState[static_cast<size_t>(core::ShaderStage::MAX)][core::SHADER::MAX_SLOT_COUNT];
		core::TextureID			m_curTexture[static_cast<size_t>(core::ShaderStage::MAX)][core::SHADER::MAX_SLOT_COUNT];

		core::ShaderID			m_curShader;				///< 指定中のシェーダー
		core::MaterialID			m_curMaterial;			///< 指定中のマテリアル
		core::RenderBufferID		m_curRenderBuffer;		///< 指定中のレンダーバッファ
		core::RenderTargetID		m_curRenderTargets[MAX_RENDER_TARGET];	///< 指定中のレンダーターゲット
		core::DepthStencilID		m_curDepthStencil;		///< 指定中のデプスステンシル
	};
}

#endif // !_D3D11_RENDER_CONTEXT_
