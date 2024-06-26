<!--
UNIT17_EXCERCISE UNIT17 演習手順
SKINNED MESH FBX SDK

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

FBX SDK
Scene
Node
-->

# UNIT17_EXCERCISE SKINNED MESH FBX SDK 演習手順

---

UNIT17 ではまず FBX の初期化と読み込み、そして中にあるすべてのノードの情報の表示までをやってみましょう。


* 目次
    * [1. FBX SDK のセットアップ](#FBXSDKのセットアップ_)
	* [2. スキンメッシュクラスの実装](#スキンメッシュクラスの実装_)
	* [3. スキンメッシュの変数を宣言](#スキンメッシュの変数を宣言_)
	* [4. スキンメッシュのオブジェクトを生成](#スキンメッシュのオブジェクトを生成_)
	* [5. 実行し確認](#実行し確認_)
	* [6. 別のFBXファイルをロードして実行し確認](#別のFBXファイルをロードして実行し確認_)

---

#### <span style="color:#334488;">1. FBX SDK のセットアップ</span><a name="FBXSDKのセットアップ_"></a>

    演習手順１

演習手順の１－１から１－６は FBX の設定です。
２つの sdk のインストールとヘッダファイルとライブラリの設定を行っていきます。
以前の DirectXTK で行った時と同じ要領で 3dgp のプロパティを開いて指定されている場所に指定のパスやファイル名を記載していってください。

---

#### <span style="color:#334488;">2. スキンメッシュクラスの実装</span><a name="スキンメッシュクラスの実装_"></a>

    演習手順２－１


まずは新規で 
	
	skinned_mesh.h
	skinned_mesh.cpp
	
の２つのファイルを作成して下さい。

---

#### skinned_mesh.h にシーン構造体を定義する

    演習手順２－２

**FbxScene** に流し込んだ**ノード**の情報すべてを **vector** に保管し保存しておくためのデータ保管用の構造体です。
ノードごとに設定されているユニークな ID を調べてそのノードの **vector** 上での添え字番号を取得することのできる **indexof(...)** メンバ関数を実装しておくことで、**ノード**の検索を比較的簡単に出来るようにしています。

|実装するファイル|
--|--
skinned_mesh.h|

```cpp
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>

/// <summary>
/// シーン
/// </summary>
struct scene
{
    /// <summary>
    /// ノード
    /// </summary>
    struct node
    {
        // 識別 ID
        uint64_t unique_id{ 0 };
        // 名商
        std::string name;
        // 種類
        FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
        // 親ノードの識別 ID
        int64_t parent_index{ -1 };
    };
    std::vector<node> nodes;

    /// <summary>
    /// 上記のノード内にある指定のユニーク ID のインデックス番号を調べる
    /// </summary>
    /// <param name="unique_id">指定のユニーク ID</param>
    /// <returns>インデックス番号</returns>
    int64_t indexof(uint64_t unique_id) const
    {
        int64_t index{ 0 };
        for (const node& node : nodes)
        {
            if (node.unique_id == unique_id)
            {
                return index;
            }
            ++index;
        }
        return -1;
    }
};
```

---

#### skinned_mesh.h にスキンメッシュクラスを定義する

基本はスタティックメッシュとあまり違いはありませんが上記で実装した **scene** の構造体を内包しており、その **scene** にあるノードを利用して頂点データを構築します。

|実装するファイル|
--|--
skinned_mesh.h|

```cpp
・・・省略(上に scene 構造体)・・・

/// <summary>
/// スキンメッシュ
/// </summary>
class skinned_mesh
{
public:
    // 頂点フォーマット
    struct vertex
    {
        DirectX::XMFLOAT3 position;	// 頂点座標
        DirectX::XMFLOAT3 normal;	// 法線
        DirectX::XMFLOAT2 texcoord;	// テクスチャ座標
    };

    // 定数バッファフォーマット
    struct constants
    {
        DirectX::XMFLOAT4X4 world;			// ワールド行列
        DirectX::XMFLOAT4 material_color;	// マテリアルカラー
    };

private:
    // 頂点シェーダー
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    // ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    // 入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
public:
    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="fbx_filename">fbx ファイル名</param>
    /// <param name="triangulate">true...多角形で作られたポリゴンを三角形化	false...三角形化しない</param>
    skinned_mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate = false);
    virtual ~skinned_mesh() = default;
protected:
    // この fbx の親シーン
    scene scene_view;
};
```

---


#### skinned_mesh.cpp にスキンメッシュのコンストラクタを実装する

    演習手順２－３

OBJ ファイルのモデルを表示する際に OBJ ファイルの解析があったように FBX ファイルのモデルを表示する為の FBX ファイルの解析をスキンメッシュのコンストラクタで行います。

OBJ ファイルの解析は

	文字列を１行１行読み込み、１行に対してのそれぞれ(キーワードごと)の処理を行う

というものでした。この「それぞれの処理を行う」の中に頂点データを読み込む処理などが含まれており、その読み込んだ頂点データを頂点バッファとして登録することでモデルを表示させています。

FBX ファイルの解析も、読み込んだ頂点データを頂点バッファに登録するという部分まで行けばほぼほぼ同じなのですが、頂点データを読み込むという部分が以下になります。

	FBX SDK を利用してノードを取得し、ノードに対してのそれぞれ(Attributeごと)の処理を行う

というのが FBX ファイルにおける解析になります。

ということでまずは FBX SDK を初期化して全ノードを取得しノードに何が記述されているかを確認してみよう。

|実装するファイル
--|--
**skinned_mesh.cpp**




```cpp
#include "misc.h"
#include "skinned_mesh.h"
#include <sstream>
#include <functional>
using namespace DirectX;

/// <summary>
/// constructor
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="fbx_filename">fbx ファイル名</param>
/// <param name="triangulate">true...多角形で作られたポリゴンを三角形化	false...三角形化しない</param>
skinned_mesh::skinned_mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate)
{

    // fbx マネージャーを生成
    FbxManager* fbx_manager{ FbxManager::Create() };
    // fbx シーンを生成
    FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };

    // fbx インポーターの生成
    FbxImporter* fbx_importer{FbxImporter::Create(fbx_manager, "")};
    bool import_status{ false };

    // fbx ファイルを読み込む
    import_status = fbx_importer->Initialize(fbx_filename);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    // 読み込んだファイルの情報を fbx シーンに流し込む
    import_status = fbx_importer->Import(fbx_scene);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    FbxGeometryConverter fbx_converter(fbx_manager);
    if (triangulate)
    {
        // 多角形で作られたポリゴンをすべて三角形にする
        fbx_converter.Triangulate(fbx_scene, true, false);
        fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
    }

    // ノードの情報を解析する関数
    std::function<void(FbxNode*)> traverse {
        [&](FbxNode* fbx_node) {
            // scene_view.nodes に新しくノードを取り付け、取り付けたノードをローカルの node に代入し値を設定していく
            scene::node& node{scene_view.nodes.emplace_back()};
            node.attribute = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
            node.name = fbx_node->GetName();
            node.unique_id = fbx_node->GetUniqueID();
            node.parent_index = scene_view.indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
            // 現在解析しているノードに取り付けられているの子ノードを解析していく
            for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
            {
                traverse(fbx_node->GetChild(child_index));
            }
        } 
    };

    // ルートノードから解析開始
    traverse(fbx_scene->GetRootNode());

#if 1
    // シーンに取り付けたすべてのノードの情報を表示
    for (const scene::node& node : scene_view.nodes)
    {
        // ノード名でノードを検索し取得
        FbxNode* fbx_node{fbx_scene->FindNodeByName(node.name.c_str())};
        std::string node_name = fbx_node->GetName();
        uint64_t uid = fbx_node->GetUniqueID();
        uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;
        int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;

        std::stringstream debug_string;
        debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << "\n";
        OutputDebugStringA(debug_string.str().c_str());
    }
#endif
    fbx_manager->Destroy();
}
```

---

#### <span style="color:#334488;">3. スキンメッシュの変数を宣言</span><a name="スキンメッシュの変数を宣言_"></a>

    演習手順３

framework クラスのメンバ変数として skinned_mesh *型の配列を要素数８で宣言します。

|実装するファイル|実装するクラス|
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・

*   // スキンメッシュ
*   std::unique_ptr<skinned_mesh> skinned_meshes[8];

    framework(HWND hwnd);
    ・・・省略・・・
```

---

#### <span style="color:#334488;">4. スキンメッシュのオブジェクトを生成</span><a name="スキンメッシュのオブジェクトを生成_"></a>

    演習手順４

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.000.fbx");

*   return true;
```

---

#### <span style="color:#334488;">5. 実行し確認</span><a name="実行し確認_"></a>

    演習手順５

実行して確認を行います。

今回から FBX のみを表示していくので前回までに表示していたすべてを非表示にしておいてください。

つまり **framework::initialize()** にある以下の部分をコメントアウトするか削除しておいてください。

```cpp
static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Rock\\rock.obj", false	);

/// バウンディングボックス用のメッシュ
static_meshes[1] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\cube.obj", false);
create_ps_from_cso(device.Get(), "geometric_primitive_ps.cso", pixel_shaders[0].GetAddressOf());
```

あと **framework::render(...)** にある以下の部分をコメントアウトするか削除しておいてください。

```cpp
immediate_context->RSSetState(rasterizer_states[0].Get());
{
    // 拡大縮小行列
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
    // 回転行列
    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
    // 平行移動行列
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) };
    // ワールド行列
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, S * R * T);
    // 描画
    static_meshes[0]->render(immediate_context.Get(), world, material_color);
}

/// <summary>
/// バウンディングボックスの描画
/// </summary>
immediate_context->RSSetState(rasterizer_states[3].Get());
{
    const DirectX::XMFLOAT3 min[2]{ static_meshes[0]->bounding_box[0], static_meshes[1]->bounding_box[0] };
    const DirectX::XMFLOAT3 max[2]{ static_meshes[0]->bounding_box[1], static_meshes[1]->bounding_box[1] };

    DirectX::XMFLOAT3 dimensions[2]
    {
        { max[0].x - min[0].x, max[0].y - min[0].y, max[0].z - min[0].z },
        { max[1].x - min[1].x, max[1].y - min[1].y, max[1].z - min[1].z }
    };
    DirectX::XMFLOAT3 barycenters[2]
    {
        { (max[0].x + min[0].x) * 0.5f, (max[0].y + min[0].y) * 0.5f, (max[0].z + min[0].z) * 0.5f },
        { (max[1].x + min[1].x) * 0.5f, (max[1].y + min[1].y) * 0.5f, (max[1].z + min[1].z) * 0.5f }
    };
    DirectX::XMFLOAT3 ratio{ dimensions[0].x / dimensions[1].x, dimensions[0].y / dimensions[1].y, dimensions[0].z / dimensions[1].z };
    DirectX::XMFLOAT3 offset{ barycenters[0].x - barycenters[1].x, barycenters[0].y - barycenters[1].y, barycenters[0].z - barycenters[1].z };
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(ratio.x * scaling.x, ratio.y * scaling.y, ratio.z * scaling.z) };
    DirectX::XMMATRIX O{ DirectX::XMMatrixTranslation(offset.x, offset.y, offset.z) };
    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) };
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, S * O * R * T);
    static_meshes[1]->render(immediate_context.Get(), world, { 1, 0, 0, 1 }, pixel_shaders[0].Get());
}
```

起動して Visual Studio の「出力」に以下のようにノードの情報が表示されていれば完了です。

@import "images/出力01.png"

---

#### <span style="color:#334488;">6. 別のFBXファイルをロードして実行し確認</span><a name="別のFBXファイルをロードして実行し確認_"></a>

    演習手順６

演習手順にあるように plantune.fbx を読み込んでノード情報を確認してみましょう。

framework::initialize() の以下の部分のファイル名を変更し、起動して確認してみてください。

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.000.fbx");
```

さっきとは違いたくさんの情報が表示されたはずです。

