#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include <wrl.h>
#include<vector>

class Sprite_batch
{
public:
    //頂点フォーマット
    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };

    Sprite_batch(ID3D11Device* device, const wchar_t* filename,size_t max_sprites);
    ~Sprite_batch();

    //シェーダーとテクスチャの設定
    void Begin(ID3D11DeviceContext* immediate_context);

    //変更した頂点データの編集と一括描画
    void End(ID3D11DeviceContext* immediate_context);

    //描画
    void Render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,     //短形の左上の座標(スクリーン座標系)
        float dw, float dh,     //短形のサイズ(スクリーン座標系)
        float r, float g, float b, float a,       //色と透明度
        float angle             //角度(degree)
    );

    void Render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,     //短形の左上の座標(スクリーン座標系)
        float dw, float dh,     //短形のサイズ(スクリーン座標系)
        float r, float g, float b, float a,       //色と透明度
        float angle,             //角度(degree)
        float sx, float sy, float sw, float sh  //左,上,幅,高さ
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
    //シェーダーリソースビュー
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
    //テクスチャ
    D3D11_TEXTURE2D_DESC texture2d_desc;

private:
    const size_t max_vertices;
    std::vector<vertex> vertices;
};