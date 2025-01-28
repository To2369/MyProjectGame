#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include <wrl.h>
#include"Shader.h"
#include<string>
class Sprite
{
public:
    // 頂点フォーマット
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };

    Sprite(ID3D11Device* device, const wchar_t* filename);
    virtual ~Sprite();

    // 描画
    void Render(ID3D11DeviceContext* dc,
        float dx, float dy,     // 短形の左上の座標(スクリーン座標系)
        float dw, float dh,     // 短形のサイズ(スクリーン座標系)
        float r, float g,float b, float a,       // 色と透明度
        float angle             // 角度(degree)
    );

    void Render(ID3D11DeviceContext* dc,
        float dx, float dy,     // 短形の左上の座標(スクリーン座標系)
        float dw, float dh,     // 短形のサイズ(スクリーン座標系)
        float r, float g, float b, float a,       // 色と透明度
        float angle,             // 角度(degree)
        float sx, float sy, float sw, float sh  // 左,上,幅,高さ
    );

    // 画面に文字を出す
    void Textout(ID3D11DeviceContext* dc,
        std::string s,
        float x, float y, float w, float h,
        float r, float g, float b, float a);

    // テクスチャ幅取得
    int GetTextureWidth() const { return textureWidth; }

    // テクスチャ高さ取得
    int GetTextureHeight() const { return textureHeight; }

    // 頂点バッファの取得
    const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return vertexBuffer; }

    // シェーダーリソースビューの取得
    const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const { return shaderResourceView; }

    void SetShaderResourceView(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, int texWidth, int texHeight);
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

    int textureWidth = 0;
	int textureHeight = 0;
};