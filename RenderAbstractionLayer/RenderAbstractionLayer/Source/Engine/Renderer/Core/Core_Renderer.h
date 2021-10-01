/*****************************************************************//**
 * \file   Core_Renderer.h
 * \brief  レンダラークラス
 * 
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/

#ifndef _CORE_RENDERER_
#define _CORE_RENDERER_

// 前宣言
class Engine;

/// @brief レンダラーのベースクラス
/// @class CoreRenderer
class CoreRenderer
{
public:
	//------------------------------------------------------------------------------
	// public methods
	//------------------------------------------------------------------------------

	/// @brief コンストラクタ
	CoreRenderer() = default;
	/// @brief デストラクタ
	virtual ~CoreRenderer() = default;

	/// @brief 終了処理
	virtual void finalize() = 0;

	/// @brief 画面クリア
	virtual void clear() = 0;
	/// @brief 画面更新
	virtual void present() = 0;

	/// @brief コピーコンストラクタ削除
	CoreRenderer(const CoreRenderer&) = delete;
	/// @brief ムーブコンストラクタ削除
	CoreRenderer(CoreRenderer&&) = delete;
	
protected:
	//------------------------------------------------------------------------------
	// protected variables
	//------------------------------------------------------------------------------

	Engine* m_pEngine;	///< エンジンのポインタ

	///< デバイスクラス
	///< コンテストクラス

};

#endif // !_CORE_RENDERER_
