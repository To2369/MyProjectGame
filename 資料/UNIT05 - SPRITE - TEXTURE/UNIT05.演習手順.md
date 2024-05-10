<!--
UNIT05_EXERCISE UNIT05 演習手順
SPRITE - TEXTURE
-->

# UNIT05 SPRITE - TEXTURE 演習手順

---

#### <span style="color:#334488;">DirectXTK を利用出来るようにする</span>

    演習手順１

手順通り **DirectXTK** をインクルードできるように設定していきましょう。

画像ファイルを読み込みテクスチャである **ID3D11Texture2D** を生成するために必要な便利な関数を持っているファイルを扱えるようにしています。

---

#### <span style="color:#334488;">画像ファイルを読み込むための準備</span>

    演習手順２

画像ファイルを読み込んでテクスチャを生成するのですが、テクスチャをシェーダー側で直接使用することはできないため、シェーダーリソースビューというものを作成して、テクスチャをラップしシェーダーで使用するという事をします。

なので用意する変数は２つ。

|ファイル|クラス
--|--
**sprite.h**|**sprite**\

```cpp
    // シェーダーリソースビュー
    ID3D11ShaderResourceView* shader_resource_view;
    // テクスチャ
    D3D11_TEXTURE2D_DESC texture2d_desc;
```

---

#### <span style="color:#334488;">テクスチャ座標の追加</span>

    演習手順３

頂点フォーマットにテクスチャ座標を追加しましょう。
演習手順の２でテクスチャを扱えるようにした上で、テクスチャのどの位置がポリゴン頂点座標のどの位置に対応しているのかを指定するための座標です。

|ファイル|構造体名
--|--
**sprite.h**|**sprite::vertex**

```cpp
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
*   DirectX::XMFLOAT2 texcoord;
```

---

#### <span style="color:#334488;">外部から読み込む画像ファイルを変更できるようにする</span>

    演習手順４

sprite ごとに画像を変更できるようコンストラクタでファイル名を受け取れるようにしましょう。

sprite クラスの下記のコンストラクタを

```cpp
sprite(ID3D11Device* device);
```

下記のように変更しましょう。

|ファイル名|クラス名|
--|--
**sprite.h**|**sprite**


```cpp
sprite(ID3D11Device* device, const wchar_t* filename);
```

これに伴い実装側も随時変更していきましょう。下記のコンストラクタの定義部分の

```cpp
sprite::sprite(ID3D11Device* device) {
```

|ファイル名|
--|--
**sprite.cpp**|

```cpp
sprite::sprite(ID3D11Device* device, const wchar_t* filename) {
```

---

    演習手順５－１

頂点フォーマットに **XMFLOAT2** のテクスチャ座標追加されたので cpp 側で設定している頂点データにもテクスチャ座標を追加しましょう。

|ファイル名|メンバ関数名
--|--
**sprite.cpp**|**sprite::sprite(ID3D11Device\* device, const wchar_t\* filename)**

```cpp
    HRESULT hr{ S_OK };

    vertex vertices[]
    {
*       { { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {0,0} },
*       { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {1,0} },
*       { { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {0,1} },
*       { { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {1,1} },
    };

    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(vertices);
    ・・・省略・・・
```

テクスチャ座標は画像ファイル上での４隅を座標で表したもので、最大が 1.0 となっていて

    左上    (0, 0)
    右上    (1, 0)
    左下    (0, 1)
    右下    (1, 1)

@import "images/テクスチャ座標.png"

これを NDC 座標に準拠しているポリゴンの頂点座標とテクスチャの頂点座標の４隅が合わさるように設定しています。

@import "images/NDC2.png"

上記の頂点データの設定だと読み込んだ画像が画面いっぱいに表示されることになる。

この際ポリゴンとテクスチャの比率が違っていたらどうなるのか？
結果としてはテクスチャが伸縮されます。

@import "images/アスペクトずれ.png"

---

    演習５－２

頂点データが変化した事を GPU 側に伝えるために入力レイアウトもテクスチャ座標の追加を行いましょう。

|ファイル名|メンバ関数名
--|--
**sprite.cpp**|**sprite::sprite(ID3D11Device\* device, const wchar_t\* filename)**

```cpp

    ・・・省略・・・
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 
*       D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    ・・・省略・・・
```

指定している型が **DXGI_FORMAT_R32G32_FLOAT** である事は理解しておいて下さい。

---

#### <span style="color:#334488;">画像ファイルの読み込み</span>

    演習手順５－３

画像ファイルを読み込むために必要な、便利な関数を呼び出すために必要なヘッダをインクルードしましょう。

|ファイル名
--|--
**sprite.cpp**|

```cpp
    ・・・省略・・・

    #include <functional>
*   #include <WICTextureLoader.h>

    sprite::sprite(ID3D11Device* device, const wchar_t* filename)
    {
        ・・・省略・・・
```

ヘッダをインクルードしたらピクセルシェーダーの読み込みの後に画像ファイルの読み込みを実装しましょう。

|ファイル名|メンバ関数名
--|--
**sprite.cpp**|**sprite::sprite(ID3D11Device\* device, const wchar_t\* filename)**

```cpp

        ・・・省略・・・
        hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, &pixel_shader);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

*    ID3D11Resource* resource{};
     // 画像ファイルからリソースとシェーダーリソースビューを生成
*    hr = DirectX::CreateWICTextureFromFile(device, filename, &resource, &shader_resource_view);
*    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
*    resource->Release();
```

**CreateWICTextureFromFile** を呼び出す事で CPU 側で使われるリソースの情報と GPU 側で使われるシェーダーリソースビューの情報が生成されます。

---

    演習手順５－４

リソースの情報からテクスチャを生成し、テクスチャの情報を texture2d_desc に取り出しましょう。

```cpp
    ID3D11Texture2D* texture2d{};
    // リソースからテクスチャを生成
    hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    // テクスチャからテクスチャの情報を取り出す
    texture2d->GetDesc(&texture2d_desc);
    texture2d->Release();
```

---

    演習手順６－１

テクスチャ座標をコンストラクタで設定するのではなく sprite クラスの描画部分である render メンバ関数でテクスチャ座標を変更できるようにしておきましょう。

|ファイル名|メンバ関数名
--|--
**sprite.cpp**|**sprite::render(ID3D11DeviceContext\* immediate_context, ...)**


```cpp
    ・・・省略・・・
    vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };

*   vertices[0].texcoord = { 0, 0 };
*   vertices[1].texcoord = { 1, 0 };
*   vertices[2].texcoord = { 0, 1 };
*   vertices[3].texcoord = { 1, 1 };
}

immediate_context->Unmap(vertex_buffer, 0);


・・・省略・・・
```

---

    演習手順６－２

CPP 側の最後の設定として生成したテクスチャの情報を持っているシェーダーサブリソースをシェーダー側に登録しましょう。

|ファイル名|メンバ関数名
--|--
**sprite.cpp**|**render(ID3D11DeviceContext\* immediate_context, ...)**

```cpp
    ・・・省略・・・
    immediate_context->PSSetShader(pixel_shader, nullptr, 0);

*   immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);

    immediate_context->Draw(4, 0);
```

**PSSetShaderResources** が処理されることでテクスチャの情報がシェーダー側で、register(t0) として登録され利用することが出来るようになります。**演習手順９**でつながりを説明します。

---

    演習手順７

頂点データ、入力レイアウトともにテクスチャ座標の情報を追加したのでシェーダー側のコードにもテクスチャ座標の情報を追加して扱えるようにしましょう。

|ファイル名|
--|--
**sprite.hlsli**|

```hlsl
struct VS_OUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
*   float2 texcoord : TEXCOORD;
}
```

---

入力レイアウトを追加したので頂点シェーダーの main 関数の引数を追加しましょう。

    演習手順８

|ファイル名|
--|--
**sprite_vs.hlsl**|

```hlsl
*   VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
    {
        ・・・省略・・・
        vout.color = color;

*       vout.texcoord = texcoord;

        return vout;
    }

```

---

    演習手順９

シェーダーリソースビューの登録とサンプラーステートの登録および設定を行います。

```hlsl
    #include "sprite.hlsli"
*   Texture2D color_map : register(t0);
*   SamplerState point_sampler_state : register(s0);
*   SamplerState liner_sampler_state : register(s1);
*   SamplerState anisotropic_state : register(s2);
    float4 main(VS_OUT pin) : SV_TARGET
    {
*       return color_map.Sample(point_sampler_state, pin.texcoord);
    }
```

テクスチャの色の情報が設定されラップされている **shader_resource_view** は、
演習手順６－２の **PSSetShaderResources** でシェーダー側で利用できるようになりました。

シェーダー側で利用できるようになったテクスチャの情報がどこに言ったかというと

    Texture2D color_map : register(t0);

上記のコードの **t0** の値になります。

**t0** はピクセルシェーダーが処理される段階で、 **register(t0)** として処理され、**Texture2D** にキャストされて **color_map** 変数に代入されます。

---

#### <span style="color:#334488;">サンプラーステートの登録</span>

上記でテクスチャの情報を利用できるようにしたようにサンプラーステートの情報をここで利用できるようにしているのが下記の３つの設定になります。

```
    SamplerState point_sampler_state : register(s0);
    SamplerState liner_sampler_state : register(s1);
    SamplerState anisotropic_state : register(s2);
```

上からポイント、リニア、アニソトロピックでコード上で利用しているのはポイントを利用しています。

ここでもテクスチャと同じように register(...) という形になっているので CPP 側からの設定が必要になります。 その部分は演習手順１１からになります。

---

#### <span style="color:#334488;">ピクセルシェーダーの処理</span>

**main 関数**をみるとポイントフィルタ利用しているのがわかります。

    return color_map.Sample(point_sampler_state, pin.texcoord);

**color_map** にはテクスチャの情報が入っており、その情報の **pin.texcoord** のテクスチャ座標の位置の色を**ポイントフィルター**で表示するという処理になります。

pin.texcoord には 0.0f, 0.0f から 1.0f, 1.0f の値が設定されておりテクセル空間上の座標を指しています。

@import "images/テクスチャ座標.png"

---

スプライトの描画部分の実装が完了したのでまずは先ほど変更したコンストラクタの変更部分を修正しましょう。

    演習手順１０

|ファイル|メンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    viewport.MaxDepth = 1.0f;
    immediate_context->RSSetViewports(1, &viewport);

*   sprites[0] = new sprite(device, L".¥¥resources¥¥cyberpunk.jpg");

    return true;
```

---

#### <span style="color:#334488;">サンプラーステートの生成</span>

ピクセルシェーダー側に用意したサンプラーステートの生成を行います。

なぜ framework クラスに実装しているのか？

テクスチャはスプライトごとに異なる場合はあるのでスプライトごとにテクスチャを生成し設定していますが、サンプラーステートは種類が３つほどしかないのでスプライトごとに生成するとメモリがもったいないので framework クラスで生成し利用するスプライトにそれぞれ設定するという形になっています。

なので framework クラスに実装しています。

まずは生成したサンプラーステートの情報を保存しておくための変数を用意します。ポイント、リニア、アニソトロピックの３つ分を用意します。

    演習手順１１－１

|ファイル|クラス|
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
    sprite* sprites[8];

*   ID3D11SamplerState* sampler_states[3];

    framework(HWND hwnd);
    ・・・省略・・・
```

---

変数を用意できたらサンプラーステートの生成を実装しましょう。

    演習手順１１－２

|ファイル|メンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    viewport.MaxDepth = 1.0f;
    immediate_context->RSSetViewports(1, &viewport);

*   // サンプラーステートを生成するための構造体
*   D3D11_SAMPLER_DESC sampler_desc;
*   // テクスチャをサンプリングするときに使用するフィルタリング方法
*   // 縮小、拡大、およびミップレベルのサンプリングにはポイントサンプリングを使用します。
*   sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
*   // ０から１の範囲外のauテクスチャ座標を解決するために使用するメソッド
*   sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
*   // ０から１の範囲外のavテクスチャ座標を解決するために使用する方法
*   sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
*   // ０から１の範囲外のawテクスチャ座標を解決するために使用する方法
*   sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
*   // 計算されたミップマップレベルからのオフセット。
*   sampler_desc.MipLODBias = 0;
*   // クランプ値
*   sampler_desc.MaxAnisotropy = 16;
*   // サンプリングされたデータを既存のサンプリングされたデータと比較する関数
*   sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
*   // AddressU,V,W にD3D11_TEXTURE_ADDRESS_BORDER が指定されている場合に使用する境界線の色
*   sampler_desc.BorderColor[0] = 0;
*   sampler_desc.BorderColor[1] = 0;
*   sampler_desc.BorderColor[2] = 0;
*   sampler_desc.BorderColor[3] = 0;
*   // アクセスをクランプするミップマップの範囲の下限
*   sampler_desc.MinLOD = 0;
*   // アクセスをクランプするミップマップの範囲の上限
*   sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
*   // ポイントフィルタとしてサンプラーステートを生成
*   hr = device->CreateSamplerState(&sampler_desc, &sampler_states[0]);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // リニアフィルタとしてサンプラーステートを生成
*   sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
*   hr = device->CreateSamplerState(&sampler_desc, &sampler_states[1]);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // アニソトロピックとしてサンプラーステートを生成
*   sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
*   hr = device->CreateSamplerState(&sampler_desc, &sampler_states[2]);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

```

ごちゃごちゃしていてわかりづらいかもしれないが、**sampler_desc.Filter** に

    D3D11_FILTER_MIN_MAG_MIP_POINT
    D3D11_FILTER_MIN_MAG_MIP_LINEAR
    D3D11_FILTER_ANISOTROPIC

を代入している部分と **device->CreateSamplerState** の第二引数の **sampler_states** がそれぞれ

    sampler_states[0]
    sampler_states[1]
    sampler_states[2]

になっているだけで後は同じ設定です。

つまり **sampler_desc.Filter** に **D3D11_FILTER_MIN_MAG_MIP_POINT** を代入して **device->CreateSamplerState** を呼び出すとポイントフィルタとして **sampler_states[0]** にサンプラーステートが生成され、
**sampler_desc.Filter** に **D3D11_FILTER_MIN_MAG_MIP_LINEAR** を代入して **device->CreateSamplerState** を呼び出すとリニアフィルタとして **sampler_states[1]** にサンプラーステートが生成されるという事です。

---

#### <span style="color:#334488;">サンプラーステートをシェーダー側に設定</span>

生成したサンプラーステートをシェーダ側の

```
    register(s0);
    register(s1);
    register(s2);
```

の部分の s0 s1 s2 として呼び出せるように設定しましょう。

    演習手順１１－３

|ファイル|メンバ関数|
--|--
**framework.cpp**|**framework::render(float elapsed_time)**

```cpp
    ・・・省略・・・
    immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

*   // ポイントフィルタを s0
*   immediate_context->PSSetSamplers(0, 1, &sampler_states[0]);
*   // リニアフィルタを s1
*   immediate_context->PSSetSamplers(1, 1, &sampler_states[1]);
*   // アニソトロピックを s2
*   immediate_context->PSSetSamplers(2, 1, &sampler_states[2]);

    sprites[0]->render(immediate_context, 0, 0, 1200, 720, 1, 1, 1, 1, 0);
    ・・・省略・・・
```

これでピクセルシェーダが処理される際にテクスチャと同じようにサンプラーステートの設定も登録されシェーダー側で利用できるようになります。

---

    演習手順１２

実行して下記の画像が表示されることを確認してみてください。

@import "images/実行結果.png"

その際警告がでます。サンプラーステートも COM オブジェクトですが解放を行っていないという警告になります。適切な場所で解放してみてください。