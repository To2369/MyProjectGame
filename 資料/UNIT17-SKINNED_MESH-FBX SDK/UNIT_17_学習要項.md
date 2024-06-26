<!--
UNIT17_LEARNING UNIT17 学習要項
SKINNED MESH FBX SDK

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

FBX SDK
Scene
Node
-->

# UNIT17_LEARNING SKINNED MESH FBX SDK 学習要項

---

以下のサイトを抜粋したので詳しくは以下のサイトなどで

https://yttm-work.jp/model_render/model_render_0008.html

---

## <span style="color:#334488;">FBX SDK</span>
#### <span style="color:#334488;">FBX SDK</span>

FBX ファイルを読み込み解析する為のフレームワーク。
ジオメトリのエクスポートに優れており、最も人気のある3Dアプリケーションの多くでサポートされている。

##### FBX ファイル

Autodesk が所有しているデジタルコンテンツ作成(DCC)アプリケーション間の相互運用性を提供するために使用されているファイル形式。

ファイル識別子は **.fbx** です。


---

## <span style="color:#334488;">Node</span>
#### <span style="color:#334488;">ノード</span>

FBX ファイルからポリゴンを表示するために必要な頂点座標、法線、テクスチャ座標などなどのさまざまな情報を取得していきます。それらの情報が含まれているのが FBX における**ノード**になります。

各ノードは様々なデータを保存しており、保存しているデータの数や種類は **FbxNodeAttribute** ごとに異なっています。ノードの種類である Attribute は FBX SDK の中で列挙型が用意されています。
Attribute の種類は 

* eMesh:メッシュ
* eSkeleton:ボーン
* eCamera:カメラ
* eLight:ライト
* eUnknown:不明

などがあり、それぞれの種類を調べれば以下のような、Attribute のノードごとにどのような情報が保存されているかがわかるかと思います。

@import "images/ノード01.png"

FBX の**ノード**は以下のようなツリー構造になっています。

@import "images/ノード02.png"

ツリー構造になっているので **Root** の**ノード**さえ取得出来れば、すべての**ノード**にアクセスでき取得することができるという仕組みになっています。

---

## <span style="color:#334488;">Scene</span>
#### <span style="color:#334488;">シーン</span>

頂点座標や法線などの情報を取得するためには**ノード**の情報が必要であるという説明を上記でしました。
**ノード**を取得する為には**ルートのノード**が必要であるという説明もしました。

この**ルートのノード**を取得するためには**シーン**が必要になります。

@import "images/シーン01.png"

読み込むファイルを**シーン**である **FbxScene** に流し込み **GetRootNode()** メンバ関数を利用して**ルートのノード**を取得します。