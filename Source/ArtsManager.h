#pragma once

#include "Arts.h"
#include <vector>
// �Z�Ǘ�
class ArtsManager
{
public:
    ArtsManager();
    ~ArtsManager();

    void Update(float elpasedTime);

    void Render(ID3D11DeviceContext* dc);

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �e�o�^
    void Regist(Arts* arts);

    // �e�S�폜
    void Clear();

    void Remove(Arts* arts);

    // �e���擾
    int GetArtsCount() const { return static_cast<int>(artses.size()); }

    // �e�擾
    Arts* GetArts(int index) { return artses.at(index); }
private:
    std::vector<Arts*> artses;
    std::vector<Arts*> removes;
};