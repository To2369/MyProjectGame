#pragma once
#include "d3d11.h"
#include <DirectXMath.h>
#include <wrl.h>
#include "Shader.h"
#include "Sprite.h"
#include "RenderContext.h"
#include <memory>
#ifdef USE_IMGUI
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"
#endif
class ColorGraging :public SpriteShader
{
public :
    ColorGraging(ID3D11Device* device);
    ~ColorGraging()override {}

    void Begin(const RenderContext& rc)override;
    void Draw(const RenderContext& rc, Sprite* sprite)override;
    void End(const RenderContext& rc)override;

    struct colorFilter
    {
        float fueShift;
        float brightress;
        float saturation;
        float dummy;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> color_filter_constant_buffer;
    DirectX::XMFLOAT4 color_filter_parameter{ 0.0f, 1.0f, 1.0f, 0.0f };
    //std::unique_ptr<Sprite> sprite;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

    Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

    Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;

};