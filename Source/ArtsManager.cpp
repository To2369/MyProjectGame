#include "ArtsManager.h"

ArtsManager::ArtsManager()
{

}

ArtsManager::~ArtsManager()
{
    Clear();
}

// �X�V����
void ArtsManager::Update(float elapsedTime)
{
    for (Arts* arts : artses)
    {
        arts->Update(elapsedTime);
    }

    // �j������
    for (Arts* arts : removes)
    {
        auto itr = std::find(artses.begin(), artses.end(), arts);
        if (itr != artses.end())
        {
            artses.erase(itr);
        }
        // bullets ����폜
        delete arts;
    }
    // �j�����X�g���N���A
    removes.clear();
}

// �`�揈��
void ArtsManager::Render(ID3D11DeviceContext* dc)
{
    for (Arts* arts : artses)
    {
        arts->Render(dc);
    }
}

// �f�o�b�O�v���~�e�B�u�`��
//void ArtsManager::DrawDebugPrimitive()
//{
//    for (Arts* arts : artses)
//    {
//        arts->DrawDebugPrimitive();
//    }
//}

// �e�o�^
void ArtsManager::Regist(Arts* arts)
{
    artses.emplace_back(arts);
}

// �e�S�폜
void ArtsManager::Clear()
{
    for (Arts* arts : artses)
    {
        delete arts;
    }
    artses.clear();
}

// �e�폜
void ArtsManager::Remove(Arts* arts)
{
    auto itr = std::find(removes.begin(), removes.end(), arts);

    // �d�����Ă�����ǉ����Ȃ�
    if (itr != removes.end())
        return;

    // �j�����X�g�ɒǉ�
    removes.push_back(arts);
}