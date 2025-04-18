#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include <wrl.h>
#include<vector>
#include"Shader.h"

class SpriteBatch
{
public:
    // 頂点フォーマット
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };

    SpriteBatch(ID3D11Device* device, const wchar_t* filename,size_t maxSprites);
    ~SpriteBatch();

    // シェーダーとテクスチャの設定
    void Begin(ID3D11DeviceContext* dc,
        ID3D11PixelShader* replacedPixselShader = nullptr,
        ID3D11ShaderResourceView* replacedShaderResourceView = nullptr);

    // 変更した頂点データの編集と一括描画
    void End(ID3D11DeviceContext* dc);

    // 描画
    void Render(ID3D11DeviceContext* dc,
        float dx, float dy,     // 短形の左上の座標(スクリーン座標系)
        float dw, float dh      // 短形のサイズ(スクリーン座標系)
    );

    // 描画
    void Render(ID3D11DeviceContext* dc,
        float dx, float dy,     // 短形の左上の座標(スクリーン座標系)
        float dw, float dh,     // 短形のサイズ(スクリーン座標系)
        float r, float g, float b, float a,       // 色と透明度
        float angle             // 角度(degree)
    );

    // 描画
    void Render(ID3D11DeviceContext* dc,
        float dx, float dy,     // 短形の左上の座標(スクリーン座標系)
        float dw, float dh,     // 短形のサイズ(スクリーン座標系)
        float r, float g, float b, float a,       // 色と透明度
        float angle,             // 角度(degree)
        float sx, float sy, float sw, float sh  // 左,上,幅,高さ
    );

    ID3D11PixelShader* GetReplaced_pixel_shader() const
    {
        return replacedPixelShader.Get();
    }
    ID3D11ShaderResourceView* GetReplaced_Shader_resource_view() const
    {
        return replacedShaderResourceView.Get();
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
    // 頂点シェーダー
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    // ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    // 入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    // 頂点データ
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    // シェーダーリソースビュー
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
    // テクスチャ
    D3D11_TEXTURE2D_DESC texture2dDesc;

    // テクスチャ
    D3D11_TEXTURE2D_DESC replacedTexture2dDesc;
    // 差し替え用ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> replacedPixelShader;
    // 差し替え用のテクスチャ
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> replacedShaderResourceView;
private:
    const size_t maxVertices;
    std::vector<Vertex> vertices;
};