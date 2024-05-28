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
#include<DirectXMath.h>
#include"Graphics/Graphics.h"
#include"Graphics/RenderState.h"
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
    bool ISReady() { return ready; }

    //���������ݒ�
    void SetReady() { ready = true; }

public:
    // �V�[���萔
    struct SceneConstants
    {
        DirectX::XMFLOAT4X4		viewProjection;
        DirectX::XMFLOAT4		lightDirection;
    };
    // �V�[���萔�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
private:
    bool ready = false;
};