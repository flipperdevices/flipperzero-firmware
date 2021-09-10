#include "test.pb.h"
#include "unittests.h"

int main()
{
    int status = 0;
    MyMessage msg = MyMessage_init_default;
    TEST(msg.myfield >= 1.23399f);
    TEST(msg.myfield <= 1.23401f);
    return status;
}
