/*****************************************************************//**
 * \file   Util_TextureLoader.h
 * \brief  テクスチャローダー
 *
 * \author USAMI KOSHI
 * \date   2021/12/09
 *********************************************************************/
#ifndef _UTIL_TEXTURE_LOADER_
#define _UTIL_TEXTURE_LOADER_

#include <Renderer\Core\Core_Texture.h>
#include <memory>

namespace DirectX
{
	struct TexMetadata;
	class ScratchImage;
}

namespace util
{
	class TextureLoader
	{
	public:
		/// @brief コンストラクタ
		/// @param filename ファイル名
		TextureLoader(std::string filename);

		core::TextureDesc& GetDesc() { return m_desc; }
		DirectX::TexMetadata* GetMetaData() { return m_pMetaData.get(); }
		DirectX::ScratchImage* GetScratchImage() { return m_pScratchImage.get(); }

	private:
		core::TextureDesc						m_desc;
		std::unique_ptr<DirectX::TexMetadata>	m_pMetaData;
		std::unique_ptr<DirectX::ScratchImage>	m_pScratchImage;
	};

}

#endif // !_UTIL_TEXTURE_LOADER_

