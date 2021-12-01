/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/


 //===== インクルード =====
#include <windows.h>

#include <Engine/Core_Engine.h>
#include <Engine/Platform/Win/Win_Window.h>
#include <Engine/Renderer/D3D11/D3D11_Renderer.h>
#include <Engine/Renderer/D3D12/D3D12_Renderer.h>

#include "TestScene.h"

//-------- ライブラリのリンク
#pragma comment(lib, "imm32")


//===== 定数定義 =====


//===== プロトタイプ宣言 =====
// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ウィンドウ初期化
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);


/// @brief エントリポイント
/// @param hInstance		インスタンスハンドル
/// @param hPrevInstance	前回のインスタンスハンドル
/// @param lpCmdLine		コマンドライン
/// @param nCmdShow			ウィンドウ状態
/// @return 終了コード
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 未使用宣言
	UNREFERENCED_PARAMETER(lpCmdLine);		// 未使用宣言

	// エンジン取得
	auto& engine = core::CoreEngine::get();
	// ウィンドウの生成
	win::WinWindow* pWin = engine.createWindow<win::WinWindow>("CoreEngine", 1280, 720);
	// レンダラーの生成
	d3d12::D3D12Renderer* pRenderer = engine.createRenderer<d3d12::D3D12Renderer>();

	// ウィンドウの初期化
	pWin->initialize(hInstance, "RenderAppClass", nCmdShow, WndProc);
	// レンダラーの初期化
	pRenderer->initialize(pWin->getWindowHandle(), pWin->getWindowWidth(), pWin->getWindowHeight());
	// エンジンの初期化
	engine.initialize();

	// シーンマネージャーの読み込み
	engine.getSceneManager()->CreateScene<TestScene>();


	// メッセージループ
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			// エンジンの更新
			engine.tick();
			// FPS
			char fps[256];
			sprintf_s(fps, "FPS:%u", engine.getCurrentFPS());
			SetWindowText(pWin->getWindowHandle(), (fps));

			Sleep(0);
		}
	}


	// エンジンの終了処理
	engine.finalize();

	return (int)msg.wParam;
}



/// @brief プロシージャ
/// @param hWnd 
/// @param uMsg 
/// @param wParam 
/// @param lParam 
/// @return 
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
	{//----- ウィンドウが生成された
		return OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	}
	case WM_DESTROY:				//----- ウィンドウ破棄指示がきた
		PostQuitMessage(0);				// システムにスレッドの終了を要求
		break;
	case WM_KEYDOWN:				//----- キーボードが押された
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]キーが押された
			PostMessage(hWnd, WM_CLOSE, 0, 0);	// [x]が押されたように振舞う
			return 0;
		}
		break;
	case WM_MENUCHAR:
		return MNC_CLOSE << 16;			// [Alt]+[Enter]時のBEEPを抑止
	case WM_DPICHANGED:
	{
		return OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	}
	break;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// @brief WM_CREATEメッセージハンドラ
/// @param hWnd 
/// @param lpcs 
/// @return 
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	// クライアント領域サイズをSCREEN_WIDTH×SCREEN_HEIGHTに再設定.
	RECT rcClnt;
	int SCREEN_WIDTH = core::CoreEngine::get().getWindowWidth();
	int SCREEN_HEIGHT = core::CoreEngine::get().getWindowHeight();

	GetClientRect(hWnd, &rcClnt);
	rcClnt.right -= rcClnt.left;
	rcClnt.bottom -= rcClnt.top;
	if (rcClnt.right != SCREEN_WIDTH || rcClnt.bottom != SCREEN_HEIGHT) {
		RECT rcWnd;
		GetWindowRect(hWnd, &rcWnd);
		SIZE sizeWnd;
		sizeWnd.cx = (rcWnd.right - rcWnd.left) - rcClnt.right + SCREEN_WIDTH;
		sizeWnd.cy = (rcWnd.bottom - rcWnd.top) - rcClnt.bottom + SCREEN_HEIGHT;

		SetWindowPos(hWnd, nullptr, 0, 0, sizeWnd.cx, sizeWnd.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	// IME無効化
	ImmAssociateContext(hWnd, nullptr);

	return 0;	// -1を返すとCreateWindow[Ex]が失敗する.
}
