<!--
UNIT14_EXERCISE UNIT14 学習要項
STATIC MESH - TEXTURE

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

MTL File format
MTL File parser
-->

# UNIT14_EXCERCISE STATIC MESH - TEXTURE 演習手順

OBJ ファイル中に記載されているマテリアルの詳細が記載されている MTL ファイルを読み取り、MTL ファイル中に記載されているテクスチャファイルを読み取りスタティックメッシュにテクスチャを貼り付けます。

OBJ ファイルには既に貼り付けられてあるクスチャのテクスチャ座標も設定されています。

解析でやるべきことは

    * OBJ ファイルを読み込む
    * 頂点データ(頂点座標、法線、テクスチャ座標)とインデックスデータを解析
    * OBJ ファイルから MTL ファイルを読み込む
    * MTL ファイルからテクスチャを読み込む(マテリアルファイル解析)

です。

頂点データにテクスチャ座標が追加されたのと MTL ファイルの読み込み部分が増えたのが前回との違いです。
今回はこの追加部分を実装していきます。

---

    演習手順１

**framework::initialize()** で static_mesh を生成している以下の部分を

```
static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\torus.obj");
```

以下のように変更してください。

|実装するファイル名|実装するメンバ関数名|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
*   static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Cup\\Cup.obj");
```

---

    演習手順２－１

OBJ ファイルからマテリアルのファイルを読み込み、マテリアルのファイルから画像を読み込んでテクスチャを生成する、ということをします。

つまりこのクラスでテクスチャを扱えるようにする必要があるので以下の２つのメンバ変数を **static_mesh** クラスに宣言しましょう。

#### wstring を利用するので string をインクルードしておいてください。

|実装するファイル|実装するクラス|
--|--
**static_mesh.h**|**static_mesh**

```cpp
        ・・・省略・・・
        DirectX::XMFLOAT4 material_color;	// マテリアルカラー
    };

    // テクスチャファイル名
    std::wstring texture_filename;
    // テクスチャの情報
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

private:
    // 頂点バッファオブジェクト
    ・・・省略・・・
```

---

    演習手順２－２

テクスチャを扱うので頂点フォーマットにテクスチャ座標を追加しましょう。

|実装するファイル|実装するクラス|
--|--
**static_mesh.h**|**stati_mesh::vertex**

```cpp
    DirectX::XMFLOAT3 position;	// 座標
    DirectX::XMFLOAT3 normal;	// 法線
*   DirectX::XMFLOAT2 texcoord;	// テクスチャ座標
```

頂点フォーマットを変更したので入力レイアウトも変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
        ・・・省略・・・
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
*       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
*           D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    ・・・省略・・・
```

---

    演習手順２－３

準備が出来たので OBJ ファイルの解析をしていきましょう。

まずは OBJ ファイルからテクスチャ座標とマテリアルファイル名を取得するための変数を用意します。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
    ・・・省略・・・
    std::vector<XMFLOAT3> normals;
*   std::vector<XMFLOAT2> texcoords;
*   std::vector<std::wstring> mtl_filenames;

    std::wifstream fin(obj_filename);
    ・・・省略・・・
```

テクスチャ座標は頂点座標分存在するのでベクトルで管理しています。
１つのスタティックメッシュに複数のマテリアルファイルを利用している場合もあるのでマテリアルファイル名もベクトル管理しています。

---

次にテクスチャ座標を読み込んでいきましょう。
OBJ ファイルフォーマットのキーワードにおいてテクスチャ座標は **vt** です。
キーワードが **vt** だったらテクスチャ座標を追加するようにしましょう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
        ・・・省略・・・
        // １番上にある１行を削除
        fin.ignore(1024, L'\n');
    }
*   else if (0 == wcscmp(command, L"vt"))
*   {
*       float u, v;
*       fin >> u >> v;
*       texcoords.push_back({u, v});
*       fin.ignore(1024, L'\n');
*   }
    else if (0 == wcscmp(command, L"f"))
    {
        ・・・省略・・・
```

---

前回はインデックスの設定部分にテクスチャ座標の情報が含まれていませんでしたが、今回からは含まれているので、インデックスを設定する部分でテクスチャ座標の OBJ ファイル上での番号の設定を追加し、テクスチャ座標を頂点データに設定しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
    ・・・省略・・・
    if (L'/' != fin.peek())
    {
        fin >> vt;
*       vertex.texcoord = texcoords.at(vt - 1);
    }
    ・・・省略・・・
```

---

最後にマテリアルファイル名を読み込みましょう。
マテリアルファイル名は **mtllib** というキーワード部分に記載されています。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
            ・・・省略・・・
            // 頂点インデックスを設定
            indices.push_back(current_index++);
        }
        fin.ignore(1024, L'\n');
    }
*   else if (0 == wcscmp(command, L"mtllib"))
*   {
*      // マテリアルファイル名を取得
*       wchar_t mtllib[256];
*       fin >> mtllib;
*       mtl_filenames.push_back(mtllib);
*   }
    else
    ・・・省略・・・
```

---

    演習手順２－４

取得したマテリアルファイル名から外部にあるマテリアルファイルを読み込み、さらに中に記載されているテクスチャファイル名を取得します。

この際注意しなければならないのは**パス**です。つまりファイルやフォルダの場所です。

MTL ファイル名を OBJ ファイルから取得するとファイル名だけが記載されています。
今回であれば

    cup.mtl

です。
プロジェクト側からファイルを読み込む際はソリューションのある階層から読み込まなくてはならないので、今回指定しなければならないパスは

    .\\resources\\Cup\\cup.mtl

です。なので読み込むファイル名に対して **.\\\\resources\\\\Cup\\\\** の情報をつなげてから読み込む必要があります。

ですが、ソリューションからの相対的な情報である

    .\\resources\\Cup\\
    
の部分を手で打ち込んだ場合、 OBJ ファイルが Cup ではなくなったらをフォルダの名前が変化してしまうためファイルが見つからないという事でエラーが出てしまいます。

パスの問題を解決するフレームワークはいろいろありますが、今回は

    読み込むファイル(読み込む OBJ ファイル)と同じフォルダ階層のパスの情報が欲しい

という事なので、 **filesystem** というフレームワークを利用します。 
**filesystem** を利用するために以下のヘッダをインクルードしましょう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
    ・・・省略・・・
    #include <fstream>
*   #include <filesystem>

    using namespace DirectX;
    ・・・省略(下に static_mesh のコンストラクタの定義)・・・
```

この仕組みを利用するには **C++17** を利用します。

上記タブにある

    1. 「プロジェクト」を選択し、
    
一番下の

    2. 「d3gp のプロパティ」選択します。

@import "images/設定01.png"

3dgp のプロパティページウィンドウが開いたら、左ペインにある

    3. 「C/C++」のプルダウンを開き

    4. 「言語」を選択

C++ 言語標準の項目が「規定(ISO C++14 標準)」になっていると思うのでその行をクリックすると右側にプルダウンが表示されるので

    5. C++ 言語標準右側の「プルダウンを開く」

@import "images/設定02.png"

プルダウンが開いたら

    6.  ISO C++17 標準(/std:c++17) を選択

@import "images/設定03.png"

設定を反映させるため、

    7. 「適用」を選択して

    8. 「OK」を選択して

完了です。

---

インクルードと設定が完了したらしたら**欲しいパスの情報を持っているファイル名を第１引数**に指定して宣言しましょう。

今回であれば OBJ ファイルのファイル名が代入されている **obj_filename** を第１引数に指定します。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
            fin.ignore(1024, L'\n');
        }
    }
    fin.close();

*   // MTL ファイル名、OBJ ファイル名を設定
*   std::filesystem::path mtl_filename(obj_filename);

    create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
```

次に今設定した部分のパス部分は残してファイル名部分のみをマテリアルファイル名に入れ替える

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
    // MTL ファイル名、OBJ ファイル名を設定
    std::filesystem::path mtl_filename(obj_filename);
*   // ファイル名部分のみ MTL ファイル名に入れ替える
*   mtl_filename.replaece_filename(std::filesystem::path(mtl_filename[0]).filename());

    create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
```

obj_filename に代入されている文字列は以下で

    .\\resources\\Cup\\cup.obj

マテリアルファイル名の **cup.mtl** をパスは残してファイル名部分のみ入れ替えるという事は

    .\\resources\\Cup\\cup.mtl

という文字列になるという事。

これでマテリアルファイルを解析する準備ができたので解析していきます。
解析方法は OBJ ファイルと同じです。
読み込むべきものはテクスチャファイル名でキーワードは **map_Kd** です。

読み込んだテクスチャのファイル名も単なるファイル名でしかないので、上記でやった方法と同じ方法でパスの解決を行います。

それではやってみましょう。

---

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
    mtl_filename.replace_filename(std::filesystem::path(mtl_filenames[0]).filename());

*   // マテリアルファイルを開く
*   fin.open(mtl_filename);
*   _ASSERT_EXPR(fin, L"'MTL file not found.");

*   while (fin)
*   {
*       // 1番上にある１行を取得
*       fin >> command;

*       if (0 == wcscmp(command, L"map_Kd"))
*       {
*           fin.ignore();

*           // テクスチャファイル名を読み込む
*           wchar_t map_Kd[256];
*           fin >> map_Kd;

*           // テクスチャファイル名にパスを取り付ける
*           std::filesystem::path path(obj_filename);
*           path.replaece_filename(std::filesystem::path(map_Kd).filename());
*           texture_filename = path;

*           fin.ignore(1024, L'\n');
*       }
*       else
*       {
*           fin.ignore(1024, L'\n');
*       }
*   }

    create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
```

マテリアル解析でテクスチャの名前を読み込ん後はテクスチャの生成を行いましょう。
#### **load_texture_from_file(...)** を利用するので **static_mesh.cpp** に **texture.h** をインクルードしておいてください。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
        ・・・省略・・・
        hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*       // 読み込んだテクスチャを生成する
*       D3D11_TEXTURE2D_DESC texture2d_desc{};
*       load_texture_from_file(device, texture_filename.c_str(), shader_resource_view.GetAddressOf(), &texture2d_desc);
    }

    ・・・省略(下に static_mesh::render(...)の定義)・・・
```

---

    演習手順３－１

読み込んだテクスチャを扱えるようにシェーダー側にも手を加えていきます。
すでに頂点フォーマットと入力レイアウトにはテクスチャ座標を追加したので、シェーダーヘッダーの出力情報にもテクスチャ座標を追加しましょう。

|実装するファイル|実装する構造体|
--|--
**static_mesh.hlsli**|**VS_OUT**

```cpp
    float4 position : SV_POSITION;
    float4 color : COLOR;
*   float2 texcoord : TEXCOORD;
```

---

    演習手順３－２

頂点シェーダー側も引数でテクスチャ座標を受け取れるようにし出力情報に設定しましょう。

|実装するファイル|
--|--
**static_mesh_vs.hlsl**|

```cpp
*   VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
    {
        ・・・省略・・・

*       vout.texcoord = texcoord;

        return vout;
    }
```

---

    演習手順３－３

pdf にある通りピクセルシェーダー側のサンプラーステートを変更して効果を確認してみましょう。
ピクセルシェーダーが以下のコードになっていない場合は以下のコードにしておいてください。

|実装するファイル|
--|--
**static_mesh_ps.hlsl**|

```hlsl
#include "static_mesh.hlsli"
Texture2D color_map : register(t0);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float4 main(VS_OUT pin) : SV_TARGET
{
    return color_map.Sample(anisotropic_sampler_state, pin.texcoord) * pin.color;
}
```

シェーダー側の用意が完了したら描画部分で読み込んだテクスチャの情報をシェーダー側に設定する処理を追加しておいてください。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::render(...)**

```cpp
    ・・・省略・・・
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

*   // テクスチャの情報をシェーダー側に設定
*   immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

    // 定数バッファとして、ワールド行列とマテリアルカラーを設定
    constants data{ world, material_color };
    ・・・省略・・・
```

起動してテクスチャが貼り付けられ以下の画像が描画されることを確認しましょう。

@import "images/cup01.png"

---

    演習手順４－１

テクスチャの貼り付けられたモデルを読み込むことが出来たところで別のモデルを読み込んでみましょう。

読み込んでいるモデルをを Cup から Bison に変更してください。

```
static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Cup\\Cup.obj");
```


|実装するファイル|実装するメンバ関数|
--|--
framework.cpp|framework::framework(...)

```cpp
*   static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Bison\\Bison.obj");
```

遠くにあるのでモデルを近づけて回転したり拡大したりして確認してみると Cup の時はきれいに貼り付けられていたテクスチャ座標がおかしなことになっています。

理由は pdf 側に記載されていますね。

    UV 座標系は、OBJ ファイルを書き出した DCC ツールによって、原点の場所・V 軸(上下成分)の向きが違う場合がある

です。DDC ツールとは MAYA とか Blender とかのツールの事です。

---

    演習手順４－２

ですので読み込んだモデルがどちらでも対応できるよう、V の部分であるテクスチャ座標の Y の座標を反転できるようにコードを修正していきましょう。

まずは static_mesh のコンストラクタの引数を追加します。

|実装するファイル|実装するクラス|
--|--
**static_mesh.h**|**static_mesh**

```cpp
*   static_mesh(ID3D11Device* device, const wchar_t* obj_filename, bool flipping_v_coordinates);
```

コンストラクタの定義部分の引数も追加しておきましょう。

|実装するファイル|
--|--
**static_mesh.cpp**|

```cpp
*   static_mesh::static_mesh(ID3D11Device* device, const wchar_t* obj_filename, bool flipping_v_coordinates)
```

この引数は static_mesh を生成する際に true だったらテクスチャ座標の上下を反転する、というフラグです。
false だったら今まで通りそのままのテクスチャ座標で表示します。

---

    演習手順４－２

ではこのフラグにテクスチャ座標の上下を反転する処理を実装していきましょう。

true だったら OBJ ファイルを解析後の テクスチャ座標の Y を反転します。

つまり、テクスチャ座標を読み込んでいる以下の部分を

    texcoords.push_back({u, v});

以下のように変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
*   texcoords.push_back({ u, flipping_v_coordinates ? 1.0f - v : v });
```

３項演算子を利用してます。説明の必要はないかもしれませんが一応・・・ 

    if (flipping_v_coordinates == true)
    {
        return 1.0f - v;
    }
    else
    {
        return v;
    }

とだいたい同じ意味ですね。
**return** ではありませんが、表現できないので意味合い的にこの値を返しているよ、という意味という事で **return** を使っています。

これでテクスチャ座標の Y を反転させる実装が完了しました。

---

    演習手順４－３

コンストラクタを追加したので static_mesh の生成部分を変更しておきましょう。
現状 Bison を読み込んでテクスチャ座標が反転のものを表示しているので true を代入しておきましょう

|||
--|--
**framework.cpp**|**framework::initialize()**

```cpp
*   static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Bison\\Bison.obj", true);
```

テクスチャが反転しているマテリアルであっても対応できる **static_mesh** クラスが完成したので、起動して以下の画像が描画されるか確認してみましょう。

@import "images/Bison01.png"