/*****************************************************************//**
 * \file   Core_Window.h
 * \brief  ウィンドウクラス
 * 
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/

#ifndef _CORE_WINDOW_
#define _CORE_WINDOW_

#include <string>

namespace core
{
	// 前定義
	class Engine;

	/// @class CoreWindow
	/// @brief コアウィンドウクラス
	class CoreWindow
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		/// @param windowName[in] ウィンドウ名
		/// @param windowWidth[in] ウィンドウの幅
		/// @param windowHeight[in] ウィンドウの高さ
		explicit CoreWindow(std::string windowName, int windowWidth, int windowHeight);

		/// @brief デストラクタ
		virtual ~CoreWindow() noexcept = default;

		/// @brief 終了処理
		virtual void finalize() = 0;

		/// @brief ウィンドウ名取得
		/// @return ウィンドウ名
		[[nodiscard]] std::string getWindowName() const noexcept { return m_windowName; }

		/// @brief ウィンドウの幅取得
		/// @return ウィンドウの幅(整数)
		[[nodiscard]] int getWindowWidth() const noexcept { return m_windowWidth; }

		/// @brief ウィンドウの高さ取得
		/// @return ウィンドウの高さ(整数)
		[[nodiscard]] int getWindowHeight() const noexcept { return m_windowHeight; }

		/// @brief エンジンの取得
		/// @return エンジンのポインタ
		Engine* getEngine() const noexcept { return m_pEngine; }

		/// @brief エンジンの設定
		/// @param pEngine エンジンのポインタ
		void setEngine(Engine* pEngine) noexcept { m_pEngine = pEngine; }

		/// @brief コピーコンストラクタ削除
		CoreWindow(const CoreWindow&) = delete;
		/// @brief ムーブコンストラクタ削除
		CoreWindow(CoreWindow&&) = delete;

	protected:
		//------------------------------------------------------------------------------
		// protected variables
		//------------------------------------------------------------------------------

		Engine*			m_pEngine;			///< エンジンのポインタ
		std::string		m_windowName;		///< ウィンドウ名
		int				m_windowWidth;		///< ウィンドウの幅
		int				m_windowHeight;		///< ウィンドウの高さ
	};
}

#endif // !_CORE_WINDOW_
