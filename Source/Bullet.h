#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
// 弾管理の前方宣言
class BulletManager;

// 弾
class Bullet
{
public:
    Bullet(BulletManager* manager);
	virtual ~Bullet(){}

	virtual void Update(float elapsedTIme) = 0;

	virtual void Render(ID3D11DeviceContext* dc) = 0;

    // デバッグプリミティブ描画
	virtual void DrawDebugPrimitive();

    void Destroy();

public:
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    // 方向取得
    const DirectX::XMFLOAT3& GetDirection() const { return direction; }

    // 方向の設定
    void SetDirection(const DirectX::XMFLOAT3& direction) { this->direction = direction; }

    // 拡大縮小取得
    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    // 半径取得
    float GetRadius() const { return radius; }
protected:
    // 行列更新処理
    void UpdateTransform();

protected:
    DirectX::XMFLOAT3	position = { 0,0,0 };	// 位置
    DirectX::XMFLOAT3	direction = { 0,0,1 };	// 方向
    DirectX::XMFLOAT3	scale = { 1,1,1 };		// 拡大縮小
    // 姿勢行列
    DirectX::XMFLOAT4X4	transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    BulletManager* bulletMgr = nullptr;

    float radius = 0.5f;	// 半径
};