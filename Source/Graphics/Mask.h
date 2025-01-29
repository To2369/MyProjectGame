#pragma once
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
class Mask
{
public :
    Mask(ID3D11Device* device);
    ~Mask() {}

    void Begin(const RenderContext& rc);
private:
    struct CbMask
    {
        float				dissolveThreshold;	// ディゾルブ値
        float				edgeThreshold;		// 縁の閾値
        DirectX::XMFLOAT2	dummy;
        DirectX::XMFLOAT4	edgeColor;			// 縁の色
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer>				maskConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;
    D3D11_TEXTURE2D_DESC maskTexture2dDesc;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> maskTexture;
};