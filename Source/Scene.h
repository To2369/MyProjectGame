#pragma once
#ifdef USE_IMGUI

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//extern ImWchar glyphRangesJapanese[];
#endif
#include<wrl.h>
class Scene
{
public:
    Scene(){}
    virtual ~Scene(){}

    //初期化
    virtual void Initialize() = 0;

    //終了化
    virtual void Finalize() = 0;

    //更新処理
    virtual void Update(float elapsedTime) = 0;

    //描画更新勝利
    virtual void Render() = 0;

    //準備完了しているか
    bool ISReady() { return ready; }

    //準備完了設定
    void SetReady() { ready = true; }

private:
    bool ready = false;
};