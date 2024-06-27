<!--
UNIT17_EXCERCISE UNIT18 演習手順
SKINNED MESH GEOMETRY

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Scene
Node
Mesh
-->

# UNIT18_EXCERCISE SKINNED MESH GEOMETRY 演習手順


* 目次
    * [1. シェーダーファイルの追加](#シェーダーファイルの追加_)
    * [2. スキンメッシュクラスの実装](#スキンメッシュクラスの実装)
    * [3. フレームワーククラスの描画メンバ関数でスキンメッシュクラスの描画メンバ関数を呼び出す](#フレームワーククラスの描画メンバ関数でスキンメッシュクラスの描画メンバ関数を呼び出す_)
    * [4. 実行して正立方体が描画される事を確認する](#実行して正立方体が描画される事を確認する_)

---

#### <span style="color:#334488;">1. シェーダーファイルの追加</span><a name="シェーダーファイルの追加_"></a>

---

    演習手順１－１

スキンメッシュ用のシェーダーヘッダーファイル **skinned_mesh.hlsli** を新規で作成して、実装しましょう。

|実装するファイル|
--|--
**skinned_mesh.hlsli**|

```hlsl
struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION;
    float4 world_normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 material_color;
};
cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    float4 light_direction;
    float4 camera_position;
};
```

頂点シェーダー側の引数として設定される入力レイアウトの部分も VS_IN として構造体で実装している点に注意して下さい。

---

    演習手順１－２

スキンメッシュ用の頂点シェーダーファイル **skinned_mesh_vs.hlsl** を新規で作成し、実装しましょう。

|実装するファイル|
--|--
**skinned_mesh_vs.hlsl**|

```hlsl
#include "skinned_mesh.hlsli"
VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.position = mul(vin.position, mul(world, view_projection));
    vout.world_position = mul(vin.position, world);
    vin.normal.w = 0;
    vout.world_normal = normalize(mul(vin.normal, world));
    vout.texcoord = vin.texcoord;
    vout.color = material_color;
    return vout;
}
```

シェーダーヘッダー側で実装した main に設定する引数も構造体として設定しています。
後で説明しますが、ここが構造体になっても入力レイアウト側の設定は変化しません。

---

    演習手順１－３

スキンメッシュ用のピクセルシェーダーファイル **skinned_mesh_ps.hlsl** を新規で作成し、実装しましょう。

|実装するファイル|
--|--
**skinned_mesh_ps.hlsl**|

```hlsl
#include "skinned_mesh.hlsli"
float4 main(VS_OUT pin) : SV_TARGET
{
    float3 N = normalize(pin.world_normal.xyz);
    float3 L = normalize(-light_direction.xyz);
    float3 diffuse = max(0, dot(N,L));
    return float4(diffuse, 1) * pin.color;
}
```

---

#### <span style="color:#334488;">2. スキンメッシュクラスの実装</span><a name="スキンメッシュクラスの実装_"></a>

#### スキンメッシュクラスにメッシュ構造体を実装

    演習手順２－１

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
    ・・・省略・・・
        DirectX::XMFLOAT4 material_color;
    };

*   /// <summary>
*   /// メッシュ情報
*   /// </summary>
*   struct mesh
*   {
*       uint64_t unique_id{ 0 };        // 識別 ID
*       std::string name;               // メッシュ名
*       int64_t node_index{ 0 };        // ノード ID
*       std::vector<vertex> vertices;   // 頂点座標
*       std::vector<uint32_t> indices;  // 頂点インデックス

*   private:
*       Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;     // 頂点バッファ
*       Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;      // インデックスバッファ
*       friend class skinned_mesh;
*   };
*   std::vector<mesh> meshes;

    ・・・省略・・・
```

この部分に注目

    friend class skinned_mesh;

mesh 構造体では **vertex_buffer** および **index_buffer** を **private** 指定していますが、あくまで **skinned_mesh** クラス外部でアクセス出来ないようにする為なので **skinned_mesh** クラスでは自由にアクセスさせたいという事で **friend** 指定を付けています。

**private** なメンバも **friend** 指定したクラスだけは **public** として扱う事ができるようになります。
今回の場合、**vertex_buffer**、**index_buffer** が **skinned_mesh** クラスで **public** としてアクセスできるようになっています。

その他のメンバは構造体なのでデフォルトで **public** な扱いになっています。

---

#### skinned_mesh クラスに fetch_meshes メンバ関数を定義・実装する

    演習手順２－２

UNIT17 で取り出したノードの情報から、今度は描画に必要な頂点座標と頂点インデックスの情報であるメッシュの情報取り出します。

メッシュ情報取り出しようのメンバ関数を static_mesh クラスに宣言しましょう。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
    protected:
        scene scene_view;
        
*       /// <summary>
*       /// メッシュ情報の取り出し
*       /// </summary>
*       /// <param name="fbx_scene">メッシュ情報の元データがあるノードの情報を持っているシーン</param>
*       /// <param name="meshes">取り出し後にメッシュ情報を設定するための出力用メッシュ</param>
*       void fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes);
```

宣言したら定義を実装しましょう。

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
・・・省略(上に skinned_mesh::skinned_mesh(...) コンストラクタの定義)・・・

/// <summary>
/// メッシュ情報の取り出し
/// </summary>
/// <param name="fbx_scene">メッシュ情報の元データがあるノードの情報を持っているシーン</param>
/// <param name="meshes">取り出し後にメッシュ情報を設定するための出力用メッシュ</param>
void skinned_mesh::fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
    // シーンの中にあるノードの情報をすべて検索
    for (const scene::node& node : scene_view.nodes)
    {
        // ノードの中からメッシュの属性を持っているノードをチェック
        if (node.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        // シーンから FbxNode を取得
        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
        // メッシュ情報を取得
        FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

        // メッシュを新規で作成し取り付け
        mesh& mesh{ meshes.emplace_back() };
        // メッシュの識別 ID の設定
        mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
        // メッシュ名の設定
        mesh.name = fbx_mesh->GetNode()->GetName();
        // メッシュに対するノード ID の割り振り
        mesh.node_index = scene_view.indexof(mesh.unique_id);

        const int polygon_count{ fbx_mesh->GetPolygonCount() };	// ポリゴン数
        mesh.vertices.resize(polygon_count * 3LL);				// 頂点座標数
        mesh.indices.resize(polygon_count * 3LL);				// 頂点インデックス数

        // uv 名の取得。後々テクスチャ座標の取得に利用。
        FbxStringList uv_names;
        fbx_mesh->GetUVSetNames(uv_names);

        // コントロールポイントの取得
        const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
        // ポリゴンの数だけ頂点データを取得
        for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
        {
            // 三角形の数分の頂点の情報を取得する
            for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
            {
                // 利用するインデックスの配列を計算
                const int vertex_index{ polygon_index * 3 + position_in_polygon };

                // 頂点座標の取得
                vertex vertex;
                const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon)};
                vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
                vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
                vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

                // 法線の取得
                if (fbx_mesh->GetElementNormalCount() > 0)
                {
                    FbxVector4 normal;
                    fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
                    vertex.normal.x = static_cast<float>(normal[0]);
                    vertex.normal.y = static_cast<float>(normal[1]);
                    vertex.normal.z = static_cast<float>(normal[2]);
                }

                // テクスチャ座標の取得
                if (fbx_mesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmapped_uv;
                    fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon, uv_names[0], uv, unmapped_uv);
                    vertex.texcoord.x = static_cast<float>(uv[0]);
                    vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }

                // 現在のインデックス番号部分に頂点データを設定
                mesh.vertices.at(vertex_index) = std::move(vertex);
                // 現在のインデックス番号を設定
                mesh.indices.at(vertex_index) = vertex_index;
            }
        }
    }
}
```

---

#### skinned_mesh クラスに create_com_objects メンバ関数を定義・実装する

    演習手順２－３

演習手順 ２－２の fetch_meshes メンバ関数内で描画に必要な情報を取り出したので、次は create_com_objects メンバ関数内で頂点とインデックスをバッファとして生成しましょう。

まずは宣言です。

**private** 指定部分に宣言して下さい。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
        ・・・省略・・・
    private:
        ・・・省略・・・

*       /// <summary>
*       /// バッファの生成
*       /// </summary>
*       /// <param name="device">デバイス</param>
*       /// <param name="fbx_filename">fbx ファイル名</param>
*       void create_com_objects(ID3D11Device* device, const char* fbx_filename);
        
        ・・・省略・・・
```


次に定義部分です。

#### インクルードしなければならないファイルはインクルードしておきましょう。

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
・・・省略(上に skinned_mesh::fetch_meshes() メンバ関数の定義)・・・

/// <summary>
/// バッファの生成
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="fbx_filename">fbx ファイル名</param>
void skinned_mesh::create_com_objects(ID3D11Device* device, const char* fbx_filename)
{
    for (mesh& mesh : meshes)
    {
        HRESULT hr{ S_OK };
        D3D11_BUFFER_DESC buffer_desc{};
        D3D11_SUBRESOURCE_DATA subresource_data{};
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * mesh.vertices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        subresource_data.pSysMem = mesh.vertices.data();
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh.vertex_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh.indices.data();
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh.index_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#if 1
        mesh.vertices.clear();
        mesh.indices.clear();
#endif
    }

    HRESULT hr = S_OK;
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    create_vs_from_cso(device, "skinned_mesh_vs.cso", vertex_shader.ReleaseAndGetAddressOf(),
        input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    create_ps_from_cso(device, "skinned_mesh_ps.cso", pixel_shader.ReleaseAndGetAddressOf());

    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
```

取り出したメッシュの情報分だけバッファを作りだしています。
頂点データごとにメッシュ情報のちがう頂点データだった場合生成するバッファも変更しています。シーン内に存在するメッシュごとの頂点バッファを生成しています。
入力レイアウト、頂点シェーダー、ピクセルシェーダーおよび定数バッファは全メッシュ共通で同じものを利用します。もちろん今回の設定では、ということになります。

---

#### skinned_mesh コンストラクタのコードを変更する

    演習手順２－４

先ほど実装した **fetch_meshes(...)** と **create_com_objects(...)** の２つのメンバ関数を **skinned_mesh()** コンストラクタのノード解析の後に呼び出しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh()**

```cpp
    ・・・省略・・・

*   fetch_meshes(fbx_scene, meshes);

    fbx_manager->Destroy();

*   create_com_objects(device, fbx_filename);
```

UNIT17 で実装した以下のデバッグ表示の部分はマクロで無効化しておいてください( if 1 を if 0 にしておく)。
全部削除しても構いません。

```cpp
#if 1
	for (const scene::node& node : scene_view.nodes)
	{
		FbxNode* fbx_node{fbx_scene->FindNodeByName(node.name.c_str())};
		// 出力ウィンドウにデバッグデータを表示
		std::string node_name = fbx_node->GetName();
		uint64_t uid = fbx_node->GetUniqueID();
		uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;
		int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;

		std::stringstream debug_string;
		debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << "\n";
		OutputDebugStringA(debug_string.str().c_str());
	}
#endif
```

---

#### skinned_mesh クラスに render メンバ関数を定義・実装する

    演習天順２－５

fbx の解析、描画に必要なメッシュ(頂点情報など)の取り出し、バッファの生成。描画の準備の実装が完了したので最後に描画部分を実装しましょう。

描画関数は framework 側で呼び出すことになるので **public** 指定に宣言して下さい。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
    public:
        ・・・省略・・・

*       /// <summary>
*       /// 描画処理
*       /// </summary>
*       /// <param name="immediate_context">デバイスコンテキスト</param>
*       /// <param name="world">ワールド行列</param>
*       /// <param name="material_color">マテリアルカラー</param>
*       void render(ID3D11DeviceContext* immediate_context,
*           const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color);

        ・・・省略・・・
```

次に定義を実装しましょう。

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
・・・省略(上に skinned_mesh::create_com_objects() メンバ関数の定義)・・・

/// <summary>
/// 描画処理
/// </summary>
/// <param name="immediate_context">デバイスコンテキスト</param>
/// <param name="world">ワールド行列</param>
/// <param name="material_color">マテリアルカラー</param>
void skinned_mesh::render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color)
{
    // メッシュごとに描画処理を行う
    for (const mesh& mesh : meshes)
    {
        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
        immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        immediate_context->IASetInputLayout(input_layout.Get());

        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

        constants data;
        data.world = world;
        data.material_color = material_color;
        immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
        immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

        D3D11_BUFFER_DESC buffer_desc;
        mesh.index_buffer->GetDesc(&buffer_desc);
        immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
    }
}
```

コメントにも記載していますが、メッシュごとに生成したバッファをそれぞれ描画するようになっています。

---

#### <span style="color:#334488;">3. フレームワーククラスの描画メンバ関数でスキンメッシュクラスの描画メンバ関数を呼び出す</span><a name="フレームワーククラスの描画メンバ関数でスキンメッシュクラスの描画メンバ関数を呼び出す_"></a>

    演習手順３

**skinned_mesh** クラスを描画します、が、ひとまず **static_mesh** を描画している場合はそちらを非表示にしておいてください。

読みこむ fbx のファイルを **cube.000.fbx** に変更しておいてください。

---

#### 拡大縮小 (S)・回転 (R)・平行移動 (T) 行列を計算する

    演習手順３－１

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
        ・・・省略・・・
        immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

*       DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
*       DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
*       DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) };

    #ifdef USE_IMGUI
        ・・・省略・・・
```

---

#### 上記３行列を合成しワールド変換行列 (world) を作成する

    演習手順３－２

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
        ・・・省略・・・
        DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) };

*       DirectX::XMFLOAT4X4 world;
*       DirectX::XMStoreFloat4x4(&world, S * R * T);

    #ifdef USE_IMGUI
        ・・・省略・・・
```

---

#### skinned_mesh クラスの render メンバ関数を呼び出す

    演習手順３－３

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
        ・・・省略・・・
        DirectX::XMStoreFloat4x4(&world, S * R * T);

*       skinned_meshes[0]->render(immediate_context.Get(), world, material_color);

    #ifdef USE_IMGUI
        ・・・省略・・・
```

これで **skinned_mesh** の描画呼び出しの実装が完了しました。

---

#### <span style="color:#334488;">4. 実行して正立方体が描画される事を確認する</span><a name="実行して正立方体が描画される事を確認する_"></a>

fbx ファイルを読み込み、解析し、メッシュを生成し、描画までの実装が完了したので実行してみましょう。

以下の立方体が表示されます。

@import "images/cube000.png"