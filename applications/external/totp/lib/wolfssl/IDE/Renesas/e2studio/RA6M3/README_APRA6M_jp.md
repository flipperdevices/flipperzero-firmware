wolfSSL/AlphaProject AP-RA6M-0A ボードデモ　セットアップガイド
=================================================

## はじめに

このフォルダにはルネサス社製 RA 32-bit MCU を搭載するアルファプロジェクト製 AP-RA6M-0A で wolfSSL を動作させるための手順です。
サンプルプログラムには、暗号テスト、ベンチマーク、及びクライアント・サーバーを含んでいます。

wolfSSL のプロジェクトファイルは、wolfSSL と wolfCrypt の両方で構成され、Renesas RA のコンフィグレーションを含む`Renesas RA C/C++ Library Project`としてビルドされます。
その他、ベンチマーク、暗号テスト、及びクライアント・サーバーのサンプルプログラムは、`Renesas RA C Project Using RA Library`としてビルドされます。

プロジェクトの概要と全ての関連するソフトウェアコンポーネントに関する情報を下記になります。

### プロジェクトの概要
|要素|名前/バージョン|
|:--|:--|
|e2studio|2020-07|
|Board|AP-RA6M-0A|
|Device|R7FA6M3AH3CFC|
|Toolchain|GCC ARM Embedded|
|FSP Version|1.3.0|

#### 必要なソフトウェアコンポーネント
|コンポーネント|バージョン|
|:--|:--|
|Board Support Package Common Files|v1.3.0|
|Arm CMSIS Version 5 - Core (M) |v5.7.0|
|Board support package for R7FA6M3AH3CFC |v1.3.0|
|Board support package for RA6M3|v1.3.0  |
|Board support package for RA6M3 - FSP Data|v1.3.0|
|FreeRTOS|v1.3.0|
|FreeRTOS - Buffer Allocation 2 |v1.3.0|
|FreeRTOS+TCP|v1.3.0|
|r_ether to FreeRTOS+TCP Wrapper|v1.3.0|
|Ethernet |v1.3.0   |
|Ethernet PHY|v1.3.0|
|I/O Port|v1.3.0    |
|BSP-Board|v1.2.0   |

## セットアップ手順

プロジェクトのフォルダーには、ビルドに必要なファイルが不足しています。そららのファイルをダミーのプロジェクトを作成し補います。

次に続くステップは、不足しているファイルを作成し、それらを必要としているプロジェクトにコピーする手順です。

1.) [アルファプロジェクト社のホームページ](https://www.apnet.co.jp/product/ra/ap-ra6m-0a.html)からサンプルプログラムをダウンロード

+ ダウンロードしたサンプルプログラムを適当なフォルダーへ解凍

2.) e2Studio で'ダミー' Renesas RA C Library プロジェクトを作成

+ ファイル→新規→`RA C/C++ Project`をクリック
+ `Renesas RA C/C++ Library Project`を選択し、次へをクリック
+ 'dummy_library` とプロジェクト名を入力します。
+ `Board:` ドロップダウンから `EK-RA6M3T`を選択します
+ `RTOS: No TROS` を `FreeRTOS` を選択します。
+ `Build Artifact Selection` の `Static Library`を選択し、次へをクリック
+ `FreeRTOS - Minimal - Static Allocation` を選択し、終了をクリック

3.) e2Studio で 'ダミー'の Renesas RA C/C++ Project Using RA Library を作成

+ ファイル→新規→`RA C/C++ Project`をクリック
+ `Renesas RA C/C++ Library Project`を選択し、次へをクリック
+ 'dummy_app` とプロジェクト名を入力します。
+ `Board:` ドロップダウンから `EK-RA6M3T`を選択します
+ `RTOS: No TROS` を `FreeRTOS` を選択しまし、次へをクリック
+ `Build Artifact Selection` の `Executable Using an RA Static Library`を選択し、終了をクリック
+ 'dummy_app` とプロジェクト名を入力し、次へクリック
+ `RA library project`の `Select RA Library`から, `dummy_library`を選択し、終了をクリック

4.) 全ての wolfSSL e2studio プロジェクトをインポート

+ メニューの「ファイル」→「ファイル・システムからプロジェクトを開く」をクリック
+ インポート元の `ディレクトリー...` をクリック
+ RA6M3 フォルダーを選択。wolfssl/IDE/Renesas/e2studio/RA6M3
+ Eclipseのプロジェクトではない、RA6M3を除外します。
   その他、ベンチマーク、暗号テスト、クライアント・サーバーの各プロジェクトは選択しておく。
+ 終了をクリック

5.) `dummy_library`からwolfSSL_RA6M3Gへ必要なファイルをコピー

+ `dummy_library` と `wolfSSL_RA6M3G` プロジェクトを開く
  プロジェクト名横にある矢印マークをクリック
+ `dummy_library` の以下のフォルダーとファイルを選択

    `ra/`  
    `ra_gen/`  
    `ra_cfg/`  
    `script/`

+ 選択したフォルダーとファイルを `wolfSSL_RA6M3G`プロジェクトに貼り付け
+ `dummy_library`プロジェクトは削除しても構いません
+ `APRA6M0A.pincfg` を解凍した ap_ra6m_0a_sample\sample\ap_ra6m_0a_ether_sample から `wolfSSL_RA6M3G`プロジェクトへコピー
+ `wolfSSL_RA6M3G`フォルダー内の `R7FA6M3AH3CFC.pincfg` は削除します。
+ プロジェクトに必要なファイルを生成します。
+ `Open RA Configuration`(上部のアイコンバーにある灰色歯車ボタン)をクリック

  + `BSP`　タブに移動し、CMSIS Pack のインポートボタンをクリック
  + インポート画面で、CMSIS pack ファイルを指定
   ステップ 1で解凍したap_ra6m_0a_sample\sampleフォルダー中の AP.APRA6M0A.x.x.x.pack を指定します。
  + `APRA6M0A` を Board として指定
  + `Pins`タブに移動し、`APRA6M0A.pincfg`を選択
  + `Stacks`タブに移動し、Heap 4 stack を New Stack から追加
  + `Generate Project Content`（右上部にある緑色アイコン）をクリックし、ファイルを生成
+ `wolfSSL_RA6M3G`をビルド

6.) `dummy_app` から必要なファイルを`./IDE/Renesas/e2studio/RA6M3/common/ra6m3g/`へコピー

    **NOTE:** この作業は、e2studio ではなく、Explorer などを使用します。

+ `dummy_app`の以下のフォルダーをコピー

    `src/`  
    `script/`

+ 選択したフォルダーを`./IDE/Renesas/e2studio/RA6M3/common/ra6m3g/`へコピー
`(暗号テスト、ベンチマーク、クライアント・サーバーの各プロジェクトはこのフォルダーを参照)`
+ `dummy_app`プロジェクトは削除しても構いません

7.) ネットワーク環境について

        クライアント・サーバーのプロジェクト内のwolfssl_thread_entry.hにネットワーク設定があります。
        それらの設定（ucIPAddress ... ucDNSServerAddress)は、ご使用のネットワーク環境に合わせて変更して
        してください。g_ether0_mac_address は、`wolfSSL_RA6M3G`プロジェクト内の RA configuration
        で定義されているデフォルトのMACアドレスです。クライアントのwolfssl_thread_entry.h は
        ターゲットのサーバーのSERVER_IP と DEFAULT_PORTの定義を持ちます。それらはご使用のサーバーの
        の設定に応じて変更してください。

## ビルドと実行

### 各プロジェクトをビルド
各プロジェクトで右クリックし、ビルドを選択

### 暗号テストとベンチマークを実行

1.) プロジェクト名を選択し、右クリック\
2.) `デバック` → `Renesas GDB Hardware Debugging`\
3.) `J-Link ARM`を選択し、OK をクリック\
4.) `R7FA6M3AH`を選択し、OK をクリック

### wolfSSL TLS サンプルサーバーを実行

1.) プロジェクト名を選択し、右クリック\
2.) `デバック` → `Renesas GDB Hardware Debugging`\
3.) `J-Link ARM`を選択し、OK をクリック\
4.) `R7FA6M3AH`を選択し、OK をクリック\
5.)以下のサンプルのクライアントプログラムを実行

```
./examples/client/client -v 4 -h "ucIPAddress" -p 11111 -A ./certs/1024/ca-cert.pem
```

**NOTE:** "ucIPAddress" はデフォルトでは "192.168.1.241"  (参照： wolfssl_thread_entry.h)

### wolfSSL TLS サンプルクライアントを実行
1.)以下のサンプルのサーバープログラムを実行

```
./examples/server/server -b -d -p 11111 -c ./certs/1024/server-cert.pem -k ./certs/1024/server-key.pem
```

TLS 1.3 で接続する際には、引数に "-v 4" を追加します。
```
./examples/server/server -v 4 -b -d -p 11111 -c ./certs/1024/server-cert.pem -k ./certs/1024/server-key.pem
```
**NOTE:** wolfssl_thread_entry.h中にデフォルトのポート番号 11111 定義(DEFAULT_PORT)
もし、DEFAULT_PORTを変更している場合、、上記のコマンドの "-p" の値は対応するポート番号に要変更

2.) プロジェクト名を選択し、右クリック\
3.) `デバック` → `Renesas GDB Hardware Debugging`\
4.) `J-Link ARM`を選択し、OK をクリック\
5.) `R7FA6M3AH`を選択し、OK をクリック

## トラブルシューティング

+ サンプルのクライアント・サーバープログラムは、wolfSSL のルートディレクトリから実行する必要があります。
+ user_settings.h の #define DEBUG_WOLFSSL を有効にすることで、デバックメッセージを\
  `Renesas Virtual Debug Console`へ出力します。
+ プロジェクトのビルドでリンクエラーが出た場合、リビルドしリフレッシュすることで解決することがあります。

[Support Forum](https://www.wolfssl.com/forums/)

Support Email: support@wolfssl.com

## 参考リンク

[wolfSSL Website](https://www.wolfssl.com/)

[wolfSSL Wiki](https://github.com/wolfSSL/wolfssl/wiki)

[wolfSSL Manual](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-toc.html)

[wolfSSL API Reference](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-17-wolfssl-api-reference.html)

[wolfCrypt API Reference](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-18-wolfcrypt-api-reference.html)

[TLS 1.3](https://www.wolfssl.com/docs/tls13/)
