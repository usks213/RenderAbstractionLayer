/*****************************************************************//**
 * \file   D3D11_RenderTarget.h
 * \brief  DirectX11レンダーターゲット
 * 
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/
#ifndef _D3D11_RENDER_TARGET_
#define _D3D11_RENDER_TARGET_

#include <Renderer/Core/Core_RenderTarget.h>
#include "D3D11_Defines.h"

namespace d3d11
{
	/// @brief DreictX11レンダーターゲットDesc
	struct D3D11RenderTargetDesc
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		D3D11RenderTargetDesc() :
			id(core::NONE_RENDER_TARGET_ID),
			name(),
			width(0.0f),
			height(0.0f),
			format(DXGI_FORMAT_B8G8R8A8_UNORM),
			isMSAA(false),
			sampleDesc({ 1,0 })
		{}

	public:
		//------------------------------------------------------------------------------
		// public variables
		//------------------------------------------------------------------------------

		core::RenderTargetID id;
		std::string name;
		float width;
		float height;
		DXGI_FORMAT format;
		bool isMSAA;
		DXGI_SAMPLE_DESC sampleDesc;
	};

	class D3D11RenderTarget final : public core::CoreRenderTarget
	{
	public:
		//------------------------------------------------------------------------------
		// public methods
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		/// @param device デバイス
		/// @param desc レンダーターゲットDesc
		explicit D3D11RenderTarget(ID3D11Device1* device, const D3D11RenderTargetDesc& desc);
		/// @brief デストラクタ
		~D3D11RenderTarget() = default;

	public:
		//------------------------------------------------------------------------------
		// public variables
		//------------------------------------------------------------------------------

		/// @brief D3D11テクスチャ
		ComPtr<ID3D11Texture2D>				m_tex;
		/// @brief D3D11シェーダーリソースビュー
		ComPtr<ID3D11ShaderResourceView>	m_srv;
		/// @brief D3D11レンダーターゲットビュー
		ComPtr<ID3D11RenderTargetView>		m_rtv;
	};
}

#endif // !_D3D11_RENDER_TARGET_
