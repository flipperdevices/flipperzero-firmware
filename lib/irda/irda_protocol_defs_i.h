#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "irda.h"
#include "irda_common_i.h"

#define IRDA_COMMON_CARRIER_FREQUENCY      38000
#define IRDA_COMMON_DUTY_CYCLE             0.33

/***************************************************************************************************
*   NEC protocol description
*   https://radioparty.ru/manuals/encyclopedia/213-ircontrol?start=1
****************************************************************************************************
*     Preamble   Preamble      Pulse Distance/Width          Pause       Preamble   Preamble  Stop
*       mark      space            Modulation                             repeat     repeat    bit
*                                                                          mark       space
*
*        9000      4500         32 bit + stop bit         40000/100000     9000       2250
*     __________          _ _ _ _  _  _  _ _ _  _  _ _ _                ___________            _
* ____          __________ _ _ _ __ __ __ _ _ __ __ _ _ ________________           ____________ ___
*
***************************************************************************************************/

#define IRDA_NEC_PREAMBULE_MARK         9000
#define IRDA_NEC_PREAMBULE_SPACE        4500
#define IRDA_NEC_BIT1_MARK              560
#define IRDA_NEC_BIT1_SPACE             1600
#define IRDA_NEC_BIT0_MARK              560
#define IRDA_NEC_BIT0_SPACE             560
#define IRDA_NEC_REPEAT_PAUSE_MIN       30000
#define IRDA_NEC_REPEAT_PAUSE           40000
#define IRDA_NEC_REPEAT_PAUSE_MAX       150000
#define IRDA_NEC_REPEAT_MARK            9000
#define IRDA_NEC_REPEAT_SPACE           2250
#define IRDA_NEC_CARRIER_FREQUENCY      38000
#define IRDA_NEC_DUTY_CYCLE             0.33
#define IRDA_NEC_PREAMBLE_TOLERANCE     0.07    // percents
#define IRDA_NEC_BIT_TOLERANCE          120     // us

void* irda_decoder_nec_alloc(void);
void* irda_decoder_necext_alloc(void);
void irda_encoder_nec_encode(uint32_t address, uint32_t command, bool repeat);
void irda_encoder_necext_encode(uint32_t address, uint32_t command, bool repeat);
void irda_decoder_nec_reset(void* decoder);
void irda_decoder_nec_free(void* decoder);
IrdaMessage* irda_decoder_nec_decode(void* decoder, bool level, uint32_t duration);

/***************************************************************************************************
*   SAMSUNG32 protocol description
*   https://www.mikrocontroller.net/articles/IRMP_-_english#SAMSUNG
****************************************************************************************************
*  Preamble   Preamble     Pulse Distance/Width        Pause       Preamble   Preamble  Bit1  Stop
*    mark      space           Modulation                           repeat     repeat          bit
*                                                                    mark       space
*
*     4500      4500        32 bit + stop bit       40000/100000     4500       4500
*  __________          _  _ _  _  _  _ _ _  _  _ _                ___________            _    _
* _          __________ __ _ __ __ __ _ _ __ __ _ ________________           ____________ ____ ___
*
***************************************************************************************************/

#define IRDA_SAMSUNG_PREAMBULE_MARK         4500
#define IRDA_SAMSUNG_PREAMBULE_SPACE        4500
#define IRDA_SAMSUNG_BIT1_MARK              550
#define IRDA_SAMSUNG_BIT1_SPACE             1650
#define IRDA_SAMSUNG_BIT0_MARK              550
#define IRDA_SAMSUNG_BIT0_SPACE             550
#define IRDA_SAMSUNG_REPEAT_PAUSE_MIN       30000
#define IRDA_SAMSUNG_REPEAT_PAUSE           47000
#define IRDA_SAMSUNG_REPEAT_PAUSE_MAX       150000
#define IRDA_SAMSUNG_REPEAT_MARK            4500
#define IRDA_SAMSUNG_REPEAT_SPACE           4500
#define IRDA_SAMSUNG_CARRIER_FREQUENCY      38000
#define IRDA_SAMSUNG_DUTY_CYCLE             0.33
#define IRDA_SAMSUNG_PREAMBLE_TOLERANCE     0.07    // percents
#define IRDA_SAMSUNG_BIT_TOLERANCE          120     // us

void* irda_decoder_samsung32_alloc(void);
void irda_encoder_samsung32_encode(uint32_t address, uint32_t command, bool repeat);
void irda_decoder_samsung32_reset(void* decoder);
void irda_decoder_samsung32_free(void* decoder);
IrdaMessage* irda_decoder_samsung32_decode(void* decoder, bool level, uint32_t duration);

/***************************************************************************************************
*   RC6 protocol description
*   https://www.mikrocontroller.net/articles/IRMP_-_english#RC6_.2B_RC6A
****************************************************************************************************
*      Preamble                       Manchester/biphase                       Silence
*     mark/space                          Modulation
*
*    2666     889        444/888 - bit (x2 for toggle bit)                       2666
*
*  ________         __    __  __  __    ____   __  __  __  __  __  __  __  __
* _        _________  ____  __  __  ____    ___  __  __  __  __  __  __  __  ______________
*                   | 1 | 0 | 0 | 0 |   0   |      ...      |      ...      |             |
*                     s  m2  m1  m0     T     address (MSB)   command (MSB)
*
*    s - start bit (always 1)
*    m0-2 - mode (000 for RC6)
*    T - toggle bit, twice longer
*    address - 8 bit
*    command - 8 bit
***************************************************************************************************/

#define IRDA_RC6_PREAMBULE_MARK             2666
#define IRDA_RC6_PREAMBULE_SPACE            889
#define IRDA_RC6_BIT                        444     // half of time-quant for 1 bit
#define IRDA_RC6_CARRIER_FREQUENCY          38000
#define IRDA_RC6_DUTY_CYCLE                 0.33
#define IRDA_RC6_PREAMBLE_TOLERANCE         0.07    // percents
#define IRDA_RC6_BIT_TOLERANCE              120     // us

void* irda_decoder_rc6_alloc(void);
void irda_decoder_rc6_reset(void* decoder);
void irda_decoder_rc6_free(void* decoder);
IrdaMessage* irda_decoder_rc6_decode(void* decoder, bool level, uint32_t duration);
void* irda_encoder_rc6_alloc(void);
void irda_encoder_rc6_reset(void* decoder);
void irda_encoder_rc6_free(void* decoder);
IrdaStatus irda_encoder_rc6_encode(void* encoder_ptr, uint32_t* duration, bool* polarity);

bool interpret_rc6(IrdaCommonDecoder* decoder);
IrdaStatus irda_common_decode_rc6(IrdaCommonDecoder* decoder);
IrdaStatus irda_encode_rc6_manchester(void* encoder_ptr, uint32_t* duration, bool* polarity);

const IrdaCommonProtocolSpec protocol_rc6 = {
    .timings = {
        .preamble_mark = IRDA_RC6_PREAMBULE_MARK,
        .preamble_space = IRDA_RC6_PREAMBULE_SPACE,
        .bit1_mark = IRDA_RC6_BIT,
        .preamble_tolerance = IRDA_RC6_PREAMBLE_TOLERANCE,
        .bit_tolerance = IRDA_RC6_BIT_TOLERANCE,
        .duty_cycle = IRDA_RC6_DUTY_CYCLE,
        .carrier_frequency = IRDA_RC6_CARRIER_FREQUENCY,
    },
    .databit_len = 1 + 3 + 1 + 8 + 8,   // start_bit + 3 mode bits, + 1 toggle bit (x2 timing) + 8 address + 8 command
    .manchester_inverse_level = false,
    .decode = irda_common_decode_rc6,
    .interpret = interpret_rc6,
    .decode_repeat = NULL,
    .encode = irda_encode_rc6_manchester,
    .encode_repeat = NULL,
};

