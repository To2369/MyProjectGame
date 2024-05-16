#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<functional>
#include<memory>
#include"../Buffer.h"
enum class SAMPLER_STATE
{
    POINT,
    LINEAR,
    ANISOTROPIC,
};

enum class DEPTH_STENCIL_STATE
{
    ON_ON,          //深度テストをON深度書き込みをON
    ON_OFF,         //深度テストをON深度書き込みをOFF
    OFF_ON,         //深度テストをOFF深度書き込みをON
    OFF_OFF,        //深度テストをOFF深度書き込みをOFF
};

enum class BLEND_STATE
{
	NONE,
	ALPHABLENDING,
	ADD,
	MULTIPLE
};

enum class RASTERIZER_STATE
{
    SOLID_CULLBACK,
    WIRE_CULLBACK,
    SOLID_CULLNONE,
    WIRE_CULLNONE
};

struct RenderContext
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[3];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[4];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[4];
};

class GraphicsManager
{
private:
	GraphicsManager() {}
	~GraphicsManager() {}

public:
	static GraphicsManager* Instance()
	{
		static GraphicsManager ins;
		return &ins;
	}

	bool Initialize(ID3D11Device* device);

	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState(SAMPLER_STATE state) const { return renderContext.sampler_states[static_cast<int>(state)]; }

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetDepthStencilStates(DEPTH_STENCIL_STATE state) const { return renderContext.depth_stencil_states[static_cast<int>(state)]; }

	Microsoft::WRL::ComPtr<ID3D11BlendState> GetBlendStates(BLEND_STATE state) const { return renderContext.blendStates[static_cast<int>(state)]; }

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterizerStates(RASTERIZER_STATE state) const { return renderContext.rasterizerStates[static_cast<int>(state)]; }

	void SettingRenderContext(std::function<void(RenderContext&)> SettingCallback);

	//DebugRenderer* getDebugRenderer() const { return debugRenderer.get(); }

	//LineRenderer* getLineRenderer() const { return lineRenderer.get(); }

private:
	RenderContext renderContext;

	//std::unique_ptr<DebugRenderer> debugRenderer;
	//std::unique_ptr<LineRenderer> lineRenderer;

};