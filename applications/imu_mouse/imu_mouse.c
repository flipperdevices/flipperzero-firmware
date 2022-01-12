#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

#include <furi_hal_console.h>

#include <math.h>

#define TAG "IMU"

#define IMU_ADDR (0x68 << 1)
#define IMU_ID_MPU6050 0x68

#define IMU_CALI_AVG 64

#define SENSITIVITY_K 20.f

#define SAMPLE_FREQ 120.f // sample frequency in Hz
#define FILTER_BETA 0.05f // 2 * proportional gain (default: 0.1)

typedef enum {
    EventTypeInput,
    EventTypeTick,
} EventType;

typedef struct {
    union {
        InputEvent input;
    };
    EventType type;
} ImuMouseEvent;

typedef struct {
    int16_t g_cali[3];
    float gx;
    float gy;
    float gz;
    float ax;
    float ay;
    float az;

    float q0;
    float q1;
    float q2;
    float q3;
} ImuData;

typedef struct {
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
} ImuDataRaw;

static void imu_madgwick_filter(ImuData* imu);

static void imu_mouse_update(void* ctx) {
    furi_assert(ctx);
    osMessageQueueId_t event_queue = ctx;
    ImuMouseEvent event = {.type = EventTypeTick};
    osMessageQueuePut(event_queue, &event, 0, 0);
}

static void imu_mouse_render_callback(Canvas* canvas, void* ctx) {
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, "IMU demo");

    // canvas_set_font(canvas, FontSecondary);
    // canvas_draw_str(canvas, 0, 63, "Hold [back] to exit");
}

static void imu_mouse_input_callback(InputEvent* input_event, void* ctx) {
    osMessageQueueId_t event_queue = ctx;

    ImuMouseEvent event;
    event.type = EventTypeInput;
    event.input = *input_event;
    osMessageQueuePut(event_queue, &event, 0, osWaitForever);
}

static bool imu_probe() {
    uint8_t reg_data = 0;

    bool ret =
        furi_hal_i2c_read_reg_8(&furi_hal_i2c_handle_external, IMU_ADDR, 0x75, &reg_data, 100);
    if((!ret) || (reg_data != IMU_ID_MPU6050)) return false;

    ret = furi_hal_i2c_write_reg_8(
        &furi_hal_i2c_handle_external, IMU_ADDR, 0x6B, 0x00, 100); // PWR_MGMT_1
    if(!ret) return false;

    ret = furi_hal_i2c_write_reg_8(
        &furi_hal_i2c_handle_external, IMU_ADDR, 0x6C, 0x00, 100); // PWR_MGMT_2
    if(!ret) return false;

    ret = furi_hal_i2c_write_reg_8(
        &furi_hal_i2c_handle_external, IMU_ADDR, 0x19, 0x00, 100); // SMP_RATE
    if(!ret) return false;

    ret = furi_hal_i2c_write_reg_8(
        &furi_hal_i2c_handle_external, IMU_ADDR, 0x1B, 0x08, 100); // GYRO_CONFIG
    if(!ret) return false;

    ret = furi_hal_i2c_write_reg_8(
        &furi_hal_i2c_handle_external, IMU_ADDR, 0x1C, 0x08, 100); // ACCEL_CONFIG
    if(!ret) return false;

    return true;
}

static bool imu_read(ImuDataRaw* data) {
    uint8_t data_buf[6];

    bool ret =
        furi_hal_i2c_read_mem(&furi_hal_i2c_handle_external, IMU_ADDR, 0x3B, data_buf, 6, 10);
    if(!ret) return false;
    data->accel_x = (data_buf[0] << 8) | (data_buf[1]);
    data->accel_y = (data_buf[2] << 8) | (data_buf[3]);
    data->accel_z = (data_buf[4] << 8) | (data_buf[5]);

    ret = furi_hal_i2c_read_mem(&furi_hal_i2c_handle_external, IMU_ADDR, 0x43, data_buf, 6, 10);
    if(!ret) return false;
    data->gyro_x = (data_buf[0] << 8) | (data_buf[1]);
    data->gyro_y = (data_buf[2] << 8) | (data_buf[3]);
    data->gyro_z = (data_buf[4] << 8) | (data_buf[5]);

    return true;
}

static bool imu_calibrate(ImuData* data) {
    ImuDataRaw read_val;

    int32_t gyro_x = 0;
    int32_t gyro_y = 0;
    int32_t gyro_z = 0;

    for(uint8_t i = 0; i < IMU_CALI_AVG; i++) {
        if(imu_read(&read_val) == false) {
            return false;
        }
        gyro_x += read_val.gyro_x;
        gyro_y += read_val.gyro_y;
        gyro_z += read_val.gyro_z;
        osDelay(5);
    }

    gyro_x /= IMU_CALI_AVG;
    gyro_y /= IMU_CALI_AVG;
    gyro_z /= IMU_CALI_AVG;

    data->g_cali[0] = gyro_x;
    data->g_cali[1] = gyro_y;
    data->g_cali[2] = gyro_z;

    return true;
}

static float imu_angle_diff(float a, float b) {
    float diff = a - b;
    if(diff > 180.f)
        diff -= 360.f;
    else if(diff < -180.f)
        diff += 360.f;

    return diff;
}

int32_t imu_mouse_app(void* p) {
    osMessageQueueId_t event_queue = osMessageQueueNew(8, sizeof(ImuMouseEvent), NULL);
    furi_check(event_queue);
    ViewPort* view_port = view_port_alloc();
    ImuDataRaw raw;
    ImuData imu;
    bool imu_ok = false;

    float yaw_last = 0.f;
    float pitch_last = 0.f;
    float diff_x = 0.f;
    float diff_y = 0.f;

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    if(imu_probe() == false) {
        FURI_LOG_I(TAG, "Init failed");
    } else {
        imu_ok = true;
        imu_calibrate(&imu);
    }

    UsbInterface* usb_mode_prev = furi_hal_usb_get_config();
    furi_hal_usb_set_config(&usb_hid);

    view_port_draw_callback_set(view_port, imu_mouse_render_callback, NULL);
    view_port_input_callback_set(view_port, imu_mouse_input_callback, event_queue);

    osTimerId_t timer = osTimerNew(imu_mouse_update, osTimerPeriodic, event_queue, NULL);
    osTimerStart(timer, osKernelGetTickFreq() / (uint32_t)SAMPLE_FREQ);

    imu.q0 = 1.f;
    imu.q1 = 0.f;
    imu.q2 = 0.f;
    imu.q3 = 0.f;

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    ImuMouseEvent event;
    while(1) {
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, osWaitForever);

        if(event_status == osOK) {
            if(event.type == EventTypeInput) {
                if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                    break;
                }

                if(event.input.key == InputKeyOk) {
                    if(event.input.type == InputTypePress) {
                        furi_hal_hid_mouse_press(HID_MOUSE_BTN_LEFT);
                    } else if(event.input.type == InputTypeRelease) {
                        furi_hal_hid_mouse_release(HID_MOUSE_BTN_LEFT);
                    }
                }

                if(event.input.key == InputKeyUp) {
                    if(event.input.type == InputTypePress) {
                        furi_hal_hid_mouse_press(HID_MOUSE_BTN_RIGHT);
                    } else if(event.input.type == InputTypeRelease) {
                        furi_hal_hid_mouse_release(HID_MOUSE_BTN_RIGHT);
                    }
                }

                if(event.input.type == InputTypeShort && event.input.key == InputKeyRight) {
                    if(imu_ok == true) imu_calibrate(&imu);
                }

            } else if((event.type == EventTypeTick) && (imu_ok == true)) {
                if(imu_read(&raw) == true) {
                    // apply gyro calibration
                    raw.gyro_x -= imu.g_cali[0];
                    raw.gyro_y -= imu.g_cali[1];
                    raw.gyro_z -= imu.g_cali[2];

                    // accelerometer: get values in g
                    imu.ax = ((float)(raw.accel_x)) / 32768.f * 4.f;
                    imu.ay = ((float)(raw.accel_y)) / 32768.f * 4.f;
                    imu.az = ((float)(raw.accel_z)) / 32768.f * 4.f;

                    // gyro: get values in degrees/s
                    imu.gx = ((float)(raw.gyro_x)) / 32768.f * 500.f;
                    imu.gy = ((float)(raw.gyro_y)) / 32768.f * 500.f;
                    imu.gz = ((float)(raw.gyro_z)) / 32768.f * 500.f;

                    // gyro: degrees/s to rads/s
                    imu.gx = imu.gx / 180.f * M_PI;
                    imu.gy = imu.gy / 180.f * M_PI;
                    imu.gz = imu.gz / 180.f * M_PI;

                    // sensor fusion algorithm
                    imu_madgwick_filter(&imu);

                    // quaternion to euler angles
                    float roll = atan2f(
                        2.f * (imu.q0 * imu.q1 + imu.q2 * imu.q3),
                        imu.q0 * imu.q0 - imu.q1 * imu.q1 - imu.q2 * imu.q2 + imu.q3 * imu.q3);
                    float pitch = -asinf(2.f * (imu.q1 * imu.q3 - imu.q0 * imu.q2));
                    float yaw = atan2f(
                        2.f * (imu.q1 * imu.q2 + imu.q0 * imu.q3),
                        imu.q0 * imu.q0 + imu.q1 * imu.q1 - imu.q2 * imu.q2 - imu.q3 * imu.q3);

                    // euler angles: rads to degrees
                    roll = roll / M_PI * 180.f;
                    pitch = pitch / M_PI * 180.f;
                    yaw = yaw / M_PI * 180.f;

                    // for debugging with https://github.com/thecountoftuscany/PyTeapot-Quaternion-Euler-cube-rotation
                    //furi_hal_console_printf("w%5.3fwa%5.3fab%5.3fbc%5.3fc\r\n", imu.q0, imu.q1, imu.q2, imu.q3);
                    //furi_hal_console_printf("y%5.3fyp%5.3fpr%5.3fr\r\n", yaw, pitch, roll);

                    if((pitch > -75.f) && (pitch < 75.f) && (isfinite(pitch) != 0)) {
                        int8_t mouse_x = 0;
                        int8_t mouse_y = 0;

                        diff_x += imu_angle_diff(yaw_last, yaw) * SENSITIVITY_K;
                        if(diff_x < -127.f) {
                            mouse_x = -127;
                        } else if(diff_x > 127.f) {
                            mouse_x = 127;
                        } else {
                            mouse_x = (int8_t)diff_x;
                        }
                        diff_x -= (float)mouse_x;
                        yaw_last = yaw;

                        diff_y += imu_angle_diff(pitch_last, pitch) * SENSITIVITY_K;
                        if(diff_y < -127.f) {
                            mouse_y = -127;
                        } else if(diff_y > 127.f) {
                            mouse_y = 127;
                        } else {
                            mouse_y = (int8_t)diff_y;
                        }
                        diff_y -= (float)mouse_y;
                        pitch_last = pitch;

                        furi_hal_hid_mouse_move(mouse_x, mouse_y);
                    }

                    (void)roll;

                } else {
                    FURI_LOG_I(TAG, "Read failed");
                }
            }
        }
        view_port_update(view_port);
    }

    furi_hal_usb_set_config(usb_mode_prev);

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);

    // remove & free all stuff created by app
    osTimerStop(timer);
    osDelay(10);
    osTimerDelete(timer);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    osMessageQueueDelete(event_queue);

    return 0;
}

static float imu_inv_sqrt(float x) {
    /* close-to-optimal method with low cost from http://pizer.wordpress.com/2008/10/12/fast-inverse-square-root */
    unsigned int i = 0x5F1F1412 - (*(unsigned int*)&x >> 1);
    float tmp = *(float*)&i;
    return tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
}

static void imu_madgwick_filter(ImuData* imu) {
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-imu->q1 * imu->gx - imu->q2 * imu->gy - imu->q3 * imu->gz);
    qDot2 = 0.5f * (imu->q0 * imu->gx + imu->q2 * imu->gz - imu->q3 * imu->gy);
    qDot3 = 0.5f * (imu->q0 * imu->gy - imu->q1 * imu->gz + imu->q3 * imu->gx);
    qDot4 = 0.5f * (imu->q0 * imu->gz + imu->q1 * imu->gy - imu->q2 * imu->gx);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if(!((imu->ax == 0.0f) && (imu->ay == 0.0f) && (imu->az == 0.0f))) {
        // Normalise accelerometer measurement
        recipNorm = imu_inv_sqrt(imu->ax * imu->ax + imu->ay * imu->ay + imu->az * imu->az);
        imu->ax *= recipNorm;
        imu->ay *= recipNorm;
        imu->az *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0 = 2.0f * imu->q0;
        _2q1 = 2.0f * imu->q1;
        _2q2 = 2.0f * imu->q2;
        _2q3 = 2.0f * imu->q3;
        _4q0 = 4.0f * imu->q0;
        _4q1 = 4.0f * imu->q1;
        _4q2 = 4.0f * imu->q2;
        _8q1 = 8.0f * imu->q1;
        _8q2 = 8.0f * imu->q2;
        q0q0 = imu->q0 * imu->q0;
        q1q1 = imu->q1 * imu->q1;
        q2q2 = imu->q2 * imu->q2;
        q3q3 = imu->q3 * imu->q3;

        // Gradient decent algorithm corrective step
        s0 = _4q0 * q2q2 + _2q2 * imu->ax + _4q0 * q1q1 - _2q1 * imu->ay;
        s1 = _4q1 * q3q3 - _2q3 * imu->ax + 4.0f * q0q0 * imu->q1 - _2q0 * imu->ay - _4q1 +
             _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * imu->az;
        s2 = 4.0f * q0q0 * imu->q2 + _2q0 * imu->ax + _4q2 * q3q3 - _2q3 * imu->ay - _4q2 +
             _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * imu->az;
        s3 = 4.0f * q1q1 * imu->q3 - _2q1 * imu->ax + 4.0f * q2q2 * imu->q3 - _2q2 * imu->ay;
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
    imu->q0 += qDot1 * (1.0f / SAMPLE_FREQ);
    imu->q1 += qDot2 * (1.0f / SAMPLE_FREQ);
    imu->q2 += qDot3 * (1.0f / SAMPLE_FREQ);
    imu->q3 += qDot4 * (1.0f / SAMPLE_FREQ);

    // Normalise quaternion
    recipNorm = imu_inv_sqrt(
        imu->q0 * imu->q0 + imu->q1 * imu->q1 + imu->q2 * imu->q2 + imu->q3 * imu->q3);
    imu->q0 *= recipNorm;
    imu->q1 *= recipNorm;
    imu->q2 *= recipNorm;
    imu->q3 *= recipNorm;
}
