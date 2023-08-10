#include "RTOS.h"
#include "BSP.h"
#include <stdio.h>
#include <wolfcrypt/benchmark/benchmark.h>

static OS_STACKPTR int WLFSTACK[20000];   /* Stack Size for Task */
static OS_TASK WLFTASK;                   /* Task */

static void wolfTask(void) {
  printf("Begin Benchmark Tests\n");
  benchmark_test(NULL);
  printf("Benchmark Tests Complete\n");
  while (1) {
    BSP_ToggleLED(1);
    OS_Delay(200);
  }
}


int main(void) {
  OS_IncDI();                      /* Disable interrupts */
  OS_InitKern();                   /* Initialize OS */
  OS_InitHW();                     /* Initialize Hardware */
  BSP_Init();                      /* Initialize LED ports */
  /* You need to create at least one task before calling OS_Start() */
  OS_CREATETASK(&WLFTASK, "Tests task", wolfTask, 100, WLFSTACK);
  OS_Start();                      /* Start the OS */
  return 0;
}
