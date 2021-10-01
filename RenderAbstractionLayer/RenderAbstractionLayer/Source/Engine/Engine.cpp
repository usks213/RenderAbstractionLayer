/*****************************************************************//**
 * \file   Engine.h
 * \brief  エンジンクラス
 *
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/

#include "Engine.h"
using namespace core;


/// @brief コンストラクタ
Engine::Engine()
	: m_pWindow(), //m_rendererManager(), m_worldManager(),
	m_nCurrentFPS(0), m_nFrameCount(0), m_deltaTime(0), m_fixedDeltaTime(0)
{
	m_nMaxFPS = 90;
	m_nFixedTime = 20; // ミリ秒
}

/// @brief 初期化処理
/// @param pWindow 
/// @param pRenderer 
/// @return 成功か
bool Engine::initialize()
{
	// フレームカウント初期化
	m_ExecLastTime = m_FPSLastTime = 
		m_CurrentTime = m_FixedExecLastTime =
		std::chrono::system_clock::now();
	m_nFrameCount = 0;

	return true;
}

/// @brief 更新
void Engine::tick()
{
	// タイマー更新
	m_CurrentTime = std::chrono::system_clock::now();

	// FPS測定	0.5秒ごとに実行
	std::int64_t fpsTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(m_CurrentTime - m_FPSLastTime).count();
	if (fpsTime >= 500)
	{
		m_nCurrentFPS = static_cast<std::uint32_t>(m_nFrameCount * 1000 / (fpsTime));
		m_FPSLastTime = m_CurrentTime;
		m_nFrameCount = 0;
	}

	//--- 固定フレームレート更新 ---
	std::int64_t fixedTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(m_CurrentTime - m_FixedExecLastTime).count();
	if (fixedTime >= m_nFixedTime)
	{
		m_FixedExecLastTime = m_CurrentTime;

		// FixedUpdate
	}

	//--- 可変フレームレート更新 ---
	std::int64_t deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(m_CurrentTime - m_ExecLastTime).count();
	if (deltaTime >= (1000 / m_nMaxFPS))
	{
		m_ExecLastTime = m_CurrentTime;

		// レンダラーのクリア
		//m_rendererManager->clear();

		// Update

		// Render
		

		// 画面更新
		//m_rendererManager->present();

		// フレームカウンタ更新
		m_nFrameCount++;
	}
}

/// @brief 終了処理
void Engine::finalize()
{
	// レンダラーマネージャーのの終了処理
	//m_rendererManager->finalize();
	// ウィンドウマネージャーの終了処理
	m_pWindow->finalize();
}
