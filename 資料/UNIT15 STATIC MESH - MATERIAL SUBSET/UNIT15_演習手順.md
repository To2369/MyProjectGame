<!--
UNIT15_EXERCISE UNIT15 演習手順
STATIC MESH - MATERIAL SUBSET

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Subsets
Materials
-->


# UNIT15_EXCERCISE STATIC MESH - MATERIAL SUBSET 演習手順

前回の演習ではマテリアルファイルを読み込み、そのファイルに記載されているマテリアル(材質)であるテクスチャを描画しました。

ですが、現状複数のマテリアル(材質)が適応されるようには作られていません。

今回の演習では複数のマテリアル(材質)、つまり複数のテクスチャを使って作られているモデルを描画できるよう実装していきましょう。

---

    演習手順１

複数のマテリアルを作る為に**サブセット**という考え方を利用します。

**サブセット化**する項目は以下の３つです。

    マテリアル名
    頂点インデックス開始番号
    利用する頂点インデックスの数

マテリアル名には読み込むテクスチャの情報が含まれています。
読み込んだテクスチャは指定のポリゴンに貼り付けるのでテクスチャ座標の情報が必要になります。
テクスチャ座標は頂点座標と紐づいています。
そして、OBJ ファイルは以下のフォーマットに従っています。

    OBJ ファイルのキーワード usemtl に続く文字列がマテリアル名で、
    次の行以降に連続するキーワード f がそのマテリアルを使用するポリゴンであり、
    次のキーワード usemtl が出現するまで適用される

つまり、

    uesmtl から 次の usemtl までの f の情報がテクスチャに利用される頂点データの情報で
    DrawIndex に設定される情報だよ

という事です。

なので**サブセット**としてまとめる為に必要な情報としてもう少し詳しく説明すると

    描画に必要なテクスチャ名の情報を持っているマテリアル名
    上のテクスチャに紐づいているテクスチャ座標を含んでいる頂点インデックスの開始番号
    上のテクスチャに紐づいているテクスチャ座標を含んでいる頂点インデックスの開始番号からの数

という事になります。

この３つを**サブセット化**しリスト化しておき、最終的に **static_mesh::render(...)** 部分でリスト化したこの情報を 

    if (紐づいてマテリアル名かどうか)
        DrawIndex(開始番号からの数, 開始番号, 0);

としてやることで、複数のテクスチャであっても描画部分をシンプルに行うことが可能になります。

---

    演習手順２－１

まずは上記にある３つを**サブセット化**していきましょう。

|実装するファイル|実装するクラス|
--|--
static_mesh.h|static_mesh

```cpp
        ・・・省略・・・
        DirectX::XMFLOAT4 material_color;	// マテリアルカラー
    };

*   /// <summary>
*   /// マテリアル名と DrawIndex() に必要な情報部分をサブセット化
*   /// </summary>
*   struct subset
*   {
*       std::wstring usemtl;			// マテリアル名
*       uint32_t index_start{ 0 };		// インデックスの開始位置
*       uint32_t index_count{ 0 };		// インデックスの数(頂点数)
*   };
*   std::vector<subset> subsets;

    // テクスチャファイル名
    ・・・省略・・・
```

---

    演習手順２－２

次にマテリアルの情報を格納しておくための構造体を定義し、複数扱えるようにリスト化(vector)して宣言しておきます。
単体でのマテリアル読み込み対応部分の以下の部分を

```cpp
// テクスチャファイル名
std::wstring texture_filename;
// テクスチャの情報
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
```

以下のように変更しましょう。

|実装するファイル|実装するクラス|
--|--
static_mesh.h|static_mesh

```cpp
// マテリアル
struct material {
    // マテリアル名
    std::wstring name;
    DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
    DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
    DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };
    // テクスチャファイル名
    std::wstring texture_filename;
    // テクスチャの情報
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
};
// 読み込んだマテリアル
std::vector<material> materials;
```

#### vector を利用しているので必要なヘッダーファイルをインクルードしておいてください。

---

    演習手順３－１

サブセットの準備と複数のマテリアルを読み込む準備が出来たらコンストラクタ部分でマテリアルを読み込んでいきます。

マテリアルのキーワードである **usemtl** がきたら、サブセットにあるマテリアル名とインデックス開始番号のみを記録する。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh(...)**

```cpp
        ・・・省略・・・
        mtl_filenames.push_back(mtllib);
    }
*   else if (0 == wcscmp(command, L"usemtl"))
*   {
*       // マテリアル名
*       wchar_t usemtl[MAX_PATH]{ 0 };
*       fin >> usemtl;

*       // マテリアル名とインデックス開始番号を保存
*       subsets.push_back({ usemtl, static_cast<uint32_t>(indices.size()),0 });
*   }
    else
    {
        fin.ignore(1024, L'\n');
        ・・・省略・・・
```

現在取り付けられているインデックスのサイズがインデックス開始番号になる。

---

    演習手順３－２

**usemtl** が来るたびに新しいサブセットが生成され取り付けられる仕組みを上記で作りました。

それぞれのサブセットにはマテリアル名とそのマテリアルが利用する頂点インデックスの開始番号を設定しました。
ここではそれぞれのサブセットが利用する頂点インデックスの数を計算していきます。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh(...)**

```cpp
            ・・・省略・・・
            fin.ignore(1024, L'\n');
        }
    }
    fin.close();

*   // それぞれのサブセットのインデックスの数を計算していく
*   std::vector<subset>::reverse_iterator iterator = subsets.rbegin();
*   iterator->index_count = static_cast<uint32_t>(indices.size()) - iterator->index_start;
*   for (iterator = subsets.rbegin() + 1; iterator != subsets.rend(); ++iterator)
*   {
*       iterator->index_count = (iterator - 1)->index_start - iterator->index_start;
*   }

    // MTL ファイル名、OBJ ファイル名を設定
    std::filesystem::path mtl_filename(obj_filename);
    ・・・省略・・・
```

この部分は OBJ ファイルからインデックス数を計算している部分です。開始位置から次の開始位置までを逆イテレーターを利用して後ろから計算していっています。見ていきましょう。

以下の部分で後ろから順に捜査する為のイテレーターを取得しています。

    std::vector<subset>::reverse_iterator iterator = subsets.rbegin();

次の行は

    iterator->index_count = static_cast<uint32_t>(indices.size()) - iterator->index_start;

はつまり

    最後のサブセットのインデックス数 = メッシュの総インデックス数 - 最後のサブセットのインデックス開始位置

という事です。例えば開始位置が 120 でメッシュの総インデックス数が 300 だとしてら利用されるインデックス数は 180 という事になります。
次の行では繰り返し分になっており逆イテレーターの次のイテレーターからとなっています。通常のイテレーターなら始まりから終わりまでを繰り返すのですが逆イテレーターなので終わりから始まりまでを繰り返します。
処理中のコードでは

    iterator->index_count = (iterator - 1)->index_start - iterator->index_start;

となっています。つまり

    現在のサブセットのインデックス数 = ひとつ前のサブセットの開始位置 - 現在のサブセットの開始位置

という事で、例えば１つ前が先ほどの 120 だとして繰り返し中で現在のサブセットの開始位置が 30 だとすると利用されるインデックス数は 90 という事になります。このままいけば最後のサブセットの利用されるインデックス数が 30 という事になります。

なぜ逆イテレーターを利用しているのか？
仮に通常のイテレーターで考えてみましょう。以下のようにすると・・・

```
for (std::vector<subset>::iterator iterator = subsets.begin(); iterator != subsets.end() - 1; ++iterator) {
    iterator->index_count = (iterator + 1)->index_start - iterator->index_start;
}
```

マテリアルが１つしかない場合繰り返し分の中に入らず、結果インデックス数を設定することが出来ません。

繰り返し分の中に入らせるために無理やり以下のようにしても・・・

```cpp
std::vector<subset>::iterator iterator = subsets.begin();
do {
    iterator->index_count = (iterator + 1)->index_start - iterator->index_start;
} while (iterator != subsets.end() - 1);
```

結局 (iterator + 1) の部分で１つしかマテリアルがなかったら次のイテレータは存在しないので落ちます。

今回は逆からイテレーターを捜査することでスマートにインデックスの計算が行えるようになっています。


---

    演習手順３－３

サブセットに必要な情報をすべて設定したら複数のマテリアルを読み込んで行きます。
今までは単体のマテリアルを読み込むように実装されていたのでその部分を修正していく形になります。

現状エラーが出ているであろう以下の単体でマテリアルを読み込んでいる部分を

```cpp
texture_filename = path;
```

以下のように変更してください。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh(...)**

```cpp
materials.rbegin()->texture_filename = path;
```

これで複数のマテリアルに設定されているテクスチャファイル名を取得していけるようになりました。

---

次にマテリアルファイル側の新規マテリアルのキーワードである **newmtl** をチェックして **newmtl** が検出されたら**マテリアル**を新規で作成してマテリアル名を設定しマテリアル配列(ベクター)に取り付けていきましょう。

処理の順番としてはまずこの処理から先に行われ次に上で行っているテクスチャファイル名の設定が行われていると考えてください。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh(...)**

```cpp
        ・・・省略・・・
        materials.rbegin()->texture_filename = path;
        fin.ignore(1024, L'\n');
    }
*   else if (0 == wcscmp(command, L"newmtl"))
*   {
*       fin.ignore();
*       wchar_t newmtl[256];
*       // 新規でマテリアルを作成
*       material mat;
*       fin >> newmtl;
*       mat.name = newmtl;
*       // マテリアルを取り付け
*       materials.push_back(mat);
*   }
    else
    {
        fin.ignore(1024, L'\n');
        ・・・省略・・・
```

演習手順側では続けて色の検出も行っていますがここでは説明を分けるためにコードを分離致しました。
ということで次に色の検出を行います。マテリアルファイルの色が指定されている部分のキーワードは **Kd** です。
**Kd** を取得して色の情報をマテリアルの Kd に設定しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh(...)**

```cpp
        ・・・省略・・・
        // マテリアルを取り付け
        materials.push_back(mat);

    }
*   else if (0 == wcscmp(command, L"Kd"))
*   {
*       // 色を取得し生成したマテリアルに設定
*       float r, g, b;
*       fin >> r >> g >> b;
*       materials.rbegin()-> Kd = { r, g, b, 1 };
*       fin.ignore(1024, L'\n');
*   }
    else
    {
        fin.ignore(1024, L'\n');
        ・・・省略・・・
```

---

    演習手順３－４

取り付けられているすべてのマテリアルにあるテクスチャファイル名からテクスチャを読み込み、テクスチャの情報とシェーダーリソースビューを生成していきます。

以下のエラーが出ているであろう単体でテクスチャを読み込む実装の形になっている部分を

```cpp
load_texture_from_file(device, texture_filename.c_str(), shader_resource_view.GetAddressOf(), &texture2d_desc);
```

以下のように変更してください。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh(...)**

```cpp
for (material& mat : materials)
{
    load_texture_from_file(device, mat.texture_filename.c_str(),
        mat.shader_resource_view.GetAddressOf(), &texture2d_desc);
}
```

現状 CPP 側でテクスチャの情報を利用していないので

    D3D11_TEXTURE2D_DESC texture2d_desc{};

を複数対応していません。この部分は今後複数対応が必要になった時に material 構造体側に追加し拡張実装していってください。

---

    演習手順４

読み込んだ複数のマテリアルの情報と設定したサブセットの情報を描画部分に反映させます。

サブセットには設定されている頂点を利用するマテリアルの名前が設定されているので、マテリアル配列(ベクター)側にあるマテリアル名と一致するマテリアルを探し、一致したマテリアルにあるテクスチャを利用するように実装しましょう。

エラーがでているであろう単体マテリアル対応の実装がされている **static_mesh::render(...)** の以下の部分を

```cpp
// テクスチャの情報をシェーダー側に設定
immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

// 定数バッファとして、ワールド行列とマテリアルカラーを設定
constants data{ world, material_color };
immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
// 定数(コンスタント)バッファオブジェクトの設定
immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

// インデックスを指定して描画
D3D11_BUFFER_DESC buffer_desc{};
index_buffer->GetDesc(&buffer_desc);
immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
```

以下のように変更しよう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::render(...)**

```cpp
// マテリアル配列(ベクター)にあるマテリアルをすべて描画
for (const material& mat : materials)
{
    // テクスチャの情報をシェーダー側に設定
    immediate_context->PSSetShaderResources(0, 1, mat.shader_resource_view.GetAddressOf());

    // 定数バッファとして、ワールド行列とマテリアルカラーを設定
    constants data{ world, material_color };
    // マテリアルカラーは読み込んだ色も反映
    XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&mat.Kd));
    immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
    // 定数(コンスタント)バッファオブジェクトの設定
    immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

    // サブセットを捜査
    for (const subset& s : subsets)
    {
        // サブセットにあるマテリアル名と一致するものをチェック
        if (mat.name == s.usemtl)
        {
            // 一致したサブセットのインデックスの数と開始番号をを指定
            immediate_context->DrawIndexed(s.index_count, s.index_start, 0);
        }
    }
}
```

---

    演習手順５

確認を行うため複数のマテリアルを持っているモデルを読み込みましょう。
framework::initialize() の static_mesh オブジェクトを生成している以下の部分を

```cpp
static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Bison\\Bison.obj", true);
```

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Mr.Incredible\\Mr.Incredible.obj", true);
```

---

    演習手順６

実行して確認しましょう。