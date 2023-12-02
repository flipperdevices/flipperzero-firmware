char* flipbip_strtok(char* s, const char* delim);
char* flipbip_strtok_r(char* s, const char* delim, char** last);

void flipbip_btox(const unsigned char* in, int in_len, char* str);
void flipbip_xtob(const char* str, unsigned char* out, int out_len);

void flipbip_cipher(
    const unsigned char* key_in,
    const unsigned int key_len,
    const char* in,
    char* out,
    const unsigned int io_len);