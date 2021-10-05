/*****************************************************************//**
 * \file   D3D11_Texture.h
 * \brief  DirectX11�e�N�X�`��
 * 
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/
#ifndef _D3D11_TEXTURE_
#define _D3D11_TEXTURE_

#include <Renderer/Core/Core_Texture.h>
#include "D3D11_Defines.h"

namespace d3d11
{
	/// @class D3D11Texture
	/// @brief DirectX11�e�N�X�`��
	class D3D11Texture final : public core::CoreTexture
	{
	public:
		/// @brief �R���X�g���N�^
		explicit D3D11Texture(const core::TextureID& id, const std::string& path);

		/// @brief �f�X�g���N�^
		~D3D11Texture() = default;

		/// @brief �t�@�C������e�N�X�`���̓ǂݍ���
		HRESULT CreateFromFile(ID3D11Device1* device, ID3D11DeviceContext1* context);
		/// @brief ����������e�N�X�`���̓ǂݍ���
		HRESULT CreateFromMemory(ID3D11Device1* device, const uint8_t* wicData, size_t wicDataSize);

		ComPtr<ID3D11Texture2D>          m_tex; // �e�N�X�`��
		ComPtr<ID3D11ShaderResourceView> m_srv; // �V�F�[�_���\�[�X�r���[

	private:

		//HRESULT CreateTextureFromFile(_In_ ID3D11Device1* d3dDevice,
		//	_In_ ID3D11DeviceContext1* context,
		//	_In_z_ const wchar_t* szFileName,
		//	_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
		//);

		//HRESULT CreateTextureFromFile(_In_ ID3D11Device1* d3dDevice,
		//	_In_ ID3D11DeviceContext1* context,
		//	_In_z_ const char* szFileName,
		//	_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
		//);

		//HRESULT CreateTextureFromMemory(_In_ ID3D11Device1* d3dDevice,
		//	_In_bytecount_(wicDataSize) const uint8_t* wicData,
		//	_In_ size_t wicDataSize,
		//	_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
		//);
	};
}

#endif // !_D3D11_TEXTURE_
