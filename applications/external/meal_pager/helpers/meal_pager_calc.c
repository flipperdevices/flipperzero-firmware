#include "meal_pager_calc.h"

void customConcat(char* dest, const char* src) {
    // Find the end of the destination string
    while(*dest != '\0') {
        dest++;
    }

    // Copy characters from src to dest
    while(*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }

    // Null-terminate the concatenated string
    *dest = '\0';
}

char* encManchester(const char* bits, int mode) {
    // Allocate memory for the result string
    char* res = (char*)malloc((strlen(bits) * 2 + 1) * sizeof(char));

    int index = 0;
    for(int i = 0; bits[i] != '\0'; i++) {
        char c = bits[i];
        if(c == '0') {
            if(mode) {
                res[index++] = '1';
                res[index++] = '0';
            } else {
                res[index++] = '0';
                res[index++] = '1';
            }
        } else if(c == '1') {
            if(mode) {
                res[index++] = '0';
                res[index++] = '1';
            } else {
                res[index++] = '1';
                res[index++] = '0';
            }
        } else {
            // Handle 'EE' case (error)
            res[index++] = 'E';
            res[index++] = 'E';
        }
    }

    // Null-terminate the result string
    res[index] = '\0';

    return res;
}

void uint32ToBinaray(uint32_t number, char* str, int8_t length) {
    int i = 0;
    length--; // count length without 0
    for(i = length; i >= 0; i--) {
        // Bitwise AND extration of the i-th bit
        int bit = (number >> i) & 1;
        // convert the bit to a character of 1 or 0
        str[length - i] = bit + '0';
    }
    // Terminate the string
    str[length + 1] = '\0';
}

void reverse(char* str) {
    int length = strlen(str);
    int start = 0;
    int end = length - 1;
    while(start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}