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

private:
    std::vector<Stage*> stages;
};