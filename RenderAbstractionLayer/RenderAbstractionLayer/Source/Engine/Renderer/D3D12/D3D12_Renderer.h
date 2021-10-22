/*****************************************************************//**
 * \file   D3D12_Renderer.h
 * \brief  DirectX12�����_���[
 * 
 * \author USAMI KOSHI
 * \date   2021/10/08
 *********************************************************************/
#ifndef _D3D12_RENDERER_
#define _D3D12_RENDERER_

#include <Renderer\Core\Core_Renderer.h>
#include "D3D12_Device.h"
#include "D3D12_CommandList.h"

namespace d3d12
{
	class D3D12Renderer final : public core::CoreRenderer
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		explicit D3D12Renderer();
		/// @brief �f�X�g���N�^
		~D3D12Renderer() noexcept = default;

		/// @brief ����������
		/// @param hWnd �E�B���h�E�n���h��
		/// @param width �E�B���h�E�̕�
		/// @param height �E�B���h�E�̍���
		/// @return ������: ���� true | ���s false
		HRESULT initialize(HWND hWnd, UINT width, UINT height);

		/// @brief �I������
		void finalize() override;

		/// @brief ��ʃN���A
		void clear() override;
		/// @brief ��ʍX�V
		void present() override;

		/// @brief �f�o�C�X�̎擾
		/// @return �f�o�C�X�̃|�C���^
		core::CoreDevice* getDevice() override
		{
			return &m_device;
		}

		/// @brief �R�}���h���X�g�̎擾
		/// @return �R�}���h���X�g�̃|�C���^ 
		core::CoreCommandList* getContext() override
		{
			if (m_useCmdListCnt >= m_cmdLists.size())
			{
				auto up = std::make_unique<D3D12CommandList>();
				auto* ptr = up.get();
				ptr->initialize(this, &m_device);
				m_cmdLists.push_back(std::move(up));
				++m_useCmdListCnt;
				return ptr;
			}

			return m_cmdLists[m_useCmdListCnt++].get();
		}

		/// @brief �R�s�[�R���X�g���N�^�폜
		D3D12Renderer(const D3D12Renderer&) = delete;
		/// @brief ���[�u�R���X�g���N�^�폜
		D3D12Renderer(D3D12Renderer&&) = delete;

	private:
		//------------------------------------------------------------------------------
		// private methods
		//------------------------------------------------------------------------------

	private:
	public:
		//------------------------------------------------------------------------------
		// private variables
		//------------------------------------------------------------------------------

		D3D12Device						m_device;				///< �f�o�C�X�N���X
		// �R�}���h�L���[

		std::vector<std::unique_ptr<D3D12CommandList>>	m_cmdLists;		///< �R�}���h���X�g�z��
		std::uint32_t								m_useCmdListCnt;	///< �g�p����Ă���R�}���h���X�g��

		// d3d12 system param
		ComPtr<ID3D12Device>				m_pD3DDevice;
		ComPtr<IDXGIFactory6>				m_pDXGIFactory;
		ComPtr<IDXGISwapChain4>			m_pSwapChain;

		D3D12_VIEWPORT					m_viewport;
		D3D12_RECT						m_scissorrect;

		// d3d12 fence param
		ComPtr<ID3D12Fence>				m_pFence;
		UINT64							m_nFenceVal;

		// d3d12 command param
		ComPtr<ID3D12CommandQueue>		m_pCmdQueue;
		ComPtr<ID3D12CommandAllocator>		m_pCmdAllocator;		///< �R�}���h�A���P�[�^�[
		ComPtr<ID3D12GraphicsCommandList>	m_pCmdList;			///< �R�}���h���X�g

		// d3d12 rtv param
		static constexpr UINT				BACK_BUFFER_COUNT = 2;
		ComPtr<ID3D12DescriptorHeap>		m_pBackBufferHeap;
		ComPtr<ID3D12Resource>			m_pBackBuffer[BACK_BUFFER_COUNT];
		UINT								m_nBackBufferSize;

		// d3d12 dsv param
		ComPtr<ID3D12DescriptorHeap>		m_pDepthStencilHeap;
		ComPtr<ID3D12Resource>			m_pDepthStencil;

#ifdef _DEBUG
		// d3d12 debug
		ComPtr<ID3D12Debug>					m_pDebugLater;
#endif // _DEBUG

	};
}

#endif // !_D3D12_RENDERER_

