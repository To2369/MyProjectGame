#pragma once
#include"Scene.h"
#include"Graphics/Sprite.h"
#include"Graphics/Sprite_batch.h"
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
    std::unique_ptr<Sprite_batch> sprite_batches[8];
    const wchar_t* filename[8] = { 
        L".\\Data\\Fonts\\font4.png",
        L".\\Data\\Sprite\\player-sprites.png",
     L".\\Data\\Sprite\\vice.png" };
};