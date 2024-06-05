#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include <wrl.h>
#include<vector>
#include"Shader.h"

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
    void Begin(ID3D11DeviceContext* immediate_context,
        ID3D11PixelShader* replaced_pixsel_shader = nullptr,
        ID3D11ShaderResourceView* replaced_shader_resource_view = nullptr);

    //変更した頂点データの編集と一括描画
    void End(ID3D11DeviceContext* immediate_context);

    //描画
    void Render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,     //短形の左上の座標(スクリーン座標系)
        float dw, float dh     //短形のサイズ(スクリーン座標系)
    );

    //描画
    void Render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,     //短形の左上の座標(スクリーン座標系)
        float dw, float dh,     //短形のサイズ(スクリーン座標系)
        float r, float g, float b, float a,       //色と透明度
        float angle             //角度(degree)
    );

    //描画
    void Render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,     //短形の左上の座標(スクリーン座標系)
        float dw, float dh,     //短形のサイズ(スクリーン座標系)
        float r, float g, float b, float a,       //色と透明度
        float angle,             //角度(degree)
        float sx, float sy, float sw, float sh  //左,上,幅,高さ
    );

    ID3D11PixelShader* GetReplaced_pixel_shader() const
    {
        return replaced_pixel_shader.Get();
    }
    ID3D11ShaderResourceView* GetReplaced_Shader_resource_view() const
    {
        return replaced_shader_resource_view.Get();
    }
private:
    static void rotate(float& x, float& y, float cx, float cy, float sin, float cos)
    {
        x -= cx;
        y -= cy;

        float tx{ x }, ty{ y };
        x = cos * tx + -sin * ty;
        y = sin * tx + cos * ty;

        x += cx;
        y += cy;
    }
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

    //テクスチャ
    D3D11_TEXTURE2D_DESC replaced_texture2d_desc;
    //差し替え用ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> replaced_pixel_shader;
    // 差し替え用のテクスチャ
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> replaced_shader_resource_view;
private:
    const size_t max_vertices;
    std::vector<vertex> vertices;

    ShaderManager* shaderMgr;
};