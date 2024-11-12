#include"SceneGame.h"
#include"Graphics/RenderContext.h"
#include"Graphics/RenderState.h"
#include "EnemySlime.h"
#include "EnemyManager.h"
#include "StageManager.h"
#include "Graphics\Shader.h"
//#include "Effect/EffectManager.h"
//初期化
void SceneGame::Initialize()
{
    // ポイントライト・スポットライトの初期位置設定
    {
        point_light[0].position.x = 10;
        point_light[0].position.y = 1;
        point_light[0].range = 10;
        point_light[0].color = { 1, 0, 0, 1 };
        point_light[1].position.x = -10;
        point_light[1].position.y = 1;
        point_light[1].range = 10;
        point_light[1].color = { 0, 1, 0, 1 };
        point_light[2].position.y = 1;
        point_light[2].position.z = 10;
        point_light[2].range = 10;
        point_light[2].position.y = 1;
        point_light[2].color = { 0, 0, 1, 1 };
        point_light[3].position.y = 1;
        point_light[3].position.z = -10;
        point_light[3].range = 10;
        point_light[3].color = { 1, 1, 1, 1 };
        point_light[4].range = 10;
        point_light[4].color = { 1, 1, 1, 1 };
        ZeroMemory(&point_light[5], sizeof(point_lights) * 3);

        spot_light[0].position = { 15, 3, 15, 0 };
        spot_light[0].direction = { -1, -1, -1, 0 };
        spot_light[0].range = 100;
        spot_light[0].color = { 1, 0, 0, 1 };
        spot_light[1].position = { -15, 3, 15, 0 };
        spot_light[1].direction = { +1, -1, -1, 0 };
        spot_light[1].range = 100;
        spot_light[1].color = { 0, 1, 0, 1 };
        spot_light[2].position = { 15, 3, -15, 0 };
        spot_light[2].direction = { -1, -1, +1, 0 };
        spot_light[2].range = 100;
        spot_light[2].color = { 0, 0, 1, 1 };
        spot_light[3].position = { -15, 3, -15, 0 };
        spot_light[3].direction = { +1, -1, +1, 0 };
        spot_light[3].range = 100;
        spot_light[3].color = { 1, 1, 1, 1 };
        ZeroMemory(&spot_light[4], sizeof(spot_lights) * 4);
    }


    Graphics* graphics = Graphics::Instance();

    //定数バッファの作成
    CreateBuffer<SceneGame::Scene_constants>(graphics->GetDevice(), buffer.GetAddressOf());
    CreateBuffer<SceneGame::light_constants>(graphics->GetDevice(), light_constant_buffer.GetAddressOf());
    CreateBuffer<SceneGame::gaussian_filter_constants>(graphics->GetDevice(), gaussian_filter_constant_buffer.GetAddressOf());
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
    //shadowMapCaster = std::make_unique<ShadowMapCaster>(graphics->GetDevice());
    //colorGrading = std::make_unique<ColorGraging>(graphics->GetDevice());
    framebuffers[0] = std::make_unique<FrameBuffer>(graphics->GetDevice(), 1280, 720);
    framebuffers[1] = std::make_unique<FrameBuffer>(graphics->GetDevice(), 1280 / 2, 720 / 2);
    sprite = std::make_unique<Sprite>(graphics->GetDevice(), L".\\Data\\resources\\screenshot.jpg");

    ShaderManager::Instance()->CreatePsFromCso(graphics->GetDevice(), ".\\Data\\Shader\\SpritePS.cso",
        gaussian_filter_pixel_shader.GetAddressOf());
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
    Graphics* graphics = Graphics::Instance();
    ImGui::Begin("ImGUI");
    player->DrawDebugGUI();
    EnemyManager::Instance().DrawDebugGUI();
    ImGui::SliderFloat("hueShift", &colorGradingData.hueShift, 0.0f, +360.0f);
    ImGui::SliderFloat("saturation", &colorGradingData.saturation, 0.0f, +2.0f);
    ImGui::SliderFloat("brightness", &colorGradingData.brightness, 0.0f, +2.0f);
    ImGui::SliderFloat4("light_direction", &graphics->GetLight()->directionalLightDirection.x, -1.0f, +1.0f);
    
    if (ImGui::TreeNode("points"))
    {
        for (int i = 0; i < 8; ++i)
        {
            std::string p = std::string("position") + std::to_string(i);
            ImGui::SliderFloat3(p.c_str(), &point_light[i].position.x, -10.0f, +10.0f);
            std::string c = std::string("color") + std::to_string(i);
            ImGui::ColorEdit3(c.c_str(), &point_light[i].color.x);
            std::string r = std::string("range") + std::to_string(i);
            ImGui::SliderFloat(r.c_str(), &point_light[i].range, 0.0f, +100.0f);
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("spots"))
    {
        for (int i = 0; i < 8; ++i)
        {
            std::string p = std::string("position") + std::to_string(i);
            ImGui::SliderFloat3(p.c_str(), &spot_light[i].position.x, -10.0f, +10.0f);
            std::string d = std::string("direction") + std::to_string(i);
            ImGui::SliderFloat3(d.c_str(), &spot_light[i].direction.x, 0.0f, +1.0f);
            std::string c = std::string("color") + std::to_string(i);
            ImGui::ColorEdit3(c.c_str(), &spot_light[i].color.x);
            std::string r = std::string("range") + std::to_string(i);
            ImGui::SliderFloat(r.c_str(), &spot_light[i].range, 0.0f, +1000.0f);
            std::string ic = std::string("inner") + std::to_string(i);
            ImGui::SliderFloat(ic.c_str(), &spot_light[i].innerCorn, -1.0f, +1.0f);
            std::string oc = std::string("outer") + std::to_string(i);
            ImGui::SliderFloat(oc.c_str(), &spot_light[i].outerCorn, -1.0f, +1.0f);
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("gaussian_filter"))
    {
        ImGui::SliderInt("kernel", &gaussian_filter_data.kernelSize, 1, KERNEL_MAX);
        ImGui::SliderFloat("sigma", &gaussian_filter_data.sigma, 1.0f, 10.0f);
        ImGui::TreePop();
    }
    ImGui::Separator();

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
    //3Dモデルの描画に必要な情報
    Scene_constants scene_data{};
    // ビュー行列
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&rc.view);
    // プロジェクション行列
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&rc.projection);
    DirectX::XMStoreFloat4x4(&scene_data.viewProjection, View * Projection);
    scene_data.lightDirection = graphics->GetLight()->directionalLightDirection;
    // 3D 描画設定
    ID3D11SamplerState* samplers[] =
    {
        rc.renderState->GetSamplerState(SAMPLER_STATE::POINT),
        rc.renderState->GetSamplerState(SAMPLER_STATE::LINEAR),
        rc.renderState->GetSamplerState(SAMPLER_STATE::ANISOTROPIC),
    };
    dc->PSSetSamplers(0, ARRAYSIZE(samplers), samplers);
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

        // 定数バッファの更新
        {
          /*  light_constants lights{};
            lights.ambient_color = ambient_color;
            lights.directional_light_direction = directional_light_direction;
            lights.directional_light_color = directional_light_color;
            memcpy_s(lights.point_light, sizeof(lights.point_light), point_light, sizeof(point_light));
            memcpy_s(lights.spot_light, sizeof(lights.spot_light), spot_light, sizeof(spot_light));
            dc->UpdateSubresource(light_constant_buffer.Get(), 0, 0, &lights, 0, 0);
            dc->VSSetConstantBuffers(2, 1, light_constant_buffer.GetAddressOf());
            dc->PSSetConstantBuffers(2, 1, light_constant_buffer.GetAddressOf());*/
        }
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

        graphics->GetLineRenderer()->Render(dc, rc.view, rc.projection);
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

        //rc.colorGradingData = colorGradingData;

        sprite->SetShaderResourceView(framebuffers[0]->shader_resource_views[0], 1280, 720);
        shader->Draw(rc, sprite.get());
        //sprite->Render(dc, 256, 128, 1280-256*2, 720-128*2, 1, 1, 1, 1, 0);
        //	ガウスフィルター
        {
          /*  gaussian_filter_data.textureSize = { 1280, 720 };
            calc_gaussian_filter_constant(gaussian_filter_constant, gaussian_filter_data);
            dc->UpdateSubresource(gaussian_filter_constant_buffer.Get(), 0, 0, &gaussian_filter_constant, 0, 0);
            dc->VSSetConstantBuffers(1, 1, gaussian_filter_constant_buffer.GetAddressOf());
            dc->PSSetConstantBuffers(1, 1, gaussian_filter_constant_buffer.GetAddressOf());
            dc->PSSetShader(gaussian_filter_pixel_shader.Get(), nullptr, 0);
            sprite->Render(dc, 0, 0, 1280, 720, 1, 1, 1, 1, 0);*/
        }
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

void SceneGame::calc_gaussian_filter_constant(gaussian_filter_constants& constant, const gaussian_filter_datas& data)
{
    //	偶数の場合は奇数に直す
    int kernelSize = data.kernelSize;
    if (kernelSize % 2 == 0)
        kernelSize++;
    constant.kernelSize = kernelSize;
    constant.texcel.x = 1.0f / data.textureSize.x;
    constant.texcel.y = 1.0f / data.textureSize.y;
    //	重みを算出
    float sum = 0.0f;
    int id = 0;
    for (int y = -kernelSize / 2; y <= kernelSize / 2; y++)
    {
        for (int x = -kernelSize / 2; x <= kernelSize / 2; x++)
        {
            constant.weights[id].x = (float)x;
            constant.weights[id].y = (float)y;
            constant.weights[id].z = (float)exp(-(x * x + y * y) / (2.0f * data.sigma * data.sigma)) / (2.0f * DirectX::XM_PI * data.sigma);
            sum += constant.weights[id].z;
            id++;
        }
    }
    //	平均化
    for (int i = 0; i < kernelSize * kernelSize; i++)
    {
        constant.weights[i].z /= sum;
    }

}