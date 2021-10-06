/*****************************************************************//**
 * \file   Core_RenderContext.h
 * \brief  レンダーコンテキストクラス
 *
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#ifndef _CORE_RENDER_CONTEXT_
#define _CORE_RENDER_CONTEXT_

namespace core
{
	/// @class CoreRenderContext
	/// @brief レンダーコンテキスト
	class CoreRenderContext
	{
		friend class CoreRenderer;
	public:

		/// @brief コンストラクタ
		explicit CoreRenderContext() = default;

		/// @brief デストラクタ
		virtual ~CoreRenderContext() noexcept = default;

	protected:

	private:

	};
}

#endif // !_CORE_RENDER_CONTEXT_
