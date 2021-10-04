/*****************************************************************//**
 * \file   D3D11_RenderContext.h
 * \brief  DirectX11�����_�[�R���e�L�X�g�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#include "D3D11_RenderContext.h"
#include "D3D11_RenderDevice.h"
using namespace core;
using namespace d3d11;

//------------------------------------------------------------------------------
// public methods 
//------------------------------------------------------------------------------

/// @brief �R���X�g���N�^
D3D11RenderContext::D3D11RenderContext() :
	m_pRenderer(nullptr),
	m_pDevice(nullptr),
	m_d3dContext(nullptr),
	m_d3dDefferedContext(nullptr),
	m_curBlendState(BlendState::NONE),
	m_curRasterizeState(RasterizeState::CULL_NONE),
	m_curDepthStencilState(DepthStencilState::UNKNOWN),
	m_curPrimitiveTopology(PrimitiveTopology::UNKNOWN)
{
}


/// @brief ����������
/// @param pRenderer D3D11�����_���[�|�C���^
/// @param pDevice D3D11�f�o�C�X�|�C���^
/// @return ������: ���� true | ���s false
HRESULT D3D11RenderContext::initialize(D3D11Renderer* pRenderer, D3D11RenderDevice* pDevice)
{
	// ������
	m_pRenderer = pRenderer;
	m_pDevice = pDevice;

	m_d3dContext = m_pDevice->m_d3dContext.Get();
	m_d3dDefferedContext = m_pDevice->m_d3dDefferedContext.Get();

	return S_OK;
}

