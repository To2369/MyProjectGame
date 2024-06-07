#pragma once
#include "RenderState.h"
#include "..\Camera.h"

struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	const RenderState* renderState;
	const Camera* camera;
	DirectX::XMFLOAT4		lightDirection;
};
