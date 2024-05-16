#include"SceneTitle.h"
//初期化
void SceneTitle::Initialize()
{
    // シーン定数バッファの作成
    createBuffer<SceneTitle::SceneConstants>(DeviceManager::Instance()->GetDevice(), buffer.GetAddressOf());

    spr[0] = std::make_unique<Sprite>(DeviceManager::Instance()->GetDevice(), filename[0]);
    spr[1]= std::make_unique<Sprite>(DeviceManager::Instance()->GetDevice(), filename[1]);
}

//終了化
void SceneTitle::Finalize()
{

}
int a = 4;
void SceneTitle::Update(float elapsedTime)
{

#ifdef USE_IMGUI
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
    ImGui::Begin("ImGUI");

    ImGui::SliderInt("a", &a, 0, 4);
    ImGui::End();
#endif
}
void SceneTitle::Render()
{
    DeviceManager* deviceMgr = DeviceManager::Instance();
    GraphicsManager* graphicsMgr = GraphicsManager::Instance();

    ID3D11DeviceContext* dc = deviceMgr->GetDeviceContext();
    ID3D11RenderTargetView* rtv = deviceMgr->GetRenderTargetView();
    ID3D11DepthStencilView* dsv = deviceMgr->GetDepthStencilView();

    // 画面クリア＆レンダーターゲット設定
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // 3D モデルの描画に必要な情報
    //SceneConstants sc;

    // 3D 描画設定
    //graphicsMgr->SettingRenderContext([&](RenderContext& rc) {
    //    // サンプラーステートの設定（アニソトロピック）
    //    dc->PSSetSamplers(0, 1, rc.sampler_states[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
    //    // ブレンドステートの設定（アルファ）
    //    dc->OMSetBlendState(rc.blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
    //    // 深度ステンシルステートの設定（深度テストオン、深度書き込みオン）
    //    dc->OMSetDepthStencilState(rc.depth_stencil_states[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
    //    // ラスタライザステートの設定（ソリッド、裏面表示オフ）
    //    dc->RSSetState(rc.rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
    //    });
    
    // 3D 描画
    {
        // 定数バッファの登録
        //dc->UpdateSubresource(buffer.Get(), 0, 0, &sc, 0, 0);
        //dc->VSSetConstantBuffers(1, 1, buffer.GetAddressOf());
        //dc->PSSetConstantBuffers(1, 1, buffer.GetAddressOf());

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
    // 2D 描画設定
    graphicsMgr->SettingRenderContext([&](RenderContext& rc)
        {
            //サンプラーステートの設定（ポイント）
            dc->PSSetSamplers(0, 1, rc.sampler_states[static_cast<uint32_t>(SAMPLER_STATE::POINT)].GetAddressOf());
            //サンプラーステートの設定（リニア）
            //dc->PSSetSamplers(1, 1, rc.sampler_states[static_cast<uint32_t>(SAMPLER_STATE::POINT)].GetAddressOf());
            //サンプラーステートの設定（アニソトロピック）
            //dc->PSSetSamplers(2, 1, rc.sampler_states[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
            //ブレンドステートの設定（アルファ）
            //dc->OMSetBlendState(rc.blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
            //深度ステンシルステートの設定（深度テストオフ、深度書き込みオフ）
            dc->OMSetDepthStencilState(rc.depth_stencil_states[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 1);
            //ラスタライザステートの設定（ソリッド、裏面表示オフ）
            //dc->RSSetState(rc.rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
        });
    // 2D 描画
    {
        spr[0]->Render(dc, 0, 0, 1280, 720, 1, 1, 1, 1, 0);
        spr[1]->Render(dc, 500, 200, 200, 200, 1, 1, 1, 1, 0, 0, 0, 140, 240);
    }
    // 2DデバッグGUI描画
    {
#ifdef USE_IMGUI
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
    }
}