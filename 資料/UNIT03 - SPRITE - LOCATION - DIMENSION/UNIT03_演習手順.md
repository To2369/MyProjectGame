<!--
UNIT03_EXERCISE UNIT03 演習手順
SPRITE - LOCATION/DIMENSION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;">

Affine transformation
Screen coordinates
Transformation from screen coordinates to NDC
-->


# UNIT03 SPRITE - LOCATION/DIMENSION 演習手順

---

学習した **アフィン変換** で **スクリーン座標** から **NDC座標** への変換を行ってスプライトの座標を変換し、初期位置からの移動や色の変更を行ってみましょう。

ですがただスプライトのコンストラクタに設定している頂点データの座標を変更しても意味がありません。

#### なぜか？

位置や色をスプライトクラスのコンストラクタに設定しているだけでは、リアルタイムな変更ができません。これでは初期位置が変更するだけで移動したり、色が移り変わったりを見ることはできません。

#### ではどうすればよいか？

作成したスプライトの位置や色の変更を、リアルタイムに行うには描画部分で編集します。

描画部分でデバイスコンテキストを介して GPU から現在の頂点情報を取得し、取得した現在の頂点情報に対して変更を行うという方法をとります。

それではやっていきましょう。

---

### <span style="color:#334488;">アクセスフラグの変更</span>

    演習手順１

しかし、描画部分で頂点情報を取得し変更しようとしても怒られてしまいます。

#### なぜか？

前回(UNIT02)頂点バッファを作成する際にアクセスフラグに対して以下の設定を行いました。

    「CPU 側からはアクセス出来ないように、アクセスは GPU 側からのみ」
    
つまり

    「ソースコード側(cpp)からは変更したらダメ、頂点シェーダー側(.hlsl)でなら変更していいよ」
    
という設定を行ったからです。

まずはこの部分をどちらからでもアクセス可能で、なおかつ読み書きが可能であるように設定を変更しましょう。

sprite のコンストラクタ部分の以下の２か所

    // バッファの読み取りと書き込みの方法を特定。GPU からしかアクセスできないよう設定
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;

    // CPU へアクセスする際のフラグ。CPU のアクセスを使ってないのでここでは０
    buffer_desc.CPUAccessFlags = 0;

この２つを下記のように変更してください。


|ファイル|コンストラクタ|
--|--
**sprite.cpp**|**sprite::sprite(...)**

```cpp
    ・・・省略・・・
    // バッファの読み取りと書き込みの方法を特定。GPU CPU ともにアクセス可能
*   buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    ・・・省略・・・
    // CPU へアクセスする際のフラグ。CPU 側から読み書き可能
*   buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ・・・省略・・・
```

これで描画処理部分から取り出した頂点バッファに対して変更を加える事が可能になりました。

---

    演習手順２

スプライトごとに変更を加えれるように sprite の描画関数である **render メンバ関数** の引数を増やし framework 側から変更出来るようにしましょう。

---

### <span style="color:#334488;">スクリーン座標系からNDC 座標系に変換</span>

    演習手順３－１

描画処理中の頂点バッファを取り出し頂点情報に変更が加えられる用意ができたので、実際に頂点位置の変更を行いましょう。

**render メンバ関数**に追加した引数を NDC 座標に変換するためにはスクリーンのサイズが必要になります。

スクリーンのサイズはビューポートが保持しているのでデバイスコンテキストを介してビューポートを取得します。

|ファイル|メンバ関数|
--|--
**sprite.cpp**|**sprite::render(ID3D11DeviceContext\* immediate_context, ...)**

```cpp

*   D3D11_VIEWPORT viewport{};  // ビューポート情報
*   UINT num_viewports{ 1 };    // ビューポートの数
*   immediate_context->RSGetViewports(&num_viewports, &viewport);

    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    ・・・省略・・・
```

---

    演習手順３－２

**render メンバ関数**で受け取った値を計算しやすいように４点に設定しています。

|ファイル|メンバ関数|
--|--
**sprite.cpp**|**sprite::render(ID3D11DeviceContext\* immediate_context, ...)**

```cpp
    ・・・省略・・・
    immediate_context->RSGetViewports(&num_viewports, &viewport);

*   // (x0, y0) *----* (x1, y1)
*   //          |   /|
*   //          |  / |
*   //          | /  |
*   //          |/   |
*   // (x2, y2) *----* (x3, y3)

*   // left-top
*   float x0(dx);
*   float y0(dy);
*   // right-top
*   float x1(dx + dw);
*   float y1(dy);
*   // left-bottom
*   float x2(dx);
*   float y2(dy + dh);
*   // right-bottom
*   float x3(dx + dw);
*   float y3(dy + dh);

    UINT stride{ sizeof(vertex) };
    ・・・省略・・・
```

---


    演習手順３－３

上記で設定した４点を頂点バッファに設定する為に、スクリーン座標系からNDC 座標系に変換しています。

|ファイル|メンバ関数|
--|--
**sprite.cpp**|**sprite::render(ID3D11DeviceContext\* immediate_context, ...)**

```cpp
    ・・・省略・・・
    // right-bottom
    float x3(dx + dw);
    float y3(dy + dh);

*   // スクリーン座標系から NDC 座標系に変換
*   x0 = 2.0f * x0 / viewport.Width - 1.0f;
*   y0 = 1.0f - 2.0f * y0 / viewport.Height;
*   x1 = 2.0f * x1 / viewport.Width - 1.0f;
*   y1 = 1.0f - 2.0f * y1 / viewport.Height;
*   x2 = 2.0f * x2 / viewport.Width - 1.0f;
*   y2 = 1.0f - 2.0f * y2 / viewport.Height;
*   x3 = 2.0f * x3 / viewport.Width - 1.0f;
*   y3 = 1.0f - 2.0f * y3 / viewport.Height;

    UINT stride{ sizeof(vertex) };
    ・・・省略・・・
```

---

    演習手順３－４

デバイスコンテキストから頂点情報を取得し、上記で計算した座標に変更してみましょう。

|ファイル|メンバ関数|
--|--
**sprite.cpp**|**sprite::render(ID3D11DeviceContext\* immediate_context, ...)**

```cpp
    ・・・省略・・・
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

*   HRESULT hr{ S_OK };

*   // サブリソースである頂点情報を受け取るための構造体
*   D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
*   // 生成した頂点バッファと頂点情報が入っている受け取り用のサブリソースを設定し、
*   // 書き込み権限を指定して呼び出す
*   hr = immediate_context->Map(
*       vertex_buffer, 0,
*       D3D11_MAP_WRITE_DISCARD,    // 書き込み権限を指定
*       0, &mapped_subresource);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // サブリソースから現在の頂点情報を取り出す
*   vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
*   // 取り出した頂点情報に対して変更を加えていく
*   if (vertices != nullptr) {
*       // 座標を編集
*       vertices[0].position = { x0, y0, 0 };
*       vertices[1].position = { x1, y1, 0 };
*       vertices[2].position = { x2, y2, 0 };
*       vertices[3].position = { x3, y3, 0 };
*       // 色を編集(現状白で固定している状態)
*       vertices[0].color = vertices[1].color = 
*       vertices[2].color = vertices[3].color = { 1, 1, 1, 1 };
*   }

*   // 頂点バッファの変更を反映する
*   immediate_context->Unmap(vertex_buffer, 0);

    UINT stride{ sizeof(vertex) };
    ・・・省略・・・
```

重要なのは頂点バッファの編集開始の部分である **Map メンバ関数** と

    immediate_context->Map(...);

頂点バッファの編集の終了の部分である **Unmap メンバ関数**

    immediate_context->Unmap(...);

です。
編集の終了のタイミングで編集した頂点情報の変更を GPU 側に反映するという処理も行っています。

---

    演習手順４

頂点情報を動的に変更できるようにしたので UNIT03.pdf にあるように framework::render メンバ関数の実引数を変更してみましょう。

---

    演習手順５

UNIT02 の演習課題が終了した状態だと sprite_ps.hlsl が

```hlsl
#include "sprite.hlsli"
float4 main(VS_OUT pin) : SV_TARGET
{
    const float2 center = float2(1280 / 2, 720 / 2);
    float distance = length(center - pin.position.xy);
    if (distance > 200) return 1;
    else return float4(1, 0, 0, 1);
}
```

になっているので下記に戻したうえで確認してくださいという事です。

```hlsl
#include "sprite.hlsli"
float4 main(VS_OUT pin) : SV_TARGET
{
    return pin.color;
}
```


---

    演習手順６

演習手順３－４で色もリアルタイムに編集できるように実装しましたが、現状白に固定、つまりテクスチャの色そのままに固定されています。これを外部から変更できるようにしてみましょう。

**sprite クラス** の **render メンバ関数** に引数を追加して render 内部のコードを少し変更すれば出来ると思うのでやってみて下さい。

追加するのは **sprite::render** の以下の部分です。

|ファイル|クラス|
--|--
**sprite.h**|**sprite**

```cpp
    void sprite::render(ID3D11DeviceContext* immediate_context,
        float dx, float dy, float dw, float dh,
*       float r, float g, float b, float a);
```