#pragma once

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//extern ImWchar glyphRangesJapanese[];

#include<wrl.h>
#include<DirectXMath.h>
#include"Graphics/Graphics.h"
#include"Graphics/RenderState.h"
#include"Graphics/Buffer.h"
class Scene
{
public:
    Scene(){}
    virtual ~Scene(){}

    //������
    virtual void Initialize() = 0;

    //�I����
    virtual void Finalize() = 0;

    //�X�V����
    virtual void Update(float elapsedTime) = 0;

    //�`��X�V����
    virtual void Render() = 0;

    //�����������Ă��邩
    bool IsReady() { return ready; }

    //���������ݒ�
    void SetReady() { ready = true; }
private:
    bool ready = false;
};