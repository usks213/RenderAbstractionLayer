/*****************************************************************//**
 * \file   D3D11_Texture.h
 * \brief  DirectX11テクスチャ
 *
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/

#include "D3D11_Texture.h"
using namespace d3d11;

template<typename T> static constexpr T numMipmapLevels(T width, T height)
{
	T levels = 1;
	while ((width | height) >> levels) {
		++levels;
	}
	return levels;
}


 /// @brief コンストラクタ
D3D11Texture::D3D11Texture(const core::TextureID& id, const std::string& path)
	: core::CoreTexture(id, path)
{
}

/// @brief ファイルからテクスチャの読み込み
HRESULT D3D11Texture::CreateFromFile(ID3D11Device1* device, ID3D11DeviceContext1* context)
{
	return S_OK;
	//return CreateTextureFromFile(device, context, m_name.c_str());
}

/// @brief メモリからテクスチャの読み込み
HRESULT D3D11Texture::CreateFromMemory(ID3D11Device1* device, const uint8_t* wicData, size_t wicDataSize)
{
	return S_OK;
	//return CreateTextureFromMemory(device, wicData, wicDataSize);
}

// // ワイド文字列
//HRESULT D3D11Texture::CreateTextureFromFile(_In_ ID3D11Device1* d3dDevice,
//	_In_ ID3D11DeviceContext1* context,
//	_In_z_ const wchar_t* szFileName,
//	_Out_opt_ TexMetadata* pTexInfo)
//{
//	HRESULT hr = S_OK;
//	TexMetadata meta;
//	ScratchImage image;
//	WCHAR wszExt[_MAX_EXT];
//	_wsplitpath(szFileName, nullptr, nullptr, nullptr, wszExt);
//
//	D3D11_TEXTURE2D_DESC d3d11Desc = {};
//	d3d11Desc.SampleDesc.Count = 1;
//	d3d11Desc.SampleDesc.Quality = 0;
//	d3d11Desc.Usage = D3D11_USAGE_DEFAULT;
//	d3d11Desc.CPUAccessFlags = 0;
//	//d3d11Desc.Usage = D3D11_USAGE_DYNAMIC;
//	//d3d11Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	d3d11Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//	//if (isCubeMap == 0) {
//	//	//d3d11Desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
//	//	//d3d11Desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
//	//}
//	//else
//	//{
//	//	d3d11Desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
//	//}
//
//	if (_wcsicmp(wszExt, L".tga") == 0)
//	{
//		hr = LoadFromTGAFile(szFileName, &meta, image);
//		if (FAILED(hr)) return hr;
//		if (pTexInfo) *pTexInfo = meta;
//
//		d3d11Desc.Width = meta.width;
//		d3d11Desc.Height = meta.height;
//		d3d11Desc.MipLevels = meta.mipLevels;
//		d3d11Desc.ArraySize = meta.arraySize;
//		d3d11Desc.Format = meta.format;
//
//		D3D11_SUBRESOURCE_DATA d3d11SubresourceData = {};
//		d3d11SubresourceData.pSysMem = image.GetPixels();
//		d3d11SubresourceData.SysMemPitch = static_cast<UINT>(meta.width * 4);
//		d3d11SubresourceData.SysMemSlicePitch = static_cast<UINT>(meta.width * meta.height * 4);
//
//		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
//		SRVDesc.Format = d3d11Desc.Format;
//		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//		//if (isCubeMap)
//		//{
//		//	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
//		//	SRVDesc.TextureCube.MostDetailedMip = 0;
//		//	SRVDesc.TextureCube.MipLevels = 1;
//		//}
//
//		hr = d3dDevice->CreateTexture2D(&d3d11Desc, &d3d11SubresourceData, m_tex.GetAddressOf());
//		if (FAILED(hr)) return hr;
//		hr = d3dDevice->CreateShaderResourceView(m_tex.Get(), &SRVDesc, m_srv.GetAddressOf());
//		if (FAILED(hr)) return hr;
//	}
//	else if (_wcsicmp(wszExt, L".dds") == 0)
//	{
//		//hr = LoadFromDDSFile(szFileName, DDS_FLAGS_FORCE_RGB, &meta, image);
//		hr = (DirectX::CreateDDSTextureFromFileEx(
//			d3dDevice, szFileName, 0,
//			d3d11Desc.Usage, d3d11Desc.BindFlags, d3d11Desc.CPUAccessFlags, d3d11Desc.MiscFlags,
//			false, reinterpret_cast<ID3D11Resource**>(m_tex.GetAddressOf()),
//			m_srv.GetAddressOf()));
//		if (FAILED(hr)) return hr;
//	}
//	else if (_wcsicmp(wszExt, L".hdr") == 0)
//	{
//		 hr = LoadFromHDRFile(szFileName, &meta, image);
//		 if (FAILED(hr)) return hr;
//		 //DirectX::CreateTexture();
//		d3d11Desc.Usage = D3D11_USAGE_DEFAULT;
//		d3d11Desc.CPUAccessFlags = 0;
//		 //meta.mipLevels = 0;	
//		 //if (meta.mipLevels == 0) {
//			 d3d11Desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
//			 d3d11Desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
//		 //}
//
//		 d3d11Desc.Width = meta.width;
//		 d3d11Desc.Height = meta.height;
//		 d3d11Desc.MipLevels = numMipmapLevels(meta.width, meta.height);
//		 d3d11Desc.ArraySize = meta.arraySize;
//		 d3d11Desc.Format = meta.format;
//
//		 D3D11_SUBRESOURCE_DATA d3d11SubresourceData = {};
//		 d3d11SubresourceData.pSysMem = image.GetPixels();
//		 d3d11SubresourceData.SysMemPitch = static_cast<UINT>(meta.width * 4 * 4);
//		 d3d11SubresourceData.SysMemSlicePitch = static_cast<UINT>(meta.width * meta.height * 4 * 4);
//
//		 D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
//		 SRVDesc.Format = d3d11Desc.Format;
//		 SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//		 SRVDesc.Texture2D.MostDetailedMip = 0;
//		 SRVDesc.Texture2D.MipLevels = d3d11Desc.MipLevels;
//		 
//		 hr = (d3dDevice->CreateTexture2D(&d3d11Desc, nullptr, m_tex.GetAddressOf()));
//		 if (FAILED(hr)) return hr;
//		 hr = (d3dDevice->CreateShaderResourceView(m_tex.Get(), &SRVDesc, m_srv.GetAddressOf()));
//		 if (FAILED(hr)) return hr;
//
//		 context->UpdateSubresource(m_tex.Get(), 0, nullptr, d3d11SubresourceData.pSysMem, d3d11SubresourceData.SysMemPitch, d3d11SubresourceData.SysMemSlicePitch);
//		 context->GenerateMips(m_srv.Get());
//
//		 //CreateShaderResourceView(d3dDevice, image.GetImages(), image.GetImageCount(), meta, m_srv.GetAddressOf());
//	}
//	else
//	{
//		hr = (DirectX::CreateWICTextureFromFileEx(
//			d3dDevice,
//			szFileName,
//			0,
//			d3d11Desc.Usage,
//			d3d11Desc.BindFlags,
//			d3d11Desc.CPUAccessFlags,
//			d3d11Desc.MiscFlags,
//			WIC_LOADER_DEFAULT,
//			reinterpret_cast<ID3D11Resource**>(m_tex.GetAddressOf()),
//			m_srv.GetAddressOf()));
//		if (FAILED(hr)) return hr;
//	}
//
//	return hr;
//}
//
//
//// マルチバイト文字
//HRESULT D3D11Texture::CreateTextureFromFile(_In_ ID3D11Device1* d3dDevice,
//	_In_ ID3D11DeviceContext1* context,
//	_In_z_ const char* szFileName,
//	_Out_opt_ TexMetadata* pTexInfo)
//{
//	WCHAR wszTexFName[_MAX_PATH];
//	int nLen = MultiByteToWideChar(CP_ACP, 0, szFileName, lstrlenA(szFileName), wszTexFName, _countof(wszTexFName));
//	if (nLen <= 0) return E_FAIL;
//	wszTexFName[nLen] = L'\0';
//	return CreateTextureFromFile(d3dDevice, context, wszTexFName, pTexInfo);
//}
//
//// メモリから生成
//HRESULT D3D11Texture::CreateTextureFromMemory(_In_ ID3D11Device1* d3dDevice,
//	_In_bytecount_(wicDataSize) const uint8_t* wicData,
//	_In_ size_t wicDataSize,
//	_Out_opt_ TexMetadata* pTexInfo)
//{
//	TexMetadata meta;
//	ScratchImage image;
//	HRESULT hr;
//	if (wicDataSize >= 18 && memcmp(&wicData[wicDataSize - 18], "TRUEVISION-XFILE.", 18) == 0) {
//		hr = LoadFromTGAMemory(wicData, wicDataSize, &meta, image);
//	}
//	else if (wicDataSize >= 4 && memcmp(wicData, "DDS ", 4) == 0) {
//		hr = LoadFromDDSMemory(wicData, wicDataSize, DDS_FLAGS_FORCE_RGB, &meta, image);
//	}
//	else {
//		hr = LoadFromWICMemory(wicData, wicDataSize, WIC_FLAGS_FORCE_RGB, &meta, image);
//	}
//	if (FAILED(hr)) return hr;
//	if (pTexInfo) *pTexInfo = meta;
//
//	m_tex.Reset();
//	auto* pTex = static_cast<ID3D11Resource*>(m_tex.Get());
//	hr = DirectX::CreateTexture(d3dDevice, image.GetImages(), image.GetImageCount(), meta, &pTex);
//	if (FAILED(hr)) return hr;
//
//	return CreateShaderResourceView(d3dDevice, image.GetImages(), image.GetImageCount(), meta, m_srv.ReleaseAndGetAddressOf());
//}