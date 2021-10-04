/*****************************************************************//**
 * \file   D3D11_Renderer.cpp
 * \brief  DirectX11レンダラークラス
 * 
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#include "D3D11_Renderer.h"
 // ライブラリリンク
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "DXGI.lib")

using namespace d3d11;


/// @brief コンストラクタ
D3D11Renderer::D3D11Renderer()
{

}

 /// @brief 初期化処理
 /// @param hWnd ウィンドウハンドル
 /// @param width ウィンドウの幅
 /// @param height ウィンドウの高さ
 /// @return 初期化: 成功 true | 失敗 false
HRESULT D3D11Renderer::initialize(HWND hWnd, UINT width, UINT height)
{
	HRESULT hr = S_OK;

	// デバイスの初期化
	CHECK_FAILED(hr = m_device.initialize(hWnd, width, height));

	// コンテキストの初期化
	CHECK_FAILED(hr = m_context.initialize(this, &m_device));

	return hr;
}

/// @brief 終了処理
void D3D11Renderer::finalize()
{

}

/// @brief 画面クリア
void D3D11Renderer::clear()
{
	// イベント開始
	m_device.m_d3dAnnotation->BeginEvent(L"Clear");
	m_context.m_d3dContext->OMSetBlendState(nullptr, nullptr, std::numeric_limits<UINT>::max());

	// バッファのクリア
	float ClearColor[4] = { 0.2f, 0.22f, 0.22f, 1.0f };
	m_context.m_d3dContext->ClearRenderTargetView(m_device.m_backBufferRTV.Get(), ClearColor);
	m_context.m_d3dContext->OMSetRenderTargets(1, m_device.m_backBufferRTV.GetAddressOf(), 
		m_device.m_depthStencilView.Get());

	// ビューポート
	m_context.m_d3dContext->RSSetViewports(1, &m_device.m_viewport);

	// イベント終了
	m_device.m_d3dAnnotation->EndEvent();
}

/// @brief 画面更新
void D3D11Renderer::present()
{
	// バックバッファに戻す
	m_context.m_d3dContext->OMSetRenderTargets(1, m_device.m_backBufferRTV.GetAddressOf(),
		m_device.m_depthStencilView.Get());

	//m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);	// ティアリング許容描画
	//m_swapChain->Present(0, 0);							// 非垂直同期描画
	m_device.m_swapChain->Present(1, 0);					// 垂直同期描画
}
