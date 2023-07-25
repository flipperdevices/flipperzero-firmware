#include "eth_worker_i.h"
#include "eth_worker.h"
#include "eth_save_process.h"

#include <furi_hal.h>
#include "dhcp.h"
#include "ping.h"
#include "socket.h"
#include "stm32wbxx_hal_gpio.h"
#include "wizchip_conf.h"

#define TAG "EthWorker"

static EthWorker* static_worker = NULL;

EthWorker* eth_worker_alloc() {
    EthWorker* worker = malloc(sizeof(EthWorker));

    worker->config = ehternet_save_process_malloc();
    furi_assert(worker->config);

    worker->init_process = ethernet_view_process_malloc(EthWorkerProcessInit, worker->config);
    worker->dhcp_process = ethernet_view_process_malloc(EthWorkerProcessDHCP, worker->config);
    worker->stat_process = ethernet_view_process_malloc(EthWorkerProcessStatic, worker->config);
    worker->ping_process = ethernet_view_process_malloc(EthWorkerProcessPing, worker->config);
    worker->reset_process = ethernet_view_process_malloc(EthWorkerProcessReset, worker->config);
    worker->active_process = worker->init_process;

    static_worker = worker;

    worker->state = worker->next_state = EthWorkerStateNotAllocated;

    eth_log(EthWorkerProcessReset, "Finik Ethernet [START]");

    return worker;
}

void eth_worker_free(EthWorker* worker) {
    eth_log(EthWorkerProcessReset, "Finik Ethernet [STOP]");

    eth_run(worker, EthWorkerProcessExit);

    static_worker = NULL;
    furi_assert(worker);

    ethernet_view_process_free(worker->init_process);
    ethernet_view_process_free(worker->dhcp_process);
    ethernet_view_process_free(worker->stat_process);
    ethernet_view_process_free(worker->ping_process);
    ethernet_view_process_free(worker->reset_process);
    ehternet_save_process_free(worker->config);
    free(worker);
}

void eth_worker_change_state(EthWorker* worker, EthWorkerState state) {
    furi_assert(worker);
    worker->state = state;
}

void eth_worker_set_active_process(EthWorker* worker, EthWorkerProcess state) {
    furi_assert(worker);
    switch(state) {
    case EthWorkerProcessInit:
        worker->active_process = worker->init_process;
        break;
    case EthWorkerProcessDHCP:
        worker->active_process = worker->dhcp_process;
        break;
    case EthWorkerProcessStatic:
        worker->active_process = worker->stat_process;
        break;
    case EthWorkerProcessPing:
        worker->active_process = worker->ping_process;
        break;
    case EthWorkerProcessReset:
        worker->active_process = worker->reset_process;
        break;
    }
}

void eth_worker_log(EthWorker* worker, const char* str) {
    furi_assert(worker);
    ehternet_save_process_print(worker->config, str);
}

static EthViewProcess* get_process(EthWorker* worker, EthWorkerProcess process) {
    furi_assert(worker);
    switch(process) {
    case EthWorkerProcessInit:
        return worker->init_process;
    case EthWorkerProcessDHCP:
        return worker->dhcp_process;
    case EthWorkerProcessStatic:
        return worker->stat_process;
    case EthWorkerProcessPing:
        return worker->ping_process;
    case EthWorkerProcessReset:
        return worker->reset_process;
    case EthWorkerProcessActive:
        return worker->active_process;
    default:
        NULL;
    }
}

void eth_log(EthWorkerProcess process, const char* format, ...) {
    furi_assert(static_worker);
    va_list args;
    va_start(args, format);
    FuriString* fstring = furi_string_alloc_vprintf(format, args);
    const char* string = furi_string_get_cstr(fstring);
    va_end(args);

    FURI_LOG_I(TAG, "%s", string);
    ehternet_save_process_print(static_worker->config, string);
    ethernet_view_process_print(get_process(static_worker, process), string);
    if(process != EthWorkerProcessReset) {
        ethernet_view_process_print(get_process(static_worker, EthWorkerProcessReset), string);
    }
    furi_string_free(fstring);
}

void eth_run(EthWorker* worker, EthWorkerProcess process) {
    furi_assert(worker);
    switch(process) {
    case EthWorkerProcessInit:
        if(worker->state == EthWorkerStateNotAllocated) {
            worker->thread = furi_thread_alloc();
            furi_thread_set_name(worker->thread, "EthWorker");
            furi_thread_set_stack_size(worker->thread, 8192);
            furi_thread_set_callback(worker->thread, eth_worker_task);
            furi_thread_set_context(worker->thread, worker);
            worker->state = EthWorkerStateNotInited;
            worker->next_state = EthWorkerStateInit;
            furi_thread_start(worker->thread);
        }
        worker->state = EthWorkerStateNotInited;
        worker->next_state = EthWorkerStateInit;
        break;
    case EthWorkerProcessDHCP:
        if((uint8_t)worker->state < EthWorkerStateInited) {
            eth_log(EthWorkerProcessDHCP, "[error] module not inited");
            break;
        }
        worker->next_state = EthWorkerStateDHCP;
        break;
    case EthWorkerProcessStatic:
        if((uint8_t)worker->state < EthWorkerStateInited) {
            eth_log(EthWorkerProcessStatic, "[error] module not inited");
            break;
        }
        worker->next_state = EthWorkerStateStaticIp;
        eth_log(EthWorkerProcessStatic, "Fuck you");
        break;
    case EthWorkerProcessPing:
        if((uint8_t)worker->state < EthWorkerStateInited) {
            eth_log(EthWorkerProcessPing, "[error] module not inited");
            break;
        }
        worker->next_state = EthWorkerStatePing;
        eth_log(EthWorkerProcessPing, "Fuck you");
        break;
    case EthWorkerProcessReset:
        worker->next_state = EthWorkerStateNotInited;
        if((uint8_t)worker->state > EthWorkerStateNotAllocated) {
            worker->state = EthWorkerStateNotInited;
        }
        eth_log(EthWorkerProcessReset, "reset module");
        break;
    case EthWorkerProcessExit:
        if(worker->state != EthWorkerStateNotAllocated) {
            worker->next_state = EthWorkerStateStop;
            furi_thread_join(worker->thread);
            furi_thread_free(worker->thread);
            worker->state = EthWorkerStateNotAllocated;
        }
        break;
    default:
        break;
    }
}

/************************** Ethernet Worker Thread *****************************/

static uint8_t ip_assigned = 0;
static GpioPin cspin = {.port = GPIOA, .pin = GPIO_PIN_4};
static GpioPin resetpin = {.port = GPIOC, .pin = GPIO_PIN_3};

static void W5500_Select(void) {
    furi_hal_gpio_write(&cspin, false);
}

static void W5500_Unselect(void) {
    furi_hal_gpio_write(&cspin, true);
}

static void Callback_IPAssigned(void) {
    eth_log(
        EthWorkerProcessDHCP, "Callback: IP assigned! Leased time: %d sec", getDHCPLeasetime());
    ip_assigned = 1;
}

static void Callback_IPConflict(void) {
    eth_log(EthWorkerProcessDHCP, "Callback: IP conflict!");
}

static void W5500_ReadBuff(uint8_t* buff, uint16_t len) {
    furi_hal_spi_bus_rx(&furi_hal_spi_bus_handle_external, buff, len, 1000);
}

static void W5500_WriteBuff(uint8_t* buff, uint16_t len) {
    furi_hal_spi_bus_tx(&furi_hal_spi_bus_handle_external, buff, len, 1000);
}

static uint8_t W5500_ReadByte(void) {
    uint8_t byte;
    W5500_ReadBuff(&byte, sizeof(byte));
    return byte;
}

static void W5500_WriteByte(uint8_t byte) {
    W5500_WriteBuff(&byte, sizeof(byte));
}

static void wait_ms(int ms) {
    furi_delay_ms(ms);
}

static wiz_NetInfo gWIZNETINFO;
void update_WIZNETINFO(uint8_t is_dhcp) {
    furi_assert(static_worker);
    memcpy(gWIZNETINFO.mac, static_worker->config->mac, 6);
    if(is_dhcp) {
        memset(gWIZNETINFO.ip, 0, 4);
        memset(gWIZNETINFO.sn, 0, 4);
        memset(gWIZNETINFO.gw, 0, 4);
        memset(gWIZNETINFO.dns, 0, 4);
        gWIZNETINFO.dhcp = NETINFO_DHCP;
    } else {
        memcpy(gWIZNETINFO.ip, static_worker->config->ip, 4);
        memcpy(gWIZNETINFO.sn, static_worker->config->mask, 4);
        memcpy(gWIZNETINFO.gw, static_worker->config->gateway, 4);
        memcpy(gWIZNETINFO.dns, static_worker->config->dns, 4);
        gWIZNETINFO.dhcp = NETINFO_STATIC;
    }
}

#define DHCP_SOCKET 0
uint8_t ping_auto(uint8_t s, uint8_t* addr);

int32_t eth_worker_task(void* context) {
    furi_assert(context);
    EthWorker* worker = (EthWorker*)context;
    furi_hal_power_insomnia_enter();

    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_external);
    uint8_t W5500FifoSize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
    uint8_t dhcp_buffer[2000];

    reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
    reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);
    reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);

    furi_hal_gpio_write(&resetpin, true);
    furi_hal_gpio_write(&cspin, true);
    furi_hal_gpio_init(&resetpin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(&cspin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);

    while(worker->next_state != EthWorkerStateStop && worker->state != EthWorkerStateStop) {
        if(worker->state == EthWorkerStateNotInited) {
            if(worker->next_state != EthWorkerStateInit &&
               worker->next_state != EthWorkerStateNotInited) {
                eth_log(EthWorkerProcessActive, "[error] try using not inited module");
                worker->next_state = EthWorkerStateNotInited;
            }
            if(worker->next_state == EthWorkerStateInit) {
                worker->state = EthWorkerStateInit;
                furi_hal_power_enable_otg();
                furi_delay_ms(300);
                furi_hal_gpio_write(&resetpin, false);
                furi_delay_ms(50);
                furi_hal_gpio_write(&resetpin, true);
                if(ctlwizchip(CW_INIT_WIZCHIP, (void*)W5500FifoSize) == -1) {
                    eth_log(EthWorkerProcessInit, "[error] W5500 init fail");
                    worker->state = worker->next_state = EthWorkerStateNotInited;
                    continue;
                }
                eth_log(EthWorkerProcessInit, "W5500 inited");
                furi_delay_ms(50);
                update_WIZNETINFO(false);
                wizchip_setnetinfo(&gWIZNETINFO);
                wiz_NetInfo readed_net_info;
                wizchip_getnetinfo(&readed_net_info);
                if(memcmp(&readed_net_info, &gWIZNETINFO, sizeof(wiz_NetInfo))) {
                    eth_log(EthWorkerProcessInit, "[error] module not detected");
                    worker->state = EthWorkerStateNotInited;
                    continue;
                }
                setSHAR(gWIZNETINFO.mac);
                wiz_PhyConf conf;
                wizphy_getphyconf(&conf);
                eth_log(
                    EthWorkerProcessInit,
                    "conf %d %d %d %d",
                    conf.by,
                    conf.mode,
                    conf.speed,
                    conf.duplex);
                eth_log(EthWorkerProcessInit, "net info setted");
                eth_log(
                    EthWorkerProcessInit,
                    "mac: %02X-%02X-%02X-%02X-%02X-%02X",
                    gWIZNETINFO.mac[0],
                    gWIZNETINFO.mac[1],
                    gWIZNETINFO.mac[2],
                    gWIZNETINFO.mac[3],
                    gWIZNETINFO.mac[4],
                    gWIZNETINFO.mac[5]);
                worker->state = EthWorkerStateInited;
                continue;
            }
        } else if(worker->state == EthWorkerStateInited) {
            if(worker->next_state == EthWorkerStateDHCP) {
                worker->state = EthWorkerStateDHCP;
                uint8_t temp = PHY_LINK_OFF;
                while(temp == PHY_LINK_OFF && worker->state == EthWorkerStateDHCP) {
                    if(ctlwizchip(CW_GET_PHYLINK, (void*)&temp) == -1) {
                        eth_log(EthWorkerProcessDHCP, "Unknown PHY link status");
                    }
                    furi_delay_ms(1);
                }
                if(worker->state != EthWorkerStateDHCP) {
                    break;
                }
                reg_dhcp_cbfunc(Callback_IPAssigned, Callback_IPAssigned, Callback_IPConflict);
                DHCP_init(DHCP_SOCKET, dhcp_buffer);
                uint8_t dhcp_ret = DHCP_STOPPED;
                uint8_t next_cycle = 1;
                while(next_cycle && worker->state == EthWorkerStateDHCP) {
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
                        eth_log(
                            EthWorkerProcessDHCP,
                            "DHCP IP Leased Time : %ld Sec",
                            getDHCPLeasetime());
                        break;
                    case DHCP_FAILED:
                        eth_log(EthWorkerProcessDHCP, "DHCP Failed");
                        break;
                    }
                    furi_delay_ms(1000);
                    next_cycle = (dhcp_ret == DHCP_RUNNING);
                }
                if(worker->state != EthWorkerStateDHCP) {
                    break;
                }
                //wizchip_getnetinfo(&gWIZNETINFO);
                eth_log(
                    EthWorkerProcessDHCP,
                    "IP address:\n %d.%d.%d.%d",
                    gWIZNETINFO.ip[0],
                    gWIZNETINFO.ip[1],
                    gWIZNETINFO.ip[2],
                    gWIZNETINFO.ip[3]);
                eth_log(
                    EthWorkerProcessDHCP,
                    "SM Mask:\n %d.%d.%d.%d",
                    gWIZNETINFO.sn[0],
                    gWIZNETINFO.sn[1],
                    gWIZNETINFO.sn[2],
                    gWIZNETINFO.sn[3]);
                eth_log(
                    EthWorkerProcessDHCP,
                    "Gate way:\n %d.%d.%d.%d",
                    gWIZNETINFO.gw[0],
                    gWIZNETINFO.gw[1],
                    gWIZNETINFO.gw[2],
                    gWIZNETINFO.gw[3]);
                eth_log(
                    EthWorkerProcessDHCP,
                    "DNS Server:\n %d.%d.%d.%d",
                    gWIZNETINFO.dns[0],
                    gWIZNETINFO.dns[1],
                    gWIZNETINFO.dns[2],
                    gWIZNETINFO.dns[3]);
                worker->state = EthWorkerStateOnline;
            }
        } else if(worker->state == EthWorkerStateOnline) {
            if(worker->next_state == EthWorkerStatePing) {
                worker->state = EthWorkerStatePing;
                uint8_t* adress = static_worker->config->ping_ip;
                eth_log(
                    EthWorkerProcessDHCP,
                    "ping %d.%d.%d.%d",
                    adress[0],
                    adress[1],
                    adress[2],
                    adress[3]);
                const uint8_t tryes = 4;
                uint8_t try = 0;
                while(try < tryes && worker->state == EthWorkerStatePing) {
                    try++;
                    uint32_t start_time = furi_get_tick();
                    uint8_t res = 3; //ping_auto(1, adress);
                    uint32_t res_time = furi_get_tick();
                    if(res == 3) {
                        eth_log(
                            EthWorkerProcessDHCP, "%d success %d ms", try, res_time - start_time);
                    } else {
                        eth_log(
                            EthWorkerProcessDHCP,
                            "%d error %d, %d",
                            try,
                            res,
                            res_time - start_time);
                        break;
                    }
                }
                if(worker->state != EthWorkerStatePing) {
                    break;
                }
                worker->state = EthWorkerStateOnline;
            } else {
            }
        }
        furi_delay_ms(50);
    }

    furi_hal_spi_release(&furi_hal_spi_bus_handle_external);
    furi_hal_power_disable_otg();
    furi_hal_power_insomnia_exit();

    return 0;
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
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_external);
    uint8_t W5500FifoSize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
    FURI_LOG_I(TAG, "Registering W5500 callbacks");

    reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
    reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);
    reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);

    FURI_LOG_I(TAG, "Registered W5500 callbacks");

    GpioPin resetpin = {.port = GPIOC, .pin = GPIO_PIN_3};
    furi_hal_gpio_write(&resetpin, true);
    furi_hal_gpio_write(&cspin, true);
    furi_hal_gpio_init(&resetpin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(&cspin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);

    FURI_LOG_I(TAG, "GPIO inited");

    furi_hal_power_enable_otg();
    furi_delay_ms(1000);
    //eth_worker->callback(EthCustomEventModulePowerOn, eth_worker->context);
    furi_delay_ms(2000);
    furi_hal_gpio_write(&resetpin, false);
    furi_delay_ms(10);
    furi_hal_gpio_write(&resetpin, true);

    FURI_LOG_I(TAG, "GPIO used");

    //eth_worker->callback(EthCustomEventModuleConnect, eth_worker->context);

    if(ctlwizchip(CW_INIT_WIZCHIP, (void*)W5500FifoSize) == -1) {
        FURI_LOG_I(TAG, "W5500 initialized fail.\r\n");
        //eth_worker->callback(EthCustomEventModuleError, eth_worker->context);
    }

    FURI_LOG_I(TAG, "W5500 initialized success.\r\n");
    furi_delay_ms(2000);

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
    //    furi_delay_ms(1);
    //} while (temp == PHY_LINK_OFF);

    //FURI_LOG_I(TAG, "W5500 gWIZNETINFO success.\r\n");
    ////eth_worker->callback(EthCustomEventPHYConnect, eth_worker->context);

    FURI_LOG_I(TAG, "W5500 before delay\r\n");

    furi_delay_ms(2000);

    FURI_LOG_I(TAG, "W5500 after delay\r\n");

    //furi_hal_power_disable_otg();

    //FURI_LOG_I(TAG, "W5500 power off\r\n");

    ////eth_worker->callback(EthCustomEventModuleError, eth_worker->context);

    furi_delay_ms(2000);

    ////eth_worker->callback(EthCustomEventModuleConnected, eth_worker->context);

    furi_hal_spi_release(&furi_hal_spi_bus_handle_external);
}
