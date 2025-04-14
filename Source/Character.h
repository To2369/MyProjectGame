#pragma once
#include <DirectXMath.h>
#ifdef USE_IMGUI

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//extern ImWchar glyphRangesJapanese[];
#endif
// キャラクター
class Character
{
public:
	Character(){}
	virtual ~Character() {}

	//行列更新
	void UpdateTransform();

	// ダメージ処理
	bool ApplyDamage(float invincibleTime, int damage);

	// 衝撃を与える
	void AddImpulse(const DirectX::XMFLOAT3& impulse);
public:
	//位置取得
	const DirectX::XMFLOAT3& GetPosition()const { return position; }

	//位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	//回転取得
	const DirectX::XMFLOAT4& GetQuarternion()const { return quaternion; }

	//回転設定
	void SetQuarternion(const DirectX::XMFLOAT4& quaternion) { this->quaternion = quaternion; }

	//スケール設定
	const DirectX::XMFLOAT3& GetScale()const { return scale; }

	//スケール取得
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	// 半径の取得
	float GetRadius() const { return radius; }

	// 着地したかどうかのフラグの取得
	bool isGrounded() const { return groundedFlag; }

	// 高さ取得
	float GetHeight() const { return height; }

	// ライフを取得
	int GetHealth() const { return health; }

	// 最大ライフを取得
	int GetMaxHealth() const { return maxHealth; }

	// 気力を取得
	int GetSpritEnergy() const { return spiritEnergy; }

	// 最大気力を取得
	int GetMaxSpritEnergy() const { return maxSpritEnergy; }

	int GetSpritGaugeCount() const { return spiritGaugeCount; }
	int GetSpritOneGauge() const { return spritOneGauge; }
	// 気力を取得
	int GetSkillEnergy() const { return skillEnergy; }

	// 最大気力を取得
	int GetMaxSkillEnergy() const { return maxSkillEnergy; }

	bool IsUseSpiritEnergy()const { return useSpiritEnergyFlag; }

	const DirectX::XMVECTOR& GetRight() const{ return right; }
	const DirectX::XMVECTOR& GetFront() const { return front; }
private:
	// 垂直速度更新処理
	void UpdateVerticalVelocity(float elapsedTime);

	// 垂直移動更新処理
	void UpdateVerticalMove(float elapsedTime);

	// 水平速度更新処理
	void UpdateHorizontalVelocity(float elapsedTime);

	// 水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);
protected:
	// 移動方向を決定
	void Move(float vx, float vz, float speed);
	// 旋回処理
	void Turn(float elapsedTime, float vx, float vz, float speed);
	// ジャンプ処理
	void Jump(float speed);
	void Fly(float speed);
	// 速度処理更新
	void UpdateVelocity(float elpasedTime);

	// 無敵時間の更新
	void UpdateInvincibleTimer(float elapsedTime);

	//ステータス更新
	void UpdateStatus(float elapsedTime);

	// 着地したときに呼び出される
	virtual void OnLanding() {}

	// ダメージを受けたときに呼ばれる
	virtual void OnDamaged() {}

	// 死亡したときに呼ばれる
	virtual void OnDead() {}
protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT4 quaternion = { 0,0,0,1 };
	DirectX::XMFLOAT4 quaternion_additional = { 0,0,0,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMVECTOR right = { 1,0,0 };
	DirectX::XMVECTOR up = { 0,1,0 };
	DirectX::XMVECTOR front = { 0,0,1 };
	float radius = 0.5f;
	float gravity = -1.0f;	// 重力（フレーム単位の値）
	DirectX::XMFLOAT3 velocity = { 0,0,0 };	// 速度
	//姿勢制御
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	bool groundedFlag = false;	// true...着地した
	float height = 2.0f;	// 高さ
	int health = 5; // ライフ
	int maxHealth = 5;	// 最大ライフ
	int spiritEnergy = 300;	//気力
	int maxSpritEnergy = 500;//最大気力
	int skillEnergy = 200;	//技力
	int maxSkillEnergy = 700;	//最大技力

	float invincibleTimer = 0.5f;   // 無敵時間
	float friction = 0.5f; // 摩擦
	float acceleration = 1.0f;  // 加速度
	float maxMoveSpeed = 5.0f;  // 最大移動速度
	DirectX::XMFLOAT3 direction = { 0,0,0 };    // 移動方向
	float airControl = 0.3f;    // 空中制御用係数
	float slopeRate = 1.0f; // 傾斜率

	bool useSpiritEnergyFlag = false;
	float energyRecoveryTimer = 0.0f;
	float energyNoRecoveryTimer = 1.0f;
	int spiritHealSpeed = 5;
	int spritOneGauge = 0;
	int oneGaugeMax = 100;
	int spiritGaugeCount = 1;


	int useDashSkillEnergy = 1;
	float skillEnergyTimer = 0.0f;	// 技力を使って何秒たったか

	bool dashFlag = false;
};