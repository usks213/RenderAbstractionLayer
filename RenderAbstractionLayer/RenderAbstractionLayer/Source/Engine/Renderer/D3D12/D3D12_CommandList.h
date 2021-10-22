/*****************************************************************//**
 * \file   D3D12_CommandList.h
 * \brief  DiectX12レンダーコマンドリスト
 * 
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/

#ifndef _D3D12_RENDER_CONTEXT_
#define _D3D12_RENDER_CONTEXT_

#include <Renderer/Core/Core_CommandList.h>
#include "D3D12_Shader.h"
#include "D3D12_Material.h"

#include <tuple>
#include <map>

namespace d3d12
{
	// 前定義
	class D3D12Renderer;
	class D3D12Device;

	/// @class D3D12CommandList
	/// @brief DirectX11レンダーコマンドリスト
	class D3D12CommandList final : public core::CoreCommandList
	{
		friend class D3D12Renderer;
	public:
		//------------------------------------------------------------------------------
		// public methods 
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		explicit D3D12CommandList();

		/// @brief デストラクタ(デフォルト)
		~D3D12CommandList() noexcept = default;

		/// @brief 初期化処理
		/// @param pRenderer D3D12レンダラーポインタ
		/// @param pDevice D3D12デバイスポインタ
		/// @return 初期化: 成功 true | 失敗 false
		HRESULT initialize(D3D12Renderer* pRenderer, D3D12Device* pDevice);


		//----- リソース指定命令 -----

		void setMaterial(const core::MaterialID& materialID) override;

		void setRenderBuffer(const core::RenderBufferID& renderBufferID)  override;

		//----- バインド命令 -----

		void bindBuffer(const std::string& bindName, const core::ShaderID& shaderID, const core::BufferID bufferID) override;

		void bindTexture(const std::string& bindName, const core::ShaderID& shaderID, const core::TextureID textureID) override;

		void bindSampler(const std::string& bindName, const core::ShaderID& shaderID, const core::SamplerState sampler) override;

		//----- 描画命令

		void render(const core::RenderBufferID& renderBufferID, std::uint32_t instanceCount = 1)  override;

	private:
		//------------------------------------------------------------------------------
		// private methods 
		//------------------------------------------------------------------------------

		void setCBufferResource(std::uint32_t rootIndex, const core::BufferID& bufferID);

		void setTextureResource(std::uint32_t rootIndex, const core::TextureID& textureID);

		void setSamplerResource(std::uint32_t rootIndex, core::SamplerState state);

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		D3D12Renderer*					m_pRenderer;			///< レンダラー
		D3D12Device*						m_pDevice;			///< デバイス

		ComPtr<ID3D12CommandAllocator>		m_pCmdAllocator;		///< コマンドアロケーター
		ComPtr<ID3D12GraphicsCommandList>	m_pCmdList;			///< コマンドリスト

	};
}

#endif // !_D3D12_RENDER_CONTEXT_
