/*****************************************************************//**
 * \file   Core_DepthStencil.h
 * \brief  デプスステンシル
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
	/// @brief 深度ステンシルID
	enum class DepthStencilID : std::uint32_t {};
	/// @brief 存在しない深度ステンシルID
	constexpr DepthStencilID NONE_DEPTH_STENCIL_ID = std::numeric_limits<DepthStencilID>::max();

	/// @class DepthStencil
	/// @brief 深度ステンシル
	class DepthStencil
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		/// @param id 深度ステンシルID
		/// @param name 名前
		explicit DepthStencil(const DepthStencilID& id, const std::string& name) :
			m_id(id), m_name(name)
		{
		}

		/// @brief デストラクタ
		~DepthStencil() = default;

	public:
		//------------------------------------------------------------------------------
		// public variables
		//------------------------------------------------------------------------------

		DepthStencilID	m_id;	///< 深度ステンシルID
		std::string		m_name;	///< 名前

	};
}

#endif // !_CORE_DEPTH_STENCIL_
