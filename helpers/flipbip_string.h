#include <stdint.h>

char * flipbip_strtok(char *s, const char *delim);
char * flipbip_strtok_r(char *s, const char *delim, char **last);

void flipbip_itox(uint8_t i, char *str);