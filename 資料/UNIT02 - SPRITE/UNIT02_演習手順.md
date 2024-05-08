<!--
    UNIT02 SPRITE 演習手順.md

    <div style="text-align: center;"></div>

    <span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

    <a href="https://github.com/KhronosGroup/Vulkan-ValidationLayers" class="Link--external">オープンソース</a>

    * [検証レイヤーとは](#01_)
    ## <span style="color:#334488;">検証レイヤーとは</span><a name="01_"></a>
-->


# <div style="text-align: center;"><span style="color:#334499; font-size:100%; font-weight: bold; ">UNIT02 SPRITE 演習手順</span></div>

---

ここではスプライトの描画の説明を致します。

前回は描画を行うために必要な基本的なリソースを５つ作成しました。
プログラム的にいうと**デバイスコンテキスト**が持っている **Draw メンバ関数** を呼び出す為に以下のリソースを作成しました。

    // DirectX11 で利用する様々なリソース(もの)を作成するデバイス
    ID3D11Device* device;
    // 様々な描画命令を GPU に伝えるデバイスコンテキスト
    ID3D11DeviceContext* immediate_context;
    // キャンバスに描いた画を額(ウィンドウ)にいれるスワップチェイン
    IDXGISwapChain* swap_chain;
    // 色を書き込むキャンバスであるレンダーターゲットビュー
    ID3D11RenderTargetView* render_target_view;
    // 奥行き情報を書き込むキャンバス
    ID3D11DepthStencilView* depth_stencil_view;

前回作ったこれらを**レンダリングパイプライン**に従って、**ポリゴン**を作成し、**テクスチャ**を貼り付けて**スプライト**として描画致します。

---

UNIT02 の実行結果は以下になります。

@import "images/UNIT02_01.png"

---

### <span style="color:#334488;">HLSL ヘッダーファイル</span>

演習手順１と演習手順２－１はそのままの意味なので飛ばして、

    演習手順２－２

外部ファイルであるシェーダーファイルを作成していきます。
新規で HLSL の HLSL ヘッダーファイルを選択して

||
--|--
sprite.hlsli|

```hlsl
*   struct VS_OUT
*   {
*       float4 position : SV_POSITION;
*       float4 color : COLOR;
*   };
```

今回は位置と色を利用するので**float4**の**position**と**float4**の**color**を定義し、それぞれ、セマンティクスに**SV_POSITION**、**COLOR**を指定しました。

---

### <span style="color:#334488;">頂点シェーダーファイルの作成</span>

    演習手順２－３

つづいて頂点シェーダーを実装いたします。
新規でシェーダーファイルを作り、先ほど作った HLSL ヘッダーファイルをインクルードしてください。

||
--|--
sprite_vs.hlsl|

```hlsl
*   #include "sprite.hlsli"
*   VS_OUT main(float4 position : POSITION, float4 color : COLOR)
*   {
*       VS_OUT vout;
*       vout.position = position;
*       vout.color = color;
*       return vout;
*   }
```

頂点シェーダが呼び出されるとコンパイルされたこのファイルが呼び出され、**main 関数**が呼び出されることになります。
この**main 関数**に設定されている引数の**position**や**color**に対しての値の設定を後で行っていくことになります。

ここでの実装は後で設定する位置と色がそのまま**vout**に設定されることになります。
**vout**とは最終的な頂点データになります。
なので**cpp側**で設定された位置や色がそのまま頂点の位置と色になって頂点が決定される、という設定です。

---

### <span style="color:#334488;">ピクセルシェーダーファイルの作成</span>

    演習手順２－４

つづいてピクセルシェーダーを実装いたします。
こちらも頂点シェーダと同じように新規でシェーダーファイルを作り、先ほど作った HLSL ヘッダーファイルをインクルードしてください。

||
--|--
sprite_ps.hlsl|

```hlsl
*   #include "sprite.hlsli"
*   float4 main(VS_OUT pin) : SV_TARGET
*   {
*       return pin.color;
*   }
```

頂点シェーダーで頂点が決定したあとラスタライザによりピクセル化が行われます。ここではそのピクセルの１つ１つが引数として取得されます。

コードでは受け取った色を編集することなくそのまま返しています。

後で説明しますが、ピクセルシェーダーが Draw 処理の最終的な処理となるので、ここで返される色がピクセルの最終的な色として表示されることになります。

---

    演習手順２－５から２－８

シェーダーファイルのコンパイル後のシェーダーのデバッグファイルの設定を行っています。

---

    演習手順３

シェーダーファイルの実装が完了したらさっそく描画の実装に移りましょう。
ポリゴンでを描画するのに必要な、

    頂点データ
    入力レイアウト
    頂点シェーダー
    ピクセルシェーダー
    描画ステート

このうち描画ステートはここではデフォルトを使用するので

    頂点データ
    入力レイアウト
    頂点シェーダー
    ピクセルシェーダー

この４つを用意し、描画を行うためのメンバ関数、そして上記で記載した頂点シェーダーの main() 関数の引数に渡すための頂点フォーマットと呼ばれる構造体。これらを用意したスプライトクラスを定義しましょう。
UNIT02.pdf の演習手順３にあるようにヘッダファイルを新規で作成し sprite.h としましょう。

||
--|--
sprite.h|

```cpp
*   #include <d3d11.h>
*   #include <directxmath.h>

*   class sprite
*   {
*   private:
*       // 頂点シェーダー
*       ID3D11VertexShader* vertex_shader;
*       // ピクセルシェーダー
*       ID3D11PixelShader* pixel_shader;
*       // 入力レイアウト
*       ID3D11InputLayout* input_layout;
*       // 頂点データ
*       ID3D11Buffer* vertex_buffer;

*   public:
*       // 頂点フォーマット
*       struct vertex
*       {
*           DirectX::XMFLOAT3 position;
*           DirectX::XMFLOAT4 color;
*       };

*       sprite(ID3D11Device *device);
*       virtual ~sprite();

*       // 描画
*       void render(ID3D11DeviceContext* immediate_context);
*   };
```

インクルードした **directxmath.h** は数学計算系の関数やクラスが詰まったファイルで **XMFLOAT3** などを利用する際に必要となります。

シェーダー側では **float4** であった頂点フォーマットに利用されている **position** ですが、こちらでは**FLOAT3** となっている点に注意してください。この場合シェーダー側に値が渡される際は w には０が代入されます。

頂点シェーダ、ピクセルシェーダ、入力レイアウト、頂点データをコンストラクタで生成しますが、これらの生成にはデバイスが必須な為、コンストラクタの引数で外部からデバイスを受け取れるようにしています。 

描画も同じような理由で、描画処理にはデバイスコンテキストが必須なので外部からデバイスコンテキストを **render 関数** で受け取り、**sprite** 内部でも描画ができるような設計にしています。

---

### <span style="color:#334488;">頂点バッファ・頂点シェーダー・ピクセルシェーダー・入力レイアウトの生成</span>

    演習手順４

**sprite** のクラスの定義が完了したら必要な項目を、デバイスを使ってそれぞれ生成していきましょう。

新規でソースファイル **sprite.cpp** を作成してください。

#### 演習手順４の依存するインクルードファイルの記載を忘れないように！！

||
--|--
sprite.cpp|

```cpp
*   #include "sprite.h"
*   #include "misc.h"
*   #include <sstream>
```

必要なファイルをインクルードしたらコンストラクタ部分で必要な項目を生成していきましょう。

---

### <span style="color:#334488;">頂点データ</span>

まずは頂点データを設定します。
実際のポリゴンをかたち作る点をどこに打つのかという情報ですね。
頂点データは頂点フォーマットの型によって設定する値が変化しますが、今回は**position**(位置)と**color**(色)です。

    演習手順の４－１

|||
--|--
sprite.cpp|sprite::sprite(ID3D11Device* device)|

```cpp
*   // 頂点情報のセット
*   vertex vertices[]
*   {
*       { { -0.5, +0.5, 0 }, { 1, 1, 1, 1 } },
*       { { +0.5, +0.5, 0 }, { 1, 0, 0, 1 } },
*       { { -0.5, -0.5, 0 }, { 0, 1, 0, 1 } },
*       { { +0.5, -0.5, 0 }, { 0, 0, 1, 1 } },
*   };
```

**sprite**クラスで定義した頂点フォーマットにしたがって位置を色の設定を行いました。ここでの頂点の位置はスクリーン座標ではなく**NDC**にしたがって設定致してください。

@import "images/NDC.png"

この座標系で設定した頂点位置に設定した色でうたれると考えてください。
つまり vertices の要素０番目の

    { -0.5, +0.5, 0 }, { 1, 1, 1, 1 } },

であれば、スクリーン座標に変換すると x, y = 320, 320 の位置に色は白としてうたれる、ということになります。

---

### <span style="color:#334488;">頂点バッファオブジェクトの作成</span>

上記で作成した頂点データを頂点シェーダーに引き渡すためには頂点バッファに vertices のアドレスを渡す必要があります。

    演習手順４－２

|||
--|--
sprite.cpp|sprite::sprite(ID3D11Device* device)|

```cpp
        ・・・省略・・・
        { { -0.5, -0.5, 0 }, { 0, 1, 0, 1 } },
        { { +0.5, -0.5, 0 }, { 0, 0, 1, 1 } },
    };

*   HRESULT hr{ S_OK };

*   // 頂点バッファオブジェクトの設定を行うための構造体
*   D3D11_BUFFER_DESC buffer_desc{};
*   // 頂点情報のサイズを指定
*   buffer_desc.ByteWidth = sizeof(vertices);
*   // バッファの読み取りと書き込みの方法を特定。GPU からしかアクセスできないよう設定
*   buffer_desc.Usage = D3D11_USAGE_DEFAULT;
*   // バッファがパイプラインにバインドされる方法を特定
*   // このバッファオブジェクトを頂点バッファとして設定
*   buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
*   // CPU へアクセスする際のフラグ。CPU のアクセスを使ってないのでここでは０
*   buffer_desc.CPUAccessFlags = 0;
*   // その他のフラグ。不要な場合は０
*   buffer_desc.MiscFlags = 0;
*   // バッファが構造化バッファを表す場合のバッファ構造内の各要素のサイズ。
*   buffer_desc.StructureByteStride = 0;

*   // サブリソースデータ
*   D3D11_SUBRESOURCE_DATA subresource_data{};
*   // 初期化データへのポインタ
*   // 頂点情報を設定
*   subresource_data.pSysMem = vertices;
*   // テクスチャの１行の先頭から次の行までの距離(バイト単位)。
*   subresource_data.SysMemPitch = 0;
*   // ある深度レベルの開始から次の深度レベルまでの距離(バイト単位)
*   subresource_data.SysMemSlicePitch = 0;
    
*   // デバイスを使って、頂点バッファのサブリソースとして頂点情報を設定して頂点バッファを生成
*   hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
```

問題がなければ内部で生成された頂点バッファのインスタンスのアドレスが **vertex_buffer** に代入されます。

**D3D11_BUFFER_DESC**は BindFlags に入れるフラグによって扱うデータも変化します。
**D3D11_BIND_VERTEX_BUFFER**を代入しているので頂点バッファとなり頂点情報を取り扱えるようになります。

<span style="color:#994433;font-size:80%;font-weight: bold;">
TIPS：D3D11_BUFFER_DESC は後々出てくる特殊なバッファにも使用しますが、フラグが変化する事で D3D11_BUFFER_DESC の設定の形がガラッと変わってしまうので注意しておこう。<br><br></span>

つまり、今回はフラグに対して

    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

として、サブリソースデータに頂点データを設定し

    subresource_data.pSysMem = vertices;

vertex_buffer (頂点バッファ) を生成致しました。

    hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);

vertex_buffer には頂点情報が設定されている状態になっているので、演習手順５－１で、この vertex_buffer をデバイスコンテキストに渡してやることで、頂点シェーダーが呼び出された際の main() 関数の引数に、上記で設定した配列 vertices の値が渡されることになります。

---

### <span style="color:#334488;">頂点シェーダーオブジェクトの生成</span>

    演習手順４－３

演習手順２－３で作成した頂点シェーダーファイルである sprite_vs.hlsl のコンパイルされたファイルである sprite_vs.cso をここで読み込み、cso ファイルに記載されている文字列を取り出し、デバイスを使って頂点シェーダーとして生成します。

|||
--|--
sprite.cpp|sprite::sprite(ID3D11Device* device)
```cpp
    ・・・省略・・・

    // デバイスを使って、頂点バッファのサブリソースとして頂点情報を設定して頂点バッファを生成
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   {
*       const char* cso_name{ "sprite_vs.cso" };

*       // sprite_vs.cso ファイルを開く
*       FILE* fp{};
*       fopen_s(&fp, cso_name, "rb");
*       _ASSERT_EXPR_A(fp, "CSO File not found");

*       // ファイルのサイズを調べて
*       fseek(fp, 0, SEEK_END);
*       long cso_sz{ ftell(fp) };
*       fseek(fp, 0, SEEK_SET);

*       // ファイル内の文字を cso_data に文字列として取り出し、ファイルを閉じる
*       std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
*       fread(cso_data.get(), cso_sz, 1, fp);
*       fclose(fp);

*       HRESULT hr{ S_OK };
*       hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &vertex_shader);
*       _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
*   }
```

演習手順には記載されていませんがこの部分はブロックで囲っておきましょう。 
**HRESULT hr** が再度定義されているのと、この後の演習手順４－３でも同じような手順で実装してもらうことになるので変数の重複を避けるためにブロックで区切る事をおすすめ致します。
つまり後々楽になるのでブロックで区切っておきましょう。

ここではコンパイルされた頂点シェーダーファイルを文字列として **cso_data** に格納し、その頂点シェーダーが記載された文字列を使って頂点シェーダーオブジェクトの生成を行っています。

デバイスから CreateVertexShader を呼び出しています。問題がなければ内部で生成された頂点シェーダーオブジェクトのインスタンスのアドレスが vertex_shader に代入されます。

---

### <span style="color:#334488;">入力レイアウトオブジェクトの生成</span>

    演習手順４－４

次は入力レイアウトオブジェクトの生成を行いましょう。頂点シェーダーに渡す頂点情報の詳細を定義します。
演習手順４－１、４－２で頂点バッファを生成することで頂点の情報である位置と色の情報をシェーダー側に送る準備は整いました。

    { -0.5, +0.5, 0 }, { 1, 1, 1, 1 }
    { +0.5, +0.5, 0 }, { 1, 0, 0, 1 }
    { -0.5, -0.5, 0 }, { 0, 1, 0, 1 }
    { +0.5, -0.5, 0 }, { 0, 0, 1, 1 }

学習要項の方で説明しましたが、GPU にこれを送っても XMFLOAT3 と XMFLOAT4 と認識してもらえず、
    
    -0.5, +0.5, 0, 1, 1, 1, 1, +0.5, +0.5, 0, 1, 0, 0, 1 -0.5, -0.5, 0, 0, 1, 0, 1, +0.5, -0.5, 0, ...

といった敷居の無い単なる数値の羅列として認識されエラーとなります。
この数値の羅列が、どのようなデータで、どこに敷居があり、その敷居をどのように呼ぶかなどを GPU が理解できるよう設定しましょう。

|||
--|--
script.cpp|sprite::sprite(ID3D11Device* device)

```cpp
        ・・・省略・・・
        hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &vertex_shader);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*       // 入力レイアウトオブジェクトの設定を行うための構造体
*       D3D11_INPUT_ELEMENT_DESC input_element_desc[]
*       {
*           { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
*               D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*           { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
*               D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*       };

*       // 入力レイアウトオブジェクトを生成
*       hr = device->CreateInputLayout(
*           input_element_desc,             // 入力レイアウトの設定を行うための構造体 
*           _countof(input_element_desc),   // 構造体の配列の数
*           cso_data.get(),                 // コンパイルされた頂点シェーダー文字列
*           cso_sz,                         // コンパイルされた頂点シェーダーのサイズ
*           &input_layout                   // 成功時の入力レイアウトオブジェクト
*       );
*       _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
*   }
```

問題がなければ内部で生成された入力レイアウトオブジェクトのインスタンスのアドレスが input_layout に代入されます。

D3D11_INPUT_ELEMENT_DESC において重要なのは、

１つ目の名称が **"POSITION"** であり型が **DXGI_FORMAT_R32G32B32_FLOAT** であることと、
２つ目の名称が **"COLOR"** であり **DXGI_FORMAT_R32G32B32A32_FLOAT** であること。

です。

名称の部分は頂点シェーダーファイルの **main 関数**の引数のセマンティクス部分に設定した名称と同じ名前に設定しなければならないので注意して下さい。

    VS_OUT main(float4 position : POSITION, float4 color : COLOR)

この部分に詳細設定が反映されます。

次に **DXGI_FORMAT_R32G32B32_FLOAT** と **DXGI_FORMAT_R32G32B32A32_FLOAT** の部分。

頂点フォーマットで設定したのは位置が XMFLOAT3 で色が XMFLOAT4 でした。
位置の方は **R32 G32 B32** の **FLOAT**。
色の方は **R32 G32 B32 A32** の **FLOAT**。
これで単なる数値の羅列に敷居を作られシェーダー側も下記のように値を受け取る事ができるようになりました。

    { -0.5, +0.5, 0 }, { 1, 1, 1, 1 }
    { +0.5, +0.5, 0 }, { 1, 0, 0, 1 }
    { -0.5, -0.5, 0 }, { 0, 1, 0, 1 }
    { +0.5, -0.5, 0 }, { 0, 0, 1, 1 }

<span style="color:#994433;font-size:80%;font-weight: bold;">TIPS：DXGI_FORMAT_ の部分はこのフラグの名称みたいなものなのであまり意味はありません。</span>

<span style="color:#334499;font-size:80%;font-weight: bold;">POINT：その他の設定項目も各自調べておきましょう。</span>

---

### <span style="color:#334488;">ピクセルシェーダーオブジェクトの生成</span>

    演習手順４－５

|||
--|--
sprite.cpp|sprite::sprite(ID3D11Device* device)

```cpp
            ・・・省略・・・
            &input_layout
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
*   {
*       // ピクセルシェーダオブジェクトの生成
*       const char* cso_name{ "sprite_ps.cso" };

*       FILE* fp{};
*       fopen_s(&fp, cso_name, "rb");
*       _ASSERT_EXPR_A(fp, "SCO File not found");

*       fseek(fp, 0, SEEK_END);
*       long cso_sz{ ftell(fp) };	// コンパイルされた頂点シェーダのサイズ
*       fseek(fp, 0, SEEK_SET);

*       std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
*       fread(cso_data.get(), cso_sz, 1, fp);
*       fclose(fp);

*       HRESULT hr{ S_OK };

*       hr = device->CreatePixelShader(
*           cso_data.get(),
*           cso_sz,
*           nullptr,
*           &pixel_shader	// 生成されたピクセルシェーダ
*       );

*       _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
*   }
```

---

### <span style="color:#334488;">スプライトの描画</span>

描画に必要なすべてのオブジェクトの準備が完了したら描画処理を実装をしましょう。

ここではこれまでにコンストラクタで作ったオブジェクトをデバイスコンテキストを介して GPU に渡していくという作業を行い、最後に Draw() を呼び出します。

    演習手順５－１

まずは頂点情報を持っている頂点バッファ情報を GPU に渡します。

|||
--|--
sprite.cpp|sprite::render(ID3D11DeviceContext* immediate_context)

```cpp
*   UINT stride{ sizeof(vertex) };
*   UINT offset{ 0 };
*   // 頂点バッファを設定
*   immediate_context->IASetVertexBuffers(
*       0,                  // 頂点バッファの開始スロット。
*       1,                  // 配列内の頂点バッファの数。
*       &vertex_buffer,     // 頂点バッファ
*       &stride,            // その頂点バッファーから使用される要素のサイズ
*       &offset             // 頂点バッファの最初の要素と使用される最初の要素の間のバイト数です
*   );
```

IASetVertexBuffers の IA は Draw が呼ばれた後に処理される順番をさします。

    Input Assembler (IA)
        頂点情報やインデックス情報の入力
    Vertex Shader (VS)
        頂点シェーダ
    Hull Shader (HS)
        ハルシェーダー、パッチをどう分割するかを設定
    Tesselator (TS)
        テッセレータ、パッチを分割
    Domain Shader (DS)
        生成頂点を設定
    Geometry Shader (GS)
        頂点を増やしたりする
    Rasterizer (RS)
        頂点をピクセル化
    Pixel Shader (PS)
        テクスチャなどを参照
    Output Merger (OM)
        レンダーターゲットや深度への出力

今回は IA と VS と PS を利用するので、今回は

    IA VS RS PS 出力

という順番で処理が行われます。

---

    演習手順５－２

|||
--|--
sprite.cpp|sprite::render(ID3D11DeviceContext* immediate_context)

```cpp
        ・・・省略・・・
        &offset             // 頂点バッファの最初の要素と使用される最初の要素の間のバイト数です
    );

*   // プリミティブトポロジーの設定
*   immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
```

頂点情報で設定した点をどのように結ぶかの設定です。
D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP を設定しているので、0 1 2 1 2 3 の順番で三角形ポリゴンが形成されます。

この番号は頂点データに設定した配列のインデックス番号をさします。
頂点データは下記のように設定していました。

```cpp
    vertex vertices[]
    {
0       { { -0.5, +0.5, 0 }, { 1, 1, 1, 1 } },
1       { { +0.5, +0.5, 0 }, { 1, 0, 0, 1 } },
2       { { -0.5, -0.5, 0 }, { 0, 1, 0, 1 } },
3       { { +0.5, -0.5, 0 }, { 0, 0, 1, 1 } },
    };
```

これをプリミティブトポロジーに設定したフラグの順番通りに三角形を作ると

```cpp
0   { -0.5, +0.5, 0 }, { 1, 1, 1, 1 }
1   { +0.5, +0.5, 0 }, { 1, 0, 0, 1 }
2   { -0.5, -0.5, 0 }, { 0, 1, 0, 1 }

1   { +0.5, +0.5, 0 }, { 1, 0, 0, 1 }
2   { -0.5, -0.5, 0 }, { 0, 1, 0, 1 }
3   { +0.5, -0.5, 0 }, { 0, 0, 1, 1 }
```

という２つの三角形が形成され、今回はこの２つの三角形を使って四角形のポリゴンを描画します。

---

    演習手順５－３
    演習手順５－４

生成した入力レイアウトオブジェクト、頂点シェーダーオブジェクト、ピクセルシェーダーオブジェクトを設定しています。

|||
--|--
sprite.cpp|sprite::render(ID3D11DeviceContext* immediate_context)

```cpp
    ・・・省略・・・

    // プリミティブトポロジーの設定
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
*   // 入力レイアウトオブジェクトの設定
*   immediate_context->IASetInputLayout(input_layout);

*   // 頂点シェーダーオブジェクトの設定
*   immediate_context->VSSetShader(vertex_shader, nullptr, 0);
*   // ピクセルシェーダーオブジェクトの設定
*   immediate_context->PSSetShader(pixel_shader, nullptr, 0);
```

IA VS PS の処理の順番は Draw() が呼ばれた際に内部で決まっているのでここに記載する順番は気にする必要はありません。

---

    演習手順５－５

|||
--|--
sprite.cpp|sprite::render(ID3D11DeviceContext* immediate_context)

```cpp
    ・・・省略・・・
    // ピクセルシェーダーオブジェクトの設定
    immediate_context->PSSetShader(pixel_shader, nullptr, 0);

*   // 描画処理
*   immediate_context->Draw(4, 0);
```

生成したオブジェクトをデバイスコンテキストを介して GPU に渡し Draw() を呼び出すことで、IA VS RS PS 出力 の順番で処理が実行され、ウィンドウに表示が行われます。

---

### COM オブジェクトの解放

コンストラクタで生成した４つの COM オブジェクトをデストラクタで解放しておきましょう。

    頂点バッファ
    入力レイアウト
    頂点シェーダー
    ピクセルシェーダー

---

### sprite の変数を宣言

実装した sprite を利用しましょう。

    演習手順６

framework のメンバ変数として宣言。

|||
--|--
framework.h|framework

```cpp
    ・・・省略・・・
    ID3D11DepthStencilView* depth_stencil_view;

*   // スプライト
*   sprite* sprites[8];

    framework(HWND hwnd);
    ・・・省略・・・
```

配列にしているのは後々複数のスプライトを表示するので配列にしています。

---

    演習手順７

宣言した sprite の生成を行います。

|||
--|--
framework.cpp|framework::initialize()

```cpp
    ・・・省略・・・
    viewport.MaxDepth = 1.0f;
    immediate_context->RSSetViewports(1, &viewport);

*   sprites[0] = new sprite(device);

    return true;
```

今回生成するのは１つだけなので配列の０番に生成しています。
生成時に sprite のコンストラクタにデバイスを渡しています。

---

    演習手順８

生成したので忘れないように破棄も記載しておきましょう。

|||
--|--
framework.cpp|framework::uninitialize()

```cpp
    ・・・省略・・・
    depth_stencil_view->Release();

*   for (sprite* p : sprites) delete p;

    return true;
```

---

    演習手順９

描画処理部分です。

|||
--|--
framework.cpp|framework::render(float elapsed_time)

```cpp
    ・・・省略・・・
    immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

*   sprites[0]->render(immediate_context);

    #ifdef USE_IMGUI
    ・・・省略・・・
```

生成したスプライトの０番の描画を呼び出しています。
描画処理には必ずデバイスコンテキストが必要になることに注目してください。

---

    演習手順１０

実行して確認してみましょう。

---

演習手順１１、１２、１３は確認や修正作業になるので割愛します。

    