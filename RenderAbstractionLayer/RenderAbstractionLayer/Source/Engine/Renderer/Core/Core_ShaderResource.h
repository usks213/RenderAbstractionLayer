/*****************************************************************//**
 * \file   Core_ShaderResource.h
 * \brief  シェーダーリソース情報
 * 
 * \author USAMI KOSHI
 * \date   2021/10/05
 *********************************************************************/
#ifndef _CORE_SHADER_RESOURCE_
#define _CORE_SHADER_RESOURCE_

#include "Core_Light.h"

namespace core
{ 
	namespace SHADER {
		// CBuffer Name
		constexpr char SHADER_CB_NAME_GBUFFER[] = "Gbuffer";
		constexpr char SHADER_CB_NAME_SYSTEM[] = "System";
		constexpr char SHADER_CB_NAME_TRANSFORM[] = "Transform";
		constexpr char SHADER_CB_NAME_ANIMATION[] = "Animation";
		// CBuffer Slot
		constexpr std::uint32_t SHADER_CB_SLOT_GBUFFER = 4;
		constexpr std::uint32_t SHADER_CB_SLOT_SYSTEM = 5;
		constexpr std::uint32_t SHADER_CB_SLOT_TRANSFORM = 6;
		constexpr std::uint32_t SHADER_CB_SLOT_ANIMATION = 7;
		// CoreTexture Slot
		constexpr std::uint32_t SHADER_SRV_SLOT_MAINTEX = 4;
		constexpr std::uint32_t SHADER_SRV_SLOT_SHADOWMAP = 5;
		constexpr std::uint32_t SHADER_SRV_SLOT_SKYDOOM = 6;
		// StructuredBuffer Slot
		constexpr std::uint32_t SHADER_SRV_SLOT_POINTLIGHT = 8;
		constexpr std::uint32_t SHADER_SRV_SLOT_SPOTLIGHT = 9;
		// Samplear Slot
		constexpr std::uint32_t SHADER_SS_SLOT_MAIN = 4;
		constexpr std::uint32_t SHADER_SS_SLOT_SHADOW = 5;
		constexpr std::uint32_t SHADER_SS_SLOT_SKYBOX = 6;
		// UAV Slot

		// max register
		constexpr std::uint32_t MAX_CB_SLOT_COUNT = 15;
		constexpr std::uint32_t MAX_SS_SLOT_COUNT = 16;
		constexpr std::uint32_t MAX_SRV_SLOT_COUNT = 16;
		constexpr std::uint32_t MAX_UAV_SLOT_COUNT = 16;

		// Light
		constexpr std::uint32_t MAX_POINT_LIGHT_COUNT = 128;
		constexpr std::uint32_t MAX_SPOT_LIGHT_COUNT = 128;

		// AnimationCB Bone
		constexpr std::uint32_t MAX_ANIMATION_BONE_COUNT = 512;

		// システム定数バッファ
		struct SystemBuffer
		{
			Matrix	_mView;
			Matrix	_mProj;
			Matrix	_mProj2D;
			Matrix	_mViewInv;
			Matrix	_mProjInv;
			Vector4 _viewPos;

			CoreDirectionalLight _directionalLit;

			std::uint32_t _pointLightNum;
			std::uint32_t _spotLightNum;

			Vector2 _padding2;
		};

		// トランスフォーム定数バッファ
		struct TransformBuffer
		{
			Matrix _mWorld;
		};

		// アニメーション定数バッファ
		struct AnimationBuffer
		{
			Matrix _mBone[MAX_ANIMATION_BONE_COUNT];
		};
	}
}

#endif // !_CORE_SHADER_RESOURCE_
