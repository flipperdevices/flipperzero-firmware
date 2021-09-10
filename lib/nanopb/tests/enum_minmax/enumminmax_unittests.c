#include "unittests.h"
#include "enumminmax.pb.h"

int main()
{
    int status = 0;

    COMMENT("Verify min/max on unsorted enum");
    {
        TEST(_Language_MIN == Language_UNKNOWN);
        TEST(_Language_MAX == Language_SPANISH_ES_MX);
        TEST(_Language_ARRAYSIZE == (Language_SPANISH_ES_MX+1));
    }

    return status;
}
