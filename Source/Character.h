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
protected:
	//移動処理
	void Move(float elapsedTime, float vx, float vz, float speed);
	// 旋回処理
	void Turn(float elapsedTime, float vx, float vz, float speed);
	// ジャンプ処理
	void Jump(float speed);
	// 速度処理更新
	void UpdateVelocity(float elpasedTime);

	// 着地したときに呼び出される
	virtual void onLanding() {}
protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	float radius = 0.5f;
	float gravity = -1.0f;	// 重力（フレーム単位の値）
	DirectX::XMFLOAT3 velocity = { 0,0,0 };	// 速度
	bool groundedFlag = false;	// true...着地した
	//姿勢制御
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
};