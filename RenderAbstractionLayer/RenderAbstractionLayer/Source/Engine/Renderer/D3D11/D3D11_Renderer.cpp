/*****************************************************************//**
 * \file   D3D11_Renderer.cpp
 * \brief  DirectX11レンダラークラス
 * 
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#include "D3D11_Renderer.h"
#include <vector>

using namespace d3d11;


/// @brief コンストラクタ
D3D11Renderer::D3D11Renderer() :
	m_device(),
	m_context(),
	m_d3dDevice(),
	m_d3dAnnotation(),
	m_d3dContext(),
	m_d3dDefferedContext(),
	m_dxgiFactory()
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

	// デバイスとコンテキストの作成
	CHECK_FAILED(hr = createDiveceAndContext(hWnd));

	// デバイスの初期化
	CHECK_FAILED(hr = m_device.initialize(m_d3dDevice.Get(), 
		m_dxgiFactory.Get(), hWnd, width, height));

	// コンテキストの初期化
	m_context.m_pD3DContext = m_d3dContext.Get();
	m_context.m_pD3DDeffered = m_d3dDefferedContext.Get();
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
	m_d3dAnnotation->BeginEvent(L"Clear");
	m_d3dContext->OMSetBlendState(nullptr, nullptr, std::numeric_limits<UINT>::max());

	// バッファのクリア
	float ClearColor[4] = { 0.2f, 0.22f, 0.22f, 1.0f };
	m_d3dContext->ClearRenderTargetView(m_device.m_backBufferRTV.Get(), ClearColor);
	m_d3dContext->ClearDepthStencilView(m_device.m_depthStencilView.Get(), 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_d3dContext->OMSetRenderTargets(1, m_device.m_backBufferRTV.GetAddressOf(), 
		m_device.m_depthStencilView.Get());

	// ビューポート
	m_d3dContext->RSSetViewports(1, &m_device.m_viewport);

	// イベント終了
	m_d3dAnnotation->EndEvent();
}

/// @brief 画面更新
void D3D11Renderer::present()
{
	// バックバッファに戻す
	m_d3dContext->OMSetRenderTargets(1, m_device.m_backBufferRTV.GetAddressOf(),
		m_device.m_depthStencilView.Get());

	//m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);	// ティアリング許容描画
	//m_swapChain->Present(0, 0);							// 非垂直同期描画
	m_device.m_swapChain->Present(1, 0);					// 垂直同期描画
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------

/// @brief デバイスとコンテキストの生成
/// @return HRESULT
HRESULT D3D11Renderer::createDiveceAndContext(HWND hWnd)
{
	HRESULT hr = S_OK;

	D3D_FEATURE_LEVEL featureLevels[] = {
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1,
	};

	// ファクトリーの取得
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)(&m_dxgiFactory));
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateDXGIFactory1", L"Err", MB_ICONSTOP);
		return hr;
	}
	// アダプターの取得
	IDXGIAdapter* iAdapter = NULL;
	std::vector<IDXGIAdapter*> aiAdapter;
	aiAdapter.push_back(iAdapter);
	for (unsigned int index = 0;; index++)
	{
		HRESULT	ret = m_dxgiFactory->EnumAdapters(index, &iAdapter);

		if (ret == DXGI_ERROR_NOT_FOUND)
		{
			break;
		}
		// ～ アダプタの選択
		aiAdapter.push_back(iAdapter);
	}
	aiAdapter.pop_back();
	if (aiAdapter.size() <= 0)
	{
		aiAdapter.push_back(NULL);
	}


	//--- デバイスの生成 ---
	ComPtr<ID3D11Device>		device;
	ComPtr<ID3D11DeviceContext> context;
	ComPtr<ID3D11DeviceContext> defferedContext;

	// デバイス。コンテキストの生成
	hr = D3D11CreateDevice(
		aiAdapter.back(),
		aiAdapter.back() ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		featureLevels,
		7,
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		NULL,
		context.GetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"D3D11CreateDevice", L"Err", MB_ICONSTOP);
		return hr;
	}

	// 遅延コンテキスト作成
	hr = device->CreateDeferredContext(0, defferedContext.GetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateDeferredContext", L"Err", MB_ICONSTOP);
		return hr;
	}

	// 格納
	hr = device.As(&m_d3dDevice);
	hr = context.As(&m_d3dContext);
	hr = context.As(&m_d3dAnnotation);
	hr = defferedContext.As(&m_d3dDefferedContext);

	return S_OK;
}
