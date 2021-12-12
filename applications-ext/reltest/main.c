int printf(const char* __restrict, ...);

int main(void) {
    printf("Hello, out is %d\r\n", 123);
    return 0;
}