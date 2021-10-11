/*****************************************************************//**
 * \file   D3D12_Renderer.h
 * \brief  DirectX12�����_���[
 *
 * \author USAMI KOSHI
 * \date   2021/10/08
 *********************************************************************/

#include "D3D12_Renderer.h"
using namespace d3d12;


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

/// @brief �R���X�g���N�^
D3D12Renderer::D3D12Renderer()
{

}

/// @brief ����������
/// @param hWnd �E�B���h�E�n���h��
/// @param width �E�B���h�E�̕�
/// @param height �E�B���h�E�̍���
/// @return ������: ���� true | ���s false
HRESULT D3D12Renderer::initialize(HWND hWnd, UINT width, UINT height)
{
	HRESULT hr = S_OK;

	//----- �f�o�b�O���C���[�E�t�@�N�g���[ -----

#ifdef _DEBUG
	// �f�o�b�O���C���[
	{
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(m_pDebugLater.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
		m_pDebugLater->EnableDebugLayer();
	}
#endif // _DEBUG

	// �t�@�N�g���[�̐���
	{
#ifdef _DEBUG
		hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(m_pDXGIFactory.ReleaseAndGetAddressOf()));
#elif
		hr = CreateDXGIFactory1(IID_PPV_ARGS(m_pDXGIFactory.ReleaseAndGetAddressOf()));
#endif // _DEBUG
		CHECK_FAILED(hr);
	}

	//----- �A�_�v�^�E�f�o�C�X -----

	// �f�o�C�X����
	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m_pD3DDevice.ReleaseAndGetAddressOf()));
	CHECK_FAILED(hr);


	//----- �R�}���h�L���[�E�R�}���h�A���P�[�^�[�E�R�}���h���X�g -----
	D3D12_COMMAND_LIST_TYPE cmdType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	UINT nodeMask = 0;

	// �R�}���h�L���[�̐���
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = cmdType;									// �R�}���h�o�b�t�@�̎��
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// �R�}���h�L���[�̗D��x
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;				// ??? GPUTimeOut
		desc.NodeMask = nodeMask;								// GPU���ʃ}�X�N(1��0)

		hr = m_pD3DDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pCmdQueue.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
	}

	// �R�}���h�A���P�[�^�[�̐���
	{
		hr = m_pD3DDevice->CreateCommandAllocator(cmdType, 
			IID_PPV_ARGS(m_pCmdAllocator.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
	}

	// �R�}���h���X�g�̐���
	{
		hr = m_pD3DDevice->CreateCommandList(nodeMask, cmdType, m_pCmdAllocator.Get(),
			nullptr, IID_PPV_ARGS(m_pCmdList.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
		m_pCmdList->Close();
	}


	//----- �X���b�v�`�F�C���E�t�F���X�E�����_�[�^�[�Q�b�g -----

	// �X���b�v�`�F�C���̐���
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = false;								// �t���X�N���[���w��
		desc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };	
		desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;			// �o�b�t�@����
		desc.BufferCount = BACK_BUFFER_COUNT;						// �o�b�N�o�b�t�@��
		desc.Scaling = DXGI_SCALING_STRETCH;				// �o�b�N�o�b�t�@�̃X�P�[�����O�w��
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// �X���b�v���̃o�b�t�@�̈���
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;		// �A���t�@���[�h??
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;// ����I�v�V���� (�t���X�N���[���؂�ւ���)

		IDXGISwapChain1* pTempSwapChain1;
		hr = m_pDXGIFactory->CreateSwapChainForHwnd(
			m_pCmdQueue.Get(),
			hWnd,
			&desc,
			nullptr,
			nullptr,
			&pTempSwapChain1
		);
		CHECK_FAILED(hr);
		
		// �X���b�v�`�F�C���S�ɕϊ�
		hr = pTempSwapChain1->QueryInterface(IID_PPV_ARGS(m_pSwapChain.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
		
		// �t���[���o�b�t�@�ԍ��擾

	}

	// �t�F���X�̐���
	{
		m_nFenceVal = 0;
		hr = m_pD3DDevice->CreateFence(m_nFenceVal, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf()));
		CHECK_FAILED(hr);
	}

	// �����_�[�^�[�Q�b�g�q�[�v�̐���
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		// �s�[�v�^�C�v
		desc.NumDescriptors = BACK_BUFFER_COUNT;		// �o�b�t�@��
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// �V�F�[�_�[���\�[�X��
		desc.NodeMask = nodeMask;

		hr = m_pD3DDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pBackBufferHeap.ReleaseAndGetAddressOf()));
	}

	// �����_�[�^�[�Q�b�g�r���[�̐���
	{
		// �擪�A�h���X�ƃI�t�Z�b�g�T�C�Y���擾
		auto handle = m_pBackBufferHeap->GetCPUDescriptorHandleForHeapStart();
		m_nBackBufferSize = m_pD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// MSAA off

		// �o�b�N�o�b�t�@���烌���_�[�^�[�Q�b�g�𐶐�
		for (UINT i = 0; i < BACK_BUFFER_COUNT; ++i)
		{
			// �o�b�N�o�b�t�@�擾
			hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pBackBuffer[i].ReleaseAndGetAddressOf()));
			CHECK_FAILED(hr);

			// �����_�[�^�[�Q�b�g�r���[�̐���
			m_pD3DDevice->CreateRenderTargetView(m_pBackBuffer[i].Get(), &desc, handle);
			// �n���h���̃|�C���^��i�߂�
			handle.ptr += m_nBackBufferSize;
		}
	}

	return hr;
}

/// @brief �I������
void D3D12Renderer::finalize()
{

}

/// @brief ��ʃN���A
void D3D12Renderer::clear()
{
	// �R�}���h�A���P�[�^�[�ƃR�}���h���X�g�����Z�b�g
	m_pCmdAllocator->Reset();
	m_pCmdList->Reset(m_pCmdAllocator.Get(), nullptr);

	// �����_�[�^�[�Q�b�g�n���h���̎擾
	auto handlRTV = m_pBackBufferHeap->GetCPUDescriptorHandleForHeapStart();
	UINT backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	handlRTV.ptr += backBufferIndex * m_nBackBufferSize;

	// �����_�[�^�[�Q�b�g�̃o���A�w��
	D3D12_RESOURCE_BARRIER barrierDesc = {};
	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;					// �o���A���(�J��)
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;						// �o���A�����p
	barrierDesc.Transition.pResource = m_pBackBuffer[backBufferIndex].Get();	// ���\�[�X�|�C���^
	barrierDesc.Transition.Subresource = 										// �T�u���\�[�X�̐�
		D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;								// ���\�[�X���̂��ׂẴT�u���\�[�X�𓯎��Ɉڍs
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;			// �J�ڑO�̃��\�[�X���
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;		// �J�ڌ�̃��\�[�X���
	m_pCmdList->ResourceBarrier(1, &barrierDesc);

	// �����_�[�^�[�Q�b�g�̃Z�b�g
	m_pCmdList->OMSetRenderTargets(1, &handlRTV, FALSE, nullptr);

	// �����_�[�^�[�Q�b�g�̃N���A
	static float a = 0;
	a += 0.1f;
	FLOAT clearColor[] = { sinf(a), 0.58f, 0.92f, 1.0f };
	m_pCmdList->ClearRenderTargetView(handlRTV, clearColor, 0, nullptr);

}

/// @brief ��ʍX�V
void D3D12Renderer::present()
{
	HRESULT hr = S_OK;

	// �����_�[�^�[�Q�b�g�̃o���A�w��
	UINT backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrierDesc = {};
	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;					// �o���A���(�J��)
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;						// �o���A�����p
	barrierDesc.Transition.pResource = m_pBackBuffer[backBufferIndex].Get();	// ���\�[�X�|�C���^
	barrierDesc.Transition.Subresource = 										// �T�u���\�[�X�̐�
		D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;								// ���\�[�X���̂��ׂẴT�u���\�[�X�𓯎��Ɉڍs
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// �J�ڑO�̃��\�[�X���
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// �J�ڌ�̃��\�[�X���
	m_pCmdList->ResourceBarrier(1, &barrierDesc);

	// �R�}���h�̋L�^�I��
	hr = m_pCmdList->Close();
	CHECK_FAILED(hr);

	// �R�}���h�̎��s
	ID3D12CommandList* ppCmdList[] = { m_pCmdList.Get() };
	m_pCmdQueue->ExecuteCommandLists(_countof(ppCmdList), ppCmdList);

	// �R�}���h�����҂�
	hr = m_pCmdQueue->Signal(m_pFence.Get(), ++m_nFenceVal);
	CHECK_FAILED(hr);

	// �t�F���X����
	if (m_pFence->GetCompletedValue() != m_nFenceVal)
	{
		// �C�x���g���s
		auto hEvent = CreateEvent(nullptr, false, false, nullptr);
		hr = m_pFence->SetEventOnCompletion(m_nFenceVal, hEvent);
		CHECK_FAILED(hr);
		// �C�x���g�I���҂�
		WaitForSingleObject(hEvent, INFINITE);
		// �C�x���g�����
		CloseHandle(hEvent);
	}

	// �\��
	m_pSwapChain->Present(1, 0);
	
}
