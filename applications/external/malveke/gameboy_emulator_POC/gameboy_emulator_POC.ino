#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_partition.h>
#include <esp_wifi.h>
#include <esp_eth.h>
#include <stdio.h>
#include "emulator_timer.h"
#include "emulator_rom.h"
#include "emulator_mem.h"
#include "emulator_cpu.h"
#include "emulator_lcd.h"
#include "emulator_sdl.h"
// #include "gbrom.h"
#include <esp32-hal.h>
// #include "pk_yellow.h"
#include "pk_red.h"

// #define PERF_REPORT
#define REPORT_INTERVAL 60

static constexpr uint32_t emulator_cpu_freq = 4200000/4;
static constexpr uint32_t frames_per_sec = 60;
static uint32_t cpu_freq = 0;
static uint32_t cycles_per_frame = 0;
static uint32_t cycles_in_micro_sec = 0;


void setup() {
  // Serial.begin(115200);
  Serial.begin(115200);
  Serial1.begin(250000, SERIAL_8N1, 17, 18);
  Serial.println("welcome to esp32 gameboy with no display.\n");
   // Desactivar WiFi y Bluetooth
  esp_wifi_stop();

  // put your setup code here, to run once:
  int r = rom_init(gb_rom);

  sdl_init();

  printf("ROM OK!\n");

  mem_init();
  printf("Mem OK!\n");

  cpu_init();
  printf("CPU OK!\n");

  cpu_freq = getCpuFrequencyMhz();
  printf("CPU Freq = %u Mhz\n", cpu_freq);
  cpu_freq *= 1000000;

  cycles_per_frame = cpu_freq / frames_per_sec;
  cycles_in_micro_sec = cpu_freq / 1000000;
  printf("cycles_per_frame %d cycles_in_micro_sec %d\n", cycles_per_frame, cycles_in_micro_sec);

}

void loop() {
  bool screen_updated = false;
  #ifdef PERF_REPORT
  uint32_t loop_start = ESP.getCycleCount();
  uint32_t adjust = 100;
  for (int i = 0; i < 3 && adjust >= 100; ++i) {
    uint32_t adjust_start = ESP.getCycleCount();
    NOP();
    uint32_t adjust_end = ESP.getCycleCount();
    // -1 to compensate nop
    adjust = adjust_end - adjust_start - 1;
  }
  assert(adjust < 100);

  static uint32_t prev_loop_exit = 0;
  static int frames_count = 0;
  static uint32_t total_cpu = 0;
  static uint32_t total_lcd = 0;
  static uint32_t total_sdl = 0;
  static uint32_t total_timer = 0;
  static uint32_t total_delay = 0;
  static uint32_t total_outside_loop = 0;
  static int sdl_count = 0;
  static uint32_t emulator_cpu_cycle_begin = 0;
  static int opcode_profile[256];
  static int sample_no = 0;
  uint32_t start_bank_switches = mem_get_bank_switches();
  static uint32_t frame_cycles[REPORT_INTERVAL] = {};
  static int bank_switches[REPORT_INTERVAL] = {};
  #endif
  uint32_t start_frame_cycle = ESP.getCycleCount();
  uint32_t emulator_cpu_cycle = 0;

  while (!screen_updated) {
    #ifdef PERF_REPORT
    auto pc = cpu_get_pc();
    unsigned char opcode = mem_get_byte(pc);

    uint32_t cpu_start = ESP.getCycleCount();
    #endif

    emulator_cpu_cycle = cpu_cycle();

    #ifdef PERF_REPORT
    uint32_t lcd_start = ESP.getCycleCount();
    uint32_t cpu_end = lcd_start;
    #endif

    screen_updated = lcd_cycle(emulator_cpu_cycle);

    #ifdef PERF_REPORT
    uint32_t timer_start = ESP.getCycleCount();
    uint32_t lcd_end = timer_start;
    #endif

    timer_cycle(emulator_cpu_cycle);

    #ifdef PERF_REPORT
    uint32_t timer_end = ESP.getCycleCount();

    total_cpu += cpu_end - cpu_start - adjust;
    if (cpu_end - cpu_start - adjust > 1000000) {
      printf("cpu timer seems incorrect:\n    end %u, start %u, adjust %u\n", cpu_end, cpu_start, adjust);
    }
    total_lcd += lcd_end - lcd_start - adjust;
    total_timer += timer_end - timer_start - adjust;
    opcode_profile[opcode] += cpu_end - cpu_start - adjust;
    #endif
  }

  #ifdef PERF_REPORT
  uint32_t sdl_start = ESP.getCycleCount();
  #endif

  sdl_update();

  #ifdef PERF_REPORT
  uint32_t sdl_end = ESP.getCycleCount();
  uint32_t delay_start = sdl_end; 
  #endif

  // delay next frame
  uint32_t end_frame_cycle = ESP.getCycleCount();
  uint32_t cycles_delta = end_frame_cycle - start_frame_cycle;
  if (cycles_delta < cycles_per_frame){
    delayMicroseconds(cycles_delta / cycles_in_micro_sec);
  }

  #ifdef PERF_REPORT
  uint32_t delay_end = ESP.getCycleCount();

  total_outside_loop += loop_start - prev_loop_exit - adjust;

  total_sdl += sdl_end - sdl_start - adjust;
  total_delay += delay_end - delay_start - adjust;
  frame_cycles[frames_count] = total_delay + total_timer + total_sdl + total_lcd + total_cpu;
  assert(frame_cycles[frames_count] < 1000000000);
  bank_switches[frames_count] = mem_get_bank_switches() - start_bank_switches;

  frames_count += 1;
  sdl_count += 1;
  if (frames_count >= REPORT_INTERVAL) {
    uint32_t min_cycles_per_frame = frame_cycles[0];
    uint32_t max_cycles_per_frame = frame_cycles[0];
    uint32_t avg_cycles_per_frame = 0;
    int total_bank_switches = 0;
    for (int i = 0; i < REPORT_INTERVAL; ++i) {
      min_cycles_per_frame = std::min(min_cycles_per_frame, frame_cycles[i] - frame_cycles[i-1]);
      max_cycles_per_frame = std::max(max_cycles_per_frame, frame_cycles[i] - frame_cycles[i-1]);
      total_bank_switches += bank_switches[i];
    }
    avg_cycles_per_frame = frame_cycles[REPORT_INTERVAL - 1];
    if (avg_cycles_per_frame > 1000000000) {
      printf("avg_cycles_per_frame look incorrect\n  frame cycles: ");
      for (int j = 0; j < REPORT_INTERVAL; ++j)
        printf(" %u,", frame_cycles[j]);
    }
    assert(avg_cycles_per_frame < 1000000000);
    avg_cycles_per_frame /= frames_count;

    assert(sdl_count == frames_count);
    printf("sample no: %d\n", sample_no);
    printf("cpu avg: %d\n", total_cpu/frames_count);
    printf("lcd avg: %d\n", total_lcd/frames_count);
    printf("sdl avg: %d\n", total_sdl/sdl_count);
    printf("timer avg: %d\n", total_timer/frames_count);
    printf("delay avg: %d\n", total_delay/frames_count);
    printf("outside loop avg: %d\n", total_outside_loop/frames_count);
    uint32_t host_cycles = total_cpu+total_lcd+total_sdl+total_timer;
    uint32_t emulated_cycles = emulator_cpu_cycle - emulator_cpu_cycle_begin;
    float perf_ratio = ((float)emulator_cpu_freq/cpu_freq) * host_cycles / emulated_cycles;
    printf("emulator/real hardware ratio: %f\n", perf_ratio);
    printf("emulated cycles: %d\n", emulated_cycles);
    printf("average cycles per frame: %d\n", avg_cycles_per_frame);
    printf("min cycles per frame: %d\n", min_cycles_per_frame);
    printf("max cycles per frame: %d\n", max_cycles_per_frame);
    printf("bank switches: %d\n", total_bank_switches);

    int longest_opcode = 0;
    int opcode_cycles = opcode_profile[0];
    for (int i = 0; i < sizeof(opcode_profile)/sizeof(int); ++i) {
      if (opcode_profile[i] > opcode_cycles) {
        opcode_cycles = opcode_profile[i];
        longest_opcode = i;
      }
      opcode_profile[i] = 0;
    }
    printf("longest opcode: %d, took %d cycles\n\n", longest_opcode, opcode_cycles);

    frames_count = 0;
    sdl_count = 0;
    emulator_cpu_cycle_begin = emulator_cpu_cycle;
    total_cpu = total_lcd = total_timer = total_sdl = total_delay = total_outside_loop = 0;
    sample_no++;
  }
  prev_loop_exit = ESP.getCycleCount();
  #endif
}