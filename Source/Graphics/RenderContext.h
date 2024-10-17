#pragma once
#include "RenderState.h"
#include "..\Camera.h"

struct DirectionalLightData
{
	DirectX::XMFLOAT4 direction;	// Œü‚«
	DirectX::XMFLOAT4 color;	// F
};

struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	const RenderState* renderState;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4 lightDirection;	// Œü‚«
};
