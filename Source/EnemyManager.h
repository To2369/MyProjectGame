#pragma once
#include <vector>
#include "Enemy.h"
#include <memory>
#include<set>
// �G�Ǘ�
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

	//�G�l�~�[�S�폜
	void Clear();

	//�G�l�~�[�o�^
	void Regist(std::unique_ptr<Enemy> enemy);

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	//�G�l�~�[�폜
	void Remove(Enemy* enemy);
	// �G�̑������擾
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// �G���擾
	Enemy* GetEnemy(int index) { return enemies.at(index).get(); }
private:
	void CollisionEnemiesAndEnemies();
private:
	std::vector<std::unique_ptr<Enemy>> enemies;
	std::set<Enemy*> removes;
};