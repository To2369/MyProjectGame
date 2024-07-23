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

    void Render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& world,
        const DirectX::XMFLOAT4& material_color);

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
        DirectX::XMFLOAT4 material_color;   // マテリアルカラー
    };
protected:
    GeometricPrimitive(ID3D11Device* device);

    // 頂点バッファのオブジェクトの作成
    void CreateComBuffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
        uint32_t* indices, size_t index_count);

    // 頂点シェーダーオブジェクト
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    // ピクセルシェーダーオブジェクト
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;

    // 入力レイアウトオブジェクト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    // 定数バッファのオブジェクト
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

    // 定数バッファ
    D3D11_BUFFER_DESC buffer_desc{};
private:
    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    // インデックスバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

    D3D11_SUBRESOURCE_DATA subresource_data{};
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
    GeometricCylinder(ID3D11Device* device, uint32_t slices);
};

// 球
class GeometricSphere :public GeometricPrimitive
{
public:
    GeometricSphere(ID3D11Device* device,
        uint32_t slices,
        uint32_t stacks);
};

// カプセル
class GeometricCapsule :public GeometricPrimitive
{
public:
    GeometricCapsule(ID3D11Device* device,
        float mantle_height,
        const DirectX::XMFLOAT3& radius,
        uint32_t slices,
        uint32_t ellipsoid_stacks,
        uint32_t mantle_stacks);
};