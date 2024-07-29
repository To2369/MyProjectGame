<!--
UNIT30_EXCERCISE UNIT30 演習手順
SERIALIZATION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Serialization
Cereal
-->

# <span style="color:#444444;">UNIT30 演習手順</span>

## <span style="color:#334488;">SERIALIZATION</span>

---

* 目次
    1. Cereal を導入して skinned_mesh クラスにシリアリズ機能を実装する
        1. [Cereal を GitHub からダウンロード(https://github.com/USCiLab/cereal)](#1_1_)
        2. ダウンロードした ZIP を x3dgp.00 プロジェクトフォルダに展開
        3. [3dgp プロパティページの「C/C++」→「全般」→「追加のインクルード…」に以下のパスを追加](#1_3_)
    2. [skinned_mesh.h に必要なヘッダファイルをインクルードする](#2_)
    3. [skinned_mesh.h に DirectXMath 構造体用の serialize テンプレート関数を定義する](#3_)
    4. [シリアライズしたいデータを持つ構造体に serialize テンプレート関数を定義する](#4_)
    5. [skinned_mesh クラスのコンストラクタにコードを追加する](#5_)
    6. [".\\resources\\nico.fbx"を skinned_mesh クラスのコンストラクタに指定し、１回目と２回目のロード時間の違いを確認する](#6_)
    7. [skinned_mesh クラスの append_animations メンバ関数でアニメーションを追加した場合はシリアライズの対象にならない](#7_)


---

### <span style="color:#334488;">Serialization</span>
#### <span style="color:#334488;">シリアライズ</span>

シリアライズとは、複数の要素を一列に並べる操作や処理のこと。
単にシリアライズといった場合にはデータ構造などを独自形式ファイルに変換する事を指します。

今回のケースでは skinned_mesh クラスにあるデータを独自形式ファイルに変換します。

@import "images/シリアライズ01.png"

モデルが複雑になるにつれて起動時間が遅くなってきていることに気付いているかと思う。

@import "images/シリアライズ02.png"

なので skinned_mesh に読み込んだ必要なデータだけを独自形式ファイルに変換して外部に保存し、次回以降の読み込みを、外部保存した独自形式ファイルから取り出す事で読み込みにかかる時間を限りなく減らすというものです。

@import "images/シリアライズ03.png"

これらの仕組みの事を**シリアライズ**と呼びます。

また、シリアライズの仕組みを利用している中で「元データを独自形式ファイルに変換する処理の事」を**シリアライズ**と呼び、「独自形式ファイルを元データに復元する処理の事」を**デシリアライズ**と呼びます。

---

### <span style="color:#334488;">Cereal</span>
#### <span style="color:#334488;">Cereal</span>

上記のシリアライズを行う事の出来る C++11 用のシリアライズライブラリ。

バイナリ、JSON, XML の複数のフォーマットにシリアライズでき、比較的簡単に導入ができる。

---

#### <span style="color:#334488;">1. Cereal を導入して skinned_mesh クラスにシリアリズ機能を実装する</span>

---

#### <span style="color:#334488;">1 - 1. Cereal を GitHub からダウンロード(https://github.com/USCiLab/cereal)</span><a name="1_1_"></a>

    ※「Code」から「Download ZIP」を選択

@import "images/cereal01.png"

---

#### <span style="color:#334488;">1 - 2. ダウンロードした ZIP を x3dgp.00 プロジェクトフォルダに展開</span>

---

#### <span style="color:#334488;">1 - 3. 3dgp プロパティページの「C/C++」→「全般」→「追加のインクルード…」に以下のパスを追加</span><a name="1_3_"></a>

    .\cereal-master\include

@import "images/cereal02.png"

---

#### <span style="color:#334488;">2. skinned_mesh.h に必要なヘッダファイルをインクルードする
</span><a name="2_"></a>

|実装するファイル|
--|--
**skinned_mesh.h**


```cpp
    ・・・省略・・・
    #include <unordered_map>
    
*   #include <cereal/archives/binary.hpp>
*   #include <cereal/types/memory.hpp>
*   #include <cereal/types/vector.hpp>
*   #include <cereal/types/set.hpp>
*   #include <cereal/types/unordered_map.hpp>

    ・・・省略(下に struct skeleton の定義)・・・
```


---

#### <span style="color:#334488;">3. skinned_mesh.h に DirectXMath 構造体用の serialize テンプレート関数を定義する</span><a name="3_"></a>

今回シリアライズするデータは「ベクトルの値」と「行列の値」になります。
まずはそれらのデータを cereal フレームワークを利用して保存するための関数を実装しましょう。

|実装するファイル|
--|--
**skinned_mesh.h**|

```cpp
・・・省略(上にファイルの include )・・・

namespace DirectX
{
    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
            cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
            cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
            cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
            cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

・・・省略(下に struct skeleton)・・・
```

---

#### <span style="color:#334488;">4. シリアライズしたいデータを持つ構造体に serialize テンプレート関数を定義する</span><a name="4_"></a>

上記で作成した serialize 関数は汎用的な２次元、３次元、４次元のベクトルと行列の情報を保存する為の関数でした。

それら以外の固有の特殊な値を保有している構造体の値を保存するための serialize 関数を、それぞれ保存したい構造体の内部に定義していきましょう。

これらの serialzie 関数は定義した構造体が生成されるタイミングで保存する形に変換されます。

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**skeleton::bone**

```cpp
        ・・・省略・・・
        // true...親無しのボーン
        bool is_orphan() const { return parent_index < 0; };

*       template<class T>
*       void serialize(T& archive)
*       {
*           archive(unique_id, name, parent_index, node_index, offset_transform);
*       }
    };
    std::vector<bone> bones;
    ・・・省略・・・
```

---

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**skeleton**

```cpp
            ・・・省略・・・
            return -1;
        }

*       template<class T>
*       void serialize(T& archive)
*       {
*           archive(bones);
*       }
    };

    ・・・省略・・・
```

---

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**animation::keyframe::node**

```cpp
        ・・・省略・・・
        DirectX::XMFLOAT3 translation{ 0,0,0 };

*       template<class T>
*       void serialize(T& archive)
*       {
*           archive(global_transform, scaling, rotation, translation);
*       }
    };
    std::vector<node> nodes;
    ・・・省略・・・
```

---

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**animation::keyframe**

```cpp
        ・・・省略・・・
        std::vector<node> nodes;

*       template<class T>
*       void serialize(T& archive)
*       {
*           archive(nodes);
*       }
    };
    std::vector<keyframe> sequence;
    ・・・省略・・・
```

---

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**animation**

```cpp
        ・・・省略・・・
        std::vector<keyframe> sequence;

*       template<class T>
*       void serialize(T& archive)
*       {
*           archive(name, sampling_rate, sequence);
*       }
    };
    ・・・省略・・・
```

---

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**scene::node**

```cpp
        ・・・省略・・・
        int64_t parent_index{ -1 };

*       template<class T>
*       void serialize(T& archive)
*       {
*           archive(unique_id, name, attribute, parent_index);
*       }
    };
    std::vector<node> nodes;
    ・・・省略・・・
```

---

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**scene**

```cpp
            ・・・省略・・・
            return -1;
        }

*       template<class T>
*       void serialize(T& archive)
*       {
*           archive(nodes);
*       }
    };
    ・・・省略・・・
```

---

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**skinned_mesh::vertex**

```cpp
        ・・・省略・・・
        uint32_t bone_indices[MAX_BONE_INFLUENCES]{};		// ボーン番号

*       template<class T>
*       void serialize(T& archive)
*       {
*           archive(position, normal, tangent, texcoord, bone_weights, bone_indices);
*       }
    };
    ・・・省略・・・
```

---

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**skinned_mesh::mesh::subset**

```cpp
        ・・・省略・・・
        uint32_t index_count{ 0 };			// インデックスの数(頂点数)

*       template<class T>
*       void serialize(T& archive)
*       {
*           archive(material_unique_id, material_name, start_index_location, index_count);
*       }
    };
    std::vector<subset> subsets;
    ・・・省略・・・
```

---

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**skinned_mesh::mesh**

```cpp
        ・・・省略・・・
        { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX}
    };

*   template<class T>
*   void serialize(T& archive)
*   {
*       archive(unique_id, name, node_index, subsets, default_global_transform, bind_pose, bounding_box, vertices, indices);
*   }

    private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    ・・・省略・・・
```

---

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**skinned_mesh::material**

```cpp
        ・・・省略・・・
        bool operator<(const material& rhs) const { return unique_id < rhs.unique_id; }

*       template<class T>
*       void serialize(T& archive)
*       {
*           archive(unique_id, name, Ka, Kd, Ks, texture_filenames);
*       }
    };
    std::set<material> materials;
    ・・・省略・・・
```

---

#### <span style="color:#334488;">5. skinned_mesh クラスのコンストラクタにコードを追加する</span><a name="5_"></a>

    ※このコードでは与えられた変数 fbx_filename の拡張子を“cereal”に変え、そのファイルが存在する場合はシリアライズされたデータからロードする。また、存在しない場合は従来通り FBX ファイルからロードする。
    ※skinnd_mesh クラスのデータ構造に変更があった場合はシリアライズされたファイルは無効になるので削除する必要がある。

とあります。Cereal フレームワークを利用し serialize 関数を通った値は、.cereal というファイルに保存されます。つまりシリアライズが成功した場合は cereal というファイルが外部に生成されているので、シリアライズされたそのファイルを優先的に読み込みに行くように実装変更します。
１度目の起動時や２度目でもシリアライズに失敗したりした際は .cereal ファイルが生成されません。その場合は通常通り FBX ファイルを読みに行くようにします。

必要なファイルとして **fstream** をインクルードしておいてください。

それではやってみましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::skinned_mesh(...)**

```cpp
*   // 通常のファイル名の拡張子を .fbx から .cereal に変更する
*   std::filesystem::path cereal_filename(fbx_filename);
*   cereal_filename.replace_extension("cereal");
*   if (std::filesystem::exists(cereal_filename.c_str()))
*   {
*       // シリアライズされたファイルが存在する場合
*       // シリアライズされているファイルを読み込んで
*       std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
*       cereal::BinaryInputArchive deserialization(ifs);
*       // データをデシリアライズ
*       deserialization(scene_view, meshes, materials, animation_clips);
*   }
*   else
*   {
*       // シリアライズされたファイルが存在しない場合
*       // 通常の FBX ファイル読み込み

        // fbx マネージャーを生成
        FbxManager* fbx_manager{ FbxManager::Create() };
        // fbx シーンを生成
        FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };

        ・・・・・・・・
        ・・・省略・・・
        ・・・・・・・・

        fbx_manager->Destroy();

*       // FBX ファイルを読み込んだら
*       std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
*       cereal::BinaryOutputArchive serialization(ofs);
*       // 外部ファイルにシリアライズ
*       serialization(scene_view, meshes, materials, animation_clips);
*   }

    create_com_objects(device, fbx_filename);
```

---

#### <span style="color:#334488;">6. ".\\resources\\nico.fbx"を skinned_mesh クラスのコンストラクタに指定し、１回目と２回目のロード時間の違いを確認する</span><a name="6_"></a>

skinned_mesh::initialize() の以下の skinned_meshes[0] に生成しているファイルを

```
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), 何かしらのファイル名);
```

以下のファイルに変更して下さい。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::initialize(...)**

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\nico.fbx");
```

変更したら起動して確認します。

１回目の起動してからの表示される速度と、２回目以降の起動してからの表示される速度を比べてみて下さい。
２回目以降は表示される速度がかなり早くなっているはずです。

表示される画面は以下になります。

@import "images/nico.png"

---

#### <span style="color:#334488;">7. skinned_mesh クラスの append_animations メンバ関数でアニメーションを追加した場合はシリアライズの対象にならない</span><a name="7_"></a>

現状外部にあるアニメーション用 FBX を読み込むような実装すらされていないので、まずは外部にあるアニメーション用 FBX を読み込む為の処理をコンストラクタに実装します。
すでに実装されているコンストラクタとは別に外部アニメーション用 FBX 読み込みの処理も行うコンストラクタを追加で実装していきます。

まずは宣言から、

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
    ・・・省略・・・
    skinned_mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate = false, float sampling_rate = 0);

*   /// <summary>
*   /// アニメーション用 FBX 読み込み用コンストラクタ
*   /// </summary>
*   /// <param name="device">デバイス</param>
*   /// <param name="fbx_filename">fbx ファイル名</param>
*   /// <param name="animation_filenames">アニメーション FBX ファイル名のリスト</param>
*   /// <param name="triangulate">true...多角形で作られたポリゴンを三角形化	false...三角形化しない</param>
*   /// <param naem="sampling_rate">サンプリングレート</param>
*   skinned_mesh(ID3D11Device* device, const char* fbx_filename, std::vector<std::string>& animation_filenames, bool triangulate = false, float sampling_rate = 0);

    virtual ~skinned_mesh() = default;
    ・・・省略・・・
```

---

次に定義ですが、まずはすでに実装されている skinned_mesh コンストラクタの中身をすべてコピー＆ペーストしてください。

|実装するファイル|
--|--
**skinned_mesh.h**|

```cpp
*   /// <summary>
*   /// アニメーション用 FBX 読み込み用コンストラクタ
*   /// </summary>
*   /// <param name="device">デバイス</param>
*   /// <param name="fbx_filename">fbx ファイル名</param>
*   /// <param name="animation_filenames">アニメーション FBX ファイル名のリスト</param>
*   /// <param name="triangulate">true...多角形で作られたポリゴンを三角形化	false...三角形化しない</param>
*   /// <param naem="sampling_rate">サンプリングレート</param>
*   skinned_mesh::skinned_mesh(ID3D11Device* device, const char* fbx_filename, std::vector<std::string>& animation_filenames, bool triangulate = false, float sampling_rate = 0)
*   {
*       ・・・省略(既に実装済みのコンストラクタの中身を己ぴー＆ペースト)・・・
*   }
```

---

コピー＆ペーストしたらアニメーション用 FBX の読み込みの実装を挿入します。
通常の FBX ファイルの読み込みが完了した段階でアニメーション用の FBX の読み込みを行うようにします。

|実装するファイル|実装するメンバ関数(新しく宣言したコンストラクタ)
--|--
**skinned_mesh.cpp**|**skinned_mesh::skinned_mesh(...)**

```cpp
    ・・・省略・・・
    fetch_animations(fbx_scene, animation_clips, sampling_rate);

    fbx_manager->Destroy();

*   // 通常の FBX ファイルの読み込みの後にアニメーション FBX ファイルの読み込みを行うようにする
*   for (const std::string animation_filename : animation_filenames)
*   {
*       append_animations(animation_filename.c_str(), sampling_rate);
*   }

    // FBX ファイルを読み込んだら
    std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
    ・・・省略・・・
```

これで完成です。

アニメーション用 FBX のシリアライズ化は実装だけ行っていて確認のための呼び出しなどは行っていません。
確認は各自で行ってください。