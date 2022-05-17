#include "subghz_frequency_analyzer_worker.h"
#include <lib/drivers/cc112x.h>

#include <furi.h>

#include "../subghz_i.h"

#define SUBGHZ_FREQUENCY_ANALYZER_THRESHOLD -70.0f

struct SubGhzFrequencyAnalyzerWorker {
    FuriThread* thread;

    volatile bool worker_running;
    uint8_t count_repet;
    FrequencyRSSI frequency_rssi_buf;
    SubGhzSetting* setting;

    float filVal;

    SubGhzFrequencyAnalyzerWorkerPairCallback pair_callback;
    void* context;
};

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

    FrequencyRSSI frequency_rssi = {.frequency = 0, .rssi = 0};
    float rssi;
    uint32_t frequency;
    uint32_t frequency_start;
    uint8_t timeout = 0;

    //Start CC1120
    furi_hal_subghz_reset();

    furi_hal_subghz_set_frequency(433920000);
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG0, CC112XIocfgRSSIValid);
    cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_AGC_CFG3, 0b11100000);
    cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_AGC_CFG2, 0b11000000);
    cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_AGC_CFG1, 0b00010111);
    cc112x_write_reg(
        &furi_hal_spi_bus_handle_subghz,
        CC112X_AGC_CFG0,
        0b00110000); //RSSI - 2 samples, AGC_ASK_DECAY - 600 samples
    cc112x_write_reg(
        &furi_hal_spi_bus_handle_subghz, CC112X_FS_SPARE, 0xAC); // Frequency Synthesizer Spare

    //ToDo whether it is necessary?
    // cc112x_calibrate(&furi_hal_spi_bus_handle_subghz);
    // while(true) {
    //     CC112XStatus status = cc112x_get_status(&furi_hal_spi_bus_handle_subghz);
    //     if(status.STATE == CC112XStateIDLE) break;
    // }

    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);

    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);

    furi_hal_subghz_flush_rx();
    furi_hal_subghz_set_path(FuriHalSubGhzPathIsolate);
    furi_hal_subghz_rx();

    while(instance->worker_running) {
        osDelay(10);
        frequency_rssi.rssi = -128.0f;
        furi_hal_subghz_idle();

        furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
        cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_CHAN_BW, 0b00000001); //200kHz
        furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);

        for(size_t i = 0; i < subghz_setting_get_frequency_count(instance->setting); i++) {
            if(furi_hal_subghz_is_frequency_valid(
                   subghz_setting_get_frequency(instance->setting, i))) {
                furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
                cc112x_switch_to_idle(&furi_hal_spi_bus_handle_subghz);
                frequency = cc112x_set_frequency(
                    &furi_hal_spi_bus_handle_subghz,
                    subghz_setting_get_frequency(instance->setting, i));
                cc112x_switch_to_rx(&furi_hal_spi_bus_handle_subghz);
                furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);

                timeout = 10;
                do {
                    osDelay(1);
                    timeout--;
                } while(!furi_hal_gpio_read(&gpio_cc1101_g0) && timeout);

                rssi = furi_hal_subghz_get_rssi();
                if(frequency_rssi.rssi < rssi) {
                    frequency_rssi.rssi = rssi;
                    frequency_rssi.frequency = frequency;
                }
            }
        }

        if(frequency_rssi.rssi > SUBGHZ_FREQUENCY_ANALYZER_THRESHOLD) {
            //  -0.5 ... 433.92 ... +0.5
            frequency_start = frequency_rssi.frequency - 500000;

            frequency_rssi.rssi = -128.0;
            furi_hal_subghz_idle();

            furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
            //cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_CHAN_BW, 0b00000100); //50kHz
            cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_CHAN_BW, 0b00001000); //25kHz
            furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);

            //step 25KHz
            for(uint32_t i = frequency_start; i < frequency_start + 500000; i += 25000) {
                if(furi_hal_subghz_is_frequency_valid(i)) {
                    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
                    cc112x_switch_to_idle(&furi_hal_spi_bus_handle_subghz);
                    frequency = cc112x_set_frequency(&furi_hal_spi_bus_handle_subghz, i);
                    cc112x_switch_to_rx(&furi_hal_spi_bus_handle_subghz);
                    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);

                    timeout = 30;
                    do {
                        osDelay(1);
                        timeout--;
                    } while(!furi_hal_gpio_read(&gpio_cc1101_g0) && timeout);

                    rssi = furi_hal_subghz_get_rssi();
                    if(frequency_rssi.rssi < rssi) {
                        frequency_rssi.rssi = rssi;
                        frequency_rssi.frequency = frequency;
                    }
                }
            }
        }

        if(frequency_rssi.rssi > SUBGHZ_FREQUENCY_ANALYZER_THRESHOLD) {
            instance->count_repet = 20;
            if(instance->filVal) {
                frequency_rssi.frequency =
                    subghz_frequency_analyzer_worker_expRunningAverageAdaptive(
                        instance, frequency_rssi.frequency);
            }
            if(instance->pair_callback)
                instance->pair_callback(
                    instance->context, frequency_rssi.frequency, frequency_rssi.rssi);

        } else {
            if(instance->count_repet > 0) {
                instance->count_repet--;
            } else {
                instance->filVal = 0;
                if(instance->pair_callback) instance->pair_callback(instance->context, 0, 0);
            }
        }
    }

    //Stop CC1120
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_subghz_idle();
    furi_hal_subghz_sleep();

    return 0;
}

SubGhzFrequencyAnalyzerWorker* subghz_frequency_analyzer_worker_alloc() {
    SubGhzFrequencyAnalyzerWorker* instance = malloc(sizeof(SubGhzFrequencyAnalyzerWorker));

    instance->thread = furi_thread_alloc();
    furi_thread_set_name(instance->thread, "SubGhzFAWorker");
    furi_thread_set_stack_size(instance->thread, 2048);
    furi_thread_set_context(instance->thread, instance);
    furi_thread_set_callback(instance->thread, subghz_frequency_analyzer_worker_thread);

    instance->setting = subghz_setting_alloc();
    subghz_setting_load(instance->setting, "/ext/subghz/assets/setting_frequency_analyzer_user");
    return instance;
}

void subghz_frequency_analyzer_worker_free(SubGhzFrequencyAnalyzerWorker* instance) {
    furi_assert(instance);

    furi_thread_free(instance->thread);
    subghz_setting_free(instance->setting);
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
