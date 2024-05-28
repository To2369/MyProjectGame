#pragma once
#include"Scene.h"
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
    std::unique_ptr<Sprite> spr[8];
    const wchar_t* filename[8] = { L".\\Data\\Sprite\\cyberpunk.jpg", L".\\Data\\Sprite\\player-sprites.png" };
};