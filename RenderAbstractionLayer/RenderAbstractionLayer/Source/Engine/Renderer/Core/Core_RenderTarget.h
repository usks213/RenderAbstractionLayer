/*****************************************************************//**
 * \file   Core_RenderTarget.h
 * \brief  レンダーターゲット
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
	/// @brief レンダーターゲットID
	enum class RenderTargetID : std::uint32_t {};
	/// @brief 存在しないレンダーターゲットID
	constexpr RenderTargetID NONE_RENDER_TARGET_ID = std::numeric_limits<RenderTargetID>::max();

	/// @class CoreRenderTarget
	/// @brief レンダーターゲット
	class CoreRenderTarget
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		/// @param id レンダーターゲットID
		/// @param name 名前
		explicit CoreRenderTarget(const RenderTargetID& id, const std::string& name) :
			m_id(id), m_name(name)
		{
		}
		/// @brief デストラクタ
		~CoreRenderTarget() = default;

		/// @brief レンダーターゲットテクスチャの取得
		/// @return テクスチャID
		//virtual const TextureID getRTTexture() const = 0;

	public:
		//------------------------------------------------------------------------------
		// public variables
		//------------------------------------------------------------------------------

		/// @brief レンダーターゲットID
		RenderTargetID	m_id;
		/// @brief 名前
		std::string		m_name;
	};
}

#endif // !_CORE_RENDER_TARGET_
