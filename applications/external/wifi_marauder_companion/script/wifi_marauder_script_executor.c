#include "../wifi_marauder_app_i.h"
#include "wifi_marauder_script_executor.h"

void _wifi_marauder_script_delay(WifiMarauderScriptWorker* worker, uint32_t delay_secs) {
    for (uint32_t i=0; i<delay_secs && worker->is_running; i++)
        furi_delay_ms(1000);
}

void _send_stop() {
    const char stop_command[] = "stopscan\n";
    wifi_marauder_uart_tx((uint8_t*)(stop_command), strlen(stop_command));
}

void _send_line_break() {
    wifi_marauder_uart_tx((uint8_t*)("\n"), 1);
}

void _send_channel_select(int channel) {
    char command[30];
    wifi_marauder_uart_tx((uint8_t*)("\n"), 1);
    snprintf(command, sizeof(command), "channel -s %d\n", channel);
    wifi_marauder_uart_tx((uint8_t*)(command), strlen(command));
}

void _wifi_marauder_script_execute_scan(WifiMarauderScriptStageScan* stage, WifiMarauderScriptWorker* worker) {
    char command[15];
    // Set channel
    if (stage->channel > 0) {
        _send_channel_select(stage->channel);
    }
    // Start scan
    if (stage->type == WifiMarauderScriptScanTypeAp) {
        snprintf(command, sizeof(command), "scanap\n");
    } else {
        snprintf(command, sizeof(command), "scansta\n");
    }
    wifi_marauder_uart_tx((uint8_t*)(command), strlen(command));
    _wifi_marauder_script_delay(worker, stage->timeout);
    _send_stop();
}

void _wifi_marauder_script_execute_select(WifiMarauderScriptStageSelect* stage) {
    const char* select_type = NULL;
    switch (stage->type) {
        case WifiMarauderScriptSelectTypeAp:
            select_type = "-a";
            break;
        case WifiMarauderScriptSelectTypeStation:
            select_type = "-c";
            break;
        case WifiMarauderScriptSelectTypeSsid:
            select_type = "-s";
            break;
        default:
            return;  // invalid stage
    }

    char command[256];
    if (strcmp(stage->filter, "all") == 0) {
        snprintf(command, sizeof(command), "select %s all\n", select_type);
    } else {
        snprintf(command, sizeof(command), "select %s {%s}\n", select_type, stage->filter);
    }

    const size_t command_length = strlen(command);
    wifi_marauder_uart_tx((uint8_t*)command, command_length);
}

void _wifi_marauder_script_execute_deauth(WifiMarauderScriptStageDeauth* stage, WifiMarauderScriptWorker* worker) {
    const char attack_command[] = "attack -t deauth\n";
    wifi_marauder_uart_tx((uint8_t*)(attack_command), strlen(attack_command));
    _wifi_marauder_script_delay(worker, stage->timeout);
    _send_stop();
}

void _wifi_marauder_script_execute_sniff_beacon(WifiMarauderScriptStageSniffBeacon* stage, WifiMarauderScriptWorker* worker) {
    const char sniff_command[] = "sniffbeacon\n";
    wifi_marauder_uart_tx((uint8_t*)sniff_command, strlen(sniff_command));
    _wifi_marauder_script_delay(worker, stage->timeout);
    _send_stop();
}

void _wifi_marauder_script_execute_sniff_pmkid(WifiMarauderScriptStageSniffPmkid* stage, WifiMarauderScriptWorker* worker) {
    char attack_command[50] = "sniffpmkid";
    int len = strlen(attack_command);
    
    if (stage->channel > 0) {
        len += snprintf(attack_command + len, sizeof(attack_command) - len, " -c %d", stage->channel);
    }

    if (stage->force_deauth) {
        len += snprintf(attack_command + len, sizeof(attack_command) - len, " -d");
    }

    len += snprintf(attack_command + len, sizeof(attack_command) - len, "\n");

    wifi_marauder_uart_tx((uint8_t*)attack_command, len);
    _wifi_marauder_script_delay(worker, stage->timeout);
    _send_stop();
}

void _wifi_marauder_script_execute_beacon_list(WifiMarauderScriptStageBeaconList* stage, WifiMarauderScriptWorker* worker) {
    const char clearlist_command[] = "clearlist -s\n";
    wifi_marauder_uart_tx((uint8_t*)(clearlist_command), strlen(clearlist_command));

    char command[100];
    char *ssid;

    for (int i = 0; i < stage->ssid_count; i++) {
        ssid = stage->ssids[i];
        snprintf(command, sizeof(command), "ssid -a -n \"%s\"", ssid);
        wifi_marauder_uart_tx((uint8_t*)(command), strlen(command));
        _send_line_break();
    }
    const char attack_command[] = "attack -t beacon -l\n";
    wifi_marauder_uart_tx((uint8_t*)(attack_command), strlen(attack_command));
    _wifi_marauder_script_delay(worker, stage->timeout);
    _send_stop();
}

void wifi_marauder_script_execute_start(void *context) {
    furi_assert(context);
    WifiMarauderScriptWorker* worker = context;
    WifiMarauderScript* script = worker->script;
    char command[100];

    // Enables or disables the LED according to script settings
    snprintf(command, sizeof(command), "settings -s EnableLED %s", script->enable_led ? "enable" : "disable");
    wifi_marauder_uart_tx((uint8_t*)command, strlen(command));
    _send_line_break();

    // Enables or disables PCAP saving according to script settings
    snprintf(command, sizeof(command), "settings -s SavePCAP %s", script->save_pcap ? "enable" : "disable");
    wifi_marauder_uart_tx((uint8_t*)command, strlen(command));
    _send_line_break();
}


void wifi_marauder_script_execute_stage(WifiMarauderScriptStage* stage, void *context) {
    furi_assert(context);
    WifiMarauderScriptWorker* worker = context;
    void *stage_data = stage->stage;

    switch (stage->type) {
        case WifiMarauderScriptStageTypeScan:
            _wifi_marauder_script_execute_scan((WifiMarauderScriptStageScan*)stage_data, worker);
            break;
        case WifiMarauderScriptStageTypeSelect:
            _wifi_marauder_script_execute_select((WifiMarauderScriptStageSelect*)stage_data);
            break;
        case WifiMarauderScriptStageTypeDeauth:
            _wifi_marauder_script_execute_deauth((WifiMarauderScriptStageDeauth*)stage_data, worker);
            break;
        case WifiMarauderScriptStageTypeSniffBeacon:
            _wifi_marauder_script_execute_sniff_beacon((WifiMarauderScriptStageSniffBeacon*)stage_data, worker);
            break;
        case WifiMarauderScriptStageTypeSniffPmkid:
            _wifi_marauder_script_execute_sniff_pmkid((WifiMarauderScriptStageSniffPmkid*)stage_data, worker);
            break;
        case WifiMarauderScriptStageTypeBeaconList:
            _wifi_marauder_script_execute_beacon_list((WifiMarauderScriptStageBeaconList*)stage_data, worker);
            break;
        default:
            break;
    }
}