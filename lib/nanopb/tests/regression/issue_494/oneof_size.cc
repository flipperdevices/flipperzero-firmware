#include "oneof.pb.h"
#include "unittests.h"

extern "C" int main()
{
    int status = 0;

    // Expected maximum encoded size:
    // 1 byte for MyMessage.foo tag
    // 1-5 bytes for MyMessage.foo submsg length
    // 1 byte for SubMessage3.foo tag
    // 5 bytes for SubMessage3.foo value
    // 1 byte for SubMessage3.bar tag
    // 5 bytes for SubMessage3.bar value
    printf("Size: %d\n", (int)MyMessage_size);
    TEST(MyMessage_size == 18);

    return status;
}
