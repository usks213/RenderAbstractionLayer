/*****************************************************************//**
 * \file   D3D12_CommonState.h
 * \brief  DirectX12共通ステート
 * 
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/
#ifndef _D3D12_COMMON_STATE_
#define _D3D12_COMMON_STATE_

#include <Renderer/Core/Core_CommonState.h>
#include "D3D12_Defines.h"

namespace d3d12
{
    /// @brief D3D12のリソース使用識別取得
    /// @param usage リソース使用識別
    /// @return D3D12リソース使用識別
    D3D12_USAGE getD3D12Usage(core::Usage usage);

    /// @brief D3D12のバインドフラグ取得
    /// @param flags バインドフラグ
    /// @return 符号なし整数型バインドフラグ
    UINT32 getD3D12BindFlags(core::BindFlags flags);

    /// @brief D3D12のCPUアクセスフラグ取得
    /// @param flags CPUアクセスフラグ
    /// @return 符号なし整数型CPUアクセスフラグ
    UINT32 getD3D12CPUAccessFlags(core::CPUAccessFlags flags);

    /// @brief D3D12のバッファフラグ取得
    /// @param flags バッファフラグ
    /// @return 符号なし整数型バッファフラグ
    UINT32 getD3D12MiscFlags(core::MiscFlags flags);

    /// @brief D3D12のプリミティブトポロジー取得
    /// @param topology プリミティブトポロジー
    /// @return D3D12プリミティブトポロジー
    D3D12_PRIMITIVE_TOPOLOGY getD3D12PrimitiveTopology(core::PrimitiveTopology topology);

    /// @brief DXGIのテクスチャフォーマット取得
    /// @param format テクスチャフォーマット
    /// @return DXGIフォーマット
    DXGI_FORMAT getDXGIFormat(core::TextureFormat format);
}

#endif // !_D3D12_COMMON_STATE_

