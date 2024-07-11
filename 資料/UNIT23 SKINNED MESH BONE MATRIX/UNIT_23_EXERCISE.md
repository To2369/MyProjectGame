<!--
UNIT23_EXCERCISE UNIT23 演習手順
SKINNED MESH BONE MATRIX

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Bone matrices
-->

# UNIT23_EXCERCISE SKINNED MESH BONE MATRIX 演習手順

* 目次
    1. 前回使用した３つのボーンを持つ FBX ファイル (cube.004.fbx) をロードしダミー行列をセットしテストする
    2. [skinned_mesh クラスに定義した定数バッファー(constants)にボーン行列を追加する](#2_)
    3. [シェーダ(skinned_mesh.hlsli)に定義した定数バッファー(OBJECT_CONSTANT_BUFFER)にボーン行列を追加する](#3_)
    4. [頂点シェーダで頂点の位置と法線にボーン行列の影響を与える](#4_)
    5. [skinned_mesh クラスの render メンバ関数でダメー行列を定数バッファ(data.bone_transforms)にセットする](#5_)
    6. [実行し、メッシュの変形を確認する](#6_)

---

#### <span style="color:#334488;">1. 前回使用した３つのボーンを持つ FBX ファイル (cube.004.fbx) をロードしダミー行列をセットしテストする</span>

ボーンに対して行列を適用し頂点シェーダーが側で頂点の位置と法線に対してボーンが持っている行列の影響を与えるように実装してみよう。

---

#### <span style="color:#334488;">2. skinned_mesh クラスに定義した定数バッファー(constants)にボーン行列を追加する</span><a name="2_"></a>

    演習手順２

シェーダー側でボーンに設定されている行列と頂点にあるウェイト値を利用して最終的な座標位置を計算します。

ですのでボーンに設定されている行列をシェーダー側で利用できるようにするために定数バッファに宣言しましょう。

|実装するファイル|
--|--
skinned_mesh.h|

```cpp
*   static const int MAX_BONES{ 256 };
    // 定数バッファフォーマット
    struct constants
    {
        DirectX::XMFLOAT4X4 world;			// ワールド行列
        DirectX::XMFLOAT4 material_color;	// マテリアルカラー
*       DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES]{ {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1} };
    };
```

---

#### <span style="color:#334488;">3. シェーダ(skinned_mesh.hlsli)に定義した定数バッファー(OBJECT_CONSTANT_BUFFER)にボーン行列を追加する</span><a name="3_"></a>

    演習手順３

定数バッファフォーマットにボーン行列を追加したのでシェーダー側の定数バッファにもボーン行列を追加して値を受け取れるようにしておきましょう。

|実装するファイル|
--|--
**skinned_mesh.hlsli**|

```cpp
*   static const int MAX_BONES = 256;
    cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
    {
        row_major float4x4 world;
        float4 material_color;
*       row_major float4x4 bone_transforms[MAX_BONES];
    };
```

---

#### <span style="color:#334488;">4. 頂点シェーダで頂点の位置と法線にボーン行列の影響を与える</span><a name="4_"></a>

    演習手順４

影響下にあるボーンすべてのボーン行列をボーン影響度であるウェイト値に従って現在の座標位置を割り出す計算をおこないます。

ボーン行列の影響が計算された最終的な座標でワールド行列とビュープロジェクト行列を利用してスクリーン座標の値を求めます。

|実装するファイル|実装する関数|
--|--
**skinned_mesh_vs.hlsl**|**main(...)**

```hlsl
    {
*       vin.normal.w = 0;
*       float4 blended_position = { 0, 0, 0, 1 };
*       float4 blended_normal = { 0, 0, 0, 0 };
*       for (int bone_index = 0; bone_index < 4; ++bone_index)
*       {
*           blended_position += vin.bone_weights[bone_index] * mul(vin.position, bone_transforms[vin.bone_indices[bone_index]]);
*           blended_normal += vin.bone_weights[bone_index] * mul(vin.normal, bone_transforms[vin.bone_indices[bone_index]]);
*       }
*       vin.position = float4(blended_position.xyz, 1.0f);
*       vin.normal = float4(blended_normal.xyz, 0.0f);

        VS_OUT vout;
        vout.position = mul(vin.position, mul(world, view_projection));
```

---

#### <span style="color:#334488;">5. skinned_mesh クラスの render メンバ関数でダメー行列を定数バッファ(data.bone_transforms)にセットする</span><a name="5_"></a>

    演習手順５

頂点にボーン行列を取り出し反映させる準備ができました。

ですが cube.004.fbx のボーン行列の値には回転などの変化は設定されていないので、今回はダミーの値をボーン行列に対して設定し、変化を確認してみましょう。

    ※動作確認後#if-#endifディレクティブのコードは無効にすること

とあるので、演習手順６での実行後は #if 0 にしてマクロを切っておきましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::render(...)**

```cpp
            constants data;
            //data.world = world;
            XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
*   #if 1
*           XMStoreFloat4x4(&data.bone_transforms[0], XMMatrixIdentity());
*           XMStoreFloat4x4(&data.bone_transforms[1], XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(+45)));
*           XMStoreFloat4x4(&data.bone_transforms[2], XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(-45)));
*   #endif

            for (const mesh::subset& subset : mesh.subsets)
```

---

#### <span style="color:#334488;">6. 実行し、メッシュの変形を確認する</span><a name="6_"></a>

    演習手順７

実行し確認すると以下の画像が表示されます。

@import "images/0040_1.png"