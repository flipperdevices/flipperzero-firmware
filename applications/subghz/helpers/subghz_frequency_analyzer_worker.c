#include "subghz_frequency_analyzer_worker.h"
#include <lib/drivers/cc1101.h>

#include <lib/drivers/si446x_regs.h>
#include <lib/drivers/si446x.h>

#include <furi.h>

#define TAG "SubghzFrequencyAnalyzerWorker"

#define SUBGHZ_FREQUENCY_ANALYZER_THRESHOLD -75.0f

//#define CC1101
#ifdef CC1101

static const uint8_t subghz_preset_ook_58khz[][2] = {
    {CC1101_MDMCFG4, 0b11110111}, // Rx BW filter is 58.035714kHz
    /* End  */
    {0, 0},
};

static const uint8_t subghz_preset_ook_650khz[][2] = {
    {CC1101_MDMCFG4, 0b00010111}, // Rx BW filter is 650.000kHz
    /* End  */
    {0, 0},
};
#else

#endif

struct SubGhzFrequencyAnalyzerWorker {
    FuriThread* thread;

    volatile bool worker_running;
    uint8_t sample_hold_counter;
    FrequencyRSSI frequency_rssi_buf;
    SubGhzSetting* setting;

    float filVal;

    SubGhzFrequencyAnalyzerWorkerPairCallback pair_callback;
    void* context;
};

#ifdef CC1101
static void subghz_frequency_analyzer_worker_load_registers(const uint8_t data[][2]) {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    size_t i = 0;
    while(data[i][0]) {
        cc1101_write_reg(&furi_hal_spi_bus_handle_subghz, data[i][0], data[i][1]);
        i++;
    }
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}
#else
static bool subghz_frequency_analyzer_worker_is_valid(uint32_t freq_hz) {
    // See Si446x Data Sheet section 5.3.1

    if(!(freq_hz >= 142000000 && freq_hz <= 175000000) &&
       !(freq_hz >= 284000000 && freq_hz <= 525000000) &&
       !(freq_hz >= 850000000 && freq_hz <= 1050000000)) {
        return false;
    }
    return true;
}

static FuriHalSubGhzPath subghz_frequency_analyzer_worker_get_path(uint32_t freq_hz) {
    FuriHalSubGhzPath ret = FuriHalSubGhzPathIsolate;
    if(freq_hz >= 142000000 && freq_hz <= 175000000) {
        ret = FuriHalSubGhzPath315;
    } else if(freq_hz >= 284000000 && freq_hz <= 386999999) {
        //ret = FuriHalSubGhzPath315;
        ret = FuriHalSubGhzPath433;
    } else if(freq_hz >= 387000000 && freq_hz <= 525000000) {
        ret = FuriHalSubGhzPath433;
    } else if(freq_hz >= 850000000 && freq_hz <= 1050000000) {
        ret = FuriHalSubGhzPath868;
    } else {
        furi_crash(NULL);
    }
    return ret;
}

#endif

// running average with adaptive coefficient
static uint32_t subghz_frequency_analyzer_worker_expRunningAverageAdaptive(
    SubGhzFrequencyAnalyzerWorker* instance,
    uint32_t newVal) {
    float k;
    float newValFloat = newVal;
    // the sharpness of the filter depends on the absolute value of the difference
    if(fabs(newValFloat - instance->filVal) > 500000)
        k = 0.9;
    else
        k = 0.03;

    instance->filVal += (newValFloat - instance->filVal) * k;
    return (uint32_t)instance->filVal;
}

/** Worker thread
 * 
 * @param context 
 * @return exit code 
 */
static int32_t subghz_frequency_analyzer_worker_thread(void* context) {
    SubGhzFrequencyAnalyzerWorker* instance = context;

    FrequencyRSSI frequency_rssi = {.frequency = 0, .rssi = 0, .channel = 0};
    float rssi = 0;

#ifdef CC1101
    uint32_t frequency = 0;
    CC1101Status status;
    //Start CC1101
    furi_hal_subghz_reset();

    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    cc1101_flush_rx(&furi_hal_spi_bus_handle_subghz);
    cc1101_flush_tx(&furi_hal_spi_bus_handle_subghz);
    cc1101_write_reg(&furi_hal_spi_bus_handle_subghz, CC1101_IOCFG0, CC1101IocfgHW);
    cc1101_write_reg(&furi_hal_spi_bus_handle_subghz, CC1101_MDMCFG3,
                     0b01111111); // symbol rate
    cc1101_write_reg(
        &furi_hal_spi_bus_handle_subghz,
        CC1101_AGCCTRL2,
        0b00000111); // 00 - DVGA all; 000 - MAX LNA+LNA2; 111 - MAGN_TARGET 42 dB
    cc1101_write_reg(
        &furi_hal_spi_bus_handle_subghz,
        CC1101_AGCCTRL1,
        0b00001000); // 0; 0 - LNA 2 gain is decreased to minimum before decreasing LNA gain; 00 - Relative carrier sense threshold disabled; 1000 - Absolute carrier sense threshold disabled
    cc1101_write_reg(
        &furi_hal_spi_bus_handle_subghz,
        CC1101_AGCCTRL0,
        0b00110000); // 00 - No hysteresis, medium asymmetric dead zone, medium gain ; 11 - 64 samples agc; 00 - Normal AGC, 00 - 4dB boundary

    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);

    furi_hal_subghz_set_path(FuriHalSubGhzPathIsolate);

    while(instance->worker_running) {
        osDelay(10);

        float rssi_min = 26.0f;
        float rssi_avg = 0;
        size_t rssi_avg_samples = 0;

        frequency_rssi.rssi = -127.0f;
        furi_hal_subghz_idle();
        subghz_frequency_analyzer_worker_load_registers(subghz_preset_ook_650khz);

        // First stage: coarse scan
        for(size_t i = 0; i < subghz_setting_get_frequency_count(instance->setting); i++) {
            if(furi_hal_subghz_is_frequency_valid(
                   subghz_setting_get_frequency(instance->setting, i))) {
                furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
                cc1101_switch_to_idle(&furi_hal_spi_bus_handle_subghz);
                frequency = cc1101_set_frequency(
                    &furi_hal_spi_bus_handle_subghz,
                    subghz_setting_get_frequency(instance->setting, i));

                cc1101_calibrate(&furi_hal_spi_bus_handle_subghz);
                do {
                    status = cc1101_get_status(&furi_hal_spi_bus_handle_subghz);
                } while(status.STATE != CC1101StateIDLE);

                cc1101_switch_to_rx(&furi_hal_spi_bus_handle_subghz);
                furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);

                // delay will be in range between 1 and 2ms
                osDelay(3);

                rssi = furi_hal_subghz_get_rssi();

                rssi_avg += rssi;
                rssi_avg_samples++;

                if(rssi < rssi_min) rssi_min = rssi;

                if(frequency_rssi.rssi < rssi) {
                    frequency_rssi.rssi = rssi;
                    frequency_rssi.frequency = frequency;
                }
            }
        }

        FURI_LOG_T(
            TAG,
            "RSSI: avg %f, max %f at %u, min %f",
            (double)(rssi_avg / rssi_avg_samples),
            (double)frequency_rssi.rssi,
            frequency_rssi.frequency,
            (double)rssi_min);

        // Second stage: fine scan
        if(frequency_rssi.rssi > SUBGHZ_FREQUENCY_ANALYZER_THRESHOLD) {
            FURI_LOG_D(TAG, "~:%u:%f", frequency_rssi.frequency, (double)frequency_rssi.rssi);

            frequency_rssi.rssi = -127.0;
            furi_hal_subghz_idle();
            subghz_frequency_analyzer_worker_load_registers(subghz_preset_ook_58khz);
            //-0.3 ... 433.92 ... +0.3 step 10KHz
            for(uint32_t i = frequency_rssi.frequency - 300000;
                i < frequency_rssi.frequency + 300000;
                i += 20000) {
                if(furi_hal_subghz_is_frequency_valid(i)) {
                    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
                    cc1101_switch_to_idle(&furi_hal_spi_bus_handle_subghz);
                    frequency = cc1101_set_frequency(&furi_hal_spi_bus_handle_subghz, i);

                    cc1101_calibrate(&furi_hal_spi_bus_handle_subghz);
                    do {
                        status = cc1101_get_status(&furi_hal_spi_bus_handle_subghz);
                    } while(status.STATE != CC1101StateIDLE);

                    cc1101_switch_to_rx(&furi_hal_spi_bus_handle_subghz);
                    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);

                    // delay will be in range between 1 and 2ms
                    osDelay(3);

                    rssi = furi_hal_subghz_get_rssi();

                    FURI_LOG_T(TAG, "#:%u:%f", frequency, (double)rssi);

                    if(frequency_rssi.rssi < rssi) {
                        frequency_rssi.rssi = rssi;
                        frequency_rssi.frequency = frequency;
                    }
                }
            }
        }

        // Deliver results
        if(frequency_rssi.rssi > SUBGHZ_FREQUENCY_ANALYZER_THRESHOLD) {
            FURI_LOG_D(TAG, "=:%u:%f", frequency_rssi.frequency, (double)frequency_rssi.rssi);

            instance->sample_hold_counter = 20;
            if(instance->filVal) {
                frequency_rssi.frequency =
                    subghz_frequency_analyzer_worker_expRunningAverageAdaptive(
                        instance, frequency_rssi.frequency);
            }
            // Deliver callback
            if(instance->pair_callback) {
                instance->pair_callback(
                    instance->context, frequency_rssi.frequency, frequency_rssi.rssi);
            }
        } else {
            if(instance->sample_hold_counter > 0) {
                instance->sample_hold_counter--;
            } else {
                instance->filVal = 0;
                if(instance->pair_callback) instance->pair_callback(instance->context, 0, 0);
            }
        }
    }

    //Stop CC1101
    furi_hal_subghz_idle();
    furi_hal_subghz_sleep();
#else
    uint32_t freq = 433920000;
    uint32_t step = 10000;
    FuriHalSubGhzPath path = FuriHalSubGhzPathIsolate;
    //float rssi1 = 0;

    //Start Si4463
    furi_hal_subghz_reset();
    //furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650AsyncFreq);
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);

    furi_hal_subghz_idle();
    //furi_hal_subghz_set_path(FuriHalSubGhzPath433);

    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullDown, GpioSpeedLow);
    si446x_write_gpio(&furi_hal_spi_bus_handle_subghz, SI446X_GPIO1, SI446X_GPIO_MODE_CCA);
    //si446x_write_gpio(&furi_hal_spi_bus_handle_subghz, SI446X_NIRQ, SI446X_GPIO_MODE_CCA_LATCH);
    //si446x_write_gpio(&furi_hal_spi_bus_handle_subghz, SI446X_GPIO1, SI446X_GPIO_MODE_RX_DATA);

    uint8_t modem_rssi_comp = 0;
    si446x_get_properties(
        &furi_hal_spi_bus_handle_subghz, SI446X_PROP_MODEM_RSSI_COMP, &modem_rssi_comp, 1);
    FURI_LOG_D(TAG, "modem_rssi_comp = 0x%X", modem_rssi_comp);

    uint8_t modem_rssi_thresh = {0x70};
    si446x_set_properties(
        &furi_hal_spi_bus_handle_subghz, SI446X_PROP_MODEM_RSSI_THRESH, &modem_rssi_thresh, 1);
    modem_rssi_thresh = 0;
    si446x_set_bps(&furi_hal_spi_bus_handle_subghz, 200000);
    si446x_get_properties(
        &furi_hal_spi_bus_handle_subghz, SI446X_PROP_MODEM_RSSI_THRESH, &modem_rssi_thresh, 1);
    FURI_LOG_D(TAG, "modem_rssi_thresh = 0x%X", modem_rssi_thresh);

    uint8_t rssi_control = 0b00010011;
    si446x_set_properties(
        &furi_hal_spi_bus_handle_subghz, SI446X_PROP_MODEM_RSSI_CONTROL, &rssi_control, 1);
    rssi_control = 0;
    si446x_get_properties(
        &furi_hal_spi_bus_handle_subghz, SI446X_PROP_MODEM_RSSI_CONTROL, &rssi_control, 1);
    FURI_LOG_D(TAG, "rssi_control = 0x%X", rssi_control);

    uint8_t modem_fast_delay = 0xFF;
    si446x_set_properties(
        &furi_hal_spi_bus_handle_subghz, SI446X_PROP_MODEM_FAST_RSSI_DELAY, &modem_fast_delay, 1);
    FURI_LOG_D(TAG, "rssi_control = 0x%X", modem_fast_delay);

    while(instance->worker_running) {
        osDelay(10);
        frequency_rssi.rssi = -127.0f;

        for(size_t freq_ind = 0; freq_ind < subghz_setting_get_frequency_count(instance->setting);
            freq_ind++) {
            if(subghz_frequency_analyzer_worker_is_valid(
                   subghz_setting_get_frequency(instance->setting, freq_ind))) {
                //get freq from setting
                freq = subghz_setting_get_frequency(instance->setting, freq_ind);
                //set path if need;
                if(path != subghz_frequency_analyzer_worker_get_path(freq)) {
                    path = subghz_frequency_analyzer_worker_get_path(freq);
                    furi_hal_subghz_set_path(path);
                }
                si446x_set_frequency_and_step_channel(&furi_hal_spi_bus_handle_subghz, freq, step);
                si446x_switch_to_start_rx(
                    &furi_hal_spi_bus_handle_subghz, 0, SI446X_STATE_NOCHANGE, 0);

                //if(furi_hal_gpio_read(&gpio_cc1101_g0)) {
                osDelay(4);
                // rssi = ((float)si446x_get_rssi(&furi_hal_spi_bus_handle_subghz) / 2.0f) -
                //        modem_rssi_comp - 70;
                // rssi1 = ((float)si446x_get_lqi(&furi_hal_spi_bus_handle_subghz) / 2.0f) -
                //         modem_rssi_comp - 70;
                rssi = ((float)si446x_get_fast_reg(
                            &furi_hal_spi_bus_handle_subghz, SI446X_CMD_FRR_A_READ) /
                        2.0f) -
                       modem_rssi_comp - 70;

                if(frequency_rssi.rssi < rssi) {
                    frequency_rssi.rssi = rssi;
                    frequency_rssi.frequency = freq;
                    frequency_rssi.channel = freq_ind;
                }
                //}
                //printf("%ld, %d\r\n", freq + step * i, (int)rssi);
                printf("%d\r\n", (int)rssi);
                rssi = -127;
            }
        }

        // for(size_t freq_ind = 0; freq_ind < subghz_setting_get_frequency_count(instance->setting);
        //     freq_ind++) {
        //     if(subghz_frequency_analyzer_worker_is_valid(
        //            subghz_setting_get_frequency(instance->setting, freq_ind))) {
        //         //get freq from setting
        //         freq = subghz_setting_get_frequency(instance->setting, freq_ind);
        //         //set path if need;
        //         if(path != subghz_frequency_analyzer_worker_get_path(freq)) {
        //             path = subghz_frequency_analyzer_worker_get_path(freq);
        //             furi_hal_subghz_set_path(path);
        //         }
        //         //check + - 0.5 MHz from the required frequency, step 10KHz
        //         freq -= 250000;
        //         for(size_t i = 0; i < 50; i++) {
        //             si446x_set_frequency_and_step_channel(
        //                 &furi_hal_spi_bus_handle_subghz, freq + step * i, step);
        //             si446x_switch_to_start_rx(
        //                 &furi_hal_spi_bus_handle_subghz, 0, SI446X_STATE_NOCHANGE, 0);

        //             //if(furi_hal_gpio_read(&gpio_cc1101_g0)) {
        //                 osDelay(2);
        //                 // rssi = ((float)si446x_get_rssi(&furi_hal_spi_bus_handle_subghz) / 2.0f) -
        //                 //        modem_rssi_comp - 70;
        //                 // rssi1 = ((float)si446x_get_lqi(&furi_hal_spi_bus_handle_subghz) / 2.0f) -
        //                 //         modem_rssi_comp - 70;
        //                 rssi = ((float)si446x_get_fast_reg(
        //                             &furi_hal_spi_bus_handle_subghz, SI446X_CMD_FRR_A_READ) /
        //                         2.0f) -
        //                        modem_rssi_comp - 70;

        //                 if(frequency_rssi.rssi < rssi) {
        //                     frequency_rssi.rssi = rssi;
        //                     frequency_rssi.frequency = freq + step * i;
        //                     frequency_rssi.channel = i;
        //                 }
        //             //}
        //             //printf("%ld, %d\r\n", freq + step * i, (int)rssi);
        //             printf("%d\r\n", (int)rssi);
        //             rssi = -127;
        //         }
        //     }
        // }

        if(frequency_rssi.rssi > SUBGHZ_FREQUENCY_ANALYZER_THRESHOLD) {
            instance->sample_hold_counter = 20;
            if(instance->filVal) {
                frequency_rssi.frequency =
                    subghz_frequency_analyzer_worker_expRunningAverageAdaptive(
                        instance, frequency_rssi.frequency);
            }
            if(instance->pair_callback)
                instance->pair_callback(
                    instance->context, frequency_rssi.frequency, frequency_rssi.rssi);

        } else {
            if(instance->sample_hold_counter > 0) {
                instance->sample_hold_counter--;
            } else {
                instance->filVal = 0;
                if(instance->pair_callback) instance->pair_callback(instance->context, 0, 0);
            }
        }
    }
    //Stop Si4463
    furi_hal_subghz_idle();
    furi_hal_subghz_sleep();
#endif

    return 0;
}

SubGhzFrequencyAnalyzerWorker* subghz_frequency_analyzer_worker_alloc(void* context) {
    furi_assert(context);
    SubGhzFrequencyAnalyzerWorker* instance = malloc(sizeof(SubGhzFrequencyAnalyzerWorker));

    instance->thread = furi_thread_alloc();
    furi_thread_set_name(instance->thread, "SubGhzFAWorker");
    furi_thread_set_stack_size(instance->thread, 2048);
    furi_thread_set_context(instance->thread, instance);
    furi_thread_set_callback(instance->thread, subghz_frequency_analyzer_worker_thread);

    SubGhz* subghz = context;
    instance->setting = subghz->setting;
    return instance;
}

void subghz_frequency_analyzer_worker_free(SubGhzFrequencyAnalyzerWorker* instance) {
    furi_assert(instance);

    furi_thread_free(instance->thread);
    free(instance);
}

void subghz_frequency_analyzer_worker_set_pair_callback(
    SubGhzFrequencyAnalyzerWorker* instance,
    SubGhzFrequencyAnalyzerWorkerPairCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(context);
    instance->pair_callback = callback;
    instance->context = context;
}

void subghz_frequency_analyzer_worker_start(SubGhzFrequencyAnalyzerWorker* instance) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);

    instance->worker_running = true;

    furi_thread_start(instance->thread);
}

void subghz_frequency_analyzer_worker_stop(SubGhzFrequencyAnalyzerWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_running);

    instance->worker_running = false;

    furi_thread_join(instance->thread);
}

bool subghz_frequency_analyzer_worker_is_running(SubGhzFrequencyAnalyzerWorker* instance) {
    furi_assert(instance);
    return instance->worker_running;
}
