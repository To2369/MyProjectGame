#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

class SkyMap
{
public:
	SkyMap(ID3D11Device* device, const wchar_t* filename, bool generate_mips = false);
	virtual ~SkyMap() = default;
	SkyMap(const SkyMap&) = delete;
	SkyMap& operator =(const SkyMap&) = delete;
	SkyMap(SkyMap&&) noexcept = delete;
	SkyMap& operator =(SkyMap&&) noexcept = delete;

	void Blit(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& viewProjection);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> skyMapVS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> skyMapPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> skyBoxPS;

	struct constants
	{
		DirectX::XMFLOAT4X4 inverseViewProjection;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	bool isTextureCube = false;

};