<!--
UNIT09_EXERCISE UNIT09 演習手順
SPRITE - BATCHING

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;">

Optimaizations
Batching
Primitive topology
-->

# UNIT09 SPRITE - BATCHING 演習手順

---

#### <span style="color:#334488;">スプライトバッチクラスの作成</span>

**sprite クラス** と **sprite_batch クラス** を明確に分離します。

背景のように一　枚一枚が別の画像になるような物は **sprite** で表示し、敵やアイテムのような複数表示が必要ではあってもレンダーステートなどの描画設定などは使いまわすようなものは **sprite_batch** で表示するというように使い分けるようにしましょう。

    演習手順１

手順にあるように **sprite_batch** 用のヘッダファイルとソースファイルを新規で作成して下さい。

    演習手順２
    演習手順３

手順にあるように **sprite** のヘッダの内容はヘッダに、ソースの内容はソースにコピーしてください。

---

    演習手順４－１

**sprite_batch** 側のクラス名を **sprite_batch** に変更しましょう。

**sprite_batch.h** の下記の部分を
```cpp
class sprite
```
以下のように変更しましょう。

||
--|--
sprite_batch.h|

```cpp
    ・・・省略・・・
*   class sprite_batch
    {
        ・・・省略・・・
```

---

    演習手順４－２

**sprite_batch クラス** のコンストラクタ名と引数を修正しましょう。

**sprite_batch.h** の以下のコンストラクタのコードを
```
sprite(ID3D11Device *device, const wchar_t* filename);
```
以下のように変更しましょう。

||
--|--
sprite_batch.h|
```cpp
        ・・・省略・・・
    public:
*       sprite_batch(ID3D11Device *device, const wchar_t* filename, size_t max_sprites);
        ・・・省略・・・
```


**max_sprites** は画面に表示できるスプライトの最大数です。
一括で **Draw** を呼び出す必要があるのでスプライトの数だけ頂点数を表示する必要があります。
なので頂点数は表示するスプライトによって変化します。
ただ、頂点数を無駄に最大にしておくのは逆に重くなるので、ある程度の最大数を設定しておけるようにしておくことで、その数だけポリゴンを予め生成しておきます。


---

    演習手順４－３

**sprite_batch クラス** のデストラクタ名を修正しましょう。

**sprite_batch.h** の以下のデストラクタのコードを

```
virtual ~sprite();
```
以下のように変更しましょう。

||
--|--
sprite_batch.h|
```cpp
    ・・・省略・・・
    sprite_batch(ID3D11Device *device, const wchar_t* filename, size_t max_sprites);
*   virtual ~sprite_batch();
    ・・・省略・・・
```

デストラクタの定義も変更しましょう。
```
sprite::~sprite()
```
以下のように変更しましょう。
||
--|--
sprite_batch.cpp|
```cpp

    ・・・省略・・・
*   sprite_batch::~sprite_batch()
    {
        ・・・省略・・・
```

---

#### <span style="color:#334488;">最大スプライト数分の頂点データを保管するための準備/span>

    演習手順４－４

コンストラクタで受け取った最大スプライト数から生成する頂点数を保存しておくための変数と頂点数分生成した頂点データを保存しておく為の変数を用意します。

|||
--|--
sprite_batch.h|sprite_batch

```cpp
            ・・・省略・・・
            DirectX::XMFLOAT2 texcoord;
        };
        
*   private:
*       const size_t max_vertices;
*       std::vector<vertex> vertices;

    public:
        sprite_batch(ID3D11Device *device, const wchar_t* filename, size_t max_sprites);
        ・・・省略・・・
```

---

#### <span style="color:#334488;">一括で描画するスプライトに一括で描画の設定を指定する準備</span>

    演習手順４ー５

* シェーダーの設定とテクスチャの設定
* 頂点データを作成
* 変更した頂点データの編集と一括描画

今まではこの３つが **render メンバ関数** で行われていましたが、これらを分離し３つのメンバ関数でそれぞれ処理致します。

|説明|メンバ関数名|
--|--
シェーダーの設定とテクスチャの設定|begin
頂点データを作成|render
変更した頂点データの編集と一括描画|end

まずはメンバ関数を２つ追加しましょう。

|||
--|--
sprite_batch.h|sprite_batch

```cpp
        ・・・省略・・・
        void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh);

*       void begin(ID3D11DeviceContext* immediate_context);
*       void end(ID3D11DeviceContext* immediate_context);
    }
```

#### <span style="color:#334488;">コンストラクタを修正</span>

    演習手順５－１

インクルード部分が **"sprite.h"** になっているので **"sprite_batch.h"** に変更しましょう。

**sprite_batch.cpp** のコンストラクタの定義部分を修正していきましょう。

```
sprite::sprite(ID3D11Device* device, const wchar_t* filename)
```
以下のように変更しましょう。
||
--|--
sprite_batch.cpp|
```cpp
    ・・・省略・・・
    #include <WICTextureLoader.h>

*   sprite_batch::sprite_batch(ID3D11Device* device, const wchar_t* filename, 
        size_t max_sprites) : max_vertices(max_sprites * 6)
    {
        ・・・省略・・・
```

変更したらコンストラクタの中身も変更していきます。頂点データは描画時に編集するのでコンストラクタで作成する必要はありません。ただ、生成するスプライト数によって頂点の数が変化し、生成する頂点データの数も変わります。

また、生成した頂点データをシェーダーに渡す際に下記のように subresource_data.pSysMem に配列の先頭アドレス設定していたのですが、

    subresource_data.pSysMem = vertices;

配列ではなく **std::unique_ptr<vertex[ ]>** に なったのでスマートポインタとして設定されている先頭のアドレスを渡す必要があります。つまり、

    subresource_data.pSysMem = vertices.get();

となります。それではやってみましょう。

|||
--|--
sprite_batch.cpp|sprite_batch::sprite_batch(...)

```cpp
    HRESULT hr{ S_OK };
 
*   //vertex vertices[]
*   //{
*   // { { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },
*   // { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },
*   // { { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },
*   // { { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },
*   //};
*   std::unique_ptr<vertex[]> vertices{ std::make_unique<vertex[]>(max_vertices) };
 
    D3D11_BUFFER_DESC buffer_desc{};
*   buffer_desc.ByteWidth = sizeof(vertex) * max_vertices;
    ・・・省略・・・
*   subresource_data.pSysMem = vertices.get();
    ・・・省略・・・
```

---

#### <span style="color:#334488;">render メンバ関数の変更</span>

    演習手順５－２

ここでは頂点バッファの編集のみを行います。

|||
--|--
sprite_batch.cpp|sprite_batch(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)

```cpp

    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

*   // テクスチャ座標の設定
*   float u0{ sx / texture2d_desc.Width };
*   float v0{ sy / texture2d_desc.Height };
*   float u1{ (sx + sw) / texture2d_desc.Width };
*   float v1{ (sy + sh) / texture2d_desc.Height };
    
*   // 頂点データの設定
*   vertices.push_back({ { x0, y0 , 0 }, { r, g, b, a }, { u0, v0 } });
*   vertices.push_back({ { x1, y1 , 0 }, { r, g, b, a }, { u1, v0 } });
*   vertices.push_back({ { x2, y2 , 0 }, { r, g, b, a }, { u0, v1 } });
*   vertices.push_back({ { x2, y2 , 0 }, { r, g, b, a }, { u0, v1 } });
*   vertices.push_back({ { x1, y1 , 0 }, { r, g, b, a }, { u1, v0 } });
*   vertices.push_back({ { x3, y3 , 0 }, { r, g, b, a }, { u1, v1 } });
}


```

---

#### <span style="color:#334488;">描画設定処理</span>

    演習手順５－３

**begin メンバ関数**の定義を実装しましょう。

**sprite クラス** の **render メンバ関数**から持ってきて変更しても問題ありませんし、下のコードをそのまま書いてもらってもかまいません。

||
--|--
sprite_batch.cpp|
```cpp
    ・・・省略(上にデストラクタの定義)・・・

*   void sprite_batch::begin(ID3D11DeviceContext* immediate_context)
*   {
*       vertices.clear();
*       immediate_context->VSSetShader(vertex_shader, nullptr, 0);
*       immediate_context->PSSetShader(pixel_shader, nullptr, 0);
*       immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);
*   }
```

---

#### <span style="color:#334488;">描画の実行</span>

    演習手順５－４

**end メンバ関数**の定義を実装しましょう。

**sprite クラス** の **render メンバ関数**から持ってきて変更しても問題ありませんし、下のコードをそのまま書いてもらってもかまいません。

||
--|--
sprite_batch.cpp|
```cpp
    ・・・省略(上に begin メンバ関数の定義)・・・

*   void sprite_batch::end(ID3D11DeviceContext* immediate_context)
*   {
*       HRESULT hr{ S_OK };
*       D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
*       hr = immediate_context->Map(vertex_buffer,
             0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
*       _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        
*       size_t vertex_count = vertices.size();
*       _ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
*        vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
*       if (data != nullptr)
*       {
*           const vertex* p = &(vertices.at(0));
*           memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
*       }
*       immediate_context->Unmap(vertex_buffer, 0);

*       UINT stride{ sizeof(vertex) };
*       UINT offset{ 0 };
*       immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
*       immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
*       immediate_context->IASetInputLayout(input_layout);

*       immediate_context->Draw(static_cast<UINT>(vertex_count), 0);
*   }
```

重要なのは **sprite_batch** の **render メンバ関数** で編集した頂点データの配列をこのメンバ関数の **mapped_subresource** として設定しているという下記の部分。

    vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (data != nullptr)
    {
        const vertex* p = &(vertices.at(0));
        memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
    }

それと、**プリミティブトポロジー**を **TRIANGLESTRIP** から **TRIANGLELIST** に変更した下記の点。

    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

この２点は押さえておいて下さい。

---

#### <span style="color:#334488;">パフォーマンステスト</span>

    演習手順６－１

実装した **sprite_batch** クラスを **framework** クラスに宣言しましょう。

|||
--|--
framework.h|framework
```cpp
    ・・・省略・・・

    sprite* sprites[8];
	
*   sprite_batch* sprite_batches[8];

    ID3D11SamplerState* sampler_states[3];
    ・・・省略・・・
```

---

#### <span style="color:#334488;">スプライトバッチのオブジェクトを生成</span>

    演習手順６－２

|||
--|--
framework.cpp|framework::initialize()
```cpp
    ・・・省略・・・
    sprites[1] = new sprite(device, L".\\resources\\player-sprites.png");

*   sprite_batches[0] = new sprite_batch(device, L".\\resources\\player-sprites.png", 2048);

    return true;
}
```

---

#### <span style="color:#334488;">フレームワークの描画関数にテストコードを実装</span>

    演習手順６－３

前回利用していた **render メンバ関数**をコメントアウトしてください。

|||
--|--
framework.cpp|framework::render(...)
```cpp
        ・・・省略・・・
*       //sprites[0]->render(immediate_context, 0, 0, 1280, 720, 1, 1, 1, 1, 0);
*       //sprites[1]->render(immediate_context, 500, 200, 200, 200, 1, 1, 1, 1, 0, 0, 0, 140, 240);

*       float x{ 0 };
*       float y{ 0 };
*   #if 0
*       for (size_t i = 0; i < 1092; ++i)
*       {
*           sprites[1]->render(immediate_context, x, static_cast<float>(static_cast<int>(y) % 720), 64, 64, 1, 1, 1, 1, 0, 140 * 0, 240 * 0, 140, 240);
*           x += 32;
*           if (x > 1280 - 64)
*           {
*               x = 0;
*               y += 24;
*           }
*       }
*   #else
*       sprite_batches[0]->begin(immediate_context);
*       for (size_t i = 0; i < 1092; ++i)
*       {
*           sprite_batches[0]->render(immediate_context, x, static_cast<float>(static_cast<int>(y) % 720), 64, 64, 1, 1, 1, 1, 0, 140 * 0, 240 * 0, 140, 240);
*           x += 32;
*           if (x > 1280 - 64)
*           {
*               x = 0;
*               y += 24;
*           }
*       }
*       sprite_batches[0]->end(immediate_context);
*   #endif

    #ifdef USE_IMGUI
        ImGui::Render();
        ・・・省略・・・
```

---

#### <span style="color:#334488;">最適化処理</span>

    演習手順７

演習手順にある通り**sprite_batch** の **render メンバ関数** 内で利用されている**ラムダ式**である **rotate** がある程度処理を重くしてしまっている原因になっています。

これを軽くするにはまずは

* 関数内での三角関数の処理を外部に移す
* インライン関数にする

この２点を行うことで処理が軽くなると記載されています。

**関数内での三角関数の処理を外部に移す** とは、

    四角形を形作るスプライトの角度が９０度であれば４点ともに９０度だから、
    １点１点内部で sin cos の計算を毎回する必要はないよね。
    
ということです。つまり **rotate ラムダ式内** で行っている

    float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
    float sin{ sinf(DirectX::XMConvertToRadians(angle)) };

この計算を外部で行って、結果だけを rotate ラムダ式の引数に渡してしまえば計算は１回で済むということです。

もう１つは、**インライン関数** にするという事は、現状同じ関数を４回呼び出している事自体が時間かかっているので**インライン関数**にしてコードを埋め込んでしまえば呼び出す時間が０になるので早くなるよね、ということです。

それではやっていきましょう。

**sprite_batch** の下記の部分をコメントアウトしておきましょう。

```
std::function<void(float&, float&, float, float, float)> rotate = [](float& x, float& y, float cx, float cy, float angle)
{
    x -= cx;
    y -= cy;

    float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
    float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
    float tx{ x }, ty{ y };
    x = cos * tx + -sin * ty;
    y = sin * tx + cos * ty;

    x += cx;
    y += cy;
};
```

**sprite_batch** クラスに **rotate メンバ関数**を追加しましょう。
引数部分を外部で計算した sin と cos の結果が受け取れるような形に変更しているので、注意してください。

|||
--|--
sprite_batch.h|sprite_batch
```cpp
    private:
*       static void rotate(float& x, float& y, float cx, float cy, float sin, float cos)
*       {
*           x -= cx;
*           y -= cy;

*           float tx{ x }, ty{ y };
*           x = cos * tx + -sin * ty;
*           y = sin * tx + cos * ty;

*           x += cx;
*           y += cy;
*       }
```

**sprite_batch クラス** にメンバ関数の定義を記述することでこのメンバ関数は自動で **インラインメンバ関数** になりコンパイル時に **rotate(...)** と呼び出された部分にマクロのようにコードが埋め込まれます。

それでは **rotate メンバ関数** の呼び出し部分を多少修正しましょう。
sprite_batch の **render メンバ関数** の下記の部分を
```
float cx = dx + dw * 0.5f;
float cy = dy + dh * 0.5f;
rotate(x0, y0, cx, cy, angle);
rotate(x1, y1, cx, cy, angle);
rotate(x2, y2, cx, cy, angle);
rotate(x3, y3, cx, cy, angle);
```
以下のように変更しましょう。

|||
--|--
sprite_batch.cpp|sprite_batch::render(...)
```cpp
*   float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
*   float sin{ sinf(DirectX::XMConvertToRadians(angle)) };

    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
*   rotate(x0, y0, cx, cy, sin, cos);
*   rotate(x1, y1, cx, cy, sin, cos);
*   rotate(x2, y2, cx, cy, sin, cos);
*   rotate(x3, y3, cx, cy, sin, cos);
```

これでラムダ式での処理を最適化することが出来ました。

<span style="color:#994433;font-size:80%;font-weight: bold;">TIPS:ラムダ式というものが処理が重いわけではありません。</span>
