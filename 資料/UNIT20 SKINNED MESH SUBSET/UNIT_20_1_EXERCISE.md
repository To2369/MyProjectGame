<!--
UNIT20_EXCERCISE UNIT20 演習手順
SKINNED MESH SUBSET

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Multimaterials
Subsets
-->

# UNIT20_EXCERCISE SKINNED MESH SUBSET 演習手順

* 目次
    * 1. 複数のマテリアル情報をもつメッシュをロードし描画する
    * [2. マテリアル単位で描画を行うためにの subset 構造体を skinned_mesh::mesh クラスに定義する](#2)
    * [3. skinned_mesh クラスの fetch_meshes メンバ関数にコードを追加・変更する](#3)
    * [4. skinned_mesh クラスの render メンバ関数をサブセット単位で描画するように変更する](#4)

---

#### <span style="color:#334488;">2. マテリアル単位で描画を行うためにの subset 構造体を skinned_mesh::mesh クラスに定義する</span><a name="2_"></a>

OBJ ファイルの時と同じように複数のマテリアルを作る為に**サブセット**という考え方を利用します。

今回**サブセット化**する項目は以下の４つになります。

    マテリアル識別 ID
    マテリアル名
    頂点インデックス開始番号
    利用する頂点インデックスの数

FBX ノードに設定されているマテリアルの識別 ID になります。
マテリアル名は描画する際に描画する頂点とこの設定を反映するマテリアルを紐づける為のものです。

**サブセット**としてまとめる為に必要な情報は OBJ ファイルとほぼ同じで

    FbxNode に含まれている自身のマテリアル識別 ID
    描画に必要なテクスチャ名の情報を持っているマテリアル名
    上のテクスチャに紐づいているテクスチャ座標を含んでいる頂点インデックスの開始番号
    上のテクスチャに紐づいているテクスチャ座標を含んでいる頂点インデックスの開始番号からの数

という事になります。

この４つを**サブセット化**しリスト化しておき、最終的に **skinned_mesh::render(...)** 部分で

    const material& material(materials.find(描画するマテリアルの識別 ID));
    ・・・省略・・・
    DrawIndex(開始番号からの数, 開始番号, 0);

としてやることで、FBX の複数のテクスチャであっても描画部分をシンプルに行うことが可能になります。

---

まずはサブセット用の構造体を定義しましょう。
OBJ ファイルでは **static_mesh** クラスがメッシュ１つ分であり、それに対しての表示すべきマテリアルごとのサブセットを用意しましたが、FBX ファイルでは複数のメッシュが **skinned_mesh** クラスに情報として入っており、それらの複数のメッシュは **skinned_mesh::mesh** 構造体に定義してあります。ですので今回サブセットクラスをこの **skinned_mesh::mesh** 構造体内に定義します。

ですが、マテリアル自体は **skinned_mesh** クラスで利用される全マテリアルの情報として **material** 構造体に定義されているので **skinned_mesh::mesh** クラスごとに定義するような実装はしていません。

それではサブセット構造体を実装しましょう。

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**skinned_mesh::mesh**

```cpp
        std::vector<uint32_t> indices;          // 頂点インデックス

*       /// <summary>
*       /// サブセット情報
*       /// </summary>
*       struct subset
*       {
*           uint64_t material_unique_id{ 0 };   // 識別 ID
*           std::string material_name;          // マテリアル名
*           uint32_t start_index_location{ 0 }; // インデックスの開始位置
*           uint32_t index_count{ 0 };          // インデックスの数(頂点数)
*       };

    private:
```

---

#### <span style="color:#334488;">3. skinned_mesh クラスの fetch_meshes メンバ関数にコードを追加・変更する</span><a name="3_"></a>

ノード内にあるマテリアルの情報を抜き取っていきましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::fetch_meshes(...)**

```cpp
    ・・・省略・・・
    mesh.node_index = scene_view.indexof(mesh.unique_id);

*   std::vector<mesh::subset>& subsets{ mesh.subsets };
*   // マテリアル数を取得
*   const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
*   // サブセットの数をリサイズ
*   subsets.resize(material_count > 0 ? material_count : 1);
*   for (int material_index = 0; material_index < material_count; ++material_index)
*   {
*       // マテリアル情報を取得
*       const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
*       subsets.at(material_index).material_name = fbx_material->GetName();
*       subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
*   }

*   if (material_count > 0)
*   {
*       const int polygon_count{ fbx_mesh->GetPolygonCount() };
*       for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
*       {
*           // メッシュのポリゴンの番号からインデックス番号を取得しマテリアルのインデックスの番号として割り当てる
*           const int material_index{fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index)};
*           // サブセットに対して割り当てられたマテリアルのインデックスの番号にインデックスの数を頂点数分(3)増やしていく
*           subsets.at(material_index).index_count += 3;
*       }

*       // インデックスの数からそれぞれの開始の数を計算して設定していく
*       uint32_t offset{ 0 };
*       for (mesh::subset& subset : subsets)
*       {
*           subset.start_index_location = offset;
*           offset += subset.index_count;
*           subset.index_count = 0;
*       }
*   }

    const int polygon_count{ fbx_mesh->GetPolygonCount() };	// ポリゴン数
    ・・・・・・・・
    ・・・省略・・・
    ・・・・・・・・
    // ポリゴンの数だけ頂点データを取得
    for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
    {
 *      const int material_index{ material_count > 0 ?
 *          fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
 *      mesh::subset& subset{ subsets.at(material_index) };
 *      const uint32_t offset{ subset.start_index_location + subset.index_count };

        // 三角形の数分の頂点の情報を取得する
        for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
        {
            ・・・・・・・・
            ・・・省略・・・
            ・・・・・・・・
            // 現在のインデックス番号部分に頂点データを設定
            mesh.vertices.at(vertex_index) = std::move(vertex);
            // 現在のインデックス番号を設定
*           //mesh.indices.at(vertex_index) = vertex_index;
*           mesh.indices.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;
*           subset.index_count++;
        }
        ・・・省略・・・
```

---

#### <span style="color:#334488;">4. skinned_mesh クラスの render メンバ関数をサブセット単位で描画するように変更する</span><a name="4_"></a>

メッシュ内にあるサブセットの情報の中の設定したマテリアルの識別 ID をもつマテリアルをシェーダー側に設定して描画を行うように変更を行っていきましょう。

**skinned_mesh::draw(...)** の以下の部分を

```cpp
    constants data;
    data.world = world;
*   XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&materials.crbegin()->Kd));
    immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
    immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

    immediate_context->PSSetShaderResources(0, 1,
*       shader_resource_views.at(materials.crbegin()->texture_filenames[0]).GetAddressOf());

*   D3D11_BUFFER_DESC buffer_desc;
*   mesh.index_buffer->GetDesc(&buffer_desc);
*   immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
```

を以下のように変更しましょう。
コメントアウトする部分やコードを追加する部分もあるので注意して下さい。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::render(...)**

```cpp
    constants data;
    data.world = world;

*   for (const mesh::subset& subset : mesh.subsets)
*   {
*       // マテリアルの識別ID からマテリアルを取得し参照として設定
*       const material& material{ materials.find({subset.material_unique_id}).operator*() };

*       XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));
        immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
        immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

        immediate_context->PSSetShaderResources(0, 1,
*           shader_resource_views.at(material.texture_filenames[0]).GetAddressOf());

*       //D3D11_BUFFER_DESC buffer_desc;
*       //mesh.index_buffer->GetDesc(&buffer_desc);
*       immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
*   }
```

---

あとはファイル名をマルチマテリアルのある FBX ファイルに変更して実行し、確認するのですが、

### 演習手順５以降を考えてやってみましょう。