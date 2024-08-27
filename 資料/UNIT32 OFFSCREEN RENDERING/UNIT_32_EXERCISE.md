<!--
UNIT32_EXERCISE UNIT32 演習手順
OFFSCREEN RENDERING

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Frame buffer
Gaussian blur
Downsampling
Linear color space
HDR(High Dynamic Range)
SDR(Standard Dynamic Range)
Gamma correction
Inverse gamma correction(Degamma)
Tone mapping
-->

# オフスクリーンレンダリング

* 目次
    * [1. テスト用オブジェクトの生成と描画](#01_)
        * [1. テクスチャのロードをモジュール化](#テクスチャのロードをモジュール化_)

---

### 1 テスト用オブジェクトの生成と描画

#### 1-1 

|||
--|--
**framework.cpp**|**framework::initialize()**

インスタンスの生成を一旦以下の２つだけにして他はコメントアウトするか削除して下さい。

```cpp
    ・・・省略・・・

*   sprite_batches[0] = std::make_unique<sprite_batch>(device.Get(), L".\\resources\\screenshot.jpg", 1);
*   skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\nico.fbx");

    return true;
```

---

#### 1-2 1-3 1-4

|||
--|--
**framework.cpp**|**framework::render(...)**

```cpp
    ・・・省略・・・
    immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

    // 2D 描画（面カリングなし、深度テストなし、深度ライトなし）
*   immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
*   immediate_context->RSSetState(rasterizer_states[2].Get());

*   sprite_batches[0]->begin(immediate_context.Get());
*   sprite_batches[0]->render(immediate_context.Get(), 0, 0, 1280, 720);
*   sprite_batches[0]->end(immediate_context.Get());

    // 3D 描画（面カリングあり、深度テストあり、深度ライトあり）
*   immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);
*   immediate_context->RSSetState(rasterizer_states[0].Get());

    const DirectX::XMFLOAT4X4 coordinate_system_transforms[]{
        { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 0:右手系 Y-UP
    ・・・省略・・・
```

2D の深度設定とラスタライズの設定を行って 2D のスプライトバッチの描画を行ったらすぐに、 3D 深度設定とラスタライズの設定にもどしています。

---

### 2 オフスクリーンレンダリングを行うために、framebuffer クラスを定義・実装する

オフスクリーンレンダリングは 2D にしか行えない処理を 3D にほどこすための処理です。

いままでずっとスクリーンとして書き込んでいた場所はどこだったでしょうか？
それはレンダーターゲットとして指定していたスワップチェインのバックバッファテクスチャです。
いつもはここに書き込んでいた処理を、一旦単なる 2D のテクスチャに書き込むとい事を行います。
スクリーンでは無い場所、つまりオフスクリーンの場所に書き込むという事を行います。

3D のモデルがレンダリングパイプラインを通り、単なる 2D のテクスチャに書き込まれたら、あとはその 3D のモデルが書き込まれた 2D のテクスチャに対して 2D でしかできなかった様々なシェーダーエフェクトを実装していくということを行えるのです。

ということで、まずはスクリーンではない別の書き込み先となる単なる 2D のテクスチャとそれを持ったレンダーターゲットを作成しましょう。

新規で framebuffer.h と framebuffer.cpp を作成してください。

||
--|--
**framebuffer.h**|

```cpp
#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

/// <summary>
/// オフスクリーン描画用
/// </summary>
class framebuffer
{
public:
    framebuffer(ID3D11Device* device, uint32_t width, uint32_t height);
    virtual ~framebuffer() = default;

    // 書き込む用のレンダーターゲットビュー
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
    
    // 書き込み用の深度ステンシルビュー
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

    // 0...レンダーターゲットビュー用 1...深度ステンシルビュー用
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[2];
    
    // 書き込み用のビューポート
    D3D11_VIEWPORT viewport;

    // クリア処理
    void clear(ID3D11DeviceContext* immediate_context,
        float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1);
    
    // オフスクリーンレンダリングの設定に連結
    // 今までの描画情報をキャッシュに退避させオフスクリーン書き込み用にターゲットを設定
    void activate(ID3D11DeviceContext* immediate_context);
    
    // オフスクリーンの設定から連結を解除
    void deactivate(ID3D11DeviceContext* immediate_context);

private:
    // 退避用のキャッシュ情報
    UINT viewport_count{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
    D3D11_VIEWPORT cached_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;
};
```

---

||
--|--
**framebuffer.cpp**|

```cpp
#include "framebuffer.h"
#include "misc.h"

framebuffer::framebuffer(ID3D11Device* device, uint32_t width, uint32_t height)
{
    HRESULT hr{ S_OK };
    
    // オフスクリーンレンダリング用の描画先のテクスチャを作成
    Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    texture2d_desc.Width = width;
    texture2d_desc.Height = height;
    texture2d_desc.MipLevels = 1;
    texture2d_desc.ArraySize = 1;
    texture2d_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texture2d_desc.SampleDesc.Count = 1;
    texture2d_desc.SampleDesc.Quality = 0;
    texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
    texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture2d_desc.CPUAccessFlags = 0;
    texture2d_desc.MiscFlags = 0;
    hr = device->CreateTexture2D(&texture2d_desc, 0, render_target_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // オフスクリーンレンダリング用の描画先のレンダーターゲットビューを生成
    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc{};
    // 描画先のテクスチャとして上記で作成したテクスチャを設定
    render_target_view_desc.Format = texture2d_desc.Format;
    render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = device->CreateRenderTargetView(render_target_buffer.Get(), &render_target_view_desc,
        render_target_view.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // 上記で作成したレンダーターゲットビュー用のテクスチャのシェーダーリソースビューを作成
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
    shader_resource_view_desc.Format = texture2d_desc.Format;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(render_target_buffer.Get(), &shader_resource_view_desc,
        shader_resource_views[0].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // オフスクリーンレンダリング用の描画先の深度ステンシル用テクスチャを作成
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
    texture2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    hr = device->CreateTexture2D(&texture2d_desc, 0, depth_stencil_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // オフスクリーンレンダリング用の描画先の深度ステンシルビューを作成
    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
    depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Flags = 0;
    hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc,
        depth_stencil_view.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // 上記で作成した深度ステンシルビュー用のテクスチャのシェーダーリソースビューを作成
    shader_resource_view_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &shader_resource_view_desc,
        shader_resource_views[1].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // オフスクリーンレンダリング用のビューポートのサイズを設定
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
}

/// <summary>
/// 現状このレンダーターゲットに表示されているピクセル情報をクリア
/// </summary>
void framebuffer::clear(ID3D11DeviceContext* immediate_context,
    float r, float g, float b, float a, float depth)
{
    float color[4]{ r, g, b, a };
    immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
    immediate_context->ClearDepthStencilView(depth_stencil_view.Get(),D3D11_CLEAR_DEPTH, depth, 0);
}

/// <summary>
/// オフスクリーンレンダリングの設定に連結
/// 今までの描画情報をキャッシュに退避させオフスクリーン書き込み用にターゲットを設定
/// </summary>
void framebuffer::activate(ID3D11DeviceContext* immediate_context)
{
    // 現在指定中のビューポートとレンダーターゲットと深度ステンシルの情報を退避
    viewport_count = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    immediate_context->RSGetViewports(&viewport_count, cached_viewports);
    immediate_context->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(),
        cached_depth_stencil_view.ReleaseAndGetAddressOf());

    // オフスクリーン用にこのクラスで作成したビューポートとレンダーターゲットと深度ステンシルを設定
    immediate_context->RSSetViewports(1, &viewport);
    immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
}

/// <summary>
/// オフスクリーンレンダリングの設定から連結を解除
/// </summary>
void framebuffer::deactivate(ID3D11DeviceContext* immediate_context)
{
    // 退避させていたビューポートとレンダーターゲットと深度ステンシルの情報を再設定
    immediate_context->RSSetViewports(viewport_count, cached_viewports);
    immediate_context->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(),
        cached_depth_stencil_view.Get());
}
```

---

## 3 オフスクリーンバッファ(framebuffer) に描画する

framework クラスに framebuffer を定義し、演習手順１で生成した sprite_batch と skinned_mesh の描画を
framebuffer のオフスクリーン用のレンダーターゲットに描画してみましょう。

まずは framebuffer を宣言し、生成しましょう。

---

### 3-1 framework クラスのメンバ変数を定義する

* 必要なファイルはインクルードしておいてください。

|||
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・

    // アニメーション制御できる値 { 回転、位置 }
    float factors[4]{ 0.0f, 121.438332f };

*   // オフスクリーン用
*   std::unique_ptr<framebuffer> framebuffers[8];

    framework(HWND hwnd);
    ・・・省略・・・
```

---

### 3-2 framework クラスの initialize メンバ関数で framebuffer オブジェクトを生成する

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\nico.fbx");

*   // オフスクリーン用のバッファを作成
*   framebuffers[0] = std::make_unique<framebuffer>(device.Get(), 1280, 720);

    return true;
```

---

### 3-3 framework クラスの render メンバ関数で描画先をオフスクリーンバッファ (framebuffer) に変更する

生成が完了したら描画部分で sprite_batch と skinned_mesh の描画を framebuffer の activate() と deactivate() で挟み込みます。 

ですが呼び出す位置がそれなりに重要になります。
オフスクリーン用のクラス内部で深度ステンシルビューの設定も行っているので、描画する sprite_batch や skinned_mesh の為に設定している深度ステンシルステートの情報も activate() と deactivate() の間に挟みこんでいないといけません。
定数バッファはシェーダー側の情報というだけなので挟みこむ必要性が無いというのは何となく分かると思います。

ということでオフスクリーンレンダリングを実装しましょう。


|||
--|--
**framework.cpp**|**framework::render(...)**

```cpp
        ・・・省略・・・
        // 設定したシーン定数をバッファをピクセルシェーダー側に登録
        immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

*       framebuffers[0]->clear(immediate_context.Get());
*       framebuffers[0]->activate(immediate_context.Get());

        // 2D 描画(面カリングなし、深度テストなし、深度書き込みなし)
        immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
        immediate_context->RSSetState(rasterizer_states[2].Get());

        ・・・省略・・・
        {
            skinned_meshes[0]->render(immediate_context.Get(), world, material_color, nullptr);
        }

*       framebuffers[0]->deactivate(immediate_context.Get());

    #ifdef USE_IMGUI
        ImGui::Render();
        ・・・省略・・・
```

clear() でオフスクリーンレンダリングのスクリーンをクリアにしてから activate で描画先であるレンダーのターゲットを変更。
この処理以降に描画を行う事でオフスクリーン用のレンダーターゲットに設定したテクスチャに書き込みが行われます。
この状態が deactivate() によって以前のレンダーターゲットの状態に戻されるまで続きます。

よって sprite_batch と skinned_mesh はオフスクリーン用のテクスチャに書き込まれることになります。

---

### 3-4 実行し、シーン（背景とキャラクタ）が描画されない事を確認する

現状の状態で起動することでオフスクリーン用のレンダーターゲットに設定されたテクスチャに書き込みが行われているので画面に何も表示されていない事が確認できると思います。

---

## 4 framebuffer オブジェクトをテクスチャ(シェーダーリソースビュー)として扱い、画面に描画する

さまざまな 2D 用のシェーダーをほどこす前にまずは 2D のテクスチャに問題なく描画されていたのかどうかを確認してみましょう。

確認としてはオフスクリーン用の framebuffer クラスにあるシェーダーリソースビューを 2D のテクスチャとして描画するだけなので、既に実装してある sprite クラスのシェーダーリソースビューとして設定して描画しても良いのですが無駄な処理が多いので、スクリーン描画をする為だけの全画面描画専用の処理速度を重視したスプライトクラスを別途作成しましょう。

新規で fullscreen_quad.hlsli fullscreen_quad_vs.hlsl fullscreen_quad_ps.hlsl fullscreen_quad.h fullscreen_quad.cpp を作成して下さい。

---

### 4-1 fullscreen_quad_vs 頂点シェーダーの実装 (プロジェクトに fullscreen_quad_vs.hlsl を新規追加する)

fullscreen_quad.hlsli は考えての実装になりますが、ここでは記載しておくので新規追加して以下を追加して下さい。

||
--|--
**fullscreen_quad.hlsli**|

```cpp
struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};
```

---

||
--|--
**fullscreen_quad_vs.hlsl**|

```cpp
#include "fullscreen_quad.hlsli"

VS_OUT main(in uint vertexid : SV_VERTEXID)
{
    VS_OUT vout;
    const float2 position[4] = { {-1, +1},{+1, +1},{-1, -1},{+1, -1} };
    const float2 texcoords[4] = { { 0, 0 }, { 1, 0 },{ 0, 1 }, { 1, 1 } };
    vout.position = float4(position[vertexid], 0, 1);
    vout.texcoord = texcoords[vertexid];
    return vout;
}
```

---

### 4-2 fullscreen_quad_ps ピクセルシェーダーの実装 (プロジェクトに fullscreen_quad_ps.hlsl を新規追加する)

||
--|--
**fullscreen_quad_ps.hlsl**|

```cpp
#include "fullscreen_quad.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
    return texture_map.Sample(sampler_states[LINEAR], pin.texcoord);
}
```

---

### 4-3 fullscreen_quad クラスの実装 (プロジェクトに fullscreen_quad.h を新規追加する)

||
--|--
**fullscreen_quad.h**|

```cpp
#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

/// <summary>
/// オフスクリーン描画用のシェーダーリソースビュー描画用のスプライト
/// </summary>
class fullscreen_quad
{
public:
    fullscreen_quad(ID3D11Device* device);
    virtual ~fullscreen_quad() = default;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> embedded_vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> embedded_pixel_shader;

public:
    /// <summary>
    /// オフスクリーン描画したテクスチャのスクリーンへの最終的な描画処理
    /// </summary>
    /// <param name="immediate_context">デバイスコンテキスト</param>
    /// <param name="shader_resource_view">オフスクリーン描画されたテクスチャの配列</param>
    /// <param name="start_slot">配列の描画開始番号</param>
    /// <param name="num_views">配列の描画数</param>
    /// <param name="replaced_pixel_shader"></param>
    void blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView** shader_resource_view,
        uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader = nullptr);
};
```

---

### 4-4 fullscreen_quad クラスの実装 (プロジェクトに fullscreen_quad.cpp を新規追加する)

オフスクリーンとして描かれたテクスチャを設定
複数枚のテクスチャが設定される場合は shader_resource_view は配列になっている
start_slot には描画を開始するテクスチャの配列の添え字番号を
num_views には重ねて描画するテクスチャの配列の描画数

||
--|--
**fullscreen_quad.cpp**|

```cpp
#include "fullscreen_quad.h"
#include "shader.h"
#include "misc.h"

fullscreen_quad::fullscreen_quad(ID3D11Device* device)
{
    // 専用の頂点シェーダーの作成
    create_vs_from_cso(device, "fullscreen_quad_vs.cso", embedded_vertex_shader.ReleaseAndGetAddressOf(),
        nullptr, nullptr, 0);

    // 専用のピクセルシェーダーの作成
    create_ps_from_cso(device, "fullscreen_quad_ps.cso", embedded_pixel_shader.ReleaseAndGetAddressOf());
}

void fullscreen_quad::blit(ID3D11DeviceContext* immediate_context,
    ID3D11ShaderResourceView** shader_resource_view, uint32_t start_slot, uint32_t num_views, 
    ID3D11PixelShader* replaced_pixel_shader)
{
    // 頂点などはこちらからは送らないので統べて null
    immediate_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    // シェーダー側で直に値を設定するのでプリミティブトポロジーは設定
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    // 頂点シェーダーに送る値は無いので入力レイアウトは null
    immediate_context->IASetInputLayout(nullptr);

    // 頂点シェーダーを設定
    immediate_context->VSSetShader(embedded_vertex_shader.Get(), 0, 0);
    // ピクセルシェーダーを設定
    replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, 0, 0) :
    immediate_context->PSSetShader(embedded_pixel_shader.Get(), 0, 0);

    // オフスクリーンとして描かれたテクスチャを設定
    immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_view);

    immediate_context->Draw(4, 0);
}
```

---

## 4-5 framework クラスのメンバ変数を定義する

オフスクリーン描画用のシェーダーリソースビュー描画用のスプライトの実装が完了したら変数を宣言しましょう。

* 必要なファイルはインクルードしておいてください。

|||
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・

    // オフスクリーン用
    std::unique_ptr<framebuffer> framebuffers[8];
*   // オフスクリーン描画用のシェーダーリソースビュー描画用のスプライト
*   std::unique_ptr<fullscreen_quad> bit_block_transfer;

    framework(HWND hwnd);
    ・・・省略・・・
```

---

## 4-5 framework クラスの initialize メンバ関数で fullscreen_quad オブジェクトを生成する

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・

    // オフスクリーン用のバッファを作成
    framebuffers[0] = std::make_unique<framebuffer>(device.Get(), 1280, 720);
*   // オフスクリーン描画用のシェーダーリソースビュー描画用のスプライトの作成
*   bit_block_transfer = std::make_unique<fullscreen_quad>(device.Get());

    return true;
```

---

### 4-7 framework クラスの render メンバ関数でオフスクリーンバッファ(framebuffer) の内容を画面に描画する
※面カリングなし、深度テストなし、深度ライトなし
※動作確認後 #if-#endif ディレクティブのコードは無効にすること

生成したらオフスクリーン描画をもとのレンダーターゲットに戻す deactive() 関数の後に blit() を呼び出して、オフスクリーン描画で書き込んだテクスチャを通常のレンダーターゲットにスプライトとして描画しましょう。

|||
--|--
**framework.cpp**|**framework::render(...)**

```cpp
        ・・・省略・・・

        framebuffers[0]->deactivate(immediate_context.Get());

*   #if 1
*       immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
*       immediate_context->RSSetState(rasterizer_states[2].Get());
*       bit_block_transfer->blit(immediate_context.Get(), framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1);
*   #endif


    #ifdef USE_IMGUI
        ・・・省略・・・
```

## 4-8 実行し、シーン（背景とキャラクタ）が描画されている事を確認する

オフスクリーン描画用のシェーダーリソースビューを描画する為のスプライトである fullscreen_quad に対して、これまでに描画した情報である framebuffers[0] の shader_resource_views[0] を設定して描画処理である blit() を呼び出すことで、オフスクリーンとしてテクスチャ(シェーダーリソースビュー)に描画され情報がスクリーンに描画される。

これまで通りに描画されていますが、スプライトとして描画されているという点を理解しておきましょう。

@import "images/オフスクリーン描画.png"

---

## 5 framebuffer オブジェクトに描画されたシーンから高輝度成分を抽出する

今までスワップチェインのバックバッファのテクスチャに描画して、スクリーンに描画していましたが、無事オフスクリーンにあるテクスチャに描画し、そのシェーダーリソースビューをスプライトとして表示することが出来たので、2D にほどこせるいろいろな演出を実装していきましょう。

まずは高輝度成分抽出であるブルーム処理を実装してみましょう。

### 5-1 framework クラスの initialize メンバ関数で framebuffer オブジェクトを生成する

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・

    // オフスクリーン用のバッファを作成
    framebuffers[0] = std::make_unique<framebuffer>(device.Get(), 1280, 720);
*   // 高輝度抽出用のオフスクリーン用のレンダーターゲットを作成
*   framebuffers[1] = std::make_unique<framebuffer>(device.Get(), 1280 / 2, 720 / 2);
    // オフスクリーン描画用のシェーダーリソースビュー描画用のスプライトの作成
    bit_block_transfer = std::make_unique<fullscreen_quad>(device.Get());

    ・・・省略・・・
```

生成しているサイズを半分にしているのは縮小バッファと呼ばれる方法でピクセルシェーダーの処理の高速化の為にやっています。レンダーターゲットのサイズは半分になりますが、表示するサイズは変わらないという仕組みなので、単純に表示する１ピクセルのサイズが大きくなるため、ピクセルの数が減る、つまりピクセルシェーダーが呼び出される回数が減るという仕組みです。

---

### 5-2 luminance_extraction_ps ピクセルシェーダーの実装(プロジェクトに luminance_extraction_pc.hlsl を新規追加する)

||
--|--
**luminance_extraction_ps.hlsl**|

```cpp
#include "fullscreen_quad.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
	float alpha = color.a;
	color.rgb = smoothstep(0.6, 0.8, dot(color.rgb, float3(0.299, 0.587, 0.114))) * color.rgb;
	return float4(color.rgb, alpha);
}
```

---

### 5-3 framework クラスのメンバ変数を定義する

|||
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
    // オフスクリーン描画用のシェーダーリソースビュー描画用のスプライト
    std::unique_ptr<fullscreen_quad> bit_block_transfer;

*   // 2D にほどこせる様々なピクセルシェーダー
*   Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader[8];

    framework(HWND hwnd);
    ・・・省略・・・
```

---


### 5-4 framework クラスの initialize メンバ関数でピクセルシェーダーオブジェクトを生成する

|||
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
    // オフスクリーン描画用のシェーダーリソースビュー描画用のスプライトの作成
    bit_block_transfer = std::make_unique<fullscreen_quad>(device.Get());

*   // 高輝度抽出用ピクセルシェーダーの作成
*   create_ps_from_cso(device.Get(), "luminance_extraction_ps.cso", pixel_shaders[0].GetAddressOf());

    return true;
```

---


### 5-5 framework クラスの render メンバ関数で framebuffers[0] から高輝度成分を抽出し framebuffers[1] に転送する
※動作確認後 #if-#endif ディレクティブのコードは無効にすること

|||
--|--
**framework.h**|**framework**

```cpp
        framebuffers[0]->deactivate(immediate_context.Get());

*   #if 0
        immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);
        immediate_context->RSSetState(rasterizer_states[2].Get());
        bit_block_transfer->blit(immediate_context.Get(), framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1);
    #endif

*       framebuffers[1]->clear(immediate_context.Get());
*       framebuffers[1]->activate(immediate_context.Get());
*       immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
*       immediate_context->RSSetState(rasterizer_states[2].Get());
*       // オフスクリーン描画したテクスチャに高輝度抽出用ピクセルシェーダーを設定してオフスクリーン描画
*       bit_block_transfer->blit(immediate_context.Get(), framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1, pixel_shaders[0].Get());
*       framebuffers[1]->deactivate(immediate_context.Get());

*   #if 1
*       
*       immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
*       immediate_context->RSSetState(rasterizer_states[2].Get());
*       // 高輝度抽出したオフスクリーン描画のテクスチャをスクリーンに描画
*       bit_block_transfer->blit(immediate_context.Get(),
*           framebuffers[1]->shader_resource_views[0].GetAddressOf(), 0, 1);
*   #endif

    #ifdef USE_IMGUI
```

---

### 5-8 実行し、抽出された高輝度成分が描画されている事を確認する

@import "images/高輝度成分抽出.png"

---

#### 5-8-2 ※ smoothstep のパラメータを変更し変化を確認する(定数バッファを定義して CPU から制御する)

高輝度成分用の定数バッファ用の構造体を定義します。

以降もブラーやらブルームやらの定数バッファとして利用するのでポストエフェクト用の定数バッファとします。

||
--|--
**luminance_extraction_ps.hlsl**|

```cpp
    #include "fullscreen_quad.hlsli"

*   cbuffer PARAMETRIC_CONSTANT_BUFFER : register(b2)
*   {
*       float luminance_extraction_lower_edge;
*       float luminance_extraction_higher_edge;
*       float2 dummy;
*   };

    #define POINT 0
```

---

設定した定数バッファの値で高輝度成分の値を操作できるよう luminance_extraction_ps.hlsl の main() の 以下の部分を

```cpp
color.rgb = smoothstep(0.6, 0.8, dot(color.rgb, float3(0.299, 0.587, 0.114))) * color.rgb;
```

以下のように変更して下さい。

|||
--|--
**luminance_extraction_ps.hlsl**|**main(...)**

```cpp
color.rgb = smoothstep(luminance_extraction_lower_edge, luminance_extraction_higher_edge, dot(color.rgb, float3(0.299, 0.587, 0.114))) * color.rgb;
```

---

シェーダー側に定義したポストエフェクト用定数バッファに従って framework クラスにポストエフェクト用定数バッファフォーマットを定義しましょう。

定数バッファオブジェクト自体は既に宣言されている constant_buffers の 1 番を利用します。

|||
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
        DirectX::XMFLOAT4 camera_position;		// カメラ位置
    };

*   // ポストエフェクト用定数バッファ
*   struct parametric_constants
*   {
*       float luminance_extraction_lower_edge{ 0.6f };
*       float luminance_extraction_higher_edge{ 0.8f };
*       DirectX::XMFLOAT2 dummy;
*   };
*   parametric_constants parametric_constants;

    // 定数バッファオブジェクト
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];
    ・・・省略・・・
```

---

用意したら constant_buffers[1] に定数バッファオブジェクトを生成しましょう。

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[0].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // ポストエフェクト用定数バッファオブジェクトの生成
*   buffer_desc.ByteWidth = sizeof(parametric_constants);
*   buffer_desc.Usage = D3D11_USAGE_DEFAULT;
*   buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
*   buffer_desc.CPUAccessFlags = 0;
*   buffer_desc.MiscFlags = 0;
*   buffer_desc.StructureByteStride = 0;
*   hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[1].GetAddressOf());
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    sprite_batches[0] = std::make_unique<sprite_batch>(device.Get(), L".\\resources\\screenshot.jpg", 1);
    skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\nico.fbx");
    ・・・省略・・・
```

---

framework::update(...) で ImGUI から値を操作して

|||
--|--
**framework.cpp**|**framework::update(...)**

```cpp
    ・・・省略・・・
    ImGui::SliderFloat("factors[2]", &factors[2], +0.0f, +1.0f);

*   ImGui::SliderFloat("luminance_extraction_lower_edge", &parametric_constants.luminance_extraction_lower_edge, +0.0f, +1.0f);
*   ImGui::SliderFloat("luminance_extraction_higher_edge", &parametric_constants.luminance_extraction_higher_edge, +0.0f, +5.0f);

    ImGui::End();
    ・・・省略・・・
```

---

framework::render(...) で定数バッファに値の設定を行いましょう。

|||
--|--
**framework.cpp**|**framework::render(...)**

```cpp
    ・・・省略・・・
    immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

*   // ポストエフェクト用定数バッファに値を設定し、ピクセルシェーダー側に登録
*   immediate_context->UpdateSubresource(constant_buffers[1].Get(), 0, 0, &parametric_constants, 0, 0);
*   immediate_context->PSSetConstantBuffers(2, 1, constant_buffers[1].GetAddressOf());

    framebuffers[0]->clear(immediate_context.Get());
    ・・・省略・・・
```

---

## 6 抽出された高輝度成分にブラーエフェクトをかける

### 6-1 blur_ps ピクセルシェーダーの実装（プロジェクトに blur_ps.hlsl を新規追加する）

オフスクリーンレンダリングした高輝度成分のテクスチャに対して、再度オフスクリーンレンダリングでブラーのエフェクトを行います。

まずはブラー用のシェーダーを作成しましょう。

||
--|--
**blur_ps.hlsl**|

```cpp
#include "fullscreen_quad.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
	uint mip_level = 0, width, height, number_of_levels;
	texture_maps[1].GetDimensions(mip_level, width, height, number_of_levels);
	
	float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
	float alpha = color.a;

	float3 blur_color = 0;
	float gaussian_kernel_total = 0;

	const int gaussian_half_kernel_size = 3;
	const float gaussian_sigma = 1.0;

	[unroll]
	for (int x = -gaussian_half_kernel_size; x <= +gaussian_half_kernel_size; x += 1)
	{
		for (int y = -gaussian_half_kernel_size; y <= +gaussian_half_kernel_size; y += 1)
		{
			float gaussian_kernel = exp(-(x * x + y * y) / (2.0 * gaussian_sigma * gaussian_sigma)) /
				(2 * 3.14159265358979 * gaussian_sigma * gaussian_sigma);
			blur_color += texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord +
				float2(x * 1.0 / width, y * 1.0 / height)).rgb * gaussian_kernel;
			gaussian_kernel_total += gaussian_kernel;
		}
	}
	blur_color /= gaussian_kernel_total;
	const float bloom_intensity = 1.0;
	color.rgb += blur_color * bloom_intensity;

	return float4(color.rgb, alpha);
}
```

---

### 6-2 framework クラスの initialize メンバ関数でピクセルシェーダーオブジェクトを生成する

framework クラスに宣言した pixel_shaders 配列の 1 番に上記のブラーを設定、生成しましょう。

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    // 高輝度抽出用ピクセルシェーダーの作成
    create_ps_from_cso(device.Get(), "luminance_extraction_ps.cso", pixel_shaders[0].GetAddressOf());
*   // ブラー用ピクセルシェーダーの作成
*   create_ps_from_cso(device.Get(), "blur_ps.cso", pixel_shaders[1].GetAddressOf());

    return true;
```

---

### 6-3 framework クラスの render メンバ関数でシーン画像とブラーをかけた高輝度成分画像を合成し、画面に出力する
※シーン画像とは framebuffers[0] のシェーダーリソースビューのこと
※高輝度成分画像とは framebuffers[1] のシェーダーリソースビューのこと

framework::render(...) で高輝度成分画像を表示していた以下の部分の

```cpp
*   #if 1
        immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
        immediate_context->RSSetState(rasterizer_states[2].Get());
        bit_block_transfer->blit(immediate_context.Get(), framebuffers[1]->shader_resource_views[0].GetAddressOf(), 0, 1);
    #endif
```

マクロを切って

|||
--|--
**framework.cpp**|**framework::render(...)**

```cpp
*   #if 0
        immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
        immediate_context->RSSetState(rasterizer_states[2].Get());
        bit_block_transfer->blit(immediate_context.Get(), framebuffers[1]->shader_resource_views[0].GetAddressOf(), 0, 1);
    #endif
```

高輝度成分画像とブラー画像を掛け合わせた画像を最終的なシーンのレンダーターゲットに描画するように実装しましょう。

|||
--|--
**framework.cpp**|**framework::render(...)**

```cpp
    ・・・省略・・・
        bit_block_transfer->blit(immediate_context.Get(), framebuffers[1]->shader_resource_views[0].GetAddressOf(), 0, 1);
    #endif

*       immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
*       immediate_context->RSSetState(rasterizer_states[2].Get());
*       ID3D11ShaderResourceView* shader_resource_views[2]{ framebuffers[0]->shader_resource_views[0].Get(), framebuffers[1]->shader_resource_views[0].Get() };
*       // 高輝度処理をほどこしたテクスチャとブラーをほどこしたテクスチャを合成して描画
*       bit_block_transfer->blit(immediate_context.Get(), shader_resource_views, 0, 2, pixel_shaders[1].Get());


    #ifdef USE_IMGUI
    ・・・省略・・・
```

---

### 6-4 実行し、元画像にブルームがかかっていることを確認する

@import "images/高輝度成分とブラー.png"

---

※① gaussian_half_kernel_size の値を変更し変化を確認する

blur_ps.hlsl の以下の部分を

```cpp
const int gaussian_half_kernel_size = 3;
```

以下のように変更する

|||
--|--
**blur_ps.hlsl**|**main(...)**

```cpp
const int gaussian_half_kernel_size = 10;
```

---

※① gaussian_sigma の値を変更し変化を確認する（定数バッファを定義して CPU から制御する）
※① bloom_intensity の値を変更し変化を確認する（定数バッファを定義して CPU から制御する）

parametric_constants に gaussian_sigma と bloom_intensity を追加しましょう。
framework ヘッダにある定数バッファの以下の部分を 

```cpp
DirectX::XMFLOAT2 dummy;
```

以下のように変更しましょう。

|||
--|--
**framework.h**|**framework::parametric_constants**

```cpp
float gaussian_sigma{ 1.0f };
float bloom_intensity{ 1.0f };
```

---

blur_ps.hlsl 側に定数バッファを追加しますが luminance_extraction_lower_edge と luminance_extraction_higher_edge の値が入っている上２つは利用しないので dummy (ダミーの値) となります。

||
--|--
**blur_ps.hlsl**|

```cpp
    #include "fullscreen_quad.hlsli"

*   cbuffer PARAMETRIC_CONSTANT_BUFFER : register(b2)
*   {
*       float2 dummy;
*       float gaussian_sigma;
*       float bloom_intensity;
*   };

    #define POINT 0
    ・・・省略・・・
```

---

ブラー用の定数バッファを追加したら値が反映できるようコードを変更していきます。

blur_ps.hlsl の main() 関数の gauusian_sigma と bloom_intensity をローカル宣言している以下の部分を

```cpp
・・・省略・・・
const float gaussian_sigma = 1.0;
・・・省略・・・
const float bloom_intensity = 1.0;
・・・省略・・・
```

コメントアウトするか削除して下さい。

|||
--|--
**blur_ps.hlsl**|**main(...)**

```cpp
・・・省略・・・
//const float gaussian_sigma = 1.0;
・・・省略・・・
//const float bloom_intensity = 1.0;
・・・省略・・・
```

---

最後にこの値を ImGui で操作できるようにしましょう。

|||
--|--
**framework.cpp**|**framework::update(...)**

```cpp
    ・・・省略・・・
    ImGui::SliderFloat("luminance_extraction_higher_edge", &parametric_constants.luminance_extraction_higher_edge, +0.0f, +5.0f);
*   ImGui::SliderFloat("gaussian_sigma", &parametric_constants.gaussian_sigma, +0.0f, +10.0f);
*   ImGui::SliderFloat("bloom_intensity", &parametric_constants.bloom_intensity, +0.0f, +10.0f);

    ImGui::End();
    ・・・省略・・・
```

@import "images/ガウスとブルームを調整.png"

---

## 7 リニア色空間で処理を行う
### 7-1 テクスチャからサンプリングした色情報に逆ガンマ補正を施す
### 7-2 sprite_ps.hlsl を変更する

sprite_ps.hlsl の以下の部分を

```cpp
Texture2D color_texture : register(t0);
```

以下のように変更して下さい。

||
--|--
**sprite_ps.hlsl**|

```cpp
Texture2D color_map : register(t0);
```

---

続いて sprite_ps.hlsl の main() の以下の部分を

```
return color_texture.Sample(point_sampler_state, pin.texcoord) * pin.color;
```

以下のように変更して下さい。

|||
--|--
**sprite_ps.hlsl**|**main(...)**

```cpp
    float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
    float alpha = color.a;

#if 1
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
#endif

    return float4(color.rgb, alpha) * pin.color;
```

---

### 7-3 skinned_mesh_ps.hlsl を変更する

|||
--|--
**skinned_mesh_ps.hlsl**|**main(...)**

```cpp
        ・・・省略・・・
        float alpha = color.a;

*   #if 1
*       // Inverse gamma process
*       const float GAMMA = 2.2;
*       color.rgb = pow(color.rgb, GAMMA);
*   #endif

        float3 N = normalize(pin.world_normal.xyz);
        ・・・省略・・・
```

---

### 7-3 blur_ps.hlsl を変更し画面への最終出力値にガンマ補正とトーンマッピングを施す

|||
--|--
**blur_ps.hlsl**|**main(...)**

```cpp
        ・・・省略・・・
        color.rgb += blur_color * bloom_intensity;

*   #if 1
*       // Tone mapping : HDR -> SDR
*       const float exposure = 1.2;
*       color.rgb = 1 - exp(-color.rgb * exposure);
*   #endif

*   #if 1
*       // Gamma process
*       const float GAMMA = 2.2;
*       color.rgb = pow(color.rgb, 1.0 / GAMMA);
*   #endif

        return float4(color.rgb, alpha);
```

---

### 7-4 実行し、事項結果を確認する

@import "images/リニア空間.png"

---

exposure の値を変更し変化を確認する(定数バッファを定義して CPU から制御する)

blue_ps.hlsl の PARAMETRIC_CONSTANT_BUFFER 構造体に exposure と dummy2 を追加

|||
--|--
**blur_ps.hlsl**|**PARAMETRIC_CONSTANT_BUFFER**

```cpp
    ・・・省略・・・
    float exposure;
    float3 dummy2;
```

blur_ps.hlsl の main() の exposure が利用されている以下の部分を

```
const float exposure = 1.2;
```

コメントアウトする

|||
--|--
**blur_ps.hlsl**|**main(...)**

```cpp
//const float exposure = 1.2;
```

---

framework クラスの定数バッファ parametric_constants に exposure と dummy2 を追加

|||
--|--
**framework.h**|**framework::parametric_constants**

```cpp
・・・省略・・・
float exposure{ 1.0f };
DirectX::XMFLOAT3 dummy2;
```

---

ImGUI で利用できるようにしましょう。

|||
--|--
**framework.cpp**|**framework::update(...)**

```cpp
    ・・・省略・・・
    ImGui::SliderFloat("bloom_intensity", &parametric_constants.bloom_intensity, +0.0f, +10.0f);
*   ImGui::SliderFloat("exposure", &parametric_constants.exposure, +0.0f, +10.0f);
    ImGui::End();
    ・・・省略・・・
```

---

## 8 画面の上下左右の端にアーティファクトノイズが出る、原因を考察し解決しなさい
※サンプラーステートの D3D11_TEXTURE_ADDRESS_WRAP が原因、D3D11_TEXTURE_ADDRESS_BORDER に変更（BorderColor 値に注意せよ）

framework クラスのサンプラーステートの以下の部分を

```cpp
*   // 0...ポイントステート	1...リニアステート	2...アニソトロピックステート
*   Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[3];
```

以下のように変更して下さい。

|||
--|--
**framework.h**|**framework**

```cpp
*   // 0...ポイントステート	1...リニアステート	2...アニソトロピックステート 3...リニアボーダーブラック 4...リニアボーダーホワイト
*   Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[5];
```

---

frmework::initialize でサンプラーステートを新規追加します。

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    hr = device->CreateSamplerState(&sampler_desc, sampler_states[2].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // リニアボーダーブラック
*   sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
*   sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
*   sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
*   sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
*   sampler_desc.BorderColor[0] = 0;
*   sampler_desc.BorderColor[1] = 0;
*   sampler_desc.BorderColor[2] = 0;
*   sampler_desc.BorderColor[3] = 0;
*   hr = device->CreateSamplerState(&sampler_desc, sampler_states[3].GetAddressOf());
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    ・・・省略・・・
*   // リニアボーダーホワイト
*   sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
*   sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
*   sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
*   sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
*   sampler_desc.BorderColor[0] = 1;
*   sampler_desc.BorderColor[1] = 1;
*   sampler_desc.BorderColor[2] = 1;
*   sampler_desc.BorderColor[3] = 1;
*   hr = device->CreateSamplerState(&sampler_desc, sampler_states[4].GetAddressOf());
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // 深度ステンシルステートオブジェクトの作成
    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
    ・・・省略・・・
```

---

生成したサンプラーステートをシェーダー側に追加します。

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    immediate_context->PSSetSamplers(2, 1, sampler_states[2].GetAddressOf());
*   immediate_context->PSSetSamplers(3, 1, sampler_states[3].GetAddressOf());
*   immediate_context->PSSetSamplers(4, 1, sampler_states[4].GetAddressOf());

    immediate_context->OMSetBlendState(blend_states[1].Get(), nullptr, 0xFFFFFFFF);
    ・・・省略・・・
```

---

オフスクリーン描画用のスプライトである fullscreen_quad_ps.hlsl 側で追加したサンプラーステートを利用できるように設定しましょう。

fullscreen_quad_ps.hlsl の以下の部分を

```cpp
SamplerState sampler_states[3] : register(s0);
```

以下のように変更しましょう。

||
--|--
**fullscreen_quad_ps.hlsl**|

```cpp
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
SamplerState sampler_states[5] : register(s0);
```

---

作成したアーティファクトノイズ解消用のサンプラーステートを利用してテクスチャから色情報を取り出しましょう。

fullscreen_quad_ps.hlsl の main() 関数の以下の部分を

```cpp
return texture_map.Sample(sampler_states[LINEAR], pin.texcoord);
```

以下のように変更しましょう。

|||
--|--
**fullscreen_quad_ps.hlsl**|**main(...)**

```cpp
return texture_map.Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
```

起動して確認すると上下左右の端のアーティファクトノイズが無くなっています。

---

## 9 実行時中 VisualStudio の出力ウィンドウにシェーダーの警告(D3D11 WARNING) がでる、原因を考察し解決しなさい
※フレームバッファは同時にレンダーターゲットビューおよびシェーダーリソースビューとしてシェーダーにバインドすることはできない
※下記コードを framework クラスの render メンバ関数の先頭に挿入する

現状起動すると出力画面に以下の警告が常に表示されています。

@import "images/フレームバッファの警告.png"

これはレンダーターゲットビューとシェーダーリソースビューとしてシェーダーに同時にバインドすることが出来ないという警告です。
この警告を解消する為に以下のコードを framework の描画の始めに呼び出しておいてください。

|||
--|--
**framework.cpp**|**framework::render(...)**

```cpp
    HRESULT hr{ S_OK };

*   // フレームバッファはレンダーターゲットビューおよびシェーダーリソースビューとしてシェーダーに
*   // 同時にバインドすることが出来ない警告の解決
*   ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
*   immediate_context->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
*   ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
*   immediate_context->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
*   immediate_context->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);

    FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
```

起動すると警告が無くなっています。