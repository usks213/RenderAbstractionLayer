/*****************************************************************//**
 * \file   D3D11_DepthStencil.h
 * \brief  DirectX11�f�v�X�X�e���V��
 * 
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/
#ifndef _D3D11_DEPTH_STENCIL_
#define _D3D11_DEPTH_STENCIL_

#include <Renderer/Core/Core_DepthStencil.h>
#include <Renderer/D3D11/D3D11_Defines.h>

namespace d3d11
{
	/// @struct D3D11DepthStencilDesc
	/// @brief DreictX11�[�x�X�e���V��Desc
	struct D3D11DepthStencilDesc
	{
	public:
		//------------------------------------------------------------------------------
		// public methods 
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		D3D11DepthStencilDesc() :
			id(core::NONE_DEPTH_STENCIL_ID),
			name(),
			width(0.0f),
			height(0.0f),
			format(DXGI_FORMAT_D32_FLOAT),
			isMSAA(false),
			sampleDesc({ 1,0 })
		{}

	public:
		//------------------------------------------------------------------------------
		// public variables 
		//------------------------------------------------------------------------------

		core::DepthStencilID id;
		std::string name;
		float width;
		float height;
		DXGI_FORMAT format;
		bool isMSAA;
		DXGI_SAMPLE_DESC sampleDesc;
	};

	/// @class D3D11DepthStencil
	/// @brief DreictX11�[�x�X�e���V��
	class D3D11DepthStencil : public core::CoreDepthStencil
	{
	public:
		//------------------------------------------------------------------------------
		// public methods 
		//------------------------------------------------------------------------------

		/// @brief �R���X�g���N�^
		/// @param device �f�o�C�X
		/// @param desc �[�x�X�e���V��Desc
		explicit D3D11DepthStencil(ID3D11Device1* device, const D3D11DepthStencilDesc& desc);

		/// @brief �f�X�g���N�^
		~D3D11DepthStencil() = default;

	public:
		//------------------------------------------------------------------------------
		// public variables 
		//------------------------------------------------------------------------------

		/// @brief D3D11�e�N�X�`��
		ComPtr<ID3D11Texture2D>				m_tex;
		/// @brief D3D11�V�F�[�_�[���\�[�X�r���[
		ComPtr<ID3D11ShaderResourceView>	m_srv;
		/// @brief D3D11�[�x�X�e���V���r���[
		ComPtr<ID3D11DepthStencilView>		m_dsv;
	};
}

#endif // !_D3D11_DEPTH_STENCIL_
