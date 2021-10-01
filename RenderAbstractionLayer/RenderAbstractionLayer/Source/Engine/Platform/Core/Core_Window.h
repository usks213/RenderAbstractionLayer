/*****************************************************************//**
 * \file   Core_Window.h
 * \brief  �E�B���h�E�N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/

#ifndef _CORE_WINDOW_
#define _CORE_WINDOW_

#include <string>

namespace core
{
	// �O��`
	class Engine;

	/// @class CoreWindow
	/// @brief �R�A�E�B���h�E�N���X
	class CoreWindow
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		/// @param windowName[in] �E�B���h�E��
		/// @param windowWidth[in] �E�B���h�E�̕�
		/// @param windowHeight[in] �E�B���h�E�̍���
		explicit CoreWindow(std::string windowName, int windowWidth, int windowHeight);

		/// @brief �f�X�g���N�^
		virtual ~CoreWindow() noexcept = default;

		/// @brief �I������
		virtual void finalize() = 0;

		/// @brief �E�B���h�E���擾
		/// @return �E�B���h�E��
		[[nodiscard]] std::string getWindowName() const noexcept { return m_windowName; }

		/// @brief �E�B���h�E�̕��擾
		/// @return �E�B���h�E�̕�(����)
		[[nodiscard]] int getWindowWidth() const noexcept { return m_windowWidth; }

		/// @brief �E�B���h�E�̍����擾
		/// @return �E�B���h�E�̍���(����)
		[[nodiscard]] int getWindowHeight() const noexcept { return m_windowHeight; }

		/// @brief �G���W���̎擾
		/// @return �G���W���̃|�C���^
		Engine* getEngine() const noexcept { return m_pEngine; }

		/// @brief �G���W���̐ݒ�
		/// @param pEngine �G���W���̃|�C���^
		void setEngine(Engine* pEngine) noexcept { m_pEngine = pEngine; }

		/// @brief �R�s�[�R���X�g���N�^�폜
		CoreWindow(const CoreWindow&) = delete;
		/// @brief ���[�u�R���X�g���N�^�폜
		CoreWindow(CoreWindow&&) = delete;

	protected:
		//------------------------------------------------------------------------------
		// protected variables
		//------------------------------------------------------------------------------

		Engine*			m_pEngine;			///< �G���W���̃|�C���^
		std::string		m_windowName;		///< �E�B���h�E��
		int				m_windowWidth;		///< �E�B���h�E�̕�
		int				m_windowHeight;		///< �E�B���h�E�̍���
	};
}

#endif // !_CORE_WINDOW_
