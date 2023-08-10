評価ボードGR-ROSE 用 wolfSSLサンプルプロジェクト
======

<br>

## 1. 概要
-----

Renesas社製MCU RX65Nを搭載した評価ボードGR-ROSEをターゲットとしてwolfSSLを評価するためのサンプルプログラムを提供します。サンプルプログラムはリアルタイムOSを使わないベアメタル環境で動作し、IDEとしてe2 studioを使用するものです。本ドキュメントではサンプルプログラムのビルドから実行までの手順を説明します。

提供するサンプルプログラムは次の3機能を評価できる単一のアプリケーションです：

- CryptoTest: 各種暗号化オペレーション機能を自動テストするプログラム
- Benchmark: 各種暗号化オペレーションの実行速度を測定するプログラム
- TlsClient: 対向するTLSサーバーアプリケーションとTLS通信を行うプログラム

評価ボードを動作させる為のH/W設定、S/W設定は既に用意していますから、サンプルプロジェクトの動作までは最少の設定で済むようになっています。また、TLS通信において必要となる、RootCA証明書、サーバー側証明書もサンプル使用に限定したものが既に設定済みです。

以降のセクションはサンプルアプリケーションの実行までのステップを順に説明していきます。
<br><br>


## 2. ターゲットとしているH/W、コンポーネント、ライブラリ
-----

このサンプルプログラムでは以下のハードウエアやソソフトウエアライブラリを使用しています。皆さんがご利用時にバージョンの新しいソフトウエアコンポーネントが利用可能であれば適宜更新の上ご利用ください。
<br><br>

|要素|名称/バージョン|
|:--|:--|
|評価ボード|GR-ROSE|
|Device|R5F565NEHxFP|
|IDE| Renesas e2Studio Version:2022-01 |
|エミュレーター| E1, E2エミュレーターLite |
|Toolchain|CCRX v3.04.00|
|TSIP| TSIP v1.17|
<br>

本サンプルプログラムのプロジェクトには以下のFITコンポーネントを使用する設定ファイルが用意されています。
しかし、FITコンポーネント群そのものは本サンプルアプリケーションの配布物としては含まれていません。皆さん自身でIDE内の操作によってダウンロードしていただく必要があります。一部のFITコンポーネント（TSIP）はIDE内から直接ダウンロードできず、Renesas社のダウンロードサイトから別途ダウンロードが必要なものがあります。
<br><br>



|FITコンポーネント|バージョン|略称|
|:--|:--|:--|
|Board Support Packages|6.21|r_bsp|
|CMT driver|4.90|r_cmt_rx|
|Ethernet Driver|1.21|r_ether_rx|1.21|r_ether_rx|
|Generic system timer for RX MCUs|1.01|r_sys_time_rx|
|TCP/IP protocol stack[M3S-T4-Tiny] - RX Ethernet Driver Interface|1.09|r_t4_driver_rx|
|TCP/IP protocol stack[M3S-T4-Tiny] for Renesas MCUs|2.10|r_t4_rx|
|TSIP(Trusted Secure IP) driver|1.17.l|r_tsip_rx|

（注意）2022年4月現在、TIPSv1.15はFITコンポーネントとしてスマートコンフィギュレータパースペクティブのコンポーネントの追加操作では追加できないようです。後ほど説明する手動での追加方法を使って追加してください。<br>


<br><br>

## 3. サンプルプログラムプロジェクトのe2Studioへのインポート
----

サンプルプログラムは新規作成する必要はありません。すでにプロジェクトファイルが用意されていますからIDEから以下の手順でプロジェクトをインポートしてください。


+ 1.e2studioの"ファイル"メニュー > "ファイル・システムからプロジェクトを開く..." > "ディレクトリ(R)..."インポート元ボタンを押して、プロジェクトのインポート元となるフォルダーを選択します。本READMEファイルが存在するフォルダ(Renesas/e2studio/{MCU}/{board-name-folder})を選択してください。

+ インポートできるプロジェクトが４つリストアップされますが、"smc", "test" と"wolfssl"の3プロジェクトにのみチェックを入れて”終了”ボタンを押してください。

プロジェクト・エクスプローラーにインポートしたsmc,test,wolfsslの3プロジェクトが表示されるはずです。
<br><br>

## 4. FITモジュールの取得とスマートコンフィギュレータによるファイル生成
----

FITモジュールは皆さん自身で取得していただく必要があります。以下の手順で取得してください。
<br><br>


1. プロジェクト・エクスプローラー上でsmcプロジェクトを開き**smc.scfg**ファイルをダブルクリックして、スマートコンフィギュレータパースペクティブを開きます。

2. ソフトウエアコンポーネント設定画面の"コンポーネント"タブを選択 > 画面右上の"コンポーネントの追加" ボタンを押して下さい。ソフトウエアコンポーネントの選択ダイアログが表示されます。ここで、画面下部の"最新版のFITドライバとミドルウエアをダウンロードする"を押して、モジュール群を取得しておいてください。ダウンロード先のフォルダは"基本設定..."を押して確認することができます。

3. TSIPコンポーネントは上記ステップ２の方法ではRenesas社の対応が遅れていて最新版が自動的には取得できないことがあります。その場合にはRenesas社のサイトから手動でダウンロードできます。ダウンロードしたコンポーネントのを解凍し、FITModulesフォルダに含まれているファイル群をステップ2のダウンロード先フォルダに格納してください。

4.  先に示したFITコンポーネントを一覧から選択して"終了"ボタンを押します。この操作を必要なFITコンポーネントを得るまで繰り返し行います。

5.  ソフトウエアコンポーネント設定画面のコンポーネントタブを選択し、r_t4_rxコンポーネントを選択します。右の設定ペインに"# IP address for ch0, when DHCP disable."プロパティの値として、このボードのIPアドレスを皆さんの環境に合わせてて指定します（例:
192.168.1.9)。

6. ソフトウエアコンポーネント設定画面の右上の”コードの生成"ボタンを押して、スマートコンフィギュレータにソースファイルを生成させます。smcプロジェクトの下にsrc/smc_genフォルダが生成され、ソースファイル、ライブラリ等が格納されされます。
<br><br>

## 5. testプロジェクトへのsmc_genファイル群の追加
----

testプロジェクトはサンプルアプリケーションのプロジェクトです。プロジェクト・エクスプローラー上でtestプロジェクトを開くとsrcフォルダが存在し、そこにサンプルアプリケーションのソースファイルが格納されています。このsrcフォルダ直下に4-5で生成したsmc_genフォルダごとコピー&ペーストしてソースファイルを追加します。

test/src/smc_gen

のようなフォルダ階層になることを確認してください。
<br><br>

## 6. testアプリケーションの動作を選択する
----
testプロジェクトが唯一のサンプルアプリケーションですが、その動作を設定で切り替えることによって、

- CryptoTest: 各種暗号化オペレーション機能を自動テストするプログラム
- Benchmark: 各種暗号化オペレーションの実行速度を測定するプログラム
- TlsClient: 対向するTLSサーバーアプリケーションとTLS通信を行うプログラム

のいずれかの動作を行うよう指定できます。

設定はtest/src/wolfssl_demo.h ファイル内の、

- #define CRYPT_TEST
- #define BENCHMARK
- #define TLS_CLIENT

のいずれか一つを有効にしてアプリケーションをビルドすることで行います。
<br><br>


## 7. testアプリケーションのビルドと実行
-----

testアプリケーションのビルドの準備が整ったので、ビルドを行います。

1．プロジェクト・エクスプローラー上のwolfsslプロジェクトをビルドし、次にtestプロジェクトをビルドします。

2．ビルドが成功したら、ターゲットボードとエミュレーターを接続し、外部電源を入力します。

3．"実行"メニュー > "デバッグ"を選択し、デバッグパースペクティブを開きます。

4． testアプリケーションは標準出力に動作状況を出力します。この標準出力の表示用に、"Renesas Debug Virtual Console"を開いておきます。

5．実行ボタンを押してtestアプリケーションを動作させます。

6．CryptoTest、BenchmarkあるいはTLS_Client選択していた動作に応じた実行結果を表示したあと無限ループに入るので出力が停止したと思われたらデバッグを停止させます。
<br><br>

## 8. TLS_Clientとしてのtestアプリケーションの実行
-----
<br>

### 8.1 testアプリケーションのサポートするTLSバージョン
<br>
TLS1.2に加えてTLS1.3プロトコルが使用できます。{board-name-folder}/common/user_settings.hに既定で以下のマクロ定義が設定されています。
<br><br>

```
#define WOLFSSL_TLS13
```
この定義によりTLS1.3プロトコルを使用する設定となります。TLS1.2を使用する場合には、このマクロ定義をコメントアウトしてして、testアプリケーションを再ビルドしてください。

<br>

### 8.2 使用する証明書のタイプ

<br>

testアプリケーションとサーバーアプリケーションではTLS通信時に使用する証明書（RootCA証明書、サーバー証明書、クライアント証明書）のタイプが選択できます。証明書に含まれている公開鍵がRSA公開鍵であるRSA証明書とECC公開鍵を含むECDSA証明書が選択できます。既定ではECDSA証明書を使用するようになっています。

<br>
{board-name-folder}/common/user_settings.h に既定で以下のマクロ定義が設定されています。RSA証明書を使用する場合には上記定義をコメントアウトして、testアプリケーションを再ビルドしてください。
<br><br>

```
#define USE_ECC_CERT
```

<br>
この定義により、testアプリケーションはサーバーの提示するECC公開鍵を含んだサーバー証明書を検証できるRootCA証明書を使うようになります。また、クライアント認証に用いるクライアント証明書もECDSA証明書が使われます。
<br><br>


### 8.3 暗号化スイートの選択

<br>
testアプリケーションでは、TLSバージョンと証明書のタイプにより、testアプリケーションで使用する暗号化スイートを決定します。下表に使用できる暗号化スイートを示します。

<br>


|バージョン|証明書|暗号化スイート|
|:--|:--|:--|
|TLS1.3|RSA/ECDSA証明書|  |
|||TLS_AES_128_GCM_SHA256|
|||TLS_AES_128_CCM_SHA256| 
|TLS1.2|RSA証明書|
|||TLS_RSA_WITH_AES_128_CBC_SHA|
|||TLS_RSA_WITH_AES_256_CBC_SHA|
|||TLS_RSA_WITH_AES_128_CBC_SHA256|
|||TLS_RSA_WITH_AES_256_CBC_SHA256|
|||TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256|
|||TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256|
||ECDSA証明書||
|||TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256|
|||TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256|


<br>

### 8.4 TLS対向アプリケーションのビルドと実行

<br>

testアプリケーションをTLS_Clientとしての動作を行わせる場合には、TLS通信の相手方となる対向アプリケーションが必要となります。wolfSSLパッケージにはこの用途に使用できるTLSserverサンプルアプリケーションが用意されています。このプログラムはwolfsslをビルドすることで生成されます。wolfSSLのビルドにはgccがインストールされているLinux(MacOS, WSLも含む)でのビルドとVisualStudioを使ってのビルドが可能です。以下ではWSL上でのビルドを紹介します。
<br><br>

コンフィギュレーションオプションは使用する証明書タイプによって変更する必要があります。

<br>

#### 8.4.1 ECDSA証明書を使用する場合のコンフィギュレーション

<br>

```
$ cd {wolfssl-folder}
$ ./autogen.sh
$ ./configure --enable-ecc --enable-dsa --enable-aesccm CFLAGS="-DWOLFSSL_STATIC_RSA -DHAVE_AES_CBC -DHAVE_DSA -DHAVE_ALL_CURVES -DHAVE_ECC -DNO_RSA"
$ make
```
(注意）"-DNO_RSA"の指定をわすれないこと

<br>

#### 8.4.2 RSA証明書を使用する場合のコンフィギュレーション

<br>

```
$ cd {wolfssl-folder}
$ ./autogen.sh
$ ./configure --enable-ecc --enable-dsa --enable-aesccm CFLAGS="-DWOLFSSL_STATIC_RSA -DHAVE_AES_CBC -DHAVE_DSA -DHAVE_ALL_CURVES -DHAVE_ECC"
$ make
```

<br>

#### 8.4.3 サーバーアプリケーションの実行
<br>
上記のビルドによって、{wolfssl-folder}/examples/server/server
が生成されます。この実行プログラムがサーバーアプリケーションとして機能します。以下のオプションを与えて実行させるとtestアプリケーションからの接続を待ち受け状態になります。　使用するTLSバージョンとして TLS1.3を使用する場合には "-v4"を指定し、TLS1.2を使用する場合には"-v3"を指定してください。
<br><br>

```
$ examples/server/server -b -v4 -i
```
<br>
testアプリケーションには、サーバーアプリケーションに割り当てられたIPアドレスを指定します。

wolf_client.cを開き、#define SIMPLE_TLSSEVER_IPにアドレスを指定します。

```
-- wolf_client.c --

...
#define SIMPLE_TLSSEVER_IP  192.168.53.9
...
```
<br>
ファイルをセーブしてtestアプリケーションを再ビルドしてください。testアプリケーションを実行すると、対向アプリケーションとの間でTLS接続が行われ、その後簡単な文字列を交換して標準出力に以下のような表示を行います。表示される暗号化スイートはTLSバージョンと証明書タイプの組み合わせによって異なります。

<br><br>

```
cipher : ECDHE-RSA-AES128-GCM-SHA256
Received: I hear you fa shizzle!

cipher : ECDHE-RSA-AES128-SHA256
Received: I hear you fa shizzle!

cipher : AES128-SHA256
Received: I hear you fa shizzle!

cipher : AES256-SHA256
Received: I hear you fa shizzle!

cipher : ECDHE-RSA-AES128-SHA256
Received: I hear you fa shizzle!

cipher : ECDHE-RSA-AES128-GCM-SHA256
Received: I hear you fa shizzle!
```
<br>


## 9. ユーザーが用意したRootCA証明書を利用する際に必要なこと
-----

本サンプルプログラムでは、TLS_Clientとして動作する際に必要なRootCA証明書とサンプルTLSサーバーアプリケーションが使用するサーバー証明書などは評価用でのみ利用可能な証明書です。
機能評価を超えた目的で利用する場合にはRootCA証明書をユーザー自身で用意する必要があります。それに伴い、
 1. Provisioning key
 2. RootCA証明書の検証の為に必要なRSA鍵ペア
 3. RootCA証明書を上記２の秘密鍵で生成した署名

が必要になります。それらの生成方法はRenesas社提供のマニュアルを参照してください。

<br>

## 10. クライアント認証を行うための必要事項
-----

クライアント認証機能は以下のようにサポートしています。
-	ECDSA証明書あるいはRSA証明書はTSIPを使って処理します。


(1) クライアント証明書のロード
wolfSSL_CTX_use_certificate_buffer あるいはwolfSSL_CTX_use_certificate_chain_buffer_format を使ってクライアント証明書をロードしてください。

(2) クライアント秘密鍵/公開鍵のロード

クライアント証明書の種類に応じてロードすべき鍵が決まります。以下に従って必要な鍵をロードしてください。

a) ECDSA証明書の場合：
- tsip_use_PrivateKey_bufferを使って秘密鍵をロードしてください。

b) RSA証明書の場合：
- tsip_use_PrivateKey_bufferを使って秘密鍵をロードしてください。
- tsip_use_PublicKey_bufferを使って公開鍵をロードしてください。

RSA証明書の場合には署名処理を内部で検証する目的で公開鍵も使用します。その為に、公開鍵のロードが必要です。

(3) encrypted keyの作成

鍵ロードすべき秘密鍵あるいは公開鍵はRensas　Secure Flash ProgrammerあるいはSecurityKeyManagementToolを使って出力されたencrypted keyを渡してください。encrypted keyの作成方法はアプリケーションノート“RXファミリ TSIPモジュールFirmware Integration Technology” の7.1.4”encrypted key, encrypted provisioning keyの使用方法“に説明されています。

(4) 必要なマクロ定義

user_settings.hにWOLF_PRIVATE_KEY_IDの定義を行ってください。


<br>

## 11. 制限事項
-----
TSIPv1.17をサポートしたwolfSSLでは以下の機能制限があります。

1. TLSハンドシェーク中にサーバーと交換したメッセージパケットが平文でメモリ上に蓄積されています。これはハンドシェークメッセージのハッシュ計算に使用されます。内容はセッション終了時に削除されます。

2. TSIPを使ってのセッション再開およびearly dataはサポートされません。

上記制限事項は次版以降のTSIPあるいはwolfSSLによって改善が見込まれています。
