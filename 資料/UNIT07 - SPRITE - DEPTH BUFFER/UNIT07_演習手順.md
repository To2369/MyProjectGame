<!--
UNIT07_EXERCISE UNIT07 演習手順
SPRITE - DEPTH BUFFER

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;">

Depth testing
Depth writing
-->

# UNIT07 SPRITE - DEPTH BUFFER 演習手順

---

今回の実行結果は次のようになります。

@import "images/深度テスト02.png"

---

    演習手順１

framework の render メンバ関数にある下記の部分の引数部分を

```
    sprites[0]->render(immediate_context, 100, 100, 500, 500, 1, 1, 1, 1, 45);
    sprites[1]->render(immediate_context, 700, 200, 200, 200, 1, 1, 1, 1, 45, 0, 0, 140, 240);
```

以下のように変更しよう。

|実装するクラス|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
    // 画像全体を画面全体に描画する
*   sprites[0]->render(immediate_context, 0, 0, 1280, 720, 1, 1, 1, 1, 0);
*   sprites[1]->render(immediate_context, 500, 200, 200, 200, 1, 1, 1, 1, 0, 0, 0, 140, 240);
```

実行すると矩形指定して描画している方のスプライトが描画出来なくなっているのが確認できるはずです。

@import "images/深度テスト01.png"

---

##### <span style="color:#334488;">描画順序を整理する</span>

現状、**深度テストが行われている**ので深度値によって描画の順序が決定されています。ですので深度テストを **OFF** にすることで描画を呼び出した順に表示されるよう変更しましょう。

つまり **Draw** を後に呼び出したものが手前に表示されるように制御します。

深度テストを設定するためのリソースを用意します。

    演習問題２－１

|実装するファイル|実装するクラス|
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
    ID3D11SamplerState* sampler_states[3];

    // 深度・ステンシルの状態
    ID3D11DepthStencilState* depth_stencil_states[4];

    framework(HWND hwnd);
    ・・・省略・・・
```

深度・ステンシルの設定は

* 深度テストを ON、深度への書き込み ON
* 深度テストを ON、深度への書き込み OFF
* 深度テストを OFF、深度への書き込み ON
* 深度テストを OFF、深度への書き込み OFF

の４つを設定出来るようにするので、配列として４つを用意しています。

---

##### <span style="color:#334488;">深度ステンシルの状態を生成</span>

    演習問題２－２

４つの**深度ステンシル**を生成します。

|実装するクラス|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize(...)**

```cpp
    ・・・省略・・・
    hr = device->CreateSamplerState(&sampler_desc, &sampler_states[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)]);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // 深度テストを ON、深度への書き込み ON
*   D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
*   depth_stencil_desc.DepthEnable = TRUE;
*   depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
*   depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[0]]);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // 深度テストを ON、深度への書き込み OFF
*   depth_stencil_desc.DepthEnable = TRUE;
*   depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
*   depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[1]);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // 深度テストを OFF、深度への書き込み ON
*   depth_stencil_desc.DepthEnable = FALSE;
*   depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
*   depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[2]);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // 深度テストを OFF、深度への書き込み OFF
*   depth_stencil_desc.DepthEnable = FALSE;
*   depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
*   depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[3]);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    sprites[0] = new sprite(device, L".\\resources\\cyberpunk.jpg");
    ・・・省略・・・
```

---

##### <span style="color:#334488;">深度ステンシルを設定/span>

    演習手順２－３

生成した深度ステンシルの中から指定の深度ステンシルをシェーダ側に設定します。
今回は深度テストを OFF 、深度書き込み OFF を設定します。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
    ・・・省略・・・
    immediate_context->PSSetSamplers(2, 1, &sampler_states[2]);

*   // 深度テストを OFF 深度書き込み OFF
*   immediate_context->OMSetDepthStencilState(depth_stencil_states[3], 1);

    sprites[0]->render(immediate_context, 0, 0, 1280, 720, 1, 1, 1, 1, 0);
    ・・・省略・・・
```

設定が実装できたら実行して確認してみよう。

@import "images/深度テスト02.png"

---

##### <span style="color:#334488;">深度ステンシルの解放/span>

    演習手順３

深度ステンシルは COM オブジェクトなので解放処理を忘れないようにしましょう。
ここでは **framework** の **uninitialize メンバ関数** で解放処理を行いましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::uninitialize**

```cpp
    ・・・省略・・・
    for (ID3D11SamplerState* p : sampler_states) p->Release();

*   for (ID3D11DepthStencilState* p : depth_stencil_states) p->Release();

    return true;
```

