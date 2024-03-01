#include "imu_mouse.h"
#include <furi_hal.h>
#include <furi.h>
#include "sensors/ICM42688P.h"

#define TAG "IMU"

#define ACCEL_GYRO_RATE DataRate1kHz

#define FILTER_SAMPLE_FREQ 1000.f
#define FILTER_BETA 0.08f

#define SCROLL_RATE_DIV 50
#define SCROLL_SENSITIVITY_K 0.25f
#define MOUSE_SENSITIVITY_K 30.f
#define EXP_RATE 1.1f

#define IMU_CALI_AVG 64

typedef enum {
    ImuMouseStop = (1 << 0),
    ImuMouseNewData = (1 << 1),
    ImuMouseRightPress = (1 << 2),
    ImuMouseRightRelease = (1 << 3),
    ImuMouseLeftPress = (1 << 4),
    ImuMouseLeftRelease = (1 << 5),
    ImuMouseScrollOn = (1 << 6),
    ImuMouseScrollOff = (1 << 7),
} ImuThreadFlags;

#define FLAGS_ALL                                                                 \
    (ImuMouseStop | ImuMouseNewData | ImuMouseRightPress | ImuMouseRightRelease | \
     ImuMouseLeftPress | ImuMouseLeftRelease | ImuMouseScrollOn | ImuMouseScrollOff)

typedef struct {
    float q0;
    float q1;
    float q2;
    float q3;
    float roll;
    float pitch;
    float yaw;
} ImuProcessedData;

struct ImuThread {
    FuriThread* thread;
    ICM42688P* icm42688p;
    const ImuHidApi* hid;
    void* hid_inst;
    ImuProcessedData processed_data;
};

static void imu_madgwick_filter(
    ImuProcessedData* out,
    ICM42688PScaledData* accel,
    ICM42688PScaledData* gyro);

static void imu_irq_callback(void* context) {
    furi_assert(context);
    ImuThread* imu = context;
    furi_thread_flags_set(furi_thread_get_id(imu->thread), ImuMouseNewData);
}

static void imu_process_data(ImuThread* imu, ICM42688PFifoPacket* in_data) {
    ICM42688PScaledData accel_data;
    ICM42688PScaledData gyro_data;

    // Get accel and gyro data in g and degrees/s
    icm42688p_apply_scale_fifo(imu->icm42688p, in_data, &accel_data, &gyro_data);

    // Gyro: degrees/s to rads/s
    gyro_data.x = gyro_data.x / 180.f * M_PI;
    gyro_data.y = gyro_data.y / 180.f * M_PI;
    gyro_data.z = gyro_data.z / 180.f * M_PI;

    // Sensor Fusion algorithm
    ImuProcessedData* out = &imu->processed_data;
    imu_madgwick_filter(out, &accel_data, &gyro_data);

    // Quaternion to euler angles
    float roll = atan2f(
        out->q0 * out->q1 + out->q2 * out->q3, 0.5f - out->q1 * out->q1 - out->q2 * out->q2);
    float pitch = asinf(-2.0f * (out->q1 * out->q3 - out->q0 * out->q2));
    float yaw = atan2f(
        out->q1 * out->q2 + out->q0 * out->q3, 0.5f - out->q2 * out->q2 - out->q3 * out->q3);

    // Euler angles: rads to degrees
    out->roll = roll / M_PI * 180.f;
    out->pitch = pitch / M_PI * 180.f;
    out->yaw = yaw / M_PI * 180.f;
}

static void calibrate_gyro(ImuThread* imu) {
    ICM42688PRawData data;
    ICM42688PScaledData offset_scaled = {.x = 0.f, .y = 0.f, .z = 0.f};

    icm42688p_write_gyro_offset(imu->icm42688p, &offset_scaled);
    furi_delay_ms(10);

    int32_t avg_x = 0;
    int32_t avg_y = 0;
    int32_t avg_z = 0;

    for(uint8_t i = 0; i < IMU_CALI_AVG; i++) {
        icm42688p_read_gyro_raw(imu->icm42688p, &data);
        avg_x += data.x;
        avg_y += data.y;
        avg_z += data.z;
        furi_delay_ms(2);
    }

    data.x = avg_x / IMU_CALI_AVG;
    data.y = avg_y / IMU_CALI_AVG;
    data.z = avg_z / IMU_CALI_AVG;

    icm42688p_apply_scale(&data, icm42688p_gyro_get_full_scale(imu->icm42688p), &offset_scaled);
    FURI_LOG_I(
        TAG,
        "Offsets: x %f, y %f, z %f",
        (double)offset_scaled.x,
        (double)offset_scaled.y,
        (double)offset_scaled.z);
    icm42688p_write_gyro_offset(imu->icm42688p, &offset_scaled);
}

static float imu_angle_diff(float a, float b) {
    float diff = a - b;
    if(diff > 180.f)
        diff -= 360.f;
    else if(diff < -180.f)
        diff += 360.f;

    return diff;
}

static int8_t mouse_exp_rate(float in) {
    int8_t sign = (in < 0.f) ? (-1) : (1);
    float val_in = (in * sign) / 127.f;
    float val_out = powf(val_in, EXP_RATE) * 127.f;
    return ((int8_t)val_out) * sign;
}

static int32_t imu_thread(void* context) {
    furi_assert(context);
    ImuThread* imu = context;
    furi_assert(imu->hid);

    float yaw_last = 0.f;
    float pitch_last = 0.f;
    float scroll_pitch = 0.f;
    float diff_x = 0.f;
    float diff_y = 0.f;
    uint32_t sample_cnt = 0;
    uint32_t hid_rate_div = FILTER_SAMPLE_FREQ / imu->hid->report_rate_max;

    bool scroll_mode = false;

    calibrate_gyro(imu);

    icm42688p_accel_config(imu->icm42688p, AccelFullScale16G, ACCEL_GYRO_RATE);
    icm42688p_gyro_config(imu->icm42688p, GyroFullScale2000DPS, ACCEL_GYRO_RATE);

    imu->processed_data.q0 = 1.f;
    imu->processed_data.q1 = 0.f;
    imu->processed_data.q2 = 0.f;
    imu->processed_data.q3 = 0.f;
    icm42688_fifo_enable(imu->icm42688p, imu_irq_callback, imu);

    while(1) {
        uint32_t events = furi_thread_flags_wait(FLAGS_ALL, FuriFlagWaitAny, FuriWaitForever);

        if(events & ImuMouseStop) {
            break;
        }

        if(events & ImuMouseRightPress) {
            imu->hid->mouse_key_press(imu->hid_inst, HID_MOUSE_BTN_RIGHT);
        }
        if(events & ImuMouseRightRelease) {
            imu->hid->mouse_key_release(imu->hid_inst, HID_MOUSE_BTN_RIGHT);
        }
        if(events & ImuMouseLeftPress) {
            imu->hid->mouse_key_press(imu->hid_inst, HID_MOUSE_BTN_LEFT);
        }
        if(events & ImuMouseLeftRelease) {
            imu->hid->mouse_key_release(imu->hid_inst, HID_MOUSE_BTN_LEFT);
        }
        if(events & ImuMouseScrollOn) {
            scroll_pitch = pitch_last;
            scroll_mode = true;
        }
        if(events & ImuMouseScrollOff) {
            scroll_mode = false;
        }

        if(events & ImuMouseNewData) {
            uint16_t data_pending = icm42688_fifo_get_count(imu->icm42688p);
            ICM42688PFifoPacket data;
            while(data_pending--) {
                icm42688_fifo_read(imu->icm42688p, &data);
                imu_process_data(imu, &data);

                if((imu->processed_data.pitch < -75.f) || (imu->processed_data.pitch > 75.f) ||
                   (isfinite(imu->processed_data.pitch) == 0)) {
                    continue;
                }

                if(scroll_mode) {
                    yaw_last = imu->processed_data.yaw;
                    pitch_last = -imu->processed_data.pitch;

                    sample_cnt++;
                    if(sample_cnt >= SCROLL_RATE_DIV) {
                        sample_cnt = 0;

                        float scroll_speed =
                            -imu_angle_diff(scroll_pitch, -imu->processed_data.pitch) *
                            SCROLL_SENSITIVITY_K;
                        scroll_speed = CLAMP(scroll_speed, 127.f, -127.f);

                        imu->hid->mouse_scroll(imu->hid_inst, scroll_speed);
                    }
                } else {
                    diff_x +=
                        imu_angle_diff(yaw_last, imu->processed_data.yaw) * MOUSE_SENSITIVITY_K;
                    diff_y += imu_angle_diff(pitch_last, -imu->processed_data.pitch) *
                              MOUSE_SENSITIVITY_K;

                    yaw_last = imu->processed_data.yaw;
                    pitch_last = -imu->processed_data.pitch;

                    sample_cnt++;
                    if(sample_cnt >= hid_rate_div) {
                        sample_cnt = 0;

                        float mouse_x = CLAMP(diff_x, 127.f, -127.f);
                        float mouse_y = CLAMP(diff_y, 127.f, -127.f);

                        imu->hid->mouse_move(
                            imu->hid_inst, mouse_exp_rate(mouse_x), mouse_exp_rate(mouse_y));

                        diff_x -= (float)(int8_t)mouse_x;
                        diff_y -= (float)(int8_t)mouse_y;
                    }
                }
            }
        }
    }

    imu->hid->mouse_key_release(imu->hid_inst, HID_MOUSE_BTN_RIGHT | HID_MOUSE_BTN_LEFT);

    icm42688_fifo_disable(imu->icm42688p);

    return 0;
}

void imu_mouse_key_press(ImuThread* imu, ImuMouseKey key, bool state) {
    furi_assert(imu);
    uint32_t flag = 0;
    if(key == ImuMouseKeyRight) {
        flag = (state) ? (ImuMouseRightPress) : (ImuMouseRightRelease);
    } else if(key == ImuMouseKeyLeft) {
        flag = (state) ? (ImuMouseLeftPress) : (ImuMouseLeftRelease);
    }

    furi_thread_flags_set(furi_thread_get_id(imu->thread), flag);
}

void imu_mouse_scroll_mode(ImuThread* imu, bool enable) {
    furi_assert(imu);
    uint32_t flag = (enable) ? (ImuMouseScrollOn) : (ImuMouseScrollOff);
    furi_thread_flags_set(furi_thread_get_id(imu->thread), flag);
}

ImuThread* imu_start(ICM42688P* icm42688p, const ImuHidApi* hid, void* hid_inst) {
    ImuThread* imu = malloc(sizeof(ImuThread));
    imu->icm42688p = icm42688p;
    imu->hid = hid;
    imu->hid_inst = hid_inst;
    imu->thread = furi_thread_alloc_ex("ImuThread", 4096, imu_thread, imu);
    furi_thread_start(imu->thread);

    return imu;
}

void imu_stop(ImuThread* imu) {
    furi_assert(imu);

    furi_thread_flags_set(furi_thread_get_id(imu->thread), ImuMouseStop);

    furi_thread_join(imu->thread);
    furi_thread_free(imu->thread);

    free(imu);
}

static float imu_inv_sqrt(float number) {
    union {
        float f;
        uint32_t i;
    } conv = {.f = number};
    conv.i = 0x5F3759Df - (conv.i >> 1);
    conv.f *= 1.5f - (number * 0.5f * conv.f * conv.f);
    return conv.f;
}

/* Simple madgwik filter, based on: https://github.com/arduino-libraries/MadgwickAHRS/ */

static void imu_madgwick_filter(
    ImuProcessedData* out,
    ICM42688PScaledData* accel,
    ICM42688PScaledData* gyro) {
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-out->q1 * gyro->x - out->q2 * gyro->y - out->q3 * gyro->z);
    qDot2 = 0.5f * (out->q0 * gyro->x + out->q2 * gyro->z - out->q3 * gyro->y);
    qDot3 = 0.5f * (out->q0 * gyro->y - out->q1 * gyro->z + out->q3 * gyro->x);
    qDot4 = 0.5f * (out->q0 * gyro->z + out->q1 * gyro->y - out->q2 * gyro->x);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if(!((accel->x == 0.0f) && (accel->y == 0.0f) && (accel->z == 0.0f))) {
        // Normalise accelerometer measurement
        recipNorm = imu_inv_sqrt(accel->x * accel->x + accel->y * accel->y + accel->z * accel->z);
        accel->x *= recipNorm;
        accel->y *= recipNorm;
        accel->z *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0 = 2.0f * out->q0;
        _2q1 = 2.0f * out->q1;
        _2q2 = 2.0f * out->q2;
        _2q3 = 2.0f * out->q3;
        _4q0 = 4.0f * out->q0;
        _4q1 = 4.0f * out->q1;
        _4q2 = 4.0f * out->q2;
        _8q1 = 8.0f * out->q1;
        _8q2 = 8.0f * out->q2;
        q0q0 = out->q0 * out->q0;
        q1q1 = out->q1 * out->q1;
        q2q2 = out->q2 * out->q2;
        q3q3 = out->q3 * out->q3;

        // Gradient decent algorithm corrective step
        s0 = _4q0 * q2q2 + _2q2 * accel->x + _4q0 * q1q1 - _2q1 * accel->y;
        s1 = _4q1 * q3q3 - _2q3 * accel->x + 4.0f * q0q0 * out->q1 - _2q0 * accel->y - _4q1 +
             _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * accel->z;
        s2 = 4.0f * q0q0 * out->q2 + _2q0 * accel->x + _4q2 * q3q3 - _2q3 * accel->y - _4q2 +
             _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * accel->z;
        s3 = 4.0f * q1q1 * out->q3 - _2q1 * accel->x + 4.0f * q2q2 * out->q3 - _2q2 * accel->y;
        recipNorm =
            imu_inv_sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Apply feedback step
        qDot1 -= FILTER_BETA * s0;
        qDot2 -= FILTER_BETA * s1;
        qDot3 -= FILTER_BETA * s2;
        qDot4 -= FILTER_BETA * s3;
    }

    // Integrate rate of change of quaternion to yield quaternion
    out->q0 += qDot1 * (1.0f / FILTER_SAMPLE_FREQ);
    out->q1 += qDot2 * (1.0f / FILTER_SAMPLE_FREQ);
    out->q2 += qDot3 * (1.0f / FILTER_SAMPLE_FREQ);
    out->q3 += qDot4 * (1.0f / FILTER_SAMPLE_FREQ);

    // Normalise quaternion
    recipNorm = imu_inv_sqrt(
        out->q0 * out->q0 + out->q1 * out->q1 + out->q2 * out->q2 + out->q3 * out->q3);
    out->q0 *= recipNorm;
    out->q1 *= recipNorm;
    out->q2 *= recipNorm;
    out->q3 *= recipNorm;
}
