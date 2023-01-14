#include "brainfuck_i.h"

char* wOutput = 0;

uint32_t* bfStack = 0;
int stackSize = BF_STACK_INITIAL_SIZE;

char* workerGetOutput(){
    return wOutput;
}

void initWorker(){
    wOutput = (char*)malloc(BF_OUTPUT_SIZE);
    bfStack = (uint32_t*)malloc(BF_STACK_INITIAL_SIZE);
}

void beginWorker(){

}