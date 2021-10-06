/*****************************************************************//**
 * \file   Core_Texture.h
 * \brief  テクスチャ
 * 
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/
#ifndef _CORE_TEXTURE_
#define _CORE_TEXTURE_

#include <cstdint>
#include <string>
#include "Core_CommonState.h"

namespace core
{
	/// @brief テクスチャID
	enum class TextureID : std::uint32_t {};
	/// @brief 存在しないテクスチャID
	constexpr TextureID NONE_TEXTURE_ID = std::numeric_limits<TextureID>::max();

	/// @struct TextureData
	/// @brief テクスチャ初期化データ
	struct TextureData
	{
		const void* pInitData = nullptr;
		std::size_t size = 0;
	};

	/// @struct TextureDesc
	/// @brief テクスチャDesc
	struct TextureDesc
	{
		std::string			name;										///< テクスチャ名orファイルパス
		std::uint32_t		width			= 0;						///< 横幅(px)
		std::uint32_t		height			= 0;						///< 縦幅(px)
		std::uint32_t		byteSize		= 0;						///< フォーマット区切りのバイトサイズ
		std::uint32_t		mipLevels		= 0;						///< ミップマップレベル(0:自動生成)
		std::uint32_t		arraySize		= 1;						///< 配列サイズ
		TextureFormat		format			= TextureFormat::UNKNOWN;	///< フォーマット
		Usage				usage			= Usage::DEFAULT;			///< リソース使用識別
		BindFlags			bindFlags		= BindFlags::UNKNOWN;		///< バインドフラグ
		CPUAccessFlags		cpuAccessFlags	= CPUAccessFlags::NONE;		///< CPUアクセスフラグ
		MiscFlags			miscFlags		= MiscFlags::UNKNONE;		///< バッファ指定フラグ
	};

	/// @class CoreTexture
	/// @brief テクスチャ
	class CoreTexture
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ(ファイル読み込み)
		/// @param id テクスチャID
		/// @param filepath ファイルパス
		explicit CoreTexture(const TextureID& id, const std::string& filepath) :
			m_id(id), m_desc()
		{}

		/// @brief コンストラクタ(Descから生成)
		/// @param id テクスチャID
		/// @param desc テクスチャDesc
		explicit CoreTexture(const TextureID& id, TextureDesc& desc, 
			const TextureData* pData = nullptr) :
			m_id(id), m_desc(desc)
		{}

	public:
		//------------------------------------------------------------------------------
		// public variables
		//------------------------------------------------------------------------------

		TextureID		m_id;		///< テクスチャID
		TextureDesc		m_desc;		///< テクスチャDesc
	};
}

#endif // !_CORE_TEXTURE_
