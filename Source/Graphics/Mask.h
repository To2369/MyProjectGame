#pragma once

#include <memory>
#include <wrl.h>
#include "Shader.h"
class Mask
{
public :
    Mask(ID3D11Device* device);
	~Mask() {};

	/*void Begin(ID3D11DeviceContext* dc);
	void Draw(ID3D11DeviceContext* dc, Sprite* sprite);
	void End(ID3D11DeviceContext* dc);*/
    void Render(ID3D11DeviceContext* dc,Sprite* sprite);
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;

	struct dissolve_constants 
	{
		DirectX::XMFLOAT4 parameters;	//	x : ディゾルブ適応量、yzw : 空き
	};

	float dissolve_value{ 0.5f };
	Microsoft::WRL::ComPtr<ID3D11Buffer> dissolve_constant_buffer;

	ID3D11ShaderResourceView* mask_texture;
};