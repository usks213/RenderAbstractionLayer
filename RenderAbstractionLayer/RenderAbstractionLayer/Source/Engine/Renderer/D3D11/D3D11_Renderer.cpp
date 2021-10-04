/*****************************************************************//**
 * \file   D3D11_Renderer.cpp
 * \brief  DirectX11�����_���[�N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#include "D3D11_Renderer.h"
 // ���C�u���������N
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "DXGI.lib")

using namespace d3d11;


/// @brief �R���X�g���N�^
D3D11Renderer::D3D11Renderer()
{

}

 /// @brief ����������
 /// @param hWnd �E�B���h�E�n���h��
 /// @param width �E�B���h�E�̕�
 /// @param height �E�B���h�E�̍���
 /// @return ������: ���� true | ���s false
HRESULT D3D11Renderer::initialize(HWND hWnd, UINT width, UINT height)
{
	HRESULT hr = S_OK;

	// �f�o�C�X�̏�����
	CHECK_FAILED(hr = m_device.initialize(hWnd, width, height));

	// �R���e�L�X�g�̏�����
	CHECK_FAILED(hr = m_context.initialize(this, &m_device));

	return hr;
}

/// @brief �I������
void D3D11Renderer::finalize()
{

}

/// @brief ��ʃN���A
void D3D11Renderer::clear()
{
	// �C�x���g�J�n
	m_device.m_d3dAnnotation->BeginEvent(L"Clear");
	m_context.m_d3dContext->OMSetBlendState(nullptr, nullptr, std::numeric_limits<UINT>::max());

	// �o�b�t�@�̃N���A
	float ClearColor[4] = { 0.2f, 0.22f, 0.22f, 1.0f };
	m_context.m_d3dContext->ClearRenderTargetView(m_device.m_backBufferRTV.Get(), ClearColor);
	m_context.m_d3dContext->OMSetRenderTargets(1, m_device.m_backBufferRTV.GetAddressOf(), 
		m_device.m_depthStencilView.Get());

	// �r���[�|�[�g
	m_context.m_d3dContext->RSSetViewports(1, &m_device.m_viewport);

	// �C�x���g�I��
	m_device.m_d3dAnnotation->EndEvent();
}

/// @brief ��ʍX�V
void D3D11Renderer::present()
{
	// �o�b�N�o�b�t�@�ɖ߂�
	m_context.m_d3dContext->OMSetRenderTargets(1, m_device.m_backBufferRTV.GetAddressOf(),
		m_device.m_depthStencilView.Get());

	//m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);	// �e�B�A�����O���e�`��
	//m_swapChain->Present(0, 0);							// �񐂒������`��
	m_device.m_swapChain->Present(1, 0);					// ���������`��
}
