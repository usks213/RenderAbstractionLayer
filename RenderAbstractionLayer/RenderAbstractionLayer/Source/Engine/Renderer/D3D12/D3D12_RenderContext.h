/*****************************************************************//**
 * \file   D3D12_RenderContext.h
 * \brief  DiectX12レンダーコンテキスト
 * 
 * \author USAMI KOSHI
 * \date   2021/10/13
 *********************************************************************/

#ifndef _D3D12_RENDER_CONTEXT_
#define _D3D12_RENDER_CONTEXT_

#include <Renderer/Core/Core_RenderContext.h>
#include "D3D12_Shader.h"
#include "D3D12_Material.h"

#include <tuple>

namespace d3d12
{
	// 前定義
	class D3D12Renderer;
	class D3D12RenderDevice;

	/// @class D3D12RenderContext
	/// @brief DirectX11レンダーコンテキスト
	class D3D12RenderContext final : public core::CoreRenderContext
	{
		friend class D3D12Renderer;
	public:
		//------------------------------------------------------------------------------
		// public methods 
		//------------------------------------------------------------------------------

		/// @brief コンストラクタ
		explicit D3D12RenderContext();

		/// @brief デストラクタ(デフォルト)
		~D3D12RenderContext() noexcept = default;

		/// @brief 初期化処理
		/// @param pRenderer D3D12レンダラーポインタ
		/// @param pDevice D3D12デバイスポインタ
		/// @return 初期化: 成功 true | 失敗 false
		HRESULT initialize(D3D12Renderer* pRenderer, D3D12RenderDevice* pDevice);


		//----- リソース指定命令 -----

		void setPipelineState(const core::MaterialID& materialID, const core::RenderBufferID& renderBufferID);

		void setMaterial(const core::MaterialID& materialID) override;

		void setRenderBuffer(const core::RenderBufferID& renderBufferID)  override;

		void setTexture(std::uint32_t slot, const core::TextureID& textureID, core::ShaderStage stage)  override;

		void setSampler(std::uint32_t slot, core::SamplerState state, core::ShaderStage stage)  override;

		void setPrimitiveTopology(core::PrimitiveTopology topology)  override;

		//----- バッファ指定命令 -----

		void sendSystemBuffer(const core::SHADER::SystemBuffer& systemBuffer)  override;

		void sendTransformBuffer(const Matrix& mtxWorld)  override;

		void sendAnimationBuffer(std::vector<Matrix>& mtxBones)  override;

		void sendLightBuffer(std::vector<core::CorePointLight>& pointLights,
			std::vector<core::CoreSpotLight>& spotLights)  override;

		//----- 描画命令

		void render(const core::RenderBufferID& renderBufferID)  override;

	private:
		//------------------------------------------------------------------------------
		// private methods 
		//------------------------------------------------------------------------------

		void setMaterialResource(const D3D12Material& d3dMaterial, const D3D12Shader& d3dShader);

	private:
		//------------------------------------------------------------------------------
		// private variables 
		//------------------------------------------------------------------------------

		D3D12Renderer*						m_pRenderer;			///< レンダラー
		D3D12RenderDevice*					m_pDevice;				///< デバイス

		// コマンドリスト
		ID3D12GraphicsCommandList*			m_pCmdList;

		// グラフィクスパイプラインステート
		std::unordered_map<std::tuple<core::MaterialID, core::RenderBufferID>, 
			ComPtr<ID3D12PipelineState>>	m_pPipelineState;

		//----- シェーダーバッファ
		//ComPtr<ID3D12Buffer>				m_systemBuffer;
		//ComPtr<ID3D12Buffer>				m_transformBuffer;
		//ComPtr<ID3D12Buffer>				m_animationBuffer;

		//ComPtr<ID3D12Buffer>				m_pointLightBuffer;
		//ComPtr<ID3D12Buffer>				m_spotLightBuffer;
		//ComPtr<ID3D12ShaderResourceView>	m_pointLightSRV;
		//ComPtr<ID3D12ShaderResourceView>	m_spotLightSRV;
	};
}

#endif // !_D3D12_RENDER_CONTEXT_
