#pragma once
#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include "Shader.h"
#include "StaticMesh.h"
class ShadowMapCaster
{
public:
    ShadowMapCaster(ID3D11Device* device);
    ~ShadowMapCaster(){}

    void Render(ID3D11DeviceContext* immediateContext);
   /* void Begin(const RenderContext& rc)override;
    void Draw(const RenderContext& rc, const Model* model)override;
    void End(const RenderContext& rc)override;*/
private:
    static constexpr int shadowBufferSize = 4;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowMapDepthStencilView[shadowBufferSize];
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowMapShaderResourceView[shadowBufferSize];
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shadowMapCasterVertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> shadowMapCasterInputLayout;

    DirectX::XMFLOAT4 directional_light_direction{ 0.0f,-1.0f,1.0f,1.0f };

    struct shadowMapCasterConstants
    {
        DirectX::XMFLOAT4X4 view_projection;
        DirectX::XMFLOAT4 options;
        DirectX::XMFLOAT4 camera_position;
    };
    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

    POINT cursor_position;
    float timer{ 0.0f };
    bool flag{ false };

    DirectX::XMFLOAT3 translation{ 0, 0, 0 };
    DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
    DirectX::XMFLOAT3 rotation{ 0, 0, 0 };
    DirectX::XMFLOAT4 material_color{ 1 ,1, 1, 1 };
    std::vector<std::unique_ptr<StaticMesh>> dummy_static_mesh;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
};