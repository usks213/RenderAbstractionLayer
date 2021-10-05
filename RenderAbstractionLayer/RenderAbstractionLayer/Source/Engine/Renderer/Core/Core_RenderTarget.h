/*****************************************************************//**
 * \file   Core_RenderTarget.h
 * \brief  �����_�[�^�[�Q�b�g
 * 
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/
#ifndef _CORE_RENDER_TARGET_
#define _CORE_RENDER_TARGET_

#include <cstdint>
#include <string>

namespace core
{
	/// @brief �����_�[�^�[�Q�b�gID
	enum class RenderTargetID : std::uint32_t {};
	/// @brief ���݂��Ȃ������_�[�^�[�Q�b�gID
	constexpr RenderTargetID NONE_RENDER_TARGET_ID = std::numeric_limits<RenderTargetID>::max();

	/// @class CoreRenderTarget
	/// @brief �����_�[�^�[�Q�b�g
	class CoreRenderTarget
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		/// @param id �����_�[�^�[�Q�b�gID
		/// @param name ���O
		explicit CoreRenderTarget(const RenderTargetID& id, const std::string& name) :
			m_id(id), m_name(name)
		{
		}
		/// @brief �f�X�g���N�^
		~CoreRenderTarget() = default;

		/// @brief �����_�[�^�[�Q�b�g�e�N�X�`���̎擾
		/// @return �e�N�X�`��ID
		//virtual const TextureID getRTTexture() const = 0;

	public:
		//------------------------------------------------------------------------------
		// public variables
		//------------------------------------------------------------------------------

		/// @brief �����_�[�^�[�Q�b�gID
		RenderTargetID	m_id;
		/// @brief ���O
		std::string		m_name;
	};
}

#endif // !_CORE_RENDER_TARGET_
