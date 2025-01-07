#pragma once
#include"Scene.h"
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
//�V�[���}�l�[�W���[
class SceneManager
{
private:
    SceneManager() {}
    ~SceneManager() {}
public:
    //�B��̃C���X�^���X�擾
    static SceneManager& Instance()
    {
        static SceneManager ins;
        return ins;
    }

    //�X�V����
    void Update(float elapsedTime);

    //�`��X�V
    void Render();

    //�V�[���N���A
    void Clear();

    //�V�[���؂�ւ�
    void ChangeScene(Scene* scene);
private:
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;
};