# ioLibrary Driver
The ioLibrary means “Internet Offload Library” for WIZnet chip. It includes drivers and application protocols.
The driver (ioLibrary) can be used for the application design of WIZnet TCP/IP chips as [W5500](http://wizwiki.net/wiki/doku.php?id=products:w5500:start), W5300, W5200, W5100 [W5100S](http://wizwiki.net/wiki/doku.php?id=products:w5100s:start).

## ioLibrary
This driver provides the Berkeley Socket type APIs.
- Directory Structure
<!-- ioLibrary pic -->
![ioLibrary](http://wizwiki.net/wiki/lib/exe/fetch.php?media=products:w5500:iolibrary_bsd.jpg "ioLibrary")

- Ethernet : SOCKET APIs like BSD & WIZCHIP([W5500](http://wizwiki.net/wiki/doku.php?id=products:w5500:start) / W5300 /  W5200 / W5100 / [W5100S](http://wizwiki.net/wiki/doku.php?id=products:w5100s:start)) Driver
- Internet :
  - DHCP client
  - DNS client
  - FTP client
  - FTP server
  - SNMP agent/trap
  - SNTP client
  - TFTP client
  - HTTP server
  - MQTT Client
  - Others will be added.

## How to add an ioLibrary in project through github site.
  - Example, refer to https://www.youtube.com/watch?v=mt815RBGdsA
  - [ioLibrary Doxygen doument](https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/Socket_APIs_V3.0.3.chm) : Refer to **TODO** in this document
    - Define what chip is used in **wizchip_conf.h**
    - Define what Host I/F mode is used in **wizchip_conf.h**

## Revision History
  * ioLibrary V4.0.0 Released : 29, MAR, 2018
    * New features added: Library for W5100S added.
  * ioLibrary V3.1.1 Released : 14, Dec, 2016
    * Bug fixed : In Socket.c Fixed MACraw & IPraw sendto function.
  * ioLibrary V3.1.0 Released : 05, Dec, 2016
    * Internet application protocol add to MQTT Client (using paho MQTT 3.11)
  * ioLibrary V3.0.3 Released : 03, May, 2016
    * In W5300, Fixed some compile errors in close(). Refer to M20160503
    * In close(), replace socket() with some command sequences.
  * ioLibrary V3.0.2 Released : 26, April, 2016
    * Applied the erratum #1 in close() of socket.c (Refer to A20160426)
  * ioLibrary V3.0.1 Released : 15, July, 2015
    * Bug fixed : In W5100, Fixed CS control problem in read/write buffer with SPI. Refer to M20150715.
  * ioLibrary V3.0 Released : 01, June, 2015
    * Add to W5300
    * Typing Error in comments
    * Refer to 20150601 in sources.

  * Type casting error Fixed : 09, April. 2015
    In socket.c, send() : Refer to M20150409

  * ioLibrary V2.0 released : April. 2015
    * Added to W5100, W5200
    * Correct to some typing error
    * Fixed the warning of type casting.

  * Last release : Nov. 2014

