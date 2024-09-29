#pragma once
#include <vector>
#include "Enemy.h"
#include <memory>
#include<set>
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

	//エネミー全削除
	void Clear();

	//エネミー登録
	void Regist(std::unique_ptr<Enemy> enemy);

	//デバッグ用GUI描画
	void DrawDebugGUI();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	//エネミー削除
	void Remove(Enemy* enemy);
	// 敵の総数を取得
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// 敵を取得
	Enemy* GetEnemy(int index) { return enemies.at(index).get(); }
private:
	void CollisionEnemiesAndEnemies();
private:
	std::vector<std::unique_ptr<Enemy>> enemies;
	std::set<Enemy*> removes;
};