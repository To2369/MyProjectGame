#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include <wrl.h>

class Sprite
{
public:
    //頂点フォーマット
    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

    Sprite(ID3D11Device* device);
    ~Sprite();

    //描画
    void Render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,     //短形の左上の座標(スクリーン座標系)
        float dw, float dh,     //短形のサイズ(スクリーン座標系)
        float r, float g,float b, float a       //色と透明度
    );
private:
    //頂点シェーダー
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    //ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    //入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    //頂点データ
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

    //D3D11_TEXTURE2D_DESC texture2d_desc;
};