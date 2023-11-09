#include "worker.h"
#include <furi_hal_resources.h>
#include <furi.h>

bool killswitch = false;

int status = 0; //0: idle, 1: running, 2: failure

char* inst = 0;
int instCount = 0;
int instPtr = 0;
int runOpCount = 0;

char* wOutput = 0;
int wOutputPtr = 0;

char* wInput = 0;
int wInputPtr = 0;

uint8_t* bfStack = 0;
int stackPtr = 0;
int stackSize = BF_STACK_INITIAL_SIZE;
int stackSizeReal = 0;

BFApp* wrkrApp = 0;

void killThread() {
    killswitch = true;
}

bool validateInstPtr() {
    if(instPtr > instCount || instPtr < 0) {
        return false;
    }
    return true;
}

bool validateStackPtr() {
    if(stackPtr > stackSize || stackPtr < 0) {
        return false;
    }
    return true;
}

char* workerGetOutput() {
    return wOutput;
}

int getStackSize() {
    return stackSizeReal;
}

int getOpCount() {
    return runOpCount;
}

int getStatus() {
    return status;
}

void initWorker(BFApp* app) {
    wrkrApp = app;

    //rebuild output
    if(wOutput) {
        free(wOutput);
    }
    wOutput = (char*)malloc(BF_OUTPUT_SIZE);
    wOutputPtr = 0;

    //rebuild stack
    if(bfStack) {
        free(bfStack);
    }
    bfStack = (uint8_t*)malloc(BF_STACK_INITIAL_SIZE);
    memset(bfStack, 0x00, BF_STACK_INITIAL_SIZE);
    stackSize = BF_STACK_INITIAL_SIZE;
    stackSizeReal = 0;
    stackPtr = 0;

    //set instructions
    inst = wrkrApp->dataBuffer;
    instCount = wrkrApp->dataSize;
    instPtr = 0;
    runOpCount = 0;

    //set input
    wInput = wrkrApp->inputBuffer;
    wInputPtr = 0;

    //set status
    status = 0;
}

void rShift() {
    runOpCount++;
    stackPtr++;
    if(!validateStackPtr()) {
        status = 2;
        return;
    }

    while(stackPtr > stackSize) {
        stackSize += BF_STACK_STEP_SIZE;
        void* tmp = realloc(bfStack, stackSize);

        if(!tmp) {
            status = 2;
            return;
        }

        memset((tmp + stackSize) - BF_STACK_STEP_SIZE, 0x00, BF_STACK_STEP_SIZE);
        bfStack = (uint8_t*)tmp;
    };
    if(stackPtr > stackSizeReal) {
        stackSizeReal = stackPtr;
    }
}

void lShift() {
    runOpCount++;
    stackPtr--;
    if(!validateStackPtr()) {
        status = 2;
        return;
    }
}

void inc() {
    runOpCount++;
    if(!validateStackPtr()) {
        status = 2;
        return;
    }
    bfStack[stackPtr]++;
}

void dec() {
    runOpCount++;
    if(!validateStackPtr()) {
        status = 2;
        return;
    }
    bfStack[stackPtr]--;
}

void print() {
    runOpCount++;
    wOutput[wOutputPtr] = bfStack[stackPtr];
    wOutputPtr++;
    if(wOutputPtr > (BF_OUTPUT_SIZE - 1)) {
        wOutputPtr = 0;
    }
}

void input() {
    runOpCount++;

    bfStack[stackPtr] = (uint8_t)wInput[wInputPtr];
    if(wInput[wInputPtr] == 0x00 || wInputPtr >= 64) {
        wInputPtr = 0;
    } else {
        wInputPtr++;
    }
}

void loop() {
    runOpCount++;
    if(bfStack[stackPtr] == 0) {
        int loopCount = 1;
        while(loopCount > 0) {
            instPtr++;
            if(!validateInstPtr()) {
                status = 2;
                return;
            }
            if(inst[instPtr] == '[') {
                loopCount++;
            } else if(inst[instPtr] == ']') {
                loopCount--;
            }
        }
    }
}

void endLoop() {
    runOpCount++;
    if(bfStack[stackPtr] != 0) {
        int loopCount = 1;
        while(loopCount > 0) {
            instPtr--;
            if(!validateInstPtr()) {
                status = 2;
                return;
            }
            if(inst[instPtr] == ']') {
                loopCount++;
            } else if(inst[instPtr] == '[') {
                loopCount--;
            }
        }
    }
}

static const NotificationSequence led_on = {
    &message_blue_255,
    &message_do_not_reset,
    NULL,
};

static const NotificationSequence led_off = {
    &message_blue_0,
    NULL,
};

void input_kill(void* _ctx) {
    UNUSED(_ctx);
    killswitch = true;
}

void beginWorker() {
    status = 1;

    //redefined from furi_hal_resources.c
    const GpioPin gpio_button_back = {.port = GPIOC, .pin = LL_GPIO_PIN_13};

    while(inst[instPtr] != 0x00) {
        if(runOpCount % 500 == 0) {
            text_box_set_text(wrkrApp->text_box, workerGetOutput());
            notification_message(wrkrApp->notifications, &led_on);
        }

        //status 2 indicates failure
        if(status == 2) {
            status = 0;
            break;
        }

        //read back button directly to avoid weirdness in furi
        if(killswitch || !furi_hal_gpio_read(&gpio_button_back)) {
            status = 0;
            killswitch = false;
            break;
        }

        switch(inst[instPtr]) {
        case '>':
            rShift();
            break;
        case '<':
            lShift();
            break;

        case '+':
            inc();
            break;

        case '-':
            dec();
            break;

        case '.':
            print();
            break;

        case ',':
            input();
            break;

        case '[':
            loop();
            break;

        case ']':
            endLoop();
            break;

        default:
            break;
        }
        instPtr++;
        if(!validateInstPtr()) {
            status = 0;
            break;
        }
    }

    notification_message(wrkrApp->notifications, &led_off);
    text_box_set_text(wrkrApp->text_box, workerGetOutput());
    status = 0;
}