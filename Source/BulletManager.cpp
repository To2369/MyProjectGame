#include "BulletManager.h"

BulletManager::BulletManager()
{

}

BulletManager::~BulletManager()
{
   Clear();
}

// �X�V����
void BulletManager::Update(float elapsedTime)
{
    for (Bullet* bullet : bullets)
    {
        bullet->Update(elapsedTime);
    }

    // �j������
    for (Bullet* bullet : removes)
    {
        auto itr = std::find(bullets.begin(), bullets.end(), bullet);
        if (itr != bullets.end())
        {
            bullets.erase(itr);
        }
        // bullets ����폜
        delete bullet;
    }
    // �j�����X�g���N���A
    removes.clear();
}

// �`�揈��
void BulletManager::Render(ID3D11DeviceContext* dc)
{
    for (Bullet* bullet : bullets)
    {
        bullet->Render(dc);
    }
}

// �f�o�b�O�v���~�e�B�u�`��
void BulletManager::DrawDebugPrimitive()
{
    for (Bullet* bullet : bullets)
    {
        bullet->DrawDebugPrimitive();
    }
}

// �e�o�^
void BulletManager::Regist(Bullet* bullet)
{
    bullets.emplace_back(bullet);
}

// �e�S�폜
void BulletManager::Clear()
{
    for (Bullet* bullet : bullets)
    {
        delete bullet;
    }
    bullets.clear();
}

// �e�폜
void BulletManager::Remove(Bullet* bullet)
{
    auto itr = std::find(removes.begin(), removes.end(), bullet);

    // �d�����Ă�����ǉ����Ȃ�
    if (itr != removes.end())
        return;

    // �j�����X�g�ɒǉ�
    removes.push_back(bullet);
}