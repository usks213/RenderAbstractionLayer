/*****************************************************************//**
 * \file   Core_RenderContext.h
 * \brief  �����_�[�R���e�L�X�g�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#ifndef _CORE_RENDER_CONTEXT_
#define _CORE_RENDER_CONTEXT_

namespace core
{
	/// @class CoreRenderContext
	/// @brief �����_�[�R���e�L�X�g
	class CoreRenderContext
	{
		friend class CoreRenderer;
	public:

		/// @brief �R���X�g���N�^
		explicit CoreRenderContext() = default;

		/// @brief �f�X�g���N�^
		virtual ~CoreRenderContext() noexcept = default;

	protected:

	private:

	};
}

#endif // !_CORE_RENDER_CONTEXT_
