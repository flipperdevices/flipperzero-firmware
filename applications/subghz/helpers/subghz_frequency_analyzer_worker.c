#include "subghz_frequency_analyzer_worker.h"
#include <lib/drivers/cc1101.h>

#include <lib/drivers/si446x_regs.h>
#include <lib/drivers/si446x.h>

#include <furi.h>

#define TAG "SubghzFrequencyAnalyzerWorker"

#define SUBGHZ_FREQUENCY_ANALYZER_THRESHOLD -85.0f

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

#define Si446x_SET_BW_10_08_KHZ_4800bps                                                           \
    {                                                                                             \
        0x0A, 0x11, 0x20, 0x06, 0x1E, 0x72, 0x10, 0x0C, 0xE8, 0x00, 0x41, 0x10, 0x11, 0x21, 0x0C, \
            0x00, 0x39, 0x2B, 0x00, 0xC3, 0x7F, 0x3F, 0x0C, 0xEC, 0xDC, 0xDC, 0xE3, 0xED, 0x10,   \
            0x11, 0x21, 0x0C, 0x0C, 0xF6, 0xFD, 0x15, 0xC0, 0xFF, 0x0F, 0x39, 0x2B, 0x00, 0xC3,   \
            0x7F, 0x3F, 0x10, 0x11, 0x21, 0x0C, 0x18, 0x0C, 0xEC, 0xDC, 0xDC, 0xE3, 0xED, 0xF6,   \
            0xFD, 0x15, 0xC0, 0xFF, 0x0F                                                          \
    }

#define Si446x_SET_BW_57_73_KHZ_4800bps                                                           \
    {                                                                                             \
        0x0A, 0x11, 0x20, 0x06, 0x1E, 0x34, 0x20, 0x0C, 0xE8, 0x00, 0x62, 0x10, 0x11, 0x21, 0x0C, \
            0x00, 0xFF, 0xBA, 0x0F, 0x51, 0xCF, 0xA9, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01, 0x10,   \
            0x11, 0x21, 0x0C, 0x0C, 0xFC, 0xFD, 0x15, 0xFF, 0x00, 0x0F, 0xFF, 0xBA, 0x0F, 0x51,   \
            0xCF, 0xA9, 0x10, 0x11, 0x21, 0x0C, 0x18, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01, 0xFC,   \
            0xFD, 0x15, 0xFF, 0x00, 0x0F                                                          \
    }
const uint8_t subghz_preset_ook_57khz1[] = Si446x_SET_BW_57_73_KHZ_4800bps;

#define Si446x_SET_BW_661_10_KHZ_4800bps                                                          \
    {                                                                                             \
        0x0A, 0x11, 0x20, 0x06, 0x1E, 0x0A, 0x30, 0x0C, 0xE8, 0x00, 0xC3, 0x10, 0x11, 0x21, 0x0C, \
            0x00, 0xA2, 0x81, 0x26, 0xAF, 0x3F, 0xEE, 0xC8, 0xC7, 0xDB, 0xF2, 0x02, 0x08, 0x10,   \
            0x11, 0x21, 0x0C, 0x0C, 0x07, 0x03, 0x15, 0xFC, 0x0F, 0x00, 0xA2, 0x81, 0x26, 0xAF,   \
            0x3F, 0xEE, 0x10, 0x11, 0x21, 0x0C, 0x18, 0xC8, 0xC7, 0xDB, 0xF2, 0x02, 0x08, 0x07,   \
            0x03, 0x15, 0xFC, 0x0F, 0x00                                                          \
    }
const uint8_t subghz_preset_ook_650khz1[] = Si446x_SET_BW_661_10_KHZ_4800bps;

#define Si446x_SET_BW_915_70_KHZ_4800bps                                                          \
    {                                                                                             \
        0x0A, 0x11, 0x20, 0x06, 0x1E, 0x0A, 0x30, 0x0C, 0xE8, 0x00, 0xC3, 0x10, 0x11, 0x21, 0x0C, \
            0x00, 0xFF, 0xBA, 0x0F, 0x51, 0xCF, 0xA9, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01, 0x10,   \
            0x11, 0x21, 0x0C, 0x0C, 0xFC, 0xFD, 0x15, 0xFF, 0x00, 0x0F, 0xFF, 0xBA, 0x0F, 0x51,   \
            0xCF, 0xA9, 0x10, 0x11, 0x21, 0x0C, 0x18, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01, 0xFC,   \
            0xFD, 0x15, 0xFF, 0x00, 0x0F                                                          \
    }

#define Si446x_SET_BW_2_15_KHZ_500bps                                                             \
    {                                                                                             \
        0x0A, 0x11, 0x20, 0x06, 0x1E, 0xF2, 0x10, 0x34, 0xE8, 0x00, 0x4E, 0x09, 0x11, 0x20, 0x05, \
            0x24, 0x06, 0x8D, 0xB9, 0x03, 0x48, 0x07, 0x11, 0x20, 0x03, 0x30, 0x03, 0x0E, 0x80,   \
            0x05, 0x11, 0x20, 0x01, 0x35, 0x68, 0x0D, 0x11, 0x20, 0x09, 0x38, 0x11, 0x11, 0x11,   \
            0x80, 0x02, 0xFF, 0xFF, 0x00, 0x28, 0x0E, 0x11, 0x20, 0x0A, 0x45, 0x8C, 0x07, 0xFF,   \
            0x01, 0x00, 0x80, 0x06, 0x02, 0x18, 0x40, 0x10, 0x11, 0x21, 0x0C, 0x00, 0xFF, 0xC4,   \
            0x30, 0x7F, 0xF5, 0xB5, 0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C, 0x10, 0x11, 0x21, 0x0C,   \
            0x0C, 0x03, 0x00, 0x15, 0xFF, 0x00, 0x00, 0xFF, 0xC4, 0x30, 0x7F, 0xF5, 0xB5, 0x10,   \
            0x11, 0x21, 0x0C, 0x18, 0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C, 0x03, 0x00, 0x15, 0xFF,   \
            0x00, 0x00,                                                                           \
    }

#define Si446x_SET_BW_57_23_KHZ_500bps                                                            \
    {                                                                                             \
        0x0A, 0x11, 0x20, 0x06, 0x1E, 0x3A, 0x20, 0x14, 0xE8, 0x00, 0x75, 0x09, 0x11, 0x20, 0x05, \
            0x24, 0x04, 0x5E, 0x7B, 0x02, 0x30, 0x07, 0x11, 0x20, 0x03, 0x30, 0x4A, 0x3A, 0x80,   \
            0x05, 0x11, 0x20, 0x01, 0x35, 0x68, 0x0D, 0x11, 0x20, 0x09, 0x38, 0x11, 0x1A, 0x1A,   \
            0x80, 0x02, 0xFF, 0xFF, 0x00, 0x28, 0x0E, 0x11, 0x20, 0x0A, 0x45, 0x8D, 0x02, 0xAA,   \
            0x01, 0x00, 0x80, 0x06, 0x02, 0x18, 0x40, 0x10, 0x11, 0x21, 0x0C, 0x00, 0xFF, 0xBA,   \
            0x0F, 0x51, 0xCF, 0xA9, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01, 0x10, 0x11, 0x21, 0x0C,   \
            0x0C, 0xFC, 0xFD, 0x15, 0xFF, 0x00, 0x0F, 0xFF, 0xBA, 0x0F, 0x51, 0xCF, 0xA9, 0x10,   \
            0x11, 0x21, 0x0C, 0x18, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01, 0xFC, 0xFD, 0x15, 0xFF,   \
            0x00, 0x0F,                                                                           \
    }

#define Si446x_SET_BW_661_10_KHZ_500bps                                                           \
    {                                                                                             \
        0x0A, 0x11, 0x20, 0x06, 0x1E, 0x0A, 0x30, 0x14, 0xE8, 0x07, 0x53, 0x09, 0x11, 0x20, 0x05, \
            0x24, 0x00, 0x45, 0xE8, 0x00, 0x23, 0x07, 0x11, 0x20, 0x03, 0x30, 0xA9, 0xF9, 0x80,   \
            0x05, 0x11, 0x20, 0x01, 0x35, 0x6A, 0x0D, 0x11, 0x20, 0x09, 0x38, 0x11, 0xFF, 0xFF,   \
            0x80, 0x02, 0xFF, 0xFF, 0x00, 0x2B, 0x0E, 0x11, 0x20, 0x0A, 0x45, 0x8F, 0x00, 0xAA,   \
            0x01, 0x00, 0x80, 0x08, 0x02, 0x18, 0x40, 0x10, 0x11, 0x21, 0x0C, 0x00, 0xA2, 0x81,   \
            0x26, 0xAF, 0x3F, 0xEE, 0xC8, 0xC7, 0xDB, 0xF2, 0x02, 0x08, 0x10, 0x11, 0x21, 0x0C,   \
            0x0C, 0x07, 0x03, 0x15, 0xFC, 0x0F, 0x00, 0xA2, 0x81, 0x26, 0xAF, 0x3F, 0xEE, 0x10,   \
            0x11, 0x21, 0x0C, 0x18, 0xC8, 0xC7, 0xDB, 0xF2, 0x02, 0x08, 0x07, 0x03, 0x15, 0xFC,   \
            0x0F, 0x00,                                                                           \
    }

#define Si446x_SET_BW_824_50_KHZ_500bps                                                           \
    {                                                                                             \
        0x0A, 0x11, 0x20, 0x06, 0x1E, 0x0A, 0x30, 0x14, 0xE8, 0x07, 0x53, 0x09, 0x11, 0x20, 0x05, \
            0x24, 0x00, 0x45, 0xE8, 0x00, 0x23, 0x07, 0x11, 0x20, 0x03, 0x30, 0xC0, 0x97, 0x80,   \
            0x05, 0x11, 0x20, 0x01, 0x35, 0x6A, 0x0D, 0x11, 0x20, 0x09, 0x38, 0x11, 0xFF, 0xFF,   \
            0x80, 0x02, 0xFF, 0xFF, 0x00, 0x2B, 0x0E, 0x11, 0x20, 0x0A, 0x45, 0x8F, 0x00, 0xAA,   \
            0x01, 0x00, 0x80, 0x08, 0x02, 0x18, 0x40, 0x10, 0x11, 0x21, 0x0C, 0x00, 0xFF, 0xC4,   \
            0x30, 0x7F, 0xF5, 0xB5, 0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C, 0x10, 0x11, 0x21, 0x0C,   \
            0x0C, 0x03, 0x00, 0x15, 0xFF, 0x00, 0x00, 0xFF, 0xC4, 0x30, 0x7F, 0xF5, 0xB5, 0x10,   \
            0x11, 0x21, 0x0C, 0x18, 0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C, 0x03, 0x00, 0x15, 0xFF,   \
            0x00, 0x00,                                                                           \
    }
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
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650AsyncFreq);
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
                //check + - 0.5 MHz from the required frequency, step 10KHz
                freq -= 250000;
                for(size_t i = 0; i < 50; i++) {
                    si446x_set_frequency_and_step_channel(
                        &furi_hal_spi_bus_handle_subghz, freq + step * i, step);
                    si446x_switch_to_start_rx(
                        &furi_hal_spi_bus_handle_subghz, 0, SI446X_STATE_NOCHANGE, 0);

                    //if(furi_hal_gpio_read(&gpio_cc1101_g0)) {
                        osDelay(2);
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
                            frequency_rssi.frequency = freq + step * i;
                            frequency_rssi.channel = i;
                        }
                    //}
                    //printf("%ld, %d\r\n", freq + step * i, (int)rssi);
                    printf("%d\r\n", (int)rssi);
                    rssi = -127;
                }
            }
        }

        if(frequency_rssi.rssi > SUBGHZ_FREQUENCY_ANALYZER_THRESHOLD) {
            instance->sample_hold_counter = 10;
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
