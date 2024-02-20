#include <furi.h>
#include <furi_hal.h>

#include "usbif.h"
#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/menu.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_box.h>
#include <dialogs/dialogs.h>

#include <storage/storage.h>
#include <stream/stream.h>
#include <stream/buffered_file_stream.h>
#include <toolbox/stream/file_stream.h>
#include "constants.h"
#include <assets_icons.h>
#include "hidtransfer_icons.h"

#define HEX_VIEWER_APP_PATH_FOLDER "/any"
#define HEX_VIEWER_APP_EXTENSION "*"

#define HEX_VIEWER_BYTES_PER_LINE 4u
#define HEX_VIEWER_LINES_ON_SCREEN 4u
#define HEX_VIEWER_BUF_SIZE (HEX_VIEWER_LINES_ON_SCREEN * HEX_VIEWER_BYTES_PER_LINE)

#define VIEW_DISPATCHER_MENU 0
#define VIEW_DISPATCHER_SEND 1
#define VIEW_DISPATCHER_SEND_SINGLE_THREADED 3
#define VIEW_DISPATCHER_RECEIVE 2
#define VIEW_DISPATCHER_POPUP 3
#define VIEW_DISPATCHER_DEBUG_SEND 99
#define VIEW_DISPATCHER_DEBUG_RECEIVE 98

typedef struct {
    uint8_t file_bytes[HEX_VIEWER_LINES_ON_SCREEN][HEX_VIEWER_BYTES_PER_LINE];
    uint32_t file_offset;
    uint32_t file_read_bytes;
    uint32_t file_size;
    Stream* stream;
    bool mode; // Print address or content
} DataTransferAppModel;

typedef struct {
    DataTransferAppModel* model;

    ViewDispatcher* view_dispatcher;
    Gui* gui;
    Storage* storage;
} DataTransferApp;

typedef enum MessageType {
    MessageMetadata = 0,
    MessageFullPayload = 1,
    MessagePartPayload = 2
} MessageType;

// 3 byte
typedef struct {
    uint32_t counter; // 22 bit LSB
    MessageType messageType; // 2 bit
} MessageHeader;

// 5 byte + len(fileName)
typedef struct {
    MessageHeader header;
    uint32_t fileSize;
    const char* fileName;
} FileMetadataMessage;

// max. 64 byte, see payloadLength
typedef struct {
    // 3 byte
    MessageHeader header;
    // 61 byte
    uint8_t* payload;
} FullPayloadMessage;

// max. 64 byte, see payloadLength
typedef struct {
    // 3 byte
    MessageHeader header;
    uint8_t payloadLength;
    // 61 byte
    uint8_t* payload;
} PartPayloadMessage;

TextBox* textBoxReceive;
Popup* popup;
DataTransferApp* app;
void openMenu(void* bla);

static void* parseMessage(MessageType* outMsg, void* msgBuffer) {
    uint32_t header = 0;
    memcpy(&header, msgBuffer, 3);
    FURI_LOG_D(TAG, "Parse message, header: %lu", header);

    MessageType msgType = header & 3;
    uint32_t counter = header >> 2;

    *outMsg = msgType;

    if(msgType == MessageMetadata) {
        FURI_LOG_D(TAG, "Parse Metadata message");
        furi_check(counter == 0);
        uint32_t fileSize;

        int strl = strlen(msgBuffer + 7);
        char* fileName = malloc(strl + 1);

        memcpy(&fileSize, msgBuffer + 3, sizeof(fileSize));
        strncpy(fileName, msgBuffer + 7, strl);

        FileMetadataMessage* msg = malloc(sizeof(FileMetadataMessage));
        memset(msg, 0, sizeof(FileMetadataMessage));

        *msg = (FileMetadataMessage){
            .header = {.counter = counter, .messageType = MessageMetadata},
            .fileName = fileName,
            .fileSize = fileSize};
        return msg;
    }

    FURI_LOG_E(TAG, "Tried to parse unknown msg! %d", msgType);
    furi_check(false);
}

static DataTransferApp* dataTransferApp_alloc() {
    FURI_LOG_D(TAG, "alloc");
    DataTransferApp* instance = malloc(sizeof(DataTransferApp));

    instance->model = malloc(sizeof(DataTransferAppModel));
    memset(instance->model, 0x0, sizeof(DataTransferAppModel));

    instance->view_dispatcher = view_dispatcher_alloc();

    instance->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);

    instance->storage = furi_record_open(RECORD_STORAGE);

    return instance;
}

static void dataTransferApp_free(DataTransferApp* instance) {
    furi_record_close(RECORD_STORAGE);

    view_dispatcher_free(instance->view_dispatcher);
    furi_record_close(RECORD_GUI);

    if(instance->model->stream) buffered_file_stream_close(instance->model->stream);

    free(instance->model);
    free(instance);
}

void sendMessage(uint8_t* msg) {
    MessageHeader* header = (MessageHeader*)msg;

    uint32_t c = header->counter;
    c = c << 2 | header->messageType;

    void* sendBuf = malloc(64);
    memset(sendBuf, 0x0, 64);
    memcpy(sendBuf, &c, 3);

    if(header->messageType == MessageMetadata) {
        FileMetadataMessage* m = (FileMetadataMessage*)msg;
        memcpy(sendBuf + 3, &(m->fileSize), 4);
        strncpy(sendBuf + 7, m->fileName, 64 - 7);
        //memcpy(sendBuf + 7, m->fileName, strlen(m->fileName));
    } else if(header->messageType == MessageFullPayload) {
        FullPayloadMessage* m = (FullPayloadMessage*)msg;
        memcpy(sendBuf + 3, m->payload, 61);
    } else if(header->messageType == MessagePartPayload) {
        PartPayloadMessage* m = (PartPayloadMessage*)msg;
        memcpy(sendBuf + 3, &(m->payloadLength), 1);
        memcpy(sendBuf + 4, m->payload, m->payloadLength);
    }

    sendBulkData(sendBuf, 64);
}

void sendHeader(uint32_t fileSize, const char* fileName) {
    FileMetadataMessage md = {
        .header = {.counter = 0, .messageType = MessageMetadata},
        .fileSize = fileSize,
        .fileName = fileName};

    uint8_t* bytePtr = (uint8_t*)&md;
    sendMessage(bytePtr);
}

static void dispatch_view(void* contextd, uint32_t index) {
    DataTransferApp* context = (DataTransferApp*)contextd;

    if(index == VIEW_DISPATCHER_SEND || index == VIEW_DISPATCHER_SEND_SINGLE_THREADED) {
        initializeSendingData(index == VIEW_DISPATCHER_SEND ? NUM_OF_INTERFACES : 1);

        FuriString* browser_path;
        browser_path = furi_string_alloc();

        FuriString* selected_path;
        selected_path = furi_string_alloc();

        furi_string_set(browser_path, HEX_VIEWER_APP_PATH_FOLDER);

        DialogsFileBrowserOptions browser_options;
        dialog_file_browser_set_basic_options(&browser_options, HEX_VIEWER_APP_EXTENSION, NULL);
        browser_options.hide_ext = false;
        browser_options.base_path = furi_string_get_cstr(browser_path);

        DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
        bool res =
            dialog_file_browser_show(dialogs, selected_path, browser_path, &browser_options);

        furi_record_close(RECORD_DIALOGS);
        if(!res) {
            FURI_LOG_I(TAG, "No file selected");
            furi_string_free(browser_path);
            furi_string_free(selected_path);
            view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_DISPATCHER_MENU);
            return;
        }

        view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_DISPATCHER_SEND);

        size_t idx = furi_string_search_rchar(selected_path, '/');
        FuriString* path_copy = furi_string_alloc();
        furi_string_set_n(
            path_copy, selected_path, idx + 1, furi_string_size(selected_path) - idx - 1);

        const char* path = furi_string_get_cstr(selected_path);

        Stream* fs = buffered_file_stream_alloc(context->storage);
        buffered_file_stream_open(fs, path, FSAM_READ, FSOM_OPEN_EXISTING);

        size_t file_size = stream_size(fs);

        sendHeader(file_size, furi_string_get_cstr(path_copy));
        furi_string_free(path_copy);

        uint32_t sent = 0;
        uint8_t data[64];
        memset(data, 0, 64);

        uint32_t msgCounter = 1;

        while(sent < file_size) {
            memset(data, 0, 61);
            uint8_t to_read = 61;
            if(file_size - sent < 61) {
                to_read = file_size - sent;
            }
            sent += to_read;
            stream_read(fs, data, to_read);

            if(to_read == 61) {
                FullPayloadMessage msg = {
                    .header = {.counter = msgCounter, .messageType = MessageFullPayload},
                    .payload = data,
                };
                sendMessage((uint8_t*)&msg);
            } else {
                PartPayloadMessage msg = {
                    .header = {.counter = msgCounter, .messageType = MessagePartPayload},
                    .payloadLength = to_read,
                    .payload = data};
                sendMessage((uint8_t*)&msg);
            }

            msgCounter++;

            //furi_hal_hid_u2f_send_response(data, 64);
        }

        FURI_LOG_D(TAG, "Finished sending packet");
        stopSendingData();

        buffered_file_stream_close(fs);
        free(fs);
        furi_string_free(browser_path);
        furi_string_free(selected_path);
        view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_DISPATCHER_MENU);
    } else if(index == VIEW_DISPATCHER_RECEIVE) {
        view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_DISPATCHER_RECEIVE);
        FuriString* textBoxText = furi_string_alloc_printf("Waiting for file...");
        text_box_set_text(textBoxReceive, furi_string_get_cstr(textBoxText));

        FuriMessageQueue* queue = initializeReceivingData();

        ThreadMessage threadMsg;
        FileMetadataMessage* metadataMsg;
        furi_check(furi_message_queue_get(queue, &threadMsg, FuriWaitForever) == FuriStatusOk);
        MessageType msgtype;

        void* parsedMsg = parseMessage(&msgtype, threadMsg.dataPointer);
        FURI_LOG_D(TAG, "received %d", msgtype);
        //furi_check(msgtype == MessageMetadata);
        metadataMsg = (FileMetadataMessage*)parsedMsg;

        furi_string_printf(textBoxText, "Receiving %s...", metadataMsg->fileName);
        text_box_set_text(textBoxReceive, furi_string_get_cstr(textBoxText));

        Stream* fs = buffered_file_stream_alloc(context->storage);
        storage_common_mkdir(context->storage, "/any/HIDTransfer");

        FuriString* filePath =
            furi_string_alloc_printf("/any/HIDTransfer/%s.raw", metadataMsg->fileName);
        if(!buffered_file_stream_open(
               fs, furi_string_get_cstr(filePath), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            FURI_LOG_D(TAG, "Could not open filestream");
            furi_string_printf(textBoxText, "Could not open filestream");
            text_box_set_text(textBoxReceive, furi_string_get_cstr(textBoxText));
            furi_delay_ms(1500);
            furi_crash("FSEr");
        }

        // write metadata message to file
        stream_write(fs, threadMsg.dataPointer, 64);

        uint32_t expectedNumMsgs = (uint32_t)ceil(metadataMsg->fileSize / 61.0);
        uint32_t increamentStep = (uint32_t)floor(0.1 * expectedNumMsgs);

        int finished = 0;
        uint32_t numMsgs = 0;

        while(true) {
            ThreadMessage msg;
            furi_check(furi_message_queue_get(queue, &msg, FuriWaitForever) == FuriStatusOk);
            if(msg.dataPointer == NULL) {
                finished += 1;
                if(finished == NUM_OF_INTERFACES) {
                    break;
                }
                continue;
            }

            numMsgs += 1;
            if(numMsgs % increamentStep == 0) {
                int percent = (int)ceil(numMsgs / (double)expectedNumMsgs * 100);
                furi_string_printf(
                    textBoxText, "Receiving %s...\n%d%%", metadataMsg->fileName, percent);
                text_box_set_text(textBoxReceive, furi_string_get_cstr(textBoxText));
            }

            //FURI_LOG_D(TAG, "after Queue");
            stream_write(fs, msg.dataPointer, 64);
            free(msg.dataPointer);
        }

        buffered_file_stream_close(fs);
        free(fs);
        free((void*)metadataMsg->fileName);
        free(metadataMsg);
        furi_string_free(filePath);

        int missingMsgs = expectedNumMsgs - numMsgs;

        if(missingMsgs != 0) {
            furi_string_cat_printf(
                textBoxText, "\nCAUTION: %d messages are missing.", missingMsgs);
            FuriString* txtMsg = furi_string_alloc_printf("%d messages are missing.", missingMsgs);
            popup_set_icon(popup, 4, 19, &I_Warning_30x23);
            popup_set_header(popup, "CAUTION", 53, 19, AlignLeft, AlignCenter);
            popup_set_text(popup, furi_string_get_cstr(txtMsg), 39, 28, AlignLeft, AlignTop);
            popup_set_callback(popup, &openMenu);
            view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_DISPATCHER_POPUP);
        } else {
            furi_string_cat_printf(textBoxText, "\nFinished receiving!");
            text_box_set_text(textBoxReceive, furi_string_get_cstr(textBoxText));
            furi_delay_ms(5000);
            view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_DISPATCHER_MENU);
        }

        furi_string_free(textBoxText);
    } else if(index == VIEW_DISPATCHER_DEBUG_SEND) {
        uint8_t buf[64];
        for(int i = 0; i < 64; i++) {
            buf[i] = i + 1;
        }
        sendViaEP(buf, 0);
    } else if(index == VIEW_DISPATCHER_DEBUG_RECEIVE) {
        uint8_t buf[64];
        receiveFromEP(buf, 0);
        FURI_LOG_D(TAG, "01: %d, last: %d", buf[0], buf[63]);
    }
}

static bool eventCallback(void* context) {
    UNUSED(context);
    return false;
}

void openMenu(void* bla) {
    UNUSED(bla);
    view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_DISPATCHER_MENU);
}

bool inputCallback(InputEvent* event, void* context) {
    UNUSED(context);
    UNUSED(event);
    FURI_LOG_D(TAG, "Back button pressend on sending view");
    return true;
}

int32_t hidtransfer_app() {
    furi_log_set_level(FuriLogLevelDebug);
    FURI_LOG_D(TAG, "APP STARTED");

    FuriHalUsbInterface* mode = furi_hal_usb_get_config();
    furi_hal_usb_set_config(getUsbHidBulk(), NULL);

    app = dataTransferApp_alloc();
    Menu* mainMenu = menu_alloc();
    menu_add_item(
        mainMenu,
        "Send Client",
        &I_right_14,
        VIEW_DISPATCHER_SEND_SINGLE_THREADED,
        dispatch_view,
        app);
    menu_add_item(mainMenu, "Send File", &I_right_14, VIEW_DISPATCHER_SEND, dispatch_view, app);
    menu_add_item(
        mainMenu, "Receive File", &I_left_14, VIEW_DISPATCHER_RECEIVE, dispatch_view, app);
    //menu_add_item(mainMenu, "Debug Send", &I_Pin_arrow_right_9x7, VIEW_DISPATCHER_DEBUG_SEND, dispatch_view, app);
    //menu_add_item(mainMenu, "Debug Receive", &I_Pin_arrow_left_9x7, VIEW_DISPATCHER_DEBUG_RECEIVE, dispatch_view, app);

    // Sending View
    TextBox* textBoxSend = text_box_alloc();
    text_box_set_text(textBoxSend, "Sending file...");
    text_box_set_font(textBoxSend, TextBoxFontText);
    View* textBoxView = text_box_get_view(textBoxSend);
    view_set_input_callback(textBoxView, inputCallback);

    // Receive View
    textBoxReceive = text_box_alloc();
    text_box_set_text(textBoxReceive, "Receiveing file...");
    text_box_set_font(textBoxReceive, TextBoxFontText);
    View* textBoxRecView = text_box_get_view(textBoxReceive);
    view_set_input_callback(textBoxRecView, inputCallback);

    // Popup
    popup = popup_alloc();
    popup_disable_timeout(popup);

    view_dispatcher_add_view(app->view_dispatcher, VIEW_DISPATCHER_MENU, menu_get_view(mainMenu));
    view_dispatcher_add_view(app->view_dispatcher, VIEW_DISPATCHER_SEND, textBoxView);
    view_dispatcher_add_view(app->view_dispatcher, VIEW_DISPATCHER_RECEIVE, textBoxRecView);
    view_dispatcher_add_view(app->view_dispatcher, VIEW_DISPATCHER_POPUP, popup_get_view(popup));
    view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_DISPATCHER_MENU);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, eventCallback);

    view_dispatcher_run(app->view_dispatcher);

    dataTransferApp_free(app);

    furi_hal_usb_set_config(mode, NULL);
    menu_free(mainMenu);
    text_box_free(textBoxSend);
    text_box_free(textBoxReceive);
    popup_free(popup);

    return 0;
}
