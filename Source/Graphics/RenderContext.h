#pragma once
#include "RenderState.h"
#include "..\Camera.h"

struct DirectionalLightData
{
	DirectX::XMFLOAT4 direction;	// 向き
	DirectX::XMFLOAT4 color;	// 色
};

// 色調補正情報
struct ColorGradingData
{
	float	hueShift = 0;	// 色相調整
	float	saturation = 1;	// 彩度調整
	float	brightness = 1;	// 明度調整
	float	dummy;
};

struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	const RenderState* renderState;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4 lightDirection;	// 向き
	//色調補正情報
	ColorGradingData colorGradingData;
	//GlitchNoiseData glitchnoiseData;
};

struct GlitchNoiseData
{
	float intensity = 0.25f;//強度
	float time = 0;

	//ブロック状ノイズ
	float baseIntensity = 0.2f;
	float baseFps = 7;
	DirectX::XMFLOAT2 baseAmount = { 4.0f, 32.0f };
	//色をずらす
	float caIntensity = 0.5f;//色シフトの強度
	float caFps = 2;	//色シフトのFPS
	DirectX::XMFLOAT3 caColorFactor = { 0.5f,0.5f,0.5f };//色毎のずらし量
};