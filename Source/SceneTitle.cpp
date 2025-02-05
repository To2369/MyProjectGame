#include"SceneTitle.h"
#include"Graphics/RenderContext.h"
#include"Graphics/RenderState.h"
#include "Input/InputManager.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "SceneGame.h"
//������
void SceneTitle::Initialize()
{
    Graphics* graphics = Graphics::Instance();

    //�萔�o�b�t�@�̍쐬
    CreateBuffer<SceneTitle::Scene_constants>(graphics->GetDevice(), buffer.GetAddressOf());

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
    sprite = std::make_unique<Sprite>(graphics->GetDevice(), L".\\Data\\Sprite\\Title.png");
    push = std::make_unique<Sprite>(graphics->GetDevice(), L".\\Data\\Fonts\\font4.png");
}

//�I����
void SceneTitle::Finalize()
{

}

//�X�V����
void SceneTitle::Update(float elapsedTime)
{
    cameraCtrl->Update(elapsedTime);
#ifdef USE_IMGUI
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif

    GamePad* gamePad = InputManager::Instance()->getGamePad();

    // �Ȃɂ��{�^�����������烍�[�f�B���O�V�[���֐؂�ւ�
    const GamePadButton anyButton =
        GamePad::BTN_A
        | GamePad::BTN_B
        | GamePad::BTN_X
        | GamePad::BTN_Y
        ;
    if (gamePad->GetButtonDown() & anyButton)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
    }
#ifdef USE_IMGUI
    ImGui::Begin("ImGUI");
  
    ImGui::End();
#endif
}

//�`��
void SceneTitle::Render()
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

    //3D���f���̕`��ɕK�v�ȏ��
    Scene_constants scene_data{};
    DirectX::XMStoreFloat4x4(&scene_data.viewProjection, DirectX::XMMatrixMultiply(
        DirectX::XMLoadFloat4x4(&rc.view),
        DirectX::XMLoadFloat4x4(&rc.projection)));
    scene_data.lightDirection = graphics->GetLight()->directionalLightDirection;
    
    // 2D �`��ݒ�
    rc.renderState->GetSamplerState(SAMPLER_STATE::POINT);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // 2D �`��
    {
        sprite->Render(
            dc,
            0, 0,
            1920, 1080, 1, 1, 1, 1, 0);
        dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
        dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
        dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
        push->Textout(dc, "pushbutton", 600, 900, 64, 64, 1, 1, 1, 1);
    }
    // 2D�f�o�b�OGUI�`��
    {
#ifdef USE_IMGUI
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
    }

    // 3D �`��ݒ�
    rc.renderState->GetSamplerState(SAMPLER_STATE::ANISOTROPIC);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::ON_ON), 1);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));

    // 3D �`��
    {
        //�萔�o�b�t�@�̓o�^
        BindBuffer(dc, 1, buffer.GetAddressOf(), &scene_data);
    }

    // 3D�f�o�b�O�`��
    {

    }
}