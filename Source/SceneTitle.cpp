#include"SceneTitle.h"

//初期化
void SceneTitle::Initialize()
{
    
}

//終了化
void SceneTitle::Finalize()
{

}

void SceneTitle::Update(float elapsedTime)
{
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

void SceneTitle::Render()
{
    DeviceManager* mgr = DeviceManager::Instance();

    ID3D11DeviceContext* dc = mgr->GetDeviceContext();
    ID3D11RenderTargetView* rtv = mgr->GetRenderTargetView();
    ID3D11DepthStencilView* dsv = mgr->GetDepthStencilView();

    // 画面クリア＆レンダーターゲット設定
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

#ifdef USE_IMGUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
}