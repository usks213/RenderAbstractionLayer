/*****************************************************************//**
 * \file   D3D11_RenderDevice.cpp
 * \brief  DirectX11�����_�[�f�o�C�X
 * 
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#include "D3D11_RenderDevice.h"
#include <vector>
using namespace core;
using namespace d3d11;

 //------------------------------------------------------------------------------
 // public methods
 //------------------------------------------------------------------------------

/// @brief �R���X�g���N�^
D3D11RenderDevice::D3D11RenderDevice() :
	m_pD3DDevice(nullptr),
	m_backBufferRT(nullptr),
	m_backBufferRTV(nullptr),
	m_backBufferFormat(DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM),
	m_depthStencilTexture(nullptr),
	m_depthStencilView(nullptr),
	m_depthStencilFormat(DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT),

	m_hWnd(nullptr),
	m_viewport(),
	m_backBufferCount(2u),
	m_nOutputWidth(1u),
	m_nOutputHeight(1u)
{

}

/// @brief ����������
/// @param hWnd �E�B���h�E�n���h��
/// @param width �E�B���h�E�̕�
/// @param height �E�B���h�E�̍���
/// @return ������: ���� true | ���s false
HRESULT D3D11RenderDevice::initialize(ID3D11Device1* pDevice, IDXGIFactory2* pFactory2,
	HWND hWnd, UINT width, UINT height)
{
	// ������
	m_pD3DDevice = pDevice;
	m_hWnd = hWnd;
	m_nOutputWidth = width;
	m_nOutputHeight = height;

	// �X���b�v�`�F�[���ƃo�b�t�@�̏�����
	CHECK_FAILED(createSwapChainAndBuffer(pFactory2));

	// ���ʃX�e�[�g�̏�����
	CHECK_FAILED(createCommonState());

	return S_OK;
}

 //------------------------------------------------------------------------------
 // private methods
 //------------------------------------------------------------------------------

/// @brief �X���b�v�`�F�[���ƃo�b�t�@�̐���
/// @return HRESULT
HRESULT D3D11RenderDevice::createSwapChainAndBuffer(IDXGIFactory2* pFactory2)
{
	HRESULT hr = S_OK;
	HWND &hWnd = m_hWnd;

	// �����_�[�^�[�Q�b�g�̃T�C�Y�w��
	UINT backBufferWidth = std::max<UINT>(m_nOutputWidth, 1u);
	UINT backBufferHeight = std::max<UINT>(m_nOutputHeight, 1u);

	//�g�p�\��MSAA���擾
	if (m_sampleDesc.isUse)
	{
		for (int i = 0; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i++) {
			UINT Quality;
			if SUCCEEDED(m_pD3DDevice->CheckMultisampleQualityLevels(m_backBufferFormat, i, &Quality)) {
				if (0 < Quality) {
					m_sampleDesc.count = i;
					m_sampleDesc.quality = Quality - 1;
				}
			}
		}
	}
	DXGI_SAMPLE_DESC dxgiSampleDesc = { m_sampleDesc.count, m_sampleDesc.quality };

	//--- �X���b�v�`�F�C�� ---
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = backBufferWidth;
	swapChainDesc.Height = backBufferHeight;
	swapChainDesc.Format = m_backBufferFormat;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = m_backBufferCount;
	swapChainDesc.SampleDesc = dxgiSampleDesc;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = 0;
	//swapChainDesc.Flags = (m_options & ALLOW_TEARING) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	// �t���X�N���[���w��
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
	fsSwapChainDesc.Windowed = TRUE;

	// �X���b�v�`�F�[���̍쐬
	hr = pFactory2->CreateSwapChainForHwnd(
		m_pD3DDevice,
		m_hWnd,
		&swapChainDesc,
		&fsSwapChainDesc,
		nullptr,
		m_swapChain.ReleaseAndGetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateSwapChainForHwnd", L"Err", MB_ICONSTOP);
		return hr;
	}

	// �r���I�t���X�N���[�����[�h�𖳌���
	hr = pFactory2->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"MakeWindowAssociation", L"Err", MB_ICONSTOP);
		return hr;
	}


	//--- �o�b�N�o�b�t�@RT ---

	// �t�H�[�}�b�g�T�|�[�g�`�F�b�N
	std::uint32_t formatSupport = 0;
	if (FAILED(m_pD3DDevice->CheckFormatSupport(m_backBufferFormat, &formatSupport)))
	{
		MessageBoxW(hWnd, L"CheckFormatSupport", L"Err", MB_ICONSTOP);
		return E_FAIL;
	}

	// �o�b�N�o�b�t�@�����_�[�^�[�Q�b�g�̍쐬
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)m_backBufferRT.ReleaseAndGetAddressOf());

	CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc(D3D11_RTV_DIMENSION_TEXTURE2D, m_backBufferFormat);
	if (m_sampleDesc.isUse) rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	// �����_�[�^�[�Q�b�g�r���[�쐬
	hr = m_pD3DDevice->CreateRenderTargetView(
		m_backBufferRT.Get(),
		&rtvDesc,
		m_backBufferRTV.ReleaseAndGetAddressOf());
	if (FAILED(hr)) {
		MessageBoxW(hWnd, L"CreateRenderTargetView", L"Err", MB_ICONSTOP);
		return hr;
	}

	//--- �[�x�X�e���V�� ---
	CD3D11_TEXTURE2D_DESC dsDesc(m_depthStencilFormat, backBufferWidth, backBufferHeight,
		1, 1, D3D11_BIND_DEPTH_STENCIL);
	if (m_sampleDesc.isUse) dsDesc.SampleDesc = dxgiSampleDesc;
	// �[�x�X�e���V���e�N�X�`���쐬
	hr = m_pD3DDevice->CreateTexture2D(&dsDesc, nullptr,
		m_depthStencilTexture.ReleaseAndGetAddressOf());
	CHECK_FAILED(hr);
	// �[�x�X�e���V���e�N�X�`���r���[
	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	if (m_sampleDesc.isUse) dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hr = m_pD3DDevice->CreateDepthStencilView(m_depthStencilTexture.Get(),
		&dsvDesc, m_depthStencilView.ReleaseAndGetAddressOf());
	CHECK_FAILED(hr);

	// �r���[�|�[�g
	m_viewport = CD3D11_VIEWPORT(0.0f, 0.0f,
		static_cast<float>(backBufferWidth),
		static_cast<float>(backBufferHeight));

	return S_OK;
}

/// @brief ���ʃX�e�[�g�̐���
/// @return HRESULT
HRESULT D3D11RenderDevice::createCommonState()
{
	// ���X�^���C�U�X�e�[�g�쐬
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};

		// �J�����O�Ȃ� �h��Ԃ�
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		CHECK_FAILED(m_pD3DDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::CULL_NONE].GetAddressOf()));

		// �\�ʃJ�����O �h��Ԃ�
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		CHECK_FAILED(m_pD3DDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::CULL_FRONT].GetAddressOf()));

		// ���ʃJ�����O �h��Ԃ�
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		CHECK_FAILED(m_pD3DDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::CULL_BACK].GetAddressOf()));

		// �J�����O�Ȃ� ���C���[�t���[��
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		CHECK_FAILED(m_pD3DDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::CULL_NONE_WIREFRAME].GetAddressOf()));

		// �\�ʃJ�����O ���C���[�t���[��
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		CHECK_FAILED(m_pD3DDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::CULL_FRONT_WIREFRAME].GetAddressOf()));

		// ���ʃJ�����O ���C���[�t���[��
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		CHECK_FAILED(m_pD3DDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::CULL_BACK_WIREFRAME].GetAddressOf()));

		// �V���h�E�p
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.DepthBias = 100000;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.SlopeScaledDepthBias = 2.0f;
		CHECK_FAILED(m_pD3DDevice->CreateRasterizerState(&rasterizerDesc,
			m_rasterizeStates[(size_t)RasterizeState::SHADOW].GetAddressOf()));
	}

	// �T���v���X�e�[�g�쐬
	{
		D3D11_SAMPLER_DESC samplerDesc = {};

		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0.f;
		samplerDesc.BorderColor[1] = 0.f;
		samplerDesc.BorderColor[2] = 0.f;
		samplerDesc.BorderColor[3] = 0.f;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.MaxAnisotropy = (m_pD3DDevice->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? D3D11_MAX_MAXANISOTROPY : 2;

		// ���j�A�N�����v
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		CHECK_FAILED(m_pD3DDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::LINEAR_CLAMP)].GetAddressOf()));

		// �|�C���g�N�����v
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		CHECK_FAILED(m_pD3DDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::POINT_CLAMP)].GetAddressOf()));

		// �ٕ����N�����v
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		CHECK_FAILED(m_pD3DDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::ANISOTROPIC_CLAMP)].GetAddressOf()));

		// ���j�A���b�v
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		CHECK_FAILED(m_pD3DDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::LINEAR_WRAP)].GetAddressOf()));

		// �|�C���g���b�v
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		CHECK_FAILED(m_pD3DDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::POINT_WRAP)].GetAddressOf()));

		// �ٕ������b�v
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		CHECK_FAILED(m_pD3DDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::ANISOTROPIC_WRAP)].GetAddressOf()));

		// �V���h�E
		samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		CHECK_FAILED(m_pD3DDevice->CreateSamplerState(&samplerDesc, m_samplerStates[static_cast<size_t>(SamplerState::SHADOW)].GetAddressOf()));
	}

	// �u�����h�X�e�[�g�쐬
	{
		D3D11_BLEND_DESC blendDesc = {};

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		// ����������
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		CHECK_FAILED(m_pD3DDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(BlendState::ALPHA)].GetAddressOf()));

		// ���Z����
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		CHECK_FAILED(m_pD3DDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(BlendState::ADD)].GetAddressOf()));

		// ���Z����
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		CHECK_FAILED(m_pD3DDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(BlendState::SUB)].GetAddressOf()));

		// ��Z����
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
		CHECK_FAILED(m_pD3DDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(BlendState::MUL)].GetAddressOf()));

		// ���]����
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		CHECK_FAILED(m_pD3DDevice->CreateBlendState(&blendDesc, m_blendStates[static_cast<size_t>(BlendState::INV)].GetAddressOf()));
	}

	// �[�x�X�e���V���X�e�[�g�쐬
	{
		m_depthStencilStates[static_cast<size_t>(DepthStencilState::UNKNOWN)] = nullptr;

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

		CHECK_FAILED(m_pD3DDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(DepthStencilState::ENABLE_TEST_AND_ENABLE_WRITE)].GetAddressOf()));

		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		CHECK_FAILED(m_pD3DDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(DepthStencilState::ENABLE_TEST_AND_DISABLE_WRITE)].GetAddressOf()));

		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

		CHECK_FAILED(m_pD3DDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(DepthStencilState::DISABLE_TEST_AND_ENABLE_WRITE)].GetAddressOf()));

		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		CHECK_FAILED(m_pD3DDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[static_cast<size_t>(DepthStencilState::DISABLE_TEST_AND_DISABLE_WRITE)].GetAddressOf()));
	}

	return S_OK;
}
