#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>
#include"RenderState.h"
#include "LineRenderer.h"
#include "DebugPrimitive.h"
#include "Shader.h"
#include "Light.h"
enum class ModelShaderId
{
	Default,
	Phong,
	Toon,
	ShadowmapCaster,

	Max
};

enum class SpriteShaderId
{
	Default,
	UVScroll,
	Mask,
	ColorGrading,
	GaussianBlur,
	LuminanceExtraction,
	Finalpass,

	Max
};

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
	ID3D11DeviceContext* GetDeviceContext() const { return dc.Get(); }

	// �X���b�v�`�F�[���擾
	IDXGISwapChain* GetSwapChain() const { return swapChain.Get(); }

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	// ���f���V�F�[�_�[�擾
	ModelShader* GetShader(ModelShaderId id) const { return modelShaders[static_cast<int>(id)].get(); }

	// �X�v���C�g�V�F�[�_�[�擾
	SpriteShader* GetShader(SpriteShaderId id) const { return spriteShaders[static_cast<int>(id)].get(); }

	// �X�N���[�����擾
	float GetScreenWidth() const { return screenWidth; }

	// �X�N���[�������擾
	float GetScreenHeight() const { return screenHeight; }

	// �f�o�b�O�v���~�e�B�u�擾
	DebugPrimitive* GetDebugPrimitive() const { return debugPrimitive.get(); }

	// ���C�������_���擾
	LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

	Light* GetLight()const { return light.get(); }
	//// ImGui�����_���擾
	//ImGuiRenderer* GetImGuiRenderer() const { return imguiRenderer.get(); }

	// �����_�[�X�e�[�g�擾
	RenderState* GetRenderState() { return renderState.get(); }

	std::mutex& GetMutex() { return mutex; }

private:
	float	screenWidth = 0;
	float	screenHeight = 0;

	// DirectX11�ŗ��p����l�X�ȃ��\�[�X�̍쐬
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	// �l�X�ȕ`�施�߂�GPU�ɓ`����
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	dc;

	// �L�����p�X�ɕ`��������z(�E�B���h�E)�ɓ����
	Microsoft::WRL::ComPtr<IDXGISwapChain>	swapChain;

	// �F���������݃L�����p�X
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>	depthStencilBuffer;

	// ���s�����������݃L�����p�X
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;

	D3D11_VIEWPORT viewport{};

	std::mutex mutex;

	std::unique_ptr<RenderState> renderState;
	
	std::unique_ptr<DebugPrimitive>					debugPrimitive;

	std::unique_ptr<LineRenderer>					lineRenderer;

	std::unique_ptr<Light> light;
	std::unique_ptr<ModelShader>					modelShaders[static_cast<int>(ModelShaderId::Max)];
	std::unique_ptr<SpriteShader>					spriteShaders[static_cast<int>(SpriteShaderId::Max)];
};