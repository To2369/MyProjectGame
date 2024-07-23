<!--
UNIT27_EXCERCISE UNIT27 演習手順
SKINNED MESH CONTROLLING ANIMATION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Controlling animation
-->

# UNIT27_EXCERCISE SKINNED MESH CONTROLLING ANIMATION 演習手順

* 目次
    1. [ramework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\resources\\plantune.fbx に変更する](#1_)
    2. [animation::keyframe::node 構造体にメンバ変数を追加する](#2_)
    3. [skinned_mesh クラスの fetch_animations メンバ関数を変更する](#3_)
    4. [skinned_mesh クラスに update_animation メンバ関数を追加する](#4_)
    5. [framework クラスの render メンバ関数を変更する](#5_)
    6. [実行し、プランチューン待機モーションをとりながら、首が伸び、さらに左を向いていることを確認する](#6_)
    7. [pdf にある ５．19: 20:行目の定数値を変数に変更し、ImGUI から実行時に変更できるようにする](#7_)

---

#### <span style="color:#334488;">1. ramework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\resources\\plantune.fbx に変更する</span><a name="1_"></a>

※ plantune.fbx は右手系・Y 軸アップ・センチメートル単位・三角形化済み

そろそろこの部分はできると思うので考えて変更してみてください。

---

#### <span style="color:#334488;">2. animation::keyframe::node 構造体にメンバ変数を追加する</span><a name="2_"></a>

    演習手順２

描画部分で上書きされたグローバル行列を取得してキーフレームにあるノードごとの行列を計算するので、まずはそのキーフレームにあるノードに「拡大」「回転」「位置」のメンバ変数を用意する。

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**animation::keyframe::node**

```cpp
    DirectX::XMFLOAT4X4 global_transform{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};

*   // ノードの変換データには、その親に対する平行移動、回転、および拡大率のベクトルが含まれます。
*   DirectX::XMFLOAT3 scaling{ 1,1,1 };
*   DirectX::XMFLOAT4 rotation{ 0,0,0,1 };	// Rotation quaternion
*   DirectX::XMFLOAT3 translation{ 0,0,0 };
```

---

#### <span style="color:#334488;">3. skinned_mesh クラスの fetch_animations メンバ関数を変更する</span><a name="3_"></a>

    演習手順３

ノード自身に設定されたローカルの行列を取得し、行列にある「拡大」「回転」「位置」の成分を取り出す。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::fetch_animations(...)**

```cpp
        node.global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform(time));

*       // 'local_transform'は、親のローカル座標系に関するノードの変換行列です。
*       const FbxAMatrix& local_transform{ fbx_node->EvaluateLocalTransform(time) };
*       node.scaling = to_xmfloat3(local_transform.GetS());
*       node.rotation = to_xmfloat4(local_transform.GetQ());
*       node.translation = to_xmfloat3(local_transform.GetT());
    }
```

---

#### <span style="color:#334488;">4. skinned_mesh クラスに update_animation メンバ関数を追加する</span><a name="4_"></a>

ノードに設定されている「拡大」「回転」「位置」をノードのワールド行列に反映する。

以下の計算を描画前に毎回計算します。。

$
ローカル行列=スケール行列×回転行列×移動行列\\グローバル行列=ローカル行列×親のグローバル行列
$

といことでワールド行列更新用のメンバ関数を実装。

まずは宣言。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
        void render(ID3D11DeviceContext* immediate_context,
            const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color,
            const animation::keyframe* keyframe);

*       /// <summary>
*       /// アニメーションの更新
*       /// </summary>
*       /// <param name="keyframe">キーフレーム</param>
*       void update_animation(animation::keyframe& keyframe);

    protected:
```

つづいて定義。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::update_animation(...)**

```cpp
・・・省略(上に skinned_mesh::fetch_animations(...) メンバ関数の定義)・・・

/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="keyframe">キーフレーム</param>
void skinned_mesh::update_animation(animation::keyframe& keyframe)
{
    // キーフレームに存在するすべてのノードを更新する
    const size_t node_count{ keyframe.nodes.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index) 
    {
        // ローカル行列を設定
        animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
        XMMATRIX S{ XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z) };
        XMMATRIX R{ XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation)) };
        XMMATRIX T{ XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };

        // 親のグローバル行列を取得
        const int64_t parent_index{ scene_view.nodes.at(node_index).parent_index };
        XMMATRIX P{ parent_index < 0 ? XMMatrixIdentity() : XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };

        // ローカル行列 * 親のグローバル行列
        XMStoreFloat4x4(&node.global_transform, S * R * T * P);
    }
}
```

---

#### <span style="color:#334488;">5. framework クラスの render メンバ関数を変更する</span><a name="5_"></a>

    演習手順５

デバッグ用として首の部分のノードの値を変更します。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
        animation::keyframe& keyframe{ animation.sequence.at(frame_index) };

*   #if 1
*       // デバッグ用(首を高くして左に回転)
*       XMStoreFloat4(&keyframe.nodes.at(30).rotation,
*           DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(1, 0, 0, 0), 1.5f));
*       keyframe.nodes.at(30).translation.x = 300.0f;
*       skinned_meshes[0]->update_animation(keyframe);
*   #endif

        skinned_meshes[0]->render(immediate_context.Get(), world, material_color, &keyframe);
```

---

#### <span style="color:#334488;">6. 実行し、プランチューン待機モーションをとりながら、首が伸び、さらに左を向いていることを確認する</span><a name="6_"></a>

    演習手順６

ひとまず以下のような画像が表示されます。

@import "images/plantune03.png"

---

#### <an style="color:#334488;">7. pdf にある ５．19: 20:行目の定数値を変数に変更し、ImGUI から実行時に変更できるようにする</span><a name="7_"></a>

    演習手順７

アニメーションを制御できる値を変数にして ImGUI から変更できるようにしてみましょう。

以前 UNIT11 で実装した事を思い出してやってみましょう。

変数名を **factors** の配列で宣言していますが、別になんでも構いません。

|名称|メンバ変数名|最低値|最大値|使用する<br>ImGUI<br>関数
--|--|--|--|--
**回転**|**factors[0]**|-1.5f|1.5f|SliderFloat
**移動**|**factors[1]**|0|500.0f|SliderFloat

実装できたら ImGUI から首の回転と長さを変更してみましょう。


場所
スポッチャ
    城東放出店
    千日前店
VSパーク
    VS PARK ららぽーとEXPOCITY店
    VS PARK（ブイエス パーク） セブンパーク天美店
あべのハルカス

時間帯
9:30 ~ 15:00
から選ぶ

11/11(金)
現地集合、現地解散、雨天決行