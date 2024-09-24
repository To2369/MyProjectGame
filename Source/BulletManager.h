#pragma once

#include "Bullet.h"
#include <vector>
// ’eŠÇ—
class BulletManager
{
public:
    BulletManager();
    ~BulletManager();

    void Update(float elpasedTime);

    void Render(ID3D11DeviceContext* dc);

    // ƒfƒoƒbƒOƒvƒŠƒ~ƒeƒBƒu•`‰æ
    void DrawDebugPrimitive();

    // ’e“o˜^
    void Regist(Bullet* bullet);

    // ’e‘Síœ
    void Clear();

    void Remove(Bullet* bullet);

    // ’e”æ“¾
    int GetBulletCount() const { return static_cast<int>(bullets.size()); }

    // ’eæ“¾
    Bullet* GetBullet(int index) { return bullets.at(index);}
private:
    std::vector<Bullet*> bullets;
    std::vector<Bullet*> removes;
};