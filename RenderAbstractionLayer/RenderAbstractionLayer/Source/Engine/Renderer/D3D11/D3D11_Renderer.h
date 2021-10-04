/*****************************************************************//**
 * \file   D3D11_Renderer.h
 * \brief  DirectX11レンダラークラス
 * 
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/

#ifndef _D3D11_RENDERER_
#define _D3D11_RENDERER_

#include <Renderer/Core/Core_Renderer.h>
#include <Renderer/D3D11/D3D11_RenderDevice.h>
#include <Renderer/D3D11/D3D11_RenderContext.h>

namespace d3d11
{
	/// @brief D3D11レンダラー
	/// @class D3D11Renderer
	class D3D11Renderer final : public core::CoreRenderer
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		D3D11Renderer();
		/// @brief デストラクタ
		~D3D11Renderer() noexcept = default;

		/// @brief 初期化処理
		/// @param hWnd ウィンドウハンドル
		/// @param width ウィンドウの幅
		/// @param height ウィンドウの高さ
		/// @return 初期化: 成功 true | 失敗 false
		HRESULT initialize(HWND hWnd, UINT width, UINT height);

		/// @brief 終了処理
		void finalize() override;

		/// @brief 画面クリア
		void clear() override;
		/// @brief 画面更新
		void present() override;

		/// @brief コピーコンストラクタ削除
		D3D11Renderer(const D3D11Renderer&) = delete;
		/// @brief ムーブコンストラクタ削除
		D3D11Renderer(D3D11Renderer&&) = delete;

	private:
		//------------------------------------------------------------------------------
		// private methods
		//------------------------------------------------------------------------------

		/// @brief デバイスとコンテキストの生成
		/// @return HRESULT
		HRESULT createDiveceAndContext(HWND hWnd);

	private:
		//------------------------------------------------------------------------------
		// private variables
		//------------------------------------------------------------------------------

		D3D11RenderDevice					m_device;				///< デバイスクラス
		D3D11RenderContext					m_context;				///< コンテストクラス

		ComPtr<ID3D11Device1>				m_d3dDevice;			///< デバイス
		ComPtr<ID3DUserDefinedAnnotation>	m_d3dAnnotation;		///< アノテーション
		ComPtr<ID3D11DeviceContext1>		m_d3dContext;			///< デバイスコンテキスト
		ComPtr<ID3D11DeviceContext1>		m_d3dDefferedContext;	///< 遅延コンテキスト

		ComPtr<IDXGIFactory2>				m_dxgiFactory;			///< ファクトリー

	};
}

#endif // !_D3D11_RENDERER_
