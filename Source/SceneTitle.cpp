#include"SceneTitle.h"
#include"Graphics/RenderContext.h"
#include"Graphics/RenderState.h"
//初期化
void SceneTitle::Initialize()
{
    // シーン定数バッファの作成
    spr[0] = std::make_unique<Sprite>(Graphics::Instance()->GetDevice(), filename[0]);

    sprite_batches[0] = std::make_unique<Sprite_batch>(Graphics::Instance()->GetDevice(), filename[2],2048);
}

//終了化
void SceneTitle::Finalize()
{

}
int a = 0;
//更新処理
void SceneTitle::Update(float elapsedTime)
{

#ifdef USE_IMGUI
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
    ImGui::Begin("ImGUI");

    ImGui::SliderInt("a", &a, 0.0f, 1.0f);
    ImGui::End();
#endif
}

//描画
void SceneTitle::Render()
{
    Graphics* graphics = Graphics::Instance();
    RenderState* renderState = Graphics::Instance()->GetRenderState();

    ID3D11DeviceContext* dc = graphics->GetDeviceContext();

    RenderContext rc;
    rc.deviceContext = dc;
    rc.renderState = renderState;

    // 画面クリア＆レンダーターゲット設定
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));


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
    rc.renderState->GetSamplerState(SAMPLER_STATE::POINT);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // 2D 描画
    {
       

        sprite_batches[0]->Begin(dc,sprite_batches[0]->GetReplaced_pixel_shader(),sprite_batches[0]->GetReplaced_Shader_resource_view());
        sprite_batches[0]->Render(dc,0,0,1280,720);
        sprite_batches[0]->End(dc);

        dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
        spr[0]->Textout(dc, "ECC", 0, 0, 16, 16, 1, 1, 1, 1);
    }
    // 2DデバッグGUI描画
    {
#ifdef USE_IMGUI
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
    }
}