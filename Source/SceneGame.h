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
#include "Graphics/ShadowMapCaster.h"
#include "PostprocessingRenderer.h"
#include "Graphics/ColorGrading.h"
#include "Graphics/RenderContext.h"
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

    void DrawGauge(ID3D11DeviceContext* dc);
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

    Camera* camera;
    std::unique_ptr<CameraController> cameraCtrl;
    //std::unique_ptr<ShadowMapCaster> shadowMapCaster;
    std::unique_ptr<Sprite> lifegauge;
    std::unique_ptr<Sprite> skillEnergyGauge;
    std::unique_ptr<Sprite> spritEnergyGauge;
    ColorGradingData colorGradingData;
    std::unique_ptr<Sprite> sprite;
private:
    //���C�g�̕���
    DirectX::XMFLOAT4 light_direction{ 0.0f,-1.0f,0.0f,0.0f };

    std::unique_ptr<Stage> stage;
    std::unique_ptr<Player> player;
    std::unique_ptr<Light> light;
    // �|�X�g�v���Z�X
    std::unique_ptr<PostprocessingRenderer> postprocessingRenderer;
};