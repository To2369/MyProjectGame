<!--
UNIT24_LEARNING UNIT24 学習要項
SKINNED MESH BIND POSE(SKELETON)

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Skeleton
Bone
Bind pose
Offset transform
-->

# UNIT24_LEARNING SKINNED MESH BIND POSE(SKELETON) 学習要項

---

## <span style="color:#334488;">Skeleton</span>
#### <span style="color:#334488;">スケルトン</span>

3DCGキャラクターにアニメーションを設定する際に用いる機能。

モデルの形に合わせてスケルトン（骨格）を構築し、両者をスキニングし（関連付け）、スケルトンを操作することでモデルにアニメーションを付ける。

スケルトンは、球形のジョイント（関節）と、くさび形のボーン（骨）を組み合わせた階層構造となっており、ジョイントの回転によって操作される。

@import "images/スケルトン01.png"

---

## <span style="color:#334488;">Bone</span>
#### <span style="color:#334488;">ボーン</span>

ボーンとは3Dモデルを変形させながら、アニメーションさせるために必要な要素。

その名の通り人体の骨（ボーン）のような役割で、ボーンを動かしてボーンに3Dモデル(頂点)を追従させてアニメーションさせる。 

@import "images/ボーン.gif"


FBX にはボーンという名称のものは存在しないがクラスターというものがそれに相当する。


---

## <span style="color:#334488;">Bind pose</span>
#### <span style="color:#334488;">バインドポーズ</span>

初期姿勢の行列の事。バインドポーズ、基本姿勢、初期姿勢、バインドポーズ行列、初期姿勢行列。

---

## <span style="color:#334488;">Offset transform</span>
#### <span style="color:#334488;">オフセット行列</span>

初期姿勢の逆行列の事。

どのような時に利用するのか？

仮にボーンを(2,1)から(1,2)に移動させるとして、それを全頂点やろうとすると処理が多い。
一旦原点に移動させてから目的地へ移動させれば目的地への移動行列だけですむので処理が少ない。
オフセット行列は、この原点へ移動させるための行列。

たとえば、以下のように

@import "images/オフセット行列.png"

B の三角形を A の三角形と接している地点から左に60度回転させたい場合、グローバル座標の原点に
移動させてから回転し、また元の位置に戻すという事をします。
