/*
 *  ======== main.c ========
 *  Entry point to wolfcrypt Test Application
 */

/* XDCtools Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>

/* func_args from test.h, so don't have to pull in other junk */
typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

extern int wolfcrypt_test(void* args);

/*
 *  ======== testwolfcrypt ========
 *  Run the wolfcrypt test
 */
void testwolfcrypt(UArg arg0, UArg arg1)
{
    System_printf("Running wolfcrypt tests...\n");
    System_flush();
    wolfcrypt_test((void *)arg0);
    System_printf("Tests completed.\n");

    BIOS_exit(0);
}

/*
 *  ======== main ========
 */
int main(int argc, char** argv)
{
    func_args args;
    args.argc = argc;
    args.argv = argv;
    /* Initialize the defaults and set the parameters. */
    Task_Handle handle;
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.arg0 = (UArg)&args;
    taskParams.stackSize = 65535;
    handle =Task_create(testwolfcrypt, &taskParams, NULL);
    if (handle == NULL) {
        System_printf("main: Failed to create new Task.\n");
        return (-1);
    }

    BIOS_start();
}
