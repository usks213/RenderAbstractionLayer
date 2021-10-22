/*****************************************************************//**
 * \file   D3D11_Renderer.h
 * \brief  DirectX11�����_���[�N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/10/01
 *********************************************************************/

#ifndef _D3D11_RENDERER_
#define _D3D11_RENDERER_

#include <Renderer/Core/Core_Renderer.h>
#include <Renderer/D3D11/D3D11_Device.h>
#include <Renderer/D3D11/D3D11_CommandList.h>

namespace d3d11
{
	/// @brief D3D11�����_���[
	/// @class D3D11Renderer
	class D3D11Renderer final : public core::CoreRenderer
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		explicit D3D11Renderer();
		/// @brief �f�X�g���N�^
		~D3D11Renderer() noexcept = default;

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
			return &m_cmdList;
		}

		/// @brief �R�s�[�R���X�g���N�^�폜
		D3D11Renderer(const D3D11Renderer&) = delete;
		/// @brief ���[�u�R���X�g���N�^�폜
		D3D11Renderer(D3D11Renderer&&) = delete;

	private:
		//------------------------------------------------------------------------------
		// private methods
		//------------------------------------------------------------------------------

		/// @brief �f�o�C�X�ƃR�}���h���X�g�̐���
		/// @return HRESULT
		HRESULT createDiveceAndContext(HWND hWnd);

	private:
		//------------------------------------------------------------------------------
		// private variables
		//------------------------------------------------------------------------------

		D3D11Device					m_device;				///< �f�o�C�X�N���X
		D3D11CommandList					m_cmdList;				///< �R���e�X�g�N���X

		ComPtr<ID3D11Device1>				m_d3dDevice;			///< �f�o�C�X
		ComPtr<ID3DUserDefinedAnnotation>	m_d3dAnnotation;		///< �A�m�e�[�V����
		ComPtr<ID3D11DeviceContext1>		m_d3dContext;			///< �f�o�C�X�R�}���h���X�g
		ComPtr<ID3D11DeviceContext1>		m_d3dDefferedContext;	///< �x���R�}���h���X�g

		ComPtr<IDXGIFactory2>				m_dxgiFactory;			///< �t�@�N�g���[

	};
}

#endif // !_D3D11_RENDERER_
