<!--
UNIT09_EXERCISE UNIT10 演習手順
SPRITE - ADVANCED

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;">

Completion
-->

# UNIT10 ADVANCED SPRITE 演習手順

2D 描画としてのある程度の完成で学習要項はありません。やることは多いですが項目ごとにほぼほぼ同じ手順になるので、ある程度理解出来たら見ずにやっていってみましょう。行き詰ったらこの補助教材を確認するようにするぐらいでちょうど良いと思います。行き詰った箇所から確認できるよう目次にしているので、クリックして確認したい部分から確認していってください。

左の番号が演習手順の番号になっています。

* 目次
    * [1. モジュール化](#テクスチャのロードをモジュール化_)
        * [1. テクスチャのロードをモジュール化](#テクスチャのロードをモジュール化_)
        * [モジュール化したテクスチャ読み込みを呼び出す](#モジュール化したテクスチャ読み込みを呼び出す_)
        * [2. シェーダーの生成をモジュール化](#シェーダーの生成をモジュール化_)
        * [モジュール化したシェーダー生成部分を呼び出す](#モジュール化したシェーダー生成部分を呼び出す_)
    * [3. ComPtr 化](#ComPtr化_)
        * [デバイスの ComPtr 化](#デバイスのComPtr化_)
        * [デバイスコンテキストとスワップチェーンの ComPtr 化](#デバイスコンテキストとスワップチェーンのComPtr化_)
        * [レンダーターゲットとデプスステンシルの ComPtr 化](#レンダーターゲットとデプスステンシルのComPtr化_)
        * [サンプラーステートの ComPtr 化](#サンプラーステートのComPtr化_)
        * [ブレンドステートとデプスステンシルの ComPtr 化](#ブレンドステートとデプスステンシルのComPtr化_)
        * [スプライト内部とスプライトバッチ内部のメンバ変数の ComPtr 化](#スプライト内部とスプライトバッチ内部のメンバ変数のComPtr化_)
    * [4. スマートポインタ化](#スプライトとスプライトバッチのスマートポインタ化_)
        * [スプライトとスプライトバッチのスマートポインタ化](#スプライトとスプライトバッチのスマートポインタ化_)
    * [5. 画面上の描画位置とサイズの指定のみでテクスチャ全体を描画する](#演習手順５_)
    * [6. スプライトバッチの描画時にピクセルシェーダーを差し替える](#演習手順６_)
    * [7. スプライトバッチの描画時にテクスチャを差し替える](#演習手順７_)
    * [8. フォント画像の表示](#演習手順８_)
    * [9. リリースビルド](#リリースビルド_)


---

#### <span style="color:#334488;">テクスチャのロードをモジュール化</span><a name="テクスチャのロードをモジュール化_"></a>

    演習手順１

テクスチャの読み込み部分が **sprite クラス** と **sprite_batch クラス** の両方のコンストラクタで同じコードで利用されています。この状態でまた別のクラスでテクスチャのロード処理が必要になり、どんどんコピペで対応していき、その部分を１００個作った場合、どうなるだろうか？

同じテクスチャのロードが記載された１００個のクラスがある状態でコードに修正が必要になったら、その１００箇所すべてを修正していくのか？

さすがにそれは無駄に時間がかかる。

なのでテクスチャのロード部分を関数にまとめて、**sprite クラス** と **sprite_batch クラス** で関数化したものを呼び出すという事を行います。

それではやっていきましょう。

テクスチャに関連する処理なので texture のファイルを作成してそこにコードを移動させます。

    texture.h
    texture.cpp

を作成し、下記のコードを記載しましょう。

||
--|--
**texture.h**|
```cpp
#pragma once

#include <d3d11.h>

/// <summary>
/// 外部ファイルからテクスチャを読み込む
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="filename">ファイル名</param>
/// <param name="shader_resource_view">生成されたシェーダー側で利用するテクスチャの情報</param>
/// <param name="texture2d_desc">生成されたテクスチャの情報</param>
/// <returns></returns>
HRESULT load_texture_from_file(
	ID3D11Device* device,
	const wchar_t* filename,
	ID3D11ShaderResourceView** shader_resource_view,
	D3D11_TEXTURE2D_DESC* texture2d_desc
);

// すべてのテクスチャを解放
void release_all_textures();
```

<span style="color:#994433;font-size:70%;font-weight: bold;">注：入りきらないので１引数ごとに改行しています。<br>すべて記載するのでアスタリスクも省いています。</span>

次に定義部分を実装しましょう。

||
--|--
**texture.cpp**|
```cpp
#include "texture.h"
#include "misc.h"

#include <WICTextureLoader.h>
using namespace DirectX;

#include <wrl.h>
using namespace Microsoft::WRL;

#include <string>
#include <map>
using namespace std;

/// <summary>
/// 生成されたシェーダリソースビュー
/// 生成したらここに保存していきファイル名で保存
/// ２回目以降はここから取得する
/// </summary>
static map<wstring, ComPtr<ID3D11ShaderResourceView>> resources;

/// <summary>
/// 外部ファイルからテクスチャを読み込む
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="filename">ファイル名</param>
/// <param name="shader_resource_view">生成されたシェーダー側で利用するテクスチャの情報</param>
/// <param name="texture2d_desc">生成されたテクスチャの情報</param>
/// <returns></returns>
HRESULT load_texture_from_file(
	ID3D11Device* device,
	const wchar_t* filename,
	ID3D11ShaderResourceView** shader_resource_view,
	D3D11_TEXTURE2D_DESC* texture2d_desc
) {
	HRESULT hr{ S_OK };
	ComPtr<ID3D11Resource> resource;

	auto it = resources.find(filename);
	if (it != resources.end()) {
		*shader_resource_view = it->second.Get();
		(*shader_resource_view)->AddRef();
		(*shader_resource_view)->GetResource(resource.GetAddressOf());
	}
	else {
		hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		resources.insert(make_pair(filename, *shader_resource_view));
	}

	ComPtr<ID3D11Texture2D> texture2d;
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	texture2d->GetDesc(texture2d_desc);

	return hr;
}

// すべてのテクスチャを開放
void release_all_textures() {
	resources.clear();
}
```

注意する点は２点

```cpp
static map<wstring, ComPtr<ID3D11ShaderResourceView>> resources;
```

```cpp
ComPtr<...>
```

この２点です。

上はマップを利用して一度生成したシェーダーリソースビューをファイル名で登録するためのものです。このことにより、２回目以降にこの関数が呼ばれ、同じファイル名が引数に設定された際、マップから検索が行われ、既に生成していたらその生成したものを戻り値として返します。つまり、無駄に２度同じものは生成しない、という作りになっています。

下の ComPtr は端的に説明すると COM オブジェクト用のスマートポインターです。

注意点としてはインクルード部分を確認すると

```cpp
#include <wrl.h>
using namespace Microsoft::WRL;
```

というインクルードと省略処理が行われています。
なので実際には

```cpp
Microsoft::WRL::ComPtr<...>
```

になります。

内容自体は、**shared_ptr** と同じです。
今まで **COM オブジェクト** は使い終わったら

```cpp
...->Release()
```

を行っていましたが、**ComPtr<...>** で生成することで **Release メンバ関数** を自動で呼び出してくれるというものです。
**COM オブジェクト** は通常のスマートポインタである **shared_ptr<...>** などは利用できないのでその代わりのものという事です。

---

#### <span style="color:#334488;">モジュール化したテクスチャ読み込みを呼び出す</span><a name="モジュール化したテクスチャ読み込みを呼び出す_"></a>

テクスチャ読み込みをモジュール化できたら **sprite クラス** **sprite_batch クラス** のコンストラクタのテクスチャ読み込み部分を

    load_texture_from_file(...);

に変更していこう。

まずは **sprite.cpp** で **texture.h** を**インクルード**してください。

||
--|--
**sprite.cpp**|
```cpp
    ・・・省略・・・
    #include <WICTextureLoader.h>

*   #include "texture.h"
    
    ・・・省略・・・
```

次に **sprite クラスの** の **コンストラクタ** の下記の部分を

```
ID3D11Resource* resource{};
hr = DirectX::CreateWICTextureFromFile(device, filename, &resource, &shader_resource_view);
_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
resource->Release();

ID3D11Texture2D* texture2d{};
hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);
_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
texture2d->GetDesc(&texture2d_desc);
texture2d->Release();
```

以下に変更しよう。

|||
--|--
**sprite.cpp**|**sprite::sprite(...)**
```cpp
*   load_texture_from_file(device, filename, &shader_resource_view, &texture2d_desc);
```

同じように **sprite_batch** 側も行っていきます。

まずは **sprite_batch.cpp** で **texture.h** を**インクルード**してください。

||
--|--
**sprite_batch.cpp**|
```cpp
    ・・・省略・・・
    #include <WICTextureLoader.h>

*   #include "texture.h"
    
    ・・・省略・・・
```

**sprite_batch クラス** の **コンストラクタ** の下記の部分を

```
ID3D11Resource* resource{};
hr = DirectX::CreateWICTextureFromFile(device, filename, &resource, &shader_resource_view);
_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
resource->Release();

ID3D11Texture2D* texture2d{};
hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);
_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
texture2d->GetDesc(&texture2d_desc);
texture2d->Release();
```

以下に変更しよう。

|||
--|--
**sprite_batch.cpp**|**sprite_batch::sprite_batch(...)**
```cpp
*   load_texture_from_file(device, filename, &shader_resource_view, &texture2d_desc);
```

起動しても変化はありませんがコードの重複はなくなりました。これでテクスチャの読み込み処理が必要になった際は

    load_texture_from_file(...);

を呼び出すだけで良くなり、テクスチャの読み込みに不具合が発生しても修正するのは

    load_texture_from_file(...)
    {
        ...
    }

の定義部分だけを直せば良いだけということになりました。

---

#### <span style="color:#334488;">シェーダーの生成をモジュール化</span><a name="シェーダーの生成をモジュール化_"></a>

    演習手順２

なんとなく目に入ったと思うのですが、頂点シェーダーの生成部分とピクセルシェーダーの生成部分も **sprite クラス** **sprite_batch クラス** ともに同じコードが利用されています。

ここも同じ理由なのでモジュール化してしまいましょう。

シェーダーに関連する処理なので shader のファイルを作成してそこにコードを移動させます。

    shader.h
    shader.cpp

を作成し、下記のコードを記載しましょう。

||
--|--
**shader.h**|
```cpp
#pragma once

#include <d3d11.h>

/// <summary>
/// 頂点シェーダーの生成
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="cso_name">cso ファイル名</param>
/// <param name="vertex_shader">生成された頂点シェーダー</param>
/// <param name="input_layout">生成された入力レイアウト</param>
/// <param name="input_element_desc">入力レイアウトを生成するための構造体</param>
/// <param name="num_elements">入力レイアウトの要素数</param>
/// <returns></returns>
HRESULT create_vs_from_cso(
	ID3D11Device* device,
	const char* cso_name,
	ID3D11VertexShader** vertex_shader,
	ID3D11InputLayout** input_layout,
	D3D11_INPUT_ELEMENT_DESC* input_element_desc,
	UINT num_elements
);

/// <summary>
/// ピクセルシェーダーの生成
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="cso_name">cso ファイル名</param>
/// <param name="pixel_shader">生成されたピクセルシェーダー</param>
/// <returns></returns>
HRESULT create_ps_from_cso(
	ID3D11Device* device, 
	const char* cso_name, 
	ID3D11PixelShader** pixel_shader
);
```

<span style="color:#994433;font-size:70%;font-weight: bold;">注：入りきらないので１引数ごとに改行しています</span>

次に定義部分を実装しましょう。

||
--|--
**shader.cpp**|
```cpp
#include "shader.h"
#include "misc.h"

#include "memory"
using namespace std;

/// <summary>
/// 頂点シェーダーの生成
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="cso_name">cso ファイル名</param>
/// <param name="vertex_shader">生成された頂点シェーダー</param>
/// <param name="input_layout">生成された入力レイアウト</param>
/// <param name="input_element_desc">入力レイアウトを生成するための構造体</param>
/// <param name="num_elements">入力レイアウトの要素数</param>
/// <returns></returns>
HRESULT create_vs_from_cso(
	ID3D11Device* device,
	const char* cso_name,
	ID3D11VertexShader** vertex_shader,
	ID3D11InputLayout** input_layout,
	D3D11_INPUT_ELEMENT_DESC* input_element_desc,
	UINT num_elements
) {
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]> cso_data{ make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (input_layout) {
		hr = device->CreateInputLayout(input_element_desc, num_elements, cso_data.get(), cso_sz, input_layout);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	return hr;
}

/// <summary>
/// ピクセルシェーダーの生成
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="cso_name">cso ファイル名</param>
/// <param name="pixel_shader">生成されたピクセルシェーダー</param>
/// <returns></returns>
HRESULT create_ps_from_cso(
	ID3D11Device* device,
	const char* cso_name,
	ID3D11PixelShader** pixel_shader
) {
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]> cso_data{ make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}
```

注目する点は１点

    頂点シェーダーにおいて入力レイアウトの設定が含まれていない

という点です。

#### なぜか？

**入力レイアウト**に関しては描画クラスごとに作りが変わる可能性がとても高いのでモジュール化から外して実装を行っている、というだけです。

---

#### <span style="color:#334488;">モジュール化したシェーダー生成部分を呼び出す</span><a name="モジュール化したシェーダー生成部分を呼び出す_"></a>

シェーダー生成部分をモジュール化できたら **sprite クラス** **sprite_batch クラス** のコンストラクタのシェーダー生成部分を

    create_vs_from_cso(...);
    create_ps_from_cso(...)

に変更していこう。

先に上の関数が利用できるように **shader.h** を**インクルード**しておこう。

||
--|--
**sprite.cpp**|
```cpp
    ・・・省略・・・
    #include <WICTextureLoader.h>

    #include "texture.h"
*   #include "shader.h"
    ・・・省略・・・
```

**sprite クラスの** の **コンストラクタ** の下記の部分を

<span style="color:#994433;font-size:70%;font-weight: bold;">注：入りきらないのでコメントは省いています。</span>

```
{
    const char* cso_name{ "sprite_vs.cso" };

    FILE* fp{};
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &vertex_shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc), cso_data.get(), cso_sz, &input_layout);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
{
    const char* cso_name{ "sprite_ps.cso" };

    FILE* fp{};
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, &pixel_shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
```

以下に変更しよう。

|||
--|--
**sprite.cpp**|**sprite::sprite(...)**
```cpp
*   D3D11_INPUT_ELEMENT_DESC input_element_desc[]
*   {
*       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*       { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*   };

*   create_vs_from_cso(device, "sprite_vs.cso", &vertex_shader, &input_layout, input_element_desc, _countof(input_element_desc));
*   create_ps_from_cso(device, "sprite_ps.cso", &pixel_shader);
```


同じように sprite_batch 側も行っていきます。

まずは sprite_batch.cpp で shader.h をインクルードしてください。

||
--|--
**sprite_batch.cpp**|
```cpp
    ・・・省略・・・
    #include <WICTextureLoader.h>

    #include "texture.h"
*   #include "shader.h" 
    ・・・省略・・・
```

sprite_batch クラス の コンストラクタ の下記の部分を

```
{
    const char* cso_name{ "sprite_vs.cso" };

    FILE* fp{};
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &vertex_shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc), cso_data.get(), cso_sz, &input_layout);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
{
    const char* cso_name{ "sprite_ps.cso" };

    FILE* fp{};
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, &pixel_shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
```

以下に変更しよう。

|||
--|--
**sprite_batch.cpp**|**sprite_batch::sprite_batch(...)**
```cpp
*   D3D11_INPUT_ELEMENT_DESC input_element_desc[]
*   {
*       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*       { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*   };

*   create_vs_from_cso(device, "sprite_vs.cso", &vertex_shader, &input_layout, input_element_desc, _countof(input_element_desc));
*   create_ps_from_cso(device, "sprite_ps.cso", &pixel_shader);
```

こちらも起動しても変化はありません。ですが、テクスチャのモジュール化と同じようにシェーダーの読み込み部分を関数１つで行えるようになり、なおかつ修正する際の手間を一か所に閉じ込めることが出来ました。

---

### <span style="color:#334488;">ComPtr 化</span><a name="ComPtr化_"></a>

    演習手順３

演習手順１で出てきた **COM オブジェクト** をスマートポインタ化するという **ComPtr** をすべての **COM オブジェクト**に対して行いましょう、という演習です。

演習手順の３－１，３－２、３－３をすべて行わないとエラーで動きません。

演習手順３－１は **ComPtr<...>** を利用するために必要なヘッダファイルのインクルードです。

演習手順３－２は ComPtr の宣言の書き方です。ヘッダファイルに using namespace は危険なので名前空間をすべて含めた形である **Microsoft::WRL::ComPtr<...>** と宣言します。

演習手順３－３は関数に渡す際のアドレスの問題です。例えば**デバイス**で確認してみましょう。宣言は以下のように**ポインタ**で宣言されています。

```cpp
    ID3D11Device* device;
```

なので**ポインタ**として設定されている**仮引数**に対して**実引数**として渡す際には

```cpp
    new sprite(device, ...
```

として渡し、**ダブルポインタ**として設定されている**仮引数**に対して**実引数**として渡す際には、**アンパサンドを付けて**渡しています。

```cpp
    hr = D3D11CreateDeviceAndSwapChain(..., &device, ...
```

この部分が **ComPtr** になる事で変化がおきます。**デバイス**で確認してみましょう。

```cpp
    Microsoft::WRL::ComPtr<ID3D11Device> device;
```

の場合、**ポインタ**として設定されている**仮引数**に対して**実引数**として渡す際には、

```cpp
    new sprite(device.Get(), ...
```

**ダブルポインタ**として設定されている**仮引数**に対して**実引数**として渡す際には、

```cpp
    hr = D3D11CreateDeviceAndSwapChain(..., device.GetAddressOf(), ...
```

となります。

それでは１つずつ変更していってみましょう。

まずは上記で確認した**デバイス**から変更していきましょう。

---

### <span style="color:#334488;">デバイスの ComPtr 化</span><a name="デバイスのComPtr化_"></a>

まずは ComPtr を利用できるようにヘッダをインクルードしましょう。

||
--|--
**framework.h**|
```cpp
    ・・・省略・・・
    #include "sprite_batch.h"

*   #include <wrl.h>

    CONST LONG SCREEN_WIDTH{ 1280 };
    ・・・省略・・・
```

以下の framework.h の framework クラス内の部分を

```cpp
ID3D11Device* device;
```

以下のように変更しましょう。

|||
--|--
**framework.h**|**framework**

```cpp
*   Microsoft::WRL::ComPtr<ID3D11Device> device;
```

framework.cpp で device を利用していた部分がエラーになるので、すべて変更していきます。

---

**USE_IMGUI** を利用している場合、 **framework.h** の **framework::run()** の以下の部分を

```
#ifdef USE_IMGUI
    ・・・省略・・・
    ImGui_ImplDX11_Init(device, immediate_context);
    ・・・省略・・・
#endif
```

以下のように変更してください。

|||
--|--
**framework.h**|**framework::run()**

```cpp
    #ifdef USE_IMGUI
        ・・・省略・・・
*       ImGui_ImplDX11_Init(device.Get(), immediate_context);
        ・・・省略・・・
    #endif
```

---

**framework.cpp** の **framework::initialize メンバ関数**の **D3D11CreateDeviceAndSwapChain** の以下の部分を

```
hr = D3D11CreateDeviceAndSwapChain(
    ..., 
    &device, 
    ...);
```

以下のように変更しましょう。

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    hr = D3D11CreateDeviceAndSwapChain(
        ...,
*       device.GetAddressOf(),
        ...);
```

**D3D11CreateDeviceAndSwapChain(...) 関数** にカーソルを併せて関数の詳細を確認すると第１０引数の型が

    ID3D11Device**

と指定されています。この型が指定されている場合は

    .GetAddressOf()

を利用して渡します。


---

同じく **framework::initialize メンバ関数** 内の以下の部分を

```
sprites[0] = new sprite(device, L".\\resources\\cyberpunk.jpg");
sprites[1] = new sprite(device, L".\\resources\\player-sprites.png");
sprite_batches[0] = new sprite_batch(device, L".\\resources\\player-sprites.png", 2048);
```

以下のように変更しましょう。

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
*   sprites[0] = new sprite(device.Get(), L".\\resources\\cyberpunk.jpg");
*   sprites[1] = new sprite(device.Get(), L".\\resources\\player-sprites.png");
*   sprite_batches[0] = new sprite_batch(device.Get(), L".\\resources\\player-sprites.png", 2048);
```


コンストラクタである **new sprite(...)** にカーソルを併せてコンストラクタの詳細を確認すると第１引数の型が

    ID3D11Device* 

と指定されています。この型が指定されている場合は

    .Get()

を利用して渡します。

---

最後に **framwwork::uninitialize メンバ関数** 内で行っていた以下のデバイスの解放処理を削除しておきましょう。。

```cpp
device->Release();
```

---

### <span style="color:#334488;">デバイスコンテキストとスワップチェーンの ComPtr 化</span><a name="デバイスコンテキストとスワップチェーンのComPtr化_"></a>

渡すべき引数が**ポインタ**なのか**ダブルポインタ**なのかでどのような形で渡せば良いかがなんとなくわかったところでその他の COM オブジェクトも ComPtr 化していきましょう。

**framework クラス**の以下の部分を

```
ID3D11DeviceContext* immediate_context;
IDXGISwapChain* swap_chain;
```

以下のように変更してください。

|||
--|--
**framework.h**|**framework**

```cpp
*   Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
*   Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
```

---

**USE_IMGUI** を利用している場合、 **framework.h** の **framework::run()** の以下の部分を

```
#ifdef USE_IMGUI
    ・・・省略・・・
    ImGui_ImplDX11_Init(device.Get(), immediate_context);
    ・・・省略・・・
#endif
```

以下のように変更してください。

|||
--|--
**framework.h**|**framework::run()**

```cpp
    #ifdef USE_IMGUI
        ・・・省略・・・
*       ImGui_ImplDX11_Init(device.Get(), immediate_context.Get());
        ・・・省略・・・
    #endif
```

---

**framework.cpp** の **framework::initialize メンバ関数**の **D3D11CreateDeviceAndSwapChain** の以下の部分を

```
hr = D3D11CreateDeviceAndSwapChain(
    ..., 
    &swap_chain,
    ...,
    &immediate_context 
    ...);
```

以下のように変更しましょう。

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    hr = D3D11CreateDeviceAndSwapChain(
        ...,
*       swap_chain.GetAddressOf(),
        ...,
        immediate_context.GetAddressOf(),
        ...);
```

---

**framework.cpp** の **framwwork::render(...) メンバ関数** の以下の部分を

```
sprite_batches[0]->begin(immediate_context);
for (size_t i = 0; i < 1092; ++i) {
    sprite_batches[0]->render(
        immediate_context,
        ・・・省略・・・
    );
    ・・・省略・・・
}
sprite_batches[0]->end(immediate_context);
```

以下のように変更して下さい。

|||
--|--
**framework.cpp**|**framework::render(...)**

```cpp
*   sprite_batches[0]->begin(immediate_context.Get());
    for (size_t i = 0; i < 1092; ++i) {
        sprite_batches[0]->render(
*           immediate_context.Get(),
            ・・・省略・・・
        );
        ・・・省略・・・
    }
*   sprite_batches[0]->end(immediate_context.Get());
```

---

最後に **framework.cpp** の **framework::uninitialize()** の以下の部分を**削除**してください。

```cpp
immediate_context->Release();
swap_chain->Release();
```

---

いったん起動してみてください。
見た目は UNIT09 の状態と変わりません。エラーが出なければ問題ありません。

---

### <span style="color:#334488;">レンダーターゲットとデプスステンシルの ComPtr 化</span><a name="レンダーターゲットとデプスステンシルのComPtr化_"></a>

だんだん慣れてきたかと思います。５つのリソースの内の残りの２つも ComPtr化してしまいましょう。

**framework クラス**の以下の部分を

```
ID3D11RenderTargetView* render_target_view;
ID3D11DepthStencilView* depth_stencil_view;
```

以下のように変更してください。

|||
--|--
**framework.h**|**framework**

```cpp
*   Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
*   Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
```

---

次に **render_target_view** が生成されている部分の **CreateRenderTargetView(...)** の部分で同じように **&render_target_view** を **render_target_view.GetAddressOf()** に変更するのですが、ここではもう１つローカル変数に **COM オブジェクト** である

    ID3D11Texture2D* back_buffer 
    
が利用されているのでそれも **ComPtr** 化します。

**framework.cpp** の **framework::initialize メンバ関数**の以下の部分の * を

```
    ・・・省略・・・
*   ID3D11Texture2D* back_buffer{};
*   hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   hr = device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   back_buffer->Release();
    ・・・省略・・・
```

以下のように変更しましょう。最後の行の **back_buffer->Release()** は必要なくなったので削除しているだけです。

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
*   Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer{};
*   hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(back_buffer.GetAddressOf()));
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   hr = device->CreateRenderTargetView(back_buffer.Get(), NULL, render_target_view.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    ・・・省略・・・
```

---

**depth_stencil_view** の方も同じように **CreateDepthSencilView()** で利用しているローカル変数

    ID3D11Texture2D* depth_stencil_buffer{};

が **COM オブジェクト** なのでこれも **ComPtr** 化します。

**framework.cpp** の **framework::initialize メンバ関数**の以下の部分の * を

```
*   ID3D11Texture2D* depth_stencil_buffer{};
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    texture2d_desc.Width = SCREEN_WIDTH;
    texture2d_desc.Height = SCREEN_HEIGHT;
    texture2d_desc.MipLevels = 1;
    texture2d_desc.ArraySize = 1;
    texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texture2d_desc.SampleDesc.Count = 1;
    texture2d_desc.SampleDesc.Quality = 0;
    texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
    texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texture2d_desc.CPUAccessFlags = 0;
    texture2d_desc.MiscFlags = 0;
*   hr = device->CreateTexture2D(&texture2d_desc, NULL, &depth_stencil_buffer);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
    depth_stencil_view_desc.Format = texture2d_desc.Format;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;
*   hr = device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depth_stencil_view);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   depth_stencil_buffer->Release();
```

以下のように変更しましょう。最後の行の **depth_stencil_buffer->Release()** は削除です。

```cpp
*   Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    texture2d_desc.Width = SCREEN_WIDTH;
    texture2d_desc.Height = SCREEN_HEIGHT;
    texture2d_desc.MipLevels = 1;
    texture2d_desc.ArraySize = 1;
    texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texture2d_desc.SampleDesc.Count = 1;
    texture2d_desc.SampleDesc.Quality = 0;
    texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
    texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texture2d_desc.CPUAccessFlags = 0;
    texture2d_desc.MiscFlags = 0;
*   hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
    depth_stencil_view_desc.Format = texture2d_desc.Format;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;
*   hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
```

---

**framework.cpp** の **framwwork::render(...) メンバ関数** の以下の部分を

```
immediate_context->ClearRenderTargetView(render_target_view, color);
immediate_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
```

以下のように変更して下さい。

|||
--|--
**framework.cpp**|**framework::render(...)**

```cpp
*   immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
*   immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
*   immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
```


---

最後に **framework.cpp** の **framework::uninitialize()** の以下の解放部分を**削除**してください。

```cpp
render_target_view->Release();
depth_stencil_view->Release();
```

---

こまめに起動してエラーが出ないか確認してみましょう。

---

### <span style="color:#334488;">サンプラーステートの ComPtr 化</span><a name="サンプラーステートのComPtr化_"></a>

５つの基本リソースを **ComPtr** 化出来たところで次はサンプラーステートである

    ID3D11SamplerState* sampler_states[3];

を **ComPtr** 化していきましょう。これまでとの違いは配列であるという事です。
それではやっていきましょう。

**framework クラス**の以下の部分を

```
ID3D11SamplerState* sampler_states[3];
```

以下のように変更してください。

|||
--|--
**framework.h**|**framework**

```cpp
*   Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[3];
```

---

**framework.cpp** の **framework::initialize メンバ関数**の **CreateSamplerState** の以下の * の部分を

```
*   hr = device->CreateSamplerState(&sampler_desc, &sampler_states[0]);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
*   hr = device->CreateSamplerState(&sampler_desc, &sampler_states[1]);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
*   hr = device->CreateSamplerState(&sampler_desc, &sampler_states[2]);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
```

以下のように変更しましょう。

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
*   hr = device->CreateSamplerState(&sampler_desc, sampler_states[0].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
*   hr = device->CreateSamplerState(&sampler_desc, sampler_states[1].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
*   hr = device->CreateSamplerState(&sampler_desc, sampler_states[2].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
```

---

**framework.cpp** の **framwwork::render(...) メンバ関数** の以下の部分を

```
immediate_context->PSSetSamplers(0, 1, &sampler_states[0]);
immediate_context->PSSetSamplers(1, 1, &sampler_states[1]);
immediate_context->PSSetSamplers(2, 1, &sampler_states[2]);
```

以下のように変更して下さい。

|||
--|--
**framework.cpp**|**framework::render(...)**

```cpp
*   immediate_context->PSSetSamplers(0, 1, sampler_states[0].GetAddressOf());
*   immediate_context->PSSetSamplers(1, 1, sampler_states[1].GetAddressOf());
*   immediate_context->PSSetSamplers(2, 1, sampler_states[2].GetAddressOf());
```

---

最後に **framework.cpp** の **framework::uninitialize()** の以下の解放部分を**削除**してください。

```cpp
for (ID3D11SamplerState* p : sampler_states) p->Release();
```

---

### <span style="color:#334488;">ブレンドステートとデプスステンシルの ComPtr 化</span><a name="ブレンドステートとデプスステンシルのComPtr化_"></a>

配列になったとしても単に要素の中に **ComPtr** として代入されているだけなので、やることはあまり変わりないというのは理解してもらえたと思います。

では framework クラスにある残りの２つの **COM オブジェクト**

    ID3D11DepthStencilState* depth_stencil_states[4];
	ID3D11BlendState* blend_states[4];

を **ComPtr** 化していきましょう。


**framework クラス**の以下の部分を

```
ID3D11DepthStencilState* depth_stencil_states[4];

ID3D11BlendState* blend_states[4];
```

以下のように変更してください。

|||
--|--
**framework.h**|**framework**

```cpp
*   Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];

*   Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[4];
```

---

**framework.cpp** の **framework::initialize メンバ関数**の **CreateSamplerState** の以下の * の部分である８か所、**CreateDepthStencilState()** と **CreateBlendState()** の部分を

```
    ・・・省略・・・
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[0]);
    ・・・省略・・・
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[1]);
    ・・・省略・・・
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[2]);
    ・・・省略・・・
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[3]);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    ・・・省略・・・
*	hr = device->CreateBlendState(&blend_desc, &blend_states[0]);
    ・・・省略・・・
*	hr = device->CreateBlendState(&blend_desc, &blend_states[1]);
    ・・・省略・・・
*	hr = device->CreateBlendState(&blend_desc, &blend_states[2]);
    ・・・省略・・・
*	hr = device->CreateBlendState(&blend_desc, &blend_states[3]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
```

以下のように変更しましょう。

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[0].GetAddressOf());
    ・・・省略・・・
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[1].GetAddressOf());
    ・・・省略・・・
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[2].GetAddressOf());
    ・・・省略・・・
*   hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[3].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    ・・・省略・・・
*   hr = device->CreateBlendState(&blend_desc, blend_states[0].GetAddressOf());
    ・・・省略・・・
*   hr = device->CreateBlendState(&blend_desc, blend_states[1].GetAddressOf());
    ・・・省略・・・
*   hr = device->CreateBlendState(&blend_desc, blend_states[2].GetAddressOf());
    ・・・省略・・・
*   hr = device->CreateBlendState(&blend_desc, blend_states[3].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
```

---

**framework.cpp** の **framwwork::render(...) メンバ関数** の以下の部分を

```
immediate_context->OMSetDepthStencilState(depth_stencil_states[3], 0);

immediate_context->OMSetBlendState(blend_states[1], nullptr, 0xFFFFFFFF);
```

以下のように変更して下さい。
設定している、ブレンドモードによっては番号が違うかもしれないので、そこは自身のプロジェクトに合わせてください。

|||
--|--
**framework.cpp**|**framework::render(...)**

```cpp
*   immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);

*   immediate_context->OMSetBlendState(blend_states[1].Get(), nullptr, 0xFFFFFFFF);
```

---

最後に **framework.cpp** の **framework::uninitialize()** の以下の解放部分を**削除**してください。

```cpp
for (ID3D11DepthStencilState* p : depth_stencil_states) p->Release();

for (ID3D11BlendState* p : blend_states) p->Release();
```


---

### <span style="color:#334488;">スプライト内部とスプライトバッチ内部のメンバ変数の ComPtr 化</span><a name="スプライト内部とスプライトバッチ内部のメンバ変数のComPtr化_"></a>

**framework クラス** 内部の COM オブジェクトはすべて ComPtr 化できたので、次は **sprite クラス** 内部と **sprite_batch クラス** 内部にある COM オブジェクトを ComPtr 化していきましょう。

ここでの COM オブジェクトは５つです。

まずは以下のファイルを **sprite.h** と **sprite_batch.h** にインクルードしましょう。

||
--|--
**sprite.h**|
```cpp
    ・・・省略・・・
    #include <directxmath.h>
*   #include <wrl.h>

    ・・・省略・・・
```

||
--|--
**sprite_batch.h**|

```cpp
    ・・・省略・・・
    #include <directxmath.h>
*   #include <wrl.h>

    ・・・省略・・・
```

次にそれぞれのファイルの以下の COM オブジェクトを

```
    ID3D11VertexShader* vertex_shader;
    ID3D11PixelShader* pixel_shader;
    ID3D11InputLayout* input_layout;
    ID3D11Buffer* vertex_buffer;

    ID3D11ShaderResourceView* shader_resource_view;
```

以下のように変更しましょう。


|||
--|--
**sprite.h**|**sprite**
```cpp
*   Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
*   Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
*   Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
*   Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

*   Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
```

|||
--|--
**sprite_batch.h**|**sprite_batch**
```cpp
*   Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
*   Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
*   Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
*   Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

*   Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
```

---

次にそれぞれのクラスのコンストラクタで COM オブジェクトの生成を行っている以下の部分を

```
hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

・・・省略・・・

create_vs_from_cso(device, "sprite_vs.cso", &vertex_shader, &input_layout, input_element_desc, _countof(input_element_desc));
create_ps_from_cso(device, "sprite_ps.cso", &pixel_shader);

load_texture_from_file(device, filename, &shader_resource_view, &texture2d_desc);
```

以下のように変更しましょう。

|||
--|--
**sprite.h**|**sprite**
```cpp
*   hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    ・・・省略・・・

*   create_vs_from_cso(device, "sprite_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
*   create_ps_from_cso(device, "sprite_ps.cso", pixel_shader.GetAddressOf());

*   load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
```

|||
--|--
**sprite_batch.h**|**sprite_batch**
```cpp
*   hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    ・・・省略・・・

*   create_vs_from_cso(device, "sprite_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
*   create_ps_from_cso(device, "sprite_ps.cso", pixel_shader.GetAddressOf());

*   load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
```

---

次はスプライトの描画部分から変更していきます。

複数の **render() メンバ関数** のうちの以下の **render() メンバ関数** の定義のなかの

```
void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle/*degree*/, float sx, float sy, float sw, float sh);
```

以下の部分を

```
・・・省略・・・
hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

・・・省略・・・

immediate_context->Unmap(vertex_buffer, 0);

UINT stride{ sizeof(vertex) };
UINT offset{ 0 };
immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

immediate_context->IASetInputLayout(input_layout);

immediate_context->VSSetShader(vertex_shader, nullptr, 0);
immediate_context->PSSetShader(pixel_shader, nullptr, 0);

immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);

・・・省略・・・

```

以下のように変更しましょう。

|||
--|--
**sprite.h**|**sprite**
```cpp
    ・・・省略・・・
*   hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    ・・・省略・・・

*   immediate_context->Unmap(vertex_buffer.Get(), 0);

    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
*   immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

*   immediate_context->IASetInputLayout(input_layout.Get());

*   immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
*   immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

*   immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
```

---

ComPtr 化できたら不要になった**スプライトクラス**の**デストラクタ**で利用されている Release() は削除しておきましょう。
つまり **sprite::~sprite()** の以下の部分をすべて削除しておきましょう。

```cpp
vertex_shader->Release();
pixel_shader->Release();
input_layout->Release();
vertex_buffer->Release();

shader_resource_view->Release();
```

---

次にスプライトバッチの描画開始部分を変更していきます。

**sprite_batch::begine(...)** の以下の部分を

```
immediate_context->VSSetShader(vertex_shader, nullptr, 0);
immediate_context->PSSetShader(pixel_shader, nullptr, 0);
immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);
```

以下のように変更しましょう。

```cpp
*   immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
*   immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
*   immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
```

---

次にスプライトバッチの描画終了部分を変更していきましょう。

**sprite_batch::end(...)** の以下の部分を

```
・・・省略・・・
hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

・・・省略・・・

immediate_context->Unmap(vertex_buffer, 0);

UINT stride{ sizeof(vertex) };
UINT offset{ 0 };
immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
immediate_context->IASetInputLayout(input_layout);
```

以下のように変更しましょう。

```cpp
    ・・・省略・・・
*   hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    ・・・省略・・・

*   immediate_context->Unmap(vertex_buffer.Get(), 0);

    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
*   immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
*   immediate_context->IASetInputLayout(input_layout.Get());
```

---

ComPtr 化できたら不要になった**スプライトバッチクラス**の**デストラクタ**で利用されている Release() は削除しておきましょう。
**sprite_batch::~sprite_batch()** の以下の部分をすべて削除しておきましょう。

```cpp
vertex_shader->Release();
pixel_shader->Release();
input_layout->Release();
vertex_buffer->Release();

shader_resource_view->Release();
```

---

ここまででコード上のすべての **COM オブジェクト** が **ComPtr 化** されました。
起動して確認しておいてください。見た目は変わりません。エラーがでなければ問題ありません。
エラーが出たら以下の点を確認してみてください。

* .Get() のつけ忘れ
* Release() の消し忘れ

などなどです。

---

### <span style="color:#334488;">スプライトとスプライトバッチのスマートポインタ化</span><a name="スプライトとスプライトバッチのスマートポインタ化_"></a>

    演習手順４

**framework クラス** に宣言した内の２つ

	sprite* sprites[8];
	sprite_batch* sprite_batches[8];

は COM オブジェクトではなく普通のポインタです。ただこれらも **uninitialize()** で **delete** による解放をおこなっているので、自動解放に切替ていきます。これらは **COM オブジェクト** ではないので通常の**スマートポインタ**で対応します。

**framework クラス**の以下の部分を

```
sprite* sprites[8];
sprite_batch* sprite_batches[8];
```

以下のように変更してください。

|||
--|--
**framework.h**|**framework**

```cpp
*   std::unique_ptr<sprite> sprites[8];
*   std::unique_ptr<sprite_batch> sprite_batches[8];
```

---

**framework.cpp** の **framework::initialize メンバ関数**の **D3D11CreateDeviceAndSwapChain** の以下の部分を

```
sprites[0] = new sprite(device.Get(), L".\\resources\\cyberpunk.jpg");

sprites[1] = new sprite(device.Get(), L".\\resources\\player-sprites.png");

sprite_batches[0] = new sprite_batch(device.Get(), L".\\resources\\player-sprites.png", 2048);
```

以下のように変更しましょう。

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
*   sprites[0] = std::make_unique<sprite>(device.Get(), L".\\resources\\cyberpunk.jpg");

*   sprites[1] = std::make_unique<sprite>(device.Get(), L".\\resources\\player-sprites.png");

*   sprite_batches[0] = std::make_unique<sprite_batch>(device.Get(), L".\\resources\\player-sprites.png", 2048);
```

---

**framework.cpp** の **framework::uninitialize()** の以下の解放部分を**削除**してください。

```cpp
for (sprite* p : sprites) delete p;

for (sprite_batch* p : sprite_batches) delete p;
```

---

**framework::render(...)** のを確認してもらうとわかると思いますが、現状 **sprite クラス** の描画を行っていません。

なので、見づらくならないよういったん **sprite** の生成は削除します。

**framework::initialize()** の以下の部分を削除しておいて下さい。

```cpp
sprites[0] = std::make_unique<sprite>(device.Get(), L".\\resources\\cyberpunk.jpg");

sprites[1] = std::make_unique<sprite>(device.Get(), L".\\resources\\player-sprites.png");
```

---

### <span style="color:#334488;">画面上の描画位置とサイズの指定のみでテクスチャ全体を描画する</span><a name="演習手順５_"></a>

    演習手順５

演習手順にある通り、「画面上の描画位置とサイズの指定のみでテクスチャ全体を描画する」ことのできる **render() メンバ関数** を **sprite クラス** と **sprite_batch クラス** にもう１つ追加するというものです。

|||
--|--
**sprite.h**|**sprite**

|||
--|--
**sprite_batch.h**|**sprite_batch**

```cpp
    ・・・省略・・・
    void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle/*degree*/, float sx, float sy, float sw, float sh);

    // 画面上の描画位置とサイズの指定のみでテクスチャ全体を描画
*   void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);

    ・・・省略・・・
```

||
--|--
**sprite.cpp**|

||
--|--
**sprite_batch.cpp**|

```cpp
    ・・・省略(上にその他の render(...) の定義)・・・

*   void sprite::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh)
*   {
*       render(immediate_context, 
*           dx, dy, dw, dh,
*           1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
*           0.0f, 0.0f,
*           static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height)
*       );
*   }

    ・・・省略(下にデストラクタの定義)・・・
```

最後に確認をしましょう。

**framework::render(...)** 以下の部分をいったん削除して下さい

```
sprite_batches[0]->begin(immediate_context.Get());
for (size_t i = 0; i < 1092; ++i) {
    sprite_batches[0]->render(
        immediate_context.Get(),
        x, static_cast<float>(static_cast<int>(y) % 720),
        64, 64,
        1, 1, 1, 1,
        0,
        140 * 0, 240 * 0,
        140, 240
    );
    x += 32;
    if (x > 1280 - 64) {
        x = 0;
        y += 24;
    }
}
sprite_batches[0]->end(immediate_context.Get());
```

削除した部分に以下を記述してください。

```cpp
sprite_batches[0]->begin(immediate_context.Get());
sprite_batches[0]->render(immediate_context.Get(), 0, 0, 1280, 720);
sprite_batches[0]->end(immediate_context.Get());
```

起動すると以下が表示されます。

@import "images/プレイヤー.png"

---

### <span style="color:#334488;">スプライトバッチの描画時にピクセルシェーダーを差し替える</span><a name="演習手順６_"></a>

    演習手順６

現在スプライトバッチに設定されているピクセルシェーダーファイルは

    sprite_ps.hlsl

で、内部で生成し利用しており、固定されているので以降変更することはできません。

仮にこのスプライトバッチに対して、別のシェーダーファイルを適用したくなった場合どうすればよいのか？

sprite_batch_02 クラス を別途作成してコードはすべてコピペし、シェーダーファイル名の部分だけ変更するというのも１つの手段かもしれませんが、ですがそれだと１００個のエフェクトシェーダーに対応しようとした場合どうなるのか？

考えただけでも修正や拡張が大変で管理が出来なくなるというのはわかると思います。
ですが１００個程度のエフェクトはゲームをやっていればざらにあるというのも理解できるかと思います。

なのでスプライトバッチのクラスは増やさず、描画時に別途読み込んておいたピクセルシェーダーに差し替えれるように改良する方法を考えてみようというのが演習手順の６です。

---

まずは差し替えるピクセルシェーダーファイルを作成しましょう。

新規でピクセルシェーダーファイルを作成します。ファイル名は適当で構いませんが、ひとまずここでは **effect_ps.hlsl** にしておきます。

||
--|--
**effect_ps.hlsl**|
```hlsl
#include "sprite.hlsli"

Texture2D color_map : register(t0);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float4 main(VS_OUT pin) : SV_TARGET
{
    if (pin.texcoord.y < 0.25) 
    {
        return color_map.Sample(linear_sampler_state, pin.texcoord) ;
    }
    else if (pin.texcoord.y < 0.5) 
    {
        return color_map.Sample(linear_sampler_state, pin.texcoord) * 0.75;
    }
    else if (pin.texcoord.y < 0.75) 
    {
        return color_map.Sample(linear_sampler_state, pin.texcoord) * 0.5;
    }

	return color_map.Sample(linear_sampler_state, pin.texcoord) * 0.25;
}
```

テクスチャコードの y が 0.25 単位(４分の１単位)で色が暗くなるようなシェーダーです。
つまり下に行くほど暗くなるような設定です。

エフェクト自体に決まりはないので、適当に値をいじって自分なりのエフェクトを作ってみてください。

---

### <span style="color:#334488;">ピクセルシェーダーを単体で作成する</span>

上で作成したピクセルシェーダーファイルをスプライトバッチ内部ではなく別途、単体で作成し保存しておきます。

まずは保存しておくための変数を **framework クラス** に宣言しましょう。

|||
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・

*   // 差し替え用ピクセルシェーダー
*   Microsoft::WRL::ComPtr<ID3D11PixelShader> replaced_pixel_shader;

    framework(HWND hwnd);
    ~framework();

    ・・・省略・・・
```

宣言したピクセルシェーダを **framework::initialize()** で生成しましょう。
**framework.cpp** では **create_ps_from_cso() 関数** を利用するので **shader.h** をインクルードしておきましょう。

||
--|--
**framework.h**|
```cpp
    #include "framework.h"
*   #include "shader.h"

    ・・・省略・・・
```

|||
--|--
**framework.cpp**|**framework::initialize()**
```cpp
    sprite_batches[0] = std::make_unique<sprite_batch>(device.Get(), L".\\resources\\player-sprites.png", 2048);

*   create_ps_from_cso(device.Get(), "effect_ps.cso", replaced_pixel_shader.GetAddressOf());

    return true;
```

作成したピクセルシェーダーは **replaced_pixel_shader** として、どのスプライトにも適用されず保管された状態となります。

---

### <span style="color:#334488;">作成したピクセルシェーダーに差し替える</span>

次に先ほど作成したピクセルシェーダーである **replaced_pixel_shader** をスプライトバッチで受け取り差し替えます。

**sprite_batch** でピクセルシェーダーが設定されているのは描画開始部分になります。
なので改良する部分は描画開始部分です。

考え方としては、

* sprite_batch のコンストラクタで作成したピクセルシェーダーはデフォルトのピクセルシェーダーとして利用
* 描画開始部分で差し替え用のピクセルシェーダーを受け取ったら差し替え用のピクセルシェーダーを利用

ということで sprite_batch::begin() の引数に差し替え用のピクセルシェーダーを受け取れる引数を追加するのですが、この引数に差し替え用のピクセルシェーダーが代入されていなければ、今までのピクセルシェーダーを使うように実装していきます。

まずは引数を追加します。この際、代入されていなくても大丈夫なように引数を追加します。

**sprite_batch** の以下の部分を

```
void begin(ID3D11DeviceContext* immediate_context);
```

以下のように変更してください。

|||
--|--
**sprite_batch.h**|**sprite_batch**
```cpp
*   void begin(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader = nullptr); 
```

このように**デフォルト引数**を設定することでピクセルシェーダーに何も代入されなければ **nullptr** が自動で代入されることになります。
それでは定義部分も変更していきましょう。

**sprite_batch.cpp** の以下の部分を

```
void sprite_batch::begin(ID3D11DeviceContext* immediate_context)
{
    vertices.clear();
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
}
```

以下のように変更してください。
||
--|--
**sprite_batch.cpp**|

```cpp
*   void sprite_batch::begin(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader)
    {
        vertices.clear();
        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
*       replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0) : immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
        immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
    }
```

定義側の引数も追加します。
変更したコード部分を見てみましょう。

```
replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0) : immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
```

**replaced_pixel_shader** とありますがつまり **replaced_pixel_shader != nullptr** なら **true** なので、差し替え用のピクセルシェーダーが代入されていたら、以下の処理を

    immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0)

差し替え用のピクセルシェーダーが **nullptr** なら以下の処理を行うというものです。

    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0)

仕組み自体は単純です。
これで差し替えの準備が出来ました。実際に差し替えてみましょう。

**framework::render(...)** の以下の部分を

```
sprite_batches[0]->begin(immediate_context.Get());
```

以下のように変更しましょう。

```cpp
*   sprite_batches[0]->begin(immediate_context.Get(), replaced_pixel_shader.Get());
```

第２引数に保管していた差し替え用のピクセルシェーダーを設定しました。
起動して見た目が変化していれば完成です。

ちなみに上記のシェーダーをそのまま記載していた場合は以下のような表示になります。

@import "images/差し替え.png"

これで、**sprite_batch クラス**を増やすことなく見た目を変化させるシェーダーのみを入れ替えることが出来るようになりました。

---

### <span style="color:#334488;">スプライトバッチの描画時にテクスチャを差し替える</span><a name="演習手順７_"></a>

    演習手順７

じゃあ次はテクスチャも途中で入れ替えてみようという事です。

まずは、差し替え用のテクスチャを用意します。

|||
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
    Microsoft::WRL::ComPtr<ID3D11PixelShader> replaced_pixel_shader;
*   // 差し替え用のテクスチャ
*   Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> replaced_shader_resource_view;
	
    framework(HWND hwnd);
    ・・・省略・・・
```

次に先ほどの手順と同じように別途単体で作成します。
この際　**texture.h** をインクルードしておいてください。

||
--|--
**framework.cpp**|

```cpp
    #include "framework.h"
    #include "shader.h"
*   #include "texture.h"

    ・・・省略・・・
```


|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    create_ps_from_cso(device.Get(), "effect_ps.cso", replaced_pixel_shader.GetAddressOf());

*   D3D11_TEXTURE2D_DESC replaced_texture2d_desc;
*   load_texture_from_file(device.Get(), L".\\resources\\vice.png", replaced_shader_resource_view.GetAddressOf(), &replaced_texture2d_desc);

    return true;
```

差し替える画像は適当なものを選んでいます。

---

### <span style="color:#334488;">作成したテクスチャに差し替える</span>

考え方はピクセルシェーダーの時と同じでテクスチャの情報である **replaced_shader_resource_view** をスプライトバッチの描画開始部分で受け取り差し替えるだけです。

先ほどと同じように引数から追加していきます。**デフォルト引数** として設定し **nullptr** を設定する考え方もまったく同じです。

**sprite_batch** の以下の部分を

```
void begin(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader = nullptr);
```

以下のように変更してください。

|||
--|--
**sprite_batch.h**|**sprite_batch**
```cpp
*   void begin(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader = nullptr, ID3D11ShaderResourceView* replaced_shader_resource_view = nullptr); 
```

次に定義部分を変更していきます。差し替えテクスチャがあったら差し替えテクスチャを利用、無ければ今までのテクスチャを利用という考え方は同じですが、テクスチャが変化するということは全体的な幅や高さも変化してしまうという事で、その情報を取得しなければならないということでもあります。
なので、差し替え用のテクスチャだった場合は **sprite_batch クラス** のメンバ変数である **texture2d_desc** に対して差し変わったテクスチャの情報を再設定する必要があります。

その部分も含めて実装していきましょう。

**sprite_batch.cpp** の以下の * の部分を

```
*   void sprite_batch::begin(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader)
    {
        vertices.clear();
        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
        replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0) : immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
*       immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
    }
```

以下のように変更しよう。

||
--|--
**sprite_batch.cpp**|

```cpp
*   void sprite_batch::begin(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader, ID3D11ShaderResourceView* replaced_shader_resource_view)
    {
        vertices.clear();
        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
        replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0) : immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

*       if (replaced_shader_resource_view)
*       {
*           // texture2d_desc に差し変わったテクスチャの情報を設定する
*           HRESULT hr{ S_OK };
*           Microsoft::WRL::ComPtr<ID3D11Resource> resource;
*           replaced_shader_resource_view->GetResource(resource.GetAddressOf());
*           Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
*           hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
*           _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
*           texture2d->GetDesc(&texture2d_desc);

*           // 差し変わったテクスチャを設定
*           immediate_context->PSSetShaderResources(0, 1, &replaced_shader_resource_view);
*       }
*       else
*       {
*           // 今までのテクスチャを設定
*           immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
*       }
    }
```

これでテクスチャの差し替えの準備が出来ました。

---

では差し替えてみましょう。

**framework::render(...)** の以下の部分を

```
sprite_batches[0]->begin(immediate_context.Get(), replaced_pixel_shader.Get());
```

以下のように変更しましょう。

```cpp
*   sprite_batches[0]->begin(immediate_context.Get(), replaced_pixel_shader.Get(), replaced_shader_resource_view.Get());
```

第３引数に保管していた差し替え用のテクスチャを設定しました。
起動して以下のような画像が表示されれば完成です。
ピクセルシェーダーが自前で作っている場合は多少見た目が変化していますが、画像自体が変化していれば問題ありません。

@import "images/テクスチャ.png"


---

### <span style="color:#334488;">フォント画像表示</span><a name="演習手順８_"></a>

    演習手順８

フォントを表示するのではなく、文字画像を用意し、フォントが表示されているかのように文字画像を表示させる機能をスプライトに追加しようというものです。

仕様は演習手順に記載されており

* フォント画像ファイルはアスキーコード順に 16 x 16 の文字列が配置された画像ファイル
* フォント画像ファイルは fonts フォルダのものを使用する

です。

あとは描画時に文字列を受け取り、文字に分割して、その文字を数値に変換。
その数値をアスキーコード順に配置された画像ファイルの画像を切り抜いて表示する、ということをします。

fonts フォルダにある画像ファイルを確認してみましょう。

@import "images/font0.png"

256 x 256 の画像です。

アスキーコード順に左上から０番で右下が２５５番です。
わかりづらいかもしれませんが上が２行空白になっています。

考えとしては、大文字の **'A'** を表示する際は、**A** は数値にすると６５番なので **render() メンバ関数** のテクスチャのサイズに対してそれぞれ

    sx  16 * 1
    sy  16 * 4
    sw  16
    sh  16

の値を渡してやることで A の画像が切り抜かれて表示されるという仕組みです。
つまり、後ろ４つの実引数に

    render(..., 16 * 1, 16 * 4, 16, 16);

という値を渡すことができれば良いということです。

ただそれだけだと **"AB"** という文字列を表示するときに **A** の 上に **B** が表示されてしますので、１文字表示したら２文字目は１文字分ずらして表示するという仕組みも合わせて実装します。

それではやっていきましょう。

|||
--|--
**sprite.h**|**sprite**
```cpp
    ・・・省略・・・
    void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);

*   void textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a);
```

||
--|--
**sprite.cpp**|
```cpp
    ・・・省略(上にデストラクタの定義)・・・

*   void sprite::textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a)
*   {
*       // 一文字文の幅と高さを計算
*       float sw = static_cast<float>(texture2d_desc.Width / 16);
*       float sh = static_cast<float>(texture2d_desc.Height / 16);
*       // 現在の文字位置(相対位置)
*       float carriage = 0;

*       // 文字数分だけ render() を呼び出す。
*       for (const char c : s)
*       {
*           LONG sx = c % 0x0F;
*           // 文字を表示。アスキーコードの位置にある文字位置を切り抜いて表示
*           render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, sw * (c & 0x0F), sh * (c >> 4), sw, sh);
*           // 文字位置を幅分ずらす
*           carriage += w;
*       }
*   }
```

代入された文字列を文字に分割して１文字１文字を通常の **render(...)** で描画しているんだというのはわかるかと思います。

---

１文字の幅と高さを計算とありますが、今回の文字画像ファイルは 256x256 で固定されているので 16 と指定できましたが、必ずしも 256x256 である必要はありません。皆さんの使うゲームではフォントのファイルはのサイズは違ったものになっていると思います。

仕様としては

* フォント画像ファイルはアスキーコード順に 16 x 16 の文字列が配置された画像ファイル

が守られていれば問題はありません。

16x16 が守られていれば sx と sy の以下の計算を行う際に

    sx  16 * 1
    sy  16 * 4

x 位置は

    sw * (c & 0x0F)
    
y 位置は

    sh * (c >> 4)

という計算式で位置を計算することが出来ます。仮に **'A'** の場合なら 65 なので、

    sx  16
    sy  64

という結果になります。なので

    sx  16
    sy  64
    sw  16
    sh  16

の部分である 'A' が切り抜かれ、A という文字が表示されることで、A というフォントが表示されたような仕組みが出来上がりました。

---

２文字目以降が被らないようにずらしているという部分は理解できるかと思います。**carriage** に対して幅分を足して、その足した位置に対して **render()** を行っているというだけです。

これで文字列をテクスチャを利用して表示する準備が出来ました。

---

まずは文字として利用する文字画像を生成しましょう。

|||
--|--
**framework.cpp**|**framework::initialize(...)**

```cpp
    ・・・省略・・・
    load_texture_from_file(device.Get(), L".\\resources\\vice.png", replaced_shader_resource_view.GetAddressOf(), &replaced_texture2d_desc);

*   sprites[0] = std::make_unique<sprite>(device.Get(), L".\\resources\\fonts\\font4.png");

    return true;
```

生成したら **framework::render(...)** で適当に文字列を設定して表示してみましょう。
**sprites[0]** を文字表示用のスプライトとして生成したので **sprites[0]** で文字表示用の描画を行います。

|||
--|--
**framework.cpp**|**framework::render(...)**
```cpp
        ・・・省略・・・
        sprite_batches[0]->end(immediate_context.Get());

*       sprites[0]->textout(immediate_context.Get(), "ECC College of Computer & Multimedia", 0, 0, 16, 16, 1, 1, 1, 1);

    #ifdef USE_IMGUI
        ・・・省略・・・
```

起動すると以下のように表示されたでしょうか？
左上に小さく文字が表示されれば完成です。

@import "images/文字列表示.png"

---

### <span style="color:#334488;">リリースビルド</span><a name="リリースビルド_"></a>

今までは **Debug** ビルドによる実行を行っていたかと思います。企業に提出する際は **Release** ビルドによる起動ファイル(.exe) を提出しなければなりません。

その確認です。

いままでの設定が間違っていなければ **Release** に変更してビルドするだけで成功します。

やってみましょう。

Visual Studio の上部タブの下記画像の赤矢印部分の **Debug** という部分をクリックして

@import "images/デバッグビルド.png"

**Release** に変更して以下の画像のようにして下さい。

@import "images/リリースビルド.png"

この状態で起動してエラーがでなければ成功です。

エラーがでて起動できないなどの場合は、以下を確認してみてください。

* UNIT01.pdf の演習手順８
* UNIT01.pdf の演習手順１３
* UNIT05.pdf の演習手順１

上記の３つの設定が **Debug** **Release** ともに設定されているかどうかを確認してみましょう。 

**「3dgp」** の **「プロパティ」** を開くと以下のウィンドウが表示されます。
このウィンドウの左上の **「構成」** で **Debug** と **Release** を切り替える事ができます。

**Debug** や **Release** に切り替えた際に上記 pdf の３つで設定した項目が設定されていなければ設定してください。下の画像の赤で囲んだ部分が設定した項目です。**Debug** の状態だと設定されています。

@import "images/プロパティ.png"

赤矢印部分を **Release** に変更して赤枠で囲った部分を確認してみてください。同じものが設定されていれば問題ありません。

空白になっていたら、同じものを設定してください。

これらの作業を提示した

* UNIT01.pdf の演習手順８
* UNIT01.pdf の演習手順１３
* UNIT05.pdf の演習手順１

の場所で **Debug** **Release** ともに確認してみてください。

<!--
以前行った設定にミスがあったりすると、下記画像のように **Release** にした際に、設定されているべき場所に何も設定されていなかったりなどがあります。

@import "images/設定ミス.png"

このように設定ミスがないかを確認していってみてください。
-->

設定しているけどエラーが出る場合は **「Release のみパスをミスしている」** なども結構あります。

そのほかにもいろいろとエラーの原因はあるので特定は難しいですが、友達に聞くなり、講師に聞くなりして **Release** ビルドができるようになっておきましょう。

---

### <span style="color:#334488;">2D ゲームの開発</span>

    演習手順１０

これまでに学んだ知識で 2D のゲーム開発は可能です。

入力やサウンドはありませんが、入力は調べればすぐに見つかります。見つからなければ友達に聞くなり、講師に聞くなりして下さい。

これまで学んだ技術を使って簡単でもいいので 2D ゲーム開発を行い、

* DirectX によるゲームの開発というものがどういうものなのか？
* もっといろんなテクスチャを早く読み込むにはどうすればよいか？
* スプライトのアニメーションをさせるにはどうすればよいか？
* キーボード入力するにはどうすればよいか？
* サウンドを再生するにはどうすれ良いか？
* 2D の物理計算を行うにはどうすればよいか？

など、いろいろと考えてみてください。

