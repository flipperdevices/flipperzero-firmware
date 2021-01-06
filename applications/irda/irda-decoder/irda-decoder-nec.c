#include "irda-decoder-nec.h"

const uint32_t PREAMBULA_HIGH_MIN = 9000 - 900;
const uint32_t PREAMBULA_HIGH_MAX = 9000 + 900;

const uint32_t PREAMBULA_LOW_MIN = 4500 - 450;
const uint32_t PREAMBULA_LOW_MAX = 4500 + 450;

const uint32_t PREAMBULA_RETRY_LOW_MIN = 2500 - 250;
const uint32_t PREAMBULA_RETRY_LOW_MAX = 2500 + 250;

const uint32_t BIT_HIGH_MIN = 560 - 100;
const uint32_t BIT_HIGH_MAX = 560 + 100;

const uint32_t BIT_LOW_ONE_MIN = 1690 - 200;
const uint32_t BIT_LOW_ONE_MAX = 1690 + 200;

const uint32_t BIT_LOW_ZERO_MIN = 560 - 100;
const uint32_t BIT_LOW_ZERO_MAX = 560 + 100;

#define SET_STATE(_state) \
    { decoder->state = _state; }

#define TIME_FIT(_prefix) ((time > _prefix##_MIN) && (time < _prefix##_MAX))

bool process_decoder_nec(IrDANecDecoder* decoder, bool polarity, uint32_t time) {
    bool error = true;
    bool result = false;

    switch(decoder->state) {
    case(WAIT_PREAMBULA_HIGH):
        if(polarity) {
            if(TIME_FIT(PREAMBULA_HIGH)) {
                SET_STATE(WAIT_PREAMBULA_LOW);
                error = false;
                decoder->data = 0;
                decoder->current_data_index = 0;
            }
        }
        break;
    case(WAIT_PREAMBULA_LOW):
        if(!polarity) {
            if(TIME_FIT(PREAMBULA_LOW)) {
                SET_STATE(WAIT_BIT_HIGH);
                error = false;
            } else if(TIME_FIT(PREAMBULA_RETRY_LOW)) {
                SET_STATE(WAIT_RETRY_HIGH);
                error = false;
            }
        }
        break;
    case(WAIT_RETRY_HIGH):
        if(polarity) {
            if(TIME_FIT(BIT_HIGH)) {
                SET_STATE(WAIT_PREAMBULA_HIGH);

                // retry event

                error = false;
            }
        }
        break;
    case(WAIT_BIT_HIGH):
        if(polarity) {
            if(TIME_FIT(BIT_HIGH)) {
                SET_STATE(WAIT_BIT_LOW);
                error = false;
            }
        }
        break;
    case(WAIT_BIT_STOP_HIGH):
        if(polarity) {
            if(TIME_FIT(BIT_HIGH)) {
                SET_STATE(WAIT_PREAMBULA_HIGH);

                // message end event

                error = false;
            }
        }
        break;
    case(WAIT_BIT_LOW):
        if(!polarity) {
            int8_t bit = -1;
            if(TIME_FIT(BIT_LOW_ZERO)) {
                SET_STATE(WAIT_BIT_HIGH);
                bit = 0;
                error = false;
            } else if(TIME_FIT(BIT_LOW_ONE)) {
                SET_STATE(WAIT_BIT_HIGH);
                bit = 1;
                error = false;
            }

            if(bit != -1) {
                decoder->data |= (bit << decoder->current_data_index);
                decoder->current_data_index++;

                if(decoder->current_data_index >= 31) {
                    SET_STATE(WAIT_BIT_STOP_HIGH);
                }
            }
        }
        break;
    }

    if(error) SET_STATE(WAIT_PREAMBULA_HIGH);

    return result;
}