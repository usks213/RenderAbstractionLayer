/*****************************************************************//**
 * \file   D3D12_Material.h
 * \brief  DirectX12マテリアル
 *
 * \author USAMI KOSHI
 * \date   2021/10/11
 *********************************************************************/

#include "D3D12_Material.h"
using namespace d3d12;

/// @brief コンストラクタ
/// @param device デバイス
/// @param id マテリアルID
/// @param name マテリアル名
/// @param shader シェーダ
D3D12Material::D3D12Material(ID3D12Device* device, const core::MaterialID& id,
	const std::string& name, const core::CoreShader& shader) :
	core::CoreMaterial(id,name,shader)
{
	// ヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		// ピープタイプ
	heapDesc.NumDescriptors = m_cbufferCount;					// バッファ数
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// シェーダーリソースか
	heapDesc.NodeMask = 0;	// 今は0
	CHECK_FAILED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_pCBufferHeap.ReleaseAndGetAddressOf())));

	// ヒーププロパテェの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type					= D3D12_HEAP_TYPE_UPLOAD;			// ピープのタイプ
	prop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// ページング設定
	prop.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;		// メモリプール設定
	prop.CreationNodeMask		= 1;								// 今は1
	prop.VisibleNodeMask		= 1;								// 今は1

	// リソースの設定
	D3D12_RESOURCE_DESC d3dDesc = {};
	d3dDesc.Dimension			= D3D12_RESOURCE_DIMENSION_BUFFER;	// リソースの種別
	d3dDesc.Alignment			= 0;								// 配置指定？
	d3dDesc.Width				= 0;								// 後で個別指定
	d3dDesc.Height				= 1;								// 高さは１
	d3dDesc.DepthOrArraySize	= 1;								// 3Dはリソースの深さ、1D,2Dの配列は配列サイズ
	d3dDesc.MipLevels			= 1;								// バッファは１
	d3dDesc.Format				= DXGI_FORMAT_UNKNOWN;				// バッファはUNKNOWN
	d3dDesc.SampleDesc			= DXGI_SAMPLE_DESC{ 1, 0 };			// バッファはデフォルト
	d3dDesc.Layout				= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	// とりあえずこれ！
	d3dDesc.Flags				= D3D12_RESOURCE_FLAG_NONE;			// CBufferはなし

	// ヒープハンドル
	auto handle = m_pCBufferHeap->GetCPUDescriptorHandleForHeapStart();
	auto cbufferIncreSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// コンスタントバッファの確保(GPU)
	for (core::ShaderStage stage = core::ShaderStage::VS; stage < core::ShaderStage::MAX; ++stage)
	{
		//--- CBuffer確保
		auto stageIndex = static_cast<size_t>(stage);
		for (auto& cbData : m_cbufferData[stageIndex])
		{
			// CBufferサイズ
			d3dDesc.Width	= cbData.second.size;
			// リソース生成
			device->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&d3dDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(m_d3dCbuffer[stageIndex][cbData.first].GetAddressOf())
			);
			// 初期更新なし
			cbData.second.isUpdate = true;

			// 定数バッファビューの作成
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbufferDesc = {};
			cbufferDesc.BufferLocation = m_d3dCbuffer[stageIndex][cbData.first]->GetGPUVirtualAddress();
			cbufferDesc.SizeInBytes = cbData.second.size;
			device->CreateConstantBufferView(&cbufferDesc, handle);

			// ハンドルポインタ加算
			handle.ptr += cbufferIncreSize;
		}
	}
}
