#include"SceneGame.h"
#include"Graphics/RenderContext.h"
#include"Graphics/RenderState.h"
#include "EnemySlime.h"
#include "EnemyManager.h"
#include "StageManager.h"

#include "Graphics/Shader.h"
//#include "Effect/EffectManager.h"
//������
void SceneGame::Initialize()
{
    Graphics* graphics = Graphics::Instance();

    //�萔�o�b�t�@�̍쐬
    CreateBuffer<SceneGame::Scene_constants>(graphics->GetDevice(), buffer.GetAddressOf());

    camera = &Camera::Instance();
    float x, y;
    x = static_cast<float>(graphics->GetScreenWidth());
    y = static_cast<float>(graphics->GetScreenHeight());
    camera->SetLookAt(
        DirectX::XMFLOAT3(0, 10, -10),		//�J�����̎��_(�ʒu)
        DirectX::XMFLOAT3(0, 0, 0),			//�J�����̒����_(�^�[�Q�b�g)
        DirectX::XMFLOAT3(0, 1, 0)			//�J�����̏����
    );
    camera->SetPerspectiveFov(
        DirectX::XMConvertToRadians(45),
        x / y,
        0.1f,
        1000.0f
    );
    cameraCtrl = std::make_unique<CameraController>();

    std::unique_ptr<EnemySlime> slime = std::make_unique<EnemySlime>();
    slime->SetPosition({ 0,0,5 });

    //�G�Ǘ��N���X��t
    EnemyManager& eneMgr = EnemyManager::Instance();
    eneMgr.Regist(std::move(slime));

    // �G�t�F�N�g�Ǘ��̏�����
    //EffectManager::Instance().Initialize();

    stage = std::make_unique<Stage>();
    StageManager::Instance().Regist(stage.get());

    player = std::make_unique<Player>();

    lifegauge= std::make_unique<Sprite>(graphics->GetDevice(),nullptr);
    skillEnergyGauge = std::make_unique<Sprite>(graphics->GetDevice(), nullptr);
    spritEnergyGauge = std::make_unique<Sprite>(graphics->GetDevice(), nullptr);
    light = std::make_unique<Light>(graphics->GetDevice());
    //shadowMapCaster = std::make_unique<ShadowMapCaster>(graphics->GetDevice());
    //colorGrading = std::make_unique<ColorGraging>(graphics->GetDevice());
    framebuffers[0] = std::make_unique<FrameBuffer>(graphics->GetDevice(), 1280, 720);

    framebuffers[1] = std::make_unique<FrameBuffer>(graphics->GetDevice(), 1280 / 2, 720 / 2);
    sprite = std::make_unique<Sprite>(graphics->GetDevice(), L".\\Data\\resources\\screenshot.jpg");
    // �I�t�X�N���[���`��p�̃V�F�[�_�[���\�[�X�r���[�`��p�̃X�v���C�g�̍쐬
    bit_block_transfer = std::make_unique<FullScreenQuad>(graphics->GetDevice());
    {
        //postprocessingRenderer = std::make_unique<PostprocessingRenderer>();
    }
    ShaderManager::Instance()->CreatePsFromCso(graphics->GetDevice(), ".\\Data\\Shader\\LuminanceExtractionPS.cso", pixel_shaders[0].GetAddressOf());
    ShaderManager::Instance()->CreatePsFromCso(graphics->GetDevice(), ".\\Data\\Shader\\BlurPS.cso", pixel_shaders[1].GetAddressOf());
    //ShaderManager::Instance()->CreateVsFromCso(graphics->GetDevice(),"")
    //ShaderManager::Instance()->CreateVsFromCso(graphics->GetDevice());
}

//�I����
void SceneGame::Finalize()
{
    EnemyManager::Instance().Clear();
}

//�X�V����
void SceneGame::Update(float elapsedTime)
{
    player->Update(elapsedTime);

    EnemyManager::Instance().Update(elapsedTime);

    // �G�t�F�N�g�X�V����
    //EffectManager::Instance().Update(elapsedTime);

    DirectX::XMFLOAT3 target = player->GetPosition();
    target.y += 0.5f;
    cameraCtrl->SetTarget(target);
    cameraCtrl->Update(elapsedTime);

    stage->Update(elapsedTime);

#ifdef USE_IMGUI

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
    ImGui::Begin("ImGUI");
    ImGui::SliderFloat3("cameraPos", &camera_position.x, -100.0f, 100.0f);
    player->DrawDebugGUI();
    light->Update(elapsedTime);
    EnemyManager::Instance().DrawDebugGUI();
    ImGui::SliderFloat("hueShift", &colorGradingData.hueShift, 0.0f, +360.0f);
    ImGui::SliderFloat("saturation", &colorGradingData.saturation, 0.0f, +2.0f);
    ImGui::SliderFloat("brightness", &colorGradingData.brightness, 0.0f, +2.0f);
    ImGui::SliderFloat3("light_direction", &light_direction.x, -1.0f, +1.0f);
    ImGui::End();
#endif
}

//�`��
void SceneGame::Render()
{
    Graphics* graphics = Graphics::Instance();
    RenderState* renderState = graphics->GetRenderState();

    ID3D11DeviceContext* dc = graphics->GetDeviceContext();

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
    rc.lightDirection = light_direction;	// ���C�g�����i�������j
#if 1
    //3D���f���̕`��ɕK�v�ȏ��
    Scene_constants scene_data{};
    // �r���[�s��
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&rc.view);
    // �v���W�F�N�V�����s��
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&rc.projection);
    DirectX::XMStoreFloat4x4(&scene_data.viewProjection, View * Projection);
    scene_data.lightDirection = rc.lightDirection;
#else//�Q�[���쐬�̎�����
    float aspect_ratio{ viewport.Width / viewport.Height };//����p�v�Z
    DirectX::XMMATRIX P{ DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30),aspect_ratio,0.1f,100.0f) };//�����s��i�����m�F�j

    DirectX::XMVECTOR eye{ DirectX::XMLoadFloat4(&camera_position) };
    DirectX::XMVECTOR focus{ DirectX::XMVectorSet(0.0f,0.0f,0.0f,1.0f) };
    DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f) };
    DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye,focus,up) };
    Scene_constants scene_data{};
    DirectX::XMStoreFloat4x4(&scene_data.viewProjection, V * P);
    scene_data.lightDirection = light_direction;
    scene_data.camera_position = camera_position;
#endif
    // 3D �`��ݒ�
    rc.renderState->GetSamplerState(SAMPLER_STATE::ANISOTROPIC);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::ON_ON), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));

    /*framebuffers[0]->Clear(dc);
    framebuffers[0]->Activate(dc);*/

    // 3D �`��
    {
        //�萔�o�b�t�@�̓o�^
        BindBuffer(dc, 1, buffer.GetAddressOf(), &scene_data);

        stage->Render(dc);
        //shadowMapCaster->Render(dc);
        player->Render(dc);

        EnemyManager::Instance().Render(dc);
    }

    // 3D �G�t�F�N�g�`��
    {
        //EffectManager::Instance().Render(&rc.view, &rc.projection);
    }

    // 3D�f�o�b�O�`��
    {
        graphics->GetLineRenderer()->Render(dc, rc.view, rc.projection);
        graphics->GetDebugRenderer()->Render(dc, rc.view, rc.projection);
        graphics->GetDebugPrimitive()->Render(dc);

        player->DrawDebugPrimitive();

        EnemyManager::Instance().DrawDebugPrimitive();
    }

    // �t���[���o�b�t�@
    {
        //framebuffers[0]->Deactivate(dc);
#if 0
        dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
        dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
        dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
        bit_block_transfer->Blit(dc, framebuffers[0]->shader_resource_views[static_cast<int>(SHADER_RESOURCE_VIEW::RenderTargetView)].GetAddressOf(), 0, 1);
#endif
      /*  dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
        dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
        dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));*/
        // 2D �`��

        
        //postprocessingRenderer->Render(dc);
      /*  dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
        dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
        framebuffers[1]->Clear(dc);
        framebuffers[1]->Activate(dc);
        bit_block_transfer->Blit(dc, framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1, pixel_shaders[0].Get());
        framebuffers[1]->Deactivate(dc);
        ID3D11ShaderResourceView* shaderResourceviews[2]
        { framebuffers[0]->shader_resource_views[0].Get(),framebuffers[1]->shader_resource_views[0].Get() };
        bit_block_transfer->Blit(dc, shaderResourceviews, 0, 2, pixel_shaders[1].Get());*/
    }

    // 2D �`��ݒ�
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::ADD), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // 2D �`��
    {
        SpriteShader* shader = graphics->GetShader(SpriteShaderId::ColorGrading);
        shader->Begin(rc);

        rc.colorGradingData = colorGradingData;
        shader->Draw(rc, sprite.get());

        shader->End(rc);
#ifdef USE_IMGUI
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
        // �Q�[�W�̒���
        const float lifegaugeWidth = 600.0f;
        const float lifegaugeHeight = 50.0f;

        const float spiritgaugeWidth = 500.0f;
        const float spiritgaugeHeight = 25.0f;

        const float skillgaugeWidth = 500.0f;
        const float skillgaugeHeight = 25.0f;

        float healthRate = player->GetHealth() / static_cast<float>(player->GetMaxHealth());
        float spiritEnergyRate = player->GetSpritEnergy() / static_cast<float>(player->GetMaxSpritEnergy());
        float skillEnergyRate = player->GetSkillEnergy() / static_cast<float>(player->GetMaxSkillEnergy());
        // �Q�[�W�`��
        lifegauge->Render(
            dc,
            50,
            25,
            lifegaugeWidth * healthRate,
            lifegaugeHeight,
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f,
            0, 0,
            static_cast<float>(lifegauge->GetTextureWidth()),
            static_cast<float>(lifegauge->GetTextureHeight())
        );
        skillEnergyGauge->Render(
            dc,
            50,
            75,
            skillgaugeWidth * skillEnergyRate,
            skillgaugeHeight,
            1.0f, 1.0f, 0.0f, 1.0f,
            0.0f,
            0, 0,
            static_cast<float>(skillEnergyGauge->GetTextureWidth()),
            static_cast<float>(skillEnergyGauge->GetTextureHeight())
        );
        spritEnergyGauge->Render(
            dc,
            50,
            100,
            spiritgaugeWidth * spiritEnergyRate,
            spiritgaugeHeight,
            0.0f, 0.3f, 1.0f, 1.0f,
            0.0f,
            0, 0,
            static_cast<float>(spritEnergyGauge->GetTextureWidth()),
            static_cast<float>(spritEnergyGauge->GetTextureHeight())
        );
    }
    // 2D�f�o�b�OGUI�`��
    {

}
}