<!--
UNIT24_EXCERCISE UNIT25 演習手順
SKINNED MESH SKELETL ANIMATION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Skeletal animation
Keyframe
Spampling rate
-->

# UNIT25_EXCERCISE SKINNED MESH SKELETAL ANIMATION 演習手順

* 目次
    1. 前回使用した FBX ファイル (cube.004.fbx) からアニメーションデータをロードする
    2. [skinned_mesh.h に animation 構造体を定義する](#2_)
    3. [skinned_mesh クラスにメンバ変数(animation_clips)を追加する](3_)
    4. [FBX シーンからアニメーションの情報を抽出する fetch_animations メンバ関数を skinned_mesh クラスに実装する](#4_)
    5. [skinned_mesh クラスのコンストラクタで fetch_animations メンバ関数を呼び出す](#5_)
    6. [skinned_mesh クラスの render メンバ関数の実装を変更する](#6_)
    7. [framework クラスの render メンバ関数の skinned_mesh オブジェクトの render メンバ関数の呼び出しを変更する](#7_)
    8. [実行し、アニメーションが動作する事を確認する](#8_)
    9. [skinned_mesh クラスのコンストラクタの引数にサンプリングレートを追加し、生成時に決定できるようにする](#9_)
    10. [framework クラスの initialize メンバ関数で dkinned_mesh コンストラクタ引数を .\\resources\\plantune.fbx に変更する](#10_)

---

#### <span style="color:#334488;">1. 前回使用した FBX ファイル (cube.004.fbx) からアニメーションデータをロードする</span>

---

#### <span style="color:#334488;">2. skinned_mesh.h に animation 構造体を定義する</span><a name="2_"></a>

    演習手順２

まずはサンプリングレートと複数のキーフレームを保管するためのアニメーション構造体を定義しましょう。

|実装するファイル|
--|--
skinned_mesh.h|

```cpp
・・省略(上に struct skeleton)・・
/// <summary>
/// アニメーション
/// </summary>
struct animation
{
    // アニメーションの名前
    std::string name;

    // サンプリングレート
    float sampling_rate{0};

    // キーフレーム
    struct keyframe
    {
        struct node
        {
            // キーフレームに含まれるノードの行列
            DirectX::XMFLOAT4X4 global_transform{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
        };
        // キーフレームに含まれる全てのノードの行列
        std::vector<node> nodes;
    };
    // アニメーション１つ分のデータ
    std::vector<keyframe> sequence;
};
・・省略(上に struct scene)・・
```

* キーフレームの説明
    記録の基点となるフレーム。

* サンプリングレート
    １秒間に実行する処理の回数
    １秒間に３０回のデータを取り出すより６０回のデータを取り出す方が精密だが、その分データ量が大きくなる。

---

#### <span style="color:#334488;">3. skinned_mesh クラスにメンバ変数(animation_clips)を追加する</span><a name="3_"></a>

    演習手順３

animation 構造体はアニメーション１つ分のデータになります。
fbx には複数のアニメーションが入っている事もあるので、複数のアニメーションを保存しておけるように animation 構造体のリスト構造の変数を宣言しておきましょう。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
        std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shader_resource_views;

*       // 全てのアニメーションのデータ
*       std::vector<animation> animation_clips;

    private:
        // 頂点シェーダー
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
```

---

#### <span style="color:#334488;">4. FBX シーンからアニメーションの情報を抽出する fetch_animations メンバ関数を skinned_mesh クラスに実装する</span><a name="4_"></a>

    演習手順４

fbx ファイルにある全てのアニメーションのデータを取り出していきましょう。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
    ・・・省略・・・
    void fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose);

*   /// <summary>
*   /// アニメーション情報の取り出し
*   /// </summary>
*   /// <param name="fbx_scene">アニメーション情報がノードの情報を追っているシーン</param>
*   /// <param name="animation_clips"></param>
*   /// <param name="sampling_rate">この値が0の場合、アニメーションデータはデフォルトのフレームレートでサンプリングされます。</param>
*   void fetch_animations(FbxScene* fbx_scene, std::vector<animation>& animation_clips, float sampling_rate);
```

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
・・・省略(skinned_mesh::fetch_skeleton(...) メンバ関数の定義)・・・

/// <summary>
/// アニメーション情報の取り出し
/// </summary>
/// <param name="fbx_scene">アニメーション情報がノードの情報を追っているシーン</param>
/// <param name="animation_clips"></param>
/// <param name="sampling_rate">この値が0の場合、アニメーションデータはデフォルトのフレームレートでサンプリングされます。</param>
void skinned_mesh::fetch_animations(FbxScene* fbx_scene, std::vector<animation>& animation_clips, float sampling_rate)
{
    FbxArray<FbxString*> animation_stack_names;
    // シーンからアニメーション一覧を取得
    fbx_scene->FillAnimStackNameArray(animation_stack_names);
    // アニメーション一覧の数を取得
    const int animation_stack_count{ animation_stack_names.GetCount() };
    for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
    {
        // アニメーションを取得
        animation& animation_clip{ animation_clips.emplace_back() };
        animation_clip.name = animation_stack_names[animation_stack_index]->Buffer();
        // 取得したアニメーションをアニメーションスタックとして設定
        FbxAnimStack* animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.name.c_str()) };
        
        // 現在のアニメーションをアニメーションスタックのアニメーションに変更
        fbx_scene->SetCurrentAnimationStack(animation_stack);

        const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() };
        FbxTime one_second;
        one_second.SetTime(0, 0, 1, 0, 0, time_mode);
        animation_clip.sampling_rate = sampling_rate > 0 ?
            sampling_rate : static_cast<float>(one_second.GetFrameRate(time_mode));

        // サンプリングレートからサンプリングの間隔時間を設定
        const FbxTime sampling_interval
            { static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate)};
        const FbxTakeInfo* take_info{fbx_scene->GetTakeInfo(animation_clip.name.c_str())};

        // アニメーションの開始と終了を取得
        const FbxTime start_time{ take_info->mLocalTimeSpan.GetStart() };
        const FbxTime stop_time{ take_info->mLocalTimeSpan.GetStop() };
        for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
        {
            // サンプリング間隔ごとのキーフレームを取得
            animation::keyframe& keyframe{ animation_clip.sequence.emplace_back() };

            const size_t node_count{scene_view.nodes.size()};
            keyframe.nodes.resize(node_count);
            for (size_t node_index = 0; node_index < node_count; ++node_index)
            {
                FbxNode* fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
                if (fbx_node)
                {
                    animation::keyframe::node& node{ keyframe.nodes.at(node_index) };

                    // アニメーション時間からアニメーション行列を取得
                    node.global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform(time));
                }
            }
        }
    }
    for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
    {
        delete animation_stack_names[animation_stack_index];
    }
}
```

---

#### <span style="color:#334488;">5. skinned_mesh クラスのコンストラクタで fetch_animations メンバ関数を呼び出す</span><a name="5_"></a>

    演習手順５

作成した fetch_animation() を skinned_mesh() コンストラクタで呼び出します。
取り出したアニメーションデータは animation_clips に保存されます。
サンプリングレートはひとまず０で固定しています。

```cpp
    ・・・省略・・・
    fetch_materials(fbx_scene, materials);

*   float sampling_rate{ 0 };
*   fetch_animations(fbx_scene, animation_clips, sampling_rate);

    fbx_manager->Destroy();
    ・・・省略・・・
```

---

#### <span style="color:#334488;">6. skinned_mesh クラスの render メンバ関数の実装を変更する</span><a name="6_"></a>

    演習手順６

読みこんだキーフレームを描画部分のボーンに影響するよう実装しましょう。

まずは描画関数である skinned_mesh::render(...) でキーフレームを扱えるよう引数にキーフレームを追加しましょう。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
        ・・・省略・・・
        void render(ID3D11DeviceContext* immediate_context,
            const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color,
*           const animation::keyframe* keyframe);
    protected:
        ・・・省略・・・
```

定義部分も修正し、追加しましょう。

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
    void skinned_mesh::render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color,
*       const animation::keyframe* keyframe)
    {
        // メッシュごとに描画処理を行う
        for (const mesh& mesh : meshes)
        {
            ・・・・・・・・
            ・・・省略・・・
            ・・・・・・・・
            XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));

*           const size_t bone_count{ mesh.bind_pose.bones.size() };
*           for (int bone_index = 0; bone_index < bone_count; ++bone_index)
*           {
*               const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index)};
*               const animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
*               XMStoreFloat4x4(&data.bone_transforms[bone_index],
*                   XMLoadFloat4x4(&bone.offset_transform) *
*                   XMLoadFloat4x4(&bone_node.global_transform) *
*                   XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
*               );
*           }

            for (const mesh::subset& subset : mesh.subsets)
            {
                ・・・省略・・・
```

---

#### <span style="color:#334488;">7. framework クラスの render メンバ関数の skinned_mesh オブジェクトの render メンバ関数の呼び出しを変更する</span><a name="7_"></a>

    演習手順７

演習手順の６で skinned_mesh::render(...) に keyframe の引数を追加したので呼び出しの実引数部分も追加をを行います。

現在の時間とサンプリングレートから現在のアニメーションフレームのインデックスを計算し、計算されたアニメーションのキーフレームを取得。取得したキーフレームを実引数として設定します。

ではやってみましょう。

framework::render(...) の以下の部分を

```cpp
skinned_meshes[0]->render(immediate_context.Get(), world, material_color);
```

以下のように追加、変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework::**|****

```cpp
        ・・・省略・・・
        DirectX::XMStoreFloat4x4(&world, C * S * R * T);

*       int clip_index{ 0 };
*       int frame_index{ 0 };
*       static float animation_tick{ 0 };

*       animation& animation{ skinned_meshes[0]->animation_clips.at(clip_index) };
*       frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
*       if (frame_index > animation.sequence.size() - 1)
*       {
*           frame_index = 0;
*           animation_tick = 0;
*       }
*       else
*       {
*           animation_tick += elapsed_time;
*       }
*       animation::keyframe& keyframe{ animation.sequence.at(frame_index) };
*       skinned_meshes[0]->render(immediate_context.Get(), world, material_color, &keyframe);

    #ifdef USE_IMGUI
        ・・・省略・・・

```

---

#### <span style="color:#334488;">8. 実行し、アニメーションが動作する事を確認する</span><a name="8_"></a>

実行すると以下のような画像が

@import "images/0040_1.png"

下のような画像にアニメーションするのが確認できます。

@import "images/0040_2.png"

---

#### <span style="color:#334488;">9. skinned_mesh クラスのコンストラクタの引数にサンプリングレートを追加し、生成時に決定できるようにする</span><a name="9_"></a>

    演習手順９

演習手順にあるように、まずは skinned_mesh のコンストラクタに引数を追加しましょう。

コンストラクタの以下の部分を

```cpp
/// <summary>
/// constructor
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="fbx_filename">fbx ファイル名</param>
/// <param name="triangulate">true...多角形で作られたポリゴンを三角形化	false...三角形化しない</param>
skinned_mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate = false);
```

以下のように変更しよう。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinnd_mesh**

```cpp
    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="fbx_filename">fbx ファイル名</param>
    /// <param name="triangulate">true...多角形で作られたポリゴンを三角形化	false...三角形化しない</param>
*   /// <param naem="sampling_rate">サンプリングレート</param>
*   skinned_mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate = false, float sampling_rate = 0);
```

定義部分も以下のように変更しよう。

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
    ・・・省略・・・
    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="fbx_filename">fbx ファイル名</param>
    /// <param name="triangulate">true...多角形で作られたポリゴンを三角形化	false...三角形化しない</param>
*   /// <param naem="sampling_rate">サンプリングレート</param>
*   skinned_mesh::skinned_mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate, float sampling_rate)
    {
        ・・・省略・・・
```

引数に設定した smpling_rate が反映されるように修正しよう。

コンストラクタの以下の部分をコメントアウトするかマクロで無効化して下さい。

```cpp
float sampling_rate{ 0 };
```

|実装するファイル|実装するメンバ関数|
--|--
**skinned_mesh.cpp**|**skinned_mesh::skinned_mesh**

```cpp
*   //float sampling_rate{ 0 };
```

---

#### <span style="color:#334488;">10. framework クラスの initialize メンバ関数で dkinned_mesh コンストラクタ引数を .\\resources\\plantune.fbx に変更する</span><a name="10_"></a>

    演習手順１０

読み込むモデルを変更して読みこんで見ましょう。

読み込む際に※に記載されている通り、モデルは「右手系 Y 軸アップ」で「センチメートル単位」で「三角形化済み」とあるので

それぞれ設定も修正していこう。

framework::initialize() の以下の部分を

```cpp
skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.004.fbx", true);
```

以下のように変更しよう。

フラグは「三角形化済み」なので無くしておく。

|実装するファイル|実装するメンバ関数|
--|--
framework.cpp|framework::initialize()

```cpp
*   //skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.004.fbx", true);
*   skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\plantune.fbx");
```

---

次に framework::render(...) の以下の部分を

```cpp
const float scale_factor = 1.0f;
```

以下のように変更しよう。
スケールファクタを「センチメートル単位」に合わせます。

|実装するファイル|実装するメンバ関数|
--|--
framework.cpp|framework::render(...)

```cpp
*   const float scale_factor = 0.01f;
```

---

最後にモデルの行列を「右手系 Y 軸アップ」に変更しましょう。

framework::render(...) の以下の部分を

```cpp
DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[2])
    * DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) };
```

以下のように変更します。
「右手系 Y 軸アップ」の行列は配列の 0 番になります。

|実装するファイル|実装するメンバ関数|
--|--
framework.cpp|framework::render(...)

```cpp
*   DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[0])
        * DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) };
```

変更できたら起動してみましょう。

以下のモデルがアニメーションされているのが確認されます。

後面
@import "images/plantune_1.png"

前面
@import "images/plantune_2.png"


---

再度 cube.004.fbx を描画する場合は、上記で行った３つの変更点を戻してから実行して下さい。

