/*****************************************************************//**
 * \file   Core_Window.cpp
 * \brief  �E�B���h�E�N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/

#include "Core_Window.h"

using namespace core;

/// @brief �R���X�g���N�^
/// @param windowName[in] �E�B���h�E��
/// @param windowWidth[in] �E�B���h�E�̕�
/// @param windowHeight[in] �E�B���h�E�̍���
CoreWindow::CoreWindow(std::string windowName, int windowWidth, int windowHeight) : 
	m_pEngine(nullptr),
	m_windowName(windowName), 
	m_windowWidth(windowWidth), 
	m_windowHeight(windowHeight)
{
}

