/* Tests for floating point default values +-infinity and nan. */
#undef __STRICT_ANSI__
#include <math.h>
#include <pb_decode.h>
#include "floats.pb.h"
#include "unittests.h"

bool check_floats(Floats *floats)
{
    int status = 0;

    TEST(!floats->has_float_pos_inf);
    TEST(isinf(floats->float_pos_inf));
    TEST(!signbit(floats->float_pos_inf));

    TEST(!floats->has_float_neg_inf);
    TEST(isinf(floats->float_neg_inf));
    TEST(signbit(floats->float_neg_inf));

    TEST(!floats->has_float_pos_nan);
    TEST(isnan(floats->float_pos_nan));

    TEST(!floats->has_float_neg_nan);
    TEST(isnan(floats->float_neg_nan));

    TEST(!floats->has_double_pos_inf);
    TEST(isinf(floats->double_pos_inf));
    TEST(!signbit(floats->double_pos_inf));

    TEST(!floats->has_double_neg_inf);
    TEST(isinf(floats->double_neg_inf));
    TEST(signbit(floats->double_neg_inf));

    TEST(!floats->has_double_pos_nan);
    TEST(isnan(floats->double_pos_nan));

    TEST(!floats->has_double_neg_nan);
    TEST(isnan(floats->double_neg_nan));

    return status == 0;
}

int main()
{
    int status = 0;

    {
        Floats floats = Floats_init_default;
        COMMENT("Checking init_default");
        TEST(check_floats(&floats));
    }

    {
        Floats floats = {0};
        pb_istream_t stream = pb_istream_from_buffer(NULL, 0);
        COMMENT("Check decoded");
        TEST(pb_decode(&stream, Floats_fields, &floats));
        TEST(check_floats(&floats));
    }

    return status;
}
