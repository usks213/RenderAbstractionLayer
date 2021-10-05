/*****************************************************************//**
 * \file   D3D11_Buffer.h
 * \brief  DirectX11�o�b�t�@�[
 *
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/

#include "D3D11_Buffer.h"
using namespace d3d11;

 /// @brief �R���X�g���N�^
 /// @param device �f�o�C�X
 /// @param id �o�b�t�@ID
 /// @param desc �o�b�t�@Desc
 /// @param data �������f�[�^
D3D11Buffer::D3D11Buffer(ID3D11Device1* device, const core::BufferID& id, const core::BufferDesc& desc, const core::BufferData* data) :
	core::CoreBuffer(id, desc)
{
	
}
