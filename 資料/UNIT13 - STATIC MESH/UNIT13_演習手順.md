<!--
UNIT13_EXERCISE UNIT13 学習要項
STATIC MESH - GEOMETRY

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

OBJ File format
OBJ File parser
-->

# UNIT13_EXCERCISE STATIC MESH - GEOMETRY 演習手順

---

    演習手順１

スタティックメッシュ用の専用のシェーダーを作成します。
まずは geometric_primitive と同じものを利用するので以下の３つのファイルを作成し、中身は頂点シェーダー、ピクセルシェーダー、シェーダーヘッダーそれぞれ geometoric_primitive のシェーダーのコードをコピペして下さい。


|作成するファイル|
--|--
**static_mesh.hlsli**|
**static_mesh_vs.hlsl**|
**static_mesh_ps.hlsl**|

---

    演習手順２－１

新規に以下のファイルを追加してください

|作成するファイル|
--|--
**static_mesh.h**|
**static_mesh.cpp**|

---

    演習手順２－２

演習手順にあるとおり static_mesh のクラスを定義していきます。わかる人は geometric_primitive.h の gemetric_primitive クラスをコピペして一部変更して作っても構いません。

|実装するファイル|
--|--
**static_mesh.h**|

```cpp
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>

/// <summary>
/// スタティックメッシュ
/// </summary>
class static_mesh
{
public:
    /// <summary>
    /// 頂点フォーマット
    /// </summary>
    struct vertex
    {
        DirectX::XMFLOAT3 position;	// 座標
        DirectX::XMFLOAT3 normal;	// 法線
    };

    /// <summary>
    /// 定数バッファ用フォーマット
    /// </summary>
    struct constants
    {
        DirectX::XMFLOAT4X4 world;			// ワールド行列
        DirectX::XMFLOAT4 material_color;	// マテリアルカラー
    };

    private:
    // 頂点バッファオブジェクト
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    // インデックスバッファオブジェクト
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

    // 頂点シェーダーオブジェクト
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    // ピクセルシェーダーオブジェクト
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    // 入力レイアウトオブジェクト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    // 定数バッファオブジェクト
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

    public:
    static_mesh(ID3D11Device* device, const wchar_t* obj_filename);
    virtual ~static_mesh() = default;


    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="immediate_context">デバイスコンテキスト</param>
    /// <param name="world">親のワールド行列</param>
    /// <param name="material_color">マテリアルカラー</param>
    void render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color);

    protected:
    /// <summary>
    /// 頂点バッファオブジェクトの作成
    /// </summary>
    /// <param name="device"></param>
    /// <param name="vertices">頂点情報</param>
    /// <param name="vertex_count">頂点数</param>
    /// <param name="indices">インデックス情報</param>
    /// <param name="index_count">インデックス数</param>
    void create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
        uint32_t* indices, size_t index_count);
};
```

---

    演習手順２－３

static_mesh のコンストラクタの実装を行っていきます。
前回同様この部分でメッシュの頂点データと頂点インデックスの設定を行い **create_com_buffers()** に渡すという処理を実装します。

つまり今回は OBJ ファイルを開き、OBJ ファイルを解析することで OBJ ファイルに記載されている頂点データや頂点インデックスを取り出し設定して最終的に **create_com_buffers()** に渡すという事をします。 

必要な情報は、「頂点の座標」と「法線」、そして「頂点インデックス」です。
OBJ ファイルには様々な情報が含まれていますが、現状の私たちのコードでモデルを描画する為に必要な情報はこの３つだけです。

なので OBJ ファイルにある「頂点の座標」「法線」「頂点インデックス」の情報だけを取り出して **vertices** と **indecies** に設定し **create_com_buffers()** に渡してあげればメッシュは表示される、という理屈です。

それではやってみましょう。

|実装するファイル|
--|--
**static_mesh.cpp**|

```cpp
#include "static_mesh.h"
#include "shader.h"
#include "misc.h"
#include <vector>
#include <fstream>

using namespace DirectX;

static_mesh::static_mesh(ID3D11Device* device, const wchar_t* obj_filename)
{
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t current_index{ 0 };

    std::vector<XMFLOAT3> positions;
    std::vector<XMFLOAT3> normals;

    std::wifstream fin(obj_filename);
    _ASSERT_EXPR(fin, L"'OBJ file not found.");

    // 行の情報
    wchar_t command[256];
    while (fin)
    {
        // １番上にある１行を取得
        fin >> command;

        if (0 == wcscmp(command, L"v"))
        {
            // 頂点座標を取得
            float x, y, z;
            // 区切り単位でデータを取得していく
            fin >> x >> y >> z;
            positions.push_back({ x, y, z });

            // １番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vn"))
        {
            // 法線を取得
            float i, j, k;
            fin >> i >> j >> k;
            normals.push_back({ i, j, k });

            // １番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"f"))
        {
            for (size_t i = 0; i < 3; i++)
            {
                vertex vertex;
                size_t v, vt, vn;

                // １番上にある１行目の１つ目のデータを取得し
                // データのうちの１つ目の数値を取得
                fin >> v;
                vertex.position = positions.at(v - 1);
                if (L'/' == fin.peek())
                {
                    // １文字削除
                    fin.ignore(1);
                    if (L'/' != fin.peek())
                    {
                        // スラッシュ区切りで次の数値を取得
                        fin >> vt;
                    }
                    if (L'/' == fin.peek())
                    {
                        // １文字削除
                        fin.ignore(1);
                        // スラッシュ区切りで次の数値を取得
                        fin >> vn;
                        vertex.normal = normals.at(vn - 1);
                    }
                }

                // 頂点データを設定
                vertices.push_back(vertex);

                // 頂点インデックスを設定
                indices.push_back(current_index++);
            }

            // １番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
        else
        {
            // １番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
    }
    fin.close();

    create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

    HRESULT hr{ S_OK };

    // 入力レイアウトオブジェクトの設定
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    // 頂点シェーダーオブジェクトの作成
    create_vs_from_cso(device, "static_mesh_vs.cso", vertex_shader.GetAddressOf(),
        input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    // ピクセルシェーダーオブジェクトの作成
    create_ps_from_cso(device, "static_mesh_ps.cso", pixel_shader.GetAddressOf());

    // 定数バッファの作成
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
```

OBJ ファイルを開き、繰り返し毎に１行読み込んで、頭文字のキーワードをチェックしてそれぞれの処理を行っています。
それぞれの処理の最後に一番上の１行を削除しているので、繰り返しの最初の１行を読み込む処理が必ず次の行になっています。

以下は OBJ ファイルのキーワードと説明です。

|キーワード|説明|
--|--
**v**|座標データ
**vn**|法線データ
**f**|頂点データと頂点インデックスの情報

f の部分に、「生成する三角形の頂点インデックスの情報」も含まれていますが、番号ではなく頂点データがそのまま設定されています。
例えば下記で確認すると

```
f 2/1/1 4/2/1 1/1/1
f 8/2/2 6/4/2 5/3/2
```

左から順に

```
2/1/1
```

が indecies の 0 番に設定され、次の

```
4/2/2
```

が indecies の 1 番に設定され、右までいって下の

```
8/2/2
```

は indecies の 3 番に設定されるという具合に並んでいる順にカウントされます。
記載されている番号はスラッシュを**デリミタ**として左から

    OBJファイル上の頂点座標の番号
    OBJファイル上のテクスチャ座標の番号
    OBJファイル上の法線の番号

になっているので、この３つの番号がセットで頂点データになります。

**デリミタ(またはセパレータ)** とは区切り文字の事を言います。解析に際しては必ずこの区切り文字単位でデータを解析していきます。

今回利用されている**デリミタ**は３つあります。

    頂点データのそれぞれの情報の番号を分けるための
        スラッシュ /
    キーワードやその他のデータを分けるための
        スペース
    １行１行を分けるための
        改行

です。

後はコード内のコメントを見てもらえればデリミタで区切ってキーワードをチェックしたり、データを振り分けたりしているだけだというのがなんとなくわかるかと思います。

**create_com_buffers()** の呼び出し以降は

    入力レイアウトオブジェクトの生成
    頂点シェーダーオブジェクトの生成
    ピクセルシェーダーオブジェクトの生成
    定数バッファの生成

になるので **geometric_primitive** にあるものをコピペしています。

---

    演習手順２－４
    演習手順２－５

**create_com_buffers()** と **render()** の定義を実装しますが、**geometric_primitive** と同じなのでどちらもコピペして下さい。 

---

    演習手順３

スタティックメッシュクラスが実装できたら framework クラスに変数を宣言しましょう。

#### framework.h の上の方に static_mesh.h をインクルードしておいてください。

|実装するファイル|実装するクラス|
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
    std::unique_ptr<geometric_primitive> geometric_primitives[8];

*   // スタティックメッシュ
*   std::unique_ptr<static_mesh> static_meshes[8];

    ・・・省略・・・
```

---

    演習手順４

**framework:initialize()** メンバ関数でスタティックメッシュオブジェクトを生成しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・

*   static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\cube.obj");

    // 定数バッファオブジェクトの生成
    D3D11_BUFFER_DESC buffer_desc{};
    ・・・省略・・・
```

---

    演習手順５－１

**framework::render(...)** で拡大縮小（S）・回転（R）・平行移動（T）行列を計算部分を実装しましょう。

その前に前回の geometric_primitive の描画処理部分である以下の部分をコメントアウトするかマクロで切っておくか削除しておくかしておきましょう。

以下はマクロで切った形になります。

```cpp
*   #if 0
        // 深度テスト：オン、深度ライト：オンの深度ステンシルステートを設定しておく
        immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);
        {
            immediate_context->RSSetState(rasterizer_states[0].Get());
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x - 1.0f , translation.y, translation.z) };
            DirectX::XMFLOAT4X4 world;
            DirectX::XMStoreFloat4x4(&world, S * R * T);
            // 幾何プリミティブの描画
            geometric_primitives[0]->render(immediate_context.Get(), world, material_color);
        }
        {
            immediate_context->RSSetState(rasterizer_states[1].Get());
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x + 1.0f , translation.y, translation.z) };
            DirectX::XMFLOAT4X4 world;
            DirectX::XMStoreFloat4x4(&world, S * R * T);
            geometric_primitives[0]->render(immediate_context.Get(), world, material_color);
        }
*   #endif
```

それでは行列の計算部分を実装しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**|

```cpp
            ・・・省略・・・
            geometric_primitives[0]->render(immediate_context.Get(), world, material_color);
        }
    #endif

*       // 拡大縮小行列
*       DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
*       // 回転行列
*       DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
*       // 平行移動行列
*       DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) };

    #ifdef USE_IMGUI
        ・・・省略・・・
```

---

    演習手順５－２

演習手順に書いてある通り演習手順５－１で計算した行列を合成して(掛け合わせて)ワールド変換行列を作成しましょう。

```cpp
    ・・・省略・・・
    // 平行移動行列
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) };

*   // ワールド行列
*   DirectX::XMFLOAT4X4 world;
*   DirectX::XMStoreFloat4x4(&world, S * R * T);

#ifdef USE_IMGUI
    ・・・省略・・・
```

---

    演習手順５－３

最後にスタティックメッシュオブジェクトの **render(...)** メンバ関数を呼び出します。
この際、先ほど作成したワールド行列とマテリアルカラーを設定して呼び出します。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
    ・・・省略・・・
    DirectX::XMStoreFloat4x4(&world, S* R* T);

*   // 描画
*   static_meshes[0]->render(immediate_context.Get(), world, { 0.5f, 0.8f, 0.2f, 1.0f });

#ifdef USE_IMGUI
    ・・・省略・・・
```

---

    演習手順６

実行すると正立方体が描画されるので確認しましょう。

@import "images/cube.png"

---

    演習手順７

framework の読み込んでいるファイル名の部分を変更して

    torus.obj

を読み込んでトーラスが描画されるか確認してみましょう。

@import "images/torus.png"