<!--
UNIT26_EXCERCISE UNIT26 学習要項
SKINNED MESH OBJECT ANIMATION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Object animation
-->

# UNIT26_LEARNING SKINNED MESH OBJECT ANIMATION 学習要項

---

## <span style="color:#334488;">Object animation</span>
#### <span style="color:#334488;">オブジェクト アニメーション</span>

オブジェクトアニメーションとは文字通りオブジェクト全体に対しての行列アニメーションになります。

スケルトンアニメーションはボーンに対して頂点を追従させることでそのボーンが動くと頂点が動き頂点が動くとポリゴンが変化し、見た目がアニメーションするというものでした。

オブジェクトアニメーションはキーフレームごとにモデル全体に対しての行列が設定されているのでモデルの一部分がボーンによって変化するのではなくモデル全体が設定されたキーフレームの行列に従って変化を行うというものです。

@import "images/オブジェクトアニメーション01.gif"