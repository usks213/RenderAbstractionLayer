/*****************************************************************//**
 * \file   Engine.h
 * \brief  �G���W���N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/

#ifndef _ENGINE_
#define _ENGINE_


#include <memory>
#include <chrono>

#include "Utility/Singleton.h"
#include "Platform/Core/Core_Window.h"

namespace core
{

	/// @class Engine
	/// @brief �G���W��
	class Engine final : public Singleton<Engine>
	{
	private:
		friend Singleton<Engine>;

	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief ����������
		/// @return ������: ���� true | ���s false
		[[nodiscard]] bool initialize();

		/// @brief �X�V�֐�
		void tick();

		/// @brief �I������
		void finalize();

		/// @brief �E�B���h�E�̐���
		/// @tparam T �E�B���h�E�^�C�v
		/// @tparam CoreWindow�p���^�̂�
		/// @return �E�B���h�E�̃|�C���^
		template<class T, typename = std::enable_if_t<std::is_base_of_v<CoreWindow, T>>>
		T* createWindow(std::string windowName, int windowWidth, int windowHeight) {
			m_pWindow = std::make_unique<T>(windowName, windowWidth, windowHeight);
			m_pWindow->setEngine(this);
			return static_cast<T*>(m_pWindow.get());
		}

		///// @brief �����_���[�}�l�[�W���[�̐���
		///// @tparam T �����_���[�^�C�v
		///// @tparam RendererManager�p���^�̂�
		///// @return �����_���[�̃|�C���^
		//template<class T, typename = std::enable_if_t<std::is_base_of_v<RendererManager, T>>>
		//T* createRenderer() {
		//	m_rendererManager = std::make_unique<T>();
		//	m_rendererManager->m_pEngine = this;
		//	return static_cast<T*>(m_rendererManager.get());
		//}

		/// @brief �E�B���h�E�̎擾
		/// @return �E�B���h�E�̃|�C���^
		[[nodiscard]] CoreWindow* getCoreWindow() { return m_pWindow.get(); }
		//[[nodiscard]] RendererManager* getRendererManager() { return m_rendererManager.get(); }

		/// @brief �E�B���h�E���擾
		/// @return �E�B���h�E��
		[[nodiscard]] std::string getWindowName() const noexcept { return m_pWindow->getWindowName(); }

		/// @brief �E�B���h�E�̕��擾
		/// @return �E�B���h�E�̕�(����)
		[[nodiscard]] int getWindowWidth() const noexcept { return m_pWindow->getWindowWidth(); }

		/// @brief �E�B���h�E�̍����擾
		/// @return �E�B���h�E�̍���(����)
		[[nodiscard]] int getWindowHeight() const noexcept { return m_pWindow->getWindowHeight(); }

		/// @brief ���݂̃t���[�����[�g�擾
		/// @return �t���[�����[�g(����)
		[[nodiscard]] std::uint32_t getCurrentFPS() { return m_nCurrentFPS; }

	private:
		//------------------------------------------------------------------------------
		// private methods
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		Engine();

		/// @brief �f�X�g���N�^
		~Engine() = default;

	private:
		//------------------------------------------------------------------------------
		// private variables
		//------------------------------------------------------------------------------

		std::unique_ptr<CoreWindow>				m_pWindow;				///< �E�B���h�E
		//std::unique_ptr<RendererManager>		_pRenderer;				///< �����_���[
		//--- �^�C�}�[ 
		std::uint32_t							m_nCurrentFPS;			///< ���݂�FPS
		std::uint64_t							m_nFrameCount;			///< �t���[�����[�g�J�E���^
		std::chrono::system_clock::time_point	m_FixedExecLastTime;	///< ���X�V�̑O���� 
		std::chrono::system_clock::time_point	m_ExecLastTime;			///< �ʏ�X�V�̑O����
		std::chrono::system_clock::time_point	m_FPSLastTime;			///< FPS�v���p�̑O����
		std::chrono::system_clock::time_point	m_CurrentTime;			///< ���݂̎���

		float									m_deltaTime;			///< �ʏ�X�V�̒P�ʎ���
		float									m_fixedDeltaTime;		///< ���X�V�̒P�ʎ���
		std::int32_t							m_nMaxFPS;				///< �ő�FPS
		std::int64_t							m_nFixedTime;			///< ���X�V�̎���
	};

}

#endif // !_ENGINE_
