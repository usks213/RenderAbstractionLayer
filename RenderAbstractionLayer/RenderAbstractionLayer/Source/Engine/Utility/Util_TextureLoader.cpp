/*****************************************************************//**
 * \file   Util_TextureLoader.h
 * \brief  テクスチャローダー
 *
 * \author USAMI KOSHI
 * \date   2021/12/09
 *********************************************************************/
#include "Util_TextureLoader.h"
using namespace util;

#include <Library\DirectXTex\DirectXTex.h>
#pragma comment(lib, "DirectXTex")

#include <comdef.h>
#define CHECK_FAILED(hr)													\
    if (FAILED(hr)) {                                                                                                                 \
        static char szResult[256];                                                                                                    \
        _com_error  err(hr);                                                                                                          \
        LPCTSTR     errMsg = err.ErrorMessage();                                                                                      \
        sprintf_s(szResult, sizeof(szResult), "[ERROR] HRESULT: %08X %s#L%d\n%s", static_cast<UINT>(hr), __FILE__, __LINE__, errMsg); \
        MessageBox(nullptr, szResult, "ERROR", MB_OK | MB_ICONERROR);                                                                 \
        throw std::exception(szResult);                                                                                               \
    }


TextureLoader::TextureLoader(std::string filename)
{
	// ワイド文字列変換
	WCHAR wszTexFName[_MAX_PATH];
	int nLen = MultiByteToWideChar(CP_ACP, 0, filename.c_str(), lstrlenA(filename.c_str()), wszTexFName, _countof(wszTexFName));
	if (nLen <= 0) return;
	wszTexFName[nLen] = L'\0';

	m_pMetaData = std::make_unique<DirectX::TexMetadata>();
	m_pScratchImage = std::make_unique<DirectX::ScratchImage>();
	WCHAR wszExt[_MAX_EXT];
	_wsplitpath_s(wszTexFName, nullptr, 0, nullptr, 0, nullptr, 0, wszExt, _MAX_EXT);

	m_desc.name = filename;
	m_desc.usage = core::Usage::DEFAULT;
	m_desc.cpuAccessFlags = 0;
	m_desc.bindFlags = 0 | core::BindFlags::SHADER_RESOURCE;
	m_desc.miscFlags = 0;

	if (_wcsicmp(wszExt, L".tga") == 0)
	{
		CHECK_FAILED(DirectX::LoadFromTGAFile(wszTexFName, m_pMetaData.get(), *m_pScratchImage));
	}
	else if (_wcsicmp(wszExt, L".dds") == 0)
	{
		// DSDロード
		CHECK_FAILED(DirectX::LoadFromDDSFile(wszTexFName, DirectX::DDS_FLAGS_FORCE_RGB, m_pMetaData.get(), *m_pScratchImage));
		m_desc.bindFlags |=  0 | core::BindFlags::RENDER_TARGET;
		m_desc.miscFlags |= 0 | core::MiscFlags::GENERATE_MIPS;
	}
	else if (_wcsicmp(wszExt, L".hdr") == 0)
	{
		// HDRロード
		CHECK_FAILED(DirectX::LoadFromHDRFile(wszTexFName, m_pMetaData.get(), *m_pScratchImage));
		m_desc.bindFlags |= 0 | core::BindFlags::RENDER_TARGET;
		m_desc.miscFlags |= 0 | core::MiscFlags::GENERATE_MIPS;
	}
	else
	{
		// WICロード
		CHECK_FAILED(DirectX::LoadFromWICFile(wszTexFName, DirectX::WIC_FLAGS_FORCE_RGB, m_pMetaData.get(), *m_pScratchImage));
	}

	// キューブマップ
	if (m_pMetaData->IsCubemap())
	{
		m_desc.miscFlags |=  0 | core::MiscFlags::TEXTURECUBE;
	}

	// データ格納
	m_desc.width = m_pMetaData->width;
	m_desc.height = m_pMetaData->height;
	m_desc.depth = m_pMetaData->depth;
	m_desc.arraySize = m_pMetaData->arraySize;
	m_desc.format = (core::TextureFormat)m_pMetaData->format;

}
