#include "RTOS.h"
#include "BSP.h"
#include <stdio.h>
#include <wolfcrypt/test/test.h>

static OS_STACKPTR int WLFSTACK[20000];   /* Stack Size for Task */
static OS_TASK WLFTASK;                   /* Task */

static void wolfTask(void) {
  printf("Begin wolfcrypt tests\n");
  wolfcrypt_test(NULL);
  printf("wolfcrypt tests complete.\n");
  while (1) {
    BSP_ToggleLED(1);
    OS_Delay(200);
  }
}


int main(void) {
  OS_IncDI();                      /* Disable interrupts */
  OS_InitKern();                   /* Initialize OS */
  OS_InitHW();                     /* Initialize Hardware for OS */
  BSP_Init();                      /* Initialize LED ports */
  /* You need to create at least one task before calling OS_Start() */
  OS_CREATETASK(&WLFTASK, "Tests task", wolfTask, 100, WLFSTACK);
  OS_Start();                      /* Start the OS */
  return 0;
}