#include "digital_signal.h"

#define F_TIM (64000000.0)
#define T_TIM 1562 /* 15.625 ns *100 */
#define T_TIM_DIV2 (T_TIM / 2) /* 15.625 ns / 2 *100 */

struct DigitalSignal {
    bool start_level;
    uint32_t edge_cnt;
    uint32_t edges_max_cnt;
    uint32_t* reload_reg_buff;
    int32_t remainder;
};
