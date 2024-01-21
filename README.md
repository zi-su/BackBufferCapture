# 概要
UnrealEngine5でバックバッファをキャプチャするサンプルソースです。  
UE5で追加されたデリゲートを使用し、UI描画前の段階をレンダーターゲットに描き込みます。  

# 使い方
レベルブループリントなどでBackBufferCaptureSubsystemのSetRenderTargetで任意のレンダーターゲットテクスチャを指定します。  
レンダーターゲットテクスチャが指定されるとサンプルは毎フレーム描き込みしています。  
ゲームスクリプトからのリクエストでキャプチャ実行するように変更すると使いやすいです。  

![サンプル動作](https://github.com/zi-su/BackBufferCapture/assets/963325/4b5f119a-62b3-40af-830e-47c863b9ee90)
