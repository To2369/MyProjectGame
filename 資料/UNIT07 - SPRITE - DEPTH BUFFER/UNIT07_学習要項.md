<!--
UNIT07_LEARNING UNIT07 学習要項
SPRITE - DEPTH BUFFER

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;">

Depth testing
Depth writing
-->

# UNIT07 SPRITE - DEPTH BUFFER 学習要項

---

## <span style="color:#334488;">Depth writing</span>
#### <span style="color:#334488;">深度書き込み</span>

ピクセルを表示するときに深度(奥行き)情報を書き込むか指定します。深度情報を書き込んだ後に、深度テストが有効になっているピクセルを描画すると、そのピクセルが奥にある場合、描画されなくなります。

@import "images/深度テスト1.png"

---

## <span style="color:#334488;">Depth testing</span>
#### <span style="color:#334488;">深度テスト</span>

UNIT01 で説明した時は「深度テストは描画されたピクセルが現在描画されているピクセルよりも手前 or 奥かどうかを判断し、その結果が出力するかを決めます。」でした。

もう少し付け加えて説明すると、深度情報が書き込まれている時に、深度テストが有効だと書き込まれている深度情報より奥に描画された場合、描画されなくなります。

@import "images/深度テスト2.png"

プログラム側の処理としては **深度バッファ** に対して **深度値(Z値)** を書き込むことで、値が小さいものが手前に表示されるという処理になります。

深度テストの設定は下記の COM オブジェクトで設定します。

    ID3D11DepthStencilState