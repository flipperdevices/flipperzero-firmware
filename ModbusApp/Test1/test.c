//////////////////////////   Libraries  //////////////////////////
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_box.h>
#include <assets_icons.h>

#include <stm32wbxx_ll_lpuart.h>
#include <stm32wbxx_ll_usart.h>

#include <storage/storage.h>
#include <dialogs/dialogs.h>

#define PATHAPP "apps_data/modbus"
#define PATHAPPEXT EXT_PATH(PATHAPP)
#define PATHLOGS PATHAPPEXT "/logs"

#define BR_VALUES 12
#define DATAWIDTH_VALUES 3
#define STOPBITS_VALUES 4
#define PARITY_VALUES 3
#define TIMEOUT_VALUES 255
#define DIGITALFORMAT_VALUES 2
#define ANALOGFORMAT_VALUES 2
#define SAVE_LOG_VALUES 2

#define RX_BUF_SIZE 255
#define UART_CH FuriHalSerialIdUsart
#define TEXT_BOX_LEN 4096
#define FURI_HAL_SERIAL_USART_OVERSAMPLING 0x00000000U
#define TIMEOUT_SCALER 50
//////////////////////////   Defining Structs  //////////////////////////
typedef enum { Main_Scene, Settings_Scene, ConsoleOutput_Scene, Scene_Num } Scenes;
typedef enum { Submenu_View, VarList_View, TextBox_View } Views;
typedef enum { Settings_Option, Sniffer_Option, Read_LOG_Option } Main_options;

typedef struct {
    uint8_t baudrate;
    uint8_t dataWidth;
    uint8_t stopBits;
    uint8_t parity;
    uint8_t timeout;
    bool hexOutput;
    bool saveLOG;
} Config;

typedef struct {
    Config* cfg;
    FuriThread* rxThread;
    FuriStreamBuffer* rxStream;
    FuriHalSerialHandle* serial_handle;
    uint8_t rxBuff[RX_BUF_SIZE + 1];
} Uart;
typedef struct {
    bool slave;
    FuriString* timeout;
} Modbus;

typedef struct {
    SceneManager* sceneManager;
    ViewDispatcher* viewDispatcher;
    Submenu* subMenu;
    VariableItemList* varList;
    Uart* uart;
    Modbus* modbus;
    DialogsApp* dialogs;
    Storage* storage;
    File* LOGfile;
    char logFilePath[100];
    bool LOGfileReady;

    TextBox* textBox;
    FuriString* text;
    size_t textLen;

    FuriTimer* timer;
} App;

typedef enum {
    BaudRate_Option,
    DataWidth_Option,
    StopBits_Option,
    Parity_Option,
    TimeOut_Option,
    OutputFormat_Option,
    SaveLOG_Option
} Settings_Options;

typedef enum { Refresh = 0 } UartEvents;

///////////////////////////////   Storage   ///////////////////////////////////////

char* sequential_file_resolve_path(
    Storage* storage,
    const char* dir,
    const char* prefix,
    const char* extension) {
    if(storage == NULL || dir == NULL || prefix == NULL || extension == NULL) {
        return NULL;
    }

    char file_path[256];
    int file_index = 0;

    do {
        if(snprintf(
               file_path, sizeof(file_path), "%s/%s_%d.%s", dir, prefix, file_index, extension) <
           0) {
            return NULL;
        }
        file_index++;
    } while(storage_file_exists(storage, file_path));

    return strdup(file_path);
}

void makePaths(App* app) {
    furi_assert(app);
    if(!storage_simply_mkdir(app->storage, PATHAPPEXT)) {
        dialog_message_show_storage_error(app->dialogs, "Cannot create\napp folder");
    }
    if(!storage_simply_mkdir(app->storage, PATHLOGS)) {
        dialog_message_show_storage_error(app->dialogs, "Cannot create\nlogs folder");
    }
}
bool OpenLogFile(App* app) {
    // browse for files
    FuriString* predefined_filepath = furi_string_alloc_set_str(PATHLOGS);
    FuriString* selected_filepath = furi_string_alloc();
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, ".log", NULL);
    if(!dialog_file_browser_show(
           app->dialogs, selected_filepath, predefined_filepath, &browser_options)) {
        return false;
    }
    if(storage_file_open(
           app->LOGfile, furi_string_get_cstr(selected_filepath), FSAM_READ, FSOM_OPEN_EXISTING)) {
        furi_string_reset(app->text);
        char buf[storage_file_size(app->LOGfile)];
        storage_file_read(app->LOGfile, buf, sizeof(buf));
        furi_string_cat_str(app->text, buf);
        app->uart->cfg->saveLOG = false;
    } else
        dialog_message_show_storage_error(app->dialogs, "Cannot open File");
    storage_file_close(app->LOGfile);
    furi_string_free(selected_filepath);
    furi_string_free(predefined_filepath);

    return true;
}
///////////////////////////////   UART   ///////////////////////////////////////

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),

    WorkerEvtTxStop = (1 << 2),
    WorkerEvtCdcRx = (1 << 3),
    WorkerEvtCdcTxComplete = (1 << 4),

    WorkerEvtCfgChange = (1 << 5),

    WorkerEvtLineCfgSet = (1 << 6),
    WorkerEvtCtrlLineSet = (1 << 7),

} WorkerEvtFlags;
#define WORKER_ALL_RX_EVENTS                                                      \
    (WorkerEvtStop | WorkerEvtRxDone | WorkerEvtCfgChange | WorkerEvtLineCfgSet | \
     WorkerEvtCtrlLineSet | WorkerEvtCdcTxComplete)
#define WORKER_ALL_TX_EVENTS (WorkerEvtTxStop | WorkerEvtCdcRx)

static const char* baudrateValues[] = {
    "1200",
    "2400",
    "4800",
    "9600",
    "19200",
    "28800",
    "38400",
    "57600",
    "115200",
    "230400",
    "460800",
    "921600",
};
static const char* dataWidthValues[] = {"7", "8", "9"};
static const char* stopBitsValues[] = {"0.5", "1", "1.5", "2"};
static const char* parityValues[] = {"None", "Even", "Odd"};
static const char* saveLOGValues[] = {"OFF", "ON"};
static const char* outputFormatValues[] = {"Default", "HEX"};
static const char* functionNames[] = {
    "Read Coils(01)",
    "Read Discrete Inputs(02)",
    "Read Holding Registers(03)",
    "Read Input Registers(04)",
    "Write Single Coil(05)",
    "Write Single Register(06)",
    "Write Multiple Coils(0F)",
    "Write Multiple Registers(10)"};

LL_USART_InitTypeDef buildUartSettings(Config* cfg) {
    LL_USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;

    USART_InitStruct.BaudRate = atoi(baudrateValues[cfg->baudrate]);
    USART_InitStruct.DataWidth =
        (cfg->dataWidth == 0 ? LL_USART_DATAWIDTH_7B :
         cfg->dataWidth == 2 ? LL_USART_DATAWIDTH_9B :
                               LL_USART_DATAWIDTH_8B);

    USART_InitStruct.StopBits =
        (cfg->stopBits == 0 ? LL_USART_STOPBITS_0_5 :
         cfg->stopBits == 2 ? LL_USART_STOPBITS_1_5 :
         cfg->stopBits == 3 ? LL_USART_STOPBITS_2 :
                              LL_USART_STOPBITS_1);

    USART_InitStruct.Parity =
        (cfg->parity == 1 ? LL_USART_PARITY_EVEN :
         cfg->parity == 2 ? LL_USART_PARITY_ODD :
                            LL_USART_PARITY_NONE);

    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = FURI_HAL_SERIAL_USART_OVERSAMPLING;
    return USART_InitStruct;
}
void uart_set_config(void* context) {
    furi_assert(context);
    App* app = context;
    furi_thread_flags_set(furi_thread_get_id(app->uart->rxThread), WorkerEvtCfgChange);
}
static void Serial_Begin(FuriHalSerialHandle* handle, LL_USART_InitTypeDef USART_InitStruct) {
    furi_hal_bus_enable(FuriHalBusUSART1);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);

    furi_hal_gpio_init_ex(
        &gpio_usart_tx,
        GpioModeAltFunctionPushPull,
        GpioPullUp,
        GpioSpeedVeryHigh,
        GpioAltFn7USART1);
    furi_hal_gpio_init_ex(
        &gpio_usart_rx,
        GpioModeAltFunctionPushPull,
        GpioPullUp,
        GpioSpeedVeryHigh,
        GpioAltFn7USART1);
    LL_USART_Init(USART1, &USART_InitStruct);
    LL_USART_EnableFIFO(USART1);
    LL_USART_ConfigAsyncMode(USART1);

    LL_USART_Enable(USART1);

    while(!LL_USART_IsActiveFlag_TEACK(USART1) || !LL_USART_IsActiveFlag_REACK(USART1))
        ;

    furi_hal_serial_set_br(handle, USART_InitStruct.BaudRate);
    LL_USART_DisableIT_ERROR(USART1);
}
uint16_t getCRC(uint8_t* buf, uint8_t len) {
    uint16_t crc = 0xFFFF;

    for(int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc

        for(int i = 8; i != 0; i--) { // Loop over each bit
            if((crc & 0x0001) != 0) { // If the LSB is set
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            } else // Else LSB is not set
                crc >>= 1; // Just shift right
        }
    }
    return crc;
}

void discreteValuesParser(void* context, uint8_t* buff, size_t len) {
    App* app = context;
    uint8_t value = 0;
    uint8_t offset = 0;
    while(len) {
        memcpy(&value, buff + offset, 1);
        offset++;
        if(!app->uart->cfg->hexOutput) {
            furi_string_cat_printf(app->text, "\n-Byte%d: \n->", offset);
            for(int i = 0; i < 8; i++)
                furi_string_cat_printf(
                    app->text,
                    "%s%s",
                    value >> i && 0x01 ? "ON" : "OFF",
                    i == 3 ? "\n->" :
                    i == 7 ? "" :
                             ",");
        } else
            furi_string_cat_printf(app->text, "\n->Byte%d: 0x%02X", offset, value);
        len--;
    }
}
void analogValuesParser(void* context, uint8_t* buff, size_t len) {
    App* app = context;
    uint16_t value = 0;
    uint8_t offset = 0;
    while(len) {
        memcpy(&value + 8, buff + offset, 1);
        offset++;
        memcpy(&value, buff + offset, 1);
        offset++;
        furi_string_cat_printf(
            app->text,
            app->uart->cfg->hexOutput ? "\n->Reg%d: 0x%04X" : "\n->Reg%d: %d",
            offset / 2,
            value);
        len--;
    }
}
void pduParser(void* context, bool slave, uint8_t Fn, uint8_t* pdu, size_t len) {
    App* app = context;
    size_t offset = 0;
    uint16_t address = 0;
    uint16_t qty = 0;
    uint16_t bCount = 0;
    uint16_t value = 0;
    UNUSED(len);
    //TODO: Handle error codes, unsupported codes & detect type-length missmatch
    memcpy(slave && Fn <= 4 ? &bCount : &address, pdu + offset, slave && Fn <= 4 ? 1 : 2);
    offset += slave && Fn <= 4 ? 1 : 2;
    address = address >> 8 | address << 8;
    if(app->uart->cfg->hexOutput)
        furi_string_cat_printf(
            app->text,
            slave && Fn <= 4 ? "\nbCount: 0x%02X" : "\nAddress: 0x%04X",
            slave && Fn <= 4 ? bCount : address);
    else
        furi_string_cat_printf(
            app->text,
            slave && Fn <= 4 ? "\nbCount: %d" : "\nAddress: %d",
            slave && Fn <= 4 ? bCount : address);

    if(Fn >= 0x0F || (!slave && Fn <= 0x04)) {
        memcpy(&qty, pdu + offset, 2);
        offset += 2;
        qty = qty >> 8 | qty << 8;
        furi_string_cat_printf(
            app->text, app->uart->cfg->hexOutput ? "\nQty: 0x%04X" : "\nQty: %d", qty);
    } else if(Fn >= 0x05) {
        memcpy(&value, pdu + offset, 2);
        offset += 2;
        value = value >> 8 | value << 8;
        furi_string_cat_printf(
            app->text, app->uart->cfg->hexOutput ? "\nValue: 0x%04X" : "\nValue: %d", value);
    } else if(Fn <= 0x02)
        discreteValuesParser(app, pdu + offset, bCount);
    else
        analogValuesParser(app, pdu + offset, bCount / 2);

    if(Fn >= 0x0F && !slave) {
        memcpy(&bCount, pdu + offset, 1);
        offset++;
        furi_string_cat_printf(
            app->text, app->uart->cfg->hexOutput ? "\nbCount: 0x%02X" : "\nbCount: %d", bCount);
        if(Fn == 0x0F)
            discreteValuesParser(app, pdu + offset, bCount);
        else
            analogValuesParser(app, pdu + offset, bCount / 2);
    }
}

void handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    App* app = context;
    app->textLen += len;
    buf[len] = '\0';
    if(app->textLen >= TEXT_BOX_LEN - 1) {
        furi_string_right(app->text, app->textLen / 2);
        app->textLen = furi_string_size(app->text) + len;
    }
    FuriString* data = furi_string_alloc();
    furi_string_reset(data);
    for(size_t i = 0; i < len; i++) {
        furi_string_cat_printf(data, "%02X", buf[i]);
    }
    uint8_t Slave;
    uint8_t Fn;
    size_t pduLen = len - 4;
    uint16_t crc;
    //uint16_t mycrc = getCRC(buf,len-2);

    memcpy(&Slave, buf, 1);
    memcpy(&Fn, buf + 1, 1);
    memcpy(&crc, buf + len - 2, 2);
    furi_string_cat_printf(
        app->text, "\n------%s-------", app->modbus->slave ? "-SLAVE" : "MASTER");
    furi_string_cat_printf(app->text, "\n%s", functionNames[Fn < 6 ? Fn - 1 : Fn - 9]);
    furi_string_cat_printf(
        app->text, app->uart->cfg->hexOutput ? "\nSlave: 0x%02X" : "\nSlave: %d", Slave);
    pduParser(app, app->modbus->slave, Fn, buf + 2, pduLen);
    crc = crc >> 8 | crc << 8;
    furi_string_cat_printf(app->text, "\nDRC: 0x%02X", crc);
    //for(size_t i = 0; i < furi_s tring_size(data); i++)buf[i] = (uint8_t)furi_string_get_char(data, i);
    //furi_string_cat_printf(app->text, "%s", furi_string_get_cstr(data));
    //if(app->LOGfileReady) storage_file_write(app->LOGfile, buf, furi_string_size(data));
    furi_string_free(data);
    view_dispatcher_send_custom_event(app->viewDispatcher, Refresh);
    if(app->modbus->slave) {
        app->modbus->slave = false;
        furi_timer_stop(app->timer);
    } else {
        app->modbus->slave = true;
        furi_timer_start(app->timer, app->uart->cfg->timeout * TIMEOUT_SCALER);
    }
}
static void
    on_rx_cb(FuriHalSerialHandle* handle, FuriHalSerialRxEvent ev, size_t size, void* context) {
    Uart* uart = context;
    if(ev & (FuriHalSerialRxEventData | FuriHalSerialRxEventIdle)) {
        uint8_t data[FURI_HAL_SERIAL_DMA_BUFFER_SIZE] = {0};
        while(size) {
            size_t ret = furi_hal_serial_dma_rx(
                handle,
                data,
                (size > FURI_HAL_SERIAL_DMA_BUFFER_SIZE) ? FURI_HAL_SERIAL_DMA_BUFFER_SIZE : size);
            furi_stream_buffer_send(uart->rxStream, data, ret, 0);
            size -= ret;
        };
        furi_thread_flags_set(furi_thread_get_id(uart->rxThread), WorkerEvtRxDone);
    }
}
static void serial_init(Uart* uart, uint8_t uart_ch) {
    furi_assert(!uart->serial_handle);
    uart->serial_handle = furi_hal_serial_control_acquire(uart_ch);
    furi_assert(uart->serial_handle);

    Serial_Begin(uart->serial_handle, buildUartSettings(uart->cfg));
    furi_hal_serial_dma_rx_start(uart->serial_handle, on_rx_cb, uart, false);
}
static void serial_deinit(Uart* uart) {
    furi_assert(uart->serial_handle);

    furi_hal_serial_dma_rx_stop(uart->serial_handle);
    furi_hal_serial_deinit(uart->serial_handle);
    furi_hal_serial_control_release(uart->serial_handle);
    uart->serial_handle = NULL;
}
void timerDone(void* context) {
    App* app = context;
    app->modbus->slave = false;
}
static int32_t uart_worker(void* context) {
    App* app = context;
    while(1) {
        furi_hal_gpio_write(&gpio_ext_pc0, false);
        furi_hal_gpio_write(&gpio_ext_pc1, false);
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);
        if(events & WorkerEvtStop) break;
        if(events & WorkerEvtCfgChange) {
            serial_deinit(app->uart);
            serial_init(app->uart, UART_CH);
        }
        if(events & WorkerEvtRxDone) {
            size_t len =
                furi_stream_buffer_receive(app->uart->rxStream, app->uart->rxBuff, RX_BUF_SIZE, 0);
            if(len > 0) {
                handle_rx_data_cb(app->uart->rxBuff, len, app);
            }
        }
        //TODO: Serial Write & enable DE/RE pins
    }

    furi_stream_buffer_free(app->uart->rxBuff);

    return 0;
}

//////////////////////////   Main Scene  //////////////////////////
void mainOptionsCB(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case Settings_Option:
        scene_manager_next_scene(app->sceneManager, Settings_Scene);
        break;
    case Sniffer_Option:
        scene_manager_set_scene_state(app->sceneManager, ConsoleOutput_Scene, Sniffer_Option);
        scene_manager_next_scene(app->sceneManager, ConsoleOutput_Scene);
        break;
    case Read_LOG_Option:
        scene_manager_set_scene_state(app->sceneManager, ConsoleOutput_Scene, Read_LOG_Option);
        scene_manager_next_scene(app->sceneManager, ConsoleOutput_Scene);
        break;
    default:
        break;
    }
}
void Main_Scene_OnEnter(void* context) {
    App* app = context;
    submenu_reset(app->subMenu);
    submenu_set_header(app->subMenu, "Main");
    submenu_add_item(app->subMenu, "Settings", Settings_Option, mainOptionsCB, app);
    submenu_add_item(app->subMenu, "Sniffer", Sniffer_Option, mainOptionsCB, app);
    submenu_add_item(app->subMenu, "Read LOG", Read_LOG_Option, mainOptionsCB, app);
    view_dispatcher_switch_to_view(app->viewDispatcher, Submenu_View);
}
bool Main_Scene_OnEvent(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}
void Main_Scene_OnExit(void* context) {
    App* app = context;
    submenu_reset(app->subMenu);
}

//////////////////////////   Settings Scene  //////////////////////////
void itemChangedCB(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint8_t selectedIndex = variable_item_list_get_selected_item_index(app->varList);
    switch(selectedIndex) {
    case BaudRate_Option:
        variable_item_set_current_value_text(item, baudrateValues[index]);
        app->uart->cfg->baudrate = index;
        break;
    case DataWidth_Option:
        variable_item_set_current_value_text(item, dataWidthValues[index]);
        app->uart->cfg->dataWidth = index;
        break;
    case StopBits_Option:
        variable_item_set_current_value_text(item, stopBitsValues[index]);
        app->uart->cfg->stopBits = index;
        break;
    case Parity_Option:
        variable_item_set_current_value_text(item, parityValues[index]);
        app->uart->cfg->timeout = index;
        break;
    case TimeOut_Option:
        app->uart->cfg->timeout = index;
        variable_item_set_current_value_index(item, index);
        furi_string_printf(app->modbus->timeout, "%d", index * TIMEOUT_SCALER);
        variable_item_set_current_value_text(item, furi_string_get_cstr(app->modbus->timeout));
        break;
    case OutputFormat_Option:
        variable_item_set_current_value_text(item, outputFormatValues[index]);
        app->uart->cfg->hexOutput = index;
        break;
    case SaveLOG_Option:
        variable_item_set_current_value_text(item, saveLOGValues[index]);
        app->uart->cfg->saveLOG = index;
        break;
    default:
        break;
    }
}
//TODO: itemEneterCB(void*,uint32_t)
void CFG_Scene_OnEnter(void* context) {
    App* app = context;
    VariableItem* item;
    //TODO: variable_item_list_set_enter_callback(app->varList,itemEnterCB,app);
    item = variable_item_list_add(app->varList, "Buadrate", BR_VALUES, itemChangedCB, app);
    variable_item_set_current_value_index(item, app->uart->cfg->baudrate);
    variable_item_set_current_value_text(item, baudrateValues[app->uart->cfg->baudrate]);
    item = variable_item_list_add(app->varList, "Data size", DATAWIDTH_VALUES, itemChangedCB, app);
    variable_item_set_current_value_index(item, app->uart->cfg->dataWidth);
    variable_item_set_current_value_text(item, dataWidthValues[app->uart->cfg->dataWidth]);
    item = variable_item_list_add(app->varList, "Stop bits", STOPBITS_VALUES, itemChangedCB, app);
    variable_item_set_current_value_index(item, app->uart->cfg->stopBits);
    variable_item_set_current_value_text(item, stopBitsValues[app->uart->cfg->stopBits]);
    item = variable_item_list_add(app->varList, "Parity", PARITY_VALUES, itemChangedCB, app);
    variable_item_set_current_value_index(item, app->uart->cfg->parity);
    variable_item_set_current_value_text(item, parityValues[app->uart->cfg->parity]);
    item = variable_item_list_add(app->varList, "TimeOut(ms)", TIMEOUT_VALUES, itemChangedCB, app);
    variable_item_set_current_value_index(item, app->uart->cfg->timeout);
    furi_string_printf(app->modbus->timeout, "%d", app->uart->cfg->timeout * TIMEOUT_SCALER);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->modbus->timeout));
    item = variable_item_list_add(
        app->varList, "Output Format", DIGITALFORMAT_VALUES, itemChangedCB, app);
    variable_item_set_current_value_index(item, app->uart->cfg->hexOutput ? 1 : 0);
    variable_item_set_current_value_text(
        item, outputFormatValues[app->uart->cfg->hexOutput ? 1 : 0]);
    item = variable_item_list_add(app->varList, "Save LOG?", SAVE_LOG_VALUES, itemChangedCB, app);
    variable_item_set_current_value_index(item, app->uart->cfg->saveLOG ? 1 : 0);
    variable_item_set_current_value_text(item, saveLOGValues[app->uart->cfg->saveLOG ? 1 : 0]);

    variable_item_list_set_selected_item(app->varList, 0);
    view_dispatcher_switch_to_view(app->viewDispatcher, VarList_View);
}
bool CFG_Scene_OnEvent(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}
void CFG_Scene_OnExit(void* context) {
    App* app = context;
    if(app->uart->cfg->saveLOG) {
        strcpy(
            app->logFilePath, sequential_file_resolve_path(app->storage, PATHLOGS, "Log", "log"));
        if(app->logFilePath != NULL) {
            if(storage_file_open(app->LOGfile, app->logFilePath, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
                furi_string_reset(app->text);
                app->LOGfileReady = true;
            } else {
                dialog_message_show_storage_error(app->dialogs, "Cannot open log file");
            }
        } else {
            dialog_message_show_storage_error(app->dialogs, "Cannot resolve log path");
        }
    }
    uart_set_config(app);
    variable_item_list_reset(app->varList);
}

//////////////////////////   Sniffer Scene  //////////////////////////
void Sniffer_Scene_OnEnter(void* context) {
    App* app = context;
    if(scene_manager_get_scene_state(app->sceneManager, ConsoleOutput_Scene) == Sniffer_Option) {
        text_box_reset(app->textBox);
        text_box_set_font(app->textBox, TextBoxFontText);
        text_box_set_focus(app->textBox, TextBoxFocusEnd);
        furi_string_cat_printf(
            app->text, "Baudrate: %s", baudrateValues[app->uart->cfg->baudrate]);
        furi_string_cat_printf(
            app->text, "\nData Width: %s", dataWidthValues[app->uart->cfg->dataWidth]);
        furi_string_cat_printf(
            app->text, "\nStop bits: %s", stopBitsValues[app->uart->cfg->stopBits]);
        furi_string_cat_printf(app->text, "\nParity: %s", parityValues[app->uart->cfg->parity]);
    } else if(
        scene_manager_get_scene_state(app->sceneManager, ConsoleOutput_Scene) == Read_LOG_Option) {
        OpenLogFile(app);
    }
    text_box_set_text(app->textBox, furi_string_get_cstr(app->text));
    view_dispatcher_switch_to_view(app->viewDispatcher, TextBox_View);
}
bool Sniffer_Scene_OnEvent(void* context, SceneManagerEvent event) {
    App* app = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        text_box_set_text(app->textBox, furi_string_get_cstr(app->text));
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        consumed = true;
    }
    return consumed;
}
void Sniffer_Scene_OnExit(void* context) {
    App* app = context;
    text_box_reset(app->textBox);
    furi_string_reset(app->text);
}

//////////////////////////   Scene Handlers  //////////////////////////
void (*const OnEnterHandlers[])(
    void*) = {Main_Scene_OnEnter, CFG_Scene_OnEnter, Sniffer_Scene_OnEnter};
bool (*const OnEventHandlers[])(void*, SceneManagerEvent) = {
    Main_Scene_OnEvent,
    CFG_Scene_OnEvent,
    Sniffer_Scene_OnEvent};
void (*const OnExitHandlers[])(void*) = {Main_Scene_OnExit, CFG_Scene_OnExit, Sniffer_Scene_OnExit};
static const SceneManagerHandlers SceneHandlers = {
    .on_enter_handlers = OnEnterHandlers,
    .on_event_handlers = OnEventHandlers,
    .on_exit_handlers = OnExitHandlers,
    .scene_num = Scene_Num};

//////////////////////////   ViewDispatcher Callbacks  //////////////////////////
static bool CustomEventCB(void* context, uint32_t event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->sceneManager, event);
}
static bool BackEventCB(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->sceneManager);
}
static void ThickEventCB(void* context) {
    furi_assert(context);
    App* app = context;
    scene_manager_handle_tick_event(app->sceneManager);
}
//////////////////////////   Allocating  //////////////////////////
Config* Config_Alloc() {
    Config* config = malloc(sizeof(Config));
    config->baudrate = 3;
    config->dataWidth = 1;
    config->stopBits = 1;
    config->parity = 0;
    config->timeout = 10;
    config->hexOutput = false;
    config->saveLOG = false;
    return config;
}

Uart* Uart_Alloc(void* context) {
    App* app = context;
    Uart* uart = malloc(sizeof(Uart));
    uart->cfg = Config_Alloc();
    uart->rxStream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);
    uart->rxThread = furi_thread_alloc_ex("RxThread", 1024, uart_worker, app);

    furi_thread_start(uart->rxThread);
    serial_init(uart, UART_CH);

    return uart;
}

Modbus* Modbus_alloc(void* context) {
    App* app = context;
    UNUSED(app);
    Modbus* modbus = malloc(sizeof(Modbus));
    modbus->slave = false;
    modbus->timeout = furi_string_alloc();
    return modbus;
}
static App* modbus_app_alloc() {
    App* app = malloc(sizeof(App));
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->storage = furi_record_open(RECORD_STORAGE);
    app->LOGfile = storage_file_alloc(app->storage);
    app->sceneManager = scene_manager_alloc(&SceneHandlers, app);
    app->viewDispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->viewDispatcher);
    view_dispatcher_set_event_callback_context(app->viewDispatcher, app);
    view_dispatcher_set_custom_event_callback(app->viewDispatcher, CustomEventCB);
    view_dispatcher_set_navigation_event_callback(app->viewDispatcher, BackEventCB);
    view_dispatcher_set_tick_event_callback(app->viewDispatcher, ThickEventCB, 100);
    app->subMenu = submenu_alloc();
    view_dispatcher_add_view(app->viewDispatcher, Submenu_View, submenu_get_view(app->subMenu));
    app->varList = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->viewDispatcher, VarList_View, variable_item_list_get_view(app->varList));
    app->textBox = text_box_alloc();
    view_dispatcher_add_view(app->viewDispatcher, TextBox_View, text_box_get_view(app->textBox));
    app->text = furi_string_alloc();
    furi_string_reserve(app->text, TEXT_BOX_LEN);
    makePaths(app);

    app->timer = furi_timer_alloc(timerDone, FuriTimerTypeOnce, app);
    furi_timer_set_thread_priority(FuriTimerThreadPriorityElevated);
    app->modbus = Modbus_alloc(app);
    app->uart = Uart_Alloc(app);
    return app;
}

void uartFree(void* context) {
    App* app = context;
    furi_thread_flags_set(furi_thread_get_id(app->uart->rxThread), WorkerEvtStop);
    furi_thread_join(app->uart->rxThread);
    furi_thread_free(app->uart->rxThread);
    app->LOGfileReady = false;
    if(app->LOGfile && storage_file_is_open(app->LOGfile)) {
        storage_file_close(app->LOGfile);
    }

    serial_deinit(app->uart);
    free(app->uart->cfg);
    free(app->uart);
    free(app->modbus);
    furi_timer_free(app->timer);
}
void ModbusFree(void* context) {
    Modbus* modbus = context;
    furi_string_free(modbus->timeout);
}
void modbus_app_free(App* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->viewDispatcher, TextBox_View);
    text_box_free(app->textBox);
    view_dispatcher_remove_view(app->viewDispatcher, VarList_View);
    variable_item_list_free(app->varList);
    view_dispatcher_remove_view(app->viewDispatcher, Submenu_View);
    submenu_free(app->subMenu);
    view_dispatcher_free(app->viewDispatcher);
    scene_manager_free(app->sceneManager);
    ModbusFree(app);
    uartFree(app);
    storage_file_free(app->LOGfile);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    free(app);
}

//////////////////////////   Entry Point   //////////////////////////
int32_t test_app(void* p) {
    UNUSED(p);
    furi_hal_gpio_init_simple(&gpio_ext_pc0, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeOutputPushPull);
    App* app = modbus_app_alloc();
    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->viewDispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->sceneManager, Main_Scene);
    view_dispatcher_run(app->viewDispatcher);
    furi_record_close(RECORD_GUI);
    modbus_app_free(app);
    furi_hal_gpio_init_simple(&gpio_ext_pc0, GpioModeAnalog);
    furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeAnalog);
    return 0;
}