<!--
UNIT12_EXERCISE UNIT12 演習手順
ADVANCED GEOMETRIC PRIMITIVE

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Rasterizer state
Wireframe
Cube
Cylinder
Sphere
Capsule
-->

# UNIT12_EXERCISE ADVANCED GEOMETRIC PRIMITIVE 演習手順

* 目次
    1. [ワイヤーフレーム描画](#ワイヤーフレーム描画_)
    2. 正立方体以外に円柱・球・カプセルを生成・描画する
        1. [幾何プリミティブを拡張](#幾何プリミティブを拡張_)
        2. [円柱を作成](#円柱を作成_)
        3. [球を作成](#球を作成_)
        4. [カプセルを作成](#カプセルを作成)

---

#### <span style="color:#334488;">ワイヤーフレーム描画</span><a name="ワイヤーフレーム描画_"></a>

幾何プリミティブをワイヤーフレームで描画を行います。

    演習手順１－１

ワイヤーフレームで描画するには**ラスタライザーステート**という仕組みを利用します。
そのためにまずは framework クラスに**ラスタライザステート**のメンバ変数を追加しましょう。

|実装するファイル|実装するクラス|
--|--
|**framework.h**|**framework**|

```cpp
    ・・・省略・・・
    Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[4];

*   // ラスタライザステート
*   // 0...ソリッド裏面非表示 1...ワイヤーフレーム裏面非表示 2...ソリッド両面表示 3...ワイヤーフレーム両面表示
*   Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[4];

    ・・・省略・・・
```

---

    演習手順１－２

framework の **initialize() メンバ関数** で**ラスタライザステートオブジェクト**を生成していきます。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    hr = device->CreateBlendState(&blend_desc, blend_states[3].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // ラスタライザステートを作成するための構造体
*   D3D11_RASTERIZER_DESC rasterizer_desc{};

*   // ラスタライザステート構造体共通項目
*   rasterizer_desc.FrontCounterClockwise = FALSE;  // false...時計回りを表面とする、反時計回りを裏面
*   rasterizer_desc.DepthBias = 0;
*   rasterizer_desc.DepthBiasClamp = 0;
*   rasterizer_desc.SlopeScaledDepthBias = 0;
*   rasterizer_desc.DepthClipEnable = TRUE;
*   rasterizer_desc.ScissorEnable = FALSE;
*   rasterizer_desc.MultisampleEnable = FALSE;

*   // ソリッド描画(裏面非表示)
*   rasterizer_desc.FillMode = D3D11_FILL_SOLID;
*   rasterizer_desc.CullMode = D3D11_CULL_BACK;
*   rasterizer_desc.AntialiasedLineEnable = FALSE;
*   hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[0].GetAddressOf());
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // ワイヤーフレーム描画(裏面非表示)
*   rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
*   rasterizer_desc.CullMode = D3D11_CULL_BACK;
*   rasterizer_desc.AntialiasedLineEnable = TRUE;
*   hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[1].GetAddressOf());
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // ソリッド描画(両面表示)
*   rasterizer_desc.FillMode = D3D11_FILL_SOLID;
*   rasterizer_desc.CullMode = D3D11_CULL_NONE;
*   rasterizer_desc.AntialiasedLineEnable = FALSE;
*   hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[2].GetAddressOf());
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // ワイヤーフレーム(両面表示)
*   rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
*   rasterizer_desc.CullMode = D3D11_CULL_NONE;
*   rasterizer_desc.AntialiasedLineEnable = TRUE;
*   hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[3].GetAddressOf());
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    sprite_batches[0] = std::make_unique<sprite_batch>(device.Get(), L".\\resources\\player-sprites.png", 2048);
    ・・・省略・・・
```

サンプラーステートやブレンドステートなどと同じように一部分だけを変更してラスタライザーに属する状態のオブジェクトを宣言した配列にそれぞれ生成して格納していっています。
変更しているのは 

    rasteriazer_desc.FillMode
    rasterizer_desc.CullMode
    
この部分だけですね。

---

    演習手順１－３
    演習手順１－４

現状幾何プリミティブである立方体を１つ描画していますが、これを２つ描画するように変更を加えていきます。
そのうえで**ラスタライザステート**を指定して片方はソリッド(通常)で描画し、もう片方はワイヤーフレームでの描画を行うようにしてみましょう。

まずは片方のソリッド(通常)での描画を行いましょう。

**framework::render(...)** の以下の部分を

```
DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) };

DirectX::XMFLOAT4X4 world;
DirectX::XMStoreFloat4x4(&world, S * R * T);

// 幾何プリミティブの描画
geometric_primitives[0]->render(immediate_context.Get(), world, material_color);
```

以下のように変更しましょう。

|変更するファイル|変更するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
*   {
*       // ソリッド描画を指定
*       immediate_context->RSSetState(rasterizer_states[0].Get());
        DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
        DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
*       DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x - 1.0f, translation.y, translation.z) };
        DirectX::XMFLOAT4X4 world;
        DirectX::XMStoreFloat4x4(&world, S * R * T);
        // 幾何プリミティブの描画
        geometric_primitives[0]->render(immediate_context.Get(), world, material_color);
*   }
```

２つ作るのでブロックで囲っています。２つ表示するため、位置を左に少し(-1.0f)ずらしています。

次はワイヤーフレーム描画のほうを実装していきましょう。
上のコードをコピペして修正しても構いません。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
            ・・・省略・・・
            // 幾何プリミティブの描画
            geometric_primitives[0]->render(immediate_context.Get(), world, material_color);
        }
*       {
*           immediate_context->RSSetState(rasterizer_states[1].Get());
*           DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
*           DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
*           DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x + 1.0f , translation.y, translation.z) };
*           DirectX::XMFLOAT4X4 world;
*           DirectX::XMStoreFloat4x4(&world, S * R * T);
*           geometric_primitives[0]->render(immediate_context.Get(), world, material_color);
*       }

    #ifdef USE_IMGUI
    ・・・省略・・・
```

ほぼほぼ同じです。
違いは指定している**ラスタライザステート**がワイヤーフレームになっている事と、

    immediate_context->RSSetState(rasterizer_states[1].Get());

描画位置が左に少しずれている事です。

    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x + 1.0f , translation.y, translation.z) };

---

    演習手順１－５

この状態で起動すると２つの立方体は問題なく表示されるが 2D で表示されているスプライトなどがおかしな表示になってしまっている。
これを修正しましょう。


|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework.render(...)**

```cpp
    ・・・省略・・・

    immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
    immediate_context->OMSetBlendState(blend_states[1].Get(), nullptr, 0xFFFFFFFF);

*   // 2D 用にラスタライザステートの設定を GPU 側に指定
*   immediate_context->RSSetState(rasterizer_states[0].Get());

    sprite_batches[0]->begin(immediate_context.Get(), replaced_pixel_shader.Get(), replaced_shader_resource_view.Get());
    ・・・省略・・・
```

2D のポリゴンがワイヤーフレームで描画されてしまっていたので、 2D を描画する直前に別途 2D ようにソリッドのラスタライザステートを指定して、2D を描画します。

ラスタライザステートだけではありませんが、このように１つの描画処理を行う際に、その描画に対して描画方法を指定する場合、その描画の前に GPU に対して指定するという処理を行う場合もあるので覚えておきましょう。

---

#### <span style="color:#334488;">正立方体以外に円柱・球・カプセルを生成・描画する</span>

    演習手順２

geometric_primitive クラスを拡張して円柱や球やカプセルを描画してみましょう。

#### <span style="color:#334488;">幾何プリミティブを拡張</span><a name="幾何プリミティブを拡張_"></a>

さまざまな形の幾何プリミティブを作成する為にはまずは現状の 「正立方体専用 geometric_primitive クラス」を「汎用的な geometric_primitive クラス」に変更します。
やるべき作業は

1. geometric_primitive を基底クラスをした geometric_cube (正立方体専用) クラスを新規で作る
2. geometric_primitive で正立方体として設定している頂点や面の設定を行っている部分を geometric_cube のコンストラクタに移す。
3. geometric_cube で生成して実行できるか確認する。

です。

それではやっていきましょう。
まずは正立方体専用クラスである geometric_cube クラスを geometric_primitive クラスの下に作成しましょう。

|実装するファイル|
--|--
**geometric_primitive.h**|

```cpp
    ・・・省略(上に geometric_primitive クラスの宣言)・・・

*   /// <summary>
*   /// 正立方体専用幾何プリミティブ
*   /// </summary>
*   class geometric_cube : public geometric_primitive
*   {
*   public:
*       geometric_cube(ID3D11Device* device);
*   };
```

---

次に正立方体の「頂点座標」や「面として利用している頂点インデックス」の設定を行っている以下の部分を切り取ります。
長いので一部省略しています。


```
// create a mesh for a cube
vertex vertices[24]{};
uint32_t indices[36]{};

uint32_t face{ 0 };

// top-side
// 0---------1
// |         |
// |   -Y    |
// |         |
// 2---------3
face = 0;
vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
vertices[face * 4 + 2].position = { -0.5f, +0.5f, -0.5f };

・・・省略・・・

indices[face * 6 + 3] = face * 4 + 1;
indices[face * 6 + 4] = face * 4 + 2;
indices[face * 6 + 5] = face * 4 + 3;

create_com_buffers(device, vertices, 24, indices, 36);
```

そして geometric_cube のコンストラクタを定義して貼り付けましょう。

|実装するファイル|
--|--
framework.cpp|

```cpp
    ・・・省略(上に geometric_primitive::create_com_buffers(...)の定義)・・・

*   geometric_cube::geometric_cube(ID3D11Device* device) : geometric_primitive(device)
*   {
*       // create a mesh for a cube
*       vertex vertices[24]{};
*       uint32_t indices[36]{};

*       uint32_t face{ 0 };

*       // top-side
*       // 0---------1
*       // |         |
*       // |   -Y    |
*       // |         |
*       // 2---------3
*       face = 0;
*       vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
*       vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
*       vertices[face * 4 + 2].position = { -0.5f, +0.5f, -0.5f };

        ・・・省略・・・

*       indices[face * 6 + 3] = face * 4 + 1;
*       indices[face * 6 + 4] = face * 4 + 2;
*       indices[face * 6 + 5] = face * 4 + 3;

*       create_com_buffers(device, vertices, 24, indices, 36);
*   }
```

基底クラスのコンストラクタである

    geometric_primitive(device)

を初期化子リスト部分で呼び出していることに注目してください。
ここではあくまで「頂点座標」や「面として利用している頂点インデックス」の設定を行い create_com_buffers で頂点バッファの作成を行っているだけです。入力レイアウトと頂点シェーダー、ピクセルシェーダーの作成は基底クラスである geometric_primitive 側で行っています。

---

最後に framework 側で問題なく動くかの確認を行うのですが、この段階で geometric_primitive クラス自体には「頂点座標」や「面として利用している頂点インデックス」が設定されておりません。つまり geometric_primitive クラスで new をされるとエラーになるのです。
geometric_primitive クラスはあくまでさまざまな形に派生するであろう基底のクラスとして扱っていくためこれからも頂点座標や頂点インデックスを設定することはありません。
なので　geometric_primitive クラス自体は 外部から new を出来ないように修正しておきましょう。

geometric_primitive クラスの public に宣言されている以下のコンストラクタの位置を

```
    public:
*      geometric_primitive(ID3D11Device* device);
```

protected の位置に移動させてください。

|実装するファイル|実装するクラス|
--|--
**geometric_primitive.h**|**geometric_primitive**

```cpp
        ・・・省略・・・
        void render(ID3D11DeviceContext* immediate_context,
            const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color);

    protected:
*       geometric_primitive(ID3D11Device* device);

        ・・・省略・・・
```

これで geometric_primitive クラスは外部から生成できなくなりました。

では geometric_cube クラスを new してみましょう。
framework::initialize() の以下の部分を

    geometric_primitives[0] = std::make_unique<geometric_primitive>(device.Get());

以下のように変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
geometric_primitives[0] = std::make_unique<geometric_cube>(device.Get());
```

実行すると先程と同じように正立方体が表示されたはずです。

---

#### <span style="color:#334488;">円柱を作成</span><a name="円柱を作成_"></a>

geometric_primitive を汎用的に作り変えることが出来ました。
以降は、新規でクラスを作成しコンストラクタで頂点座標や頂点インデックスを設定して create_com_buffers() を呼び出してやるだけで任意の形の幾何プリミティブが出来上がります。

試しに円柱の形を描画できる geometric_cylinder クラスを実装してみましょう。

|実装するファイル|
--|--
**geomitric_primitive.h**|

```cpp
・・・省略(上に geometric_cube クラスの宣言)・・・

/// <summary>
/// シリンダー
/// </summary>
class geometric_cylinder : public geometric_primitive
{
public:
    // 第２引数はシリンダーの円を何分割するかの数を指定
    geometric_cylinder(ID3D11Device* device, uint32_t slices);
};
```

コンストラクタ部分で頂点座標や頂点インデックスの設定を行い create_com_buffers() メンバ関数を呼び出しましょう。
初期化子リスト部分で基底クラスである geometric_primitive(device) を呼び出すのを忘れないようにしましょう。

    std::vector

を利用しているので、

#### このファイルの先頭で vector をインクルードしておいてください。

|実装するファイル||
--|--
**geometric_primitive.cpp**|

```cpp
・・・省略(上に geometric_cube::geometric_cube(...) の定義)・・・

geometric_cylinder::geometric_cylinder(ID3D11Device* device, uint32_t slices) : geometric_primitive(device)
{
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;

    // この部分は UNIT12_LEARNING の円柱の部分を確認しながら考えて実装してみよう

    create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}
```

実装できたら生成して確認してみましょう。

framework 部分の正立方体を表示している以下の部分を

    geometric_primitives[0] = std::make_unique<geometric_cube>(device.Get());

以下のように変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
geometric_primitives[0] = std::make_unique<geometric_cylinder>(device.Get(), 32);
```

これで後は geometric_cylinger のコンストラクタに円柱の頂点座標と頂点インデックスを設定すれば円柱を表示することが出来ます。

---

#### <span style="color:#334488;">球を作成</span><a name="球を作成_"></a>


球も円柱と同じように、 geometric_primitive を継承してコンストラクタをでいろいろせって出来るようにしてみましょう。
一応の仕様は以下に指定しておきます。

|||
--|--
クラス名|geometric_sphere
コンストラクタ|geometric_sphere(ID3D11Device* device, uint32_t slices, uint32_t stacks);


引数の説明
|||
--|--
slices|縦の分割数(円柱と同じ)
stacks|横の分割数



---

#### <span style="color:#334488;">カプセルを作成</span><a name="カプセルを作成_"></a>

カプセルも円柱と同じように、 geometric_primitive を継承してコンストラクタをでいろいろせって出来るようにしてみましょう。
カプセルの仕様はありません。引数の設定なども自由で構いません。



















残りの

    球
    カプセル

を正立方体や円柱にならって実装してみましょう。
クラスの作り方や作成したものを framework で生成するやり方などは同じです。

調べなければならない全然違う部分とはコンストラクタで設定している頂点座標や頂点インデックスの部分になります。
作り方自体は様々な方法がありますが、ネット上にたくさん落ちているので頑張って拾ってきて描画してみてください。
必要なものは

    頂点座標の設定
    面として利用する頂点番号の設定

の２つだけです。
円柱のコンストラクタの引数で円柱を滑らかにできたように
コンストラクタの引数を増やすことでプリミティブの形を柔軟に変更しやすくなるという点も覚えておいてください。

クラス名は好きなもので構いませんが、決められないというのであれば

|||
--|--
球|geometric_sphere
カプセル|geometric_capsule

で良いでしょう。

これは課題になるのでやってみてください。