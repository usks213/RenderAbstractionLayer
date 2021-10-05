/*****************************************************************//**
 * \file   Core_Buffer.h
 * \brief  バッファ
 * 
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/
#ifndef _CORE_BUFFER_
#define _CORE_BUFFER_

#include "Core_CommonState.h"
#include <numeric>
#include <string>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif


namespace core
{
	/// @brief バッファID
	enum class BufferID : std::uint32_t {};
	/// @brief 存在しないバッファID
	constexpr BufferID NONE_BUFFER_ID = std::numeric_limits<BufferID>::max();

	/// @struct BufferData
	/// @brief バッファ初期化データ
	struct BufferData
	{
		const void* pInitData = nullptr;
		std::size_t size = 0;
	};

	/// @struct BufferDesc
	/// @brief バッファDesc
	struct BufferDesc
	{
		std::string		name;
		std::uint32_t	count = 0;
		std::uint32_t	size = 0;
		Usage			usage = Usage::DEFAULT;
		BindFlags		bindFlags = BindFlags::UNKNOWN;
		CPUAccessFlags	cpuAccessFlags = CPUAccessFlags::NONE;
		MiscFlags		miscFlags = MiscFlags::UNKNONE;
	};

	/// @class CoreBuffer
	/// @brief バッファ
	class CoreBuffer
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		/// @param id バッファID
		/// @param desc バッファDesc
		CoreBuffer(const BufferID& id, const BufferDesc& desc) :
			m_id(id), m_desc(desc)
		{
		}

		/// @brief デストラクタ
		~CoreBuffer() = default;

	public:
		//------------------------------------------------------------------------------
		// public variables
		//------------------------------------------------------------------------------

		BufferID	m_id;	///< バッファID
		BufferDesc	m_desc;	///< バッファDesc

	};
}

#endif // !_CORE_BUFFER_
