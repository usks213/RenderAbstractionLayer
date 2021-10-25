/*****************************************************************//**
 * \file   D3D12_Device.h
 * \brief  DirectX12�f�o�C�X
 * 
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/

#ifndef _D3D12_RENDER_DEVICE_
#define _D3D12_RENDER_DEVICE_

#include <Renderer/Core/Core_Device.h>
#include "D3D12_CommonState.h"

namespace d3d12
{
	// �O��`
	class D3D12Renderer;
	class D3D12Shader;
	class D3D12Material;
	class D3D12Texture;

	/// @class D3D12Device
	/// @brief D3D12�f�o�C�X
	class D3D12Device final : public core::CoreDevice
	{
		friend class D3D12Renderer;
		friend class D3D12CommandList;
		friend class D3D12Shader;
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		explicit D3D12Device();

		/// @brief �f�X�g���N�^
		~D3D12Device() noexcept = default;

		/// @brief ����������
		/// @param pDevice D3D12�f�o�C�X�|�C���^
		/// @param hWnd �E�B���h�E�n���h��
		/// @param width �E�B���h�E�̕�
		/// @param height �E�B���h�E�̍���
		/// @return ������: ���� true | ���s false
		HRESULT initialize(ID3D12Device* pDevice, IDXGIFactory2* pFactory2, 
			HWND hWnd, UINT width, UINT height);

		//----- ���\�[�X���� -----

		core::BufferID		createBuffer(core::BufferDesc& desc, const core::BufferData* pData = nullptr) override;
		core::DepthStencilID	createDepthStencil(core::TextureDesc& desc) override;
		core::MaterialID		createMaterial(std::string name, core::ShaderID& shaderID) override;
		core::MeshID			createMesh(std::string name) override;
		core::RenderBufferID	createRenderBuffer(core::ShaderID& shaderID, core::MeshID& meshID) override;
		core::RenderTargetID	createRenderTarget(core::TextureDesc& desc) override;
		core::ShaderID		createShader(core::ShaderDesc& desc) override;
		core::TextureID		createTexture(std::string filePath) override;
		core::TextureID		createTexture(core::TextureDesc& desc, const core::TextureData* pData = nullptr) override;

	private:
		//------------------------------------------------------------------------------
		// private methods
		//------------------------------------------------------------------------------

		/// @brief ���ʃX�e�[�g�̐���
		/// @return HRESULT
		HRESULT createCommonState();

		/// @brief D3D12�e�N�X�`������(�N���A�l����)
		/// @param desc �e�N�X�`��Desc
		/// @param pClear �N���A�f�[�^
		/// @return D3D12�e�N�X�`���|�C���^
		D3D12Texture* createD3D12Texture(core::TextureDesc& desc, D3D12_CLEAR_VALUE* pClear);

		/// @brief �p�C�v���C���X�e�[�g����
		/// @param d3d12Shader �V�F�[�_�[
		/// @param d3d12Mat �}�e���A��
		/// @return �p�C�v���C���X�e�[�g
		ID3D12PipelineState* createPipelineState(D3D12Shader& d3d12Shader, D3D12Material& d3d12Mat);

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		ID3D12Device*						m_pD3DDevice;			///< �f�o�C�X�|�C���^

		//ComPtr<IDXGISwapChain1>				m_swapChain;			///< �X���b�v�`�F�[��

		//ComPtr<ID3D12Texture2D>				m_backBufferRT;			///< �o�b�N�o�b�t�@
		//ComPtr<ID3D12RenderTargetView>		m_backBufferRTV;		///< �o�b�N�o�b�t�@�r���[
		DXGI_FORMAT							m_backBufferFormat;		///< �o�b�t�@�o�b�t�@�t�H�[�}�b�g

		//ComPtr<ID3D12Texture2D>				m_depthStencilTexture;	///< Z�o�b�t�@
		//ComPtr<ID3D12DepthStencilView>		m_depthStencilView;		///< Z�o�b�t�@�r���[
		DXGI_FORMAT							m_depthStencilFormat;	///< Z�o�b�t�@�t�H�[�}�b�g

		HWND									m_hWnd;					///< �E�B���h�E�n���h��
		D3D12_VIEWPORT						m_viewport;				///< �r���[�|�[�g

		UINT									m_backBufferCount;		///< �o�b�N�o�b�t�@��
		UINT									m_nOutputWidth;			///< �o�̓T�C�Y(��)
		UINT									m_nOutputHeight;		///< �o�̓T�C�Y(����)

		// �X�e�[�g
		D3D12_RASTERIZER_DESC              	m_rasterizeStates[(size_t)core::RasterizeState::MAX];		///< ���X�^���C�U�X�e�[�g
		D3D12_BLEND_DESC                   	m_blendStates[(size_t)core::BlendState::MAX];				///< �u�����h�X�e�[�g
		D3D12_DEPTH_STENCIL_DESC           	m_depthStencilStates[(size_t)core::DepthStencilState::MAX];	///< �[�x�X�e���V���X�e�[�g

		// �T���v���[
		D3D12_STATIC_SAMPLER_DESC 			m_staticSamplers[(size_t)core::SamplerState::MAX];			///< �ÓI�T���v���X�e�[�g
		D3D12_GPU_DESCRIPTOR_HANDLE			m_dynamicSamplers[(size_t)core::SamplerState::MAX];			///< �_�C�i�b�N�T���v���[
		ComPtr<ID3D12DescriptorHeap>			m_pSamplerHeap;											///< �T���v���[�q�[�v

		// �O���t�B�N�X�p�C�v���C���X�e�[�g
		std::unordered_map<core::MaterialID, ComPtr<ID3D12PipelineState>>	m_pPipelineState;

	};
}

#endif // !_D3D12_RENDER_DEVICE_
