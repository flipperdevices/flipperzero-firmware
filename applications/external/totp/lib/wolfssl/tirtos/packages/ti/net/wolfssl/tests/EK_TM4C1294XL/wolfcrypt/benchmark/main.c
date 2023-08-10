/*
 *  ======== main.c ========
 *  Entry point for Benchmark application
 */
/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Swi.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Timer.h>

static int initialized = 0;
static double msTicks = 0;
static Timer_Handle hdl = NULL;

double current_time(int reset);
static void tick(unsigned int arg0);
void msTimer_init(void);
void runBenchmarks(UArg arg0, UArg arg1);
extern int benchmark_test(void* args);

/*
 *  ======== runBenchmarks ========
 *  Run the wolfSSL benchmark application
 */
void runBenchmarks(UArg arg0, UArg arg1)
{
    void *args = NULL;
    msTimer_init();

    System_printf("Running benchmarks...\n");
    System_flush();
    benchmark_test(args);
    System_printf("Benchmarks completed.\n");

    BIOS_exit(0);
}

/*
 *  ======== ticks ========
 *  Keeps track of time in millisec
 */
static void tick(unsigned int arg0)
{
    Swi_disable();
    msTicks++;
    Swi_enable();
}

/*
 *  ======== current_time ========
 *  Returns the time in sec (double precision)
 */
double current_time(int reset)
{
    if (reset) {
        msTicks = 0;
    }

    return (msTicks/1000);
}

/*
 *  ======== msTimer_init ========
 *  Sets up a BIOS timer with millisec period
 */
void msTimer_init(void)
{
    Timer_Params params;

    if (initialized) {
        return;
    }

    Timer_Params_init(&params);
    params.period = 1000;
    params.periodType = Timer_PeriodType_MICROSECS;
    params.runMode = Timer_RunMode_CONTINUOUS;
    params.startMode = Timer_StartMode_AUTO;
    hdl = Timer_create(-1, (ti_sysbios_hal_Timer_FuncPtr)tick,
             &params, NULL);
    if (!hdl) {
        System_abort("msTimer_init: Timer creation failed.\n");
    }

    /* Set flag indicating that initialization has completed */
    initialized = 1;
}

/*
 *  ======== main ========
 */
int main(int argc, char** argv)
{
    /* Initialize the defaults and set the parameters. */
    Task_Handle handle;
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stackSize = 65535;
    handle = Task_create(runBenchmarks, &taskParams, NULL);
    if (handle == NULL) {
        System_printf("main: Failed to create new Task.\n");
    }

    BIOS_start();
}
