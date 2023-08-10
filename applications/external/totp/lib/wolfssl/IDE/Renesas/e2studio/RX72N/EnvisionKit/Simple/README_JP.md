評価ボードRX72N EnvisionKit 用 wolfSSL シンプルプロジェクト
======



## 1. 概要
-----

Renesas社製MCU RX72Nを搭載した評価ボードRX72N EnvisionKit をターゲットとしてwolfSSLを評価するためのサンプルプログラムを提供します。サンプルプログラムはリアルタイムOSを使わないベアメタル環境で動作し、
IDEとしてe2 studioを使用するものです。本ドキュメントではサンプルプログラムのビルドから実行までの手順を説明します。

提供するサンプルプログラムは次の3機能を評価できる単一のアプリケーションです：

- CryptoTest: 各種暗号化オペレーション機能を自動テストするプログラム
- Benchmark: 各種暗号化オペレーションの実行速度を測定するプログラム
- TlsClient: 対向するTLSサーバーアプリケーションとTLS通信を行うプログラム
- TlsServer: 対向するTLSクライアントアプリケーションとTLS通信を行うプログラム


評価ボードを動作させる為のH/W設定、S/W設定は既に用意していますから、サンプルプロジェクトの動作までは最少の設定で済むようになっています。
また、TLS通信において必要となる、RootCA証明書、サーバー側証明書もサンプル使用に限定したものが既に設定済みです。
以降のセクションはサンプルアプリケーションの実行までのステップを順に説明していきます。


## 2. ターゲットとしているH/W、コンポーネント、ライブラリ
-----

このサンプルプログラムでは以下のハードウエアやソソフトウエアライブラリを使用しています。皆さんがご利用時にバージョンの新しいソフトウエアコンポーネントが利用可能であれば適宜更新の上ご利用ください。


|要素|名称/バージョン|
|:--|:--|
|評価ボード|RX72N EnvisionKit|
|Device|R5F572NNHxFB|
|IDE| Renesas e2Studio Version:2022-07 |
|エミュレーター| E2エミュレーターLite |
|Toolchain|CCRX v3.03.00|
|TSIP| TSIP v1.15|


本サンプルプログラムのプロジェクトには以下のFITコンポーネントを使用する設定ファイルが用意されています。
しかし、FITコンポーネント群そのものは本サンプルアプリケーションの配布物としては含まれていません。
皆さん自身でIDE内の操作によってダウンロードしていただく必要があります。一部のFITコンポーネント（TSIP）はIDE内から直接ダウンロードできず、Renesas社のダウンロードサイトから別途ダウンロードが必要なものがあります。


|FITコンポーネント|バージョン|略称|
|:--|:--|:--|
|Board Support Packages|7.20|r_bsp|
|CMT driver|5.20|r_cmt_rx|
|Ethernet Driver|1.22|r_ether_rx|
|Generic system timer for RX MCUs|1.01|r_sys_time_rx|
|TCP/IP protocol stack[M3S-T4-Tiny] - RX Ethernet Driver Interface|1.09|r_t4_driver_rx|
|TCP/IP protocol stack[M3S-T4-Tiny] for Renesas MCUs|2.10|r_t4_rx|
|TSIP(Trusted Secure IP) driver|1.15.l|r_tsip_rx|


## 3. サンプルプログラムプロジェクトのe2Studioへのインポート
----

サンプルプログラムは新規作成する必要はありません。すでにプロジェクトファイルが用意されていますからIDEから以下の手順でプロジェクトをインポートしてください。


+ 1.e2studioの"ファイル"メニュー > "ファイル・システムからプロジェクトを開く..." > "ディレクトリ(R)..."インポート元ボタンを押して、プロジェクトのインポート元となるフォルダーを選択します。本READMEファイルが存在するフォルダ(Renesas/e2studio/{MCU}/{board-name-folder}/Simple)を選択してください。

+ インポートできるプロジェクトが４つリストアップされますが、"test" と"wolfssl"の2プロジェクトにのみチェックを入れて”終了”ボタンを押してください。

プロジェクト・エクスプローラーにインポートしたtest,wolfsslの2プロジェクトが表示されるはずです。


## 4. FITモジュールの取得とスマートコンフィギュレータによるファイル生成
----

FITモジュールは皆さん自身で取得していただく必要があります。以下の手順で取得してください。

1. プロジェクト・エクスプローラー上でsmcプロジェクトを開き**test.scfg**ファイルをダブルクリックして、スマートコンフィギュレータパースペクティブを開きます。

2. ソフトウエアコンポーネント設定画面の"コンポーネント"タブを選択 > 画面右上の"コンポーネントの追加" ボタンを押して下さい。ソフトウエアコンポーネントの選択ダイアログが表示されます。ここで、画面下部の"最新版のFITドライバとミドルウエアをダウンロードする"を押して、モジュール群を取得しておいてください。ダウンロード先のフォルダは"基本設定..."を押して確認することができます。

3. TSIPコンポーネントは上記ステップ２の方法ではRenesas社の対応が遅れていて最新版が自動的には取得できないことがあります。その場合にはRenesas社のサイトから手動でダウンロードできます。ダウンロードしたコンポーネントのを解凍し、FITModulesフォルダに含まれているファイル群をステップ2のダウンロード先フォルダに格納してください。

4. 先に示したFITコンポーネントを一覧から選択して"終了"ボタンを押します。この操作を必要なFITコンポーネントを得るまで繰り返し行います。

5. ソフトウエアコンポーネント設定画面のコンポーネントタブを選択し、r_t4_rxコンポーネントを選択します。右の設定ペインに"# IP address for ch0, when DHCP disable."プロパティの値として、このボードのIPアドレスを皆さんの環境に合わせてて指定します（例:
192.168.1.9)。

6. ソフトウエアコンポーネント設定画面の右上の”コードの生成"ボタンを押して、スマートコンフィギュレータにソースファイルを生成させます。testプロジェクトの下にsrc/smc_genフォルダが生成され、
ソースファイル、ライブラリ等が格納されされます。


## 5. testアプリケーションの動作を選択する
----
testプロジェクトが唯一のサンプルアプリケーションですが、その動作を設定で切り替えることによって、

- CryptoTest: 各種暗号化オペレーション機能を自動テストするプログラム
- Benchmark: 各種暗号化オペレーションの実行速度を測定するプログラム
- TlsClient: 対向するTLSサーバーアプリケーションとTLS通信を行うプログラム
- TlsServer: 対向するTLSクライアントアプリケーションとTLS通信を行うプログラム

のいずれかの動作を行うよう指定できます。

設定はtest/src/wolfssl_simple_demo.h ファイル内の、

- #define CRYPT_TEST
- #define BENCHMARK
- #define SIMPLE_TCP_CLIENT
- #define SIMPLE_TLS_CLIENT
- #define SIMPLE_TLS_TSIP_CLIENT
- #define SIMPLE_TCP_SERVER
- #define SIMPLE_TLS_SERVER

のいずれか一つを有効にしてアプリケーションをビルドすることで行います。


## 6. testアプリケーションのビルドと実行
-----

testアプリケーションのビルドの準備が整ったので、ビルドを行います。

1．プロジェクト・エクスプローラー上のwolfsslプロジェクトをビルドし、次にtestプロジェクトをビルドします。

2．ビルドが成功したら、ターゲットボードとエミュレーターを接続し、外部電源を入力します。

3．"実行"メニュー > "デバッグ"を選択し、デバッグパースペクティブを開きます。

4．testアプリケーションは標準出力に動作状況を出力します。この標準出力の表示用に、"Renesas Debug Virtual Console"を開いておきます。

5．実行ボタンを押してtestアプリケーションを動作させます。

6．CryptoTest、Benchmark、TLS_ClientまたはTLS_Serverのいずれか選択していた動作に応じた実行結果を表示したあと無限ループに入るので出力が停止したと思われたらデバッグを停止させます。


## 7. クライアントとしてのtestアプリケーションの実行
-----


### 7.1 TCP クライアントとして実行
`#define SIMPLE_TCP_CLIENT`を有効にすると、TCPクライアントとしてtestプロジェクトを実行することが出来ます。対向サーバーには、[このTCP サーバーアプリケーション](https://github.com/wolfSSL/wolfssl-examples/blob/master/tls/server-tcp.c)を利用することが出来ます。

コンパイルと実行は下記のようになります。

```
$ gcc server-tcp.c -o server-tcp
$ ./server-tcp
```

クライアントの対向サーバーのIPアドレスを変更します。IPアドレスは、`SIMPLE_TCPSERVER_IP`と`SIMPLE_TCPSERVER_PORT`という定義名で`test/src/simple_tcp.c`内に定義してあります。変更後、再度コンパイルし実行します。

次のようなメッセージがRenesas Virtual Console に表示されれば、ボード上のクライアントとサーバ間でTCP通信が成功しています。

```
Received : I hear ya fa shizzle!
```

TCPサーバー側のコンソールには、
```
$ ./server-tcp
Waiting for a connection...
Client connected successfully
Client: Hello Server

Waiting for a connection...
```

### 7.2 TLS クライアントとして実行
`#define SIMPLE_TLS_CLIENT`を有効にすると、TLSクライアントとしてtestプロジェクトを実行することが出来ます。クライアントの対向サーバーのIPアドレスを変更します。IPアドレスは、`SIMPLE_TLSSERVER_IP`と`SIMPLE_TLSSERVER_PORT`という定義名で`test/src/simple_tls_tsip.c`内に定義してあります。変更後、再度コンパイルし実行します。

対向サーバーには wolfSSL に同梱されるサンプルサーバーを利用します。サンプルサーバーのコンフィグレーションとビルドは下記のようになります。

```
$ cd <wolfssl-folder>
$ ./autogen.sh
$ ./configure
$ make
```

対向サーバーの実行にはECC証明書を指定します。なぜなら、test クライアントはCA証明書としてECC証明書を使用するようにプログラムされているからです。この設定は、`user_settings.h`中の下記マクロで指定しています。

```
#define USE_ECC_CERT
```

そのため、対向サーバーの実行時にはサーバーもECCサーバー証明書を下記のように指定します。

```
./examples/server/server -v 4 -b -i -c ./certs/server-ecc.pem -k ./certs/ecc-key.pem -A ./certs/client-ecc-cert.pem
```

次のようなメッセージがRenesas Virtual Console に表示されれば、ボード上のクライアントとサーバ間でTCP通信が成功しています。

```
cipher : TLS13-AES128-GCM-SHA256
Received: I hear you fa shizzle!
```

TLSサーバー側のコンソールには次のようなメッセージが出力されます。
```
SSL version is TLSv1.3
SSL cipher suite is TLS_AES_128_GCM_SHA256
SSL curve name is SECP256R1
Client message: Hello Server
```

注)対向サーバーのコマンド中 `-v 4` オプションは、TLS 1.3を使ったTLS通信を行うことを指定しています。もし、1.3よりも下位のバージョンでTLS通信を行いたい場合、`-v 4`をオプションから取り除きます。


### 7.2 TLS クライアントをTSIPドライバを利用するように設定し実行
#define SIMPLE_TLS_TSIP_CLIENT`を有効にすると、TLSクライアントがTSIPドライバを使用してTLS通信を行うようになります。TLS 1.2 で通信を行うために、`user_settings.h`の`#define WOLFSSL_TLS13`を無効化し、また、RSA証明書を使うよう`#define USE_ECC_CERT`を無効化します。その後、wolfSSL 及び test プロジェクトを再コンパイルします。

TSIPドライバがサポートする暗号スイートは下記の一覧になります。

|Tls version |Certificate type|Cipher suites|
|:--|:--|:--|
|TLS1.3|RSA/ECDSA certificate|  |
|||TLS_AES_128_GCM_SHA256|
|||TLS_AES_128_CCM_SHA256| 
|TLS1.2|RSA certificate|
|||TLS_RSA_WITH_AES_128_CBC_SHA|
|||TLS_RSA_WITH_AES_256_CBC_SHA|
|||TLS_RSA_WITH_AES_128_CBC_SHA256|
|||TLS_RSA_WITH_AES_256_CBC_SHA256|
|||TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256|
|||TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256|
||ECDSA certificate||
|||TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256|
|||TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256|

対向サーバーには、先ほどの wolfSSL に同梱されるサンプルサーバーを利用します。サンプルサーバーのコンフィグレーション、ビルド及び実行コマンドは下記のようになります。
```
$ cd <wolfssl-folder>
$ ./autogen.sh
$ ./configure CFLAGS="-DWOLFSSL_STATIC_RSA"
```

サンプルサーバーの実行には下記を指定します。

```
$ ./examples/server/server -b -i
```

実行すると、下記のようなメッセージがRenesas Debug Virtual Console に表示されます。

```
cipher : ECDHE-RSA-AES128-GCM-SHA256
Received: I hear you fa shizzle!

cipher : ECDHE-RSA-AES128-SHA256
Received: I hear you fa shizzle!

cipher : AES128-SHA
Received: I hear you fa shizzle!

cipher : AES128-SHA256
Received: I hear you fa shizzle!

cipher : AES256-SHA
Received: I hear you fa shizzle!

cipher : AES256-SHA256
Received: I hear you fa shizzle!

```

TLS1.3で通信を行いたい場合、`#define WOLFSSL_TLS13`を有効化し、wolfSSL及びtestプロジェクトを再コンパイルします。また、サンプルサーバーの実行時に、`-v 4`オプションを追加します。

### 8. サーバーとしてのtestアプリケーションの実行

### 8.1 TCP サーバーとして実行
`#define SIMPLE_TCP_SERVER`を有効にすると、TCPサーバとしてtestプロジェクトを実行することが出来ます。対向クライアントには、[このTCP クライアントアプリケーション](https://github.com/wolfSSL/wolfssl-examples/blob/master/tls/client-tcp.c)を利用することが出来ます。

コンパイルと実行は下記のようになります。

```
$ gcc client-tcp.c -o client-tcp
```


次のようなメッセージがRenesas Virtual Console 及びTCPクライアントのターミナルに表示されれば、ボード上のサーバーとサーバ間でTCP通信が成功しています。
```
[TCP Client terminal]
$ ./client-tcp 192.168.10.33
Message for server: hello
Server: hello

[Renesas Debug Virtual Console]
Received: hello
```

### 8.1 TLS サーバーとして実行
`#define SIMPLE_TLS_SERVER`を有効にすると、TLSサーバとしてtestプロジェクトを実行することが出来ます。対向クライアントには、wolfSSL に同梱されるサンプルクライアントを利用することが出来ます。コンパイルとビルドは、[8.2 Run Client as TLS (Software Only)](#8.2-Run-Client-as-TLS-(Software-Only))に準じます。

testプロジェクトを再コンパイル後、実行するとTLSサーバーはボード上でクライアントからの接続待ちになりまうす。ターミナルか下記のように実行します。

```
$ ./examples/client/client -v 4 -h 192.168.10.33 -p 11111 -A ./certs/ca-ecc-cert.pem
SSL version is TLSv1.3
SSL cipher suite is TLS_AES_128_GCM_SHA256
SSL curve name is SECP256R1
hello wolfssl!
```

Renesas Debug Virtual Console に下記のように表示されていれば、ボード上の TLS サーバーとクライアント間でTLS通信が行われています。
```
Received: hello wolfssl!
```


## 9. ユーザーが用意したRootCA証明書を利用する際に必要なこと
-----

本サンプルプログラムでは、TLS_Clientとして動作する際に必要なRootCA証明書とサンプルTLSサーバーアプリケーションが使用するサーバー証明書などは評価用でのみ利用可能な証明書です。
機能評価を超えた目的で利用する場合にはRootCA証明書をユーザー自身で用意する必要があります。それに伴い、
 1. Provisioning key
 2. RootCA証明書の検証の為に必要なRSA鍵ペア
 3. RootCA証明書を上記２の秘密鍵で生成した署名

が必要になります。それらの生成方法はRenesas社提供のマニュアルを参照してください。


## 10. 制限事項

TSIPv1.15をサポートしたwolfSSLでは以下の機能制限があります。

1. TLSハンドシェーク中にサーバーと交換したメッセージパケットが平文でメモリ上に蓄積されています。これはハンドシェークメッセージのハッシュ計算に使用されます。内容はセッション終了時に削除されます。

1. TLS1.2ではTSIPを使ったクライアント認証機能をサポートしていません。
wolfSSL_CTX_use_certificate_buffer あるいはwolfSSL_CTX_use_certificate_chain_buffer_format を使ってクライアント証明書をロードし、wolfSSL_CTX_use_PrivateKey_bufferを使って秘密鍵をロードしてください。ソフトウエアで処理を行います。

1. TLS1.3ではTSIPを使ったクライアント認証機能はECDSAクライアント証明書の場合にのみサポートされます。RSA証明書の場合はソフトウエアでの処理となります。

1. TLS1.3ではTSIPを使ったサーバー認証機能のうち、CertificateVerifyメッセージの検証はソフトウエアでの処理となります。

1. TSIPを使ってのセッション再開およびearly dataはサポートされません。

上記制限1 ~ 4は次版以降のTSIPによって改善が見込まれています。

## 11. サポート
ご質問・ご要望は、info@wolfssl.jp まで日本語でお知らせください。