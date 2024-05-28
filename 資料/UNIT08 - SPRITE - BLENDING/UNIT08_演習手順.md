<!--
UNIT08_EXERCISE UNIT08 演習手順
SPRITE - BLENDING

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;">

Blend state
Alpha Channel
Blend mode formulas
-->

# UNIT08 SPRITE - BLENDING 演習手順

キャラクター画像の背景色を透過させてみましょう。

ブレンドを有効化し、アルファを指定して実行すると以下のように表示されます。

@import "images/結果01.png"

今までの状態である、ブレンドが無効の状態にすると、

@import "images/結果無効.png"

ブレンドを有効化し、加算を指定して実行すると以下のように表示されます。

@import "images/結果加算.png"

ブレンドを有効化し、乗算を指定して実行すると以下のように表示されます。

@import "images/結果乗算.png"

---

#### <span style="color:#334488;">ブレンドステートを用意</span>

    演習手順１－１

|実装するファイル|実装するクラス|
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
    ID3D11DepthStencilState* depth_stencil_states[4];

*   // ブレンドステート (0...無し、1...アルファ、2...加算、3...乗算)
*   ID3D11BlendState* blend_states[4];

    framework(HWND hwnd);
    ・・・省略・・・
```

コメントに記載した通り、４種類のブレンドモードを生成します。

---

#### <span style="color:#334488;">ブレンドステートを生成</span>

    演習手順１－２

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[3]);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // ブレンドステートを生成するための構造体
*   D3D11_BLEND_DESC blend_desc{};
*   // アルファ
*   blend_desc.AlphaToCoverageEnable = FALSE;
*   blend_desc.IndependentBlendEnable = FALSE;
*   blend_desc.RenderTarget[0].BlendEnable = TRUE;
*   blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
*   blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
*   blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
*   blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
*   blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
*   blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
*   blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
*   hr = device->CreateBlendState(&blend_desc, &blend_states[0]);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    sprites[0] = new sprite(device, L".\\resources\\cyberpunk.jpg");
    ・・・省略・・・
```

**blend_desc** の **SrcBlend** **DestBlend** に D3D11_BLEND の指定を行い、**BlendOp** に演算の指定をして **CreateBlendState** に渡すことで **blend_states** に生成されたブレンドステートのインスタンスが代入されます。

ここで気になるのは **SrcBlendAlpha** と **DestBlendAlpha** と **BlendOpAlpha** です。**DEST** **SRC** にある**アルファチャンネル**に対しても指定した計算を行い**アルファ値**をだすという指定です。

* アルファ
```cpp
SrcBlendAlpha = D3D11_BLEND_ONE;
DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
```
```cpp
BlendOpAlpha = D3D11_BLEND_OP_ADD;
```

シェーダーのコードにすると下記のようになります。

```hlsl
dst.a * (1 - src.a) + src.a * 1
```

RGB として計算していた部分が A になるというだけです。

* 加算
```cpp
SrcBlendAlpha = D3D11_BLEND_ZERO;
DestBlendAlpha = D3D11_BLEND_ONE;
```
```cpp
BlendOpAlpha = D3D11_BLEND_OP_ADD;
```

* 乗算
```cpp
SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
DestBlendAlpha = D3D11_BLEND_ZERO;
```
```cpp
BlendOpAlpha = D3D11_BLEND_OP_ADD;
```

アルファチャンネルに対してもそれぞれにそれぞれの設定があるので RGB の設定と アルファチャンネルの設定をセットで設定しておきましょう。

---

#### <span style="color:#334488;">ブレンドステートを設定</span>

生成したアルファのブレンドをシェーダー側に設定しましょう。

    演習手順１－３

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render()**

```cpp
    ・・・省略・・・
    immediate_context->OMSetDepthStencilState(depth_stencil_states[3], 0);

*   // ブレンドステートの設定
*   immediate_context->OMSetBlendState(blend_states[0], nullptr, 0xFFFFFFFF);

    sprites[0]->render(immediate_context, 0, 0, 1280, 720, 1, 1, 1, 1, 0);
    ・・・省略・・・
```

この状態で起動して確認してみよう。ひとまず演習手順２は飛ばして演習手順３の確認を行ってみましょう。下記の画像が表示されるはずです。

    演習手順３

@import "images/結果01.png"

---

#### <span style="color:#334488;">render メンバ関数の引数の色指定部分を機能するように修正する</span>

    演習手順２

現状 **render メンバ関数** に色を指定できる部分があります。ですが、この部分を変更しても何も変化がありません。
以下のコードの第６引数から第９引数部分です。

```cpp
sprites[0]->render(immediate_context, 0, 0, 1280, 720, 1, 1, 1, 1, 0);
sprites[1]->render(immediate_context, 500, 200, 200, 200, 1, 1, 1, 1, 0, 0, 0, 140, 240);
```

    1,1,1,1

の部分を

    1,1,1,0.5

にしてもブレンドステートを有効化しているはずの画像が半透明にはなりません。

この部分の色指定を機能させるためにピクセルシェーダー側に変更を加えます。

以下のように実装されている * の部分を
```
    #include "sprite.hlsli"
*   Texture2D color_texture : register(t0);
*   SamplerState point_sampler_state : register(s0);
*   SamplerState linear_sampler_state : register(s1);
*   SamplerState anisotropic_sampler_state : register(s2);
    float4 main(VS_OUT pin) : SV_TARGET
    {
*       return color_texture.Sample(point_sampler_state, pin.texcoord);
    }
```

下記のように変更してください。

|実装するファイル|
--|--
**sprite_ps.hlsl**|

```hlsl
    #include "sprite.hlsli"
*   Texture2D diffuse_map : register(t0);
*   SamplerState diffuse_map_sampler_state : register(s0);
    float4 main(VS_OUT pin) : SV_TARGET
    {
*       return diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * pin.color;
    }
```

サンプラーステートに従ってピクセルの色を設定する設定に対して、

    color_texture.Sample(point_sampler_state, pin.texcoord);

外部からの色を反映させる処理を計算に加えました。

    diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * pin.color;

diffuse_map と変数名を変更していますが color_texture と同じです。 
diffuse_map_sampler_state と変数名を変更していますが point_sampler_state と同じです。
なので pin.color が追加されただけです。

**render メンバ関数** の色指定の**アルファ値**を 0.5 にして起動してみると

```cpp
sprites[1]->render(immediate_context, 500, 200, 200, 200, 1, 1, 1, 0.5f, 0, 0, 0, 140, 240);
```

@import "images/結果アルファ半減.png"

---

#### <span style="color:#334488;">その他のブレンドステートを追加する</span>

    演習手順４

演習手順１－２で実装したブレンドモード部分を以下のように追加、変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[3]);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // ブレンドステートを生成するための構造体
*   D3D11_BLEND_DESC blend_desc{};
*   // 無効
*   blend_desc.AlphaToCoverageEnable = FALSE;
*   blend_desc.IndependentBlendEnable = FALSE;
*   blend_desc.RenderTarget[0].BlendEnable = FALSE;
*   blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
*   blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
*   blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
*   blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
*   blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
*   blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
*   blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
*   hr = device->CreateBlendState(&blend_desc, &blend_states[0]);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // アルファ
    blend_desc.AlphaToCoverageEnable = FALSE;
    blend_desc.IndependentBlendEnable = FALSE;
    blend_desc.RenderTarget[0].BlendEnable = TRUE;
    blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
*   hr = device->CreateBlendState(&blend_desc, &blend_states[1]);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // 加算
*   blend_desc.AlphaToCoverageEnable = FALSE;
*   blend_desc.IndependentBlendEnable = FALSE;
*   blend_desc.RenderTarget[0].BlendEnable = TRUE;
*   blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
*   blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
*   blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
*   blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
*   blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
*   blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
*   blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
*   hr = device->CreateBlendState(&blend_desc, &blend_states[2]);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // 乗算
*   blend_desc.AlphaToCoverageEnable = FALSE;
*   blend_desc.IndependentBlendEnable = FALSE;
*   blend_desc.RenderTarget[0].BlendEnable = TRUE;
*   blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
*   blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
*   blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
*   blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
*   blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
*   blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
*   blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
*   hr = device->CreateBlendState(&blend_desc, &blend_states[3]);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    sprites[0] = new sprite(device, L".\\resources\\cyberpunk.jpg");
    ・・・省略・・・
```

作れそうなら **Blend mode formulas.docx** を確認して、その他のブレンドモードを追加してみてください。その際 **blend_states** の要素数の最大値には注意してください。

---

    演習手順５

上記で作成したブレンドモードの設定を変更して試してみましょう。
**blend_states** の添え字の番号を変更して確認してみてください。