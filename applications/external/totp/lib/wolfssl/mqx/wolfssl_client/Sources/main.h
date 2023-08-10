/* main.h */

#ifndef __main_h_
#define __main_h_

#include <mqx.h>
#include <bsp.h>
#include <mfs.h>
#include <fio.h>
#include <rtcs.h>
#include <ipcfg.h>
#include <sdcard.h>
#include <spi.h>
#include <part_mgr.h>

#include <wolfssl/ssl.h>

#define MAIN_TASK 	1

extern void Main_task(uint32_t);
extern void setup_ethernet(void);
extern void setup_clock(void);
extern void client_test(void);

/* cert file locations */
static const char* caCert = "a:\\certs\\ca-cert.pem";
static const char* clientCert = "a:\\certs\\client-cert.pem";
static const char* clientKey = "a:\\certs\\client-key.pem";

static inline void err_sys(const char* msg)
{
    printf("wolfssl error: %s\n", msg);
    if (msg)
        _mqx_exit(1);
}

/* PPP device must be set manually and 
 * must be different from the default IO channel (BSP_DEFAULT_IO_CHANNEL)
 */
#define PPP_DEVICE      "ittyb:" 

/*
 * Define PPP_DEVICE_DUN only when using PPP to communicate
 * to Win9x Dial-Up Networking over a null-modem
 * This is ignored if PPP_DEVICE is not #define'd
 */
#define PPP_DEVICE_DUN  1

#ifndef ENET_IPADDR
    #define ENET_IPADDR   IPADDR(192,168,1,155)
#endif

#ifndef ENET_IPMASK
    #define ENET_IPMASK   IPADDR(255,255,255,0)
#endif

#define GATE_IPADDR		  IPADDR(192,168,1,1)

#endif /* __main_h_ */

