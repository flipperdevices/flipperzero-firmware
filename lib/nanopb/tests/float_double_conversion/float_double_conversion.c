#define _USE_MATH_DEFINES 1
#undef __STRICT_ANSI__
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include "doublemsg.pb.h"
#include "unittests.h"

/* This message mimics how DoubleMsg would appear on e.g. AVR. */
typedef struct {
    float value;
} FloatMsg;
PB_BIND(DoubleMsg, FloatMsg, AUTO)

static const double testvalues[] = {
           0.0,        -0.0,         0.1,         -0.1,
          M_PI,       -M_PI,  123456.789,  -123456.789,
#if defined(NAN) && defined(INFINITY)
      INFINITY,   -INFINITY,         NAN, INFINITY - INFINITY,
#endif
          1e38,       -1e38,        1e39,        -1e39,
         1e-38,      -1e-38,       1e-39,       -1e-39,
   3.14159e-37,-3.14159e-37, 3.14159e-43, -3.14159e-43,
         1e-60,      -1e-60,       1e-45,       -1e-45,
    0.99999999999999, -0.99999999999999, 127.999999999999, -127.999999999999
};

#define TESTVALUES_COUNT (sizeof(testvalues)/sizeof(testvalues[0]))

int main()
{
    uint8_t buf[16];
    size_t msglen;
    int status = 0;
    int i;
    for (i = 0; i < TESTVALUES_COUNT; i++)
    {
        double orig_double = testvalues[i];
        float expected_float = (float)orig_double;
        double expected_double = (double)expected_float;

        printf("\n---- Testcase: %f ----\n", expected_float);

        {
            /* Encode the original double */
            pb_ostream_t stream = pb_ostream_from_buffer(buf, sizeof(buf));
            DoubleMsg msg = { 0.0 };
            msg.value = orig_double;
            TEST(pb_encode(&stream, &DoubleMsg_msg, &msg));
            msglen = stream.bytes_written;
            TEST(msglen == 9);
        }

        {
            /* Decode as float */
            pb_ostream_t ostream;
            pb_istream_t stream = pb_istream_from_buffer(buf, msglen);
            FloatMsg msg = { 0.0f };
            TEST(pb_decode(&stream, &FloatMsg_msg, &msg));
            TEST(memcmp(&msg.value, &expected_float, sizeof(float)) == 0);

            /* Re-encode */
            ostream = pb_ostream_from_buffer(buf, sizeof(buf));
            TEST(pb_encode(&ostream, &FloatMsg_msg, &msg));
            msglen = ostream.bytes_written;
            TEST(msglen == 9);
        }

        {
            /* Decode as double */
            pb_istream_t stream = pb_istream_from_buffer(buf, msglen);
            DoubleMsg msg = { 0.0 };
            TEST(pb_decode(&stream, &DoubleMsg_msg, &msg));

            if (isnan(expected_double))
            {
                /* Bottom bits of NAN converted to double can vary */
                TEST(isnan(msg.value));
            }
            else
            {
                TEST(memcmp(&msg.value, &expected_double, sizeof(double)) == 0);
            }
        }
    }

    return status;
}
