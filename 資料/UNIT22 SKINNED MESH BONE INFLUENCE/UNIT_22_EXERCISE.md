<!--
UNIT22_EXCERCISE UNIT22 演習手順
SKINNED MESH BONE INFLUENCES

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Bone influences
-->

# UNIT22_EXCERCISE SKINNED MESH BONE INFLUENCES 演習手順

* 目次
    1. [３つのボーン持つ FBX ファイル (cube.004.fbx) をロードし各ボーンの頂点に対する影響度を確認する](#1_)
    2. [頂点構造体(vertex)にボーン番号とウェイト値を追加する](#2_)
    3. [skinned_mesh.cpp のグローバルスコープにボーン影響度を表現する構造体を定義する](#3_)
    4. [skinned_mesh.cpp のグローバルスコープにボーン影響度を FBX データから取得する関数を定義する](#4_)
    5. [頂点が影響を受けるボーン番号とウエイト値を頂点構造体(vertex)のメンバ変数にセットする](#5_)
    6. [skinned_mesh の create_com_objects メンバ関数でインプットレイアウト定義を変更する](#6_)
    7. [頂点シェーダの入力構造体(VS_IN)を変更する](#7_)
    8. [頂点シェーダ(skinned_mesh_vs.hlsl)にボーン影響度を確認するテストコードを追加する](#8_)
    9. [実行し、テスト描画を確認する](#9_)

---

#### <span style="color:#334488;">1. ３つのボーン持つ FBX ファイル (cube.004.fbx) をロードし各ボーンの頂点に対する影響度を確認する</span><a name="1_"></a>

    演習手順１

framework::initilize() の以下の部分の読み込むファイルを

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.003.1.fbx", true);
```

以下のように変更しましょう。

※の記載では cube.004.fbx は右手系のZアップのメートル単位で三角形化なし(三角形化されていない)とあるので今までと設定は変更せずにファイル名の部分のみを変更します。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.004.fbx", true);
```

実行すると以下の画像が表示されます。

@import "images/0040_1.png"

現状はボーンの影響は受けていません、座標、法線、テクスチャ座標と材質(マテリアル)の情報です。

---

#### <span style="color:#334488;">2. 頂点構造体(vertex)にボーン番号とウェイト値を追加する</span><a name="2_"></a>

    演習手順２

ボーンの情報が含まれた FBX からボーンの情報を取り出しボーンの情報をメッシュに反映させるための準備をしていきましょう。

まずはボーンの番号とウェイト値を頂点構造体に宣言しましょう。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh::vertex**

```cpp
*   // 最大ボーン影響値
*   static const int MAX_BONE_INFLUENCES{ 4 };
    // 頂点フォーマット
    struct vertex
    {
        DirectX::XMFLOAT3 position;	// 頂点座標
        DirectX::XMFLOAT3 normal;	// 法線
        DirectX::XMFLOAT2 texcoord;	// テクスチャ座標
*       float bone_weights[MAX_BONE_INFLUENCES]{ 1,0,0,0 };	// ウェイト値
*       uint32_t bone_indices[MAX_BONE_INFLUENCES]{};		// ボーン番号
    };
```

---

#### <span style="color:#334488;">3. skinned_mesh.cpp のグローバルスコープにボーン影響度を表現する構造体を定義する</span><a name="3_"></a>

    演習手順３

※bone_influence 構造体の１つのインスタンスは１つの頂点が影響を受けるボーン番号とその重みを表現する

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
・・・省略(上に to_xmfloat4(...) 関数の定義)・・・

/// <summary>
/// １つの頂点が影響を受けるボーンの情報
/// </summary>
struct bone_influence
{
    uint32_t bone_index;	// ボーン番号
    float bone_weight;		// ウェイト値
};
// １つの頂点は複数のボーンから影響を受ける場合があるので可変長配列で表現
using bone_influences_per_control_point = std::vector<bone_influence>;

・・・省略(下に skinned_mesh::skinned_mesh(...) コンストラクタの定義)・・・
```

---

#### <span style="color:#334488;">4. skinned_mesh.cpp のグローバルスコープにボーン影響度を FBX データから取得する関数を定義する</span><a name="4_"></a>

    演習手順４

ウェイト値はメッシュ内部のコントロールポイントが持っているので、実装の順番としてはすべてのスキンの中にある、すべてのクラスターの中にある、すべてのコントロールポイントからウェイト値を取得していくという実装を行います。

それではやってみましょう。

|実装するファイル|
--|--
skinned_mesh.cpp|

```cpp
・・・省略(上に using bone_influences_per_control_point の宣言)・・・

/// <summary>
/// ボーン影響度を FBX データから取得
/// </summary>
/// <param name="fbx_mesh">ボーン影響度の情報を持っている取り出し元の FBX メッシュ</param>
/// <param name="bone_influences">取り出したボーン影響度</param>
void fetch_bone_influences(const FbxMesh* fbx_mesh, std::vector<bone_influences_per_control_point>& bone_influences)
{
    // ボーン影響度の数 = FBX メッシュにあるコントロールポイントの数に設定
    const int control_points_count{ fbx_mesh->GetControlPointsCount() };
    bone_influences.resize(control_points_count);

    // メッシュにあるスキンの数を取得
    const int skin_count{ fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) };
    // メッシュにあるすべてのスキンの情報をチェック
    for (int skin_index = 0; skin_index < skin_count; ++skin_index)
    {
        // 現在のスキンを取得
        const FbxSkin* fbx_skin
            { static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index, FbxDeformer::eSkin)) };

        // スキンにあるクラスターの数を取得
        const int cluster_count{ fbx_skin->GetClusterCount() };
        // スキンにあるすべてのクラスターの情報をチェック
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
        {
            // 現在のクラスターの情報をチェック
            const FbxCluster* fbx_cluster{ fbx_skin->GetCluster(cluster_index) };

            // クラスターにあるコントロールポイントの数を取得
            const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
            // クラスターにあるすべてのコントロールポイントのウェイトの値を取得
            for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count;
                ++control_point_indices_index)
            {
                // 現在のコントロールポイントの番号
                int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
                // コントロールポイントのウェイト値
                double control_point_weight
                    { fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
                // 新規にボーン影響度を生成して追加
                bone_influence& bone_influence{ bone_influences.at(control_point_index).emplace_back() };
                // 現在のクラスターの番号をボーン番号として設定
                bone_influence.bone_index = static_cast<uint32_t>(cluster_index);
                // コントロールのウェイト値をボーンのウェイト値として設定
                bone_influence.bone_weight = static_cast<float>(control_point_weight);
            }
        }
    }
}

・・・省略(下に skinned_mesh::skinned_mesh(...) コンストラクタの定義)・・・
```

---

#### <span style="color:#334488;">5. 頂点が影響を受けるボーン番号とウエイト値を頂点構造体(vertex)のメンバ変数にセットする</span><a name="5_"></a>

    演習手順５

上記の関数を呼び出し取得したボーン影響度を頂点に宣言したボーン影響度の変数に設定していきましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::fetch_meshes(...)**

```cpp
    ・・・省略・・・
    mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

*   // メッシュからボーン影響度を取得
*   std::vector<bone_influences_per_control_point> bone_influences;
*   fetch_bone_influences(fbx_mesh, bone_influences);

    std::vector<mesh::subset>& subsets{ mesh.subsets };
    ・・・・・・・・
    ・・・省略・・・
    ・・・・・・・・
            vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

*           // 頂点数番目のボーン影響度を取得
*           const bone_influences_per_control_point& influences_per_control_point{ bone_influences.at(polygon_vertex) };
*           for (size_t influence_index = 0; influence_index < influences_per_control_point.size(); ++influence_index)
*           {
*               // ボーン影響度は最大４つ
*               if (influence_index < MAX_BONE_INFLUENCES)
*               {
*                   // 取得したボーン影響度を頂点側のボーン影響度に設定していく
*                   vertex.bone_weights[influence_index] = influences_per_control_point.at(influence_index).bone_weight;
*                   vertex.bone_indices[influence_index] = influences_per_control_point.at(influence_index).bone_index;
*               }
*           }

            // 法線の取得
            if (fbx_mesh->GetElementNormalCount() > 0)
            ・・・省略・・・
```

参考手順の※の記載では

    ※影響を受けるボーンは最大４つまでとする
    ※下記コードは影響を受けるボーン数が４つを超える場合は、
    それ以降の影響度を無視している(改善しなさい)

とありました。
上記のコードではボーンの影響を４つまでしか受けることが出来ません。それ以上の数の影響を受けたい場合はコードを変更する必要があるという事です。

この部分は考えて実装してみてください。

    課題
        ４つを超えるボーンの影響を受けれるようにコードを改善する

---

#### <span style="color:#334488;">6. skinned_mesh の create_com_objects メンバ関数でインプットレイアウト定義を変更する</span><a name="6_"></a>

    演習手順６

FBX ファイルから取得したボーン影響度の情報をシェーダー側に渡してシェーダー側で扱うので入力レイアウトに設定しましましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::create_com_objects(...)**

```cpp
    ・・・省略・・・
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*       { "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
*           D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*       { "BONES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
*           D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    ・・・省略・・・
```

---

#### <span style="color:#334488;">7. 頂点シェーダの入力構造体(VS_IN)を変更する</span><a name="7_"></a>

    演習手順７

ボーン影響度の情報を頂点シェーダーヘッダーにも追加

|実装するファイル|
--|--
skinned_mesh.hlsli|

```hlsl
    struct VS_IN
    {
        float4 position : POSITION;
        float4 normal : NORMAL;
        float2 texcoord : TEXCOORD;
*       float4 bone_weights : WEIGHTS;
*       uint4 bone_indices : BONES;
    };
    ・・・省略・・・
```

---

#### <span style="color:#334488;">8. 頂点シェーダ(skinned_mesh_vs.hlsl)にボーン影響度を確認するテストコードを追加する</span><a name="8_"></a>

    演習手順８

シェーダ側にボーン影響度の情報を持っていく準備は行ったので、実際にボーン影響度を確認するテストコードを追加しましょう。

テン州手順には動作確認後 

    #if ~ #endif ディレクティブのコードは無効にすること
    
とあるので確認がすんだら

    #if 1 
    
にしてボーン影響度側のコードは無効にしておいてください。

|実装するファイル|実装する関数|
--|--
**skinned_mesh_vs.hlsl**|**mian(...)**

```cpp
        ・・・省略・・・
        vout.texcoord = vin.texcoord;

*   #if 0
        vout.color = material_color;
*   #else
*       vout.color = 0;
*       const float4 bone_colors[4] = {
*           {1, 0, 0, 1},
*           {0, 1, 0, 1},
*           {0, 0, 1, 1},
*           {1, 1, 1, 1},
*       };
*       for (int bone_index = 0; bone_index < 4; ++bone_index)
*       {
*           vout.color += bone_colors[vin.bone_indices[bone_index] % 4]
*               * vin.bone_weights[bone_index];
*       }
*   #endif

        return vout;
    }
```
---

#### <span style="color:#334488;">9. 実行し、テスト描画を確認する</span><a name="9_"></a>

    演習手順９

実行すると以下の画像が表示されます。

@import "images/0040_2.png"