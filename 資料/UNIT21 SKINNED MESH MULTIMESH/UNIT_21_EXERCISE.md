<!--
UNIT21_EXCERCISE UNIT21 演習手順
SKINNED MESH MULTIMESH

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Multi mesh
Global transform
Coordinate system transformation
-->

# UNIT21_EXCERCISE SKINNED MESH MULTIMESH 演習手順

* 目次
    1. 複数のメッシュを持つ FBX ファイルをロードし描画する
    2. [framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を .\\resources\\cube.003.0.fbx に変更する](#2_)
    3. [実行し正四面体の上に Suzanne が配置されずに重なっている事を確認する](#3_)
        1. [mesh 構造体にメンバ変数を追加する](#3-1_)
        2. [fetch_mesh メンバ関数の適当な場所に下記のコードを挿入する](#3-2_)
        3. [skinned_mesh.cpp に to_xmlfloat4x4 関数、to_xmfloat3 関数、to_xmfloat4 関数を定義する](#3-3_)
        4. [skinned_mesh クラスの render メンバ関数で、定数バッファ構造体の world メンバ変数の計算を変更する](#3-4_)
        5. [実行し正四面体の上に Suzanne が配置されていることを確認する](#3-5_)
    4. [ロードするファイルを cube.003.1.fbx に変更する](#4_)
        1. [赤黄青の３つの正四面体が横に積まれたシーンが描画さるはずが正しく描画されない](#4-1_)
        2. [framework クラスの render メンバ関数で座標系の変換を行う](#4-2_)
        3. [上記の方法だけでは面カリングが逆になってしまうのでラスタライザステートで制御する](#4_3_)
    5. これまで使用したすべてのモデルデータが正しく描画出来ていることも確認する

---

#### <span style="color:#334488;">2. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を .\\resources\\cube.003.0.fbx に変更する</span><a name="2_"></a>

    演習手順２

framework::initilize() の以下の部分の読み込むファイルを

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.000.fbx");
```

以下のように変更

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.003.0.fbx");
```

---

#### <span style="color:#334488;">3. 実行し正四面体の上に Suzanne が配置されずに重なっている事を確認する</span><a name="3_"></a>

    演習手順３

実行すると以下の画像が表示されます。

@import "images/0030_1.png"

---

正しく表示するためにはシーン内の位置・姿勢・スケール情報（global_transform）を取得して描画に反映させることが必要であるとのことなのでそのための実装を行っていきます。

---

#### <span style="color:#334488;">3-1. mesh 構造体にメンバ変数を追加する</span><a name="3-1_"></a>

    演習手順３－１

まずは上記にあるワールド行列用の変数を mesh 構造体に用意しましょう。

|実装するファイル|実装する構造体|
--|--
**static_mesh.h**|**mesh**

```cpp
        ・・・省略・・・
        std::vector<subset> subsets;

*       /// <summary>
*       /// メッシュごとのワールド行列
*       /// </summary>
*       DirectX::XMFLOAT4X4 default_global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

    private:
        ・・・省略・・・
```

---

#### <span style="color:#334488;">3-2. fetch_mesh メンバ関数の適当な場所に下記のコードを挿入する</span><a name="3-2_"></a>

    演習手順３－２

メッシュからグローバル行列を取得し XMFLOAT4X4 に変換して演習手順３－１で宣言した変数に代入していきます。
メッシュから取得したグローバル行列は FbxAMatrix 型でになっていますが、DirectX 側では扱えないので XMFLOAT4X4 に変換して mesh 構造体に代入し扱います。

演習手順３－３で **to_xmfloat4x4(...)** 関数を実装してその関数で FbxAMatrix から XMFLOAT4X4 への変換を行います。

ひとまず変換が完了したとして default_global_transform 変数に変換した行列を代入するコードだけ記載しておきましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::fetch_meshes(...)**

```cpp
    ・・・省略・・・
    mesh.node_index = scene_view.indexof(mesh.unique_id);

*   // メッシュのグローバル行列を取得し XMFLOAT4X4 に変換して代入
*   mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

    std::vector<mesh::subset>& subsets{ mesh.subsets };
    ・・・省略・・・
```

**to_xmlfloat4x4(...)** 関数を実装していないのでエラーが出ていますが気にせず次に進んで下さい。

---

#### <span style="color:#334488;">3-3. skinned_mesh.cpp に to_xmlfloat4x4 関数、to_xmfloat3 関数、to_xmfloat4 関数を定義する</span><a name="3-3_"></a>

    演習手順３－３

演習手順３－２で出ているエラー部分の関数である **to_xmfloat4x4** 関数の実装を行っていきます。
加えて、後々利用する **to_xmfloat3** および **to_xmfloat4** 関数の実装もここで行っておきます。

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
・・・省略(上にファイルのインクルード)・・・

/// <summary>
/// FbxAMatrix から XMFLOAT4X4 に変換
/// </summary>
inline XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
{
    XMFLOAT4X4 xmfloat4x4;
    for(int row = 0; row < 4; row++)
    {
        for (int column = 0; column < 4; column++)
        {
            xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
        }
    }
    return xmfloat4x4;
}

/// <summary>
/// FbxDouble3 から XMFLOAT3 に変換
/// </summary>
inline XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3)
{
    XMFLOAT3 xmfloat3;
    xmfloat3.x = static_cast<float>(fbxdouble3[0]);
    xmfloat3.y = static_cast<float>(fbxdouble3[1]);
    xmfloat3.z = static_cast<float>(fbxdouble3[2]);
    return xmfloat3;
}

/// <summary>
/// FbxDouble4 から XMFLOAT4 に変換
/// </summary>
inline XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4)
{
    XMFLOAT4 xmfloat4;
    xmfloat4.x = static_cast<float>(fbxdouble4[0]);
    xmfloat4.y = static_cast<float>(fbxdouble4[1]);
    xmfloat4.z = static_cast<float>(fbxdouble4[2]);
    xmfloat4.w = static_cast<float>(fbxdouble4[3]);
    return xmfloat4;
}

・・・省略(下に skinned_mesh::skinned_mesh(...) コンストラクタの定義)・・・
```

---


#### <span style="color:#334488;">3-4. skinned_mesh クラスの render メンバ関数で、定数バッファ構造体の world メンバ変数の計算を変更する</span><a name="3-4_"></a>

    演習手順３－４

定数バッファに親のグローバル行列を設定する際に、メッシュごとのグローバル行列を掛けた値をグローバル行列として渡すように変更します。

ワールド行列を設定している **skinned_mesh::render(...)** の以下の部分を

```cpp
data.world = world;
```

以下のように変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::render(...)**

```cpp
XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
```

---

#### <span style="color:#334488;">3-5. 実行し正四面体の上に Suzanne が配置されていることを確認する</span><a name="3-5_"></a>

    演習手順３－５

演習手順にある通り実行してみましょう。以下のように表示されるはずです。

@import "images/0030_2.png"

---

#### <span style="color:#334488;">4-1. 赤黄青の３つの正四面体が横に積まれたシーンが描画さるはずが正しく描画されない</span><a name="4-1_"></a>

    演習手順４－１

framework::initilize() の以下の部分の読み込むファイルを

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.003.0.fbx");
```

以下のように変更

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.003.1.fbx");
```

起動すると以下のように表示されます。

@import "images/0031_1.png"

演習手順の※に記載されている通りこのファイルのメッシュデータは３角形化されていないので、ファイルのロード時に３角形化を行うようにフラグを切替えておきましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
*   skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.003.1.fbx", true);
```

コンストラクタ部分の第２引数にあたる部分の３角形化をするかどうかのフラグに true を設定しています。

これで起動して確認してみましょう。以下の画像が表示されます。

@import "images/0031_2.png"

---

#### <span style="color:#334488;">4-2. framework クラスの render メンバ関数で座標系の変換を行う</span><a name="4-2_"></a>

    演習手順４－２

演習手順４－１で書いてある通り正しい表示は赤黄青の３つの正四面体が「縦」に積まれた、とありますが、現状横に並んでいます。
原因は演習手順に記載されています。

    * このファイルは右手系・Z 軸アップで記録されている

なので、左手座標系の Y 軸アップに変換しましょう。
加えて

    * このファイルのシーンの単位はメートル記録されているのでスケールファクタは１にセットする

も、実装しましょう。

**framework::render(...)** メンバ関数の以下の部分を

```cpp
    ・・・省略・・・
    immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
    ・・・省略・・・
*   DirectX::XMStoreFloat4x4(&world, S * R * T);
    ・・・省略・・・
```
    
以下のように追加、および変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
    ・・・省略・・・
    immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

*   // 座標系変換用の行列
*   const DirectX::XMFLOAT4X4 coordinate_system_transforms[]{
*       { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 0:右手系 Y-UP
*       { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 1:左手系 Y-UP
*       { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 2:右手系 Z-UP
*       { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 3:左手系 Z-UP
*   };

*   // デフォルトのスケールファクタを設定して行列に反映
*   const float scale_factor = 1.0f;
*   DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[2])
*       * DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) };

    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
    ・・・省略・・・
*   DirectX::XMStoreFloat4x4(&world, C * S * R * T);
    ・・・省略・・・
```

座標系変換用の行列を右手系、左手系、Yアップ、Zアップそれぞれの組み合わせである4種類を用意しています。
今回利用するのは２番の右手系Zアップです。その行列をスケールファクター(尺度)、つまり基準となるスケールと掛け合わせて最終的にメッシュのグローバル行列に掛け合わせることで右手系Zアップとしての行列ができあがります。

起動してみましょう。以下の画像が表示されます。

@import "images/0031_3.png"

縦には表示されましたが、黒くなりました。

---

#### <span style="color:#334488;">4-3. 上記の方法だけでは面カリングが逆になってしまうのでラスタライザステートで制御する</span><a name="4-3_"></a>

    演習手順４－３

説明の通りカリングが逆になってしまっているため黒くなっています。三角形の表と裏の判定を逆にすることで対応します。今までは時計回りを表面として認識していましたが、反時計回りを表面として認識するようにします。

**framework::initialize()** メンバ関数の以下の部分を

```cpp
*   rasterizer_desc.FrontCounterClockwise = FALSE;  // false...時計回りを表面とする、反時計回りを裏面
```

以下のように変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
*   //rasterizer_desc.FrontCounterClockwise = FALSE;  // false...時計回りを表面とする、反時計回りを裏面
*   rasterizer_desc.FrontCounterClockwise = TRUE;   // true...反時計回りを表面とする、時計回りを裏面
```

起動してみましょう。

@import "images/0031_4.png"