#include "eth_worker_i.h"
#include "eth_worker.h"

#include <furi_hal.h>
#include "socket.h"
#include "dhcp.h"
#include "ping.h"
#include "stm32wbxx_hal_gpio.h"

#define TAG "EthWorker"

EthWorker* eth_worker_alloc() {
    EthWorker* eth_worker = malloc(sizeof(EthWorker));
    // Worker thread attributes
    eth_worker->thread = furi_thread_alloc();
    furi_thread_set_name(eth_worker->thread, "EthWorker");
    furi_thread_set_stack_size(eth_worker->thread, 8192);
    furi_thread_set_callback(eth_worker->thread, eth_worker_task);
    furi_thread_set_context(eth_worker->thread, eth_worker);

    eth_worker_change_state(eth_worker, EthWorkerStateNotInited);
    return eth_worker;
}

void eth_worker_free(EthWorker* eth_worker) {
    furi_assert(eth_worker);
    furi_thread_free(eth_worker->thread);
    free(eth_worker);
}

/************************** Ethernet Worker Thread *****************************/

int32_t eth_worker_task(void* context) {
    furi_assert(context);
    EthWorker* eth_worker = (EthWorker*)context;
    furi_hal_power_insomnia_enter();

    while(eth_worker->state != EthWorkerStateStop) {
        if(eth_worker->state == EthWorkerStateModuleInit) {
            eth_worker_w5500(eth_worker);
        }
        if(eth_worker->state == EthWorkerStateDHCP) {
            eth_worker_dhcp(eth_worker);
        }
    }

    FURI_LOG_I(TAG, "eth_worker_task exit \r\n");
    eth_worker_change_state(eth_worker, EthWorkerStateStop);
    furi_hal_power_insomnia_exit();

    return 0;
}

#define DHCP_SOCKET 0
#define PING_SOCKET 1

// MAC-àäðåñ
#define ETHADDR0 0x00 // The first octet of the Ethernet address
#define ETHADDR1 0x08 // The second octet of the Ethernet address
#define ETHADDR2 0xDC // The third octet of the Ethernet address
#define ETHADDR3 0x47 // The fourth octet of the Ethernet address
#define ETHADDR4 0x47 // The fifth octet of the Ethernet address
#define ETHADDR5 0x54 // The sixth octet of the Ethernet address
// Ïàðàìåòðû IP (åñëè DHCP îòêëþ÷åí)
#define IPADDR0 192 // The first octet of the IP address of this uIP node
#define IPADDR1 168 // The second octet of the IP address of this uIP node
#define IPADDR2 1 // The third octet of the IP address of this uIP node
#define IPADDR3 137 // The fourth octet of the IP address of this uIP node
#define NETMASK0 255 // The first octet of the netmask of this uIP node
#define NETMASK1 255 // The second octet of the netmask of this uIP node
#define NETMASK2 255 // The third octet of the netmask of this uIP node
#define NETMASK3 0 // The fourth octet of the netmask of this uIP node
#define DRIPADDR0 192 // The first octet of the IP address of the default router
#define DRIPADDR1 168 // The second octet of the IP address of the default router
#define DRIPADDR2 1 // The third octet of the IP address of the default router
#define DRIPADDR3 1 // The fourth octet of the IP address of the default router

static uint8_t ip_assigned = 0;
static GpioPin cspin = {.port = GPIOA, .pin = GPIO_PIN_4};

void W5500_Select(void) {
    furi_hal_gpio_write(&cspin, false);
}

void W5500_Unselect(void) {
    furi_hal_gpio_write(&cspin, true);
}

void Callback_IPAssigned(void) {
    FURI_LOG_I(TAG, "Callback: IP assigned! Leased time: %d sec\r\n", getDHCPLeasetime());
    ip_assigned = 1;
}

void Callback_IPConflict(void) {
    FURI_LOG_I(TAG, "Callback: IP conflict!\r\n");
}

void W5500_ReadBuff(uint8_t* buff, uint16_t len) {
    furi_hal_spi_bus_rx(&furi_hal_spi_bus_handle_external, buff, len, 1000);

    //if(buff[0] != 0)
    //    FURI_LOG_I(TAG, "spirx[%d]", buff[0]);
}

void W5500_WriteBuff(uint8_t* buff, uint16_t len) {
    furi_hal_spi_bus_tx(&furi_hal_spi_bus_handle_external, buff, len, 1000);
}

uint8_t W5500_ReadByte(void) {
    uint8_t byte;
    W5500_ReadBuff(&byte, sizeof(byte));
    return byte;
}

void W5500_WriteByte(uint8_t byte) {
    W5500_WriteBuff(&byte, sizeof(byte));
}

void wait_ms(int ms) {
    osDelay(ms);
}

wiz_NetInfo gWIZNETINFO = {
    .mac = {ETHADDR0, ETHADDR1, ETHADDR2, ETHADDR3, ETHADDR4, ETHADDR5},
    .ip = {IPADDR0, IPADDR1, IPADDR2, IPADDR3},
    .sn = {NETMASK0, NETMASK1, NETMASK2, NETMASK3},
    .gw = {DRIPADDR0, DRIPADDR1, DRIPADDR2, DRIPADDR3},
    .dns = {DRIPADDR0, DRIPADDR1, DRIPADDR2, DRIPADDR3},
    .dhcp = NETINFO_DHCP // NETINFO_STATIC
};

void eth_worker_dhcp(EthWorker* eth_worker) {
    furi_assert(eth_worker);
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_external);

    uint8_t temp;
    uint8_t W5500FifoSize[2][8] = {
        {
            2,
            2,
            2,
            2,
            2,
            2,
            2,
            2,
        },
        {2, 2, 2, 2, 2, 2, 2, 2}};

    uint8_t dhcp_buffer[2000];

    FURI_LOG_I(TAG, "Ehtping_Init\r\n");
    FURI_LOG_I(TAG, "Registering W5500 callbacks\r\n");
    FURI_LOG_I(TAG, "sizeof %d", sizeof(gWIZNETINFO));

    reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
    reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);
    reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);

    GpioPin resetpin = {.port = GPIOC, .pin = GPIO_PIN_3};
    furi_hal_gpio_write(&resetpin, true);
    furi_hal_gpio_write(&cspin, true);
    furi_hal_gpio_init(&resetpin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(&cspin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);

    while(eth_worker->state == EthWorkerStateDHCP) {
        furi_hal_power_enable_otg();
        eth_worker->callback(EthCustomEventModulePowerOn, eth_worker->context);
        osDelay(1000);
        furi_hal_gpio_write(&resetpin, false);
        osDelay(10);
        furi_hal_gpio_write(&resetpin, true);

        eth_worker->callback(EthCustomEventModuleConnect, eth_worker->context);

        if(ctlwizchip(CW_INIT_WIZCHIP, (void*)W5500FifoSize) == -1) {
            FURI_LOG_I(TAG, "W5500 initialized fail");
            eth_worker->callback(EthCustomEventModuleError, eth_worker->context);
            break;
        }

        FURI_LOG_I(TAG, "W5500 initialized success");
        osDelay(200);

        wizchip_setnetinfo(&gWIZNETINFO);
        FURI_LOG_I(TAG, "W5500 info setted 1");

        setSHAR(gWIZNETINFO.mac);
        FURI_LOG_I(TAG, "W5500 info setted 2");

        //check phy status
        do {
            if(ctlwizchip(CW_GET_PHYLINK, (void*)&temp) == -1) {
                FURI_LOG_I(TAG, "Unknown PHY link status.\r\n");
            }
            osDelay(1);
        } while(temp == PHY_LINK_OFF);

        FURI_LOG_I(TAG, "W5500 gWIZNETINFO success.\r\n");
        eth_worker->callback(EthCustomEventPHYConnect, eth_worker->context);

        osDelay(1000);

        FURI_LOG_I(TAG, "Registering DHCP callbacks.\r\n");
        reg_dhcp_cbfunc(Callback_IPAssigned, Callback_IPAssigned, Callback_IPConflict);

        eth_worker->callback(EthCustomEventDHCPConnect, eth_worker->context);
        if(gWIZNETINFO.dhcp == NETINFO_DHCP) {
            DHCP_init(DHCP_SOCKET, dhcp_buffer);

            uint8_t dhcp_ret = DHCP_STOPPED;

            while(
                !((dhcp_ret == DHCP_IP_ASSIGN) || (dhcp_ret == DHCP_IP_CHANGED) ||
                  (dhcp_ret == DHCP_FAILED) || (dhcp_ret == DHCP_IP_LEASED))) {
                dhcp_ret = DHCP_run();
                switch(dhcp_ret) {
                case DHCP_IP_ASSIGN:
                case DHCP_IP_CHANGED:
                case DHCP_IP_LEASED:
                    getIPfromDHCP(gWIZNETINFO.ip);
                    getGWfromDHCP(gWIZNETINFO.gw);
                    getSNfromDHCP(gWIZNETINFO.sn);
                    getDNSfromDHCP(gWIZNETINFO.dns);
                    gWIZNETINFO.dhcp = NETINFO_DHCP;
                    ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
                    FURI_LOG_I(
                        TAG, "\r\n>> DHCP IP Leased Time : %ld Sec\r\n", getDHCPLeasetime());
                    break;
                case DHCP_FAILED:
                    FURI_LOG_I(TAG, ">> DHCP Failed\r\n");
                    gWIZNETINFO.dhcp = NETINFO_STATIC;
                    break;
                }
                osDelay(1);
            }

            wizchip_getnetinfo(&gWIZNETINFO);
            FURI_LOG_I(
                TAG,
                "Mac address: %02x:%02x:%02x:%02x:%02x:%02x\n\r",
                gWIZNETINFO.mac[0],
                gWIZNETINFO.mac[1],
                gWIZNETINFO.mac[2],
                gWIZNETINFO.mac[3],
                gWIZNETINFO.mac[4],
                gWIZNETINFO.mac[5]);
            if(gWIZNETINFO.dhcp == NETINFO_DHCP)
                FURI_LOG_I(TAG, "DHCP\n\r");
            else
                FURI_LOG_I(TAG, "Static IP\n\r");
            FURI_LOG_I(
                TAG,
                "IP address : %d.%d.%d.%d\n\r",
                gWIZNETINFO.ip[0],
                gWIZNETINFO.ip[1],
                gWIZNETINFO.ip[2],
                gWIZNETINFO.ip[3]);
            FURI_LOG_I(
                TAG,
                "SM Mask    : %d.%d.%d.%d\n\r",
                gWIZNETINFO.sn[0],
                gWIZNETINFO.sn[1],
                gWIZNETINFO.sn[2],
                gWIZNETINFO.sn[3]);
            FURI_LOG_I(
                TAG,
                "Gate way   : %d.%d.%d.%d\n\r",
                gWIZNETINFO.gw[0],
                gWIZNETINFO.gw[1],
                gWIZNETINFO.gw[2],
                gWIZNETINFO.gw[3]);
            FURI_LOG_I(
                TAG,
                "DNS Server : %d.%d.%d.%d\n\r",
                gWIZNETINFO.dns[0],
                gWIZNETINFO.dns[1],
                gWIZNETINFO.dns[2],
                gWIZNETINFO.dns[3]);
            eth_worker->callback(EthCustomEventDHCPConnectSuccess, eth_worker->context);
            osDelay(20000);
        }

        uint8_t pDestaddr[4] = {8, 8, 8, 8};
        uint8_t tmp = ping_auto(1, pDestaddr);
        //tmp = ping_count(0,3,pDestaddr);
        if(tmp == SUCCESS) {
            eth_worker->callback(EthCustomEventPingConnect, eth_worker->context);
            FURI_LOG_I(TAG, "-----------PING TEST OK----------\r\n");
        } else {
            eth_worker->callback(EthCustomEventPingError, eth_worker->context);
            FURI_LOG_I(TAG, "----------ERROR  = %d----------\r\n", tmp);
        }
        osDelay(3000);

        osDelay(2000);
        eth_worker->callback(EthCustomEventWellDone, eth_worker->context);

        break;
    }

    furi_hal_spi_release(&furi_hal_spi_bus_handle_external);
}

static void w5500_init() {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_external);
    uint8_t W5500FifoSize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
    FURI_LOG_I(TAG, "Registering W5500 callbacks");

    reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
    reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);
    reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);

    GpioPin resetpin = {.port = GPIOC, .pin = GPIO_PIN_3};
    furi_hal_gpio_write(&resetpin, true);
    furi_hal_gpio_write(&cspin, true);
    furi_hal_gpio_init(&resetpin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(&cspin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);
}

static void w5500_deinit() {
    furi_hal_spi_release(&furi_hal_spi_bus_handle_external);
}

void eth_worker_w5500(EthWorker* eth_worker) {
    furi_assert(eth_worker);

    //uint8_t temp;

    FURI_LOG_I(TAG, "Ehtping_Init");

    while(eth_worker->state == EthWorkerStateW5500) {
        furi_hal_power_enable_otg();
        osDelay(1000);
        eth_worker->callback(EthCustomEventModulePowerOn, eth_worker->context);
        osDelay(2000);
        furi_hal_gpio_write(&resetpin, false);
        osDelay(10);
        furi_hal_gpio_write(&resetpin, true);

        eth_worker->callback(EthCustomEventModuleConnect, eth_worker->context);

        if(ctlwizchip(CW_INIT_WIZCHIP, (void*)W5500FifoSize) == -1) {
            FURI_LOG_I(TAG, "W5500 initialized fail.\r\n");
            eth_worker->callback(EthCustomEventModuleError, eth_worker->context);
            break;
        }

        FURI_LOG_I(TAG, "W5500 initialized success.\r\n");
        osDelay(2000);

        wizchip_setnetinfo(&gWIZNETINFO);
        FURI_LOG_I(TAG, "W5500 info setted 1.\r\n");

        setSHAR(gWIZNETINFO.mac);
        FURI_LOG_I(TAG, "W5500 info setted 2.\r\n");

        //check phy status
        //do
        //{
        //    if (ctlwizchip(CW_GET_PHYLINK, (void*)&temp) == -1)
        //    {
        //        FURI_LOG_I(TAG, "Unknown PHY link status.\r\n");
        //    }
        //    osDelay(1);
        //} while (temp == PHY_LINK_OFF);

        //FURI_LOG_I(TAG, "W5500 gWIZNETINFO success.\r\n");
        //eth_worker->callback(EthCustomEventPHYConnect, eth_worker->context);

        FURI_LOG_I(TAG, "W5500 before delay\r\n");

        osDelay(2000);

        FURI_LOG_I(TAG, "W5500 after delay\r\n");

        furi_hal_power_disable_otg();

        FURI_LOG_I(TAG, "W5500 power off\r\n");

        eth_worker->callback(EthCustomEventModuleError, eth_worker->context);

        osDelay(2000);

        eth_worker->callback(EthCustomEventModuleConnected, eth_worker->context);

        while(eth_worker->state == EthWorkerStateW5500) {
            osDelay(10);
        }

        break;
    }

    furi_hal_spi_release(&furi_hal_spi_bus_handle_external);
}
