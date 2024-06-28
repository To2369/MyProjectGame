<!--
UNIT19_1_EXCERCISE UNIT19 演習手順
SKINNED MESH MATERIAL

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Materials
Textures
-->

# UNIT19_1_EXCERCISE SKINNED MESH MATERIAL 演習手順

* 目次
    * [1. FBXファイルからメッシュが使用するマテリアル情報を抽出する](#1_)
    * [2. skinned_mesh の render メンバ関数でシェーダーリソースビューオブジェクトをピクセルシェーダーにバインドする](#2)
    * [3. ピクセルシェーダー（skinned_mesh_ps.hlsl）を変更す](#3_)

---

#### <span style="color:#334488;">1. FBXファイルからメッシュが使用するマテリアル情報を抽出する</span><a name="1_"></a>

---

#### skinned_mesh クラスにマテリアル構造体を実装

    演習手順１－１

OBJ ファイルでも行ったようにマテリアルが記載されている場所からマテリアルの情報を抜き取り、そこからテクスチャの情報を抜き取り、モデルの材質を生成します。
FBX はノードの中にマテリアルの情報が含まれています。
ですのでノード中からマテリアルの情報を取り出し保管します。まずはその為の構造体を定義しましょう。

構造体を定義する前に以下の２つのファイルを **skinned_mesh.h** にインクルードしておいてください。

    set
    unordererd_map

インクルードしたら構造体を定義していきましょう。
**public** 指定部分に定義して下さい。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
struct material
{
    uint64_t unique_id{ 0 };
    std::string name;

    DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
    DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
    DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };

    std::string texture_filenames[4];
    bool operator<(const material& rhs) const { return unique_id < rhs.unique_id; }
};
std::set<material> materials;
std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shader_resource_views;
```

**unique_id** はマテリアルの識別 ID で **name** はマテリアルの名前です。

**Ka** はアンビエント(環境光)で、**Kd** はディフューズ(拡散反射光)で、**Ks** はスペキュラー(鏡面反射光)です。

**texture_filenames** はテクスチャのファイル名です。

それではこれらの情報をノードから抽出していきましょう。

---

#### skinned_mesh クラスに fetch_materials メンバ関数を実装する

    演習手順１－２

マテリアル情報取り出しようのメンバ関数の宣言を **skinned_mesh** クラスに宣言しましょう。

**protected** 指定部分に宣言して下さい。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
/// <summary>
/// マテリアル情報の取り出し
/// </summary>
/// <param name="fbx_scene">マテリアル情報の元データがあるノードの情報を持っているシーン</param>
/// <param name="materials">取り出し後にマテリアル情報を設定する為の出力用マテリアル</param>
void fetch_materials(FbxScene* fbx_scene, std::set<material>& materials);
```

宣言したら定義していきましょう。

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
・・・省略(上に skinned_mesh::render(...) コンストラクタの定義)・・・

/// <summary>
/// マテリアル情報の取り出し
/// </summary>
/// <param name="fbx_scene">マテリアル情報の元データがあるノードの情報を持っているシーン</param>
/// <param name="materials">取り出し後にマテリアル情報を設定する為の出力用マテリアル</param>
void skinned_mesh::fetch_materials(FbxScene* fbx_scene, std::set<material>& materials)
{
    // シーンの中にあるノードの情報をすべて検索
    const size_t node_count{ scene_view.nodes.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        // ノードの情報１つ１つをチェック
        const scene::node& node{ scene_view.nodes.at(node_index) };
        // シーンから FbxNode を取得
        const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

        // ノードの中にあるマテリアルの情報をすべて検索
        const int material_count{ fbx_node->GetMaterialCount() };
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            // マテリアルの情報１つ１つをチェック
            const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

            material material;
            // マテリアル名の設定
            material.name = fbx_material->GetName();
            // マテリアルの識別 ID の設定
            material.unique_id = fbx_material->GetUniqueID();
            
            FbxProperty fbx_property;
            // ディフューズ(拡散反射光)の情報を取得
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            // ディフューズがあったらディフューズの情報を設定していく
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>()};
                material.Kd.x = static_cast<float>(color[0]);
                material.Kd.y = static_cast<float>(color[1]);
                material.Kd.z = static_cast<float>(color[2]);
                material.Kd.w = 1.0f;

                // テクスチャのファイル名を取得
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                // 相対パス込みでのファイル名を設定する
                material.texture_filenames[0] =
                    fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }

            // アンビエント(環境光)の情報を取得

            // スペキュラ(鏡面反射光)の情報を取得

            // 取得したマテリアルの情報を設定する
            materials.insert(std::move(material));
        }
    }
}
```

---

#### ※スペキュラ(Ks)・アンビエント(Ka)のカラー情報も取得しなさい

演習手順には スペキュラ(鏡面反射光)とアンビエント(環境光)の取得の実装は記載されておりません。

が、ディフューズとほぼほぼ同じです。
以下にヒントを記載しておきますので考えてやってみましょう。

    * FindProperty() を利用して IsValid() で確認して material のスペキュラかアンビエントの値に代入して下さい。
    * スペキュラは sSpecular、アンビエント sAnbient です。
    * ともに w は 1.0f です。
    * ディフューズではテクスチャファイル名取得の処理を実装していますが、スペキュラとアンビエントには必要ありません。

---

#### skinned_mesh コンストラクタで fetch_materials メンバ関数を呼び出す

    演習手順１－３

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::skinned_mesh(...)**

```cpp
    ・・・省略・・・

    fetch_meshes(fbx_scene, meshes);

*   fetch_materials(fbx_scene, materials);

    fbx_manager->Destroy();

    ・・・省略・・・
```

---

#### create_com_objects メンバ関数にシェーダーリソースビューオブジェクト生成のコードを追加する

    演習手順１－４

OBJ ファイル解析の時と同じようにマテリアル情報からテクスチャファイル名を取得し、テクスチャを作成しましょう。
この際、テクスチャファイル名が存在しなかった場合は、ダミーのテクスチャを作成するようにしましょう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::create_com_objects(...)**

```cpp
            mesh.indices.clear();
    #endif
        }

*       // マテリアルの中にあるテクスチャファイル名の数だけテクスチャを生成する
*       for (const material& material : materials)
*       {
*           Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
*           if (material.texture_filenames[0].size() > 0)
*           {
*               // テクスチャのパスを解決する
*               std::filesystem::path path(fbx_filename);
*               path.replace_filename(material.texture_filenames[0]);
*               D3D11_TEXTURE2D_DESC texture2d_desc;
*               // テクスチャを生成
*               load_texture_from_file(device, path.c_str(),
*                   shader_resource_view.GetAddressOf(), &texture2d_desc);
*           }
*           else
*           {
*               // ダミーテクスチャを作成
*               make_dummy_texture(device, shader_resource_view.GetAddressOf(), 0xFFFFFFFF, 16);
*           }
*           // ファイル名でシェーダーリソースビューを登録
*           shader_resource_views.insert(std::make_pair(material.texture_filenames[0], shader_resource_view));
*       }

        HRESULT hr = S_OK;
        D3D11_INPUT_ELEMENT_DESC input_element_desc[]
```

---

#### <span style="color:#334488;">2. skinned_mesh の render メンバ関数でシェーダーリソースビューオブジェクトをピクセルシェーダーにバインドする</span><a name="2_"></a>

|実装するファイル名|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::render(...)**

```cpp
    immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

*   immediate_context->PSSetShaderResources(0, 1,
*       shader_resource_views.at(materials.crbegin()->texture_filenames[0]).GetAddressOf());

    D3D11_BUFFER_DESC buffer_desc;
```

---

#### <span style="color:#334488;">3. skinned_mesh の render ピクセルシェーダー（skinned_mesh_ps.hlsl）を変更する</span><a name="3_"></a>

    演習手順３

テクスチャが描画できるようにしサンプラーステートを適用できるように **skinned_mesh_ps.hlsl** 変更しましょう。

|変更するファイル|
--|--
skinned_mesh_ps.hlsl|

```hlsl
    #include "skinned_mesh.hlsli"
*   #define POINT 0
*   #define LINEAR 1
*   #define ANISOTROPIC 2
*   SamplerState sampler_states[3] : register(s0);
*   Texture2D texture_maps[4] : register(t0);
    float4 main(VS_OUT pin) : SV_TARGET
    {
*       float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
        float3 N = normalize(pin.world_normal.xyz);
        float3 L = normalize(-light_direction.xyz);
*       float3 diffuse = color.rgb * max(0, dot(N, L));
*       return float4(diffuse, color.a) * pin.color;
    }
```

---

あとはファイル名をマテリアルのある FBX ファイルに変更して実行し、確認するのですが、

### 演習手順４以降を考えてやってみましょう。