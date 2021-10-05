/*****************************************************************//**
 * \file   Core_Buffer.h
 * \brief  �o�b�t�@
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
	/// @brief �o�b�t�@ID
	enum class BufferID : std::uint32_t {};
	/// @brief ���݂��Ȃ��o�b�t�@ID
	constexpr BufferID NONE_BUFFER_ID = std::numeric_limits<BufferID>::max();

	/// @struct BufferData
	/// @brief �o�b�t�@�������f�[�^
	struct BufferData
	{
		const void* pInitData = nullptr;
		std::size_t size = 0;
	};

	/// @struct BufferDesc
	/// @brief �o�b�t�@Desc
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
	/// @brief �o�b�t�@
	class CoreBuffer
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		/// @param id �o�b�t�@ID
		/// @param desc �o�b�t�@Desc
		CoreBuffer(const BufferID& id, const BufferDesc& desc) :
			m_id(id), m_desc(desc)
		{
		}

		/// @brief �f�X�g���N�^
		~CoreBuffer() = default;

	public:
		//------------------------------------------------------------------------------
		// public variables
		//------------------------------------------------------------------------------

		BufferID	m_id;	///< �o�b�t�@ID
		BufferDesc	m_desc;	///< �o�b�t�@Desc

	};
}

#endif // !_CORE_BUFFER_
