<!--
UNIT01_EXERCISE UNIT01 学習要項
INITIALIZATION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Completion
-->

# UNIT01 INITIALIZATION 学習要項

この資料は主に **UNITXX.pdf** の「**学習要項**」の部分にある項目の説明をしています。
**UNITXX.pdf** を進める前にこちらの説明を一通り確認し**演習手順**を進めてください。
**演習手順**を進める途中で**学習要項**で出てきた言葉でわからない事などが出てきた場合にも利用してください。

---

### <span style="color:#334488;">描画エンジンの授業フォルダ</span>

描画エンジンI・II のフォルダは以下にあります。

```
\\\\nt22\配布\GAME_CREATIVE\永瀬\描画エンジン I・II _GR2GB\
```

---

### <span style="color:#334488;">授業教材のフォルダ</span>

授業のメイン教材である pdf は上記サーバーの以下のフォルダにあります。
毎授業、利用する番号をコピーしてきておいてください。

```
Documents\
```

使用するのは **.pdf** です。**UNIT01** から前期後期併せて **UNI32** まであります。
前期は **UNIT16** までを予定しています。

<span style="color:#994433;font-size:100%;font-weight: bold;">この pdf 資料が授業の資料になります。評価などはこの資料に記載されている「評価項目」を元に評価を行うので覚えておいてください。</span>

---

### <span style="color:#334488;">描画エンジンのプロジェクト</span>

授業で利用するプロジェクトになります。

```
x3dgp.00.zip
```

これ１つだけです。
このプロジェクトにコードを追加していきます。

---

### <span style="color:#334488;">DirectXTK-master.zip</span>

授業で画像を読み込む際に利用するフレームワークです。

```
DirectXTK-master.zip
```

途中でプロジェクトに追加します。

---

### <span style="color:#334488;">授業の参考資料</span>

授業の参考資料として

```
参考資料\
```

簡単なヒントが記載されいます。メイン教材だけではわからない場合は参考資料にある **.pptx** を開いて確認してください。 基本的には **.pdf** と **.pptx** の２つで進めていければと思います。

---

### <span style="color:#334488;">その他</span>

その他は前期では利用しませんが、様子を見て先の授業教材を入れていこうと思っています。
そのうえで後期必要になる。フレームワークが以下の３つです。

* cereal-master.zip
    JSON を解析する為のフレームワーク
* fbx20202_fbxsdk_vs2019pdbs.exe
    FBX を読み込むためのフレームワーク
* fbx20202_fbxsdk_vs2019win.exe
    FBX を読み込むためのフレームワーク

---

### <span style="color:#334488;">Direct3D 11</span>

##### <span style="color:#222266;">◇コンピュータ上で３ⅮⅭＧを表示させるためのＡＰＩのこと。</span>

##### <span style="color:#222266;">◇Microsoft製のゲームや製造処理用のＡＰＩで、様々なプラットフォームやデバイスで利用されています。</span>

##### <span style="color:#222266;">◇DirectXにはバージョンがあり、バージョンが上がるごとに表現できる事が増えていきます。</span>

## DirectXの世代について

@import "images/DirectXの世代について.png"

---

### <span style="color:#334488;">COM (Component Object Model)</span>


マイクロソフトが提唱するソフトウェアの再利用を目的とした技術であり、オブジェクト指向をベースとしたコンポーネント指向による開発のための基礎的な技術でもあります。

COM は、オブジェクトの公開方法や、オブジェクト自身の寿命の管理方法などを定義しています。つまり delete などの定義を自身で管理するクラスが用意されており、それらの基底クラスを継承して作られています。

UNIT01 で利用する以下の５つは COM オブジェクトにあたります。

    ID3D11Device* device;
    ID3D11DeviceContext* immediate_context;
    IDXGISwapChain* swap_chain;
    ID3D11RenderTargetView* render_target_view;
    ID3D11DepthStencilView* depth_stencil_view;

そのほかにも

    ID3D11Texture2D

などが UNIT01 では利用されています。

##### ◇ インターフェース設計

インターフェースとは C++ で説明すると純粋仮想関数のみで作られたクラスの事です。規定となるインターフェースであるクラスを継承したクラス群で形作られたプロジェクト設計のことをインターフェース設計と呼びます。

##### ◇ Release

実装するうえで重要な点としては、**COM オブジェクト**には **delete** は利用できないという事です。
仮にデバイスを破棄しようとする際に以下のようにした場合、エラーになります。

```cpp
    delete device;
```

**COM オブジェクト**にはすべて **Release** と呼ばれるメンバ関数が用意されていてこのメンバ関数を利用して以下のように解放を行います。

```cpp
    device->Release();
```

delete が利用できないので以下のようなスマートポインターも利用できないので注意してください。以下は終了時にエラーになります。

```cpp
    shared_ptr<ID3D11Device> device
```

**COM オブジェクト**には専用のものが用意されているのでそちらを使いましょう。そちらは UNIT10 であらためて紹介します。

---

### <span style="color:#334488;">Device and Device context</span>

#### ◇ デバイス

デバイスとは DirectX で利用する様々なリソースを扱える機能です。
DirectX の頂点にある機能でもあります。

@import "images/ID3d11Device.png"

DirectX ではデバイスから必要なものを作成し、そこからさらに必要なものを作成していくという作りになります。

#### ◇ デバイスコンテキスト

デバイスコンテキストとは、描画命令を GPU に伝える機能です。
描画周りの処理を呼び出す際は必ずデバイスコンテキストを利用します。

@import "images/ID3D11DeviceContext.png"

#### ◇ CPU GPU

単純な PC の処理能力は CPU である中央処理装置が行います。

GPUとはグラフィックボードに搭載されている画像処理装置です。アイコン表示から、映像再生などに機能しています。高画質機能が必要となる3Dグラフィックなどは、特にGPUの処理能力の大きさがポイントとなります。

---

### <span style="color:#334488;">Textures and Data Resource Formats</span>

テクスチャデータはbmpやpngなどの画像のことで、様々なフォーマットが使用可能です。これらのテクスチャをメモリ上に読み込んで使用します。
また、**レンダーターゲット**に描き込んでいる画像もテクスチャデータとして使用することができます。

#### ◇ レンダーターゲット

レンダリングパイプラインから出力されるピクセル情報を保存する。
レンダーターゲットを書き込むビューをレンダーターゲットビューと呼びます。

    ID3D11RenderTargetView

わかりやすく説明すると、色を書き込むためのキャンバスの事です。

@import "images/レンダーターゲット01.png"

---

### <span style="color:#334488;">The Swap Chain and Page Flipping</span>
#### <span style="color:#334488;">スワップチェーン</span>

    IDXGISwapChain

@import "images/DirectX11初期化03.png"

@import "images/スワップチェーン.png"

---

### <span style="color:#334488;">Depth Buffering</span>
#### <span style="color:#334488;">深度バッファ(Zバッファ)</span>
**深度バッファ**とはピクセルに出力された奥行を保存する領域です。このバッファの値と新しく出力されるピクセルの奥行を比較します。深度バッファは**デプスバッファ**、**Zバッファ**とも呼ばれています。

@import "images/深度バッファ.png"

#### ◇ 深度テスト
深度テストは描画されたピクセルが現在描画されているピクセルよりも手前 or 奥かどうかを判断し、その結果が出力するかを決めます。

#### ◇ ステンシルテスト
ステンシルテストとはステンシルバッファと呼ばれるバッファの値とピクセルを描画する際に設定した参照値を比較して描画するかどうかを判定します。

@import "images/ステンシルテスト.png"

### ◇ 深度ステンシルビュー

Depth と Stencil の二つの機能を持った View のことを深度ステンシルビューと呼びます。

    ID3D11DepthStencilView

---

### <span style="color:#334488;">Texture Resource Views</span>

テクスチャをレンダリングターゲットおよびシェーダーリソースとして使用することをテクスチャリソースビューと呼びます。

---

### <span style="color:#334488;">Multisampling Theory</span>
#### <span style="color:#334488;">マルチサンプリング</span>

**マルチサンプル アンチエイリアシング**とも呼ばれ、エッジを滑らかに描画するために使用されるグラフィックス技法。

@import "images/アンチエイリアシング.png"

---

### <span style="color:#334488;">Feature Levels</span>
#### <span style="color:#334488;">フューチャーレベル</span>

FeatureLevelはドライバの機能レベルを指しており、初期化時にレベルを指定します。
例えばDirectX11の機能を求める場合、D3D_FEATURE_LEVEL_11_1やD3D_FEATURE_LEVEL_11_0を指定するとドライバがそのレベルの処理を行えるかどうかが分かります。

---

### <span style="color:#334488;">Viewport</span>
#### <span style="color:#334488;">ビューポート</span>

@import "images/RSSetViewports.png"

---

### <span style="color:#334488;">Assertions</span>
#### <span style="color:#334488;">アサーション</span>

_ASSERTマクロ群。

コード上にでてくる

    _ASSERT_EXPR(...)
    
などのアサート系のマクロ群の事。

_ASSERTマクロを使う理由は「致命的である事を知らせる」そして「パフォーマンスの犠牲を避ける」ためです。

また、アサート系のマクロはデバッグ時にしか処理されません。逆に言えば「リリースモードの時は_ASSERTマクロは『無いもの』とされます」。