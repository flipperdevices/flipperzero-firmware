#include <stdlib.h>
#include <stdio.h>
#include "options.pb.h"
#include "unittests.h"

int main()
{
    int status = 0;

    {
        HasFieldMessage msg1 = HasFieldMessage_init_default;
        HasFieldMessage msg2 = HasFieldMessage_init_zero;

        COMMENT("Test default_has option");

        /* Default initializer should obey has_default setting */
        TEST(msg1.has_present == true);
        TEST(msg1.has_missing == false);
        TEST(msg1.has_normal == false);

        /* Zero initializer should always have false */
        TEST(msg2.has_present == false);
        TEST(msg2.has_missing == false);
        TEST(msg2.has_normal == false);
    }

    return status;
}
