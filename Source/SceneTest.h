#pragma once
#include"Scene.h"
#include"Graphics/Sprite.h"
#include"Graphics/Sprite_batch.h"
#include"Camera.h"
#include"Graphics/GeometricPrimitive.h"
#include"Graphics\StaticMesh.h"
#include"CameraController.h"
//�^�C�g���V�[��
class SceneTest :public Scene
{
public:
    SceneTest() {}
    ~SceneTest()override {}

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
    };
    // �V�[���萔�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
private:
    std::unique_ptr<Sprite> spr[8];
    std::unique_ptr<Sprite_batch> sprite_batches[8];
    std::unique_ptr<GeometricPrimitive> geometric_primitives[8];
    std::unique_ptr<StaticMesh> static_mesh[8];
    const wchar_t* filename[8] = {
        L".\\Data\\Fonts\\font4.png",
        L".\\Data\\Sprite\\player-sprites.png",
     L".\\Data\\Sprite\\vice.png" };

    const wchar_t* modelfilename[8] = {
        L".\\Data\\resources\\torus.obj"
    };
    Camera* camera;
    std::unique_ptr<CameraController> cameraCtrl;

private:
    // �J�����̈ʒu
    DirectX::XMFLOAT4 camera_position{ 0.0f,0.0f,-10.0f,1.0f };

    //���C�g�̕���
    DirectX::XMFLOAT4 light_direction{ 0.0f,0.0f,1.0f,0.0f };

    DirectX::XMFLOAT3 scaling{ 1,1,1 };
    DirectX::XMFLOAT3 rotation{ 0,0,0 };
    DirectX::XMFLOAT3 tramslation{ 0,0,0 };

    //�}�e���A���J���[
    DirectX::XMFLOAT4 material_color{ 1,1,1,1 };
};