#pragma once
#include <d3d11.h>
#include<wrl.h>
#include <cstdint>

enum class SHADER_RESOURCE_VIEW
{
	RenderTargetView,
	ShaderResourceView,

	Enum_Max,
};

class FrameBuffer
{
public:
	FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height);
	virtual ~FrameBuffer() = default;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[static_cast<int>(SHADER_RESOURCE_VIEW::Enum_Max)];
	D3D11_VIEWPORT viewport;

	// �N���A����
	void Clear(ID3D11DeviceContext* immediate_context,
		float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1);

	// �I�t�X�N���[�������_�����O�̐ݒ�ɘA��
    // ���܂ł̕`������L���b�V���ɑޔ������I�t�X�N���[���������ݗp�Ƀ^�[�Q�b�g��ݒ�
	void Activate(ID3D11DeviceContext* immediate_context);

	// �I�t�X�N���[���̐ݒ肩��A��������
	void Deactivate(ID3D11DeviceContext* immediate_context);

private:
	// �ޔ�p�̃L���b�V�����
	UINT viewport_count{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT cached_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;
};