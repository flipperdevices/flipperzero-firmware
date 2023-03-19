#include "avr_isp_rw_worker.h"
#include "avr_isp_rw_worker_cmd.h"
#include "avr_isp.h"
#include "../lib/driver/avr_isp_prog_cmd.h"
#include "../lib/driver/avr_isp_chip_arr.h"

#include "flipper_i32hex_file.h"
#include <flipper_format/flipper_format.h>

#include <furi.h>

#define TAG "AvrIspRWWorker"

struct AvrIspRWWorker {
    FuriThread* thread;
    AvrIsp* avr_isp;

    uint32_t chip_arr_ind;
    bool chip_detect;
    volatile bool worker_running;
    AvrIspRWWorkerCallback callback;
    void* context;
};

void avr_isp_rw_worker_detect_chip(AvrIspRWWorker* instance) {
    furi_assert(instance);
    FURI_LOG_D(TAG, "Detecting AVR chip");
    uint8_t data[10];
    size_t ind = avr_isp_chip_arr_size + 1;
    do {
        if(!avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
            FURI_LOG_E(TAG, "Well, I managed to enter the mod program");
            break;
        }
        avr_isp_read_signature(instance->avr_isp, data);
        avr_isp_end_pmode(instance->avr_isp);

        if(data[0] != 0x1E) {
            ind = avr_isp_chip_arr_size + 1; //No detect chip
        } else {
            for(ind = 0; ind < avr_isp_chip_arr_size; ind++) {
                if(avr_isp_chip_arr[ind].avrarch != F_AVR8) continue;
                if(avr_isp_chip_arr[ind].sigs[1] == data[1]) {
                    if(avr_isp_chip_arr[ind].sigs[2] == data[2]) {
                        FURI_LOG_D(TAG, "Detect AVR chip = \"%s\"", avr_isp_chip_arr[ind].name);
                        break;
                    }
                }
            }
        }

    } while(0);

    if(instance->callback) {
        if(ind > avr_isp_chip_arr_size) {
            //ToDo add output ID chip
            instance->callback(instance->context, "No detect");
            instance->chip_detect = false;
            instance->chip_arr_ind = ind;
        } else if(ind < avr_isp_chip_arr_size) {
            instance->callback(instance->context, avr_isp_chip_arr[ind].name);
            instance->chip_detect = true;
            instance->chip_arr_ind = ind;
        } else {
            //ToDo add output ID chip
            instance->callback(instance->context, "Unknown");
            instance->chip_detect = false;
            instance->chip_arr_ind = ind;
        }
    }
    avr_isp_rw_worker_rx(instance);
}

/** Worker thread
 * 
 * @param context 
 * @return exit code 
 */
static int32_t avr_isp_rw_worker_thread(void* context) {
    AvrIspRWWorker* instance = context;

    FURI_LOG_D(TAG, "Start");
    while(instance->worker_running) {
        furi_delay_ms(100);
    }
    FURI_LOG_D(TAG, "Stop");
    return 0;
}

AvrIspRWWorker* avr_isp_rw_worker_alloc(void* context) {
    furi_assert(context);
    UNUSED(context);
    AvrIspRWWorker* instance = malloc(sizeof(AvrIspRWWorker));
    instance->avr_isp = avr_isp_alloc();

    instance->chip_detect = false;

    instance->thread =
        furi_thread_alloc_ex("AvrIspRWWorker", 2048, avr_isp_rw_worker_thread, instance);
    return instance;
}

void avr_isp_rw_worker_free(AvrIspRWWorker* instance) {
    furi_assert(instance);

    avr_isp_free(instance->avr_isp);
    furi_thread_free(instance->thread);
    free(instance);
}

void avr_isp_rw_worker_set_callback(
    AvrIspRWWorker* instance,
    AvrIspRWWorkerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(context);
    instance->callback = callback;
    instance->context = context;
}

void avr_isp_rw_worker_start(AvrIspRWWorker* instance) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);

    instance->worker_running = true;

    furi_thread_start(instance->thread);
}

void avr_isp_rw_worker_stop(AvrIspRWWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_running);

    instance->worker_running = false;

    furi_thread_join(instance->thread);
}

bool avr_isp_rw_worker_is_running(AvrIspRWWorker* instance) {
    furi_assert(instance);
    return instance->worker_running;
}

bool avr_isp_rw_worker_rx(AvrIspRWWorker* instance) {
    furi_assert(instance);
    uint8_t data[256];

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* flipper_format = flipper_format_file_alloc(storage);
    FURI_LOG_D(TAG, "Read chip");
    do {
        if(!instance->chip_detect) {
            FURI_LOG_E(TAG, "No detect AVR chip");
            break;
        }

        // if(!storage_simply_mkdir(storage, "/any/avr_isp/")) {
        //     FURI_LOG_E(TAG, "storage_simply_mkdir");
        //     break;
        // }

        // if(!storage_simply_remove(storage, "dump.avr")) {
        //     break;
        // }

        if(!flipper_format_file_open_always(flipper_format, "/any/avr_isp/dump.avr")) {
            FURI_LOG_E(TAG, "flipper_format_file_open_always");
            break;
        }
        if(!flipper_format_write_header_cstr(flipper_format, "Flipper dump AVR", 1)) {
            FURI_LOG_E(TAG, "flipper_format_write_header_cstr");
            break;
        }
        if(!flipper_format_write_comment_cstr(flipper_format, "This is comment")) {
            FURI_LOG_E(TAG, "flipper_format_write_comment_cstr");
            break;
        }
        //if(!flipper_format_write_string_cstr(file, test_string_key, test_string_data)) break;

        if(!flipper_format_write_string_cstr(
               flipper_format, "Chip name", avr_isp_chip_arr[instance->chip_arr_ind].name)) {
            FURI_LOG_E(TAG, "Chip name");
            break;
        }

        if(!avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
            FURI_LOG_E(TAG, "Well, I managed to enter the mod program");
            break;
        }

        avr_isp_read_signature(instance->avr_isp, data);

        if(!flipper_format_write_hex(flipper_format, "ID", data, 3)) {
            FURI_LOG_E(TAG, "Unable to add ID");
            break;
        }
        FURI_LOG_D(TAG, "ID = 0x%02X 0x%02X 0x%02X", data[0], data[1], data[2]);

        data[0] = avr_isp_read_fuse_high(instance->avr_isp);
        data[1] = avr_isp_read_fuse_low(instance->avr_isp);
        data[2] = avr_isp_read_fuse_extended(instance->avr_isp);
        data[3] = avr_isp_read_lock_byte(instance->avr_isp);

        if(!flipper_format_write_hex(flipper_format, "Hfuse", data, 1)) {
            FURI_LOG_E(TAG, "Unable to add Hfuse");
            break;
        }

        if(!flipper_format_write_hex(flipper_format, "Lfuse", data + 1, 1)) {
            FURI_LOG_E(TAG, "Unable to add Lfuse");
            break;
        }

        if(!flipper_format_write_hex(flipper_format, "Efuse", data + 2, 1)) {
            FURI_LOG_E(TAG, "Unable to add Efuse");
            break;
        }

        if(!flipper_format_write_hex(flipper_format, "Lock", data + 3, 1)) {
            FURI_LOG_E(TAG, "Unable to add Lock");
            break;
        }

        FURI_LOG_D(
            TAG,
            "hfuse = 0x%02X lfuse = 0x%02X efuse = 0x%02X lock = 0x%02X",
            data[0],
            data[1],
            data[2],
            data[3]);

        if(!flipper_format_write_string_cstr(flipper_format, "Dump_flash", "flash.hex")) {
            FURI_LOG_E(TAG, "Unable to add Dump_flash");
            break;
        }
        if(avr_isp_chip_arr[instance->chip_arr_ind].eepromsize > 0) {
            if(!flipper_format_write_string_cstr(flipper_format, "Dump_eeprom", "eeprom.hex")) {
                FURI_LOG_E(TAG, "Unable to add Dump_eeprom");
                break;
            }
        }

        FURI_LOG_D(TAG, "EEPROM");

        if(avr_isp_chip_arr[instance->chip_arr_ind].eepromsize > 0) {
            FlipperI32HexFile* flipper_hex_eeprom = flipper_i32hex_file_open_write(
                "/any/avr_isp/eeprom.hex", avr_isp_chip_arr[instance->chip_arr_ind].eepromoffset);
            int32_t size_data = 32;
            if(size_data > avr_isp_chip_arr[instance->chip_arr_ind].eepromsize)
                size_data = avr_isp_chip_arr[instance->chip_arr_ind].eepromsize;

            for(uint16_t i = 0; i < avr_isp_chip_arr[instance->chip_arr_ind].eepromsize;
                i += size_data) {
                avr_isp_read_page(
                    instance->avr_isp, STK_SET_EEPROM_TYPE, i, size_data, data, sizeof(data));
                flipper_i32hex_file_bin_to_i32hex_set_data(flipper_hex_eeprom, data, size_data);
                printf("%s\r\n", flipper_i32hex_file_get_string(flipper_hex_eeprom));
            }
            flipper_i32hex_file_bin_to_i32hex_set_end_line(flipper_hex_eeprom);
            printf("%s\r\n", flipper_i32hex_file_get_string(flipper_hex_eeprom));
            flipper_i32hex_file_close(flipper_hex_eeprom);
        }

        // FURI_LOG_D(TAG, "FLASH");
        // FlipperI32HexFile* flipper_hex_flash =
        //     flipper_i32hex_file_open_write("/any/avr_isp/flash.hex", avr_isp_chip_arr[instance->chip_arr_ind].flashoffset;);
        // for(uint16_t i = 0; i < avr_isp_chip_arr[instance->chip_arr_ind].flashsize / 2;
        //     i += avr_isp_chip_arr[instance->chip_arr_ind].pagesize / 2) {
        //     avr_isp_read_page(
        //         instance->avr_isp,
        //         STK_SET_FLASH_TYPE,
        //         i,
        //         avr_isp_chip_arr[instance->chip_arr_ind].pagesize,
        //         data,
        //         sizeof(data));
        //     flipper_i32hex_file_bin_to_i32hex_add_data(
        //         flipper_hex_flash, data, avr_isp_chip_arr[instance->chip_arr_ind].pagesize);
        //     printf("%s\r\n", flipper_i32hex_file_get_string(flipper_hex_flash));
        // }
        // flipper_i32hex_file_bin_to_i32hex_add_end_line(flipper_hex_flash);
        // printf("%s\r\n", flipper_i32hex_file_get_string(flipper_hex_flash));
        // flipper_i32hex_file_close(flipper_hex_flash);

        avr_isp_end_pmode(instance->avr_isp);

        // // //################################################################
        // FURI_LOG_D(TAG, "FUSE Write");
        // if(!avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
        //     FURI_LOG_E(TAG, "Well, I managed to enter the mod program");
        //     break;
        // }
        // FURI_LOG_E(TAG, "fuse %d", avr_isp_write_fuse_low(instance->avr_isp, 0xFF));
        // FURI_LOG_E(TAG, "fuse %d", avr_isp_write_fuse_extended(instance->avr_isp, 0xFD));
        // FURI_LOG_E(TAG, "fuse %d", avr_isp_write_fuse_high(instance->avr_isp, 0xDA));
        // FURI_LOG_E(TAG, "fuse %d", avr_isp_write_lock_byte(instance->avr_isp, 0xFF));


        // avr_isp_end_pmode(instance->avr_isp);
        // //##################################################################

        // //################################################################
        // FURI_LOG_D(TAG, "ERASE CHIP");
        // avr_isp_erase_chip(instance->avr_isp);
        // //##################################################################

        // //##########################################################
        // FURI_LOG_D(TAG, "FLASH WRITE");

        // if(!avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
        //     FURI_LOG_E(TAG, "Well, I managed to enter the mod program");
        //     break;
        // }
        // avr_isp_read_signature(instance->avr_isp, data);

        // FlipperI32HexFile* flipper_hex_flash =
        //     flipper_i32hex_file_open_read("/any/avr_isp/flash.hex");

        // uint32_t addr = avr_isp_chip_arr[instance->chip_arr_ind].flashoffset;
        // FlipperI32HexFileRet flipper_hex_ret =
        //     flipper_i32hex_file_i32hex_to_bin_get_data(flipper_hex_flash, data, sizeof(data));
        // FURI_LOG_D(TAG, "FLASH WRITE Page   ---%ld", addr);
        // while((flipper_hex_ret.status == FlipperI32HexFileStatusData) ||
        //       (flipper_hex_ret.status == FlipperI32HexFileStatusUdateAddr)) {
        //     // FURI_LOG_D(TAG, "EEPROM WRITE Page1");
        //     // for(size_t i = 0; i < flipper_hex_ret.data_size; i++) {
        //     //     printf("%02X ", data[i]);
        //     // }
        //     // printf("\r\n");

        //     switch(flipper_hex_ret.status) {
        //     case FlipperI32HexFileStatusData:
        //         if(!avr_isp_write_page(
        //                instance->avr_isp,
        //                STK_SET_FLASH_TYPE,
        //                avr_isp_chip_arr[instance->chip_arr_ind].flashsize,
        //                addr,
        //                avr_isp_chip_arr[instance->chip_arr_ind].pagesize,
        //                data,
        //                flipper_hex_ret.data_size)) {
        //             break;
        //         }
        //         addr += flipper_hex_ret.data_size/2;
        //         break;

        //     case FlipperI32HexFileStatusUdateAddr:
        //         addr = data[0] << 24 | data[1] << 16;
        //         break;

        //     default:
        //         furi_crash(TAG " Incorrect status.");
        //         break;
        //     }

        //     flipper_hex_ret =
        //         flipper_i32hex_file_i32hex_to_bin_get_data(flipper_hex_flash, data, sizeof(data));
        // }

        // flipper_i32hex_file_close(flipper_hex_flash);
        // avr_isp_end_pmode(instance->avr_isp);
        //##########################################################

        //     FURI_LOG_D(TAG, "CHECK HEX");
        //     FlipperI32HexFile* flipper_hex_eeprom_read =
        //         flipper_i32hex_file_open_read("/any/avr_isp/a.hex");
        //     if(flipper_i32hex_file_check(flipper_hex_eeprom_read)) {
        //         FURI_LOG_D(TAG, "Check OK");
        //     } else {
        //         FURI_LOG_E(TAG, "Check ERROR");
        //     }
        //    // flipper_i32hex_file_close(flipper_hex_eeprom_read);

        //     //##########################################################
        //     FURI_LOG_D(TAG, "EEPROM WRITE");

        //     if(!avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
        //         FURI_LOG_E(TAG, "Well, I managed to enter the mod program");
        //         break;
        //     }
        //     avr_isp_read_signature(instance->avr_isp, data);
        //     uint32_t addr = avr_isp_chip_arr[instance->chip_arr_ind].eepromoffset;
        //     FlipperI32HexFileRet flipper_hex_ret = flipper_i32hex_file_i32hex_to_bin_get_data(
        //         flipper_hex_eeprom_read, data, sizeof(data));

        //     while((flipper_hex_ret.status == FlipperI32HexFileStatusData) ||
        //           (flipper_hex_ret.status == FlipperI32HexFileStatusUdateAddr)) {
        //         FURI_LOG_D(TAG, "EEPROM WRITE Page");
        //         for(size_t i = 0; i < flipper_hex_ret.data_size; i++) {
        //             printf("%02X ", data[i]);
        //         }
        //         printf("\r\n");

        //         switch(flipper_hex_ret.status) {
        //         case FlipperI32HexFileStatusData:
        //             if(!avr_isp_write_page(
        //                    instance->avr_isp,
        //                    STK_SET_EEPROM_TYPE,
        //                    avr_isp_chip_arr[instance->chip_arr_ind].eepromsize,
        //                    addr,
        //                    avr_isp_chip_arr[instance->chip_arr_ind].eeprompagesize,
        //                    data,
        //                    flipper_hex_ret.data_size)) {
        //                 break;
        //             }
        //             addr += flipper_hex_ret.data_size;
        //             break;

        //         case FlipperI32HexFileStatusUdateAddr:
        //             addr = data[0] << 24 | data[1] << 16;
        //             break;

        //         default:
        //             furi_crash(TAG " Incorrect status.");
        //             break;
        //         }

        //         flipper_hex_ret = flipper_i32hex_file_i32hex_to_bin_get_data(
        //             flipper_hex_eeprom_read, data, sizeof(data));
        //     }

        //     flipper_i32hex_file_close(flipper_hex_eeprom_read);
        //     avr_isp_end_pmode(instance->avr_isp);
        //     //##########################################################

        // //################################################################
        // FURI_LOG_D(TAG, "ERASE CHIP");
        // avr_isp_erase_chip(instance->avr_isp);
        // //##################################################################
        FURI_LOG_D(TAG, "___END___");
    } while(0);
    flipper_format_free(flipper_format);
    furi_record_close(RECORD_STORAGE);
    return true;
}