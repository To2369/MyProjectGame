#pragma once
#include"Scene.h"
#include"Camera.h"
#include"CameraController.h"
#include"Graphics/FrameBuffer.h"
#include"Graphics/FullScreenQuad.h"
#include"Stage.h"
#include "Graphics/Sprite.h"
#include "Player.h"
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
public:
    // �V�[���萔
    struct Scene_constants
    {
        DirectX::XMFLOAT4X4		viewProjection;
        DirectX::XMFLOAT4		lightDirection;
        DirectX::XMFLOAT4		camera_position;
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

private:
    // �J�����̈ʒu
    DirectX::XMFLOAT4 camera_position{ 0.0f,0.0f,-3.0f,1.0f };

    //���C�g�̕���
    DirectX::XMFLOAT4 light_direction{ 0.0f,-1.0f,0.0f,0.0f };

    DirectX::XMFLOAT3 scaling{ 1,1,1 };
    DirectX::XMFLOAT3 rotation{ 0,0,0 };
    DirectX::XMFLOAT3 tramslation{ 0,0,0 };

    //�}�e���A���J���[
    DirectX::XMFLOAT4 material_color{ 1,1,1,1 };

    std::unique_ptr<Stage> stage;
    std::unique_ptr<Player> player;
};