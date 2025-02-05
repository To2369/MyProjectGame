#include <thread>
#include "Graphics/Graphics.h"
#include "Input/InputManager.h"
#include "SceneLoading.h"
#include "SceneManager.h"
// ������
void SceneLoading::Initialize()
{
    Graphics* graphics = Graphics::Instance();

    //�萔�o�b�t�@�̍쐬
    CreateBuffer<SceneLoading::Scene_constants>(graphics->GetDevice(), buffer.GetAddressOf());

    camera = &Camera::Instance();
    float x, y;
    x = static_cast<float>(graphics->GetScreenWidth());
    y = static_cast<float>(graphics->GetScreenHeight());
    camera->SetPerspectiveFov(
        DirectX::XMConvertToRadians(30),
        x / y,
        0.1f,
        100.0f
    );
    camera->SetLookAt(
        DirectX::XMFLOAT3(0, 0, -10),		//�J�����̎��_(�ʒu)
        DirectX::XMFLOAT3(0, 0, 0),			//�J�����̒����_(�^�[�Q�b�g)
        DirectX::XMFLOAT3(0, 1, 0)			//�J�����̏����
    );
    cameraCtrl = std::make_unique<CameraController>();
    sprite = std::make_unique<Sprite>(graphics->GetDevice(), L".\\Data\\Sprite\\LoadingIcon.png");

	// �X���b�h�J�n
	std::thread thread(LoadingThread, this);

	// �X���b�h�̊Ǘ������
	thread.detach();
}

// �I����
void SceneLoading::Finalize()
{
}

// �X�V����
void SceneLoading::Update(float elapsedTime)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;

	// ���̃V�[���̏���������������V�[����؂�ւ���
	if (nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(nextScene);
		nextScene = nullptr;
	}
}

// �`�揈��
void SceneLoading::Render()
{
    Graphics* graphics = Graphics::Instance();
    RenderState* renderState = graphics->GetRenderState();

    ID3D11DeviceContext* dc = graphics->GetDeviceContext();

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));

    //�r���[�|�[�g�擾
    D3D11_VIEWPORT viewport;
    UINT num_viewports{ 1 };
    dc->RSGetViewports(&num_viewports, &viewport);

    //�`��R���e�L�X�g
    RenderContext rc;
    rc.deviceContext = dc;
    rc.renderState = renderState;
    rc.view = camera->GetView();
    rc.projection = camera->GetProjection();

    // 2D �`��ݒ�
    rc.renderState->GetSamplerState(SAMPLER_STATE::POINT);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // 2D �`��
    {
        float screenWidth = 1920;
        float screenHeight = 1080;
        float textureWidth = static_cast<float>(sprite->GetTextureWidth());
        float textureHeight = static_cast<float>(sprite->GetTextureHeight());
        float positionX = screenWidth - textureWidth;
        float positionY = screenHeight - textureHeight;
        sprite->Render(dc,
            positionX, positionY, textureWidth, textureHeight,
            1, 1, 1, 1,
            angle,
            0,0,textureWidth, textureHeight);
    }
}

// ���[�f�B���O�X���b�h
void SceneLoading::LoadingThread(SceneLoading* scene)
{
	// COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
	CoInitialize(nullptr);

	// ���̃V�[���̏��������s��
	scene->nextScene->Initialize();

	// �X���b�h���I���O��COM�֘A�̏I����
	CoUninitialize();

	// ���̃V�[���̏��������ݒ�
	scene->nextScene->SetReady();

}
