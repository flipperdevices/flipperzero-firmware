#include "../nfc_maker.h"

size_t nfc_maker_scene_save_generate_populate_ndef_buffer(NfcMaker* app) {
    // NDEF Docs: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/protocols/nfc/index.html#nfc-data-exchange-format-ndef
    uint8_t tnf = 0x00;
    const char* type = "";
    uint8_t* payload = NULL;
    uint8_t* payload_it = NULL;
    size_t payload_len = 0;

    size_t data_len = 0;
    switch(scene_manager_get_scene_state(app->scene_manager, NfcMakerSceneStart)) {
    case NfcMakerSceneBluetooth: {
        tnf = 0x02; // Media-type [RFC 2046]
        type = "application/vnd.bluetooth.ep.oob";

        data_len = MAC_INPUT_LEN;
        payload_len = data_len + 2;
        payload = payload_it = malloc(payload_len);

        *payload_it++ = 0x08;
        *payload_it++ = 0x00;
        memcpy(payload_it, app->mac_buf, data_len);
        payload_it += data_len;
        break;
    }
    case NfcMakerSceneContact: {
        tnf = 0x02; // Media-type [RFC 2046]
        type = "text/vcard";

        FuriString* vcard = furi_string_alloc_set("BEGIN:VCARD\r\nVERSION:3.0\r\n");
        furi_string_cat_printf(
            vcard, "PRODID:-//Flipper Zero//%s//EN\r\n", version_get_version(NULL));
        furi_string_cat_printf(vcard, "N:%s;%s;;;\r\n", app->small_buf2, app->small_buf1);
        furi_string_cat_printf(
            vcard,
            "FN:%s%s%s\r\n",
            app->small_buf1,
            newstrnlen(app->small_buf2, SMALL_INPUT_LEN) ? " " : "",
            app->small_buf2);
        if(newstrnlen(app->mail_buf, MAIL_INPUT_LEN)) {
            furi_string_cat_printf(vcard, "EMAIL:%s\r\n", app->mail_buf);
        }
        if(newstrnlen(app->phone_buf, PHONE_INPUT_LEN)) {
            furi_string_cat_printf(vcard, "TEL:%s\r\n", app->phone_buf);
        }
        if(newstrnlen(app->big_buf, BIG_INPUT_LEN)) {
            furi_string_cat_printf(vcard, "URL:%s\r\n", app->big_buf);
        }
        furi_string_cat_printf(vcard, "END:VCARD\r\n");

        payload_len = furi_string_size(vcard);
        payload = payload_it = malloc(payload_len);
        memcpy(payload_it, furi_string_get_cstr(vcard), payload_len);
        payload_it += payload_len;
        furi_string_free(vcard);
        break;
    }
    case NfcMakerSceneHttps: {
        tnf = 0x01; // NFC Forum well-known type [NFC RTD]
        type = "U";

        data_len = newstrnlen(app->big_buf, BIG_INPUT_LEN);
        payload_len = data_len + 1;
        payload = payload_it = malloc(payload_len);

        *payload_it++ = 0x04; // Prepend "https://"
        memcpy(payload_it, app->big_buf, data_len);
        payload_it += data_len;
        break;
    }
    case NfcMakerSceneMail: {
        tnf = 0x01; // NFC Forum well-known type [NFC RTD]
        type = "U";

        data_len = newstrnlen(app->mail_buf, MAIL_INPUT_LEN);
        payload_len = data_len + 1;
        payload = payload_it = malloc(payload_len);

        *payload_it++ = 0x06; // Prepend "mailto:"
        memcpy(payload_it, app->mail_buf, data_len);
        payload_it += data_len;
        break;
    }
    case NfcMakerScenePhone: {
        tnf = 0x01; // NFC Forum well-known type [NFC RTD]
        type = "U";

        data_len = newstrnlen(app->phone_buf, PHONE_INPUT_LEN);
        payload_len = data_len + 1;
        payload = payload_it = malloc(payload_len);

        *payload_it++ = 0x05; // Prepend "tel:"
        memcpy(payload_it, app->phone_buf, data_len);
        payload_it += data_len;
        break;
    }
    case NfcMakerSceneText: {
        tnf = 0x01; // NFC Forum well-known type [NFC RTD]
        type = "T";

        data_len = newstrnlen(app->big_buf, BIG_INPUT_LEN);
        payload_len = data_len + 3;
        payload = payload_it = malloc(payload_len);

        *payload_it++ = 0x02;
        *payload_it++ = 'e';
        *payload_it++ = 'n';
        memcpy(payload_it, app->big_buf, data_len);
        payload_it += data_len;
        break;
    }
    case NfcMakerSceneUrl: {
        tnf = 0x01; // NFC Forum well-known type [NFC RTD]
        type = "U";

        data_len = newstrnlen(app->big_buf, BIG_INPUT_LEN);
        payload_len = data_len + 1;
        payload = payload_it = malloc(payload_len);

        *payload_it++ = 0x00; // No prepend
        memcpy(payload_it, app->big_buf, data_len);
        payload_it += data_len;
        break;
    }
    case NfcMakerSceneWifi: {
        tnf = 0x02; // Media-type [RFC 2046]
        type = "application/vnd.wfa.wsc";

        // https://android.googlesource.com/platform/packages/apps/Nfc/+/refs/heads/main/src/com/android/nfc/NfcWifiProtectedSetup.java
        // https://github.com/bparmentier/WiFiKeyShare/blob/master/app/src/main/java/be/brunoparmentier/wifikeyshare/utils/NfcUtils.java
        uint8_t ssid_len = newstrnlen(app->small_buf1, SMALL_INPUT_LEN);
        uint8_t pass_len = newstrnlen(app->small_buf2, SMALL_INPUT_LEN);
        uint8_t data_len = ssid_len + pass_len;
        payload_len = data_len + 39;
        payload = payload_it = malloc(payload_len);

        // CREDENTIAL_FIELD_ID
        *payload_it++ = 0x10;
        *payload_it++ = 0x0E;
        // CREDENTIAL_FIELD_LEN
        *payload_it++ = 0x00;
        *payload_it++ = data_len + 35;
        // CREDENTIAL_FIELD (contains all subsequent fields)

        // NETWORK_INDEX_FIELD_ID
        *payload_it++ = 0x10;
        *payload_it++ = 0x26;
        // NETWORK_INDEX_FIELD_LEN
        *payload_it++ = 0x00;
        *payload_it++ = 0x01;
        // NETWORK_INDEX_FIELD
        *payload_it++ = 0x01;

        // SSID_FIELD_ID
        *payload_it++ = 0x10;
        *payload_it++ = 0x45;
        // SSID_FIELD_LEN
        *payload_it++ = ssid_len >> 8 & 0xFF;
        *payload_it++ = ssid_len & 0xFF;
        // SSID_FIELD
        memcpy(payload_it, app->small_buf1, ssid_len);
        payload_it += ssid_len;

        // AUTH_TYPE_FIELD_ID
        *payload_it++ = 0x10;
        *payload_it++ = 0x03;
        // AUTH_TYPE_FIELD_LEN
        *payload_it++ = 0x00;
        *payload_it++ = 0x02;
        // AUTH_TYPE_FIELD
        *payload_it++ = 0x00;
        *payload_it++ = scene_manager_get_scene_state(app->scene_manager, NfcMakerSceneWifiAuth);

        // ENC_TYPE_FIELD_ID
        *payload_it++ = 0x10;
        *payload_it++ = 0x0F;
        // ENC_TYPE_FIELD_LEN
        *payload_it++ = 0x00;
        *payload_it++ = 0x02;
        // ENC_TYPE_FIELD
        *payload_it++ = 0x00;
        *payload_it++ = scene_manager_get_scene_state(app->scene_manager, NfcMakerSceneWifiEncr);

        // NETWORK_KEY_FIELD_ID
        *payload_it++ = 0x10;
        *payload_it++ = 0x27;
        // NETWORK_KEY_FIELD_LEN
        *payload_it++ = pass_len >> 8 & 0xFF;
        *payload_it++ = pass_len & 0xFF;
        // NETWORK_KEY_FIELD
        memcpy(payload_it, app->small_buf2, pass_len);
        payload_it += pass_len;

        // MAC_ADDRESS_FIELD_ID
        *payload_it++ = 0x10;
        *payload_it++ = 0x20;
        // MAC_ADDRESS_FIELD_LEN
        *payload_it++ = 0x00;
        *payload_it++ = 0x06;
        // MAC_ADDRESS_FIELD
        *payload_it++ = 0xFF;
        *payload_it++ = 0xFF;
        *payload_it++ = 0xFF;
        *payload_it++ = 0xFF;
        *payload_it++ = 0xFF;
        *payload_it++ = 0xFF;

        break;
    }
    default:
        break;
    }

    // Setup header
    uint8_t flags = 0;
    flags |= 1 << 7; // MB (Message Begin)
    flags |= 1 << 6; // ME (Message End)
    flags |= tnf; // TNF (Type Name Format)
    size_t type_len = strlen(type);

    size_t header_len = 0;
    header_len += 1; // Flags and TNF
    header_len += 1; // Type length
    if(payload_len < 0xFF) {
        flags |= 1 << 4; // SR (Short Record)
        header_len += 1; // Payload length
    } else {
        header_len += 4; // Payload length
    }
    header_len += type_len; // Payload type

    // Start consolidating into NDEF buffer
    memset(app->ndef_buffer, 0, MAX_NDEF_LEN);
    uint8_t* buf = app->ndef_buffer;

    // NDEF TLV block
    *buf++ = 0x03; // TLV type
    size_t record_len = header_len + payload_len;
    if(record_len < 0xFF) {
        *buf++ = record_len; // TLV length
    } else {
        *buf++ = 0xFF; // TLV length
        *buf++ = record_len >> 8; // ...
        *buf++ = record_len & 0xFF; // ...
    }

    // Record header
    *buf++ = flags; // Flags and TNF
    *buf++ = type_len; // Type length
    if(flags & (1 << 4)) { // SR (Short Record)
        *buf++ = payload_len; // Payload length
    } else {
        *buf++ = payload_len >> 24 & 0xFF; // Payload length
        *buf++ = payload_len >> 16 & 0xFF; // ...
        *buf++ = payload_len >> 8 & 0xFF; // ...
        *buf++ = payload_len & 0xFF; // ...
    }
    memcpy(buf, type, type_len); // Payload type
    buf += type_len;

    // Record payload
    memcpy(buf, payload, payload_len);
    buf += payload_len;
    free(payload);

    // Record terminator
    *buf++ = 0xFE;

    return buf - app->ndef_buffer; // Size of NDEF data
}

void nfc_maker_scene_save_generate_submenu_callback(void* context, uint32_t index) {
    NfcMaker* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void nfc_maker_scene_save_generate_on_enter(void* context) {
    NfcMaker* app = context;
    Submenu* submenu = app->submenu;
    size_t ndef_size = nfc_maker_scene_save_generate_populate_ndef_buffer(app);

    submenu_set_header(submenu, "Tag Type:");

    for(Ntag ntag = 0; ntag < NtagMAX; ntag++) {
        submenu_add_lockable_item(
            submenu,
            ntag_names[ntag],
            ntag,
            nfc_maker_scene_save_generate_submenu_callback,
            app,
            ndef_size > ntag_sizes[ntag],
            "Data is\ntoo large!");
    }

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, NfcMakerSceneSaveGenerate));

    view_dispatcher_switch_to_view(app->view_dispatcher, NfcMakerViewSubmenu);
}

bool nfc_maker_scene_save_generate_on_event(void* context, SceneManagerEvent event) {
    NfcMaker* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, NfcMakerSceneSaveGenerate, event.event);
        if(event.event >= NtagMAX) return consumed;
        consumed = true;

        nfc_data_generator_fill_data(ntag_generators[event.event], app->nfc_device);
        MfUltralightData* data = mf_ultralight_alloc();
        nfc_device_copy_data(app->nfc_device, NfcProtocolMfUltralight, data);

        size_t size =
            MIN(ntag_sizes[event.event], // Known size
                data->page[3].data[2] * NTAG_DATA_AREA_UNIT_SIZE // Capability Container
            );
        memcpy(&data->page[4].data[0], app->ndef_buffer, size);
        nfc_device_set_data(app->nfc_device, NfcProtocolMfUltralight, data);
        mf_ultralight_free(data);

        scene_manager_next_scene(app->scene_manager, NfcMakerSceneSaveName);
    }

    return consumed;
}

void nfc_maker_scene_save_generate_on_exit(void* context) {
    NfcMaker* app = context;
    submenu_reset(app->submenu);
}
