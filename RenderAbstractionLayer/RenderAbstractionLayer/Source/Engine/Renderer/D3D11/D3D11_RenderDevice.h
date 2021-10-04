/*****************************************************************//**
 * \file   D3D11_RenderDevice.h
 * \brief  DirectX11�����_�[�f�o�C�X�N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/

#ifndef _D3D11_RENDER_DEVICE_
#define _D3D11_RENDER_DEVICE_

#include <Renderer/Core/Core_RenderDevice.h>
#include "D3D11_CommonState.h"

namespace d3d11
{
	/// @class D3D11RenderDevice
	/// @brief D3D11�����_�[�f�o�C�X
	class D3D11RenderDevice final : public core::CoreRenderDevice
	{
		friend class D3D11Renderer;
		friend class D3D11RenderContext;
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		explicit D3D11RenderDevice();

		/// @brief �f�X�g���N�^
		~D3D11RenderDevice() noexcept = default;

		/// @brief ����������
		/// @param hWnd �E�B���h�E�n���h��
		/// @param width �E�B���h�E�̕�
		/// @param height �E�B���h�E�̍���
		/// @return ������: ���� true | ���s false
		HRESULT initialize(HWND hWnd, UINT width, UINT height);


	private:
		//------------------------------------------------------------------------------
		// private methods
		//------------------------------------------------------------------------------

		/// @brief �f�o�C�X�̐���
		/// @return HRESULT
		HRESULT createDivece();
		/// @brief �X���b�v�`�F�[���ƃo�b�t�@�̐���
		/// @return HRESULT
		HRESULT createSwapChainAndBuffer();
		/// @brief ���ʃX�e�[�g�̐���
		/// @return HRESULT
		HRESULT createCommonState();

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------
		ComPtr<ID3D11Device1>				m_d3dDevice;			///< �f�o�C�X
		ComPtr<IDXGISwapChain1>				m_swapChain;			///< �X���b�v�`�F�[��
		ComPtr<ID3DUserDefinedAnnotation>	m_d3dAnnotation;		///< �A�m�e�[�V����

		ComPtr<ID3D11DeviceContext1>		m_d3dContext;			///< �f�o�C�X�R���e�L�X�g
		ComPtr<ID3D11DeviceContext1>		m_d3dDefferedContext;	///< �x���R���e�L�X�g

		ComPtr<ID3D11Texture2D>				m_backBufferRT;			///< �o�b�N�o�b�t�@
		ComPtr<ID3D11RenderTargetView>		m_backBufferRTV;		///< �o�b�N�o�b�t�@�r���[
		DXGI_FORMAT							m_backBufferFormat;		///<

		ComPtr<ID3D11Texture2D>				m_depthStencilTexture;	///< Z�o�b�t�@
		ComPtr<ID3D11DepthStencilView>		m_depthStencilView;		///< Z�o�b�t�@�r���[
		DXGI_FORMAT							m_depthStencilFormat;	///<

		ComPtr<IDXGIFactory2>				m_dxgiFactory;			///< �t�@�N�g���[
		DXGI_SAMPLE_DESC					m_dxgiMSAA;				///< MSAA�ݒ�
		D3D11_VIEWPORT						m_viewport;				///< �r���[�|�[�g
		HWND								m_hWnd;					///< �E�B���h�E�n���h��

		UINT								m_backBufferCount;
		UINT								m_nOutputWidth;
		UINT								m_nOutputHeight;
		bool								m_bUseMSAA;				///< MSAA�g�p�t���O

		ComPtr<ID3D11RasterizerState>		m_rasterizeStates[(size_t)core::RasterizeState::MAX];		///< ���X�^���C�U�X�e�[�g
		ComPtr<ID3D11SamplerState>			m_samplerStates[(size_t)core::SamplerState::MAX];			///< �T���v���X�e�[�g
		ComPtr<ID3D11BlendState>			m_blendStates[(size_t)core::BlendState::MAX];				///< �u�����h�X�e�[�g
		ComPtr<ID3D11DepthStencilState>		m_depthStencilStates[(size_t)core::DepthStencilState::MAX];	///< �[�x�X�e���V���X�e�[�g

	};
}

#endif // !_D3D11_RENDER_DEVICE_
