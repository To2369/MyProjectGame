#include"SceneTitle.h"
#include"Graphics/RenderContext.h"
#include"Graphics/RenderState.h"
//������
void SceneTitle::Initialize()
{
    Graphics* graphics = Graphics::Instance();

    //�萔�o�b�t�@�̍쐬
    CreateBuffer<SceneTitle::Scene_constants>(graphics->GetDevice(), buffer.GetAddressOf());

    camera = Camera::Instance();
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
    rc.camera = camera;
    rc.lightDirection = { 0.0f, 0.0f, 1.0f, 0.0f };	// ���C�g�����i�������j

    //3D���f���̕`��ɕK�v�ȏ��
    Scene_constants scene_data{};
    DirectX::XMStoreFloat4x4(&scene_data.viewProjection, DirectX::XMMatrixMultiply(
        DirectX::XMLoadFloat4x4(rc.camera->GetView()),
        DirectX::XMLoadFloat4x4(rc.camera->GetProjection())));
    scene_data.lightDirection = rc.lightDirection;
    
    // 2D �`��ݒ�
    rc.renderState->GetSamplerState(SAMPLER_STATE::POINT);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // 2D �`��
    {


        /* sprite_batches[0]->Begin(dc,sprite_batches[0]->GetReplaced_pixel_shader(),sprite_batches[0]->GetReplaced_Shader_resource_view());
         sprite_batches[0]->Render(dc,0,0,1280,720);
         sprite_batches[0]->End(dc);*/

         //dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
         //spr[0]->Textout(dc, "ECC", 0, 0, 16, 16, 1, 1, 1, 1);
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

        ////�X�e�[�W�̕`��
        //StageManager::Instance()->render(dc);

        ////�v���C���[�̕`��
        //PlayerManager::Instance()->render(dc);

        ////�G�`��
        //EnemyManager::instance()->render(dc);
    }

    // 3D�f�o�b�O�`��
    {

    }
}