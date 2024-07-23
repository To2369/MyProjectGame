<!--
UNIT29_EXCERCISE UNIT29 演習手順
ADVANCED SKINNED MESH

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Normal mapping
Tangent vector
Bounding box
-->

# UNIT29_EXCERCISE ADVANCED SKINNED MESH 演習手順

* 目次
    1. [鏡面反射(スペキュラー)と法線マッピング表現を実装する]
        1. [framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\resources\\plantune.fbx に変更する](#1_1_)
        2. [skinned_mesh クラスの vertex 構造体にメンバ変数を追加する](#1_2_)
        3. [skinned_mesh クラスの create_com_objects メンバ関数で D3D11_INPUT_ELEMENT_DESC を変更する](#1_3_)
        4. [skinned_mesh クラスの fetch_meshes メンバ関数で法線ベクトルの値を取得する](#1_4_)
        5. [skinned_mesh クラスの fetch_materials メンバ関数で法線マップのファイル名を取得する](#1_5_)
        6. [skinned_mesh クラスの create_com_objects メンバ関数で法線マップのシェーダーリソースビューを生成する](#1_6_)
        7. skinned_mesh クラスの render メンバ関数で法線マップのシェーダーリソースビューをバインドする
        8. [シェーダヘッダ(skinned_mesh.hlsli)のコードを変更する](#1_8_)
        9. [頂点シェーダ(skinned_mesh_vs.hlsl) のコードを変更する](#1_9_)
        10. [ピクセルシェーダ(skinned_mesh_ps.hlsl)のコードを変更する](#1_10_)
    2. バウンディングボックス(境界ボックス)情報を保持する
        1. [skinned_mesh::mesh 構造体にメンバ変数に追加する](#2_1_)
        2. [skinned_mesh クラスの fetch_meshes メンバ関数に下記コードを追加する](#2_2_)
    3. [これまでにテストで使ったすべての FBX ファイルを描画できるようにする](#3_)
    4. スキンメッシュを使った3Dゲームを制作する

---

#### <span style="color:#334488;">1. 鏡面反射(スペキュラー)と法線マッピング表現を実装する</span>

---

#### <span style="color:#334488;">1 - 1. framework クラスの initialize メンバ関数で skinned_mesh コンストラクタ引数を.\\resources\\plantune.fbx に変更する</span><a name="1_1_"></a>

※ plantune.fbx は右手系・Z 軸アップ・センチメートル単位・三角形化済み

考えて変更してみてください。

---

#### <span style="color:#334488;">1 - 2. skinned_mesh クラスの vertex 構造体にメンバ変数を追加する</span><a name="1_2_"></a>

シェーダー側で接線ベクトルの値を利用するので頂点フォーマットに接線ベクトル用の変数を追加しましょう。

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**skinned_mesh::vertex**

```cpp
    struct vertex
    {
        DirectX::XMFLOAT3 position;	// 頂点座標
        DirectX::XMFLOAT3 normal;	// 法線
*       DirectX::XMFLOAT4 tangent;	// 接線ベクトル
        DirectX::XMFLOAT2 texcoord;	// テクスチャ座標
        float bone_weights[MAX_BONE_INFLUENCES]{ 1,0,0,0 };	// ウェイト値
        uint32_t bone_indices[MAX_BONE_INFLUENCES]{};		// ボーン番号
    };
```

---

#### <span style="color:#334488;">1 - 3. skinned_mesh クラスの create_com_objects メンバ関数で D3D11_INPUT_ELEMENT_DESC を変更する</span><a name="1_3_"></a>

    ※セマンティックは "TANGENT" にすること

頂点フォーマットに設定した接線ベクトルを入力フォーマットにも設定しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::create_com_objects(...)**

```cpp
    ・・・省略・・・
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*   { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
*       D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    ・・・省略・・・
```

---

#### <span style="color:#334488;">1 - 4. skinned_mesh クラスの fetch_meshes メンバ関数で接線ベクトルの値を取得する</span><a name="1_4_"></a>

    ※下記コードをテクスチャ座標取得のコードの後に挿入する
    ※ファイルに法線ベクトル情報を持たない場合は GenerateTangentsData 関数で生成する

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::fetch_meshes(...)**

```cpp
        ・・・省略・・・
        vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
    }

*   // 接線ベクトルの取得
*   if (fbx_mesh->GenerateTangentsData(0, false))
*   {
*       const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
*       vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
*       vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
*       vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
*       vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
*   }

    // 現在のインデックス番号部分に頂点データを設定
    mesh.vertices.at(vertex_index) = std::move(vertex);
    ・・・省略・・・
```

---

#### <span style="color:#334488;">1 - 5. skinned_mesh クラスの fetch_materials メンバ関数で法線マップのファイル名を取得する</span><a name="1_5_"></a>

    ※material 構造体の texture_filenames[1] に法線マップのファイル名を格納する

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::fetch_material(...)**

```cpp
        material.Ks.w = 1.0f;
    }

*   // 法線マップのファイル名
*   fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
*   if (fbx_property.IsValid())
*   {
*       const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
*       // ファイル名が存在したら相対パス込みでのファイル名を取得
*       material.texture_filenames[1] = fbx_texture ? fbx_texture->GetRelativeFileName() : "dummy normal map";
*   }

    // 取得したマテリアルの情報を設定する
    materials.insert(std::move(material));
```

---

#### <span style="color:#334488;">1 - 6. skinned_mesh クラスの create_com_objects メンバ関数で法線マップのシェーダーリソースビューを生成する</span><a name="1_6_"></a>

    ※UNIT.19 1-4 で実装したコードを変更する

以下のシェーダーリソースビューを作成している部分を

```cpp
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
if (material.texture_filenames[0].size() > 0)
{
    // テクスチャのパスを解決する
    std::filesystem::path path(fbx_filename);
    path.replace_filename(material.texture_filenames[0]);
    D3D11_TEXTURE2D_DESC texture2d_desc;
    // テクスチャを生成
    load_texture_from_file(device, path.c_str(),
        shader_resource_view.GetAddressOf(), &texture2d_desc);
}
else
{
    // ダミーテクスチャの生成
    make_dummy_texture(device, shader_resource_view.GetAddressOf(), 0xFFFFFFFF, 16);
}
// ファイル名でシェーダーリソースビューを登録
shader_resource_views.insert(std::make_pair(material.texture_filenames[0], shader_resource_view));
```

以下のように変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::create_com_objects(...)**

```cpp
*   for (size_t texture_index = 0; texture_index < 2; ++texture_index)
*   {
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
*       if (material.texture_filenames[texture_index].size() > 0)
        {
            // テクスチャのパスを解決する
            std::filesystem::path path(fbx_filename);
*           path.replace_filename(material.texture_filenames[texture_index]);
            D3D11_TEXTURE2D_DESC texture2d_desc;
            // テクスチャを生成
            load_texture_from_file(device, path.c_str(),
                shader_resource_view.GetAddressOf(), &texture2d_desc);
        }
        else
        {
*           const_cast<std::string*>(
*               &material.texture_filenames[texture_index])->assign(
*                   texture_index == 1 ? "dummy_normal_map" : "dummy_diffuse_map");
            // ダミーテクスチャの生成
            make_dummy_texture(
*               device, shader_resource_view.GetAddressOf(), texture_index == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
        }
        // ファイル名でシェーダーリソースビューを登録
*       shader_resource_views.insert(std::make_pair(material.texture_filenames[texture_index], shader_resource_view));
*   }
```

---

#### <span style="color:#334488;">1 - 7. skinned_mesh クラスの render メンバ関数で法線マップのシェーダーリソースビューをバインドする</span>

---

#### <span style="color:#334488;">1 - 8. シェーダヘッダ(skinned_mesh.hlsli)のコードを変更する</span><a name="1_8_"></a>

頂点フォーマット、入力レイアウトに設定した接線ベクトルの値をシェーダー側で受け取れるように実装していきましょう。

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.hlsli**|**VS_IN**

```hlsl
    ・・・省略・・・
    float4 normal : NORMAL;
*   float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    ・・・省略・・・
```

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.hlsli**|**VS_OUT**

```hlsl
    ・・・省略・・・
    float4 world_normal : NORMAL;
*   float4 world_tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    ・・・省略・・・
```

---

#### <span style="color:#334488;">1 - 9. 頂点シェーダ(skinned_mesh_vs.hlsl) のコードを変更する</span><a name="1_9_"></a>

接線ベクトルの情報を取得してボーンの影響を計算してピクセルシェーダー側に渡す処理を実装しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh_vs.hlsl**|**main(...)**

```cpp
    vin.normal.w = 0;
*   float sigma = vin.tangent.w;
*   vin.tangent.w = 0;

    float4 blended_position = { 0, 0, 0, 1 };
    float4 blended_normal = { 0, 0, 0, 0 };
*   float4 blended_tangent = { 0, 0, 0, 0 };
    for (int bone_index = 0; bone_index < 4; ++bone_index)
    {
        ・・・省略・・・
*       blended_tangent += vin.bone_weights[bone_index] * mul(vin.tangent, bone_transforms[vin.bone_indices[bone_index]]);
	}
    vin.position = float4(blended_position.xyz, 1.0f);
    vin.normal = float4(blended_normal.xyz, 0.0f);
*   vin.tangent = float4(blended_tangent.xyz, 0.0f);

    ・・・省略・・・

    vout.world_position = mul(vin.position, world);
*   //vin.normal.w = 0;
    vout.world_normal = normalize(mul(vin.normal, world));
*   vout.world_tangent = normalize(mul(vin.tangent, world));
*   vout.world_tangent.w = sigma;

    vout.texcoord = vin.texcoord;
    ・・・省略・・・
```

---

#### <span style="color:#334488;">1 - 10. ピクセルシェーダ(skinned_mesh_ps.hlsl)のコードを変更する</span><a name="1_10_"></a>

法線マップと接線ベクトルの計算をピクセルシェーダーに実装しましょう。

|実装するファイル|実装する関数|
--|--
**skinned_mesh_ps.hlsl**|**main(...)**

```hlsl
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
*   float alpha = color.a;
    float3 N = normalize(pin.world_normal.xyz);
*   float3 T = normalize(pin.world_tangent.xyz);
*   float sigma = pin.world_tangent.w;
*   T = normalize(T - dot(N, T));
*   float3 B = normalize(cross(N, T) * sigma);

*   float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
*   normal = (normal * 2.0) - 1.0;
*   N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));

    float3 L = normalize(-light_direction.xyz);
    float3 diffuse = color.rgb * max(0, dot(N, L));
*   float3 V = normalize(camera_position.xyz - pin.world_position.xyz);
*   float specular = pow(max(0, dot(N, normalize(V + L))), 128);
*   return float4(diffuse + specular, alpha) * pin.color;
```

---

#### <span style="color:#334488;">2. バウンディングボックス(境界ボックス)情報を保持する</span>

fbx モデルでバウンディングボックスを生成してみましょう。

---

#### <span style="color:#334488;">2 - 1. skinned_mesh::mesh 構造体にメンバ変数に追加する</span><a name="2_1_"></a>

右上奥と左下手前の点を用意する考え方や宣言方法は obj モデルの時に作ったバウンディングボックスと全く同じです。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh::mesh**

```cpp
        ・・・省略・・・

        // バインドポーズ(初期姿勢)
        skeleton bind_pose;

*       // バウンディングボックス
*       DirectX::XMFLOAT3 bounding_box[2]
*       {
*           { +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX},
*           { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX}
*       };

    private:
        ・・・省略・・・
```

---

#### <span style="color:#334488;">2 - 2. skinned_mesh クラスの fetch_meshes メンバ関数に下記コードを追加する</span><a name="2_2_"></a>

バウンディングボックスの座標を計算します。

これも obj モデルの時と実装は同じです。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::fetch_meshes(...)**

```cpp
            ・・・省略・・・
            subset.index_count++;
        }
    }

*   // メッシュごとのバウンディングボックスの座標を計算
*   for (const vertex& v : mesh.vertices)
*   {
*       mesh.bounding_box[0].x = std::min<float>(mesh.bounding_box[0].x, v.position.x);
*       mesh.bounding_box[0].y = std::min<float>(mesh.bounding_box[0].y, v.position.y);
*       mesh.bounding_box[0].z = std::min<float>(mesh.bounding_box[0].z, v.position.z);
*       mesh.bounding_box[1].x = std::max<float>(mesh.bounding_box[1].x, v.position.x);
*       mesh.bounding_box[1].y = std::max<float>(mesh.bounding_box[1].y, v.position.y);
*       mesh.bounding_box[1].z = std::max<float>(mesh.bounding_box[1].z, v.position.z);
*   }
    ・・・省略・・・
```

---

#### <span style="color:#334488;">3. これまでにテストで使ったすべての FBX ファイルを描画できるようにする</span><a name="3_"></a>

現状ボーンアニメーションのデータが含まれていない fbx データを描画することはできません。

なので、ボーンアニメーションデータの含まれていない fbx データに対してダミーのボーン行列を設定します。

**skinned_mesh::render(...)** の以下の部分を

```cpp
・・・省略・・・

constants data;
//data.world = world;
//XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
const animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));

const size_t bone_count{ mesh.bind_pose.bones.size() };
for (int bone_index = 0; bone_index < bone_count; ++bone_index)
{
    const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index)};
    const animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
    XMStoreFloat4x4(&data.bone_transforms[bone_index],
        XMLoadFloat4x4(&bone.offset_transform) *
        XMLoadFloat4x4(&bone_node.global_transform) *
        XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
    );
}

・・・省略・・・
```

以下のように変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::render(...)**

```cpp
    ・・・省略・・・

    constants data;

*   if (keyframe && keyframe->nodes.size() > 0)
*   {
        const animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
        XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));

        const size_t bone_count{ mesh.bind_pose.bones.size() };
*       _ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

        for (int bone_index = 0; bone_index < bone_count; ++bone_index)
        {
            const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
            const animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
            XMStoreFloat4x4(&data.bone_transforms[bone_index],
                XMLoadFloat4x4(&bone.offset_transform) *
                XMLoadFloat4x4(&bone_node.global_transform) *
                XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
            );
        }
*   }
*   else 
*   {
*       XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));

*       // ダミーのボーン行列を設定
*       for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
*       {
*           data.bone_transforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
*       }
*   }

    ・・・省略・・・
```

---

次に準備いていた法線マップテクスチャの ShaderResourceView をシェーダー側に登録しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::render(...)**

```cpp
    ・・・省略・・・

    immediate_context->PSSetShaderResources(0, 1,
        shader_resource_views.at(material.texture_filenames[0]).GetAddressOf());

*   // 法線マップテクスチャ
*   immediate_context->PSSetShaderResources(1, 1,
*       shader_resource_views.at(material.texture_filenames[1]).GetAddressOf());

    immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
    ・・・省略・・・
```

---

ダミーのボーン行列の設定と法線マップテクスチャの登録が実装出来たら実行してみましょう。

以下のようなに描画されるはずです。

@import "images/法線マップテクスチャ01.png"