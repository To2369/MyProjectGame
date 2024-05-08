<!--
UNIT01_EXERCISE UNIT01 演習手順
INITIALIZATION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Completion
-->

# UNIT01 INITIALIZATION 演習手順

---

この資料は主に **UNITXX.pdf** の進め方の説明をしています。
可能なら **UNITXX.pdf** だけで演習を進んでいけるのが一番力がつきます。
ただ、昨今はコロナなどで授業がオンラインになりがちであり授業を休んでしまいがちになるため、対面での説明がおろそかになりがちです。その際にこの資料を呼んで先に進んでいければと思っております。

---

それでは **UNIT01.pdf** をやっていきましょう。
演習手順１と演習手順２は割愛します。

演習手順の **UNIT01.pdf** の下の演習手順１４と１５を見ると

* MSDN で COM オブジェクト、メソッド、関数、構造体などを調べること
* コメントを書くこと

と記載されています。

これは描画エンジンだけに限った事ではなく、これからのすべてのプログラムにおいて行うようにしてください。調べて、調べたことをコメントで記載する。たったこれだけの事でプログラムの理解度が上がっていきます。

ちなみに今回の実行結果画面は以下の画像になります。
@import "images/実行結果.png"s
<span style="font-size:12px; color:#bb1111">左上の ImGUI は表示されません</span>

---

    演習手順３

DirectX を利用するためのファイルをインクルードします。
これがなければ DirectX が使えません。

||
--|--
framework.h|
```cpp
    ・・・省略・・・
    extern ImWchar glyphRangesnese[];
    #endif

*   #include <d3d11.h>

    CONST LONG SCREEN_WIDTH{ 1280 };
    CONST LONG SCREEN_HEIGHT{ 720 };
    ・・・省略・・・
```

<span style="color:#994433;font-size:80%;font-weight: bold;">注 : *(アスタリスク) のついている部分が追加、編集の対象です。UNITXX.pdf では、コードのすべてが追加の場合は *(アスタリスク) を付けていないのでよく確認して実装していくようにして下さい。
</span>

---

### <span style="color:#334488;">リソースの宣言</span>

次に DirectX11 の初期化に必要な最低限の変数を framework クラスのメンバ変数として定義します。

DirectX11 の初期化に必要なものとは・・・

#### <span style="color:#222299;">・デバイス</span>

#### <span style="color:#222299;">・デバイスコンテキスト</span>

#### <span style="color:#222299;">・スワップチェイン</span>

#### <span style="color:#222299;">・レンダーターゲットビュー</span>

#### <span style="color:#222299;">・デプスターゲットビュー</span>

の５つです。
まずはこれらを定義しましょう。

```
演習手順４
```

|ファイル|クラス|
--|--
framework.h|framework

```cpp
    ・・・省略・・・

    CONST HWND hwnd;

*   // DirectX11 で利用する様々なリソース(もの)を作成するやつ
*   ID3D11Device* device;
*   // 様々な描画命令を GPU に伝えるやつ
*   ID3D11DeviceContext* immediate_context;
*   // キャンバスに描いた画を額(ウィンドウ)にいれるやつ
*   IDXGISwapChain* swap_chain;
*   // 色を書き込むキャンバス
*   ID3D11RenderTargetView* render_target_view;
*   // 奥行き情報を書き込むキャンバス
*   ID3D11DepthStencilView* depth_stencil_view;

    framework(HWND hwnd);
    ~framework(); 

    ・・・省略・・・
```

ここで定義したメンバ変数はすべて COM オブジェクトと呼ばれるものです。

---

### <span style="color:#334488;">デバイス、デバイスコンテキスト、スワップチェーンの生成</span>

定義が終わったら、まず最初に

    ID3D11Device*        device;
    ID3D11DeviceContext* immediate_context;
    IDXGISwapChain*      swap_chain;

の作成から始めます。
デバイスを作らないことには何も作れませんし、デバイスコンテキストを作らないことには何も描画出来ませんし、スワップチェーンを作らないことには画面の更新を行えません。

    演習手順６の１

|ファイル|メンバ関数|
--|--
framework.cpp |framework::initialize()

```cpp

*       HRESULT hr{ S_OK };

*       UINT create_device_flags{ 0 };
*   #ifdef _DEBUG
*       create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
*   #endif

*       // Device, Context の能力レベルの設定
*       // DirectX11の基準を完全に満たしたGPU上で動作するレベルに設定
*       D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };

*       // IDXGISwapChain の設定を行うための構造体。SwapChain 作成するために必要な設定を行う
*       DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
*       // スワップチェインのバッファ数。交換するバッファの枚数
*       swap_chain_desc.BufferCount = 1;
*       // 解像度幅
*       swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;
*       // 解像度高さ
*       swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;
*       // 描画先の形式
*       swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
*       // リフレッシュレート。Denominator が分母
*       swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
*       swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
*       // バッファの使用方法の指定
*       swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
*       // ウィンドウハンドル
*       swap_chain_desc.OutputWindow = hwnd;
*       // バックバッファのマルチサンプリングに関する設定
*       // マルチサンプリングを使用しない場合は Count に１、Quality に０を指定。
*       // １ピクセルの色を決めるサンプリング数
*       swap_chain_desc.SampleDesc.Count = 1;
*       // サンプリングの品質、精度
*       swap_chain_desc.SampleDesc.Quality = 0;
*       // true...ウィンドウモード
*       swap_chain_desc.Windowed = !FULLSCREEN;

*       /// デバイス、デバイスコンテキスト、スワップチェーンを同時に生成
*       hr = D3D11CreateDeviceAndSwapChain(
*           NULL,                      // ビデオアダプタの指定(NULL は既定)
*           D3D_DRIVER_TYPE_HARDWARE,  // ドライバのタイプ
*           NULL,                      // D3D_DRIVER_TYPE_SOFTWARE 指定時に使用
*           create_device_flags,       // フラグ指定
*           &feature_levels,           // デバイスコンテキストの能力レベルを配列で指定
*           1,                         // 上記配列の要素数
*           D3D11_SDK_VERSION,         // SDKバージョン
*           &swap_chain_desc,          // スワップチェーンの詳細情報
*           &swap_chain,               // 関数成功時のスワップチェーンの出力先
*           &device,                   // 関数成功時のデバイスの出力先
*           NULL,                      // 成功した D3D_FEATURE_LEVEL の出力先
*           &immediate_context         // 関数成功時の Context の出力先
*       );
*       _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return true;
```

@import "images/CreateDeviceAndSwapChain.png"

**D3D11CreateDeviceAndSwapChain 関数**を利用してデバイス、デバイスコンテキスト、スワップチェーンの３つを同時に生成しました。
問題がなければ、

    device              デバイス
    immediate_context   デバイスコンテキスト
    swap_chain          スワップチェーン

このそれぞれのポインタ変数に対して、DirectX が内部で生成したインスタンスのアドレスが代入され、以降、これを利用してその他の生成や描画を行っていく事になります。

---

### <span style="color:#334488;">レンダーターゲットビューの生成</span>

    演習手順６の２

デバイスとデバイスコンテキストとスワップチェインの作成が出来たので
次は色を書き込むキャンバスである

    ID3D11RenderTargetView* render_target_view

の作成を行います。

|ファイル|メンバ関数|
--|--
framework.cpp|framework::initialize()

```cpp
        ・・・省略・・・
        &device,             // 関数成功時のデバイスの出力先
        NULL,                // 成功した D3D_FEATURE_LEVEL の出力先
        &immediate_context   // 関数成功時の Context の出力先
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   ID3D11Texture2D* back_buffer{};
*   hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // バックバッファを設定してレンダーターゲットビューを作成
*   hr = device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   back_buffer->Release();

    return true;
```

@import "images/レンダーターゲットビューの作成.png"

作成したスワップチェインを使ってバックバッファを取得。
取得したバックバッファを設定して**CreateRenderTargetView**を呼び出す。
問題がなければ内部で生成されたレンダーターゲットビューのインスタンスのアドレスが**render_target_view**に代入されます。

---

### <span style="color:#334488;">深度ステンシルビューの生成</span>

次は奥行き情報を書き込むキャンバスである

    ID3D11DepthStencilView* depth_stencil_view

の作成を行います。

    演習手順６－３
|||
--|--
framework.cpp|framework::initialize()

```cpp
    ・・・省略・・・

    back_buffer->Release();

*   // ID3D11Texture2D の設定を行うための構造体。ID3D11Texture2D に深度ステンシル用の設定を行う
*   ID3D11Texture2D* depth_stencil_buffer{};
*   D3D11_TEXTURE2D_DESC texture2d_desc{};
*   // テクスチャの幅(スクリーンの幅いっぱい)
*   texture2d_desc.Width = SCREEN_WIDTH;
*   // テクスチャの高さ(スクリーンの高さいっぱい)
*   texture2d_desc.Height = SCREEN_HEIGHT;
*   texture2d_desc.MipLevels = 1;
*   texture2d_desc.ArraySize = 1;
*   texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
*   texture2d_desc.SampleDesc.Count = 1;
*   texture2d_desc.SampleDesc.Quality = 0;
*   texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
*   // 深度ステンシルターゲットとしてバインド
*   texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
*   // CPU からの読み書きを不可
*   texture2d_desc.CPUAccessFlags = 0;
*   texture2d_desc.MiscFlags = 0;
*   hr = device->CreateTexture2D(&texture2d_desc, NULL, &depth_stencil_buffer);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   // ID3D11DepthStencilView の設定を行うための構造体。深度ステンシルビューを作成するために必要な設定を行う
*   D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
*   // リソースデータの形式
*   depth_stencil_view_desc.Format = texture2d_desc.Format;
*   // 深度ステンシルへのアクセス方法
*   depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
*   depth_stencil_view_desc.Texture2D.MipSlice = 0;
*   // リソースデータにアクセスするための深度ステンシルビューを作成
*   hr = device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depth_stencil_view);
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   depth_stencil_buffer->Release();

    return true;
```

**CreateDepthStencilView** を呼び出された際、問題がなければ内部で生成された深度ステンシルビューのインスタンスのアドレスが **depth_stencil_view** に代入されます。

深度ステンシルビューの説明で使われている奥行が説明したこの部分にあたる。

@import "images/深度ステンシルビュー.png"

---

### <span style="color:#334488;">ビューポートの設定</span>



必要な項目の生成が完了したら初期化の最後に画を描く場所であるビューポートの設定をコンテキストに対して行います。

    演習手順６－４

|||
--|--
framework.cpp|framework::initialize()

```cpp
    ・・・省略・・・

    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    depth_stencil_buffer->Release();

*   D3D11_VIEWPORT viewport{};
*   viewport.TopLeftX = 0;
*   viewport.TopLeftY = 0;
*   viewport.Width = static_cast<float>(SCREEN_WIDTH);
*   viewport.Height = static_cast<float>(SCREEN_HEIGHT);
*   viewport.MinDepth = 0.0f;
*   viewport.MaxDepth = 1.0f;
*   immediate_context->RSSetViewports(1, &viewport);

    return true;
```

RSSetViewports() は 第一引数にビューポートの数を設定しますが今回は１つなので１で、第二引数に使用するビューポートを設定致します。

ここまでで DirectX を利用するための初期化が完了致しました。

---

### <span style="color:#334488;">描画</span>

初期化が完了したので描画処理部分を実装してみましょう。

ただ描画するポリゴンなどは無いので画面に単色が表示されるだけとなります。 

@import "images/これから画を描くキャンバスを指定する.png"

    演習手順７

|||
--|--
framework.cpp|framework::render(...)

<span style="font-size:80%; color:#bb1111;font-weight: bold;">
注：ここで初めて UNIT01.pdf のコードの行数の左側にある *(アスタリスク) のマークがでてきます。その行のみ追加、もしくは変更点があるという印です。<br><br>
</span>

```cpp
*       HRESULT hr{ S_OK };

*       // 塗りつぶす色の設定
*       FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
*       // レンダーターゲットビューのクリア。キャンバス全体を指定した色に塗りつぶす。
*       // クリアするレンダーターゲットビューとクリアする色を指定
*       immediate_context->ClearRenderTargetView(render_target_view, color);
*       // 深度ステンシルビューのクリア。キャンバス全体の奥行情報をリセット。
*       // クリアする深度ステンシルビューとクリアする深度ステンシルのフラグ、深度バッファをクリアする値、ステンシルバッファをクリアする値
*       immediate_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
*       // これから描くキャンバスを指定する
*       immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

    #ifdef USE_IMGUI
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    #endif

*       UINT sync_interval{ 0 };
*       swap_chain->Present(sync_interval, 0);
```

**Present** が呼ばれる毎に **FrontBuffer** と **BuckBuffer** が切り替わって表示されます。

<span style="font-size:80%; color:#aa1111; font-weight: bold;">
TIPS : ImGUI は開発を楽にしてくれるツールです。演習手順の１３で ImGUI の有効化の設定を行います。<br><br>
</span>

ここではまだ**Draw**処理が呼び出されていないので確認できないのですが、描画された画は、そのままウィンドウに残ったままになるので描画の前には必ず**ClearRenderTargetView** と **ClearDepthStencilView** でキャンバスをクリアにしておく必要があります。

<!--
**Draw**が呼び出されることでバックバッファに画が書き込まれ**Present**が呼び出されることで
バッファが切り替わり、書き込まれたバッファがウィンドウに表示されます。

次のフレームは次のフレームで、次の画の書き込みが表示されていない方のバッファに書き込まれ・・・

2Dであれ3Dであれこの繰り返しを延々行うことでアニメーションしているように見せているというのが基本的な仕組みになります。
-->

---

### <span style="color:#334488;">ライブラリの読み込み</span>

    演習手順８

起動するための準備をします。

DirectX の関数の宣言やクラス定義は **d3d11.h** に記載されていますが、関数の定義やメンバ関数の定義は **d3d11.h** には記載されていません。

DirectX の関数の定義やメンバ関数の定義が書かれているファイルを読み込む作業が演習手順８の作業になります。

<span style="font-size:14px; color:#888888">TIPS : dxgi は DirectX グラフィックインフラストラクチャの略です。<br><br></span>

---

    演習手順９

実行してみましょう。指定した色を背景として

    FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };

@import "images/実行結果.png"s

上記の画面が表示されます。
<span style="font-size:12px; color:#bb1111">まだ左上の ImGUI は表示されません</span>

---

### <span style="color:#334488;">COM オブジェクトの解放</span>

    演習手順１０

これまで生成したオブジェクトの解放処理を行います。

|||
--|--
framework.cpp|framework::uninitialize()

```cpp
*   device->Release();
*   immediate_context->Release();
*   swap_chain->Release();
*   render_target_view->Release();
*   depth_stencil_view->Release();

    return true;
```

**COM オブジェクト**の解放は **delete** ではなく **Release** を使って解放を行います。**delete** での解放やスマートポインタを利用した解放を行うとエラーになるので注意してください。

---

### <span style="color:#334488;">スクリーンサイズの変更</span>

    演習手順１１

**SCREEN_WIDTH** と **SCREEN_HEIGHT** は **framework.h** に定義されているのでその部分の値を変更しましょう。

この部分を

    CONST LONG SCREEN_WIDTH{ 1280 };
    CONST LONG SCREEN_HEIGHT{ 720 };

以下ののように変更して確認してみましょう。

```cpp
*   CONST LONG SCREEN_WIDTH{ 1000 };
*   CONST LONG SCREEN_HEIGHT{ 200 };
```


--- 

### <span style="color:#334488;">フルスクリーン化</span>

    演習手順１２


**framework クラス**の **run() メンバ関数**の以下の部分の **#if 0** を

```
    ・・・省略・・・
#if 0
    BOOL fullscreen{};
    swap_chain->GetFullscreenState(&fullscreen, 0);
```

フラグを **１** にしてください。


|||
--|--
framework.h|framework::run()

```cpp
        ・・・省略・・・
*   #if 1
        BOOL fullscreen{};
        swap_chain->GetFullscreenState(&fullscreen, 0);
        if (fullscreen)
        ・・・省略・・・
```
    
---

### <span style="color:#334488;">ImGUI の表示</span>

```
 演習手順１３
```

ImGUI の設定です。フレームワークはすでに組み込まれているので、プロジェクトの設定のみを行っています。

設定が完了したら実行してください。実行すると画面のどこかに ImGUI のバーが表示されます。

---

### <span style="color:#334488;">コメントの追記</span>

    演習１４、１５

これから出てくる用語や単語はすべて調べて、調べた単語はすべてコメントとして記載していきましょう。

まずは 

    D3D11_VIEWPORT

構造体を調べてみましょう。

D3D11_VIEWPORT でググればまず MSDN のサイトが開くと思うのでそのサイトにブックマークをつけておきましょう。
詳細が英語になっていると思うので翻訳してもらい翻訳の内容をコメントとして記載しておきましょう。

    ビューポートの寸法を定義します。

とでています。ウィンドウのどこに画を書くのかという設定でした。
それの左側のX位置、上部のY位置を設定するということがわかりました。
その他のメンバ変数にもコメントをつけていきましょう。

```cpp
*   // ビューポートの設定を行うための構造体
    D3D11_VIEWPORT viewport{};
*   // ビューポートの左側の X 位置
    viewport.TopLeftX = 0;
*   // ビューポートの上側の Y 位置
    viewport.TopLeftY = 0;
*   // ビューポートの幅
    viewport.Width = static_cast<float>(SCREEN_WIDTH);
*   // ビューポートの高さ
    viewport.Height = static_cast<float>(SCREEN_HEIGHT);
*   // ビューポートの最少深度
    viewport.MinDepth = 0.0f;
*   // ビューポートの最大深度
    viewport.MaxDepth = 1.0f;
*   // ビューポートを設定
    immediate_context->RSSetViewports(1, &viewport);
```

ビューポートが終わったら

    swap_chain->GetBuffer(...)

や、深度ステンシルを生成する際の深度ステンシル用のテクスチャ周りの変数やメソッドも調べてコメントをつけていきましょう。