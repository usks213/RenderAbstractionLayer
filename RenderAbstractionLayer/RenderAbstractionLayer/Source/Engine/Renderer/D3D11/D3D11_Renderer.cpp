/*****************************************************************//**
 * \file   D3D11_Renderer.cpp
 * \brief  DirectX11�����_���[�N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#include "D3D11_Renderer.h"
#include <vector>

using namespace d3d11;


/// @brief �R���X�g���N�^
D3D11Renderer::D3D11Renderer() :
	m_device(),
	m_context(),
	m_d3dDevice(),
	m_d3dAnnotation(),
	m_d3dContext(),
	m_d3dDefferedContext(),
	m_dxgiFactory()
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

	// �f�o�C�X�ƃR���e�L�X�g�̍쐬
	CHECK_FAILED(hr = createDiveceAndContext(hWnd));

	// �f�o�C�X�̏�����
	CHECK_FAILED(hr = m_device.initialize(m_d3dDevice.Get(), 
		m_dxgiFactory.Get(), hWnd, width, height));

	// �R���e�L�X�g�̏�����
	m_context.m_pD3DContext = m_d3dContext.Get();
	m_context.m_pD3DDeffered = m_d3dDefferedContext.Get();
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
	m_d3dAnnotation->BeginEvent(L"Clear");
	m_d3dContext->OMSetBlendState(nullptr, nullptr, std::numeric_limits<UINT>::max());

	// �o�b�t�@�̃N���A
	float ClearColor[4] = { 0.2f, 0.22f, 0.22f, 1.0f };
	m_d3dContext->ClearRenderTargetView(m_device.m_backBufferRTV.Get(), ClearColor);
	m_d3dContext->ClearDepthStencilView(m_device.m_depthStencilView.Get(), 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_d3dContext->OMSetRenderTargets(1, m_device.m_backBufferRTV.GetAddressOf(), 
		m_device.m_depthStencilView.Get());

	// �r���[�|�[�g
	m_d3dContext->RSSetViewports(1, &m_device.m_viewport);

	// �C�x���g�I��
	m_d3dAnnotation->EndEvent();
}

/// @brief ��ʍX�V
void D3D11Renderer::present()
{
	// �o�b�N�o�b�t�@�ɖ߂�
	m_d3dContext->OMSetRenderTargets(1, m_device.m_backBufferRTV.GetAddressOf(),
		m_device.m_depthStencilView.Get());

	//m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);	// �e�B�A�����O���e�`��
	//m_swapChain->Present(0, 0);							// �񐂒������`��
	m_device.m_swapChain->Present(1, 0);					// ���������`��
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------

/// @brief �f�o�C�X�ƃR���e�L�X�g�̐���
/// @return HRESULT
HRESULT D3D11Renderer::createDiveceAndContext(HWND hWnd)
{
	HRESULT hr = S_OK;

	D3D_FEATURE_LEVEL featureLevels[] = {
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1,
	};

	// �t�@�N�g���[�̎擾
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)(&m_dxgiFactory));
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateDXGIFactory1", L"Err", MB_ICONSTOP);
		return hr;
	}
	// �A�_�v�^�[�̎擾
	IDXGIAdapter* iAdapter = NULL;
	std::vector<IDXGIAdapter*> aiAdapter;
	aiAdapter.push_back(iAdapter);
	for (unsigned int index = 0;; index++)
	{
		HRESULT	ret = m_dxgiFactory->EnumAdapters(index, &iAdapter);

		if (ret == DXGI_ERROR_NOT_FOUND)
		{
			break;
		}
		// �` �A�_�v�^�̑I��
		aiAdapter.push_back(iAdapter);
	}
	aiAdapter.pop_back();
	if (aiAdapter.size() <= 0)
	{
		aiAdapter.push_back(NULL);
	}


	//--- �f�o�C�X�̐��� ---
	ComPtr<ID3D11Device>		device;
	ComPtr<ID3D11DeviceContext> context;
	ComPtr<ID3D11DeviceContext> defferedContext;

	// �f�o�C�X�B�R���e�L�X�g�̐���
	hr = D3D11CreateDevice(
		aiAdapter.back(),
		aiAdapter.back() ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		featureLevels,
		7,
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		NULL,
		context.GetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"D3D11CreateDevice", L"Err", MB_ICONSTOP);
		return hr;
	}

	// �x���R���e�L�X�g�쐬
	hr = device->CreateDeferredContext(0, defferedContext.GetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateDeferredContext", L"Err", MB_ICONSTOP);
		return hr;
	}

	// �i�[
	hr = device.As(&m_d3dDevice);
	hr = context.As(&m_d3dContext);
	hr = context.As(&m_d3dAnnotation);
	hr = defferedContext.As(&m_d3dDefferedContext);

	return S_OK;
}
