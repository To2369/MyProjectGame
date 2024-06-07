#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>
#include"RenderState.h"

class Graphics
{
private:
	Graphics() = default;
	~Graphics() = default;

public:
	static Graphics* Instance()
	{
		static Graphics ins;
		return &ins;
	}

	// ����������
	void Initialize(HWND hwnd);

	// �N���A
	void Clear(float r, float g, float b, float a);

	// �����_�[�^�[�Q�b�g�ݒ�
	void SetRenderTargets();

	// ��ʕ\��
	void Present(UINT syncInterval);

	// �f�o�C�X�擾
	ID3D11Device* GetDevice() const { return device.Get(); }

	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* GetDeviceContext() const { return immediate_context.Get(); }

	// �X���b�v�`�F�[���擾
	IDXGISwapChain* GetSwapChain() const { return swap_chain.Get(); }

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* GetRenderTargetView() const { return render_target_view.Get(); }

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* GetDepthStencilView() const { return depth_stencil_view.Get(); }

	//// �V�F�[�_�[�擾
	//Shader* GetShader() const { return shader.get(); }

	// �X�N���[�����擾
	float GetScreenWidth() const { return screenWidth; }

	// �X�N���[�������擾
	float GetScreenHeight() const { return screenHeight; }

	//// �f�o�b�O�����_���擾
	//DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

	//// ���C�������_���擾
	//LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

	//// ImGui�����_���擾
	//ImGuiRenderer* GetImGuiRenderer() const { return imguiRenderer.get(); }


	// �����_�[�X�e�[�g�擾
	RenderState* GetRenderState() { return renderState.get(); }

	std::mutex& GetMutex() { return mutex; }

private:
	float	screenWidth;
	float	screenHeight;

	//DirectX11�ŗ��p����l�X�ȃ��\�[�X�̍쐬
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	//�l�X�ȕ`�施�߂�GPU�ɓ`����
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	immediate_context;

	//�L�����p�X�ɕ`��������z(�E�B���h�E)�ɓ����
	Microsoft::WRL::ComPtr<IDXGISwapChain>	swap_chain;

	//�F���������݃L�����p�X
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	render_target_view;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>	depth_stencil_buffer;

	//���s�����������݃L�����p�X
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depth_stencil_view;

	D3D11_VIEWPORT viewport;

	std::mutex mutex;

	std::unique_ptr<RenderState> renderState;
};