#pragma once

#include "Arts.h"
#include <vector>
// ‹ZŠÇ—
class ArtsManager
{
public:
    ArtsManager();
    ~ArtsManager();

    void Update(float elpasedTime);

    void Render(ID3D11DeviceContext* dc);

    // ƒfƒoƒbƒOƒvƒŠƒ~ƒeƒBƒu•`‰æ
    void DrawDebugPrimitive();

    // ’e“o˜^
    void Regist(Arts* arts);

    // ’e‘Síœ
    void Clear();

    void Remove(Arts* arts);

    // ’e”æ“¾
    int GetArtsCount() const { return static_cast<int>(artses.size()); }

    // ’eæ“¾
    Arts* GetArts(int index) { return artses.at(index); }
private:
    std::vector<Arts*> artses;
    std::vector<Arts*> removes;
};