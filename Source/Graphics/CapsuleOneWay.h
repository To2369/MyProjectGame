#pragma once
#include<DirectXMath.h>
#include<d3d11.h>
#include<wrl.h>
#include"Shader.h"
class CapsuleOneWay
{
public:
    CapsuleOneWay(ID3D11Device* device,
        float height,
        float radius,
        uint32_t slices,
        uint32_t ellipsoid_stacks,
        uint32_t stacks);
    ~CapsuleOneWay(){};

    void RegenerateGeometry();
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& world,
        const DirectX::XMFLOAT4& material_color);
    void SetGrowthRate(const float& rate) { this->growthRate = rate; }
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

    // 頂点バッファのオブジェクトの作成
    void CreateComBuffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
        uint32_t* indices, size_t index_count);
private:
    float currentHeight = 0;        // 現在の高さ
    float growthRate = 0;           // 高さの増加速度
    float radius = 0;               // カプセルの半径
    uint32_t slices = {};            // 分割数
    uint32_t stacks = {};            // 円柱の分割数
    ID3D11Device* device = {};       // デバイス参照を保持

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

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    // インデックスバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

    D3D11_SUBRESOURCE_DATA subresource_data{};
};
