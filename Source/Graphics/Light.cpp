#include "Graphics.h"
#include "Buffer.h"
#include "Light.h"
Light::Light(ID3D11Device* device)
{
	CreateBuffer<Light::lightConstants>(device, lightConstantBuffer.GetAddressOf());

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\PhongShaderVS.cso",
		meshVertexShader.GetAddressOf(), meshInputLayout.GetAddressOf(),
		inputElementDesc, ARRAYSIZE(inputElementDesc));
	ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\PhongShaderPS.cso",
		meshPixelShader.GetAddressOf());
}

void Light::Update(float elapsedTIme)
{
#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");
	ImGui::Separator();
	ImGui::ColorEdit3("ambient_color", &ambientColor.x);
	ImGui::SliderFloat3("directional_light_direction", &directionalLightDirection.x, -1.0f, +1.0f);
	ImGui::ColorEdit3("directional_light_color", &directionalLightColor.x);

	ImGui::End();
#endif
}

void Light::Render(ID3D11DeviceContext* dc)
{
	lightConstants lights{};
	lights.ambientColor = ambientColor;
	lights.directionalLightDirection = directionalLightDirection;
	lights.directionalLightColor = directionalLightColor;
	BindBuffer(dc, 2, lightConstantBuffer.GetAddressOf(), &lights);
}