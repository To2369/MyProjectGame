<!--
UNIT27_EXCERCISE UNIT27 学習要項
SKINNED MESH CONTROLLING ANIMATION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Controlling animation
-->

# UNIT27_LEARNING SKINNED MESH CONTROLLING ANIMATION 学習要項

---

## <span style="color:#334488;">Controlling animation</span>
#### <span style="color:#334488;">アニメーションの制御</span>

今までは単に FBX ファイルに記載されている情報を取得してその通りに実行するという事を行ってきました。

ここでは取得した情報に対して、プログラム上から値を変更することで FBX ファイルだけではできないアニメーションを作り出し制御するという方法を学んでいきます。

今回表示されるものは以下の **plantune** になります。

@import "images/plantune01.png"

現状アニメーションにあるキーフレームのノードが持っているグローバル行列に対して変更を加えても、ボーン行列に上書きされてしまっているので、何も起こりません。

@import "images/アニメーションデータ01.png"

@import "images/アニメーションデータ02.png"

という事を行っていきます。

最終的に **plantune** の首のノードの行列の位置と回転を変更することで以下のようなアニメーションが描画されるようにしていきましょう。

@import "images/plantune02.png"