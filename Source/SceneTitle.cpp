#include"SceneTitle.h"
#include"Graphics/RenderContext.h"
#include"Graphics/RenderState.h"
//初期化
void SceneTitle::Initialize()
{
    Graphics* graphics = Graphics::Instance();

    //定数バッファの作成
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
        DirectX::XMFLOAT3(0, 0, -10),		//カメラの視点(位置)
        DirectX::XMFLOAT3(0, 0, 0),			//カメラの注視点(ターゲット)
        DirectX::XMFLOAT3(0, 1, 0)			//カメラの上方向
    );
    cameraCtrl = std::make_unique<CameraController>();


}

//終了化
void SceneTitle::Finalize()
{

}

//更新処理
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

//描画
void SceneTitle::Render()
{
    Graphics* graphics = Graphics::Instance();
    RenderState* renderState = graphics->GetRenderState();

    ID3D11DeviceContext* dc = graphics->GetDeviceContext();

    // 画面クリア＆レンダーターゲット設定
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));

    //ビューポート取得
    D3D11_VIEWPORT viewport;
    UINT num_viewports{ 1 };
    dc->RSGetViewports(&num_viewports, &viewport);

    //描画コンテキスト
    RenderContext rc;
    rc.deviceContext = dc;
    rc.renderState = renderState;
    rc.camera = camera;
    rc.lightDirection = { 0.0f, 0.0f, 1.0f, 0.0f };	// ライト方向（下方向）

    //3Dモデルの描画に必要な情報
    Scene_constants scene_data{};
    DirectX::XMStoreFloat4x4(&scene_data.viewProjection, DirectX::XMMatrixMultiply(
        DirectX::XMLoadFloat4x4(rc.camera->GetView()),
        DirectX::XMLoadFloat4x4(rc.camera->GetProjection())));
    scene_data.lightDirection = rc.lightDirection;
    
    // 2D 描画設定
    rc.renderState->GetSamplerState(SAMPLER_STATE::POINT);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // 2D 描画
    {


        /* sprite_batches[0]->Begin(dc,sprite_batches[0]->GetReplaced_pixel_shader(),sprite_batches[0]->GetReplaced_Shader_resource_view());
         sprite_batches[0]->Render(dc,0,0,1280,720);
         sprite_batches[0]->End(dc);*/

         //dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
         //spr[0]->Textout(dc, "ECC", 0, 0, 16, 16, 1, 1, 1, 1);
    }
    // 2DデバッグGUI描画
    {
#ifdef USE_IMGUI
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
    }

    // 3D 描画設定
    rc.renderState->GetSamplerState(SAMPLER_STATE::ANISOTROPIC);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::ON_ON), 1);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));

    // 3D 描画
    {
        //定数バッファの登録
        BindBuffer(dc, 1, buffer.GetAddressOf(), &scene_data);

        ////ステージの描画
        //StageManager::Instance()->render(dc);

        ////プレイヤーの描画
        //PlayerManager::Instance()->render(dc);

        ////敵描画
        //EnemyManager::instance()->render(dc);
    }

    // 3Dデバッグ描画
    {

    }
}