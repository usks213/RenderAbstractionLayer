/*****************************************************************//**
 * \file   D3D12_Renderer.h
 * \brief  DirectX12レンダラー
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

		/// @brief コンストラクタ
		explicit D3D12Renderer();
		/// @brief デストラクタ
		~D3D12Renderer() noexcept = default;

		/// @brief 初期化処理
		/// @param hWnd ウィンドウハンドル
		/// @param width ウィンドウの幅
		/// @param height ウィンドウの高さ
		/// @return 初期化: 成功 true | 失敗 false
		HRESULT initialize(HWND hWnd, UINT width, UINT height);

		/// @brief 終了処理
		void finalize() override;

		/// @brief 画面クリア
		void clear() override;
		/// @brief 画面更新
		void present() override;

		/// @brief デバイスの取得
		/// @return デバイスのポインタ
		core::CoreDevice* getDevice() override
		{
			return &m_device;
		}

		/// @brief コマンドリストの取得
		/// @return コマンドリストのポインタ 
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

		/// @brief コピーコンストラクタ削除
		D3D12Renderer(const D3D12Renderer&) = delete;
		/// @brief ムーブコンストラクタ削除
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

		D3D12Device						m_device;				///< デバイスクラス
		// コマンドキュー

		std::vector<std::unique_ptr<D3D12CommandList>>	m_cmdLists;		///< コマンドリスト配列
		std::uint32_t								m_useCmdListCnt;	///< 使用されているコマンドリスト数

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
		ComPtr<ID3D12CommandAllocator>		m_pCmdAllocator;		///< コマンドアロケーター
		ComPtr<ID3D12GraphicsCommandList>	m_pCmdList;			///< コマンドリスト

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

