#pragma once
#include"Scene.h"
#include"DeviceManager.h"
#include"Graphics/Sprite.h"
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

private:
    std::unique_ptr<Sprite> spr;
    const wchar_t* filename = L".\\Data\\Sprite\\cyberpunk.jpg";
};