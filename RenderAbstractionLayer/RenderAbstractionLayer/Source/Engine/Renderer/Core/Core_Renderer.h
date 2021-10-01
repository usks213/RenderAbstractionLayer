/*****************************************************************//**
 * \file   Core_Renderer.h
 * \brief  �����_���[�N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/

#ifndef _CORE_RENDERER_
#define _CORE_RENDERER_

// �O�錾
class Engine;

/// @brief �����_���[�̃x�[�X�N���X
/// @class CoreRenderer
class CoreRenderer
{
public:
	//------------------------------------------------------------------------------
	// public methods
	//------------------------------------------------------------------------------

	/// @brief �R���X�g���N�^
	CoreRenderer() = default;
	/// @brief �f�X�g���N�^
	virtual ~CoreRenderer() = default;

	/// @brief �I������
	virtual void finalize() = 0;

	/// @brief ��ʃN���A
	virtual void clear() = 0;
	/// @brief ��ʍX�V
	virtual void present() = 0;

	/// @brief �R�s�[�R���X�g���N�^�폜
	CoreRenderer(const CoreRenderer&) = delete;
	/// @brief ���[�u�R���X�g���N�^�폜
	CoreRenderer(CoreRenderer&&) = delete;
	
protected:
	//------------------------------------------------------------------------------
	// protected variables
	//------------------------------------------------------------------------------

	Engine* m_pEngine;	///< �G���W���̃|�C���^

	///< �f�o�C�X�N���X
	///< �R���e�X�g�N���X

};

#endif // !_CORE_RENDERER_
