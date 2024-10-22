#include "Arts.h"
#include "ArtsManager.h"
#include "Graphics/Graphics.h"

Arts::Arts(ArtsManager* manager) : artsMgr(manager)
{
    artsMgr->Regist(this);
}

// 行列更新処理
void Arts::UpdateTransform()
{
    DirectX::XMVECTOR frontVec, upVec, rightVec;

    // 現在の方向を正規化し前方向ベクトルを計算
    frontVec = DirectX::XMLoadFloat3(&direction);
    frontVec = DirectX::XMVector3Normalize(frontVec);

    // 仮の上方向ベクトルを計算
    upVec = DirectX::XMVectorSet(0.001f, 1, 0, 0);
    upVec = DirectX::XMVector3Normalize(upVec);

    // 外積を利用して右方向ベクトルを計算
    rightVec = DirectX::XMVector3Cross(upVec, frontVec);
    rightVec = DirectX::XMVector3Normalize(rightVec);

    // 外積を利用して正しい上方向ベクトルを計算
    upVec = DirectX::XMVector3Cross(frontVec, rightVec);

    // 計算結果を XMFLOAT3 に変換
    DirectX::XMFLOAT3 right, up, front;
    DirectX::XMStoreFloat3(&right, rightVec);
    DirectX::XMStoreFloat3(&up, upVec);
    DirectX::XMStoreFloat3(&front, frontVec);

    // 計算した軸ベクトルから姿勢行列を作成
    transform._11 = right.x * scale.x;
    transform._12 = right.y * scale.x;
    transform._13 = right.z * scale.x;
    transform._14 = 0.0f;

    transform._21 = up.x * scale.y;
    transform._22 = up.y * scale.y;
    transform._23 = up.z * scale.y;
    transform._24 = 0.0;

    transform._31 = front.x * scale.z;
    transform._32 = front.y * scale.z;
    transform._33 = front.z * scale.z;
    transform._34 = 0.0f;

    transform._41 = position.x;
    transform._42 = position.y;
    transform._43 = position.z;
    transform._44 = 1.0f;

    // 発射方向
    this->direction = front;

    //　座標系変換用の行列
    const DirectX::XMFLOAT4X4 coordinate_system_transform[]
    {
        { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	//0:右手系 Y-UP
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	    //1:左手系 Y-UP
        { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	//2:右手系 Z-UP
        { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	    //3:左手系 Z-UP
    };
    //　デフォルトのスケールファクタを設定して行列に反映
    const float scale_factor = 1.0f;
    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transform[0]) * DirectX::XMMatrixScaling(scale_factor,scale_factor,scale_factor) };
    // スケール行列作成
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x,scale.y,scale.z) };
    // 回転行列作成
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion));

    // 位置行列作成
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x,position.y,position.z) };

    // 行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = C * S * R * T;
    //計算したワールド行列をtrabsformに取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}

// 破棄
void Arts::Destroy()
{
    artsMgr->Remove(this);
}