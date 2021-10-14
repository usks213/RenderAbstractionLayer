/*****************************************************************//**
 * \file   D3D12_Shader.h
 * \brief  DirectX12�V�F�[�_�[�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/10/11
 *********************************************************************/

#include "D3D12_Shader.h"
#include <Renderer/Core/Core_ShaderResource.h>
#include <d3dcompiler.h>
#include <fstream>
#include <iostream>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

using namespace d3d12;

namespace {
	/// @brief �V�F�[�_�t�@�C���p�X
	const char* SHADER_FILE_PATH = "Source/Shader/";

	/// @brief �G���g���[�|�C���g�A�t�@�C�����ʎq
	const char* SHADER_TYPES[] = {
		"VS","GS","DS","HS","PS","CS",
	};
	/// @brief HLSL�o�[�W����
	const char* HLSL_VERS[] = {
		"vs_5_1",
		"gs_5_1",
		"ds_5_1",
		"hs_5_1",
		"ps_5_1",
		"cs_5_1",
	};

	constexpr D3D12_SHADER_VISIBILITY SHADER_VISIBILITYS[static_cast<size_t>(core::ShaderStage::CS)] = {
		D3D12_SHADER_VISIBILITY_VERTEX,
		D3D12_SHADER_VISIBILITY_HULL,
		D3D12_SHADER_VISIBILITY_DOMAIN,
		D3D12_SHADER_VISIBILITY_GEOMETRY,
		D3D12_SHADER_VISIBILITY_PIXEL,
	};
}

/// @brief  �R���X�g���N�^
/// @param device �f�o�C�X
/// @param desc �V�F�[�_���
D3D12Shader::D3D12Shader(ID3D12Device* device, core::ShaderDesc desc, const core::ShaderID& id) :
	core::CoreShader(desc, id),
	m_pShaderBlob(),
	m_inputElementDesc()
{
	// ������
	for (auto& shader : m_pShaderBlob) {
		shader = nullptr;
	}

	// �R���p�C�������V�F�[�_�f�[�^
	auto& blobs = m_pShaderBlob;
	// �V�F�[�_���t���N�V����
	std::vector<ComPtr<ID3D12ShaderReflection>>	reflections(static_cast<size_t>(core::ShaderStage::MAX));
	// �V�F�[�_���ꎞ�i�[�p
	D3D12_SHADER_DESC							shaderDesc = {};


	// �V�F�[�_�X�e�[�W�������R���p�C��������
	for (auto stage = core::ShaderStage::VS; stage < core::ShaderStage::MAX; ++stage)
	{
		// �X�e�[�W���Ȃ��ꍇ�̓X�L�b�v
		if (!hasStaderStage(desc.m_stages, stage)) continue;

		ComPtr<ID3DBlob>& blob = blobs[static_cast<size_t>(stage)];
		ComPtr<ID3D12ShaderReflection>& reflection = reflections[static_cast<size_t>(stage)];

		// �p�X
		std::string filepath = SHADER_FILE_PATH + desc.m_name + "_" + 
			SHADER_TYPES[static_cast<size_t>(stage)] + ".hlsl";
		std::wstring wfilepath = std::wstring(filepath.begin(), filepath.end());
		// �G���[
		ID3DBlob* d3dError = nullptr;
		// �X�g���[��
		std::ifstream ifsShader(filepath, std::ios::in | std::ios::binary | std::ios::ate);
		if (ifsShader.good())
		{
			// �\�[�X�t�@�C������R���p�C��
			CHECK_FAILED(D3DCompileFromFile(
				wfilepath.c_str(),
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				SHADER_TYPES[static_cast<size_t>(stage)],
				HLSL_VERS[static_cast<size_t>(stage)],
				D3DCOMPILE_ENABLE_STRICTNESS,
				0,
				blob.GetAddressOf(),
				&d3dError));
			// �G���[���b�Z�[�W!!


			// �ǂݍ��ݐ���
			if (blob)
			{
				// �V�F�[�_���t���N�V�����擾
				void** ppBuffer = reinterpret_cast<void**>(reflection.GetAddressOf());
				CHECK_FAILED(D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(),
					IID_ID3D12ShaderReflection, ppBuffer));
			}

		}
		else
		{
			// �V�F�[�_�t�@�C���Ȃ�!!

		}
	}

	// ���_�V�F�[�_������ꍇ�̓C���v�b�g���C�A�E�g���쐬
	auto& vsReflection = reflections[static_cast<size_t>(core::ShaderStage::VS)];
	if (vsReflection)
	{
		auto& vsBlob = blobs[static_cast<size_t>(core::ShaderStage::VS)];
		vsReflection->GetDesc(&shaderDesc);
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayouts(shaderDesc.InputParameters);

		// ���̓��C�A�E�g����
		m_inputLayoutVariableList.resize(shaderDesc.InputParameters);
		for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC signature;
			vsReflection->GetInputParameterDesc(i, &signature);

			// �C���v�b�g���C�A�E�g�v�f
			D3D12_INPUT_ELEMENT_DESC inputDesc;
			inputDesc.SemanticName = signature.SemanticName;
			inputDesc.SemanticIndex = signature.SemanticIndex;
			//inputDesc.Format = ��..
			inputDesc.InputSlot = 0;
			inputDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			inputDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			inputDesc.InstanceDataStepRate = 0;

			// ���̓��C�A�E�g���
			m_inputLayoutVariableList[i].semanticName = inputDesc.SemanticName;
			inputDesc.SemanticName = m_inputLayoutVariableList[i].semanticName.c_str();
			m_inputLayoutVariableList[i].semanticIndex = inputDesc.SemanticIndex;
			if (i > 0)
			{
				m_inputLayoutVariableList[i].offset =
					static_cast<std::size_t>(m_inputLayoutVariableList[i - 1].formatSize) * sizeof(float) +
					m_inputLayoutVariableList[i - 1].offset;
			}

			// �r�b�g�}�X�N�Ńt�H�[�}�b�g�𕪂���
			if (signature.Mask == 0x01) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32 : inputDesc.Format = DXGI_FORMAT_R32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32 : inputDesc.Format = DXGI_FORMAT_R32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
				
			}
			else if (signature.Mask <= 0x03) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32: inputDesc.Format = DXGI_FORMAT_R32G32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32: inputDesc.Format = DXGI_FORMAT_R32G32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32G32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32G32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
			}
			else if (signature.Mask <= 0x07) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32G32B32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
			}
			else if (signature.Mask <= 0x0F) {
				switch (signature.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;  break;
				case D3D_REGISTER_COMPONENT_SINT32: inputDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;  break;
				case D3D_REGISTER_COMPONENT_FLOAT32: inputDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
				}
				m_inputLayoutVariableList[i].formatSize = InputLayoutVariable::FormatSize::R32G32B32A32;
				m_inputLayoutVariableList[i].formatType =
					static_cast<InputLayoutVariable::FormatType>(signature.ComponentType);
			}

			// ���̓��C�A�E�g�쐬�p���i�[
			inputLayouts[i] = inputDesc;
		}

		// ���̓��C�A�E�g���i�[
		if (!inputLayouts.empty()){

			m_inputElementDesc = inputLayouts;
		}
	}

	// �R���X�^���g�E�e�N�X�`���E�T���v���̍쐬
	D3D12_SHADER_BUFFER_DESC shaderBufferDesc = {};
	for (auto stage = core::ShaderStage::VS; stage < core::ShaderStage::MAX; ++stage)
	{
		const auto& stageIndex = static_cast<size_t>(stage);
		const auto& reflection = reflections[stageIndex];
		if (!reflection) continue;
		// �V�F�[�_���擾
		reflection->GetDesc(&shaderDesc);

		// ���t���N�V��������R���X�^���g�o�b�t�@�ǂݍ���
		std::uint32_t slotOffset = 0;
		m_cbufferLayouts[stageIndex].reserve(shaderDesc.ConstantBuffers);
		for (std::uint32_t cbIdx = 0; cbIdx < shaderDesc.ConstantBuffers; ++cbIdx)
		{
			auto* constantBuffer = reflection->GetConstantBufferByIndex(cbIdx);
			constantBuffer->GetDesc(&shaderBufferDesc);

			// ���ʂ̒萔�o�b�t�@�͕ʂɊi�[
			std::string cbName = shaderBufferDesc.Name;
			bool isName = false;
			for (int i = static_cast<int>(core::SHADER::CB_SLOT::GBuffer); i < static_cast<int>(core::SHADER::CB_SLOT::Max); ++i)
			{
				if (cbName == core::SHADER::CB_NAME[i])
				{
					isName = true;
					break;
				}
			}
			// ��v���Ă�����X�L�b�v�H�H
			if (isName)
			{
				++slotOffset;
				continue;
			}

			// ���C�A�E�g����
			CBufferLayout cbLayout(cbIdx - slotOffset, shaderBufferDesc.Name, shaderBufferDesc.Size);
			cbLayout.variables.resize(shaderBufferDesc.Variables);

			// CB�ϐ��̃��C�A�E�g�쐬
			D3D12_SHADER_VARIABLE_DESC varDesc;
			for (std::uint32_t v = 0; v < shaderBufferDesc.Variables; ++v)
			{
				// �ϐ����擾
				auto* variable = constantBuffer->GetVariableByIndex(v);
				variable->GetDesc(&varDesc);
				// �ϐ������
				cbLayout.variables[v].name = varDesc.Name;
				cbLayout.variables[v].size = varDesc.Size;
				cbLayout.variables[v].offset = varDesc.StartOffset;
				cbLayout.variables[v].stage = stageIndex;
				cbLayout.variables[v].slot = cbIdx - slotOffset;
				// �f�t�H���g�l������ꍇ
				if (varDesc.DefaultValue != nullptr)
				{
					std::unique_ptr<std::byte[]> defaultValue = 
						std::make_unique<std::byte[]>(varDesc.Size);
					std::memcpy(defaultValue.get(), varDesc.DefaultValue, varDesc.Size);
					m_cbufferDefaults[varDesc.Name] = std::move(defaultValue);
				}
			}
			// �R���X�^���g�o�b�t�@���C�A�E�g�i�[
			m_cbufferLayouts[stageIndex].emplace(cbIdx - slotOffset, cbLayout);
		}


		// �e�N�X�`���E�T���v���̃��t���N�V����
		for (std::uint32_t i = 0; i < shaderDesc.BoundResources; ++i)
		{
			// �o�C���h���擾
			D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
			reflection->GetResourceBindingDesc(i, &shaderInputBindDesc);


			switch (shaderInputBindDesc.Type)
			{
			case D3D_SIT_TEXTURE:
			{
				// ���ʃ��\�[�X�̓X�L�b�v
				bool isSlot = false;
				for (int i = static_cast<int>(core::SHADER::TEX_SLOT::MainTexture); i < static_cast<int>(core::SHADER::TEX_SLOT::Max); ++i)
				{
					if (shaderInputBindDesc.BindPoint == i)
					{
						isSlot = true;
						break;
					}
				}
				// ��v���Ă�����
				if (isSlot)
				{
					m_staticTextureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
					m_staticTextureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
					m_staticTextureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
					continue;
				}

				m_textureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
				m_textureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
				m_textureBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
				break;
			}
			case D3D_SIT_SAMPLER:
			{
				// ���ʃ��\�[�X�̓X�L�b�v
				bool isSlot = false;
				for (int i = static_cast<int>(core::SHADER::SS_SLOT::Main); i < static_cast<int>(core::SHADER::SS_SLOT::Max); ++i)
				{
					if (shaderInputBindDesc.BindPoint == i)
					{
						isSlot = true;
						break;
					}
				}
				// ��v���Ă�����
				if (isSlot)
				{
					m_staticSamplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
					m_staticSamplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
					m_staticSamplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
					continue;
				}
				m_samplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
				m_samplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
				m_samplerBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
				break;
			}
			// �F�X����c
			case D3D_SIT_UAV_RWTYPED:
				break;
			case D3D_SIT_STRUCTURED:
			{
				// ���ʃ��\�[�X�̓X�L�b�v
				bool isSlot = false;
				for (int i = static_cast<int>(core::SHADER::SB_SLOT::PointLights); i < static_cast<int>(core::SHADER::SB_SLOT::Max); ++i)
				{
					if (shaderInputBindDesc.BindPoint == i)
					{
						isSlot = true;
						break;
					}
				}
				// ��v���Ă�����
				if (isSlot)
				{
					m_staticStructuredBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
					m_staticStructuredBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
					m_staticStructuredBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
					continue;
				}
				m_structuredBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
				m_structuredBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
				m_structuredBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
			}
				break;
			case D3D_SIT_TBUFFER:
				break;
			case D3D_SIT_CBUFFER:
			{
				// ���ʃ��\�[�X�̓X�L�b�v
				bool isSlot = false;
				for (int i = static_cast<int>(core::SHADER::CB_SLOT::GBuffer); i < static_cast<int>(core::SHADER::CB_SLOT::Max); ++i)
				{
					if (shaderInputBindDesc.BindPoint == i)
					{
						isSlot = true;
						break;
					}
				}
				// ��v���Ă�����
				if (isSlot)
				{
					m_staticCBufferBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
					m_staticCBufferBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
					m_staticCBufferBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
					continue;
				}
				m_cbufferBindDatas[stageIndex][shaderInputBindDesc.BindPoint].name = shaderInputBindDesc.Name;
				m_cbufferBindDatas[stageIndex][shaderInputBindDesc.BindPoint].slot = shaderInputBindDesc.BindPoint;
				m_cbufferBindDatas[stageIndex][shaderInputBindDesc.BindPoint].stage = stage;
				break;
			}
			default:
				// �G���[���O
				break;
			}
		}
	}


	// ���[�g�V�O�l�`���[�̍쐬
	CreateRootSignature(device);

}


/// @brief ���[�g�V�O�l�`���[�̐���
void D3D12Shader::CreateRootSignature(ID3D12Device* device)
{
	// �S�f�B�X�N���v�^�����W�E�p�����[�^
	std::vector<D3D12_DESCRIPTOR_RANGE>		aRanges;
	aRanges.reserve(100);
	std::vector<D3D12_ROOT_PARAMETER>		aParameters;
	std::vector<D3D12_STATIC_SAMPLER_DESC>	aSamplers;

	// �R���X�^���g�o�b�t�@�̊m��(GPU)
	for (core::ShaderStage stage = core::ShaderStage::VS; stage < core::ShaderStage::CS; ++stage)
	{
		auto stageIndex = static_cast<size_t>(stage);

		//--- ���I�f�B�X�N���v�^�����W�E�p�����[�^ ---

		// �萔�o�b�t�@
		if (m_cbufferBindDatas[stageIndex].size() > 0)
		{
			D3D12_DESCRIPTOR_RANGE range = {};
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			range.NumDescriptors = m_cbufferBindDatas[stageIndex].size();
			range.BaseShaderRegister = 0;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			aRanges.push_back(range);

			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.DescriptorTable.pDescriptorRanges = &aRanges.back();
			param.DescriptorTable.NumDescriptorRanges = 1;
			param.ShaderVisibility = SHADER_VISIBILITYS[stageIndex];
			aParameters.push_back(param);
		}

		// �e�N�X�`��
		for (auto& tex : m_textureBindDatas[stageIndex])
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			param.Descriptor.ShaderRegister = tex.first;
			param.Descriptor.RegisterSpace = 0;
			param.ShaderVisibility = SHADER_VISIBILITYS[stageIndex];
			aParameters.push_back(param);
		}

		// �T���v���[
		for (auto& sam : m_samplerBindDatas[stageIndex])
		{
			D3D12_DESCRIPTOR_RANGE range = {};
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			range.NumDescriptors = m_samplerBindDatas[stageIndex].size();
			range.BaseShaderRegister = 0;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			aRanges.push_back(range);

			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.DescriptorTable.pDescriptorRanges = &aRanges.back();
			param.DescriptorTable.NumDescriptorRanges = 1;
			param.ShaderVisibility = SHADER_VISIBILITYS[stageIndex];
			aParameters.push_back(param);
		}

		// �X�g���N�`���[�h
		for (auto& sam : m_structuredBindDatas[stageIndex])
		{
			D3D12_DESCRIPTOR_RANGE range = {};
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range.NumDescriptors = m_structuredBindDatas[stageIndex].size();
			range.BaseShaderRegister = 0;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			aRanges.push_back(range);

			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.DescriptorTable.pDescriptorRanges = &aRanges.back();
			param.DescriptorTable.NumDescriptorRanges = 1;
			param.ShaderVisibility = SHADER_VISIBILITYS[stageIndex];
			aParameters.push_back(param);
		}

		//--- �ÓI�f�B�X�N���v�^�����W�E�p�����[�^ ---

		// �萔�o�b�t�@
		for (auto& cb : m_staticCBufferBindDatas[stageIndex])
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.Descriptor.ShaderRegister = cb.first;
			param.Descriptor.RegisterSpace = 0;
			param.ShaderVisibility = SHADER_VISIBILITYS[stageIndex];
			aParameters.push_back(param);
		}

		// �e�N�X�`��
		for (auto& tex : m_staticTextureBindDatas[stageIndex])
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			param.Descriptor.ShaderRegister = tex.first;
			param.Descriptor.RegisterSpace = 0;
			param.ShaderVisibility = SHADER_VISIBILITYS[stageIndex];
			aParameters.push_back(param);
		}

		// �T���v���[
		for (auto& sam : m_staticSamplerBindDatas[stageIndex])
		{

		}

		// �X�g���N�`���[�h
		for (auto& sam : m_staticStructuredBindDatas[stageIndex])
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			param.Descriptor.ShaderRegister = sam.first;
			param.Descriptor.RegisterSpace = 0;
			param.ShaderVisibility = SHADER_VISIBILITYS[stageIndex];
			aParameters.push_back(param);
		}
	}

	// ���[�g�V�O�l�`���[�̐���
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = aParameters.size();
	rootSignatureDesc.pParameters = aParameters.data();
	rootSignatureDesc.NumStaticSamplers = aSamplers.size();
	rootSignatureDesc.pStaticSamplers = aSamplers.data();
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* rootSigBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	std::string errstr;
	if (rootSigBlob)
	{
		device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
			IID_PPV_ARGS(m_pRootSignature.ReleaseAndGetAddressOf()));
	}
	else
	{
		errstr.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";
		OutputDebugStringA(errstr.c_str());
		errorBlob->Release();
	}
	rootSigBlob->Release();
}
