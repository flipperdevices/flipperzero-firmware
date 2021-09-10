#include <stdio.h>
#include "unittests.h"
#include "enum.pb.h"

int main()
{
    int status = 0;
    TEST(strcmp(MyEnum_name(MyEnum_VALUE1), "VALUE1") == 0);
    TEST(strcmp(MyEnum_name(MyEnum_VALUE2), "VALUE2") == 0);
    TEST(strcmp(MyEnum_name(MyEnum_VALUE15), "VALUE15") == 0);
    TEST(strcmp(MyShortNameEnum_name(MSNE_VALUE256), "MSNE_VALUE256") == 0);
    TEST(strcmp(MyShortNameEnum_name(9999), "unknown") == 0);
    
    if (status != 0)
        fprintf(stdout, "\n\nSome tests FAILED!\n");

    return status;
}

