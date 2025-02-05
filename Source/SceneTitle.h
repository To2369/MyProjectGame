#pragma once
#include"Scene.h"
#include"Graphics/Sprite.h"
#include"Graphics/SpriteBatch.h"
#include"Camera.h"
#include"Graphics/GeometricPrimitive.h"
#include"CameraController.h"
//�^�C�g���V�[��
class SceneTitle :public Scene
{
public:
    SceneTitle() {}
    ~SceneTitle()override {}

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
    std::unique_ptr<Sprite> sprite;
    std::unique_ptr<Sprite> push;
    std::unique_ptr<SpriteBatch> sprite_batches[8];
    const wchar_t* filename[8] = { 
        L".\\Data\\Fonts\\font4.png",
        L".\\Data\\Sprite\\player-sprites.png",
     L".\\Data\\Sprite\\vice.png" };
    Camera* camera;
    std::unique_ptr<CameraController> cameraCtrl;
};