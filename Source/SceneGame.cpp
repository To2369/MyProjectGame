#include "Graphics/RenderContext.h"
#include "Graphics/RenderState.h"
#include "Graphics/Shader.h"
#include "Input/InputManager.h"
#include "Effect/EffectManager.h"

#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "SceneTitle.h"

#include "EnemyHuman.h"
#include "EnemyManager.h"
#include "StageManager.h"
//初期化
void SceneGame::Initialize()
{
    Graphics* graphics = Graphics::Instance();

    //定数バッファの作成
    CreateBuffer<SceneGame::Scene_constants>(graphics->GetDevice(), buffer.GetAddressOf());
   
    camera = &Camera::Instance();
    float x, y;
    x = static_cast<float>(graphics->GetScreenWidth());
    y = static_cast<float>(graphics->GetScreenHeight());
    aspect = x / y;
    camera->SetLookAt(
        cameraEye,		//カメラの視点(位置)
        cameraFocus,			//カメラの注視点(ターゲット)
        cameraUp			//カメラの上方向
    );
    camera->SetPerspectiveFov(
        cameraFovY,
        aspect,
        nearZ,
        farZ
    );
    cameraCtrl = std::make_unique<CameraController>();
    for (int i = 0; i < 1; i++)
    {
        std::unique_ptr<EnemyHuman> slime = std::make_unique<EnemyHuman>();
        slime->SetPosition({ 0,0,5 });

        //敵管理クラス取付
        EnemyManager& eneMgr = EnemyManager::Instance();
        eneMgr.Regist(std::move(slime));
    }

    // エフェクト管理の初期化
    //EffectManager::Instance().Initialize();

    stage = std::make_unique<Stage>();
    StageManager::Instance().Regist(stage.get());

    player = std::make_unique<Player>();

    lifegauge= std::make_unique<Sprite>(graphics->GetDevice(),nullptr);
    skillEnergyGauge = std::make_unique<Sprite>(graphics->GetDevice(), L".\\Data\\Sprite\\SkillGauge.png");
    spritEnergyGauge = std::make_unique<Sprite>(graphics->GetDevice(),nullptr);
    //shadowMapCaster = std::make_unique<ShadowMapCaster>(graphics->GetDevice());
    //colorGrading = std::make_unique<ColorGraging>(graphics->GetDevice());
    framebuffers[0] = std::make_unique<FrameBuffer>(graphics->GetDevice(), 1920, 1080);
    framebuffers[1] = std::make_unique<FrameBuffer>(graphics->GetDevice(), 1920 / 2, 1080 / 2);

    skyMap = std::make_unique<SkyMap>(graphics->GetDevice(), L".\\Data\\sor_lake1.dds");
    // オフスクリーン描画用のシェーダーリソースビュー描画用のスプライトの作成
    bit_block_transfer = std::make_unique<FullScreenQuad>(graphics->GetDevice());

    sprite = std::make_unique<Sprite>(graphics->GetDevice(), L".\\Data\\Sprite\\screenshot.jpg");
    skillArtsSellect = std::make_unique<Sprite>(graphics->GetDevice(), L".\\Data\\Fonts\\font4.png");
    mask = std::make_unique<Mask>(graphics->GetDevice());

    reset = false;
}

//終了化
void SceneGame::Finalize()
{
    EnemyManager::Instance().Clear();
    //StageManager::Instance().Clear();
}

//更新処理
void SceneGame::Update(float elapsedTime)
{
    Mouse* mouse = InputManager::Instance()->getMouse();

    if (mouse->GetButton() & Mouse::BTN_LEFT&& mouse->GetButtonDown() & Mouse::BTN_RIGHT)
    {
    
        {
            play = true;
            SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
        }

    }

    player->Update(elapsedTime);

    EnemyManager::Instance().Update(elapsedTime);
    DirectX::XMFLOAT3 target = player->GetPosition();
    target.y += 0.5f;
    cameraCtrl->SetTarget(target);
    cameraCtrl->Update(elapsedTime);

#ifdef USE_IMGUI
    // 視点移動のONOFF
    if (GetAsyncKeyState('P') & 1)
    {
        if (!mouseMoveFlag)
        {
            mouseMoveFlag = true;
        }
        else
        {
            mouseMoveFlag = false;
        }
    }

    cameraCtrl->SetMouseMoveFlag(mouseMoveFlag);

    // カメラ移動テスト
    if (GetAsyncKeyState('1') & 1)
    {
        if (!play)
        {
            play = true;
        }
        else
        {
            play = false;
        }

    }
    cameraCtrl->SetCutInFlag(play);
#endif

    stage->Update(elapsedTime);

#ifdef USE_IMGUI

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
    Graphics* graphics = Graphics::Instance();
    ImGui::Begin("ImGUI");
    player->DrawDebugGUI();
    EnemyManager::Instance().DrawDebugGUI();
    cameraCtrl->DrawDebugGUI();
    ImGui::Separator();
    if (ImGui::TreeNode("Mask"))
    {
        //ImGui::SliderFloat("dissolve_value", &mask.get()->dissolve_value, 0.0f, +1.0f);
        //ImGui::Checkbox("utility flag", &flag);

        ImGui::TreePop();
    }
    ImGui::Separator();

    ImGui::End();
#endif

    // エフェクト更新処理
    EffectManager::Instance().Update(elapsedTime);
}

//描画
void SceneGame::Render()
{
    Graphics* graphics = Graphics::Instance();
    RenderState* renderState = graphics->GetRenderState();

    ID3D11DeviceContext* dc = graphics->GetDeviceContext();

    //ビューポート取得
    D3D11_VIEWPORT viewport;
    UINT num_viewports{ 1 };
    dc->RSGetViewports(&num_viewports, &viewport);

    //描画コンテキスト
    RenderContext rc;
    rc.deviceContext = dc;
    rc.renderState = renderState;
    rc.view = camera->GetView();
    rc.projection = camera->GetProjection();
    //3Dモデルの描画に必要な情報
    Scene_constants scene_data{};
    // ビュー行列
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&rc.view);
    // プロジェクション行列
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&rc.projection);
    DirectX::XMStoreFloat4x4(&scene_data.viewProjection, View * Projection);
    scene_data.lightDirection = graphics->GetLight()->directionalLightDirection;

   /* framebuffers[0]->Clear(dc, 0, 0, 1, 0);
    framebuffers[0]->Activate(dc);*/

   

    // 3D 描画設定
    ID3D11SamplerState* samplers[] =
    {
        rc.renderState->GetSamplerState(SAMPLER_STATE::POINT),
        rc.renderState->GetSamplerState(SAMPLER_STATE::LINEAR),
        rc.renderState->GetSamplerState(SAMPLER_STATE::ANISOTROPIC),
    };
    dc->PSSetSamplers(0, ARRAYSIZE(samplers), samplers);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));


    if (skyMap)
    {
        skyMap->Blit(graphics->GetDeviceContext(), scene_data.viewProjection);
    }

    dc->PSSetSamplers(0, ARRAYSIZE(samplers), samplers);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::ON_ON), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // 3D 描画
    {
        //定数バッファの登録
        BindBuffer(dc, 1, buffer.GetAddressOf(), &scene_data);

       

        stage->Render(dc);
        //shadowMapCaster->Render(dc);
        player->Render(dc);

        EnemyManager::Instance().Render(dc);
    }

    // 3D エフェクト描画
    {
        EffectManager::Instance().Render(&rc.view, &rc.projection);
    }

    // 3Dデバッグ描画
    {
        graphics->GetLineRenderer()->Render(dc, rc.view, rc.projection);

        graphics->GetDebugPrimitive()->Render(dc);

        player->DrawDebugPrimitive();

        EnemyManager::Instance().DrawDebugPrimitive();

        graphics->GetLineRenderer()->Render(dc, rc.view, rc.projection);

#ifdef USE_IMGUI
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    }

    // 2D 描画設定
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::SUB), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // 2D 描画
    {

        DrawGauge(dc, &rc);
        if (player->artSkillReady)
        {
            DrawSkillArtsSelect(dc, &rc);
        }
        if (player->artUltSkillReady)
        {
            DrawUltArtsSelect(dc, &rc);

        }
    }

    // フレームバッファ
    {
       /* framebuffers[0]->Deactivate(dc);
        dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
        dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
        bit_block_transfer->Blit(dc, framebuffers[0]->shaderResourceViews[static_cast<int>(SHADER_RESOURCE_VIEW::RenderTargetView)].GetAddressOf(), 0, 1);*/
    }
    // 2DデバッグGUI描画
    {

    }
}

void SceneGame::DrawGauge(ID3D11DeviceContext* dc, RenderContext* rc)
{
    dc->OMSetBlendState(rc->renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(rc->renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(rc->renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // ゲージの長さ
    const float lifegaugeWidth = 800.0f;
    const float lifegaugeHeight = 50.0f;

    const float spiritgaugeWidth = 500.0f;
    const float spiritgaugeHeight = 25.0f;

    const float skillgaugeWidth = 500.0f;
    const float skillgaugeHeight = 25.0f;

    float healthRate = player->GetHealth() / static_cast<float>(player->GetMaxHealth());
    float spiritEnergyRate = player->GetSpritEnergy() / static_cast<float>(player->GetMaxSpritEnergy());
    float skillEnergyRate = player->GetSkillEnergy() / static_cast<float>(player->GetMaxSkillEnergy());
    // ゲージ描画
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
    // 現在の技力ゲージ
    skillEnergyGauge->Render(
        dc,
        25,100,
        694*skillEnergyRate,
        16,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f,
        694,
        0,
        694,
        static_cast<float>(skillEnergyGauge->GetTextureHeight())
    );
    // 技力ゲージの囲い
    skillEnergyGauge->Render(
        dc,
        25,
        100,
        694,
        16,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f,
        0,
        0,
        694,
        static_cast<float>(skillEnergyGauge->GetTextureHeight())
    );
    spritEnergyGauge->Render(
        dc,
        50,
        150,
        spiritgaugeWidth * spiritEnergyRate,
        spiritgaugeHeight,
        0.0f, 0.3f, 1.0f, 1.0f,
        0.0f,
        0, 0,
        static_cast<float>(spritEnergyGauge->GetTextureWidth()),
        static_cast<float>(spritEnergyGauge->GetTextureHeight())
    );
}

void SceneGame::DrawSkillArtsSelect(ID3D11DeviceContext* dc,RenderContext*rc)
{
    dc->OMSetBlendState(rc->renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(rc->renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(rc->renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    skillArtsSellect->Textout(dc, "normalskill", 1000, 500, 16, 16, 0, 0, 1, 1);
}

void  SceneGame::DrawUltArtsSelect(ID3D11DeviceContext* dc, RenderContext* rc)
{
    dc->OMSetBlendState(rc->renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(rc->renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(rc->renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    skillArtsSellect->Textout(dc, "Ultskill", 1000, 500, 16, 16, 0, 0, 1, 1);
}