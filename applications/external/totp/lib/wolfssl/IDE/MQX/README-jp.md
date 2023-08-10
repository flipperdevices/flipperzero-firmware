# MQX向けビルド方法
## 概要
このMakefileはwolfSSLライブラリーとサンプルプログラムをMQX向けにビルドするためのものです。
以下のターゲットを含んでいます。
 - wolfssllib: wolfSSL静的ライブラリー
 - test: 暗号アルゴリズムのテスト
 - benchmark: 暗号アルゴリズムのベンチマーク
 - client: TLS クライアントサンプルプログラム
 - server: TLS サーバサンプルプログラム


## 準備
- 事前にGCCをインストールしておいてください。
  GNU Arm Embedded Toolchain, https://developer.arm.com
- 事前にMQXをインストールしておいてください。
  Freescale MQX RTOS 4.1以降, https://www.nxp.com/
  Freescale_MQX_4_1/doc/MQX_Getting_Started.pdfをご参照ください。

## 設定
- wolfSSL コンフィグレーションオプション
　<wolfSSL-root>/IDE/MQX/user_settings.hファイルに必要なオプションを追加または削除してください。

- Makefileの設定
  MQX_ROOT: MQX のインストールパス
  MQXLIB:   リンクするMQX ライブラリのパス
  CC:       コンパイラコマンド
  AR:       ARコマンド
  WOLF_ROOT: Makefileの格納位置を変える場合はこの定義を変更してください
  