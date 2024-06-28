<!--
UNIT19_2_EXCERCISE UNIT19 演習手順
SKINNED MESH MATERIAL

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Materials
Textures
-->

# UNIT19_2_EXCERCISE SKINNED MESH MATERIAL 演習手順

* 目次
    * [4. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\resources\\cube.001.0.fbx に変更する](#4_)
    * [5. 実行しテクスチャが貼られていることを確認する](#5_)
    * [6. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\resources\\cube.001.1.fbx に変更する](#6_)
    * [7. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\resources\\cube.001.2.fbx に変更する](#7_)
    * [8. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\resources\\cube.000.fbx に変更する](#8_)


---


#### <span style="color:#334488;">4. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\\\resources\\\\cube.001.0.fbx に変更する</span><a name="4_"></a>

    演習手順４

演習手順にも記載されていますが、 **cube.001.0.fbx** にはマテリアルが設定されてあり、テクスチャとして **black-metal-texture.jpg** が設定されています。

**framework::initialize(...)** にあるメッシュ読み込みの以下の部分を

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.000.fbx");
```

以下のようにファイル名を変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize(...)**

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.001.0.fbx");
```

---

#### <span style="color:#334488;">5. 実行しテクスチャが貼られていることを確認する</span><a name="5_"></a>

    演習手順５

演習手順４でファイル名を変更した状態で実行してみましょう。
以下のようなメッシュが表示されるはずです。

@import "images/0010.png"

---

#### <span style="color:#334488;">6. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\\\resources\\\\cube.001.1.fbx に変更する</span><a name="6_"></a>

    演習手順６

演習手順５と同じで、変更するファイルの **cube.001.1.fbx** にはマテリアルが設定されてあり、テクスチャとして **ue.png** が設定されています。

演習手順５と違う点はテクスチャである **ue.png** が **cube.001.1.fbx** のファイル内に埋め込まれているという点です。

ひとまず **framework::initialize(...)** にあるメッシュ読み込みの以下の部分を

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.001.0.fbx");
```

以下のようにファイル名を変更し、実行してみましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize(...)**

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.001.1.fbx");
```

以下のようなメッシュが表示されます。

@import "images/0011.png"

表示されたポリゴンには **ue.png** のテクスチャが貼り付けられていますが埋め込みテクスチャになっているので **resources** ファルダ内に入っている ue.png を読み込んで表示しているわけではありません。

試しに **resources** フォルダ内に入っている ue.png を削除して実行してみて下さい。
問題なく表示されます。埋め込みは FBX ファイル内部にテクスチャ自体の情報も入っている為、このような事が可能です。

演習手順に記載されてあるように埋め込みテクスチャを持っている FBX ファイルを一度起動すると同階層に FBX と同じ名前の .fbm フォルダが生成されます。

起動時に埋め込んだテクスチャをこの .fbm のフォルダ内に生成し、フォルダ内に生成した ue.png を、表示するテクスチャとして読み込んで描画を行っています。起動毎に .fbm の生成を行っている為、テクスチャが入れ替わったりした際の心配などは必要ありません。テクスチャの変更が必要ないというようなメッシュの場合はこちらの FBX を利用しテクスチャを外部ファイルにおいていろいろ入れ替えたいというような仕様であった場合は演習手順４を利用するよう、用途によって使いわけましょう。


---

#### <span style="color:#334488;">7. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\\\resources\\\\cube.001.2.fbx に変更する</span><a name="7_"></a>

    演習手順７

変更するファイルの **cube.001.2.fbx** にはマテリアルが設定されてあり、テクスチャは設定されていません。

**framework::initialize(...)** にあるメッシュ読み込みの以下の部分を

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.001.1.fbx");
```

以下のようにファイル名を変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**skinned_mesh::initialize(...)**

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.001.2.fbx");
```

---

    演習手順７－１


**cube.001.2.fbx** はマテリアルとして色が設定されており、その色の変更を確認します。
FBX に設定されている色が反映されるようコードを少し変更します。

**skinned_mesh::render(...)** の以下の部分を

```cpp
data.material_color = material_color;
```

以下のように変更して下さい。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::render(...)**

```cpp
XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&materials.crbegin()->Kd));
```

実行すると以下のメッシュが表示されます。

@import "images/0012.png"

---

#### <span style="color:#334488;">8. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\\\resources\\\\cube.000.fbx に変更する</span><a name="8_"></a>

#### skinned_mesh クラスの render メンバ関数でクラッシュする（materials の要素数が０であることが原因）

    演習手順８－１

ファイルを **cube.000.fbx** に戻しましょう。
**cube.000.fbx** にはマテリアルが設定されていません。
なのでテクスチャを生成する段階でクラッシュしてしまいます。確認してみましょう。

**framework::initialize(...)** にあるメッシュ読み込みの以下の部分を

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.001.2.fbx");
```

以下のようにファイル名を変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize(...)**

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.000.fbx");
```

起動してクラッシュする事を確認しましょう。

---

#### この問題を解決する（例えば、ダミーのマテリアルを materials に追加する）

    演習手順８－２

ダミーのテクスチャは既に生成されるように実装しているので次はダミーのマテリアルを生成します。
ダミーであってもマテリアルさえあればダミーのテクスチャが生成されるので描画が行われます。

マテリアルの生成を行っている **skinned_mesh::fetch_materials(...)** の以下の部分に追加しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::fetch_materials(...)**

```cpp
            ・・・省略・・・
            // 取得したマテリアルの情報を設定する
            materials.insert(std::move(material));
        }
    }

*   #if 1
*       // ダミーのマテリアルを挿入
*       material material;
*       materials.insert(material);
*   #endif
    ・・・省略・・・
```

起動してクラッシュしないことを確認しましょう。