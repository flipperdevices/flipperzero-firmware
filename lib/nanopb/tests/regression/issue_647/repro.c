#include <pb_decode.h>
#include <unittests.h>
#include <malloc_wrappers.h>
#include "repro.pb.h"

int main() {
  const uint8_t data[] = {0x08, 0x08, 0x2d};
  int status = 0;
  Repro repro = Repro_init_zero;

  pb_istream_t stream = pb_istream_from_buffer(data, sizeof(data));
  TEST(!pb_decode(&stream, Repro_fields, &repro));
  TEST(get_alloc_count() == 0);

  return status;
}
