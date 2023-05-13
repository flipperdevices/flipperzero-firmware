#include "ac_remote_app_i.h"
#include "ac_remote.h"

/***************************************************************************************************
* Protocol description:
* https://github.com/r45635/HVAC-IR-Control/blob/4b6b7944b28ce78247f19744c272a36935bbb305/Protocol/Mitsubishi_IR_Packet_Data_v1.1-FULL.pdf
****************************************************************************************************
* Used code from the project:
* https://github.com/r45635/HVAC-IR-Control/blob/4b6b7944b28ce78247f19744c272a36935bbb305/HVACDemo/IRremote2.cpp
***************************************************************************************************/

void send_ac_mitsubishi(ACState* acstate)
{
    char mask = 1; // our bitmask
    uint8_t data[18] = {
        0x23,
        0xCB,
        0x26,
        0x01,
        0x00,
        0b00000000, // off
        0b00100000, // mode_auto
        0b00001001, // 25
        0b00110000, // mode_auto
        0b11000000, // fan_speed_auto + vanne_auto
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00};
    // data array is a valid trame, only byte to be chnaged will be updated.

    // Byte 6 - On / Off
    if (acstate->ac_power)
    {
        data[5] = 0x20; // Tuen ON AC
    }

    // Byte 7 - Mode
    switch (acstate->ac_mode)
    {
    case AC_HOT:
        data[6] = 0b00001000;
        break;
    case AC_COLD:
        data[6] = 0b00011000;
        break;
    case AC_DRY:
        data[6] = 0b00010000;
        break;
    case AC_AUTO:
        data[6] = 0b00100000;
        break;
    default:
        break;
    }

    // Byte 8 - Temperature
    // Check Min Max For Hot Mode
    int temp;
    if (acstate->ac_temp > 31)
    {
        temp = 31;
    }
    else if (acstate->ac_temp < 16)
    {
        temp = 16;
    }
    else
    {
        temp = acstate->ac_temp;
    };
    data[7] = temp - 16;

    switch (acstate->ac_mode)
    {
    case AC_HOT:
        data[8] = 0b00000000;
        break;
    case AC_COLD:
        data[8] = 0b00000110;
        break;
    case AC_DRY:
        data[8] = 0b00000010;
        break;
    case AC_AUTO:
        data[8] = 0b00000000;
        break;
    default:
        break;
    }

    // Byte 10 - FAN / VANNE
    switch (acstate->ac_fanmode)
    {
    case FAN_SPEED_1:
        data[9] = 0b00000001;
        break;
    case FAN_SPEED_2:
        data[9] = 0b00000010;
        break;
    case FAN_SPEED_3:
        data[9] = 0b00000011;
        break;
    case FAN_SPEED_4:
        data[9] = 0b00000100;
        break;
    case FAN_SPEED_5:
        data[9] = 0b00000100;
        break; // No FAN speed 5 for MITSUBISHI so it is consider as Speed 4
    case FAN_SPEED_AUTO:
        data[9] = 0b10000000;
        break;
    case FAN_SPEED_SILENT:
        data[9] = 0b00000101;
        break;
    default:
        break;
    }

    switch (acstate->ac_vannemode)
    {
    case VANNE_AUTO:
        data[9] = data[9] | 0b01000000;
        break;
    case VANNE_H1:
        data[9] = data[9] | 0b01001000;
        break;
    case VANNE_H2:
        data[9] = data[9] | 0b01010000;
        break;
    case VANNE_H3:
        data[9] = data[9] | 0b01011000;
        break;
    case VANNE_H4:
        data[9] = data[9] | 0b01100000;
        break;
    case VANNE_H5:
        data[9] = data[9] | 0b01101000;
        break;
    case VANNE_AUTO_MOVE:
        data[9] = data[9] | 0b01111000;
        break;
    default:
        break;
    }

    // Byte 18 - CRC
    data[17] = 0;
    for (int i = 0; i < 17; i++)
    {
        data[17] = data[i] + data[17]; // CRC is a simple bits addition
    }

    uint32_t *timings = malloc(sizeof(uint32_t) * MAX_TIMINGS_AMOUNT);
    uint32_t frequency = 38000;
    float duty_cycle = 0.33;
    size_t timings_size = 0;

    int j = 0;
    if (sizeof(data) / sizeof(data[0]) + 8 < MAX_TIMINGS_AMOUNT)
    {
        while (j < 2)
        {
            timings[timings_size] = AC_MITSUBISHI_HDR_MARK;
            timings_size++;
            timings[timings_size] = AC_MITSUBISHI_HDR_SPACE;
            timings_size++;
            for (int i = 0; i < 18; i++)
            {
                for (mask = 00000001; mask > 0; mask <<= 1)
                {
                    if (data[i] & mask)
                    {
                        timings[timings_size] = AC_MITSUBISHI_BIT_MARK;
                        timings_size++;
                        timings[timings_size] = AC_MITSUBISHI_ONE_SPACE;
                        timings_size++;
                    }
                    else
                    {
                        timings[timings_size] = AC_MITSUBISHI_BIT_MARK;
                        timings_size++;
                        timings[timings_size] = AC_MISTUBISHI_ZERO_SPACE;
                        timings_size++;
                    }
                }
            }
            timings[timings_size] = AC_MITSUBISHI_RPT_MARK;
            timings_size++;
            timings[timings_size] = AC_MITSUBISHI_RPT_SPACE;
            timings_size++;
            j++;
        }
    }
    infrared_send_raw_ext(timings, timings_size, true, frequency, duty_cycle);
    free(timings);
}
