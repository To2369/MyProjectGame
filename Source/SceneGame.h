#pragma once
#include"Scene.h"
#include"Camera.h"
#include"CameraController.h"
#include"Graphics/FrameBuffer.h"
#include"Graphics/FullScreenQuad.h"
#include"Stage.h"
#include "Graphics/Sprite.h"
#include "Player.h"
#include "Graphics/Light.h"
#include "PostprocessingRenderer.h"
#include "Graphics/RenderContext.h"
#include "Graphics\Light.h"
#include "Graphics\Mask.h"
#include "Graphics/SkyMap.h"
//�^�C�g���V�[��
class SceneGame :public Scene
{
public:
    SceneGame() {}
    ~SceneGame()override {}

    //������
    void Initialize()override;

    //�I����
    void Finalize()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //�`��X�V
    void Render()override;

    void DrawGauge(ID3D11DeviceContext* dc, RenderContext* rc);

    void DrawUltArtsSelect(ID3D11DeviceContext* dc, RenderContext* rc);

    void DrawSkillArtsSelect(ID3D11DeviceContext* dc, RenderContext* rc);
public:
    // �V�[���萔
    struct Scene_constants
    {
        DirectX::XMFLOAT4X4		viewProjection;
        DirectX::XMFLOAT4		lightDirection;
        DirectX::XMFLOAT4		cameraPosition;
    };
    // �V�[���萔�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;


private:
    std::unique_ptr<FrameBuffer> framebuffers[8];
    std::unique_ptr<FullScreenQuad> bit_block_transfer;

    Camera* camera;
    std::unique_ptr<CameraController> cameraCtrl;

    std::unique_ptr<Sprite> lifegauge;
    std::unique_ptr<Sprite> skillEnergyGauge;
    std::unique_ptr<Sprite> spritEnergyGauge;
    std::unique_ptr<Sprite> sprite;

    std::unique_ptr<Sprite> skillArtsSellect;
    std::unique_ptr<Mask> mask;
    std::unique_ptr<SkyMap> skyMap = nullptr;

private:
    //���C�g�̕���
    DirectX::XMFLOAT4 light_direction{ 0.0f,-1.0f,0.0f,0.0f };

    std::unique_ptr<Stage> stage;
    std::unique_ptr<Player> player;
    // �|�X�g�v���Z�X
    std::unique_ptr<PostprocessingRenderer> postprocessingRenderer;

    // �J�����֌W
    DirectX::XMFLOAT3 cameraEye = { 0,10,-10 };
    DirectX::XMFLOAT3 cameraFocus = { 0,0,0 };
    DirectX::XMFLOAT3 cameraUp = { 0,1,0 };

    float cameraFovY = DirectX::XMConvertToRadians(45);
    float aspect = 0;
    float nearZ = 0.1f;
    float farZ = 1000.0f;

    bool mouseMoveFlag = true;
    bool play = false;

    bool reset = false;
};