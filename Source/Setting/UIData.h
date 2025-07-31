#pragma once
#include <DirectXMath.h>
#include <string>
struct UIData
{
	DirectX::XMFLOAT2 position;
	float width;
	float height;
	DirectX::XMFLOAT4 color;
	float angle;

	std::string text = nullptr;	// TextOut�g�p���̂�
};

namespace SceneGameUIDatas
{
	
}