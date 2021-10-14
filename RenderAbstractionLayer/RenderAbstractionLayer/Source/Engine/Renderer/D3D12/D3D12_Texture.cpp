/*****************************************************************//**
 * \file   D3D12_Texture.h
 * \brief  DirectX12テクスチャ
 *
 * \author USAMI KOSHI
 * \date   2021/10/14
 *********************************************************************/

#include "D3D12_Texture.h"
#include "D3D12_CommonState.h"
using namespace d3d12;


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
D3D12Texture::D3D12Texture(ID3D12Device* pDevice, const core::TextureID& id, const std::string& filepath) :
	core::CoreTexture(id, filepath),
    m_pTexHeap(nullptr),
    m_pTex(nullptr)
{

}

/// @brief コンストラクタ(Descから生成)
/// @param id テクスチャID
/// @param desc テクスチャDesc
D3D12Texture::D3D12Texture(ID3D12Device* pDevice, const core::TextureID& id, 
    core::TextureDesc& desc, const core::TextureData* pData) :
	core::CoreTexture(id, desc),
	m_pTexHeap(nullptr),
	m_pTex(nullptr)
{
    DXGI_SAMPLE_DESC d3dSampleDesc = { desc.sampleDesc.count, desc.sampleDesc.quality };

    // ヒープ設定
    D3D12_HEAP_PROPERTIES texHeapProp = {};
    texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;  // 特別
    texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;    // CPUから
    texHeapProp.CreationNodeMask = 0;
    texHeapProp.VisibleNodeMask = 0;

    // リソース
    D3D12_RESOURCE_DESC resDesc = {};
    resDesc.Format = d3d12::getDXGIFormat(desc.format);
    resDesc.Width = desc.width;
    resDesc.Height = desc.height;
    resDesc.DepthOrArraySize = desc.arraySize;
    resDesc.SampleDesc = d3dSampleDesc;
    resDesc.MipLevels = 1;//desc.mipLevels;
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


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

        //CHECK_FAILED(pDevice->CreateTexture2D(&d3d11Desc, nullptr, m_tex.GetAddressOf()));
        //if (desc.bindFlags & core::BindFlags::SHADER_RESOURCE) {
        //    CHECK_FAILED(pDevice->CreateShaderResourceView(m_tex.Get(), nullptr, m_srv.GetAddressOf()));
        //}
    }
    else {
        D3D12_SUBRESOURCE_DATA d3d11SubresourceData = {};

        // リソース生成
        CHECK_FAILED(pDevice->CreateCommittedResource(
            &texHeapProp,
            D3D12_HEAP_FLAG_NONE,//特に指定なし
            &resDesc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//テクスチャ用(ピクセルシェーダから見る用)
            nullptr,
            IID_PPV_ARGS(m_pTex.ReleaseAndGetAddressOf())
        ));
        // データ格納
        CHECK_FAILED(m_pTex->WriteToSubresource(0,
            nullptr,//全領域へコピー
            pData->pInitData,//元データアドレス
            static_cast<UINT>(desc.width * desc.depth),//1ラインサイズ
            static_cast<UINT>(desc.width * desc.height * desc.depth)//全サイズ
        ));
        // ヒープ生成
        D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
        descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
        descHeapDesc.NodeMask = 0;//マスクは0
        descHeapDesc.NumDescriptors = 1;//ビューは今のところ１つだけ
        descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//シェーダリソースビュー(および定数、UAVも)

        CHECK_FAILED(pDevice->CreateDescriptorHeap(&descHeapDesc, 
            IID_PPV_ARGS(m_pTexHeap.ReleaseAndGetAddressOf())));//生成

        //通常テクスチャビュー作成
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = resDesc.Format;//DXGI_FORMAT_R8G8B8A8_UNORM;//RGBA(0.0f〜1.0fに正規化)
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;//後述
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
        srvDesc.Texture2D.MipLevels = 1;//ミップマップは使用しないので1

       pDevice->CreateShaderResourceView(m_pTex.Get(), //ビューと関連付けるバッファ
            &srvDesc, //先ほど設定したテクスチャ設定情報
            m_pTexHeap->GetCPUDescriptorHandleForHeapStart()//ヒープのどこに割り当てるか
        );
    }
}

