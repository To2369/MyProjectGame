<!--
UNIT08_LEARNING UNIT08 学習要項
SPRITE - BLENDING

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;">

Blend state
Alpha Channel
Blend mode formulas
-->

# UNIT08 SPRITE - BLENDING 学習要項

---

## <span style="color:#334488;">Blend state</span>
#### <span style="color:#334488;">ブレンドステート</span>

ブレンドステートはピクセルシェーダで出力した値をレンダーターゲットに書き込む際に、もともとあった値とどのようにブレンドするかを指定するためのもの。

@import "images/ブレンドステート01.png"

ただ単に画像を手前に表示するだけでは無く、元の画像と混ぜ合わせて表示する仕組みの指定の事をいいます。

@import "images/ブレンドステート02.png"

この際、画面にすでに表示されている色を **Destination**(**DEST, DST**) 、これから表示する色を **Source**(**SRC**) と呼びます。

前回の処理で説明すると先に **render()** を呼び出した **sprite** の色が **DEST** になり、後に **render()** を呼び出した **sprite** の色が **SRC** になります。 

DirectX11 のブレンドの種類には以下の指定があります。

|D3D11_BLEND|説明|
--|--
D3D11_BLEND_SRC_COLOR|これから描画する画の色(R, G, B)
D3D11_BLEND_INV_SRC_COLOR|これから描画する画の色の反転(1.0 - R, 1.0 - G, 1.0 - B)
D3D11_BLEND_SRC_ALPHA|これから描画する画の透明値(A)
D3D11_BLEND_INV_SRC_ALPHA|これから描画する画の色の反転(1.0 - A)
D3D11_BLEND_DEST_COLOR|既に描画されている画の色(R, G, B)
D3D11_BLEND_INV_DEST_COLOR|既に描画されている画の色の反転(1.0 - R, 1.0 - G, 1.0 - B)
D3D11_BLEND_DEST_ALPHA|既に描画されている画の透明値(A)
D3D11_BLEND_INV_DEST_ALPHA|既に描画されている画の色の反転(1.0 - A)
D3D11_BLEND_ONE|1
D3D11_BLEND_ZERO|0

これらを **DEST** と **SRC** に指定することで様々な色の重なりを表現できるようになります。

上記を指定したうえで、**DEST** と **SRC** をどのように演算するかの指定が下記になります。

|D3D11_BLEND_OP|説明|
--|--
D3D11_BLEND_OP_ADD|DEST **＋** SRC
D3D11_BLEND_OP_SUBTRACT|SRC **ー** DEST
D3D11_BLEND_OP_REV_SUBTRACT|DEST **ー** SRC

---

## <span style="color:#334488;">Alpha Channel</span>
#### <span style="color:#334488;">アルファチャンネル</span>

**アルファチャンネル**とはピクセルに対し、色データとは別に持たせた補助データであり、一般的に画素の不透明度を表現します。

値として０．５で半透明になり、０を設定することで完全に透明になります。

ブレンドステートを扱う際にアルファ値を掛け合わせることで半透明な画像を重ねる技法の事を**アルファブレンディング**と呼びます。

---

## <span style="color:#334488;">Blend mode formulas</span>
#### <span style="color:#334488;">ブレンドモード</span>

ブレンドの種類の組み合わせから作られる様々なブレンドの事です。

種類はたくさんありますが、まずは演習で使用する４つを確認してみましょう。

    無効
    アルファ    alpha
    加算        add
    乗算        multiply

基本的な４つです。ネットで調べてもすぐに見つかるはずです。

* 無効

```cpp
// ブレンドを無効化
BlendEnbale = FALSE;
```

ブレンドを無効化しているのでブレンド自体の計算は意味がありません。

* アルファ

```cpp
// ブレンドを有効化
BlendEnbale = TRUE;
```
```cpp
SrcBlend = D3D11_BLEND_SRC_ALPHA
DestBlend = D3D11_BLEND_INV_SRC_ALPHA
```
```cpp
BlendOp = BLEND_OP_ADD
```

SrcBlend の所を見ていきましょう。**D3D11_BLEND_SRC_ALPHA** が設定されています。
**D3D11_BLEND_SRC_ALPHA** を上にある表で確認すると<span style="font-weight:bold;">「これから描画する画の透明値(A)」</span>とあります。

この設定が **SrcBlend** に設定されているので **これから表示される画像** に対して計算が行われるという事で・・・

    SRC の色 * SRC の ALPHA

という考え方になります。
これをシェーダプログラムに書き直すと

```hlsl
src.r * src.a
src.g * src.a
src.b * src.a
```

というコードになります。

DestBlend を見ていきましょう。**D3D11_BLEND_INV_SRC_ALPHA** が設定されています。
**D3D11_BLEND_INV_SRC_ALPHA** を確認すると <span style="font-weight:bold;">「これから描画する画の透明値の反転(1-A)」</span>とあります。

この設定が **DestBlend** に設定されているので **すでに表示されている画像** に対して計算が行われるという事で・・・

    DEST の色 * (1 - SRC の ALPHA)

という考え方になります。

これをシェーダプログラムに書き直すと

```hlsl
dst.r * (1 - src.a)
dst.g * (1 - src.a)
dst.b * (1 - src.a)
```

最後にこれらを **BLEND_OP_ADD** するということはつまり **DEST + SRC** ということなので、

```hlsl
r = dst.r * (1 - src.a) + src.r * src.a
g = dst.g * (1 - src.a) + src.g * src.a
b = dst.b * (1 - src.a) + src.b * src.a
```

r, g, b が最終的な混ぜ合わさった色として表示されます。

仮に下のような画像だった場合、点の位置のピクセルにはどのような色が表示されるのでしょうか？

@import "images/ブレンド01.png"

上記の式に値をいれて確認してみましょう。

四角形を **dst** として (0.0, 0.0, 1.0, 1.0) でアルファ値が 1 の不透明の青とします。
三角形を **src** として (1.0, 0.0, 0.0, 0.3) でアルファ値が 0.3 のかなり透明がかった赤とします。 これらを **アルファ** の **ブレンドモード** で混ぜ合わせると

```hlsl
r = 0.0 * (1 - 0.3) + 1.0 * 0.3
g = 0.0 * (1 - 0.3) + 0.0 * 0.3
b = 1.0 * (1 - 0.3) + 0.0 * 0.3
```

となって

```hlsl
r = 1.0 * 0.3
g = 0.0
b = 1.0 * 0.7
```

結果

```hlsl
r = 0.3
g = 0.0
b = 0.7
```

最終的に、赤 0.3 で 青 0.7 なので、青っぽい紫が表示されるという事になります。

* 加算

```cpp
// ブレンドを有効化
BlendEnbale = TRUE;
```
```cpp
SrcBlend = D3D11_BLEND_SRC_ALPHA
DestBlend = D3D11_BLEND_ONE
```
```cpp
BlendOp = BLEND_OP_ADD
```

**DEST** に設定している **D3D11_BLEND_ONE** は説明には **１** と書かれています。
そのままの意味でこれが **DestBlend** に設定されているので

```hlsl
dst.r * 1
dst.g * 1
dst.b * 1
```

という事です。

* 乗算

```cpp
// ブレンドを有効化
BlendEnbale = TRUE;
```
```cpp
SrcBlend = D3D11_BLEND_ZERO
DestBlend = D3D11_BLEND_SRC_COLOR
```
```cpp
BlendOp = BLEND_OP_ADD
```

**SRC** に設定している **D3D11_BLEND_ZERO** は説明には **０** と書かれています。
これもそのままの意味でこれが **SrcBlend** に設定されているので

```hlsl
src.r * 0
src.g * 0
src.b * 0
```

という事です。

DestBlend に設定している **D3D11_BLEND_SRC_COLOR** は SRC の色を掛けています。
つまり

    Dest の色 * SRC の色

という考え方で

```hlsl
dst.r * src.r
dst.g * src.g
dst.b * src.b
```

