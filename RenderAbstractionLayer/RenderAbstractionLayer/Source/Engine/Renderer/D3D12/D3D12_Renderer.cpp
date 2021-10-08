/*****************************************************************//**
 * \file   D3D12_Renderer.h
 * \brief  DirectX12レンダラー
 *
 * \author USAMI KOSHI
 * \date   2021/10/08
 *********************************************************************/

#include "D3D12_Renderer.h"
using namespace d3d12;


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

/// @brief コンストラクタ
D3D12Renderer::D3D12Renderer()
{

}

/// @brief 初期化処理
/// @param hWnd ウィンドウハンドル
/// @param width ウィンドウの幅
/// @param height ウィンドウの高さ
/// @return 初期化: 成功 true | 失敗 false
HRESULT D3D12Renderer::initialize(HWND hWnd, UINT width, UINT height)
{
	HRESULT hr = S_OK;

	//----- アダプタ・デバイス -----

	// デバイス生成
	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m_pD3DDevice.ReleaseAndGetAddressOf()));
	CHECK_FAILED(hr);


	//----- コマンドキュー・コマンドアロケーター・コマンドリスト -----
	D3D12_COMMAND_LIST_TYPE cmdType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	UINT nodeMask = 0;

	// コマンドキューの生成
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = cmdType;									// コマンドバッファの種類
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// コマンドキューの優先度
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;				// ??? GPUTimeOut
		desc.NodeMask = nodeMask;								// GPU識別マスク(1つは0)

		hr = m_pD3DDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pCmdQueue.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
	}

	// コマンドアロケーターの生成
	{
		hr = m_pD3DDevice->CreateCommandAllocator(cmdType, 
			IID_PPV_ARGS(m_pCmdAllocator.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
	}

	// コマンドリストの生成
	{
		hr = m_pD3DDevice->CreateCommandList(nodeMask, cmdType, m_pCmdAllocator.Get(),
			nullptr, IID_PPV_ARGS(m_pCmdList.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
		m_pCmdList->Close();
	}


	//----- ファクトリー・スワップチェイン・レンダーターゲット -----

	// ファクトリーの生成
	{
		hr = CreateDXGIFactory1(IID_PPV_ARGS(m_pDXGIFactory.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
	}

	// スワップチェインの生成
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = false;								// フルスクリーン指定
		desc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };	
		desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;			// バッファ識別
		desc.BufferCount = BACK_BUFFER_COUNT;						// バックバッファ数
		desc.Scaling = DXGI_SCALING_STRETCH;				// バックバッファのスケーリング指定
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// スワップ時のバッファの扱い
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;		// アルファモード??
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;// 動作オプション (フルスクリーン切り替え可)

		IDXGISwapChain1* pTempSwapChain1;
		hr = m_pDXGIFactory->CreateSwapChainForHwnd(
			m_pCmdQueue.Get(),
			hWnd,
			&desc,
			nullptr,
			nullptr,
			&pTempSwapChain1
		);
		CHECK_FAILED(hr);
		
		// スワップチェイン４に変換
		hr = pTempSwapChain1->QueryInterface(IID_PPV_ARGS(m_pSwapChain.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
		
		// フレームバッファ番号取得

	}

	// レンダーターゲットヒープの生成
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		// ピープタイプ
		desc.NumDescriptors = BACK_BUFFER_COUNT;		// バッファ数
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// シェーダーリソースか
		desc.NodeMask = nodeMask;

		hr = m_pD3DDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pBackBufferHeap.ReleaseAndGetAddressOf()));
	}

	// レンダーターゲットビューの生成
	{
		// 先頭アドレスとオフセットサイズを取得
		auto handle = m_pBackBufferHeap->GetCPUDescriptorHandleForHeapStart();
		m_nBackBufferSize = m_pD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// MSAA off

		// バックバッファからレンダーターゲットを生成
		for (UINT i = 0; i < BACK_BUFFER_COUNT; ++i)
		{
			// バックバッファ取得
			hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pBackBuffer[i].ReleaseAndGetAddressOf()));
			CHECK_FAILED(hr);

			// レンダーターゲットビューの生成
			m_pD3DDevice->CreateRenderTargetView(m_pBackBuffer[i].Get(), &desc, handle);
			// ハンドルのポインタを進める
			handle.ptr += m_nBackBufferSize;
		}
	}

	return hr;
}

/// @brief 終了処理
void D3D12Renderer::finalize()
{

}

/// @brief 画面クリア
void D3D12Renderer::clear()
{
	// コマンドアロケーターとコマンドリストをリセット
	m_pCmdAllocator->Reset();
	m_pCmdList->Reset(m_pCmdAllocator.Get(), nullptr);

	// レンダーターゲットハンドルの取得
	auto handlRTV = m_pBackBufferHeap->GetCPUDescriptorHandleForHeapStart();
	UINT backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	handlRTV.ptr += backBufferIndex * m_nBackBufferSize;

	// レンダーターゲットのセット
	m_pCmdList->OMSetRenderTargets(1, &handlRTV, FALSE, nullptr);

	// レンダーターゲットのクリア
	static float a = 0;
	a += 0.1f;
	FLOAT clearColor[] = { sinf(a), 0.58f, 0.92f, 1.0f };
	m_pCmdList->ClearRenderTargetView(handlRTV, clearColor, 0, nullptr);

}

/// @brief 画面更新
void D3D12Renderer::present()
{
	// コマンドの記録終了
	m_pCmdList->Close();

	// コマンドの実行
	ID3D12CommandList* ppCmdList[] = { m_pCmdList.Get() };
	m_pCmdQueue->ExecuteCommandLists(_countof(ppCmdList), ppCmdList);

	// 表示
	m_pSwapChain->Present(1, 0);

	// コマンドの完了を待機
	
}
