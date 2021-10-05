/*****************************************************************//**
 * \file   D3D11_DepthStencil.h
 * \brief  DirectX11�f�v�X�X�e���V��
 *
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/

#include "D3D11_DepthStencil.h"
using namespace d3d11;

 /// @brief �R���X�g���N�^
 /// @param device �f�o�C�X
 /// @param desc �����_�[�^�[�Q�b�gDesc
D3D11DepthStencil::D3D11DepthStencil(ID3D11Device1* device, const D3D11DepthStencilDesc& desc) :
	core::CoreDepthStencil(desc.id, desc.name)
{
	// �e�N�X�`��2D
	D3D11_TEXTURE2D_DESC tex = {};
	tex.Format = DXGI_FORMAT_R32_TYPELESS;
	tex.Width = static_cast<UINT>(desc.width);
	tex.Height = static_cast<UINT>(desc.height);
	tex.ArraySize = 1;
	tex.MipLevels = 1;
	tex.CPUAccessFlags = 0;
	tex.Usage = D3D11_USAGE_DEFAULT;
	tex.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	if (desc.isMSAA)
		tex.SampleDesc = desc.sampleDesc;
	else
		tex.SampleDesc.Count = 1;

	CHECK_FAILED(device->CreateTexture2D(
		&tex,
		nullptr,
		m_tex.ReleaseAndGetAddressOf()
	));

	// �[�x�X�e���V���r���[
	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	if (desc.isMSAA) dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	// �[�x�X�e���V���r���[�ǂݏ�����
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	CHECK_FAILED(device->CreateDepthStencilView(
		m_tex.Get(),
		&dsvDesc,
		m_dsv.ReleaseAndGetAddressOf()));

	//// �[�x�X�e���V���r���[
	//dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
	//dsvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	//CHECK_FAILED(device->CreateDepthStencilView(
	//	m_tex.Get(),
	//	&dsvDesc,
	//	m_dsvRO.ReleaseAndGetAddressOf()));

	// �V�F�[�_�[���\�[�X
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT);
	if (desc.isMSAA) srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	CHECK_FAILED(device->CreateShaderResourceView(
		m_tex.Get(),
		&srvDesc,
		m_srv.ReleaseAndGetAddressOf()
	));
}

