#include "avr_isp_worker_rw.h"
#include <furi_hal_pwm.h>
#include "avr_isp_types.h"
#include "avr_isp.h"
#include "../lib/driver/avr_isp_prog_cmd.h"
#include "../lib/driver/avr_isp_chip_arr.h"

#include "flipper_i32hex_file.h"
#include <flipper_format/flipper_format.h>

#include <furi.h>

#define TAG "AvrIspWorkerRW"

struct AvrIspWorkerRW {
    AvrIsp* avr_isp;
    FuriThread* thread;
    volatile bool worker_running;

    uint32_t chip_arr_ind;
    bool chip_detect;
    uint8_t lfuse;
    uint8_t hfuse;
    uint8_t efuse;
    uint8_t lock;
    float progress_flash;
    float progress_eeprom;
    const char* file_path;
    const char* file_name;
    AvrIspSignature signature;
    AvrIspWorkerRWCallback callback;
    void* context;

    AvrIspWorkerRWStatusCallback callback_status;
    void* context_status;
};

typedef enum {
    AvrIspWorkerRWEvtStop = (1 << 0),

    AvrIspWorkerRWEvtReading = (1 << 1),
    AvrIspWorkerRWEvtVerification = (1 << 2),
    AvrIspWorkerRWEvtWriting = (1 << 3),

} AvrIspWorkerRWEvt;
#define AVR_ISP_WORKER_ALL_EVENTS                                                          \
    (AvrIspWorkerRWEvtWriting | AvrIspWorkerRWEvtVerification | AvrIspWorkerRWEvtReading | \
     AvrIspWorkerRWEvtStop)

/** Worker thread
 * 
 * @param context 
 * @return exit code 
 */
static int32_t avr_isp_worker_rw_thread(void* context) {
    AvrIspWorkerRW* instance = context;

    /* start PWM on &gpio_ext_pa4 */
    furi_hal_pwm_start(FuriHalPwmOutputIdLptim2PA4, 4000000, 50);

    FURI_LOG_D(TAG, "Start");
    while(1) {
        uint32_t events =
            furi_thread_flags_wait(AVR_ISP_WORKER_ALL_EVENTS, FuriFlagWaitAny, FuriWaitForever);

        if(events & AvrIspWorkerRWEvtStop) {
            break;
        }

        if(events & AvrIspWorkerRWEvtWriting) {
            // if(avr_isp_worker_rw_get_dump(instance, instance->file_path, instance->file_name)) {
            //     if(instance->callback_status)
            //         instance->callback_status(
            //             instance->context_status, AvrIspWorkerRWStatusEndWriting);
            // } else {
            //     if(instance->callback_status)
            //         instance->callback_status(
            //             instance->context_status, AvrIspWorkerRWStatusErrorWriting);
            // }
        }

        if(events & AvrIspWorkerRWEvtVerification) {
            if(avr_isp_worker_rw_verification(instance, instance->file_path, instance->file_name)) {
                if(instance->callback_status)
                    instance->callback_status(
                        instance->context_status, AvrIspWorkerRWStatusEndVerification);
            } else {
                if(instance->callback_status)
                    instance->callback_status(instance->context_status, AvrIspWorkerRWStatusErrorVerification);
            }
        }

        if(events & AvrIspWorkerRWEvtReading) {
            if(avr_isp_worker_rw_get_dump(instance, instance->file_path, instance->file_name)) {
                if(instance->callback_status)
                    instance->callback_status(
                        instance->context_status, AvrIspWorkerRWStatusEndReading);
            } else {
                if(instance->callback_status)
                    instance->callback_status(instance->context_status, AvrIspWorkerRWStatusErrorReading);
            }
        }
    }
    FURI_LOG_D(TAG, "Stop");

    furi_hal_pwm_stop(FuriHalPwmOutputIdLptim2PA4);

    return 0;
}

bool avr_isp_worker_rw_detect_chip(AvrIspWorkerRW* instance) {
    furi_assert(instance);
    FURI_LOG_D(TAG, "Detecting AVR chip");
    instance->chip_detect = false;
    instance->chip_arr_ind = avr_isp_chip_arr_size + 1;

    /* start PWM on &gpio_ext_pa4 */
    furi_hal_pwm_start(FuriHalPwmOutputIdLptim2PA4, 4000000, 50);

    do {
        if(!avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
            FURI_LOG_E(TAG, "Well, I managed to enter the mod program");
            break;
        }
        instance->signature = avr_isp_read_signature(instance->avr_isp);

        if(instance->signature.vendor != 0x1E) {
            //No detect chip
        } else {
            for(uint32_t ind = 0; ind < avr_isp_chip_arr_size; ind++) {
                if(avr_isp_chip_arr[ind].avrarch != F_AVR8) continue;
                if(avr_isp_chip_arr[ind].sigs[1] == instance->signature.part_family) {
                    if(avr_isp_chip_arr[ind].sigs[2] == instance->signature.part_number) {
                        FURI_LOG_D(TAG, "Detect AVR chip = \"%s\"", avr_isp_chip_arr[ind].name);
                        FURI_LOG_D(
                            TAG,
                            "Signature = 0x%02X 0x%02X 0x%02X",
                            instance->signature.vendor,
                            instance->signature.part_family,
                            instance->signature.part_number);

                        switch(avr_isp_chip_arr[ind].nfuses) {
                        case 1:
                            instance->lfuse = avr_isp_read_fuse_low(instance->avr_isp);
                            FURI_LOG_D(TAG, "Lfuse = %02X", instance->lfuse);
                            break;
                        case 2:
                            instance->lfuse = avr_isp_read_fuse_low(instance->avr_isp);
                            instance->hfuse = avr_isp_read_fuse_high(instance->avr_isp);
                            FURI_LOG_D(
                                TAG, "Lfuse = %02X Hfuse = %02X", instance->lfuse, instance->hfuse);
                            break;
                        case 3:
                            instance->lfuse = avr_isp_read_fuse_low(instance->avr_isp);
                            instance->hfuse = avr_isp_read_fuse_high(instance->avr_isp);
                            instance->efuse = avr_isp_read_fuse_extended(instance->avr_isp);
                            FURI_LOG_D(
                                TAG,
                                "Lfuse = %02X Hfuse = %02X Efuse = %02X",
                                instance->lfuse,
                                instance->hfuse,
                                instance->efuse);
                            break;
                        default:
                            break;
                        }
                        if(avr_isp_chip_arr[ind].nlocks == 1) {
                            instance->lock = avr_isp_read_lock_byte(instance->avr_isp);
                            FURI_LOG_D(TAG, "Lock = %02X", instance->lock);
                        }
                        instance->chip_detect = true;
                        instance->chip_arr_ind = ind;
                        break;
                    }
                }
            }
        }
        avr_isp_end_pmode(instance->avr_isp);

        furi_hal_pwm_stop(FuriHalPwmOutputIdLptim2PA4);

    } while(0);
    if(instance->callback) {
        if(instance->chip_arr_ind > avr_isp_chip_arr_size) {
            //ToDo add output ID chip
            instance->callback(instance->context, "No detect", instance->chip_detect, 0);

        } else if(instance->chip_arr_ind < avr_isp_chip_arr_size) {
            instance->callback(
                instance->context,
                avr_isp_chip_arr[instance->chip_arr_ind].name,
                instance->chip_detect,
                avr_isp_chip_arr[instance->chip_arr_ind].flashsize);
        } else {
            //ToDo add output ID chip
            instance->callback(instance->context, "Unknown", instance->chip_detect, 0);
        }
    }

    return instance->chip_detect;
}

AvrIspWorkerRW* avr_isp_worker_rw_alloc(void* context) {
    furi_assert(context);
    UNUSED(context);
    AvrIspWorkerRW* instance = malloc(sizeof(AvrIspWorkerRW));
    instance->avr_isp = avr_isp_alloc();

    instance->thread =
        furi_thread_alloc_ex("AvrIspWorkerRW", 4096, avr_isp_worker_rw_thread, instance);

    instance->chip_detect = false;
    instance->lfuse = 0;
    instance->hfuse = 0;
    instance->efuse = 0;
    instance->lock = 0;
    instance->progress_flash = 0.0f;
    instance->progress_eeprom = 0.0f;

    return instance;
}

void avr_isp_worker_rw_free(AvrIspWorkerRW* instance) {
    furi_assert(instance);

    avr_isp_free(instance->avr_isp);

    furi_check(!instance->worker_running);
    furi_thread_free(instance->thread);

    free(instance);
}

void avr_isp_worker_rw_start(AvrIspWorkerRW* instance) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);

    instance->worker_running = true;

    furi_thread_start(instance->thread);
}

void avr_isp_worker_rw_stop(AvrIspWorkerRW* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_running);

    instance->worker_running = false;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), AvrIspWorkerRWEvtStop);

    furi_thread_join(instance->thread);
}

bool avr_isp_worker_rw_is_running(AvrIspWorkerRW* instance) {
    furi_assert(instance);
    return instance->worker_running;
}

void avr_isp_worker_rw_set_callback(
    AvrIspWorkerRW* instance,
    AvrIspWorkerRWCallback callback,
    void* context) {
    furi_assert(instance);
    instance->callback = callback;
    instance->context = context;
}

void avr_isp_worker_rw_set_callback_status(
    AvrIspWorkerRW* instance,
    AvrIspWorkerRWStatusCallback callback_status,
    void* context_status) {
    furi_assert(instance);
    instance->callback_status = callback_status;
    instance->context_status = context_status;
}

float avr_isp_worker_rw_get_progress_flash(AvrIspWorkerRW* instance) {
    furi_assert(instance);
    return instance->progress_flash;
}

float avr_isp_worker_rw_get_progress_eeprom(AvrIspWorkerRW* instance) {
    furi_assert(instance);
    return instance->progress_eeprom;
}

void avr_isp_worker_rw_get_dump_flash(AvrIspWorkerRW* instance, const char* file_path) {
    furi_assert(instance);
    furi_check(instance->avr_isp);

    FURI_LOG_D(TAG, "Dump FLASH %s", file_path);

    FlipperI32HexFile* flipper_hex_flash = flipper_i32hex_file_open_write(
        file_path, avr_isp_chip_arr[instance->chip_arr_ind].flashoffset);

    uint8_t data[272] = {0};

    for(uint16_t i = 0; i < avr_isp_chip_arr[instance->chip_arr_ind].flashsize / 2;
        i += avr_isp_chip_arr[instance->chip_arr_ind].pagesize / 2) {
        avr_isp_read_page(
            instance->avr_isp,
            STK_SET_FLASH_TYPE,
            i,
            avr_isp_chip_arr[instance->chip_arr_ind].pagesize,
            data,
            sizeof(data));
        flipper_i32hex_file_bin_to_i32hex_set_data(
            flipper_hex_flash, data, avr_isp_chip_arr[instance->chip_arr_ind].pagesize);
        FURI_LOG_D(TAG, "%s", flipper_i32hex_file_get_string(flipper_hex_flash));
        instance->progress_flash =
            (float)i / (avr_isp_chip_arr[instance->chip_arr_ind].flashsize / 2);
    }
    flipper_i32hex_file_bin_to_i32hex_set_end_line(flipper_hex_flash);
    FURI_LOG_D(TAG, "%s", flipper_i32hex_file_get_string(flipper_hex_flash));
    flipper_i32hex_file_close(flipper_hex_flash);
    instance->progress_flash = 1.0f;
}

void avr_isp_worker_rw_get_dump_eeprom(AvrIspWorkerRW* instance, const char* file_path) {
    furi_assert(instance);
    furi_check(instance->avr_isp);

    FURI_LOG_D(TAG, "Dump EEPROM %s", file_path);

    FlipperI32HexFile* flipper_hex_eeprom = flipper_i32hex_file_open_write(
        file_path, avr_isp_chip_arr[instance->chip_arr_ind].eepromoffset);

    int32_t size_data = 32;
    uint8_t data[256] = {0};

    if(size_data > avr_isp_chip_arr[instance->chip_arr_ind].eepromsize)
        size_data = avr_isp_chip_arr[instance->chip_arr_ind].eepromsize;

    for(uint16_t i = 0; i < avr_isp_chip_arr[instance->chip_arr_ind].eepromsize; i += size_data) {
        avr_isp_read_page(
            instance->avr_isp, STK_SET_EEPROM_TYPE, i, size_data, data, sizeof(data));
        flipper_i32hex_file_bin_to_i32hex_set_data(flipper_hex_eeprom, data, size_data);
        FURI_LOG_D(TAG, "%s", flipper_i32hex_file_get_string(flipper_hex_eeprom));
        instance->progress_eeprom = (float)i / avr_isp_chip_arr[instance->chip_arr_ind].eepromsize;
    }
    flipper_i32hex_file_bin_to_i32hex_set_end_line(flipper_hex_eeprom);
    FURI_LOG_D(TAG, "%s", flipper_i32hex_file_get_string(flipper_hex_eeprom));
    flipper_i32hex_file_close(flipper_hex_eeprom);
    instance->progress_eeprom = 1.0f;
}

bool avr_isp_worker_rw_get_dump(
    AvrIspWorkerRW* instance,
    const char* file_path,
    const char* file_name) {
    furi_assert(instance);
    furi_assert(file_path);
    furi_assert(file_name);

    FURI_LOG_D(TAG, "Read dump chip");
    instance->progress_flash = 0.0f;
    instance->progress_eeprom = 0.0f;
    bool ret = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* flipper_format = flipper_format_file_alloc(storage);
    FuriString* file_path_name = furi_string_alloc();

    if(!avr_isp_worker_rw_detect_chip(instance)) {
        FURI_LOG_E(TAG, "No detect AVR chip");
    } else {
        do {
            furi_string_printf(
                file_path_name, "%s/%s%s", file_path, file_name, AVR_ISP_APP_EXTENSION);
            if(!flipper_format_file_open_always(
                   flipper_format, furi_string_get_cstr(file_path_name))) {
                FURI_LOG_E(TAG, "flipper_format_file_open_always");
                break;
            }
            if(!flipper_format_write_header_cstr(
                   flipper_format, AVR_ISP_APP_FILE_TYPE, AVR_ISP_APP_FILE_VERSION)) {
                FURI_LOG_E(TAG, "flipper_format_write_header_cstr");
                break;
            }
            if(!flipper_format_write_string_cstr(
                   flipper_format, "Chip name", avr_isp_chip_arr[instance->chip_arr_ind].name)) {
                FURI_LOG_E(TAG, "Chip name");
                break;
            }
            if(!flipper_format_write_hex(
                   flipper_format,
                   "Signature",
                   (uint8_t*)&instance->signature,
                   sizeof(AvrIspSignature))) {
                FURI_LOG_E(TAG, "Unable to add Signature");
                break;
            }
            if(avr_isp_chip_arr[instance->chip_arr_ind].nfuses > 0) {
                if(!flipper_format_write_hex(flipper_format, "Lfuse", &instance->lfuse, 1)) {
                    FURI_LOG_E(TAG, "Unable to add Lfuse");
                    break;
                }
            }
            if(avr_isp_chip_arr[instance->chip_arr_ind].nfuses > 1) {
                if(!flipper_format_write_hex(flipper_format, "Hfuse", &instance->hfuse, 1)) {
                    FURI_LOG_E(TAG, "Unable to add Hfuse");
                    break;
                }
            }
            if(avr_isp_chip_arr[instance->chip_arr_ind].nfuses > 2) {
                if(!flipper_format_write_hex(flipper_format, "Efuse", &instance->efuse, 1)) {
                    FURI_LOG_E(TAG, "Unable to add Efuse");
                    break;
                }
            }
            if(avr_isp_chip_arr[instance->chip_arr_ind].nlocks == 1) {
                if(!flipper_format_write_hex(flipper_format, "Lock", &instance->lock, 1)) {
                    FURI_LOG_E(TAG, "Unable to add Lock");
                    break;
                }
            }
            furi_string_printf(file_path_name, "%s_flash.hex", file_name);
            if(!flipper_format_write_string_cstr(
                   flipper_format, "Dump_flash", furi_string_get_cstr(file_path_name))) {
                FURI_LOG_E(TAG, "Unable to add Dump_flash");
                break;
            }

            if(avr_isp_chip_arr[instance->chip_arr_ind].eepromsize > 0) {
                furi_string_printf(file_path_name, "%s_eeprom.hex", file_name);
                if(avr_isp_chip_arr[instance->chip_arr_ind].eepromsize > 0) {
                    if(!flipper_format_write_string_cstr(
                           flipper_format, "Dump_eeprom", furi_string_get_cstr(file_path_name))) {
                        FURI_LOG_E(TAG, "Unable to add Dump_eeprom");
                        break;
                    }
                }
            }
            ret = true;
        } while(false);
    }

    flipper_format_free(flipper_format);
    furi_record_close(RECORD_STORAGE);

    if(ret) {
        if(avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
            //Dump flash
            furi_string_printf(file_path_name, "%s/%s_flash.hex", file_path, file_name);
            avr_isp_worker_rw_get_dump_flash(instance, furi_string_get_cstr(file_path_name));
            //Dump eeprom
            if(avr_isp_chip_arr[instance->chip_arr_ind].eepromsize > 0) {
                furi_string_printf(file_path_name, "%s/%s_eeprom.hex", file_path, file_name);
                avr_isp_worker_rw_get_dump_eeprom(instance, furi_string_get_cstr(file_path_name));
            }

            avr_isp_end_pmode(instance->avr_isp);
        }
    }

    furi_string_free(file_path_name);

    return true;
}

void avr_isp_worker_rw_get_dump_start(
    AvrIspWorkerRW* instance,
    const char* file_path,
    const char* file_name) {
    furi_assert(instance);
    instance->file_path = file_path;
    instance->file_name = file_name;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), AvrIspWorkerRWEvtReading);
}

bool avr_isp_worker_rw_verification_flash(AvrIspWorkerRW* instance, const char* file_path) {
    furi_assert(instance);
    furi_assert(file_path);

    FURI_LOG_D(TAG, "Verification flash %s", file_path);
    instance->progress_flash = 0.0;
    bool ret = true;

    FlipperI32HexFile* flipper_hex_flash = flipper_i32hex_file_open_read(file_path);

    uint8_t data_read_flash[272] = {0};
    uint8_t data_read_hex[272] = {0};

    uint32_t addr = avr_isp_chip_arr[instance->chip_arr_ind].flashoffset;

    FlipperI32HexFileRet flipper_hex_ret = flipper_i32hex_file_i32hex_to_bin_get_data(
        flipper_hex_flash, data_read_hex, sizeof(data_read_hex));

    if(avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
        while(((flipper_hex_ret.status == FlipperI32HexFileStatusData) ||
               (flipper_hex_ret.status == FlipperI32HexFileStatusUdateAddr)) &&
              ret) {
            // for(size_t i = 0; i < flipper_hex_ret.data_size; i++) {
            //     printf("%02X ", data_read_hex[i]);
            // }
            // printf("\r\n");

            switch(flipper_hex_ret.status) {
            case FlipperI32HexFileStatusData:
                avr_isp_read_page(
                    instance->avr_isp,
                    STK_SET_FLASH_TYPE,
                    addr,
                    flipper_hex_ret.data_size,
                    data_read_flash,
                    sizeof(data_read_flash));

                // for(size_t i = 0; i < flipper_hex_ret.data_size; i++) {
                //     printf("%02X ", data_read_flash[i]);
                // }
                // printf("\r\n");

                if(memcmp(data_read_hex, data_read_flash, flipper_hex_ret.data_size) != 0) {
                    ret = false;
                    FURI_LOG_E(TAG, "Verification flash error");
                }

                addr += flipper_hex_ret.data_size / 2;
                instance->progress_flash =
                    (float)addr / (avr_isp_chip_arr[instance->chip_arr_ind].flashsize / 2);
                break;

            case FlipperI32HexFileStatusUdateAddr:
                addr = (data_read_hex[0] << 24 | data_read_hex[1] << 16) / 2;
                break;

            default:
                furi_crash(TAG " Incorrect status.");
                break;
            }

            flipper_hex_ret = flipper_i32hex_file_i32hex_to_bin_get_data(
                flipper_hex_flash, data_read_hex, sizeof(data_read_hex));
        }

        avr_isp_end_pmode(instance->avr_isp);
    }
    flipper_i32hex_file_close(flipper_hex_flash);
    instance->progress_flash = 1.0f;

    return ret;
}

bool avr_isp_worker_rw_verification_eeprom(AvrIspWorkerRW* instance, const char* file_path) {
    furi_assert(instance);
    furi_assert(file_path);

    FURI_LOG_D(TAG, "Verification eeprom %s", file_path);
    instance->progress_eeprom = 0.0;
    bool ret = true;

    FlipperI32HexFile* flipper_hex_eeprom = flipper_i32hex_file_open_read(file_path);

    uint8_t data_read_eeprom[272] = {0};
    uint8_t data_read_hex[272] = {0};

    uint32_t addr = avr_isp_chip_arr[instance->chip_arr_ind].eepromoffset;

    FlipperI32HexFileRet flipper_hex_ret = flipper_i32hex_file_i32hex_to_bin_get_data(
        flipper_hex_eeprom, data_read_hex, sizeof(data_read_hex));

    if(avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
        while(((flipper_hex_ret.status == FlipperI32HexFileStatusData) ||
               (flipper_hex_ret.status == FlipperI32HexFileStatusUdateAddr)) &&
              ret) {
            // for(size_t i = 0; i < flipper_hex_ret.data_size; i++) {
            //     printf("%02X ", data_read_hex[i]);
            // }
            // printf("\r\n");

            switch(flipper_hex_ret.status) {
            case FlipperI32HexFileStatusData:
                avr_isp_read_page(
                    instance->avr_isp,
                    STK_SET_EEPROM_TYPE,
                    addr,
                    flipper_hex_ret.data_size,
                    data_read_eeprom,
                    sizeof(data_read_eeprom));

                // for(size_t i = 0; i < flipper_hex_ret.data_size; i++) {
                //     printf("%02X ", data_read_eeprom[i]);
                // }
                // printf("\r\n");

                if(memcmp(data_read_hex, data_read_eeprom, flipper_hex_ret.data_size) != 0) {
                    ret = false;
                    FURI_LOG_E(TAG, "Verification eeprom error");
                }

                addr += flipper_hex_ret.data_size;
                instance->progress_flash =
                    (float)addr / (avr_isp_chip_arr[instance->chip_arr_ind].eepromsize);
                break;

            case FlipperI32HexFileStatusUdateAddr:
                addr = (data_read_hex[0] << 24 | data_read_hex[1] << 16);
                break;

            default:
                furi_crash(TAG " Incorrect status.");
                break;
            }

            flipper_hex_ret = flipper_i32hex_file_i32hex_to_bin_get_data(
                flipper_hex_eeprom, data_read_hex, sizeof(data_read_hex));
        }

        avr_isp_end_pmode(instance->avr_isp);
    }
    flipper_i32hex_file_close(flipper_hex_eeprom);
    instance->progress_eeprom = 1.0f;

    return ret;
}

bool avr_isp_worker_rw_verification(
    AvrIspWorkerRW* instance,
    const char* file_path,
    const char* file_name) {
    furi_assert(instance);
    furi_assert(file_path);
    furi_assert(file_name);

    instance->progress_flash = 0.0f;
    instance->progress_eeprom = 0.0f;
    char str_buf[64] = {0};
    bool ret = false;

    do {
        snprintf(
            str_buf, sizeof(str_buf), "%s/%s_flash.hex", instance->file_path, instance->file_name);
        if(!avr_isp_worker_rw_verification_flash(instance, str_buf)) break;
        snprintf(
            str_buf, sizeof(str_buf), "%s/%s_eeprom.hex", instance->file_path, instance->file_name);

        if(!avr_isp_worker_rw_verification_eeprom(instance, str_buf)) break;
        ret = true;
    } while(false);

    return ret;
}

void avr_isp_worker_rw_verification_start(
    AvrIspWorkerRW* instance,
    const char* file_path,
    const char* file_name) {
    furi_assert(instance);
    instance->file_path = file_path;
    instance->file_name = file_name;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), AvrIspWorkerRWEvtVerification);
}

// bool avr_isp_worker_rw_rx1(AvrIspWorkerRW* instance) {
//     furi_assert(instance);
//     uint8_t data[256];

//     Storage* storage = furi_record_open(RECORD_STORAGE);
//     FlipperFormat* flipper_format = flipper_format_file_alloc(storage);
//     FURI_LOG_D(TAG, "Read chip");
//     do {
//         if(!instance->chip_detect) {
//             FURI_LOG_E(TAG, "No detect AVR chip");
//             break;
//         }

//         // if(!storage_simply_mkdir(storage, "/any/avr_isp/")) {
//         //     FURI_LOG_E(TAG, "storage_simply_mkdir");
//         //     break;
//         // }

//         // if(!storage_simply_remove(storage, "dump.avr")) {
//         //     break;
//         // }

//         if(!flipper_format_file_open_always(flipper_format, "/any/avr_isp/dump.avr")) {
//             FURI_LOG_E(TAG, "flipper_format_file_open_always");
//             break;
//         }
//         if(!flipper_format_write_header_cstr(flipper_format, "Flipper dump AVR", 1)) {
//             FURI_LOG_E(TAG, "flipper_format_write_header_cstr");
//             break;
//         }
//         if(!flipper_format_write_comment_cstr(flipper_format, "This is comment")) {
//             FURI_LOG_E(TAG, "flipper_format_write_comment_cstr");
//             break;
//         }
//         //if(!flipper_format_write_string_cstr(file, test_string_key, test_string_data)) break;

//         if(!flipper_format_write_string_cstr(
//                flipper_format, "Chip name", avr_isp_chip_arr[instance->chip_arr_ind].name)) {
//             FURI_LOG_E(TAG, "Chip name");
//             break;
//         }

//         if(!avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
//             FURI_LOG_E(TAG, "Well, I managed to enter the mod program");
//             break;
//         }

//         // avr_isp_read_signature(instance->avr_isp, data);

//         if(!flipper_format_write_hex(flipper_format, "ID", data, 3)) {
//             FURI_LOG_E(TAG, "Unable to add ID");
//             break;
//         }
//         FURI_LOG_D(TAG, "ID = 0x%02X 0x%02X 0x%02X", data[0], data[1], data[2]);

//         data[0] = avr_isp_read_fuse_high(instance->avr_isp);
//         data[1] = avr_isp_read_fuse_low(instance->avr_isp);
//         data[2] = avr_isp_read_fuse_extended(instance->avr_isp);
//         data[3] = avr_isp_read_lock_byte(instance->avr_isp);

//         if(!flipper_format_write_hex(flipper_format, "Hfuse", data, 1)) {
//             FURI_LOG_E(TAG, "Unable to add Hfuse");
//             break;
//         }

//         if(!flipper_format_write_hex(flipper_format, "Lfuse", data + 1, 1)) {
//             FURI_LOG_E(TAG, "Unable to add Lfuse");
//             break;
//         }

//         if(!flipper_format_write_hex(flipper_format, "Efuse", data + 2, 1)) {
//             FURI_LOG_E(TAG, "Unable to add Efuse");
//             break;
//         }

//         if(!flipper_format_write_hex(flipper_format, "Lock", data + 3, 1)) {
//             FURI_LOG_E(TAG, "Unable to add Lock");
//             break;
//         }

//         FURI_LOG_D(
//             TAG,
//             "hfuse = 0x%02X lfuse = 0x%02X efuse = 0x%02X lock = 0x%02X",
//             data[0],
//             data[1],
//             data[2],
//             data[3]);

//         if(!flipper_format_write_string_cstr(flipper_format, "Dump_flash", "flash.hex")) {
//             FURI_LOG_E(TAG, "Unable to add Dump_flash");
//             break;
//         }
//         if(avr_isp_chip_arr[instance->chip_arr_ind].eepromsize > 0) {
//             if(!flipper_format_write_string_cstr(flipper_format, "Dump_eeprom", "eeprom.hex")) {
//                 FURI_LOG_E(TAG, "Unable to add Dump_eeprom");
//                 break;
//             }
//         }

//         FURI_LOG_D(TAG, "EEPROM");

//         if(avr_isp_chip_arr[instance->chip_arr_ind].eepromsize > 0) {
//             FlipperI32HexFile* flipper_hex_eeprom = flipper_i32hex_file_open_write(
//                 "/any/avr_isp/eeprom.hex", avr_isp_chip_arr[instance->chip_arr_ind].eepromoffset);
//             int32_t size_data = 32;
//             if(size_data > avr_isp_chip_arr[instance->chip_arr_ind].eepromsize)
//                 size_data = avr_isp_chip_arr[instance->chip_arr_ind].eepromsize;

//             for(uint16_t i = 0; i < avr_isp_chip_arr[instance->chip_arr_ind].eepromsize;
//                 i += size_data) {
//                 avr_isp_read_page(
//                     instance->avr_isp, STK_SET_EEPROM_TYPE, i, size_data, data, sizeof(data));
//                 flipper_i32hex_file_bin_to_i32hex_set_data(flipper_hex_eeprom, data, size_data);
//                 printf("%s\r\n", flipper_i32hex_file_get_string(flipper_hex_eeprom));
//             }
//             flipper_i32hex_file_bin_to_i32hex_set_end_line(flipper_hex_eeprom);
//             printf("%s\r\n", flipper_i32hex_file_get_string(flipper_hex_eeprom));
//             flipper_i32hex_file_close(flipper_hex_eeprom);
//         }

//         // FURI_LOG_D(TAG, "FLASH");
//         // FlipperI32HexFile* flipper_hex_flash =
//         //     flipper_i32hex_file_open_write("/any/avr_isp/flash.hex", avr_isp_chip_arr[instance->chip_arr_ind].flashoffset;);
//         // for(uint16_t i = 0; i < avr_isp_chip_arr[instance->chip_arr_ind].flashsize / 2;
//         //     i += avr_isp_chip_arr[instance->chip_arr_ind].pagesize / 2) {
//         //     avr_isp_read_page(
//         //         instance->avr_isp,
//         //         STK_SET_FLASH_TYPE,
//         //         i,
//         //         avr_isp_chip_arr[instance->chip_arr_ind].pagesize,
//         //         data,
//         //         sizeof(data));
//         //     flipper_i32hex_file_bin_to_i32hex_add_data(
//         //         flipper_hex_flash, data, avr_isp_chip_arr[instance->chip_arr_ind].pagesize);
//         //     printf("%s\r\n", flipper_i32hex_file_get_string(flipper_hex_flash));
//         // }
//         // flipper_i32hex_file_bin_to_i32hex_add_end_line(flipper_hex_flash);
//         // printf("%s\r\n", flipper_i32hex_file_get_string(flipper_hex_flash));
//         // flipper_i32hex_file_close(flipper_hex_flash);

//         avr_isp_end_pmode(instance->avr_isp);

//         // // //################################################################
//         // FURI_LOG_D(TAG, "FUSE Write");
//         // if(!avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
//         //     FURI_LOG_E(TAG, "Well, I managed to enter the mod program");
//         //     break;
//         // }
//         // FURI_LOG_E(TAG, "fuse %d", avr_isp_write_fuse_low(instance->avr_isp, 0xFF));
//         // FURI_LOG_E(TAG, "fuse %d", avr_isp_write_fuse_extended(instance->avr_isp, 0xFD));
//         // FURI_LOG_E(TAG, "fuse %d", avr_isp_write_fuse_high(instance->avr_isp, 0xDA));
//         // FURI_LOG_E(TAG, "fuse %d", avr_isp_write_lock_byte(instance->avr_isp, 0xFF));

//         // avr_isp_end_pmode(instance->avr_isp);
//         // //##################################################################

//         // //################################################################
//         // FURI_LOG_D(TAG, "ERASE CHIP");
//         // avr_isp_erase_chip(instance->avr_isp);
//         // //##################################################################

// //##########################################################
// FURI_LOG_D(TAG, "FLASH WRITE");

// if(!avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
//     FURI_LOG_E(TAG, "Well, I managed to enter the mod program");
//     break;
// }
// avr_isp_read_signature(instance->avr_isp, data);

// FlipperI32HexFile* flipper_hex_flash = flipper_i32hex_file_open_read("/any/avr_isp/flash.hex");

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
//         addr += flipper_hex_ret.data_size / 2;
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
// ##########################################################

//         //     FURI_LOG_D(TAG, "CHECK HEX");
//         //     FlipperI32HexFile* flipper_hex_eeprom_read =
//         //         flipper_i32hex_file_open_read("/any/avr_isp/a.hex");
//         //     if(flipper_i32hex_file_check(flipper_hex_eeprom_read)) {
//         //         FURI_LOG_D(TAG, "Check OK");
//         //     } else {
//         //         FURI_LOG_E(TAG, "Check ERROR");
//         //     }
//         //    // flipper_i32hex_file_close(flipper_hex_eeprom_read);

//         //     //##########################################################
//         //     FURI_LOG_D(TAG, "EEPROM WRITE");

//         //     if(!avr_isp_auto_set_spi_speed_start_pmode(instance->avr_isp)) {
//         //         FURI_LOG_E(TAG, "Well, I managed to enter the mod program");
//         //         break;
//         //     }
//         //     avr_isp_read_signature(instance->avr_isp, data);
//         //     uint32_t addr = avr_isp_chip_arr[instance->chip_arr_ind].eepromoffset;
//         //     FlipperI32HexFileRet flipper_hex_ret = flipper_i32hex_file_i32hex_to_bin_get_data(
//         //         flipper_hex_eeprom_read, data, sizeof(data));

//         //     while((flipper_hex_ret.status == FlipperI32HexFileStatusData) ||
//         //           (flipper_hex_ret.status == FlipperI32HexFileStatusUdateAddr)) {
//         //         FURI_LOG_D(TAG, "EEPROM WRITE Page");
//         //         for(size_t i = 0; i < flipper_hex_ret.data_size; i++) {
//         //             printf("%02X ", data[i]);
//         //         }
//         //         printf("\r\n");

//         //         switch(flipper_hex_ret.status) {
//         //         case FlipperI32HexFileStatusData:
//         //             if(!avr_isp_write_page(
//         //                    instance->avr_isp,
//         //                    STK_SET_EEPROM_TYPE,
//         //                    avr_isp_chip_arr[instance->chip_arr_ind].eepromsize,
//         //                    addr,
//         //                    avr_isp_chip_arr[instance->chip_arr_ind].eeprompagesize,
//         //                    data,
//         //                    flipper_hex_ret.data_size)) {
//         //                 break;
//         //             }
//         //             addr += flipper_hex_ret.data_size;
//         //             break;

//         //         case FlipperI32HexFileStatusUdateAddr:
//         //             addr = data[0] << 24 | data[1] << 16;
//         //             break;

//         //         default:
//         //             furi_crash(TAG " Incorrect status.");
//         //             break;
//         //         }

//         //         flipper_hex_ret = flipper_i32hex_file_i32hex_to_bin_get_data(
//         //             flipper_hex_eeprom_read, data, sizeof(data));
//         //     }

//         //     flipper_i32hex_file_close(flipper_hex_eeprom_read);
//         //     avr_isp_end_pmode(instance->avr_isp);
//         //     //##########################################################

//         // //################################################################
//         // FURI_LOG_D(TAG, "ERASE CHIP");
//         // avr_isp_erase_chip(instance->avr_isp);
//         // //##################################################################
//         FURI_LOG_D(TAG, "___END___");
//     } while(0);
//     flipper_format_free(flipper_format);
//     furi_record_close(RECORD_STORAGE);
//     return true;
// }