#pragma once

#include "Stage.h"
#include <vector>
#include <memory>

class StageManager
{
public:
    StageManager() {}
    ~StageManager() { stages.clear(); }

    static StageManager& Instance()
    {
        static StageManager ins;
        return ins;
    }

    // �X�e�[�W�̓o�^
    void Regist(Stage* stage) { stages.emplace_back(stage); }

    // �X�e�[�W�̎擾
    Stage* GetStage(int index) { return stages.at(index); }

    // �X�e�[�W�S�폜
    void Clear()
    {
        for (Stage* stage : stages)
        {
            delete stage;
        }
        stages.clear();
    }
    // ���C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
private:
    std::vector<Stage*> stages;
};