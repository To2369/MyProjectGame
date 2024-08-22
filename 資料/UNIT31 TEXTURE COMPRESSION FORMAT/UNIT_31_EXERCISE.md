<!--
UNIT31_EXERCISE UNIT31 演習手順
TEXTURE COMPRESSION FORMAT

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Texture compression format
Mipmap
DDS file
-->

# <span style="color:#444444;">UNIT31 演習手順</span>

## <span style="color:#334488;">TEXTURE COMPRESSION FORMAT</span>

---

* 目次
    1. texconv を使用して圧縮テクスチャを作成する
        1. resources フォルダにある texconv.exe と texconv.bat を nico.fbm フォルダにコピーする
        2. [texconv.bat をエディタで開いて内容を確認する](#1_2_)
        3. [上記コマンドの意味を下記サイトで確認する](#1_3_)
        4. [nico.fbm フォルダで texconv.bat をダブルクリックしてバッチを実行する](#1_4_)
    2. [DDS ファイルをロードしシェーダーリソースビューを生成する](#2_)
    3. [".\\resources\\nico.fbx"を skinned_mesh クラスのコンストラクタに指定し、実行する](#3_)

---

#### <span style="color:#334488;">1. texconv を使用して圧縮テクスチャを作成する</span>

---

#### <span style="color:#334488;">1 - 1. resources フォルダにある texconv.exe と texconv.bat を nico.fbm フォルダにコピーする</span>

---

#### <span style="color:#334488;">1 - 2. texconv.bat をエディタで開いて内容を確認する</span><a name="1_2_"></a>

```bat
.\texconv.exe -f BC7_UNORM -m 0 -y -ft dds *.tga
.\texconv.exe -f BC7_UNORM -m 0 -y -ft dds *.png

REM .\texconv.exe -f BC7_UNORM -w 512 -h 512 -m 1 -y -ft dds *.tga
REM .\texconv.exe -f BC7_UNORM -w 512 -h 512 -m 1 -y -ft dds *.png
```

---

#### <span style="color:#334488;">1 - 3. 上記コマンドの意味を下記サイトで確認する</span><a name="1_3_"></a>

https://github.com/microsoft/DirectXTex/wiki/Texconv

---

#### <span style="color:#334488;">1 - 4. nico.fbm フォルダで texconv.bat をダブルクリックしてバッチを実行する</span><a name="1_4_"></a>

#### コンバート実行前のフォルダ内容。

@import "images/コンバート前.png" {width=70%}

#### コンバート実行後のフォルダ内容

@import "images/コンバート後.png" {width=70%}

コンバート後はそれぞれのテクスチャと同名の .DDS ファイルが生成されます。

---

#### <span style="color:#334488;">2. DDS ファイルをロードしシェーダーリソースビューを生成する</span><a name="2_"></a>

    ※UNIT.10 に掲載した load_texture_from_file 関数の変更例
    ※必要なヘッダファイルをインクルードすること

テクスチャの読み込みでテクスチャファイルを参照する際に .DDS を優先して読み込むように実装しましょう。

load_texture_from_file() 関数のテクスチャを読み込んでいる以下の部分を

```cpp
hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
resources.insert(make_pair(filename, *shader_resource_view));
```

以下のように変更しましょう。

|実装するファイル|実装する関数|
--|--
**texture**|**load_texture_from_file**

```cpp
    ・・・省略・・・
*   // DDS ファイルが存在するかどうかチェック
*   std::filesystem::path dds_filename(filename);
*   dds_filename.replace_extension("dds");
*   if (std::filesystem::exists(dds_filename.c_str()))
*   {
*       // DDS ファイルが存在したら DDS テクスチャファイル読み込み
*       Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
*       device->GetImmediateContext(immediate_context.GetAddressOf());
*       hr = DirectX::CreateWICTextureFromFile(device, immediate_context.Get(), dds_filename.c_str(),
*           resource.GetAddressOf(), shader_resource_view);
*       _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
*   }
*   else
*   {
*       // DDS ファイルが存在しなければ通常のテクスチャ読み込み
        hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        resources.insert(make_pair(filename, *shader_resource_view));
*   }
    ・・・省略・・・
```

---

#### <span style="color:#334488;">3. ".\\resources\\nico.fbx"を skinned_mesh クラスのコンストラクタに指定し、実行する</span><a name="3_"></a>

変更していなければ既に nico.fbx が設定されているはずなので、そのまま起動して前回と同じモデルが表示されている事を確認して下さい。

@import "images/nico.png"

    ※圧縮テクスチャの使用効果により実行時間が短くなる
    ※ミップマップを生成したことにより描画の質が向上する
    ※このデータではあまり効果は実感できない

とあるように nico.fbx のモデルでは DDS したことによる恩恵はあまり感じられません。
モデルに画像ファイルをたくさん使用しているモデルで確認することで描画時の質や速さを体感できるはずです。