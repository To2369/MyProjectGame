<!--
UNIT26_EXCERCISE UNIT26 演習手順
SKINNED MESH OBJECT ANIMATION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Object animation
-->

# UNIT26_EXCERCISE SKINNED MESH OBJECT ANIMATION 演習手順

* 目次
    1. [skinned_mesh オブジェクトをアニメーション情報を持った.\\resources\\Drone166\\Drone166.1.fbx に変更する](#1_)
    2. [実行し、Drone16.1 がアニメーションしていないことを確認する](#2_)
    3. [skinned_mesh クラスの render メンバ関数を変更する](3_)
    4. [実行し、Drone16.1 がアニメーションすることを確認する](4_)

---

#### <span style="color:#334488;">1. skinned_mesh オブジェクトをアニメーション情報を持った.\\resources\\Drone166\\Drone166.1.fbx に変更する</span><a name="1_"></a>

    演習手順１

読み込むファイルを **Drone166.1.fbx** に変更する。

※にあるように「右手系・Y 軸アップ」「三角形済み」というのは前回のままで「メートル単位」という部分を変更するだけなのでこの部分の変更を前回の設定を参考にやってみてください。

つぎに以下のファイルの読み込み部分を

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\plantune.fbx");
```

以下のように変更して下さい。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
* skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\Drone166\\Drone166.1.fbx");
```

---

#### <span style="color:#334488;">2. 実行し、Drone16.1 がアニメーションしていないことを確認する</span><a name="2_"></a>

    演習手順２

実行すると以下のような画像が表示されますが、動きがない、つまりアニメーションを全くしていません。

@import "images/Drone01.png"

※印部分がきちんと出来ていない場合は表示がおかしくなっているので注意して下さい。

--

#### <span style="color:#334488;">3. skinned_mesh クラスの render メンバ関数を変更する</span><a name="3_"></a>

    演習手順３

オブジェクトアニメーションが機能するように skinned_mesh::render(...) のグローバル行列計算部分にキーフレームの情報が反映されるよう修正しましょう。

skinned_mesh::render(...) の以下の部分を

```cpp
constants data;
XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
```

以下のように変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh**|**skinned_mesh::render(...)**

```cpp
    constants data;
*   //XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
*   // メッシュの位置・姿勢行列をキーフレームから取得
*   const animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
*   // 取得したキーフレームごとの位置・姿勢行列をワールド変換行列に合成する
*   XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));
```

---

#### <span style="color:#334488;">4. 実行し、Drone16.1 がアニメーションすることを確認する</span><a name="4_"></a>

    演習手順４

実行して次はモデル全体が動いている事を確認しましょう。