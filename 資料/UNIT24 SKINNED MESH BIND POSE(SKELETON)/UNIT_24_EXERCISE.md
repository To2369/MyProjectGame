<!--
UNIT24_EXCERCISE UNIT24 演習手順
SKINNED MESH BIND POSE(SKELETON)

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Skeleton
Bone
Bind pose
Offset transform
-->

# UNIT24_EXCERCISE SKINNED MESH BIND POSE(SKELETON) 演習手順

* 目次
    1. 前回使用した FBX ファイル (cube.004.fbx) をロードしオフセット行列とダミーのポーズ行列をセットしテストする
    2. [skinned_mesh.h に skeleton 構造体を定義する](#2_)
    3. [skinned_mesh::mesh 構造体にメンバ変数(bind_pose)を追加する](#3_)
    4. [FBX メッシュからバインドポーズの情報を抽出する fetch_skeleton メンバ関数を skinned_mesh をクラスに実装する](#4_)
    5. [skinned_mesh クラスの fetch_meshs メンバ関数で fetch_skeleton メンバ関数を呼び出す](#5_)
    6. [skinned_mesh クラスの render メンバ関数でダミー行列を定数バッファ (data.bone_transforms) にセットする](#6_)
    7. [実行し、メッシュの変形を確認する](#7_)

---

#### <span style="color:#334488;">1. 前回使用した FBX ファイル (cube.004.fbx) をロードしオフセット行列とダミーのポーズ行列をセットしテストする</span>

---

#### <span style="color:#334488;">2. skinned_mesh.h に skeleton 構造体を定義する</span><a name="2_"></a>

    演習手順２

メッシュが持っているボーンが多くなってくるとメッシュ内だけでは管理できなくなるので複数のボーンの連結であるスケルトンを構造体として、まずは複数のボーンを管理する skeleton 構造体定義しましょう。

|実装するファイル|
--|--
**skinned_mesh.h**|

```cpp
・・・省略(上にファイルのインクルード)・・・

/// <summary>
/// スケルトン
/// 複数のボーンを管理
/// </summary>
struct skeleton
{
    /// <summary>
    /// メッシュのボーン情報
    /// </summary>
    struct bone
    {
        uint64_t unique_id{ 0 };
        std::string name;
        // 親ボーンの位置を参照するインデックス	-1...親無し
        int64_t parent_index{ -1 };
        // シーンのノード配列を参照するインデックス
        int64_t node_index{ 0 };

        // モデル(メッシュ)空間からボーン(ノード)に変換するために使用
        DirectX::XMFLOAT4X4 offset_transform{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

        // true...親無しのボーン
        bool is_orphan() const { return parent_index < 0; };
    };
    std::vector<bone> bones;

    int64_t indexof(uint64_t unique_id) const
    {
        int64_t index{ 0 };
        for (const bone& bone : bones)
        {
            if (bone.unique_id == unique_id)
            {
                return index;
            }
            ++index;
        }
        return -1;
    }
};

・・・省略(下に struct scene 定義)・・・
```

---

#### <span style="color:#334488;">3. skinned_mesh.h に skeleton 構造体を定義する</span><a name="3_"></a>

    演習手順３

上記で定義した skeleton 構造体を mesh 構造体に宣言しましょう。

|実装するファイル|実装する構造体|
--|--
**skinned_mesh.h**|**skinned_mesh::mesh**

```cpp
        ・・・省略・・・
        DirectX::XMFLOAT4X4 default_global_transform{ 1, 0, 0, 0, ...

*       // バインドポーズ(初期姿勢)
*       skeleton bind_pose;

    private:
        ・・・省略・・・
```

---

#### <span style="color:#334488;">4. FBX メッシュからバインドポーズの情報を抽出する fetch_skeleton メンバ関数を skinned_mesh をクラスに実装する</span><a name="4_"></a>

    演習手順４

演習手順にある通り FBX メッシュからバインドポーズ(初期姿勢)の情報を抽出するメンバ関数を実装していきます。

まずはメンバ関数の宣言

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
        ・・・省略・・・
        void fetch_materials(FbxScene* fbx_scene, std::set<material>& materials);

*       /// <summary>
*       /// バインドポーズ(初期姿勢)の情報の取り出し
*       /// </summary>
*       /// <param name="fbx_mesh">バインドポーズの元データがあるノードの情報を持っているメッシュ</param>
*       /// <param name="bind_pose">取り出し後にバインドポーズ(初期姿勢)の情報を設定する為の出力用スケルトン</param>
*       void fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose);
    };
```

宣言したら定義を実装していきましょう。

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
/// <summary>
/// バインドポーズ(初期姿勢)の情報の取り出し
/// </summary>
/// <param name="fbx_mesh">バインドポーズの元データがあるノードの情報を持っているメッシュ</param>
/// <param name="bind_pose">取り出し後にバインドポーズ(初期姿勢)の情報を設定する為の出力用スケルトン</param>
void skinned_mesh::fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose)
{
    // メッシュにあるスキンの数を取得
    const int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
    // メッシュにあるすべてのスキンの情報をチェック
    for (int deformer_index = 0; deformer_index < deformer_count; ++deformer_index)
    {
        // 現在のスキンを取得
        FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));
        // スキンにあるクラスターの数を取得
        const int cluster_count = skin->GetClusterCount();
        // メッシュのボーンの数 = メッシュのクラスターの数
        bind_pose.bones.resize(cluster_count);
        // スキンにあるすべてのクラスターの情報をチェック
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
        {
            // 現在のクラスターの情報をチェック
            FbxCluster* cluster = skin->GetCluster(cluster_index);

            skeleton::bone& bone{ bind_pose.bones.at(cluster_index) };
            bone.name = cluster->GetLink()->GetName();
            bone.unique_id = cluster->GetLink()->GetUniqueID();
            bone.parent_index = bind_pose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
            bone.node_index = scene_view.indexof(bone.unique_id);

            // リンクを含むノードに関連付けられた行列を取得
            // モデル(メッシュ)のローカル空間からシーンのグローバル空間に変換するために使用
            FbxAMatrix reference_global_init_position;
            cluster->GetTransformMatrix(reference_global_init_position);

            // リンクノードに関連付けられた行列を取得
            // ボーンのローカル空間からシーンのグローバル空間に変換するために使用
            // ローカル座標空間でのボーンの絶対位置
            FbxAMatrix cluster_global_init_position;
            cluster->GetTransformLinkMatrix(cluster_global_init_position);

            bone.offset_transform = 
                to_xmfloat4x4(cluster_global_init_position.Inverse() * reference_global_init_position);
        }
    }
}
```

---

#### <span style="color:#334488;">5. skinned_mesh クラスの fetch_meshs メンバ関数で fetch_skeleton メンバ関数を呼び出す</span><a name="5_"></a>

    演習手順５

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::fetch_meshes(...)**

```cpp
    fetch_bone_influences(fbx_mesh, bone_influences);

*   // メッシュからバインドポーズ(初期姿勢)の情報の取り出し
*   fetch_skeleton(fbx_mesh, mesh.bind_pose);

    std::vector<mesh::subset>& subsets{ mesh.subsets };
```

---

---

#### <span style="color:#334488;">6. skinned_mesh クラスの render メンバ関数でダミー行列を定数バッファ (data.bone_transforms) にセットする</span><a name="6_"></a>

    演習手順６


ボーン行列にダミーのバインドポーズを設定して確認してみましょう。

    ※動作確認後#if-#endifディレクティブのコードは無効にすること

とあるので、演習手順６での実行後は #if 0 にしてマクロを切っておきましょう。

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::render(...)**

```cpp
            ・・・省略・・・
            XMStoreFloat4x4(&data.bone_transforms[2], XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(-45)));
    #endif

*   #if 1
*           XMMATRIX B[3]; // バインドポーズ変換(オフセット行列)：モデル(メッシュ)空間からボーン空間に変換
*           B[0] = XMLoadFloat4x4(&mesh.bind_pose.bones.at(0).offset_transform);
*           B[1] = XMLoadFloat4x4(&mesh.bind_pose.bones.at(1).offset_transform);
*           B[2] = XMLoadFloat4x4(&mesh.bind_pose.bones.at(2).offset_transform);

*           XMMATRIX A[3]; // アニメーションボーン変換：ボーン空間からモデル(メッシュ)または親ボーン空間に変換
*           A[0] = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90), 0, 0);	                                // A0 空間からモデル空間へ
*           A[1] = XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(+45)) * XMMatrixTranslation(0, 2, 0);	// A1スペースから親ボーン(A0)スペースへ
*           A[2] = XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(-45)) * XMMatrixTranslation(0, 2, 0);	// A2スペースから親ボーン(A1)スペースへ

*           XMStoreFloat4x4(&data.bone_transforms[0], B[0] * A[0]);
*           XMStoreFloat4x4(&data.bone_transforms[1], B[1] * A[1] * A[0]);
*           XMStoreFloat4x4(&data.bone_transforms[2], B[1] * A[2] * A[1] * A [0]);
*   #endif

            for (const mesh::subset& subset : mesh.subsets)
            {
            ・・・省略・・・
```

---

#### <span style="color:#334488;">7. 実行し、メッシュの変形を確認する</span><a name="7_"></a>

    演習手順７

実行し確認すると以下の画像が表示されます。

@import "images/0040_1.png"