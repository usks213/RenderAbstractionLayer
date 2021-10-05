/*****************************************************************//**
 * \file   Core_DepthStencil.h
 * \brief  �f�v�X�X�e���V��
 * 
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/
#ifndef _CORE_DEPTH_STENCIL_
#define _CORE_DEPTH_STENCIL_

#include <cstdint>
#include <string>

namespace core
{
	/// @brief �[�x�X�e���V��ID
	enum class DepthStencilID : std::uint32_t {};
	/// @brief ���݂��Ȃ��[�x�X�e���V��ID
	constexpr DepthStencilID NONE_DEPTH_STENCIL_ID = std::numeric_limits<DepthStencilID>::max();

	/// @class DepthStencil
	/// @brief �[�x�X�e���V��
	class DepthStencil
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		/// @param id �[�x�X�e���V��ID
		/// @param name ���O
		explicit DepthStencil(const DepthStencilID& id, const std::string& name) :
			m_id(id), m_name(name)
		{
		}

		/// @brief �f�X�g���N�^
		~DepthStencil() = default;

	public:
		//------------------------------------------------------------------------------
		// public variables
		//------------------------------------------------------------------------------

		DepthStencilID	m_id;	///< �[�x�X�e���V��ID
		std::string		m_name;	///< ���O

	};
}

#endif // !_CORE_DEPTH_STENCIL_
