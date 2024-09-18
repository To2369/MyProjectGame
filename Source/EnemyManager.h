#pragma once
#include <vector>
#include "Enemy.h"
#include <memory>

// 敵管理
class EnemyManager
{
private:
	EnemyManager(){}
	~EnemyManager(){}

public:
	static EnemyManager& Instance()
	{
		static EnemyManager inst;
		return inst;
	}

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc);

	void Regist(std::unique_ptr<Enemy> enemy);

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// 敵の総数を取得
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// 敵を取得
	Enemy* GetEnemy(int index) { return enemies.at(index).get(); }

private:
	std::vector<std::unique_ptr<Enemy>> enemies;
};