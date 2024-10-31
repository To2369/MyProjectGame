#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "Graphics/FrameBuffer.h"
#include "Graphics/FullScreenQuad.h"
#include "Graphics/FrameBuffer.h"
#include <memory>

struct SceneViewData
{
	std::shared_ptr<FrameBuffer>	fb;
};

// �|�X�g�v���Z�X�p�̃����_���[
class PostprocessingRenderer
{
public:
	PostprocessingRenderer();
	~PostprocessingRenderer();

	//	�`��
	void	Render(ID3D11DeviceContext* deviceContext);

	//	�V�[�����̐ݒ�
	void	SetSceneData(SceneViewData sceneViewData);

	//	�f�o�b�O���̕\��
	void	DrawDebugGUI();

private:
	std::unique_ptr<FrameBuffer> framebuffers[8];
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[8];
};
