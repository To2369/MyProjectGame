#pragma once
#include<d3d11.h>
#include<wrl.h>

enum class SAMPLER_STATE
{
    POINT,
    LINEAR,
    ANISOTROPIC,

	Enum_Max,
};

enum class DEPTH_STENCIL_STATE
{
    ON_ON,          // �[�x�e�X�g��ON�[�x�������݂�ON
    ON_OFF,         // �[�x�e�X�g��ON�[�x�������݂�OFF
    OFF_ON,         // �[�x�e�X�g��OFF�[�x�������݂�ON
    OFF_OFF,        // �x�e�X�g��OFF�[�x�������݂�OFF

	Enum_Max,
};

enum class BLEND_STATE
{
	NONE,
	ALPHABLENDING,
	ADD,
	SUB,
	MULTIPLE,

	Enum_Max,
};

enum class RASTERIZER_STATE
{
    SOLID_CULLBACK,
    WIRE_CULLBACK,
    SOLID_CULLNONE,
    WIRE_CULLNONE,

	Enum_Max,
};

class RenderState
{
public:
	RenderState(ID3D11Device* device);
	~RenderState() = default;

	ID3D11SamplerState* GetSamplerState(SAMPLER_STATE state) const
	{ 
		return sampler_states[static_cast<int>(state)].Get(); 
	}

	ID3D11DepthStencilState* GetDepthStencilStates(DEPTH_STENCIL_STATE state) const 
	{ 
		return depth_stencil_states[static_cast<int>(state)].Get(); 
	}

	ID3D11BlendState* GetBlendStates(BLEND_STATE state) const 
	{ 
		return blend_states[static_cast<int>(state)].Get(); 
	}

	ID3D11RasterizerState* GetRasterizerStates(RASTERIZER_STATE state) const 
	{ 
		return rasterizer_states[static_cast<int>(state)].Get(); 
	}

	//DebugRenderer* getDebugRenderer() const { return debugRenderer.get(); }

	//LineRenderer* getLineRenderer() const { return lineRenderer.get(); }

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		sampler_states[static_cast<int>(SAMPLER_STATE::Enum_Max)];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[static_cast<int>(DEPTH_STENCIL_STATE::Enum_Max)];
	Microsoft::WRL::ComPtr<ID3D11BlendState>		blend_states[static_cast<int>(BLEND_STATE::Enum_Max)];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizer_states[static_cast<int>(RASTERIZER_STATE::Enum_Max)];
	//std::unique_ptr<DebugRenderer> debugRenderer;
	//std::unique_ptr<LineRenderer> lineRenderer;

};