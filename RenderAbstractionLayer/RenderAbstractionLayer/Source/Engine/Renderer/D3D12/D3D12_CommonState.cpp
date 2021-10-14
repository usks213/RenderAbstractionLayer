/*****************************************************************//**
 * \file   D3D12_CommonState.h
 * \brief  DirectX12���ʃX�e�[�g
 *
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/

#include "D3D12_CommonState.h"
using namespace d3d12;


/// @brief D3D12�̃��\�[�X�g�p���ʎ擾
/// @param usage ���\�[�X�g�p����
/// @return D3D12���\�[�X�g�p����
//D3D12_USAGE d3d12::getD3D12Usage(core::Usage usage) {
//	switch (usage) {
//	case core::Usage::STATIC:  return D3D12_USAGE_IMMUTABLE;
//	case core::Usage::DEFAULT: return D3D12_USAGE_DEFAULT;
//	case core::Usage::DYNAMIC: return D3D12_USAGE_DYNAMIC;
//	case core::Usage::STAGING: return D3D12_USAGE_STAGING;
//	default:             return D3D12_USAGE_DEFAULT;
//	}
//}

/// @brief D3D12�̃o�C���h�t���O�擾
/// @param flags �o�C���h�t���O
/// @return �����Ȃ������^�o�C���h�t���O
UINT32 d3d12::getD3D12BindFlags(core::BindFlags flags) {
	UINT32 d3d12BindFlags = 0;

	/*d3d12BindFlags = d3d12BindFlags | ((flags & core::BindFlags::VERTEX_BUFFER) ? D3D12_BIND_VERTEX_BUFFER : 0);
	d3d12BindFlags = d3d12BindFlags | ((flags & core::BindFlags::INDEX_BUFFER) ? D3D12_BIND_INDEX_BUFFER : 0);
	d3d12BindFlags = d3d12BindFlags | ((flags & core::BindFlags::CONSTANT_BUFFER) ? D3D12_BIND_CONSTANT_BUFFER : 0);
	d3d12BindFlags = d3d12BindFlags | ((flags & core::BindFlags::SHADER_RESOURCE) ? D3D12_BIND_SHADER_RESOURCE : 0);
	d3d12BindFlags = d3d12BindFlags | ((flags & core::BindFlags::STREAM_OUTPUT) ? D3D12_BIND_STREAM_OUTPUT : 0);
	d3d12BindFlags = d3d12BindFlags | ((flags & core::BindFlags::RENDER_TARGET) ? D3D12_BIND_RENDER_TARGET : 0);
	d3d12BindFlags = d3d12BindFlags | ((flags & core::BindFlags::DEPTH_STENCIL) ? D3D12_BIND_DEPTH_STENCIL : 0);
	d3d12BindFlags = d3d12BindFlags | ((flags & core::BindFlags::UNORDERED_ACCESS) ? D3D12_BIND_UNORDERED_ACCESS : 0);*/

	return d3d12BindFlags;
}

/// @brief D3D12��CPU�A�N�Z�X�t���O�擾
/// @param flags CPU�A�N�Z�X�t���O
/// @return �����Ȃ������^CPU�A�N�Z�X�t���O
UINT32 d3d12::getD3D12CPUAccessFlags(core::CPUAccessFlags flags) {
	UINT32 d3d12CPUAccessFlags = 0;

	//d3d12CPUAccessFlags |= (flags & core::CPUAccessFlags::READ) ? D3D12_CPU_ACCESS_READ : 0;
	//d3d12CPUAccessFlags |= (flags & core::CPUAccessFlags::WRITE) ? D3D12_CPU_ACCESS_WRITE : 0;

	return d3d12CPUAccessFlags;
}

/// @brief D3D12�̃o�b�t�@�t���O�擾
/// @param flags �o�b�t�@�t���O
/// @return �����Ȃ������^�o�b�t�@�t���O
UINT32 d3d12::getD3D12MiscFlags(core::MiscFlags flags) {
	UINT32 d3d12MiscFlags = 0;

	//d3d12MiscFlags |= (flags & core::MiscFlags::GENERATE_MIPS) ? D3D12_RESOURCE_MISC_GENERATE_MIPS : 0;
	//d3d12MiscFlags |= (flags & core::MiscFlags::TEXTURECUBE) ? D3D12_RESOURCE_MISC_TEXTURECUBE : 0;
	//d3d12MiscFlags |= (flags & core::MiscFlags::DRAWINDIRECT_ARGS) ? D3D12_RESOURCE_MISC_DRAWINDIRECT_ARGS : 0;
	//d3d12MiscFlags |= (flags & core::MiscFlags::BUFFER_ALLOW_RAW_VIEWS) ? D3D12_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0;
	//d3d12MiscFlags |= (flags & core::MiscFlags::BUFFER_STRUCTURED) ? D3D12_RESOURCE_MISC_BUFFER_STRUCTURED : 0;
	//d3d12MiscFlags |= (flags & core::MiscFlags::RESOURCE_CLAMP) ? D3D12_RESOURCE_MISC_RESOURCE_CLAMP : 0;
	//d3d12MiscFlags |= (flags & core::MiscFlags::BUFFER_STRUCTURED) ? D3D12_RESOURCE_MISC_BUFFER_STRUCTURED : 0;
	//d3d12MiscFlags |= (flags & core::MiscFlags::SHARED_KEYEDMUTEX) ? D3D12_RESOURCE_MISC_SHARED_KEYEDMUTEX : 0;
	//d3d12MiscFlags |= (flags & core::MiscFlags::TILE_POOL) ? D3D12_RESOURCE_MISC_TILE_POOL : 0;
	//d3d12MiscFlags |= (flags & core::MiscFlags::TILED) ? D3D12_RESOURCE_MISC_TILED : 0;

	return d3d12MiscFlags;
}

/// @brief D3D12�̃v���~�e�B�u�g�|���W�[�擾
/// @param topology �v���~�e�B�u�g�|���W�[
/// @return D3D12�v���~�e�B�u�g�|���W�[
D3D12_PRIMITIVE_TOPOLOGY d3d12::getD3D12PrimitiveTopology(core::PrimitiveTopology topology) {
	static D3D12_PRIMITIVE_TOPOLOGY d3dTopologies[static_cast<size_t>(core::PrimitiveTopology::MAX)] = {
		D3D_PRIMITIVE_TOPOLOGY_UNDEFINED,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
		//D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
	};

	return d3dTopologies[static_cast<size_t>(topology)];
}

/// @brief DXGI�̃e�N�X�`���t�H�[�}�b�g�擾
/// @param format �e�N�X�`���t�H�[�}�b�g
/// @return DXGI�t�H�[�}�b�g
DXGI_FORMAT d3d12::getDXGIFormat(core::TextureFormat format) {
	static DXGI_FORMAT d3dDXGIFormat[static_cast<size_t>(core::TextureFormat::MAX)] = {
	DXGI_FORMAT_UNKNOWN, // �s��

	DXGI_FORMAT_R32G32B32A32_TYPELESS,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32A32_SINT,

	DXGI_FORMAT_R32G32B32_TYPELESS,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32_SINT,

	DXGI_FORMAT_R16G16B16A16_TYPELESS,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R16G16B16A16_SNORM,
	DXGI_FORMAT_R16G16B16A16_SINT,

	DXGI_FORMAT_R32G32_TYPELESS,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_SINT,

	DXGI_FORMAT_R11G11B10_FLOAT,

	DXGI_FORMAT_R8G8B8A8_TYPELESS,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_SNORM,
	DXGI_FORMAT_R8G8B8A8_SINT,

	DXGI_FORMAT_R16G16_TYPELESS,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_R16G16_SINT,

	DXGI_FORMAT_R32_TYPELESS,
	DXGI_FORMAT_D32_FLOAT,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_SINT,

	DXGI_FORMAT_R24G8_TYPELESS,
	DXGI_FORMAT_D24_UNORM_S8_UINT,

	DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT,

	DXGI_FORMAT_R8G8_TYPELESS,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R8G8_UINT,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_R8G8_SINT,

	DXGI_FORMAT_R16_TYPELESS,
	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_D16_UNORM,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R16_SINT,

	DXGI_FORMAT_R8_TYPELESS,
	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8_UINT,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R8_SINT,

	// �u���b�N���k�e�N�X�`��
	DXGI_FORMAT_BC1_TYPELESS,
	DXGI_FORMAT_BC1_UNORM,
	DXGI_FORMAT_BC1_UNORM_SRGB,
	DXGI_FORMAT_BC2_TYPELESS,
	DXGI_FORMAT_BC2_UNORM,
	DXGI_FORMAT_BC2_UNORM_SRGB,
	DXGI_FORMAT_BC3_TYPELESS,
	DXGI_FORMAT_BC3_UNORM,
	DXGI_FORMAT_BC3_UNORM_SRGB,
	DXGI_FORMAT_BC4_TYPELESS,
	DXGI_FORMAT_BC4_UNORM,
	DXGI_FORMAT_BC4_SNORM,
	DXGI_FORMAT_BC5_TYPELESS,
	DXGI_FORMAT_BC5_UNORM,
	DXGI_FORMAT_BC5_SNORM,
	DXGI_FORMAT_BC6H_TYPELESS,
	DXGI_FORMAT_BC6H_UF16,
	DXGI_FORMAT_BC6H_SF16,
	DXGI_FORMAT_BC7_TYPELESS,
	DXGI_FORMAT_BC7_UNORM,
	DXGI_FORMAT_BC7_UNORM_SRGB,
	};

	return d3dDXGIFormat[static_cast<size_t>(format)];
}

