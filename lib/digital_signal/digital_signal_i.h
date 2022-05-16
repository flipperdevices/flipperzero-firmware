#pragma once

struct DigitalSignal {
  bool start_level;
  uint32_t edge_cnt;
  uint32_t edges_max_cnt;
  float *edge_timings;
};
