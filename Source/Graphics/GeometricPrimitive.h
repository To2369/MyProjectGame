#pragma once
#include<DirectXMath.h>
#include<d3d11.h>
#include<wrl.h>
#include"Shader.h"
// 幾何プリミティブ
// 幾何プリミティブとは...立方体、球、円柱などの形状の図形
class GeometricPrimitive
{
public:
    // コンストラクタはprotectedに
    virtual ~GeometricPrimitive() = default;

    void Render(ID3D11DeviceContext* dc,
        const DirectX::XMFLOAT4X4& world,
        const DirectX::XMFLOAT4& materialColor);

    // 頂点フォーマット
    struct vertex
    {
        DirectX::XMFLOAT3 position; // 位置
        DirectX::XMFLOAT3 normal;   // 法線
    };
    // 定数バッファ用フォーマット
    struct constants
    {
        DirectX::XMFLOAT4X4 world;          // ワールド行列
        DirectX::XMFLOAT4 materialColor;   // マテリアルカラー
    };
protected:
    GeometricPrimitive(ID3D11Device* device);

    // 頂点バッファのオブジェクトの作成
    void CreateComBuffers(ID3D11Device* device, vertex* vertices, size_t vertexCount,
        uint32_t* indices, size_t indexCount);

    // 頂点シェーダーオブジェクト
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    // ピクセルシェーダーオブジェクト
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

    // 入力レイアウトオブジェクト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    // 定数バッファのオブジェクト
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

    // 定数バッファ
    D3D11_BUFFER_DESC bufferDesc{};
private:
    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    // インデックスバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

    D3D11_SUBRESOURCE_DATA subresourceData{};
};

// 正方形
class GeometricCube :public GeometricPrimitive
{
public:
    GeometricCube(ID3D11Device* device);
};

// 円柱
class GeometricCylinder :public GeometricPrimitive
{
public:
    // 第２引数はシリンダーの円を何分割するかの数を指定
    GeometricCylinder(ID3D11Device* device,float radius,float height, uint32_t slices);
};

// 球
class GeometricSphere :public GeometricPrimitive
{
public:
    GeometricSphere(ID3D11Device* device,
        float radius,
        uint32_t slices,
        uint32_t stacks);
};

// カプセル
class GeometricCapsule :public GeometricPrimitive
{
public:
    GeometricCapsule(ID3D11Device* device,
        float height,
        float radius,
        uint32_t slices,
        uint32_t ellipsoidStacks,
        uint32_t stacks);
};