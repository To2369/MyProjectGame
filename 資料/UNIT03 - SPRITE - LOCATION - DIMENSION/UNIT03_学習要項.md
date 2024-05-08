<!--
    UNIT02 SPRITE 学習要項.md

    <div style="text-align: center;"></div>

    <span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

    <a href="https://github.com/KhronosGroup/Vulkan-ValidationLayers" class="Link--external">オープンソース</a>

    * [検証レイヤーとは](#01_)
    ## <span style="color:#334488;">検証レイヤーとは</span><a name="01_"></a>
-->

# <div style="text-align: center;"><span style="color:#334499; font-size:100%; font-weight: bold; ">UNIT03 SPRITE - LOCATION/DIMENSION 学習要項</span></div>

* 目次
    * [Affine transformation](#01_)
    * [Screen coordinates](#02_)
    * [Transformation from screen coordinates to NDC](#03_)

---

## <div style="text-align: center;"><span style="color:#334499; font-size:100%; font-weight: bold; ">Affine transformation</span></div><a name="01_"></a>

「**アフィン変換**」とは線形変換による画像の変形と平行移動を行う変換のことです。

---

##### 線形変換

線形変換とは、座標を変形する（座標変換する）演算になります。
線形変換の特徴として直線上に点A,B,Cが並んでいたとき、変換の前後でAB:BCの比が変化しないという特徴もあります。

以下の図で考えると、この 点O の座標を変形することにより、各頂点が他の座標に移動することになります。
例えば左図の 中心O の右２マスに 点A があった場合、右の図だとどの位置に 点A が表示されるかは理解できるはずです。

@import "images/線形変換01.png"

---

##### アフィン変換

上記のような画像の回転や移動をプログラム上のポリゴンに対して行うためにはどのように考えなければならないのか？

    右に５移動したら？
    ９０度回転したら？
    横に２倍伸びたら？

それぞれの処理を行う場合、ポリゴンの頂点である４点すべてに対して、なにかしらの計算を行う必要があります。

@import "images/アフィン変換.png"

元画像のそれぞれの座標の計算を以下の行列計算式で計算することが画像に対する**アフィン変換**となります。

$
\begin{pmatrix} x' \\ y' \end{pmatrix} =
\begin{pmatrix} a & b \\ c & d \end{pmatrix}
\begin{pmatrix} x \\ y \end{pmatrix}
$

    ( x , y ) 元の座標
    ( x', y') 計算後の座標

上の式に対してそれぞれ移動、回転、拡縮の行列式が決められており、それぞれの処理したい式に値をあてはめることで、画像の頂点座標を簡単に変換できるようになります。

---

まずは座標を回転させるためのアフィン変換行列から説明します。

#### <span style="color:#334488;">座標を回転させるためのアフィン変換行列</span>

$
\begin{pmatrix} x' \\ y' \end{pmatrix} =
\begin{pmatrix} \cos\theta & -\sin\theta \\ 
\sin\theta & \cos\theta \end{pmatrix}
\begin{pmatrix} x \\ y \end{pmatrix} =
\begin{pmatrix} \cos\theta*x+-\sin\theta*y \\ \sin\theta*x+\cos*y \end{pmatrix}
$

ちなみに(10,10)を90°回転させると以下になります。(角度に設定されている値はラジアン角です)

$
\begin{pmatrix} 0 & -1 \\ 1 & 0 \end{pmatrix}
\begin{pmatrix} 10 \\ 10 \end{pmatrix} =
\begin{pmatrix} 0*10-1*10 \\ 1*10+0*10 \end{pmatrix} =
\begin{pmatrix} -10 \\ 10 \end{pmatrix}
$

---

次は座標を拡縮させるためのアフィン変換行列を説明します。

#### <span style="color:#334488;">座標を拡縮させるためのアフィン変換行列</span>

$
\begin{pmatrix} x' \\ y' \end{pmatrix} =
\begin{pmatrix} X & 0 \\ 0 & Y \end{pmatrix}
\begin{pmatrix} x \\ y \end{pmatrix} =
\begin{pmatrix} X * x + 0 * y \\ 0 * x + Y * y \end{pmatrix}
$

ちなみに(10,10)をXに2倍、Yに0.5倍させると

$
\begin{pmatrix} 2 & 0 \\ 0 & 0.5 \end{pmatrix}
\begin{pmatrix} 10 \\ 10 \end{pmatrix} =
\begin{pmatrix} 2 * 10 + 0 * 10 \\ 0 * 10 + 0.5 * 10 \end{pmatrix} =
\begin{pmatrix} 20 \\ 5 \end{pmatrix}
$

---

最後に座標を移動させるためのアフィン変換行列を説明します。

#### <span style="color:#334488;">座標を移動させるためのアフィン変換行列</span>

$
\begin{pmatrix} x' \\ y' \end{pmatrix} =
\begin{pmatrix} a & b \\ c & d \end{pmatrix}
\begin{pmatrix} x \\ y \end{pmatrix} +
\boxed{
\begin{pmatrix} X \\ Y \end{pmatrix}
}
$

２次元の座標をアフィン変換する際、上の式のようになり１つの行列式で表現することが出来ません。

そこでダミーの次元を１つ作り１つの行列式で計算を行います。
以下が座標を移動させるためのアフィン変換行列です。

$
\begin{pmatrix} x' \\ y' \\ 1 \end{pmatrix} =
\begin{pmatrix} 1 & 0 & X \\ 0 & 1 & Y \\ 0 & 0 & 1 \end{pmatrix}
\begin{pmatrix} x \\ y \\ 1 \end{pmatrix} =
\begin{pmatrix} 1*x+0*y+X*1 \\ 0*x+1*y+Y*1 \\ 0*x+0*y+1*1 \end{pmatrix}
$

ちなみに (10,10)をXに5、Yに2移動させると

$
\begin{pmatrix} 1 & 0 & 5 \\ 0 & 1 & 2 \\ 0 & 0 & 1 \end{pmatrix}
\begin{pmatrix} 10 \\ 10 \\ 1 \end{pmatrix} =
\begin{pmatrix} 1*10+0*10+5*1 \\ 0*10+1*10+2*1 \\ 0*10+0*10+1*1 \end{pmatrix} =
\begin{pmatrix} 15 \\ 12 \\ 1 \end{pmatrix}
$

ただ、計算を見ればわかるかと思いますが移動も単純に

    現在位置＋移動量=目的位置
    1*x + X*1 = x'
    10 + 5 = 15

という単純な計算でしかないのであまり難しく考えないように。

---

実質的には**２x３の行列**を使って変換を行っていますが、最終的には**アフィン変換**とは画像の拡大縮小、回転、平行移動などをまとめて**３×３の行列**を使って変換する事を**アフィン変換**と呼びます。

$
\begin{pmatrix} x' \\ y' \\ 1 \end{pmatrix} =
\begin{pmatrix} a & b & tx \\ c & d & ty \\ 0 & 0 & 1 \end{pmatrix}
\begin{pmatrix} x \\ y \\ 1 \end{pmatrix}
$

冒頭で説明した回転や拡縮を３ｘ３にあてはめて計算しても結果は変化しません。

---

## <div style="text-align: center;"><span style="color:#334499; font-size:100%; font-weight: bold; ">Screen coordinates</span></div><a name="02_"></a>

スクリーン座標は、コンピュータのディスプレイに表示される画面上の座標系のことです。

スクリーン座標は、画面の左上隅を原点（ 0, 0 ）とする座標系で、水平方向を x 軸、垂直方向を y 軸とします。

スクリーン座標は、マウスポインタの位置や、ウィンドウの位置やサイズ、画像やテキストなどの表示位置を指定するために使用されます。

スクリーン座標は、通常、ピクセル単位で表現されます。

ゲーム開発ではおもに ２D の座標指定に利用されます。

---

## <div style="text-align: center;"><span style="color:#334499; font-size:100%; font-weight: bold; ">Transformation from screen coordinates to NDC</span></div><a name="03_"></a>

##### <span style="color:#334488;">スクリーン座標からNDC座標への変換</span>

---

ポリゴン頂点座標である頂点データとして下記の値を設定しました。

```cpp
// 頂点データ
vertex vertices[]
{
    { { -0.5, +0.5, 0 }, { 1, 1, 1, 1 } },
    { { +0.5, +0.5, 0 }, { 1, 0, 0, 1 } },
    { { -0.5, -0.5, 0 }, { 0, 1, 0, 1 } },
    { { +0.5, -0.5, 0 }, { 0, 0, 1, 1 } },
};
```

---

しかし移動する値を NDC で管理するのは現実的ではありません。

実際によく使われているのはスクリーン座標であり、スクリーンの横幅が 1280 であるのなら 1280 を代入すれば画面の端に表示されるというのが直観的だと理解できます。

---

ですが、CPU GPU は NDC でしか座標を扱っていない為、スクリーン座標で管理していた場合、NDC 座標に変換する必要があります。

@import "images/スクリーンからNDC.png"

下記はスクリーン座標から NDC 座標への変換の式になります。

    2.0f * x / ScreenWidth - 1.0f
    1.0f - 2.0 * y / ScreenHeight

スクリーンの幅高さが1920,1080の場合の960,270の点のNDC座標を計算してみましょう。

    2.0f * 960.0f / 1920.0f - 1.0f = 0
    1.0f - 2.0f * 270.0f / 1080.0f = 0.5

上記の NDC 側の画像の通りの位置になったかと思います。

