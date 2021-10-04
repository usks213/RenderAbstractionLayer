/*****************************************************************//**
 * \file   D3D11_RenderContext.h
 * \brief  DirectX11レンダーコンテキストクラス
 *
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#include "D3D11_RenderContext.h"
#include "D3D11_Renderer.h"

using namespace core;
using namespace d3d11;

//------------------------------------------------------------------------------
// public methods 
//------------------------------------------------------------------------------

/// @brief コンストラクタ
D3D11RenderContext::D3D11RenderContext() :
	m_pRenderer(nullptr),
	m_pDevice(nullptr),
	m_pD3DContext(nullptr),
	m_pD3DDeffered(nullptr),
	m_curBlendState(BlendState::NONE),
	m_curRasterizeState(RasterizeState::CULL_NONE),
	m_curDepthStencilState(DepthStencilState::UNKNOWN),
	m_curPrimitiveTopology(PrimitiveTopology::UNKNOWN)
{
}

/// @brief 初期化処理
/// @param pRenderer D3D11レンダラーポインタ
/// @param pDevice D3D11デバイスポインタ
/// @return 初期化: 成功 true | 失敗 false
HRESULT D3D11RenderContext::initialize(D3D11Renderer* pRenderer, D3D11RenderDevice* pDevice)
{
	// 初期化
	m_pRenderer = pRenderer;
	m_pDevice = pDevice;

	return S_OK;
}

