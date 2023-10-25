#include "lfrfid_hitag_worker.h"

#define DMA_BUFFER_SIZE \
    4000 //I need to keep this big enough? to have enough time inbetween HT & TC events perhaps?
#define HITAG_CMD_BUFFER_SIZE 100 //this was originally 1000, but can be lowered

#define READ_BUFFER_SIZE 4000
#define READ_BUFFER_COUNT 4

#define EMULATE_BUFFER_SIZE 50
#define EMULATE_BUFFER_COUNT 50

//bitlength/ID for read cmd structure
#define HITAG_STOP 102
#define HITAG_ON 101
#define HITAG_OFF 100
#define HITAG_SET 5
#define HITAG_SELECT 5
#define HITAG_CMD 4
#define HITAG_BYTE 8
#define HITAG_CRC 8

//carrier periods for read cmd structure
#define HITAG_LOW 7
#define HITAG_STOP_HIGH 40
#define HITAG_1_HIGH 21
#define HITAG_0_HIGH 15

//bitlength for emulate reply cmd structure
#define HITAG_PAGE 32
#define HITAG_STARTBIT 1
#define HITAG_STARTBIT3 3
#define HITAG_STARTBIT6 6
#define HITAG_ACK 3
#define HITAG_ACK_ADV 8

#define HITAG_BASEPERIOD 8

//carrier periods for read reply decoding
#define HITAG_DURATION_S 256
#define HITAG_DURATION_M 384
#define HITAG_DURATION_L 512
#define HITAG_DURATION_ERROR_MARGIN 0.15

#define HITAG_BITPERIODS_AC 64
#define HITAG_BITPERIODS_MC 32

//MISC
#define READ 1
#define WRITE 2

#define CRC_PRESET 0xFF
#define CRC_POLYNOM 0x1D

#define TAG "LFRFIDHitagWorker"

typedef enum {
    HitagProtocolAntiCollision,
    HitagProtocolManchesterCoding,
} LfRfidHitagProtocol;

typedef enum {
    LFRFIDHitagWorkerSignalHalfTransfer,
    LFRFIDHitagWorkerSignalTransferComplete,
    LFRFIDHitagWorkerSignalStop,
} LFRFIDHitagWorkerSignal;

typedef enum {
    HitagStateIdle,
    HitagStateSelected,
    HitagStateQuiet,
} LfRfidHitagState;

typedef enum {
    HitagModeBasic,
    HitagModeAdvanced,
} LfRfidHitagMode;

typedef struct {
    uint32_t timer_buffer_arr[DMA_BUFFER_SIZE];
    uint32_t timer_buffer_ccr[DMA_BUFFER_SIZE];
} TimerDMAData;

typedef struct {
    uint32_t CMDvalue[HITAG_CMD_BUFFER_SIZE]; //this can likely be a uint16_t instead of uint32_t
    uint8_t CMDtype[HITAG_CMD_BUFFER_SIZE];
    uint16_t CMDlength;
    uint16_t CMDposition;
    uint16_t CMDsubposition;
    bool CMDloop;
    bool CMDactive;
    uint32_t CMDcount;
} LfRfidHitagCMDData;

typedef struct {
    uint32_t replyvalue[HITAG_CMD_BUFFER_SIZE]; //this can likely be a uint8_t instead of uint32_t
    uint8_t replytype[HITAG_CMD_BUFFER_SIZE];
    uint16_t replylength;
} LfRfidHitagReplyData;

typedef struct {
    BufferStream* stream;
    VarintPair* pair;
    uint32_t capCounter;
} LfRfidHitagCaptureData;

typedef struct {
    uint8_t pageData[HITAG_PAGES * HITAG_PAGEBYTES];
    uint8_t pageKnown[HITAG_PAGES];
    uint8_t tagData[HITAG_PAGES * HITAG_PAGEBYTES + HITAG_PAGES];
    uint8_t pageRW[HITAG_PAGES];
    uint8_t pagePublic[HITAG_PAGES];
} LFRFIDHitag;

// main worker
struct LFRFIDHitagWorker {
    FuriThread* thread;
    FuriEventFlag* events;
    uint8_t DMAeventCount;

    uint8_t carrierPrescaler;

    LFRFIDHitag* tag;

    LFRFIDHitagStatus workerstatus;

    ProtocolDict* dict;
};

//------------------------------------------------------------------ forward function definitions ------------------------------------------------------------------

static int32_t lfrfid_hitag_worker_emulate_thread(void* thread_context);
static int32_t lfrfid_hitag_worker_read_thread(void* thread_context);

//------------------------------------------------------------------ shared functions ------------------------------------------------------------------

LFRFIDHitagWorker* lfrfid_hitag_worker_alloc(ProtocolDict* dict) {
    furi_assert(dict);

    LFRFIDHitagWorker* worker = malloc(sizeof(LFRFIDHitagWorker));
    worker->dict = dict;

    worker->thread = furi_thread_alloc_ex("LFRFIDHitagWorker", 2048, NULL, worker);

    worker->events = furi_event_flag_alloc(NULL);
    worker->DMAeventCount = 0;

    worker->carrierPrescaler = 2;

    worker->workerstatus = LFRFIDHitagStatusScanning;

    return worker;
}

void lfrfid_hitag_worker_free(LFRFIDHitagWorker* worker) {
    furi_thread_free(worker->thread);
    furi_event_flag_free(worker->events);
    free(worker);
}

LFRFIDHitagStatus lfrfid_hitag_worker_get_status(LFRFIDHitagWorker* worker) {
    return worker->workerstatus;
}

void lfrfid_hitag_worker_start(LFRFIDHitagWorker* worker, LFRFIDHitagWorkerSetting setting) {
    if(furi_thread_get_state(worker->thread) == FuriThreadStateStopped) {
        switch(setting) {
        case(LFRFIDHitagWorkerSettingRead):
            dolphin_deed(DolphinDeedRfidRead);
            furi_thread_set_callback(worker->thread, lfrfid_hitag_worker_read_thread);
            break;
        case(LFRFIDHitagWorkerSettingEmulate):
            dolphin_deed(DolphinDeedRfidEmulate);
            furi_thread_set_callback(worker->thread, lfrfid_hitag_worker_emulate_thread);
            break;
        default: //safety meassure, not to start a thread without a callback function
            furi_thread_set_callback(worker->thread, lfrfid_hitag_worker_read_thread);
            break;
        }
        furi_thread_start(worker->thread);
    }
}

void lfrfid_hitag_worker_stop(LFRFIDHitagWorker* worker) {
    furi_event_flag_set(worker->events, 1 << LFRFIDHitagWorkerSignalStop);
    furi_thread_join(worker->thread);
}

//------------------------------------------------------------------ READ TAG: TIMER & DMA callback functions ------------------------------------------------------------------
static void lfrfid_hitag_worker_capture_in_cc_callback(bool level, uint32_t duration, void* context) {
    LfRfidHitagCaptureData* capData = context;

    //check if there is a new pair available: pulse + period
    bool need_to_send = varint_pair_pack(capData->pair, level, duration);

    //if so put it on buffer stream so it can be processed outside this interrupt routine
    if(need_to_send) {
        buffer_stream_send_from_isr(
            capData->stream,
            varint_pair_get_data(capData->pair),
            varint_pair_get_size(capData->pair));
        varint_pair_reset(capData->pair);
    }
}

//leave this in hitag_worker
static void lfrfid_hitag_worker_carrier_out_dma_callback(bool half, void* dma_context) {
    LFRFIDHitagWorker* worker = (LFRFIDHitagWorker*)dma_context;
    worker->DMAeventCount++;
	
    if (half){
        furi_event_flag_set(worker->events, 1 << LFRFIDHitagWorkerSignalHalfTransfer);
    } else {
        furi_event_flag_set(worker->events, 1 << LFRFIDHitagWorkerSignalTransferComplete);
    }
}


//------------------------------------------------------------------ EMULATE TAG: TIMER callback functions)------------------------------------------------------------------
static void lfrfid_hitag_worker_carrier_in_callback(bool level, uint32_t duration, void* capture_context){
    UNUSED(level);

    LfRfidHitagCaptureData* capData = capture_context;
    capData->capCounter++;
    //pack varint (as part of a pair)
    //cleaner would be to pack as a varint iso varint_pair, though i'm not sure if I can get the data & size of a packed varint
    varint_pair_pack(capData->pair, true, duration);
    //but send anyhow, doesn't matter that it's not a pair
    buffer_stream_send_from_isr(
        capData->stream,
        varint_pair_get_data(capData->pair),
        varint_pair_get_size(capData->pair));
    varint_pair_reset(capData->pair);
}

//------------------------------------------------------------------ hitag data parsing functions ------------------------------------------------------------------

static void lfrfid_hitag_worker_yield_dma_buffer_BPLM(
    LfRfidHitagCMDData* dataCMD,
    LfRfidHitagCMDData* backupCMD,
    TimerDMAData* dataDMA,
    size_t start,
    size_t len) {
    //based on interal timer with freq of 1MHz (period of 1us)
    //use active command
    LfRfidHitagCMDData* data = dataCMD;
    if(!data->CMDactive) {
        data = backupCMD;
    }
    uint16_t lastarrposition = 0;
    size_t toskip = 0;
    bool itemProcessed = true;
    size_t s = start;
    //for ( ; data->CMDloop || data->CMDposition < data->CMDlength ; data->CMDposition++){
    while(1) {
        //at the end of current command (real or backup), check if there's unprocessed CMD available, if not switch to backup
        if(data->CMDposition == data->CMDlength) {
            data->CMDcount++;
            if(dataCMD->CMDposition <
               dataCMD
                   ->CMDlength) { //currently backup & new unprocessed command available --> switch over to real CMD
                data->CMDactive = false;
                data = dataCMD;
                data->CMDactive = true;
                data->CMDcount = 0;
            } else if(dataCMD
                          ->CMDloop) { //if looping (real or backup) --> reset real (current) CMD to startposition
                data->CMDposition = 0;
                data->CMDsubposition = 0;
            } else if(dataCMD
                          ->CMDactive) { //real command and not looping --> switch to backup CMD and reset backup CMD to startposition
                data->CMDactive = false;
                data = backupCMD;
                data->CMDactive = true;
                data->CMDposition = 0;
                data->CMDsubposition = 0;
                data->CMDcount = 0;
            } else { //backupCMD but set to not loop --> loop anyway: reset to startposition
                data->CMDposition = 0;
                data->CMDsubposition = 0;
            }
        }
        //start processing the next CMD item
        lastarrposition = s;
        if(s == start) {
            toskip = data->CMDsubposition;
        } else {
            toskip = 0;
            data->CMDsubposition = 0;
        }
        switch(data->CMDtype[data->CMDposition]) {
        case HITAG_STOP: //just 1 STOP cycle (HITAG_LOW lows + HITAG_STOP_HIGH highs)
            if(s > start) {
                dataDMA->timer_buffer_arr[s - 1] += HITAG_LOW * HITAG_BASEPERIOD;
            }
            for(size_t t = 0; t < HITAG_STOP_HIGH - toskip; t++) {
                if(s == len + start) {
                    //arrays are filled, do not continue out
                    itemProcessed = false;
                    break;
                }
                dataDMA->timer_buffer_ccr[s] = HITAG_BASEPERIOD / 2;
                dataDMA->timer_buffer_arr[s] = HITAG_BASEPERIOD - 1;
                s++;
            }
            break;
        case HITAG_ON: //x ON cycles --> update next X arr/ccr values
            for(size_t x = 0; x < data->CMDvalue[data->CMDposition] - toskip; x++) {
                if(s == len + start) {
                    //arrays are filled, do not continue out
                    itemProcessed = false;
                    break;
                }
                dataDMA->timer_buffer_ccr[s] = HITAG_BASEPERIOD / 2;
                dataDMA->timer_buffer_arr[s] = HITAG_BASEPERIOD - 1;
                s++;
            }
            break;
        case HITAG_OFF: //x OFF cycles (update previous arr value)
            if(s > start) {
                dataDMA->timer_buffer_arr[s - 1] +=
                    data->CMDvalue[data->CMDposition] * HITAG_BASEPERIOD;
            }
            break;
        default: // <=HITAG_BYTE: //bits to be sent
            //get each bit & update arr/ccr accordingly (7 lows + HITAG_0_HIGH/HITAG_1_HIGH highs)
            for(size_t b = 8 - data->CMDtype[data->CMDposition]; b < 8; b++) {
                if(s > start) {
                    dataDMA->timer_buffer_arr[s - 1] += HITAG_LOW * HITAG_BASEPERIOD;
                }
                if(0x80 & (data->CMDvalue[data->CMDposition] << b)) {
                    for(size_t t = 0; t < HITAG_1_HIGH; t++) {
                        if(s == len + start) {
                            //arrays are filled, do not continue out
                            itemProcessed = false;
                            break;
                        }
                        dataDMA->timer_buffer_ccr[s] = HITAG_BASEPERIOD / 2;
                        dataDMA->timer_buffer_arr[s] = HITAG_BASEPERIOD - 1;
                        s++;
                        if(toskip > 0) {
                            toskip--;
                            s--;
                        }
                    }
                } else {
                    for(size_t t = 0; t < HITAG_0_HIGH; t++) {
                        if(s == len + start) {
                            //arrays are filled, do not continue out
                            itemProcessed = false;
                            break;
                        }
                        dataDMA->timer_buffer_ccr[s] = HITAG_BASEPERIOD / 2;
                        dataDMA->timer_buffer_arr[s] = HITAG_BASEPERIOD - 1;
                        s++;
                        if(toskip > 0) {
                            toskip--;
                            s--;
                        }
                    }
                }
                if(!itemProcessed) {
                    break;
                }
            }
            break;
        }
        if(!itemProcessed) {
            break;
        }
        data->CMDposition++;
    }

    //set starting position for next time:
    data->CMDsubposition += (len + start) - lastarrposition;
}

static uint16_t lfrfid_hitag_worker_yield_dma_buffer_MC(
    LfRfidHitagReplyData* dataReply,
    TimerDMAData* dataDMA,
    uint8_t carrierPrescaler) {
    //based on external carrier with freq of 125kHz (period of 8us)

    uint16_t dma_len = 0;

    bool bit = false;
    bool prevBit = true; //must be true to initialze MC yield for first bit

    //process reply bits and add HIGH-LOW for 1 or LOW-HIGH for 0
    for(uint8_t i = 0; i < dataReply->replylength; i++) {
        for(int8_t b = dataReply->replytype[i] - 1; b >= 0; b--) {
            bit = (dataReply->replyvalue[i] >> b & 0b00000001);
            if(bit) { //HIGH-LOW
                if(prevBit) { //if first bit (initial value of prevBit=true) or if prev bit was 1: start of new default cycle
                    dataDMA->timer_buffer_arr[dma_len] = 32 - 1;
                    dataDMA->timer_buffer_ccr[dma_len] = 16;
                    dma_len++;
                } else { //previous bit was 0, so extend the ccr and arr with 16
                    dataDMA->timer_buffer_arr[dma_len - 1] += 16;
                    dataDMA->timer_buffer_ccr[dma_len - 1] += 16;
                }
            } else { //LOW-HIGH
                if(prevBit) { //previous bit was 1 so extend the LOW (arr only) of prev cycle with 16 and add new default cycle
                    dataDMA->timer_buffer_arr[dma_len - 1] += 16;

                    dataDMA->timer_buffer_arr[dma_len] = 32 - 1;
                    dataDMA->timer_buffer_ccr[dma_len] = 16;
                    dma_len++;
                } else { //previous bit was 0 so just add new default cycle
                    dataDMA->timer_buffer_arr[dma_len] = 32 - 1;
                    dataDMA->timer_buffer_ccr[dma_len] = 16;
                    dma_len++;
                }
            }
            prevBit = bit;
        }
    }

    //add extra period, to reset timer back to normal carrier_in detection after reply has finished
    //also required to make sure the last real dma values are fully executed before dma signals transfer complete, which resets worker into command detection
    dataDMA->timer_buffer_arr[dma_len] = carrierPrescaler - 1;
    dataDMA->timer_buffer_ccr[dma_len] = 0;
    dma_len++;

    return dma_len;
}

static uint16_t lfrfid_hitag_worker_yield_dma_buffer_AC(
    LfRfidHitagReplyData* dataReply,
    TimerDMAData* dataDMA,
    uint8_t carrierPrescaler) {
    //based on external carrier with freq of 125kHz (period of 8us)

    uint16_t dma_len = 0;

    //process reply bits and add 2 short pulses for 1 or 1 long pulse for 0
    for(uint8_t i = 0; i < dataReply->replylength; i++) {
        for(int8_t b = dataReply->replytype[i] - 1; b >= 0; b--) {
            if(dataReply->replyvalue[i] >> b & 0b00000001) {
                dataDMA->timer_buffer_arr[dma_len] = 32 - 1;
                dataDMA->timer_buffer_ccr[dma_len] = 16; //16
                dma_len++;
                dataDMA->timer_buffer_arr[dma_len] = 32 - 1;
                dataDMA->timer_buffer_ccr[dma_len] = 16; //16
                dma_len++;
            } else {
                dataDMA->timer_buffer_arr[dma_len] = 64 - 1;
                dataDMA->timer_buffer_ccr[dma_len] = 32; //32
                dma_len++;
            }
        }
    }

    //add extra period, to reset timer back to normal carrier_in detection after reply has finished
    //also required to make sure the last real dma values are fully executed before dma signals transfer complete, which resets worker into command detection
    dataDMA->timer_buffer_arr[dma_len] = carrierPrescaler - 1;
    dataDMA->timer_buffer_ccr[dma_len] = 0;
    dma_len++;

    return dma_len;
}

static void lfrfid_hitag_worker_decoder_feed(
    uint32_t pulse,
    uint32_t duration,
    uint32_t* memBlock,
    uint8_t* bits,
    bool* half,
    bool* finished,
    LfRfidHitagProtocol protocol,
    uint8_t startBits) {
    uint32_t bit = 1 << (32 - 1);
    if(*bits == 0) {
        *half = false;
    }
    if(protocol == HitagProtocolManchesterCoding) {
        if(HITAG_DURATION_S * (1 - HITAG_DURATION_ERROR_MARGIN) <= duration &&
           duration <= HITAG_DURATION_S * (1 + HITAG_DURATION_ERROR_MARGIN)) {
            //short period (HL) detected: same bit as previous
            if(*bits < startBits) {
                //don't store startbits in memory, just increase bitcounter
                (*bits)++;
            } else if(*bits == startBits) {
                //same as previous bit (last startbit it 1, so add 1 to the memBlock)
                memBlock[(*bits - startBits) / 32] |= (bit >> ((*bits - startBits) % 32)); //add 1
                (*bits)++;
            } else {
                //add same as previous bit
                if(memBlock[(*bits - startBits - 1) / 32] &
                   (bit >> ((*bits - startBits - 1) % 32))) {
                    memBlock[(*bits - startBits) / 32] |=
                        (bit >> ((*bits - startBits) % 32)); //add 1
                    (*bits)++;
                } else {
                    (*bits)++; //no mem action for 0, just increase bitcounter
                }
            }
        } else if(
            HITAG_DURATION_M * (1 - HITAG_DURATION_ERROR_MARGIN) <= duration &&
            duration <= HITAG_DURATION_M * (1 + HITAG_DURATION_ERROR_MARGIN)) {
            //medium period (H|HL or HL|L) detected: half 0 + full 1 or full 1 + half 0 detected
            if(*bits < startBits) {
                //assuming all startbits are 1's, this can only happen on last startbit followed by 0
                //don't store startbits, no mem action for 0, just increase bitcounter twice
                (*bits)++;
                (*bits)++;
                *half = true;
            } else if(*bits == startBits) {
                //assuming all startbits were 1's, this is a data 10 detection
                memBlock[(*bits - startBits) / 32] |= (bit >> ((*bits - startBits) % 32)); //add 1
                (*bits)++;
                (*bits)++; //no mem action for 0, just increase bitcounter
                *half = true;
            } else {
                //if we're in half of a bit, add 1, else add 10 and set half flag
                if(*half) {
                    memBlock[(*bits - startBits) / 32] |=
                        (bit >> ((*bits - startBits) % 32)); //add 1
                    (*bits)++;
                    *half = false;
                } else {
                    memBlock[(*bits - startBits) / 32] |=
                        (bit >> ((*bits - startBits) % 32)); //add 1
                    (*bits)++;
                    (*bits)++; //no mem action for 0, just increase bitcounter
                    *half = true;
                }
            }
        } else if(
            HITAG_DURATION_L * (1 - HITAG_DURATION_ERROR_MARGIN) <= duration &&
            duration <= HITAG_DURATION_L * (1 + HITAG_DURATION_ERROR_MARGIN)) {
            //long period (H|HL|L) detected: half 0 + full 1 + half 0 detected
            if(*bits <= startBits) {
                //TODO: throw error since this can't happen till after first real bit, since all startbits should be 1's
            } else {
                //TODO check that prev bit is indeed 0
                //add 10 to the memBlock
                memBlock[(*bits - startBits) / 32] |= (bit >> ((*bits - startBits) % 32)); //add 1
                (*bits)++;
                (*bits)++; //no mem action for 0, just increase bitcounter
            }
        } else if(HITAG_DURATION_L * (1 + HITAG_DURATION_ERROR_MARGIN) < duration) {
            //end of modulation by tag, check if this pulse duration includes a bit
            if(*bits > 0) {
                if(*half && pulse < HITAG_DURATION_S) {
                    //second half of 0 bit, no action
                } else if(*half && pulse < HITAG_DURATION_M) {
                    //second half or 0 bit + another 1 bit
                    memBlock[(*bits - startBits) / 32] |=
                        (bit >> ((*bits - startBits) % 32)); //add 1
                    (*bits)++;
                } else if(!(*half) && pulse < HITAG_DURATION_S) {
                    //another 1 bit
                    memBlock[(*bits - startBits) / 32] |=
                        (bit >> ((*bits - startBits) % 32)); //add 1
                    (*bits)++;
                } else {
                    //invalid pulse duration --> reset?
                }
                *finished = true;
            }
        } else {
            //invalid pulse duration: reset tempData if necessary
            if(*bits > 0) {
                memset(memBlock, 0, HITAG_BLOCKPAGES * sizeof(memBlock[0]));
                *bits = 0;
                *finished = false;
            }
        }
    } else if(protocol == HitagProtocolAntiCollision) {
        if(HITAG_DURATION_S * (1 - HITAG_DURATION_ERROR_MARGIN) <= duration &&
           duration <= HITAG_DURATION_S * (1 + HITAG_DURATION_ERROR_MARGIN)) {
            //short period (HL): first or second half of a 1 detected
            //if first short: add 1 to memBlock and increase half counter
            if(*half == false) {
                if(*bits >= startBits) {
                    memBlock[(*bits - startBits) / 32] |= ((bit >> (*bits - startBits) % 32));
                }
                (*bits)++;
                *half = true;
            } else {
                //if second small: reset half counter
                *half = false;
            }
        } else if(
            HITAG_DURATION_L * (1 - HITAG_DURATION_ERROR_MARGIN) <= duration &&
            duration <= HITAG_DURATION_L * (1 + HITAG_DURATION_ERROR_MARGIN)) {
            //long period (HHLL): 0 detected
            //should only happen after full detection of 1 or after startBits
            if(*bits < startBits || *half) {
                //reset
                memset(memBlock, 0, HITAG_BLOCKPAGES * sizeof(memBlock[0]));
                *bits = 0;
                *finished = false;
            } else {
                (*bits)++; //add 0 to memBlock
            }
        } else if(HITAG_DURATION_L * (1 + HITAG_DURATION_ERROR_MARGIN) < duration) {
            //end of modulation by tag, check if this pulse duration includes a bit
            if(*bits > 0) {
                if(!(*half) && pulse > HITAG_DURATION_S) {
                    (*bits)++; //add 0 to memBlock
                } else {
                    //invalid pulse duration: reset?
                }
                *finished = true;
            }
        } else {
            //invalid pulse duration: reset tempData if necessary
            if(*bits > 0) {
                memset(memBlock, 0, HITAG_BLOCKPAGES * sizeof(memBlock[0]));
                *bits = 0;
                *finished = false;
            }
        }
    }
}

static void lfrfid_hitag_worker_calc_crc(uint8_t* crc, uint8_t data, uint8_t bitcount) {
    *crc ^= data; // crc = crc (exor) data
    do {
        if(*crc & 0x80) // if (MSB-CRC == 1)
        {
            *crc <<= 1; // CRC = CRC bit-shift left
            *crc ^= CRC_POLYNOM; // CRC = CRC (exor) CRC_POLYNOM
        } else {
            *crc <<= 1; // CRC = CRC bit-shift left
        }
    } while(--bitcount);
}

static bool lfrfid_hitag_worker_validate_command(uint32_t* bits, uint8_t len) {
    if(len == 5 && (bits[0] >> (32 - 5)) == 0b11001) {
        //SET_CCNEW
        return true;
    } else if(len == 5 && (bits[0] >> (32 - 5)) == 0b00110) {
        //SET_CC
        return true;
    } else if(len == 45 && (bits[0] >> (32 - 5)) == 0b00000) {
        //SELECT

        //calculate CRC for first 37 bits (4bytes + 5 bits)
        uint8_t byte1 = bits[0] >> 24 & 0xff;
        uint8_t byte2 = bits[0] >> 16 & 0xff;
        uint8_t byte3 = bits[0] >> 8 & 0xff;
        uint8_t byte4 = bits[0] & 0xff;
        uint8_t byte5 = bits[1] >> 24 & 0b11111000;
        uint8_t crcRead = bits[1] >> 19 & 0xff;

        uint8_t crc = CRC_PRESET;
        lfrfid_hitag_worker_calc_crc(&crc, byte1, 8);
        lfrfid_hitag_worker_calc_crc(&crc, byte2, 8);
        lfrfid_hitag_worker_calc_crc(&crc, byte3, 8);
        lfrfid_hitag_worker_calc_crc(&crc, byte4, 8);
        lfrfid_hitag_worker_calc_crc(&crc, byte5, 5);

        //and compare result with received crc
        return (crcRead == crc);
    }
    //else if (len == 20 && (bits[0]>>31 ||  (bits[0]>>(32-4)) == 0b0111)){
    else if(len == 20 && bits[0] > 0x70000000) {
        //R/W page/block (cmd starting with 1) or HALT (0111) --> so bit0 should be >= 0b01110000 00000000 00000000 00000000 (0x70000000)
        //calculate CRC for first 12 bits (1byte + 4 bits)
        uint8_t byte1 = bits[0] >> 24 & 0xff;
        uint8_t byte2 = bits[0] >> 16 & 0b11110000;
        uint8_t crcRead = bits[0] >> 12 & 0xff;

        uint8_t crc = CRC_PRESET;
        lfrfid_hitag_worker_calc_crc(&crc, byte1, 8);
        lfrfid_hitag_worker_calc_crc(&crc, byte2, 4);

        //and compare result with received crc
        return (crcRead == crc);
    }
    return false;
}

static bool lfrfid_hitag_worker_validate_bits(uint32_t* bits, uint8_t bitcounter) {
    if(47 >= bitcounter && bitcounter >= 45) {
        for(uint8_t i = 0; i < bitcounter - 44; i++) {
            if(i > 0) {
                bits[0] = bits[0] << 1 | bits[1] >> (32 - 1);
                bits[1] = bits[1] << 1;
            }
            if(lfrfid_hitag_worker_validate_command(bits, 45)) return true;
        }
    } else if(22 >= bitcounter && bitcounter >= 20) {
        for(uint8_t i = 0; i < bitcounter - 19; i++) {
            if(i > 0) {
                bits[0] = bits[0] << 1; // | bits[1]>>(32-1);
                //bits[1] = bits[1]<<1;
            }
            if(lfrfid_hitag_worker_validate_command(bits, 20)) return true;
        }
    } else if(7 >= bitcounter && bitcounter >= 5) {
        for(uint8_t i = 0; i < bitcounter - 4; i++) {
            if(i > 0) {
                bits[0] = bits[0] << 1; // | bits[1]>>(32-1);
                //bits[1] = bits[1]<<1;
            }
            if(lfrfid_hitag_worker_validate_command(bits, 5)) return true;
        }
    }
    return false;
}

static uint8_t lfrfid_hitag_worker_validate_ripple_delta(uint32_t delta) {
    if(18 <= delta && delta <= 22) return 1;
    if(26 <= delta && delta <= 32) return 1;
    return 0;
}

void lfrfid_hitag_worker_initialize_tag_data(LFRFIDHitag* tag) {
    //initialize a tag datastructure
    //set all pages to 0x00000000
    memset(tag->pageData, 0, sizeof(tag->pageData));

    //set all pages to unknown
    for(uint8_t i = 0; i < HITAG_PAGES; i++) {
        tag->pageKnown[i] = 0;
    }

    //default RW settings:
    memset(tag->pageRW, READ, HITAG_PAGES / 2 * sizeof(tag->pageRW[0]));
    memset(tag->pageRW + HITAG_PAGES / 2, READ | WRITE, HITAG_PAGES / 2 * sizeof(tag->pageRW[0]));
    tag->pageRW[2] = 0;
    tag->pageRW[3] = 0;

    //default public/secret settings:
    for(uint8_t i = 0; i < HITAG_PAGES; i++) {
        tag->pagePublic[i] = 1;
    }
    for(uint8_t i = 2; i <= 31; i++) {
        tag->pagePublic[i] = 0;
    }
}

static void lfrfid_hitag_worker_combine_tag_data(LFRFIDHitag* tag) {
    memcpy(tag->tagData, tag->pageData, HITAG_PAGES * HITAG_PAGEBYTES);
    memcpy(tag->tagData + HITAG_PAGES * HITAG_PAGEBYTES, tag->pageKnown, HITAG_PAGES);
}

static void lfrfid_hitag_worker_split_tag_data(LFRFIDHitag* tag) {
    memcpy(tag->pageData, tag->tagData, HITAG_PAGES * HITAG_PAGEBYTES);
    memcpy(tag->pageKnown, tag->tagData + HITAG_PAGES * HITAG_PAGEBYTES, HITAG_PAGES);
}

void lfrfid_hitag_worker_process_config_page(LFRFIDHitag* tag) {
    //RW settings via byte 0
    if(tag->pageData[4 + 0] & 0b10000000) { //block 1 (logData) RW or 0
        memset(tag->pageRW + 1 * 4, READ | WRITE, 4 * sizeof(tag->pageRW[0]));
    } else {
        memset(tag->pageRW + 1 * 4, 0, 4 * sizeof(tag->pageRW[0]));
    }
    if(tag->pageData[4 + 0] & 0b01000000) { //key A&B (page 2&3) W or 0
        memset(tag->pageRW + 2, WRITE, 2 * sizeof(tag->pageRW[0]));
    } else {
        memset(tag->pageRW + 2, 0, 2 * sizeof(tag->pageRW[0]));
    }
    if(tag->pageData[4 + 0] & 0b00100000) { //block 2 RW or RO
        memset(tag->pageRW + 2 * 4, READ | WRITE, 4 * sizeof(tag->pageRW[0]));
    } else {
        memset(tag->pageRW + 2 * 4, READ, 4 * sizeof(tag->pageRW[0]));
    }
    if(tag->pageData[4 + 0] & 0b00010000) { //block 3 RW or RO
        memset(tag->pageRW + 3 * 4, READ | WRITE, 4 * sizeof(tag->pageRW[0]));
    } else {
        memset(tag->pageRW + 3 * 4, READ, 4 * sizeof(tag->pageRW[0]));
    }
    if(tag->pageData[4 + 0] & 0b00001000) { //block 4 RW or RO
        memset(tag->pageRW + 4 * 4, READ | WRITE, 4 * sizeof(tag->pageRW[0]));
    } else {
        memset(tag->pageRW + 4 * 4, READ, 4 * sizeof(tag->pageRW[0]));
    }
    if(tag->pageData[4 + 0] & 0b00000100) { //block 5 RW or RO
        memset(tag->pageRW + 5 * 4, READ | WRITE, 4 * sizeof(tag->pageRW[0]));
    } else {
        memset(tag->pageRW + 5 * 4, READ, 4 * sizeof(tag->pageRW[0]));
    }
    if(tag->pageData[4 + 0] & 0b00000010) { //block 6 RW or RO
        memset(tag->pageRW + 6 * 4, READ | WRITE, 4 * sizeof(tag->pageRW[0]));
    } else {
        memset(tag->pageRW + 6 * 4, READ, 4 * sizeof(tag->pageRW[0]));
    }
    if(tag->pageData[4 + 0] & 0b00000001) { //block 7 RW or RO
        memset(tag->pageRW + 7 * 4, READ | WRITE, 4 * sizeof(tag->pageRW[0]));
    } else {
        memset(tag->pageRW + 7 * 4, READ, 4 * sizeof(tag->pageRW[0]));
    }
    //RW settings via byte 1
    if(tag->pageData[4 + 1] & 0b00010000) { //config page (1) RW or RO
        tag->pageRW[1] = READ / WRITE;
    } else {
        tag->pageRW[1] = READ;
    }

    //public/secret settings via byte 1
    if(tag->pageData[4 + 1] & 0b00000001) { //block 4-7 public or secret
        for(uint8_t i = 4 * 4; i < 8 * 4; i++) {
            tag->pagePublic[i] = 1;
        }
    } else {
        for(uint8_t i = 4 * 4; i < 8 * 4; i++) {
            tag->pagePublic[i] = 0;
        }
    }
}

//------------------------------------------------------------------ worker threads  ------------------------------------------------------------------

static int32_t lfrfid_hitag_worker_emulate_thread(void* thread_context) {
    LFRFIDHitagWorker* worker = (LFRFIDHitagWorker*)thread_context;

    //load tag
    worker->tag = malloc(sizeof(LFRFIDHitag));
    lfrfid_hitag_worker_initialize_tag_data(worker->tag);

    size_t data_size = protocol_dict_get_data_size(worker->dict, LFRFIDProtocolHitag1);
    protocol_dict_get_data(worker->dict, LFRFIDProtocolHitag1, worker->tag->tagData, data_size);

    lfrfid_hitag_worker_split_tag_data(worker->tag);
    //TODO: check if a tag has been properly loaded with known serial nr & config page
    lfrfid_hitag_worker_process_config_page(worker->tag);

    //init capData for carrier_in
    LfRfidHitagCaptureData* capData = malloc(sizeof(LfRfidHitagCaptureData));
    capData->stream = buffer_stream_alloc(EMULATE_BUFFER_SIZE, EMULATE_BUFFER_COUNT);
    capData->pair = varint_pair_alloc();
    capData->capCounter = 0;
    
    //init dmaData for emulate mode:
    uint16_t dmaLen = 0;
    TimerDMAData* dataDMA = malloc(sizeof(TimerDMAData));

    LfRfidHitagReplyData* dataReply = malloc(sizeof(LfRfidHitagReplyData));

    // setup pins for emulating
    // ibutton low
    furi_hal_ibutton_pin_configure();
    furi_hal_ibutton_pin_write(false);
    // comparator in
    furi_hal_gpio_init(&gpio_rfid_data_in, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    // carrier_out to fixed low state
    furi_hal_gpio_init(&gpio_rfid_carrier_out, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_rfid_carrier_out, false);
    // PA15 carrier_in to alt ftn 1 (TIM2 CH1) to use it for input capture
    furi_hal_gpio_init_ex(
        &gpio_rfid_carrier, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn1TIM2);
    // pull to alternate function 1 (TIM2 CH3) to drive low (for command detection) or pull antenna via DMA (for command replies)
    furi_hal_gpio_init_ex(
        &gpio_nfc_irq_rfid_pull,
        GpioModeAltFunctionPushPull,
        GpioPullNo,
        GpioSpeedLow,
        GpioAltFn1TIM2);

    //start capture via carrier in instead of rfid in
    furi_hal_rfid_rtf_carrier_in_start(
        capData,
        worker->carrierPrescaler,
        dataDMA->timer_buffer_arr,
        dataDMA->timer_buffer_ccr,
        dmaLen,
        lfrfid_hitag_worker_carrier_in_callback);

    uint8_t prescaler = worker->carrierPrescaler;
    uint8_t period = 8 * prescaler - 1;

    bool carrier_on = false;
    int8_t carrier_count = 0;
    uint8_t carrier_threshold_on = 5;
    int8_t carrier_threshold_off = -5;

    uint32_t periodcounter = 0;

    uint8_t localMax = 0;
    uint8_t localMin = 255;
    uint32_t localMaxIndex = 0;
    uint32_t localMinIndex = 0;
    uint32_t lastMaxIndex = 0;
    uint32_t lastMinIndex = 0;
    uint8_t maxBitCounter = 0;
    uint8_t minBitCounter = 0;
    uint32_t maxBits[5];
    uint32_t minBits[5];
    int8_t cmd = 0;
    uint32_t one = 1 << (32 - 1);
    uint32_t cmdIndex = 0;

    uint32_t miniMaxDelta = 0;
    uint32_t miniMaxIndex = 0;
    uint32_t prevMaxDelta = 0;
    uint32_t maxDelta = 0;

    uint32_t miniMinDelta = 0;
    uint32_t miniMinIndex = 0;
    uint32_t prevMinDelta = 0;
    uint32_t minDelta = 0;

    uint32_t prevduration = 0;
    uint32_t duration = 0;
    uint8_t durationcounter = 0;
    uint8_t durationthreshold = 5;

    uint8_t tagState = HitagStateIdle;
    uint8_t tagMode = HitagModeBasic;

    while(1) {
        Buffer* buffer = buffer_stream_receive(capData->stream, 1);

        if(buffer != NULL) {
            size_t buffsize = buffer_get_size(buffer);
            uint8_t* buffdata = buffer_get_data(buffer);

            //parse carrier_in duration & scan for cmds
            size_t index = 0;
            while(index < buffsize) {
                prevduration = duration;
                size_t tmp_size =
                    varint_uint32_unpack(&duration, &buffdata[index], buffsize - index);

                if(tmp_size == 0) {
                    FURI_LOG_E(TAG, "can't unpack varint pair");
                    break;
                } else {
                    index += tmp_size;

                    periodcounter++;

                    if(!carrier_on) { //detect if carrier is available
                        if(0.75 * period <= duration && duration <= 1.25 * period) {
                            carrier_count++;
                            if(carrier_count >= carrier_threshold_on) {
                                carrier_on = true;
                                localMax = 0;
                                localMin = 255;
                                minBitCounter = 0;
                                maxBitCounter = 0;
                                minBits[0] = 0;
                                minBits[1] = 0;
                                maxBits[0] = 0;
                                maxBits[1] = 0;

                                carrier_count = 0;
                                //TODO reset tagstate to Idle & tagMode to basic after power_on_reset (no carrier for 10ms)
                            }
                        } else {
                            carrier_count = 0;
                        }
                    } else {
                        //detect if carrier is still available
                        if(duration <= 0.75 * period || 1.25 * period <= duration) {
                            carrier_count--;
                            if(carrier_count <= carrier_threshold_off) {
                                carrier_on = false;
                                carrier_count = 0;
                            }
                        } else {
                            carrier_count = 0;
                        }

                        if(carrier_on) { //if carrier is still available
                            //finetune the average period for reference
                            if(prevduration == duration) {
                                durationcounter++;
                                if(durationcounter == durationthreshold && period != duration) {
                                    period = duration;
                                }
                            } else {
                                durationcounter = 0;
                            }

                            //detect local MAX, and update maxCmd detection
                            if(duration > prevduration && duration > period) {
                                //rising edge above the normal periodduration: update localMax
                                localMax = duration;
                                localMaxIndex = periodcounter;
                            } else if(
                                duration < prevduration && localMax > period &&
                                periodcounter <= localMaxIndex + 2) {
                                //we passed a local maximum, derive length between current and prev max index
                                if(miniMaxDelta == 0) {
                                    prevMaxDelta = maxDelta;
                                }
                                maxDelta = (localMaxIndex - lastMaxIndex) * prescaler;

                                //check if there's a miniDelta and if it should be added to previous or next delta
                                if(miniMaxDelta > 0) {
                                    //by default minidelta is added to next
                                    //in case doing this does not result in 2 consequtive valid deltas, but adding it to previous does result in 2 consequtive valid deltas, then assign it to previous delta
                                    if((lfrfid_hitag_worker_validate_ripple_delta(
                                            prevMaxDelta + miniMaxDelta) +
                                        lfrfid_hitag_worker_validate_ripple_delta(
                                            maxDelta - miniMaxDelta)) >
                                       (lfrfid_hitag_worker_validate_ripple_delta(prevMaxDelta) +
                                        lfrfid_hitag_worker_validate_ripple_delta(maxDelta))) {
                                        //update the previous (last) bit from 0 to 1 if a command was ongoing
                                        if(maxBitCounter > 0) {
                                            maxBits[(maxBitCounter - 1) / 32] |=
                                                (one >> ((maxBitCounter - 1) % 32));
                                        }
                                        maxDelta -= miniMaxDelta;
                                        lastMaxIndex = miniMaxIndex;
                                    }
                                    miniMaxDelta = 0;
                                }

                                //now process the (possibly updated) delta
                                if(maxDelta >= 18) {
                                    if(maxDelta <= 22) {
                                        maxBitCounter++;
                                    } else if(26 <= maxDelta && maxDelta <= 32) {
                                        maxBits[(maxBitCounter) / 32] |=
                                            (one >> ((maxBitCounter) % 32));
                                        maxBitCounter++;
                                    } else if(maxBitCounter > 0) {
                                        //end of bitstring caused by invalid delta

                                        if(lfrfid_hitag_worker_validate_bits(
                                               maxBits, maxBitCounter)) {
                                            cmd = 1;
                                            cmdIndex = localMaxIndex;

                                            //CMD found, stop searching for minCMD: reset the minCmd detection
                                            minBits[0] = 0;
                                            minBits[1] = 0;
                                            minBitCounter = 0;
                                        } else {
                                            //reset the maxCmd detection
                                            maxBits[0] = 0;
                                            maxBits[1] = 0;
                                            maxBitCounter = 0;
                                        }
                                    }
                                    lastMaxIndex = localMaxIndex;
                                } else {
                                    miniMaxDelta = maxDelta;
                                    miniMaxIndex = localMaxIndex;
                                }
                                localMax = 0;
                            }

                            //terminate bitstring ico absence of next delta (a true end)
                            if(periodcounter == lastMaxIndex + 40 / prescaler &&
                               maxBitCounter > 0) {
                                if(lfrfid_hitag_worker_validate_bits(maxBits, maxBitCounter)) {
                                    cmd = 1;
                                    cmdIndex = localMaxIndex;

                                    //CMD found, stop searching for minCMD: reset the minCmd detection
                                    minBits[0] = 0;
                                    minBits[1] = 0;
                                    minBitCounter = 0;
                                } else {
                                    //reset the maxCmd detection
                                    maxBits[0] = 0;
                                    maxBits[1] = 0;
                                    maxBitCounter = 0;
                                }
                            }

                            //detect local MIN, and update minCmd detection
                            if(duration < prevduration && duration < period) {
                                //rising edge above the normal periodduration: update localMax
                                localMin = duration;
                                localMinIndex = periodcounter;
                            } else if(
                                duration > prevduration && localMin < period &&
                                periodcounter <= localMinIndex + 2) {
                                //we passed a local minimum, derive length between current and prev max index
                                if(miniMinDelta == 0) {
                                    prevMinDelta = minDelta;
                                }
                                minDelta = (localMinIndex - lastMinIndex) * prescaler;

                                //check if there's a miniDelta and if it should be added to previous or next delta
                                if(miniMinDelta > 0) {
                                    //by default minidelta is added to next
                                    //in case doing this does not result in 2 consequtive valid deltas, but adding it to previous does result in 2 consequtive valid deltas, then assign it to previous delta
                                    if((lfrfid_hitag_worker_validate_ripple_delta(
                                            prevMinDelta + miniMinDelta) +
                                        lfrfid_hitag_worker_validate_ripple_delta(
                                            minDelta - miniMinDelta)) >
                                       (lfrfid_hitag_worker_validate_ripple_delta(prevMinDelta) +
                                        lfrfid_hitag_worker_validate_ripple_delta(minDelta))) {
                                        //update the previous (last) bit from 0 to 1 if a command was ongoing
                                        if(minBitCounter > 0) {
                                            minBits[(minBitCounter - 1) / 32] |=
                                                (one >> ((minBitCounter - 1) % 32));
                                        }
                                        minDelta -= miniMinDelta;
                                        lastMinIndex = miniMinIndex;
                                    }
                                    miniMinDelta = 0;
                                }

                                //now process the (possibly updated) delta
                                if(minDelta >= 18) {
                                    if(minDelta <= 22) {
                                        minBitCounter++;
                                    } else if(26 <= minDelta && minDelta <= 32) {
                                        minBits[(minBitCounter) / 32] |=
                                            (one >> ((minBitCounter) % 32));
                                        minBitCounter++;
                                    } else if(minBitCounter > 0) {
                                        //end of bitstring caused by invalid delta
                                        if(lfrfid_hitag_worker_validate_bits(
                                               minBits, minBitCounter)) {
                                            cmd = -1;
                                            cmdIndex = localMinIndex;

                                            //CMD found, stop searching for maxCMD: reset the maxCmd detection
                                            maxBits[0] = 0;
                                            maxBits[1] = 0;
                                            maxBitCounter = 0;
                                        } else {
                                            //reset the minCmd detection
                                            minBits[0] = 0;
                                            minBits[1] = 0;
                                            minBitCounter = 0;
                                        }
                                    }
                                    lastMinIndex = localMinIndex;
                                } else {
                                    miniMinDelta = minDelta;
                                    miniMinIndex = localMinIndex;
                                }
                                localMin = 255;
                            }

                            //terminate of bitstring caused by absence of next delta (a true end)
                            if(periodcounter == lastMinIndex + 40 / prescaler &&
                               minBitCounter > 0) {
                                if(lfrfid_hitag_worker_validate_bits(minBits, minBitCounter)) {
                                    cmd = -1;
                                    cmdIndex = localMinIndex;

                                    //CMD found, stop searching for maxCMD: reset the maxCmd detection
                                    maxBits[0] = 0;
                                    maxBits[1] = 0;
                                    maxBitCounter = 0;
                                } else {
                                    //reset the minCmd detection
                                    minBits[0] = 0;
                                    minBits[1] = 0;
                                    minBitCounter = 0;
                                }
                            }

                            //if a valid cmd has been detected: process it & reply
                            if(cmd != 0) {
                                uint32_t bits[5];
                                uint8_t size = 0;
                                //select the cmd bit source (max or min detection)
                                if(cmd == 1) {
                                    bits[0] = maxBits[0];
                                    bits[1] = maxBits[1];
                                    size = maxBitCounter;

                                    maxBits[0] = 0;
                                    maxBits[1] = 0;
                                    maxBitCounter = 0;
                                } else {
                                    bits[0] = minBits[0];
                                    bits[1] = minBits[1];
                                    size = minBitCounter;

                                    minBits[0] = 0;
                                    minBits[1] = 0;
                                    minBitCounter = 0;
                                }
                                cmd = 0;

                                dmaLen = 0;
                                if(size == 5 && (bits[0] >> (32 - 5)) == 0b11001) { //SET_CCNEW
                                    if(tagState != HitagStateQuiet) {
                                        //reset selected state
                                        tagState = HitagStateIdle;

                                        //set tag in advanced mode
                                        tagMode = HitagModeAdvanced;

                                        //prepare emulate DMA buffer with advanced SN reply (page 0)
                                        dataReply->replyvalue[0] = 0b00000111;
                                        dataReply->replytype[0] = HITAG_STARTBIT3;
                                        dataReply->replyvalue[1] = worker->tag->pageData[0];
                                        dataReply->replytype[1] = HITAG_BYTE;
                                        dataReply->replyvalue[2] = worker->tag->pageData[1];
                                        dataReply->replytype[2] = HITAG_BYTE;
                                        dataReply->replyvalue[3] = worker->tag->pageData[2];
                                        dataReply->replytype[3] = HITAG_BYTE;
                                        dataReply->replyvalue[4] = worker->tag->pageData[3];
                                        dataReply->replytype[4] = HITAG_BYTE;

                                        dataReply->replylength = 5;

                                        dmaLen = lfrfid_hitag_worker_yield_dma_buffer_AC(
                                            dataReply, dataDMA, worker->carrierPrescaler);
                                    }
                                } else if(size == 5 && (bits[0] >> (32 - 5)) == 0b00110) { //SET_CC
                                    if(tagState != HitagStateQuiet) {
                                        //reset selected state
                                        tagState = HitagStateIdle;

                                        //do not (re)set tag in basic mode, this only happens during power on reset

                                        //prepare emulate DMA buffer with basic SN reply (page 0)
                                        dataReply->replyvalue[0] = 0b00000001;
                                        dataReply->replytype[0] = HITAG_STARTBIT;
                                        dataReply->replyvalue[1] = worker->tag->pageData[0];
                                        dataReply->replytype[1] = HITAG_BYTE;
                                        dataReply->replyvalue[2] = worker->tag->pageData[1];
                                        dataReply->replytype[2] = HITAG_BYTE;
                                        dataReply->replyvalue[3] = worker->tag->pageData[2];
                                        dataReply->replytype[3] = HITAG_BYTE;
                                        dataReply->replyvalue[4] = worker->tag->pageData[3];
                                        dataReply->replytype[4] = HITAG_BYTE;

                                        dataReply->replylength = 5;

                                        dmaLen = lfrfid_hitag_worker_yield_dma_buffer_AC(
                                            dataReply, dataDMA, worker->carrierPrescaler);
                                    }
                                } else if(size == 45 && (bits[0] >> (32 - 5)) == 0b00000) { //SELECT
                                    //check if SN is matching
                                    uint32_t SN_cmd = bits[0] << 5 | bits[1] >> (32 - 5);
                                    uint32_t SN_tag = worker->tag->pageData[0] << 24 |
                                                      worker->tag->pageData[1] << 16 |
                                                      worker->tag->pageData[2] << 8 |
                                                      worker->tag->pageData[3];
                                    if(SN_cmd == SN_tag) {
                                        //reply with config (page 1 from tag memory)
                                        tagState = HitagStateSelected;

                                        //prepare emulate DMA buffer with basic/advanced config page reply (page 1)
                                        if(tagMode == HitagModeBasic) {
                                            dataReply->replyvalue[0] = 0b00000001;
                                            dataReply->replytype[0] = HITAG_STARTBIT;

                                            dataReply->replyvalue[1] =
                                                worker->tag->pageData[4 + 0];
                                            dataReply->replytype[1] = HITAG_BYTE;
                                            dataReply->replyvalue[2] =
                                                worker->tag->pageData[4 + 1];
                                            dataReply->replytype[2] = HITAG_BYTE;
                                            dataReply->replyvalue[3] =
                                                worker->tag->pageData[4 + 2];
                                            dataReply->replytype[3] = HITAG_BYTE;
                                            dataReply->replyvalue[4] =
                                                worker->tag->pageData[4 + 3];
                                            dataReply->replytype[4] = HITAG_BYTE;

                                            dataReply->replylength = 5;
                                        } else if(tagMode == HitagModeAdvanced) {
                                            dataReply->replyvalue[0] = 0b00111111;
                                            dataReply->replytype[0] = HITAG_STARTBIT6;

                                            dataReply->replyvalue[1] =
                                                worker->tag->pageData[4 + 0];
                                            dataReply->replytype[1] = HITAG_BYTE;
                                            dataReply->replyvalue[2] =
                                                worker->tag->pageData[4 + 1];
                                            dataReply->replytype[2] = HITAG_BYTE;
                                            dataReply->replyvalue[3] =
                                                worker->tag->pageData[4 + 2];
                                            dataReply->replytype[3] = HITAG_BYTE;
                                            dataReply->replyvalue[4] =
                                                worker->tag->pageData[4 + 3];
                                            dataReply->replytype[4] = HITAG_BYTE;

                                            uint8_t crc = CRC_PRESET;
                                            lfrfid_hitag_worker_calc_crc(
                                                &crc, dataReply->replyvalue[1], 8);
                                            lfrfid_hitag_worker_calc_crc(
                                                &crc, dataReply->replyvalue[2], 8);
                                            lfrfid_hitag_worker_calc_crc(
                                                &crc, dataReply->replyvalue[3], 8);
                                            lfrfid_hitag_worker_calc_crc(
                                                &crc, dataReply->replyvalue[4], 8);

                                            dataReply->replyvalue[5] = crc;
                                            dataReply->replytype[5] = HITAG_CRC;

                                            dataReply->replylength = 6;
                                        }

                                        dmaLen = lfrfid_hitag_worker_yield_dma_buffer_MC(
                                            dataReply, dataDMA, worker->carrierPrescaler);
                                    }
                                } else if(size == 20 && (bits[0] >> (31))) { //R/W
                                    if(tagState == HitagStateSelected) {
                                        //reply with requested pages in case of read public (ignore secret block/pages & ignore write cmd)
                                        uint8_t rwCmd = bits[0] >> (32 - 4);
                                        uint8_t addr = bits[0] >> 20 & 0xff;
                                        uint8_t pages = 0;
                                        switch(rwCmd) {
                                        case(0b1100):
                                            //read public page
                                            if(worker->tag->pageKnown[addr] &&
                                               worker->tag->pagePublic[addr]) {
                                                pages = 1;
                                            }
                                            break;
                                        case(0b1101):
                                            //read public block
                                            pages = (64 - addr) % 4;
                                            if(pages == 0) {
                                                pages = 4;
                                            }
                                            for(uint8_t p = addr; p < addr + pages; p++) {
                                                if(!worker->tag->pageKnown[p] ||
                                                   !worker->tag->pagePublic[p]) {
                                                    pages = 0;
                                                    break;
                                                }
                                            }
                                            break;
                                        }
                                        if(pages > 0) {
                                            //pages
                                            for(uint8_t i = 0; i < pages; i++) {
                                                dataReply->replyvalue[1 + i * 4 + 0] =
                                                    worker->tag->pageData[(addr + i) * 4 + 0];
                                                dataReply->replytype[1 + i * 4 + 0] = HITAG_BYTE;
                                                dataReply->replyvalue[1 + i * 4 + 1] =
                                                    worker->tag->pageData[(addr + i) * 4 + 1];
                                                dataReply->replytype[1 + i * 4 + 1] = HITAG_BYTE;
                                                dataReply->replyvalue[1 + i * 4 + 2] =
                                                    worker->tag->pageData[(addr + i) * 4 + 2];
                                                dataReply->replytype[1 + i * 4 + 2] = HITAG_BYTE;
                                                dataReply->replyvalue[1 + i * 4 + 3] =
                                                    worker->tag->pageData[(addr + i) * 4 + 3];
                                                dataReply->replytype[1 + i * 4 + 3] = HITAG_BYTE;
                                            }
                                            //startsequence & CRC
                                            if(tagMode == HitagModeBasic) {
                                                dataReply->replyvalue[0] = 0b00000001;
                                                dataReply->replytype[0] = HITAG_STARTBIT;

                                                dataReply->replylength = 1 + pages * 4;
                                            } else if(tagMode == HitagModeAdvanced) {
                                                dataReply->replyvalue[0] = 0b00111111;
                                                dataReply->replytype[0] = HITAG_STARTBIT6;

                                                uint8_t crc = CRC_PRESET;

                                                for(uint8_t i = 0; i < pages; i++) {
                                                    lfrfid_hitag_worker_calc_crc(
                                                        &crc,
                                                        dataReply->replyvalue[1 + i * 4 + 0],
                                                        8);
                                                    lfrfid_hitag_worker_calc_crc(
                                                        &crc,
                                                        dataReply->replyvalue[1 + i * 4 + 1],
                                                        8);
                                                    lfrfid_hitag_worker_calc_crc(
                                                        &crc,
                                                        dataReply->replyvalue[1 + i * 4 + 2],
                                                        8);
                                                    lfrfid_hitag_worker_calc_crc(
                                                        &crc,
                                                        dataReply->replyvalue[1 + i * 4 + 3],
                                                        8);
                                                }

                                                dataReply->replyvalue[1 + pages * 4] = crc;
                                                dataReply->replytype[1 + pages * 4] = HITAG_CRC;

                                                dataReply->replylength = 2 + pages * 4;
                                            }

                                            dmaLen = lfrfid_hitag_worker_yield_dma_buffer_MC(
                                                dataReply, dataDMA, worker->carrierPrescaler);
                                        }
                                    }
                                } else if(size == 20 && (bits[0] >> (32 - 4)) == 0b0111) { //HALT
                                    //TODO validate if dummy address is a valid one (one of the default public pages (32-63))

                                    //acknowledge & go silent untill next reset
                                    if(tagState == HitagStateSelected) {
                                        tagState = HitagStateQuiet;

                                        if(tagMode == HitagModeBasic) {
                                            dataReply->replyvalue[0] = 0b00000101;
                                            dataReply->replytype[0] = HITAG_ACK;
                                        } else if(tagMode == HitagModeAdvanced) {
                                            dataReply->replyvalue[0] = 0b11111101;
                                            dataReply->replytype[0] = HITAG_ACK_ADV;
                                        }

                                        dataReply->replylength = 1;

                                        dmaLen = lfrfid_hitag_worker_yield_dma_buffer_MC(
                                            dataReply, dataDMA, worker->carrierPrescaler);
                                    }
                                }

                                if(dmaLen > 0) { //switch to reply mode
                                    //switch carrier mode to ETR required for replying
                                    furi_hal_rfid_rtf_carrier_in_ETR_mode(prescaler);

                                    //respect hitag WAIT1 time
                                    uint32_t wait1_periods = 180 / prescaler;
                                    while(capData->capCounter - cmdIndex < wait1_periods) {
                                        furi_delay_us(40);
                                    }

                                    //just start the DMA, stopping is handled in DMA transfer complete interrupt
                                    furi_hal_rfid_rtf_pull_out_dma_start((size_t)dmaLen);
                                }
                            }
                        }
                    }
                }
            }

            //reset buffer
            buffer_reset(buffer);
        }

        if(buffer_stream_get_overrun_count(capData->stream) > 0) {
            FURI_LOG_E(TAG, "Read overrun, recovering");
            buffer_stream_reset(capData->stream);
        }

        uint32_t flags = furi_event_flag_get(worker->events);
        if(FURI_BIT(flags, LFRFIDHitagWorkerSignalStop)) {
            break;
        }
    }

    //stop carrier capture in
    furi_hal_rfid_rtf_carrier_in_stop();

    free(worker->tag);
    free(dataDMA);
    free(dataReply);
    varint_pair_free(capData->pair);
    buffer_stream_free(capData->stream);
    free(capData);

    //clear the stop flag on workder so that i can use the worker to launch a new read routine without having to restart the app
    furi_event_flag_clear(worker->events, 1 << LFRFIDHitagWorkerSignalStop);

    return 0;
}

static int32_t lfrfid_hitag_worker_read_thread(void* thread_context) {
    LFRFIDHitagWorker* worker = (LFRFIDHitagWorker*)thread_context;

    //initialize a tag datastructure
    worker->tag = malloc(sizeof(LFRFIDHitag));
    lfrfid_hitag_worker_initialize_tag_data(worker->tag);

    //prep variables for decoder
    LfRfidHitagProtocol currentProtocol = HitagProtocolAntiCollision;
    uint8_t startBits = 1;
    uint32_t memBlock[HITAG_BLOCKPAGES] = {0};
    uint8_t bits = 0;
    bool halfBit = false;
    bool finished = false;

    //prep variables for worker
    uint8_t expectedBits = 33;
    uint8_t readings = 0;
    uint8_t currPage = 0;
    bool readSucces = false;
    uint8_t readThreshold = 3;

    // setup pins for reading
    // ibutton low
    furi_hal_ibutton_pin_configure();
    furi_hal_ibutton_pin_write(false);
    // dont pull rfid antenna
    furi_hal_gpio_init(&gpio_nfc_irq_rfid_pull, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_nfc_irq_rfid_pull, false);
    // carrier pin to timer out
    furi_hal_gpio_init_ex(
        &gpio_rfid_carrier_out,
        GpioModeAltFunctionPushPull,
        GpioPullNo,
        GpioSpeedLow,
        GpioAltFn1TIM1);
    // comparator in
    furi_hal_gpio_init(&gpio_rfid_data_in, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    //and setup TIM1 CH1N with DMA
    //fill the DMA arr & ccr buffers
    /*start with looping SETCC command*/
    LfRfidHitagCMDData* dataCMD = malloc(sizeof(LfRfidHitagCMDData));
    dataCMD->CMDlength = 0;
    dataCMD->CMDposition = 0;
    dataCMD->CMDsubposition = 0;
    dataCMD->CMDactive = false;

    LfRfidHitagCMDData* backupCMD = malloc(sizeof(LfRfidHitagCMDData));
    uint32_t value[] = {
        128,
        0b00110,
        1,
        213,
        (uint32_t)(1.05 * 33 * HITAG_BITPERIODS_AC)}; //always take 5% reading time buffer
    uint8_t type[] = {HITAG_ON, HITAG_SET, HITAG_STOP, HITAG_ON, HITAG_ON};
    for(size_t i = 0; i < 5; i++) {
        backupCMD->CMDvalue[i] = value[i];
        backupCMD->CMDtype[i] = type[i];
    }
    backupCMD->CMDlength = 5;
    backupCMD->CMDposition = 0;
    backupCMD->CMDsubposition = 0;
    backupCMD->CMDloop =
        true; //not really required since by definition the backup cmd is looping already in yield code
    backupCMD->CMDactive = true;

    TimerDMAData* dataDMA = malloc(sizeof(TimerDMAData));
    lfrfid_hitag_worker_yield_dma_buffer_BPLM(dataCMD, backupCMD, dataDMA, 0, DMA_BUFFER_SIZE / 2);
    lfrfid_hitag_worker_yield_dma_buffer_BPLM(
        dataCMD, backupCMD, dataDMA, DMA_BUFFER_SIZE / 2, DMA_BUFFER_SIZE / 2);

    //start TIM1 with DMA function
    furi_hal_rfid_rtf_carrier_out_start(
        dataDMA->timer_buffer_arr, dataDMA->timer_buffer_ccr, DMA_BUFFER_SIZE, lfrfid_hitag_worker_carrier_out_dma_callback, worker);

    // start capture
    LfRfidHitagCaptureData* capData = malloc(sizeof(LfRfidHitagCaptureData));
    capData->stream = buffer_stream_alloc(READ_BUFFER_SIZE, READ_BUFFER_COUNT);
    capData->pair = varint_pair_alloc();
    furi_hal_rfid_tim_read_capture_start(lfrfid_hitag_worker_capture_in_cc_callback, capData);

    while(1) {
        Buffer* buffer = buffer_stream_receive(capData->stream, 1);

        //process data if available
        if(buffer != NULL) {
            size_t buffsize = buffer_get_size(buffer);
            uint8_t* buffdata = buffer_get_data(buffer);

            //if scanning for tagdata, then also parse buffer data
            size_t index = 0;
            while(expectedBits > 0 && index < buffsize) {
                uint32_t duration;
                uint32_t pulse;
                size_t tmp_size;

                if(!varint_pair_unpack(
                       &buffdata[index], buffsize - index, &pulse, &duration, &tmp_size)) {
                    FURI_LOG_E(TAG, "can't unpack varint pair");
                    break;
                } else {
                    index += tmp_size;

                    //feed pulse duration to decoder
                    lfrfid_hitag_worker_decoder_feed(
                        pulse,
                        duration,
                        memBlock,
                        &bits,
                        &halfBit,
                        &finished,
                        currentProtocol,
                        startBits);

                    //see if i have a valid bit sequence
                    if(finished) {
                        //bitsequence ready for validation
                        if(bits ==
                           expectedBits) { //eligable bitsequence detected, accept it only after x consequtive readings
                            //reset the backupCMD count to ensure some pause between bitsequence detection & the CMD injection
                            backupCMD->CMDcount = 0;

                            if(readings == 0) {
                                for(uint8_t p = 0; p < (expectedBits - 1) / 32; p++) {
                                    worker->tag->pageData[(currPage + p) * 4 + 0] =
                                        memBlock[p] >> 24 & 0xff;
                                    worker->tag->pageData[(currPage + p) * 4 + 1] =
                                        memBlock[p] >> 16 & 0xff;
                                    worker->tag->pageData[(currPage + p) * 4 + 2] =
                                        memBlock[p] >> 8 & 0xff;
                                    worker->tag->pageData[(currPage + p) * 4 + 3] = memBlock[p] &
                                                                                    0xff;
                                    worker->tag->pageKnown[currPage + p] = 0;
                                }
                                readings = 1;
                            } else if(readings < readThreshold) {
                                bool match = true;
                                for(uint8_t p = 0; p < (expectedBits - 1) / 32; p++) {
                                    if(worker->tag->pageData[(currPage + p) * 4 + 0] !=
                                           (memBlock[p] >> 24 & 0xff) ||
                                       worker->tag->pageData[(currPage + p) * 4 + 1] !=
                                           (memBlock[p] >> 16 & 0xff) ||
                                       worker->tag->pageData[(currPage + p) * 4 + 2] !=
                                           (memBlock[p] >> 8 & 0xff) ||
                                       worker->tag->pageData[(currPage + p) * 4 + 3] !=
                                           (memBlock[p] & 0xff)) {
                                        match = false;
                                        break;
                                    }
                                }
                                if(match) {
                                    readings++;
                                    if(readings == readThreshold) {
                                        //succesful page(s) reading
                                        readSucces = true;

                                        for(uint8_t p = 0; p < (expectedBits - 1) / 32; p++) {
                                            worker->tag->pageKnown[currPage + p] = 1;
                                        }

                                        readings = 0;
                                        expectedBits = 0; //to prevent further updates to tag data
                                    }
                                } else {
                                    for(uint8_t p = 0; p < (expectedBits - 1) / 32; p++) {
                                        worker->tag->pageData[(currPage + p) * 4 + 0] =
                                            memBlock[p] >> 24 & 0xff;
                                        worker->tag->pageData[(currPage + p) * 4 + 1] =
                                            memBlock[p] >> 16 & 0xff;
                                        worker->tag->pageData[(currPage + p) * 4 + 2] =
                                            memBlock[p] >> 8 & 0xff;
                                        worker->tag->pageData[(currPage + p) * 4 + 3] =
                                            memBlock[p] & 0xff;
                                        worker->tag->pageKnown[currPage + p] = 0;
                                    }
                                    readings = 1;
                                }
                            }
                        }
                        //reset tempdata for next bitsequence readout
                        memset(memBlock, 0, HITAG_BLOCKPAGES * sizeof(memBlock[0]));
                        bits = 0;
                        finished = false;
                    }
                }
            }

            //reset buffer
            buffer_reset(buffer);
        }

        //if bitsequence found & after pause, prep & launch the command for reading out next bit of data
        if(readSucces && backupCMD->CMDcount >= 7) {
            readSucces = false;
            backupCMD->CMDcount =
                0; //required since otherwise the 'relaunch' of cmd is triggered straightaway

            //update protocolData
            size_t data_size = protocol_dict_get_data_size(worker->dict, LFRFIDProtocolHitag1);
            lfrfid_hitag_worker_combine_tag_data(worker->tag);
            protocol_dict_set_data(
                worker->dict, LFRFIDProtocolHitag1, worker->tag->tagData, data_size);

            if(currPage == 0) { //read config page next via SELECT cmd
                FURI_LOG_D(
                    TAG,
                    "Hitag1, [%02X %02X %02X %02X] found",
                    worker->tag->pageData[0],
                    worker->tag->pageData[1],
                    worker->tag->pageData[2],
                    worker->tag->pageData[3]);
                dolphin_deed(DolphinDeedRfidReadSuccess);
                worker->workerstatus = LFRFIDHitagStatusDetected;

                currentProtocol = HitagProtocolManchesterCoding;
                expectedBits = 33;
                currPage = 1;
            } else if(currPage == 1) { //process config page and start reading public blocks
                lfrfid_hitag_worker_process_config_page(worker->tag);
                FURI_LOG_D(
                    TAG,
                    "Hitag1, [%02X %02X %02X %02X] config page read",
                    worker->tag->pageData[0],
                    worker->tag->pageData[1],
                    worker->tag->pageData[2],
                    worker->tag->pageData[3]);
                //find next public readible block
                for(currPage = 4; currPage <= HITAG_PAGES - 4; currPage += 4) {
                    if(worker->tag->pagePublic[currPage] &&
                       (worker->tag->pageRW[currPage] & READ)) { //public readible page/block found
                        expectedBits = 129;
                        break;
                    } else {
                        FURI_LOG_D(
                            TAG,
                            "Hitag1, [%02X %02X %02X %02X] block %u not public/readible",
                            worker->tag->pageData[0],
                            worker->tag->pageData[1],
                            worker->tag->pageData[2],
                            worker->tag->pageData[3],
                            currPage / 4);
                    }
                }
            } else if(currPage < HITAG_PAGES - 4) { //scan for next public readible page
                FURI_LOG_D(
                    TAG,
                    "Hitag1, [%02X %02X %02X %02X] block %u read",
                    worker->tag->pageData[0],
                    worker->tag->pageData[1],
                    worker->tag->pageData[2],
                    worker->tag->pageData[3],
                    currPage / 4);
                for(currPage += 4; currPage <= HITAG_PAGES - 4; currPage += 4) {
                    if(worker->tag->pagePublic[currPage] &&
                       (worker->tag->pageRW[currPage] & READ)) { //public readible page/block found
                        expectedBits = 129;
                        break;
                    } else {
                        FURI_LOG_D(
                            TAG,
                            "Hitag1, [%02X %02X %02X %02X] block %u not public/readible",
                            worker->tag->pageData[0],
                            worker->tag->pageData[1],
                            worker->tag->pageData[2],
                            worker->tag->pageData[3],
                            currPage / 4);
                    }
                }
            } else if(
                currPage ==
                HITAG_PAGES -
                    4) { //all public readible pages read: signal view & break out of while loop (stops the worker)
                FURI_LOG_D(
                    TAG,
                    "Hitag1, [%02X %02X %02X %02X] block %u read",
                    worker->tag->pageData[0],
                    worker->tag->pageData[1],
                    worker->tag->pageData[2],
                    worker->tag->pageData[3],
                    currPage / 4);

                worker->workerstatus = LFRFIDHitagStatusRead;
                break;
            }

            if(expectedBits > 0) { //inject CMD
                //prep SELECT cmd bytes
                uint8_t SN0 = worker->tag->pageData[0];
                uint8_t SN1 = worker->tag->pageData[1];
                uint8_t SN2 = worker->tag->pageData[2];
                uint8_t SN3 = worker->tag->pageData[3];
                uint8_t crc = CRC_PRESET;
                lfrfid_hitag_worker_calc_crc(&crc, (0b00000 << (8 - 5)), 5);
                lfrfid_hitag_worker_calc_crc(&crc, SN0, 8);
                lfrfid_hitag_worker_calc_crc(&crc, SN1, 8);
                lfrfid_hitag_worker_calc_crc(&crc, SN2, 8);
                lfrfid_hitag_worker_calc_crc(&crc, SN3, 8);

                //prep SELECT cmd array
                uint32_t value[] = {
                    128,
                    0b00000,
                    SN0,
                    SN1,
                    SN2,
                    SN3,
                    crc,
                    1,
                    213,
                    (uint32_t)(1.05 * 33 * HITAG_BITPERIODS_MC)}; //always take 5% reading time buffer
                uint8_t type[] = {
                    HITAG_ON,
                    HITAG_SELECT,
                    HITAG_BYTE,
                    HITAG_BYTE,
                    HITAG_BYTE,
                    HITAG_BYTE,
                    HITAG_CRC,
                    HITAG_STOP,
                    HITAG_ON,
                    HITAG_ON};
                uint16_t len = 10;
                uint16_t cnt = 1;
                for(size_t i = 0; i < cnt * len; i++) {
                    dataCMD->CMDvalue[i] = value[i % len];
                    dataCMD->CMDtype[i] = type[i % len];
                }
                dataCMD->CMDlength = cnt * len;
                dataCMD->CMDposition = 0;
                dataCMD->CMDsubposition = 0;
                dataCMD->CMDloop = false;

                if(currPage < 4 * 4) { //add READPAGE cmd to array
                    crc = CRC_PRESET;
                    lfrfid_hitag_worker_calc_crc(&crc, (0b1100 << (8 - 4)), 4);
                    lfrfid_hitag_worker_calc_crc(&crc, currPage, 8);
                    uint32_t value2[] = {
                        128,
                        0b1100,
                        currPage,
                        crc,
                        1,
                        213,
                        (uint32_t)(1.05 * 33 * HITAG_BITPERIODS_MC)}; //always take 5% reading time buffer
                    uint8_t type2[] = {
                        HITAG_ON, HITAG_CMD, HITAG_BYTE, HITAG_CRC, HITAG_STOP, HITAG_ON, HITAG_ON};
                    uint16_t len2 = 7;
                    uint16_t cnt2 = 5;
                    for(size_t i = 0; i < cnt2 * len2; i++) {
                        dataCMD->CMDvalue[cnt * len + i] = value2[i % len2];
                        dataCMD->CMDtype[cnt * len + i] = type2[i % len2];
                    }
                    dataCMD->CMDlength += cnt2 * len2;
                } else { //add READBLOCK cmd to array
                    crc = CRC_PRESET;
                    lfrfid_hitag_worker_calc_crc(&crc, (0b1101 << (8 - 4)), 4);
                    lfrfid_hitag_worker_calc_crc(&crc, currPage, 8);
                    uint32_t value2[] = {
                        128,
                        0b1101,
                        currPage,
                        crc,
                        1,
                        213,
                        (uint32_t)(1.05 * 129 * HITAG_BITPERIODS_MC)}; //always take 5% reading time buffer
                    uint8_t type2[] = {
                        HITAG_ON, HITAG_CMD, HITAG_BYTE, HITAG_CRC, HITAG_STOP, HITAG_ON, HITAG_ON};
                    uint16_t len2 = 7;
                    uint16_t cnt2 = 5;
                    for(size_t i = 0; i < cnt2 * len2; i++) {
                        dataCMD->CMDvalue[cnt * len + i] = value2[i % len2];
                        dataCMD->CMDtype[cnt * len + i] = type2[i % len2];
                    }
                    dataCMD->CMDlength += cnt2 * len2;
                }
            }
        }

        //while not having x consequtive reads, relaunch the (already prepared) command
        if(!readSucces && backupCMD->CMDcount >= 7 && currPage > 0 && expectedBits > 0) {
            backupCMD->CMDcount = 0;
            dataCMD->CMDposition = 0;
            dataCMD->CMDsubposition = 0;
        }

        if(buffer_stream_get_overrun_count(capData->stream) > 0) {
            FURI_LOG_E(TAG, "Read overrun, recovering");
            buffer_stream_reset(capData->stream);
        }

        //check for stop events
        uint32_t flags = furi_event_flag_get(worker->events);
        if(FURI_BIT(flags, LFRFIDHitagWorkerSignalStop)) {
            break;
        }

        //check for DMA HT/TC signals to repopulate the DMA buffer
        if(worker->DMAeventCount > 1) {
            FURI_LOG_E(TAG, "DMA refilling can't keep up %u", worker->DMAeventCount);
        }
        if(FURI_BIT(flags, LFRFIDHitagWorkerSignalHalfTransfer)) {
            //refill first half of dma buffer
            lfrfid_hitag_worker_yield_dma_buffer_BPLM(
                dataCMD, backupCMD, dataDMA, 0, DMA_BUFFER_SIZE / 2);
        }
        if(FURI_BIT(flags, LFRFIDHitagWorkerSignalTransferComplete)) {
            //refill second half of dma buffer
            lfrfid_hitag_worker_yield_dma_buffer_BPLM(
                dataCMD, backupCMD, dataDMA, DMA_BUFFER_SIZE / 2, DMA_BUFFER_SIZE / 2);
        }
        worker->DMAeventCount = 0;
        //clear all flags (i have processed all known/expected events)
        furi_event_flag_clear(worker->events, flags);
    }

    //stop timers
    furi_hal_rfid_tim_read_capture_stop();
    furi_hal_rfid_rtf_carrier_out_stop();

    //free memory
    free(dataCMD);
    free(backupCMD);
    free(dataDMA);
    varint_pair_free(capData->pair);
    buffer_stream_free(capData->stream);
    free(capData);

    //allow the view that started the thread to signal the stop, which will properly merge the threads & de-init the worker
    if(currPage == HITAG_PAGES - 4) {
        const uint32_t available_flags = (1 << LFRFIDHitagWorkerSignalStop);
        while(true) {
            uint32_t flags = furi_event_flag_wait(
                worker->events, available_flags, FuriFlagWaitAny, FuriWaitForever);

            if(FURI_BIT(flags, LFRFIDHitagWorkerSignalStop)) {
                break;
            }
        }
    }

    free(worker->tag);

    //clear the stop flag on workder so that i can use the same worker to launch a new routine
    furi_event_flag_clear(worker->events, 1 << LFRFIDHitagWorkerSignalStop);

    return 0;
}
