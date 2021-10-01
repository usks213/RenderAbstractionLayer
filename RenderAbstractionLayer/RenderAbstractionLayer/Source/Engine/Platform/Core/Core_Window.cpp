/*****************************************************************//**
 * \file   Core_Window.cpp
 * \brief  ウィンドウクラス
 * 
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/

#include "Core_Window.h"

using namespace core;

/// @brief コンストラクタ
/// @param windowName[in] ウィンドウ名
/// @param windowWidth[in] ウィンドウの幅
/// @param windowHeight[in] ウィンドウの高さ
CoreWindow::CoreWindow(std::string windowName, int windowWidth, int windowHeight) : 
	m_pEngine(nullptr),
	m_windowName(windowName), 
	m_windowWidth(windowWidth), 
	m_windowHeight(windowHeight)
{
}

