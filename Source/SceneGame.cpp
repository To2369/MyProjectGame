#include"SceneGame.h"
#include"Graphics/RenderContext.h"
#include"Graphics/RenderState.h"
#include "EnemySlime.h"
#include "EnemyManager.h"
#include "StageManager.h"

//#include "Effect/EffectManager.h"
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
    camera->SetLookAt(
        DirectX::XMFLOAT3(0, 10, -10),		//カメラの視点(位置)
        DirectX::XMFLOAT3(0, 0, 0),			//カメラの注視点(ターゲット)
        DirectX::XMFLOAT3(0, 1, 0)			//カメラの上方向
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

    //敵管理クラス取付
    EnemyManager& eneMgr = EnemyManager::Instance();
    eneMgr.Regist(std::move(slime));

    // エフェクト管理の初期化
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
}

//終了化
void SceneGame::Finalize()
{
    EnemyManager::Instance().Clear();
}

//更新処理
void SceneGame::Update(float elapsedTime)
{
    player->Update(elapsedTime);

    EnemyManager::Instance().Update(elapsedTime);

    // エフェクト更新処理
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
    rc.lightDirection = light_direction;	// ライト方向（下方向）
    //3Dモデルの描画に必要な情報
    Scene_constants scene_data{};
    // ビュー行列
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&rc.view);
    // プロジェクション行列
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&rc.projection);
    DirectX::XMStoreFloat4x4(&scene_data.viewProjection, View * Projection);
    scene_data.lightDirection = rc.lightDirection;

    // 3D 描画設定
    rc.renderState->GetSamplerState(SAMPLER_STATE::ANISOTROPIC);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::ON_ON), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));

    framebuffers[0]->Clear(dc, 0,0,1,0);
    framebuffers[0]->Activate(dc);

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
        //EffectManager::Instance().Render(&rc.view, &rc.projection);
    }

    // 3Dデバッグ描画
    {
        graphics->GetLineRenderer()->Render(dc, rc.view, rc.projection);
        graphics->GetDebugRenderer()->Render(dc, rc.view, rc.projection);
        graphics->GetDebugPrimitive()->Render(dc);

        player->DrawDebugPrimitive();

        EnemyManager::Instance().DrawDebugPrimitive();
    }

    // フレームバッファ
    {
        framebuffers[0]->Deactivate(dc);
    }

    // ポストエフェクト
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::ADD), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    {
        SpriteShader* shader = graphics->GetShader(SpriteShaderId::ColorGrading);
        shader->Begin(rc);

        rc.colorGradingData = colorGradingData;

        sprite->SetShaderResourceView(framebuffers[0]->shader_resource_views[0], 1280, 720);
        shader->Draw(rc, sprite.get());

        shader->End(rc);
    }

    // 2D 描画設定
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // 2D 描画
    {
#ifdef USE_IMGUI
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
        DrawGauge(dc);
    }
    // 2DデバッグGUI描画
    {

}
}

void SceneGame::DrawGauge(ID3D11DeviceContext* dc)
{
    // ゲージの長さ
    const float lifegaugeWidth = 600.0f;
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