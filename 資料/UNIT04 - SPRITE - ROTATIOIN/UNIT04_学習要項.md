<!--
UNIT04_EXERCISE UNIT04 学習要項
SPRITE - ROTATION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;">

Affine transformation
Screen coordinates
Transformation from screen coordinates to NDC
-->

# UNIT04 SPRITE - ROTATION 学習要項

---

前回は座標を移動させるためのアフィン変換を行いました。今回は座標を回転させるためのアフィン変換行列を使って画像を回転させてみましょう。

---

#### <span style="color:#334488;">座標を回転させるためのアフィン変換行列</span>

前回の２次元の行列にダミーの次元を加え、３次元にしたものが下の行列です。

$
\begin{pmatrix} x' \\ y' \\ 1 \end{pmatrix} =
\begin{pmatrix} \cos\theta & -\sin\theta & 0 \\ \sin\theta & \cos\theta & 0 \\
0 & 0 & 1 \end{pmatrix}
\begin{pmatrix} x \\ y \\ 1 \end{pmatrix} =
\begin{pmatrix} \cos\theta*x+-\sin\theta*y + 0*1 \\ \sin\theta*x+\cos*y + 0*1 \\ 0*x +0*y+1*1 \end{pmatrix}\\
$

ダミーの次元を入れた状態で(10,10)を90°回転させても結果は変わりません。

$
\begin{pmatrix} 0 & -1 & 0 \\ 1 & 0 & 0 \\ 0 & 0 & 1 \end{pmatrix}
\begin{pmatrix} 10 \\ 10 \\ 1 \end{pmatrix} =
\begin{pmatrix} 0*10-1*10 + 0*10 \\ 1*10+0*10+0*10 \\ 0 * 10 +0 * 10 + 1 * 1 \end{pmatrix} =
\begin{pmatrix} -10 \\ 10 \\ 1 \end{pmatrix}
$

---



