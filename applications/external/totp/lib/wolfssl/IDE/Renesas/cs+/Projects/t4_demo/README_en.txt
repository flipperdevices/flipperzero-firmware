wolfSSL/AlphaProject Boad demo setup Guide

This demo is tested with the following condition.

  Renesas : CS+ v6.01, v8.01
  Board   : AP-RX71M-0A
  wolfSSL : 3.15.3, 4.0.0

Setup process:
1. Download software
  - Unzip AlphaProject firmware
  - Unzip wolfssl under the same directory
 
2. Set up wolfSSL
  - open wolfssl\IDE\Renesas\cs+\Projec/wolfssl\lib.mtpj with CS+ and build
  - open t4_demo.mtpj and build. This create demo program library.

3. Set up AlphaProject

  !!** When you use version 2.0 of a sample program, **!!
  !!** please replace _ether_ to _usbfunc_           **!!

  - The demo uses ap_rx71m_0a_sample_cs\Sample\ap_rx71m_0a_ether_sample_cs\
    ap_rx71m_0a_ether_sample_cs.mtpj
  - Open and edit ap_rx71m_0a_sample_cs\Sample\ap_rx71m_0a_ether_sample_cs\src\AP_RX71M_0A.c
    insert  wolfSSL_init() in echo_srv_init().

===
    sci_init();
    can_init();
    echo_srv_init();
    wolfSSL_init(); <- insert this line
===
!!** when you use version 2.0 **!!
===
    CanInit();
    SciInit();
    EthernetAppInit();
    UsbfInit();
    wolfSSL_init(); <- insert this line
===
!!******************************!!

  - Modify stack and heap size in ap_rx71m_0a_sample_cs\Sample\ap_rx71m_0a_ether_sample_cs\src\r_config
  Line 120#pragma stacksize su=0x2000
  Line 139 #define BSP_CFG_HEAP_BYTES  (0xa000)

!!** when you use version 2.0 **!!
  - Modify stack and heap size in ap_rx71m_0a_sample_cs\Sample\ap_rx71m_0a_usbfunc_sample_cs\src\smc_gen\r_bsp_config.h
  Line 154#pragma stacksize su=0x2000
  Line 175#define BSP_CFG_HEAP_BYTES  (0xa000)
!!******************************!!

  - Modify IP address Sample\ap_rx71m_0a_ether_sample_cs\src\r_t4_rx\src\config_tcpudp.c
  as needed
!!** when you use version 2.0 **!!
  - Modify IP address Sample\ap_rx71m_0a_usbfunc_sample_cs\src\tcp_sample\src\config_tcpudp.c
  as needed
!!******************************!!

===
#define MY_IP_ADDR0     192,168,1,200           /* Local IP address  */
#define GATEWAY_ADDR0   192,168,1,254           /* Gateway address (invalid if all 0s) */
#define SUBNET_MASK0    255,255,255,0
===

    - Add project properties of linking library in ap_rx71m_0a_ether_sample_cs.mtpj
    
wolfssl\IDE\Renesas\cs+\Projects\wolfssl_lib\DefaultBuild\wolfssl_lib.lib
wolfssl\IDE\Renesas\cs+\Projects\t4_demo\DefaultBuild\t4_demo.lib

   - Set CC-RX(Build Tool)->Library Geberation->Library Configuration to"C99" and enable ctype.h.

   - Build the project and start execute. You see message on the console prompting command.
   
===
wolfSSL Demo
t: test, b: benchmark, s: server, or c <IP addr> <Port>: client
$
===

Command can be executed only once. You need to reset and restart MPU for following command.

Command instruction:
t: Crypt algorithm test
b: benchmark
s: simple server acceptint at port 50000
c: simple client. Specify IP address and port as following.
   c <IP Addr> <Port Num>
   

4. Testing TLS Connection
   You can use wolfssl/examples/server and client on your PC for TLS peer test.
   
   
PC side
$ ./examples/server/server -b -d

Board side
> c <IP Addr> 11111

Board side
> s

PC side
$ ./examples/client/client -h <IP Addr> -p 50000

---
If you have any question, please contact support@wolfssl.com.

wolfSSL Inc.

