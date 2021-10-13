/*****************************************************************//**
 * \file   D3D12_CommonState.h
 * \brief  DirectX12���ʃX�e�[�g
 * 
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/
#ifndef _D3D12_COMMON_STATE_
#define _D3D12_COMMON_STATE_

#include <Renderer/Core/Core_CommonState.h>
#include "D3D12_Defines.h"

namespace d3d12
{
    /// @brief D3D12�̃��\�[�X�g�p���ʎ擾
    /// @param usage ���\�[�X�g�p����
    /// @return D3D12���\�[�X�g�p����
    D3D12_USAGE getD3D12Usage(core::Usage usage);

    /// @brief D3D12�̃o�C���h�t���O�擾
    /// @param flags �o�C���h�t���O
    /// @return �����Ȃ������^�o�C���h�t���O
    UINT32 getD3D12BindFlags(core::BindFlags flags);

    /// @brief D3D12��CPU�A�N�Z�X�t���O�擾
    /// @param flags CPU�A�N�Z�X�t���O
    /// @return �����Ȃ������^CPU�A�N�Z�X�t���O
    UINT32 getD3D12CPUAccessFlags(core::CPUAccessFlags flags);

    /// @brief D3D12�̃o�b�t�@�t���O�擾
    /// @param flags �o�b�t�@�t���O
    /// @return �����Ȃ������^�o�b�t�@�t���O
    UINT32 getD3D12MiscFlags(core::MiscFlags flags);

    /// @brief D3D12�̃v���~�e�B�u�g�|���W�[�擾
    /// @param topology �v���~�e�B�u�g�|���W�[
    /// @return D3D12�v���~�e�B�u�g�|���W�[
    D3D12_PRIMITIVE_TOPOLOGY getD3D12PrimitiveTopology(core::PrimitiveTopology topology);

    /// @brief DXGI�̃e�N�X�`���t�H�[�}�b�g�擾
    /// @param format �e�N�X�`���t�H�[�}�b�g
    /// @return DXGI�t�H�[�}�b�g
    DXGI_FORMAT getDXGIFormat(core::TextureFormat format);
}

#endif // !_D3D12_COMMON_STATE_

