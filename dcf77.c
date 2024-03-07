#include "dcf77.h"

#define DST_BIT 17
#define MIN_BIT 21
#define HOUR_BIT 29
#define DAY_BIT 36
#define WEEKDAY_BIT 42
#define MONTH_BIT 45
#define YEAR_BIT 50

static uint8_t dcf77_bits[] = {
    0, // 00: Start of minute
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 01: Weather broadcast / Civil warning bits
    8, // 15: Call bit: abnormal transmitter operation
    0, // 16: Summer time announcement. Set during hour before change
    0, 1, // 17: 01=CET, 10=CEST
    0, // 19: Leap second announcement. Set during hour before leap second
    1, // 20: Start of encoded time
    8, 0, 0, 0, 0, 0, 0, 0, // 21: Minutes (7bit + parity, 00-59)
    8, 0, 0, 0, 0, 0, 0, // 29: Hours (6bit + parity, 0-23)
    8, 0, 0, 0, 0, 0, // 36: Day of month (6bit, 1-31)
    8, 0, 0, // 42: Day of week (3bit, 1-7, Monday=1)
    8, 0, 0, 0, 0, // 45: Month number (5bit, 1-12)
    8, 0, 0, 0, 0, 0, 0, 0, 0, // 50: Year within century (8bit + parity, 00-99)
    0 // 59: Not used
};

void dcf77_encode(int start, int len, int val, int par) {
    uint8_t parity = (par != -1 ? par : dcf77_bits[start]) & 1;
    uint8_t byte = ((val / 10) << 4) + (val % 10);
    for(int bit = 0; bit < len; bit++) {
        uint8_t dcf77_bit = (byte >> bit) & 1;
        parity ^= dcf77_bit;
        dcf77_bits[start + bit] = (dcf77_bits[start + bit] & 0x0E) + dcf77_bit;
    }
    dcf77_bits[start + len] = (dcf77_bits[start + len] & 0xE) + (parity & 1);
}

void set_dcf77_time(DateTime* dt, bool is_dst) {
    dcf77_encode(DST_BIT, 2, is_dst > 0 ? 1 : 2, 1); // parity = leap second -> 0
    dcf77_encode(MIN_BIT, 7, dt->minute, 0);
    dcf77_encode(HOUR_BIT, 6, dt->hour, 0);
    dcf77_encode(DAY_BIT, 6, dt->day, 0);
    dcf77_encode(WEEKDAY_BIT, 3, dt->weekday, -1);
    dcf77_encode(MONTH_BIT, 5, dt->month, -1);
    dcf77_encode(YEAR_BIT, 8, dt->year % 100, -1);
}

bool get_dcf77_bit(int sec) {
    return dcf77_bits[sec % 60] & 1;
}

char* get_dcf77_data(int sec) {
    static char data[70];

    int idx = 0;
    int start = sec > 25 ? sec - 25 : 0;
    for(int bit = start; bit <= sec; bit++) {
        if(dcf77_bits[bit] & 8) data[idx++] = '-';
        data[idx++] = '0' + (dcf77_bits[bit] & 1);
    }
    data[idx] = 0;
    return data;
}