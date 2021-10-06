/*****************************************************************//**
 * \file   D3D11_Texture.h
 * \brief  DirectX11テクスチャ
 *
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/

#include "D3D11_Texture.h"
#include "D3D11_CommonState.h"
using namespace d3d11;


//------------------------------------------------------------------------------
// local methods
//------------------------------------------------------------------------------
namespace
{
    /// @brief ミップマップレベルを計算
    /// @return ミップマップレベル
    template<typename T> static constexpr T numMipmapLevels(T width, T height)
    {
        T levels = 1;
        while ((width | height) >> levels) {
            ++levels;
        }
        return levels;
    }
}

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

/// @brief コンストラクタ(ファイル読み込み)
/// @param id テクスチャID
/// @param filepath ファイルパス
D3D11Texture::D3D11Texture(ID3D11Device1* pDevice, const core::TextureID& id, const std::string& filepath) :
	core::CoreTexture(id, filepath),
	m_tex(nullptr),
	m_srv(nullptr)
{

}

/// @brief コンストラクタ(Descから生成)
/// @param id テクスチャID
/// @param desc テクスチャDesc
D3D11Texture::D3D11Texture(ID3D11Device1* pDevice, const core::TextureID& id, 
    core::TextureDesc& desc, const core::TextureData* pData) :
	core::CoreTexture(id, desc),
	m_tex(nullptr),
	m_srv(nullptr)
{
    DXGI_SAMPLE_DESC d3d11SampleDesc = { desc.sampleDesc.count, desc.sampleDesc.quality };

    D3D11_TEXTURE2D_DESC d3d11Desc = {
        desc.width,
        desc.height,
        desc.mipLevels,
        desc.arraySize,
        getDXGIFormat(desc.format),
        d3d11SampleDesc,
        getD3D11Usage(desc.usage),
        desc.bindFlags,
        desc.cpuAccessFlags,
        desc.miscFlags,
    };

    // ミップマップ自動生成指定
    if (desc.mipLevels == 0 && 
        desc.bindFlags & core::BindFlags::RENDER_TARGET && 
        desc.bindFlags & core::BindFlags::SHADER_RESOURCE && 
        desc.miscFlags & core::MiscFlags::GENERATE_MIPS 
        )
    {
        d3d11Desc.MipLevels = desc.mipLevels = numMipmapLevels(desc.width, desc.height);
    }


    // 初期値なし
    if (pData == nullptr) {
        if (desc.width <= 0 && desc.height <= 0) {
            // 生成不可
            return;
        }
        if (desc.usage == core::Usage::STATIC) {
            // エラー 初期値なし、書き換え不可
            return;
        }

        CHECK_FAILED(pDevice->CreateTexture2D(&d3d11Desc, nullptr, m_tex.GetAddressOf()));
        if (desc.bindFlags & core::BindFlags::SHADER_RESOURCE) {
            CHECK_FAILED(pDevice->CreateShaderResourceView(m_tex.Get(), nullptr, m_srv.GetAddressOf()));
        }
    }
    else {
        D3D11_SUBRESOURCE_DATA d3d11SubresourceData = {};
        d3d11SubresourceData.pSysMem = pData->pInitData;
        d3d11SubresourceData.SysMemPitch = desc.width * desc.depth;
        d3d11SubresourceData.SysMemSlicePitch = desc.width * desc.height * desc.depth;

        CHECK_FAILED(pDevice->CreateTexture2D(&d3d11Desc, &d3d11SubresourceData, m_tex.GetAddressOf()));
        if (desc.bindFlags & core::BindFlags::SHADER_RESOURCE) {
            CHECK_FAILED(pDevice->CreateShaderResourceView(m_tex.Get(), nullptr, m_srv.GetAddressOf()));
        }
    }
}

