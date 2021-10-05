/*****************************************************************//**
 * \file   D3D11_Buffer.h
 * \brief  DirectX11バッファー
 *
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/

#include "D3D11_Buffer.h"
using namespace d3d11;

 /// @brief コンストラクタ
 /// @param device デバイス
 /// @param id バッファID
 /// @param desc バッファDesc
 /// @param data 初期化データ
D3D11Buffer::D3D11Buffer(ID3D11Device1* device, const core::BufferID& id, const core::BufferDesc& desc, const core::BufferData* data) :
	core::CoreBuffer(id, desc)
{
	
}
