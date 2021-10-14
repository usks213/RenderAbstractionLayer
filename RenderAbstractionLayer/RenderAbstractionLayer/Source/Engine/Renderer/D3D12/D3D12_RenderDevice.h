/*****************************************************************//**
 * \file   D3D12_RenderDevice.h
 * \brief  DirectX12レンダーデバイス
 * 
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/

#ifndef _D3D12_RENDER_DEVICE_
#define _D3D12_RENDER_DEVICE_

#include <Renderer/Core/Core_RenderDevice.h>
#include "D3D12_CommonState.h"

namespace d3d12
{
	// 前定義
	class D3D12Renderer;

	/// @class D3D12RenderDevice
	/// @brief D3D12レンダーデバイス
	class D3D12RenderDevice final : public core::CoreRenderDevice
	{
		friend class D3D12Renderer;
		friend class D3D12RenderContext;
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		explicit D3D12RenderDevice();

		/// @brief デストラクタ
		~D3D12RenderDevice() noexcept = default;

		/// @brief 初期化処理
		/// @param pDevice D3D12デバイスポインタ
		/// @param hWnd ウィンドウハンドル
		/// @param width ウィンドウの幅
		/// @param height ウィンドウの高さ
		/// @return 初期化: 成功 true | 失敗 false
		HRESULT initialize(ID3D12Device* pDevice, IDXGIFactory2* pFactory2, 
			HWND hWnd, UINT width, UINT height);

		//----- リソース生成 -----

		core::BufferID			createBuffer(core::BufferDesc& desc, core::BufferData* pData = nullptr) override;
		core::DepthStencilID	createDepthStencil(std::string name) override;
		core::MaterialID		createMaterial(std::string name, core::ShaderID& shaderID) override;
		core::MeshID			createMesh(std::string name) override;
		core::RenderBufferID	createRenderBuffer(core::ShaderID& shaderID, core::MeshID& meshID) override;
		core::RenderTargetID	createRenderTarget(std::string name) override;
		core::ShaderID			createShader(core::ShaderDesc& desc) override;
		core::TextureID			createTexture(std::string filePath) override;
		core::TextureID			createTexture(core::TextureDesc& desc, core::TextureData* pData = nullptr) override;

	private:
		//------------------------------------------------------------------------------
		// private methods
		//------------------------------------------------------------------------------

		/// @brief 共通ステートの生成
		/// @return HRESULT
		HRESULT createCommonState();

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		ID3D12Device*						m_pD3DDevice;			///< デバイスポインタ

		//ComPtr<IDXGISwapChain1>				m_swapChain;			///< スワップチェーン

		//ComPtr<ID3D12Texture2D>				m_backBufferRT;			///< バックバッファ
		//ComPtr<ID3D12RenderTargetView>		m_backBufferRTV;		///< バックバッファビュー
		DXGI_FORMAT							m_backBufferFormat;		///< バッファバッファフォーマット

		//ComPtr<ID3D12Texture2D>				m_depthStencilTexture;	///< Zバッファ
		//ComPtr<ID3D12DepthStencilView>		m_depthStencilView;		///< Zバッファビュー
		DXGI_FORMAT							m_depthStencilFormat;	///< Zバッファフォーマット

		HWND								m_hWnd;					///< ウィンドウハンドル
		D3D12_VIEWPORT						m_viewport;				///< ビューポート

		UINT								m_backBufferCount;		///< バックバッファ数
		UINT								m_nOutputWidth;			///< 出力サイズ(幅)
		UINT								m_nOutputHeight;		///< 出力サイズ(高さ)

		D3D12_RASTERIZER_DESC              	m_rasterizeStates[(size_t)core::RasterizeState::MAX];		///< ラスタライザステート
		D3D12_STATIC_SAMPLER_DESC 			m_samplerStates[(size_t)core::SamplerState::MAX];			///< サンプラステート
		D3D12_BLEND_DESC                   	m_blendStates[(size_t)core::BlendState::MAX];				///< ブレンドステート
		D3D12_DEPTH_STENCIL_DESC           	m_depthStencilStates[(size_t)core::DepthStencilState::MAX];	///< 深度ステンシルステート

	};
}

#endif // !_D3D12_RENDER_DEVICE_
