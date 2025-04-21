#include "SkyMap.h"
#include "Shader.h"

#include"../misc.h"

SkyMap::SkyMap(ID3D11Device* device, const wchar_t* filename, bool generateMips)
{
	D3D11_TEXTURE2D_DESC texture2dDesc;
	ShaderManager::Instance()->LoadTextureFromFile(device, filename, shaderResourceView.GetAddressOf(), &texture2dDesc);

	if (texture2dDesc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
	{
		isTextureCube = true;
	}

	ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\SkyMapVS.cso", skyMapVS.GetAddressOf(), NULL, NULL, 0);
	ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\SkyMapPS.cso", skyMapPS.GetAddressOf());
	ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\SkyMapPS.cso", skyBoxPS.GetAddressOf());

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(constants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

void SkyMap::Blit(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& viewProjection)
{
	dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetInputLayout(NULL);

	dc->VSSetShader(skyMapVS.Get(), 0, 0);
	dc->PSSetShader(isTextureCube ? skyBoxPS.Get() : skyMapPS.Get(), 0, 0);

	dc->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	constants data;
	DirectX::XMStoreFloat4x4(&data.inverseViewProjection, DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&viewProjection)));

	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
	dc->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	dc->Draw(4, 0);

	dc->VSSetShader(NULL, 0, 0);
	dc->PSSetShader(NULL, 0, 0);
}
