#pragma once

#include "Bullet.h"
#include <vector>
// �e�Ǘ�
class BulletManager
{
public:
    BulletManager();
    ~BulletManager();

    void Update(float elpasedTime);

    void Render(ID3D11DeviceContext* dc);

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �e�o�^
    void Regist(Bullet* bullet);

    // �e�S�폜
    void Clear();

    void Remove(Bullet* bullet);

    // �e���擾
    int GetBulletCount() const { return static_cast<int>(bullets.size()); }

    // �e�擾
    Bullet* GetBullet(int index) { return bullets.at(index);}
private:
    std::vector<Bullet*> bullets;
    std::vector<Bullet*> removes;
};