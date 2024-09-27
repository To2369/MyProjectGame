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
	const DirectX::XMFLOAT3& GetAngle()const { return angle; }

	//回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

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
	// 速度処理更新
	void UpdateVelocity(float elpasedTime);

	// 無敵時間の更新
	void UpdateInvincibleTimer(float elapsedTime);

	// 着地したときに呼び出される
	virtual void OnLanding() {}

	// ダメージを受けたときに呼ばれる
	virtual void OnDamaged() {}

	// 死亡したときに呼ばれる
	virtual void OnDead() {}
protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	float radius = 0.5f;
	float gravity = -1.0f;	// 重力（フレーム単位の値）
	DirectX::XMFLOAT3 velocity = { 0,0,0 };	// 速度
	bool groundedFlag = false;	// true...着地した
	float height = 2.0f;	// 高さ
	int health = 5; // ライフ
	float invincibleTimer = 0.5f;   // 無敵時間
	float friction = 0.5f; // 摩擦
	float acceleration = 1.0f;  // 加速度
	float maxMoveSpeed = 5.0f;  // 最大移動速度
	DirectX::XMFLOAT3 direction = { 0,0,0 };    // 移動方向
	float airControl = 0.3f;    // 空中制御用係数
	float slopeRate = 1.0f; // 傾斜率
	//姿勢制御
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
};