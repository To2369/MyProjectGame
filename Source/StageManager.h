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

    // ステージの登録
    void Regist(Stage* stage) { stages.emplace_back(stage); }

    // ステージの取得
    Stage* GetStage(int index) { return stages.at(index); }

    // ステージ全削除
    void Clear()
    {
        for (Stage* stage : stages)
        {
            delete stage;
        }
        stages.clear();
    }
private:
    std::vector<Stage*> stages;
};