#pragma once
#include <DirectXMath.h>
#include "StaticMesh.h"

#ifdef USE_IMGUI
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"
#endif
// ŒõŒ¹ƒNƒ‰ƒX
class Light
{
public:
	Light(ID3D11Device* device);

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc);

	struct lightConstants
	{
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 directionalLightDirection;
		DirectX::XMFLOAT4 directionalLightColor;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightConstantBuffer;

	DirectX::XMFLOAT4 ambientColor{ 0.2f, 0.2f, 0.2f, 0.2f };
	DirectX::XMFLOAT4 directionalLightDirection{ 0.0f, -1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 directionalLightColor{ 1.0f, 1.0f, 1.0f, 1.0f };

	std::unique_ptr<StaticMesh> dummyStaticMesh;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> meshVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> meshInputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> meshPixelShader;
};
