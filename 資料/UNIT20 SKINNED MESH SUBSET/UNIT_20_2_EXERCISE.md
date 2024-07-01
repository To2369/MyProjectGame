<!--
UNIT20_EXCERCISE UNIT20 演習手順
SKINNED MESH SUBSET

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Multimaterials
Subsets
-->

# UNIT20_EXCERCISE SKINNED MESH SUBSET 演習手順

* 目次
    * [5. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\\\resources\\\\cube.002.0.fbx に変更する](#5)
    * [6. 実行し３種類のテクスチャが貼られていることを確認する](#6)
    * [7. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\\\resources\\\\cube.002.1.fbx に変更する](#7)
    * [8. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\\\resources\\\\cube.000.fbx に変更する](#8)

---

#### <span style="color:#334488;">5. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\\\resources\\\\cube.002.0.fbx に変更する</span><a name="5_"></a>

**framework::initialize(...)** にあるメッシュ読み込みの以下の部分を

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.000.fbx");
```

以下のようにファイル名を変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize(...)**

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.002.0.fbx");
```

---

#### <span style="color:#334488;">6. 実行し３種類のテクスチャが貼られていることを確認する</span><a name="6_"></a>

起動すると以下のメッシュが描画されます。

@import "images/0020.png"

**cube.002.0.fbx** にはマテリアルが設定されてあり、テクスチャとして **atari.png** **dice.png** **nintendo.png** が設定されていることを確認しましょう。

起動すると **resources** フォルダ内に **cube.002.0.fbm** フォルダが作成されているので中に上記の３つの画像ファイルが作成されているかを確認すればどの画像ファイルが埋め込まれているかが確認できます。上記以外の画像ファイルがあるみたいですがこれらは、埋め込ま絵れているが使われていない画像ファイルになります。

---

#### <span style="color:#334488;">7. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\\\resources\\\\cube.002.1.fbx に変更する</span><a name="7_"></a>

**framework::initialize(...)** にあるメッシュ読み込みの以下の部分を

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.002.0.fbx");
```

以下のようにファイル名を変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize(...)**

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.002.1.fbx");
```

起動すると以下のメッシュが描画されます。

@import "images/0021.png"

---

#### <span style="color:#334488;">8. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\\\resources\\\\cube.000.fbx に変更する</span><a name="8_"></a>


**framework::initialize(...)** にあるメッシュ読み込みの以下の部分を

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.002.0.fbx");
```

以下のようにファイル名を変更し起動してみましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize(...)**

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.002.1.fbx");
```

問題なく実行されることを確認しましょう。

---