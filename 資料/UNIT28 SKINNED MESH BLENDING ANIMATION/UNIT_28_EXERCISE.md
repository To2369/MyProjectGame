<!--
UNIT27_LEARNING UNIT28 学習要項
SKINNED MESH BLENDING ANIMATION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Blending animation
Appending animations
-->

# UNIT28_EXCERCISE SKINNED MESH BLENDING ANIMATION 演習手順

* 目次
    1. [skinned_mesh オブジェクトをアニメーション情報を持ったない.\\resources\\AimTest\\MNK_Mesh.fbx に変更する](#1_)
    2. [skinned_mesh クラスに append_animations メンバ関数を追加する](#2_)
    3. [skinned_mesh オブジェクト生成後 append_animations メンバ関数を呼び出す](#3_)
    4. [実行し、アニメーションが再生されていることを確認する](#4_)
    5. [skinned_mesh クラスに blend_animations メンバ関数を追加する](#5_)
    6. [framework クラスの render メンバ関数を変更する](#6_)
    7. [実行し、正面を狙っているポーズであることを確認する](#7_)
    8. [７. の 24:行目の定数値を変数に変更し、ImGUI から実行時に変更できるようにする](#8_)

---

#### <span style="color:#334488;">1. skinned_mesh オブジェクトをアニメーション情報を持ったない.\\resources\\AimTest\\MNK_Mesh.fbx に変更する</span><a name="1_"></a>

※ MNK_Mesh.fbx は右手系・Z 軸アップ・センチメートル単位・三角形化済み

考えて変更してみてください。

MNK_Mesh.fbx はアニメーション情報をまったく持っていないのでファイル名を変更して値を調整し実行してもクラッシュします。

---

#### <span style="color:#334488;">2. skinned_mesh クラスに append_animations メンバ関数を追加する</span><a name="2_"></a>

    演習手順２

アニメーション追加用のメンバ関数を実装します。

まずは宣言。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
        void update_animation(animation::keyframe& keyframe);

*       /// <summary>
*       /// アニメーションの追加
*       /// </summary>
*       /// <param name="animation_filename">アニメーションファイル名</param>
*       /// <param name="sampling_rate">サンプリングレート</param>
*       /// <returns></returns>
*       bool append_animations(const char* animation_filename, float sampling_rate);

    protected:
```

つぎに定義を実装。

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
・・・省略(上に skinned_mesh::append_animations(...) メンバ関数の定義 )・・・

/// <summary>
/// アニメーションの追加
/// </summary>
/// <param name="animation_filename">アニメーションファイル名</param>
/// <param name="sampling_rate">サンプリングレート</param>
/// <returns></returns>
bool skinned_mesh::append_animations(const char* animation_filename, float sampling_rate)
{
    FbxManager* fbx_manager{ FbxManager::Create() };
    FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };

    FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
    bool import_status{ false };
    import_status = fbx_importer->Initialize(animation_filename);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
    import_status = fbx_importer->Import(fbx_scene);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    // 0...デフォルト値を使用します。0 未満...フェッチしません
    fetch_animations(fbx_scene, animation_clips, sampling_rate);

    fbx_manager->Destroy();

    return true;
}
```

---

#### <span style="color:#334488;">3. skinned_mesh オブジェクト生成後 append_animations メンバ関数を呼び出す</span><a name="3_"></a>

※Aim_Space.fbx はアニメーションデータだけを持ったファイルである

    演習手順３

演習手順１で読みこんだ fbx に対してアニメーションの情報だけを持っている fbx の情報をつなぎ合わせましょう。

|||
--|--
**framework.cpp**|**framework::initilize()**

```cpp
    skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\AimTest\\MNK_Mesh.fbx");
*   skinned_meshes[0]->append_animations(".\\resources\\AimTest\\Aim_Space.fbx", 0);
```


---

#### <span style="color:#334488;">4. 実行し、アニメーションが再生されていることを確認する</span><a name="4_"></a>

※10 フレーム間隔で正面・上・下・左・左上・左下・右・右上・右下の９方向を狙ているポーズを持っている

    演習手順４

実行すると以下のようなアニメーションが順に描画される事が確認されるはずです。

@import "images/ブレンドアニメーション01.png"

---

#### <span style="color:#334488;">5. skinned_mesh クラスに blend_animations メンバ関数を追加する</span><a name="5_"></a>

登録されているアニメーションにある連続していないアニメーションをつなげて利用できるようにします。

演習手順４のままだと１番目の情報と２番目の情報はアニメーションが繋がっている情報をもっているのでスムーズに表示することが可能ですが、１番目のアニメーションから４番目のアニメーションを単純に表示させようとした場合、不具合を生じます。

これらのアニメーションを連結するための処理を実装していきましょう。

まずは宣言。

|実装するファイル|実装するクラス|
--|--
**skinned_mesh.h**|**skinned_mesh**

```cpp
    bool append_animations(const char* animation_filename, float sampling_rate);

*   /// <summary>
*   /// アニメーションの連結
*   /// </summary>
*   /// <param name="keyframes">連結するアニメーションのキー</param>
*   /// <param name="factor"> 0.0 ～ 1.0 ( 0 番のアニメーションの状態 ～ 1 番のアニメーションの状態 ) </param>
*   /// <param name="keyframe">キーフレーム</param>
*   void skinned_mesh::blend_animations(const animation::keyframe* keyframes[2], float factor, animation::keyframe& keyframe);
```

次に定義。

|実装するファイル|
--|--
**skinned_mesh.cpp**|

```cpp
・・・省略(上に akinned_mesh::append_animations(...)メンバ関数の定義)・・・

/// <summary>
/// アニメーションの連結
/// </summary>
/// <param name="keyframes">連結するアニメーションのキー</param>
/// <param name="factor"> 0.0 ～ 1.0 ( 0 番のアニメーションの状態 ～ 1 番のアニメーションの状態 ) </param>
/// <param name="keyframe">キーフレーム</param>
void skinned_mesh::blend_animations(const animation::keyframe* keyframes[2], float factor, animation::keyframe& keyframe) 
{
    size_t node_count{ keyframes[0]->nodes.size() };
    keyframe.nodes.resize(node_count);
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        XMVECTOR S[2]{
            XMLoadFloat3(&keyframes[0]->nodes.at(node_index).scaling),
            XMLoadFloat3(&keyframes[1]->nodes.at(node_index).scaling)
        };
        XMStoreFloat3(&keyframe.nodes.at(node_index).scaling, XMVectorLerp(S[0], S[1], factor));

        XMVECTOR R[2]{
            XMLoadFloat4(&keyframes[0]->nodes.at(node_index).rotation),
            XMLoadFloat4(&keyframes[1]->nodes.at(node_index).rotation)
        };
        XMStoreFloat4(&keyframe.nodes.at(node_index).rotation, XMQuaternionSlerp(R[0], R[1], factor));

        XMVECTOR T[2] {
            XMLoadFloat3(&keyframes[0]->nodes.at(node_index).translation),
            XMLoadFloat3(&keyframes[1]->nodes.at(node_index).translation)
        };
        XMStoreFloat3(&keyframe.nodes.at(node_index).translation, XMVectorLerp(T[0], T[1], factor));
    }
}
```

---

#### <span style="color:#334488;">6. framework クラスの render メンバ関数を変更する</span><a name="6_"></a>

※10 フレーム毎に正面(0)・上(10)・下(20)・左(30)・左上(40)・左下(50)・右(60)・右上(70)・右下(80)の９のポーズがある
※フレーム番号:40 は左上、フレーム番号:80 は右下のポーズである

    演習手順６

※部分はつまり、１０フレーム毎に別々のアニメーションが設定されており、今回は５番目(４０フレーム目)と９番目(８０フレーム目)のアニメーションを blend_animation(...) を使って合成しようという事です。

合成するだけで合成したアニメーションを確認できるのは演習手順８になります。

それではやっていきましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
        DirectX::XMStoreFloat4x4(&world, C * S * R * T);

*   #if 0
        int clip_index{ 0 };
        int frame_index{ 0 };
        static float animation_tick{ 0 };

        animation& animation{ skinned_meshes[0]->animation_clips.at(clip_index) };
        frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
        if (frame_index > animation.sequence.size() - 1)
        {
            frame_index = 0;
            animation_tick = 0;
        }
        else
        {
            animation_tick += elapsed_time;
        }
        animation::keyframe& keyframe{ animation.sequence.at(frame_index) };
*   #else
*       animation::keyframe keyframe;
*       // ５番目と９番目のアニメーションを設定
*       const animation::keyframe* keyframes[2]{
*           &skinned_meshes[0]->animation_clips.at(0).sequence.at(40),
*           &skinned_meshes[0]->animation_clips.at(0).sequence.at(80)
*       };
*       // アニメーションを合成
*       skinned_meshes[0]->blend_animations(keyframes, 0.5f, keyframe);
*       // 合成したアニメーションでメッシュを更新
*       skinned_meshes[0]->update_animation(keyframe);
*   #endif

        ・・・省略・・・

        skinned_meshes[0]->render(immediate_context.Get(), world, material_color, &keyframe);
```

---

#### <span style="color:#334488;">7. 実行し、正面を狙っているポーズであることを確認する</span><a name="7_"></a>

    演習手順７

５番目のアニメーションは左上を狙っているポーズ。

@import "images/５番目01.png"

９番目のアニメーションは右下を狙っているポーズ

@import "images/９番目01.png"

これをブレンドして factor が 0.5f であるアニメーション、つまり５番目と９番目の間のアニメーションを表示します。

実行すると以下のような画像が描画されるはずです。

@import "images/５番目９番目01.png"



---

#### <span style="color:#334488;">8. ７. の 24:行目の定数値を変数に変更し、ImGUI から実行時に変更できるようにする</span><a name="8_"></a>

    演習手順８

factor である 0.5f の部分を ImGUI で変更できるようにしましょう。

仕様は以下のようにします。

メンバ変数は既に factors[8] が 宣言されているので、その配列を利用して下さい。

|名称|メンバ変数名|最低値|最大値|使用する<br>ImGUI<br>関数
--|--|--|--|--
**位置**|**factors[2]**|-1.0f|1.0f|SliderFloat

実行して factors[2] のスライダーを 0.0f から 1.0f まで移動させると、５番目のアニメーションから９番目のアニメーションにスムーズに遷移するはずです。