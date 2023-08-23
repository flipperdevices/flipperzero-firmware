#include "show_device.h"
#include "Conf_MCP251XFD.h"
#include "can0_function.h"
#include "can_interface_sync.h"
#include <furi.h>

#define TAG "Show_Device"
//**********************************************************************************************************************************************************
const char CANStringsNames[2][4 + 1 /* \0 */] = {
    "CAN0",
    "CAN1",
    //.....
};

#define GetCanFromComponent (1u - ((uint8_t)(pComp == &can0) & 0x1))
#define CANx CANStringsNames[GetCanFromComponent]

//-----------------------------------------------------------------------------
const uint8_t CSIdx[2] = {2, 2};

//=============================================================================
// Show which device is detected on the console
//=============================================================================
void show_device_detected(MCP251XFD* pComp, uint32_t sysclk) {
    eERRORRESULT Error;
    uint8_t Id = 0, Revision = 0;
    eMCP251XFD_Devices Dev = MCP2517FD;
    Error = MCP251XFD_GetDeviceID(pComp, &Dev, &Id, &Revision);
    if(Error == ERR_OK) {
        unsigned int FsckI = sysclk / 1000000; // Integer form of MHz
        unsigned int FsckD =
            (sysclk / 1000) - (FsckI * 1000); // Decimal form of MHz with 3 digits of precision
        unsigned int SpiClk = pComp->SPIClockSpeed;
        unsigned int FspiI = SpiClk / 1000000; // Integer form of MHz
        unsigned int FspiD =
            (SpiClk / 1000) - (FspiI * 1000); // Decimal form of MHz with 3 digits of precision
        if(Dev == MCP2517FD)
            FURI_LOG_I(
                TAG,
                "%s: Device detected %s\t\t   @ %u.%uMHz (SPI @ %u.%uMHz)",
                CANx,
                MCP251XFD_DevicesNames[Dev],
                FsckI,
                FsckD,
                FspiI,
                FspiD);
        else
            FURI_LOG_I(
                TAG,
                "%s: Device detected %s (Id:0x%X Rev:0x%X) @ %u.%uMHz (SPI @ %u.%uMHz)",
                CANx,
                MCP251XFD_DevicesNames[Dev],
                Id,
                Revision,
                FsckI,
                FsckD,
                FspiI,
                FspiD);
    } else
        show_device_error(pComp, Error);
}

//=============================================================================
// Show device configuration
//=============================================================================
void show_device_configuration(MCP251XFD_BitTimeStats* btStats) {
    if(btStats == NULL) return;

    //--- Show nominal bitrate infos ---
    unsigned int NominalkBps = btStats->NominalBitrate / 1000u;
    FURI_LOG_I(
        TAG,
        "      Nominal Bitrate: %ukbps, Sample Point: %lu (Max bus length: %um)",
        NominalkBps,
        btStats->NSamplePoint,
        (unsigned int)btStats->MaxBusLength);

    //--- Show data bitrate infos ---
    if(btStats->DataBitrate != 0) {
        unsigned int DatakBps = btStats->DataBitrate / 1000u;
        FURI_LOG_I(
            TAG,
            "      Data    Bitrate: %ukbps, Sample Point: %lu",
            DatakBps,
            btStats->DSamplePoint);
    }
}

//=============================================================================
// Show device FIFO configuration
//=============================================================================
const char YesNoNames[3][3 + 1 /* \0 */] = {
    " NO",
    "YES",
    "N/A",
};

const char MCP251XFD_FIFONames[MCP251XFD_FIFO_COUNT][7] = {
    "  TXQ ", " FIFO1", " FIFO2", " FIFO3", " FIFO4", " FIFO5", " FIFO6", " FIFO7",
    " FIFO8", " FIFO9", "FIFO10", "FIFO11", "FIFO12", "FIFO13", "FIFO14", "FIFO15",
    "FIFO16", "FIFO17", "FIFO18", "FIFO19", "FIFO20", "FIFO21", "FIFO22", "FIFO23",
    "FIFO24", "FIFO25", "FIFO26", "FIFO27", "FIFO28", "FIFO29", "FIFO30", "FIFO31",
};

void show_device_fifo_configuration(MCP251XFD_FIFO* listFIFO, size_t count) {
    if(listFIFO == NULL) return;

    //--- Some Checks ---
    for(size_t z = 0; z < count; z++)
        if(listFIFO[z].RAMInfos == NULL) {
            FURI_LOG_I(
                TAG,
                "      Can't show FIFO configuration because one or more RAMInfos are missing");
            return;
        }

    //--- Show RAM used ---
    uint32_t TotalRAM = 0;
    for(size_t z = 0; z < count; z++) TotalRAM += listFIFO[z].RAMInfos->ByteInFIFO;
    FURI_LOG_I(TAG, "      Total RAM used: %u Bytes", (unsigned int)TotalRAM);

    //--- Show table header ---
    FURI_LOG_I(
        TAG, "      ---------------------------------------------------------------------------");
    FURI_LOG_I(
        TAG, "      |  FIFO  | Obj | Payload | TS  | ByteObj | ByteFIFO | StartAddr | EndAddr |");
    FURI_LOG_I(
        TAG, "      ---------------------------------------------------------------------------");

    //--- Show TEF infos ---
    for(size_t z = 0; z < count; z++)
        if(listFIFO[z].Name == MCP251XFD_TEF) {
            MCP251XFD_RAMInfos* RI = listFIFO[z].RAMInfos;
            size_t TS =
                ((listFIFO[z].ControlFlags & MCP251XFD_FIFO_ADD_TIMESTAMP_ON_OBJ) > 0 ? 1 : 0);
            FURI_LOG_I(
                TAG,
                "      |  TEF   |  %2u |   N/A   | %s |    %2u   |   %4u   |   0x%3X   |  0x%3X  |",
                (unsigned int)(listFIFO[z].Size + 1),
                YesNoNames[TS],
                (unsigned int)RI->ByteInObject,
                (unsigned int)RI->ByteInFIFO,
                (unsigned int)RI->RAMStartAddress,
                (unsigned int)(RI->RAMStartAddress + RI->ByteInFIFO));
            break;
        }

    //--- Show TXQ infos ---
    for(size_t z = 0; z < count; z++)
        if(listFIFO[z].Name == MCP251XFD_TXQ) {
            MCP251XFD_RAMInfos* RI = listFIFO[z].RAMInfos;
            FURI_LOG_I(
                TAG,
                "      |  TXQ   |  %2u |    %2u   | N/A |    %2u   |   %4u   |   0x%3X   |  0x%3X  |",
                (unsigned int)(listFIFO[z].Size + 1),
                MCP251XFD_PayloadToByte(listFIFO[z].Payload),
                (unsigned int)RI->ByteInObject,
                (unsigned int)RI->ByteInFIFO,
                (unsigned int)RI->RAMStartAddress,
                (unsigned int)(RI->RAMStartAddress + RI->ByteInFIFO));
            break;
        }

    //--- Show FIFOs infos ---
    for(int32_t zFIFO = 1; zFIFO < MCP251XFD_FIFO_COUNT; zFIFO++)
        for(size_t z = 0; z < count; z++)
            if(listFIFO[z].Name == zFIFO) {
                MCP251XFD_RAMInfos* RI = listFIFO[z].RAMInfos;
                size_t TS =
                    ((listFIFO[z].ControlFlags & MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX) > 0 ? 1 : 0);
                if(listFIFO[z].Direction == MCP251XFD_TRANSMIT_FIFO) TS = 2;
                FURI_LOG_I(
                    TAG,
                    "      | %s |  %2u |    %2u   | %s |    %2u   |   %4u   |   0x%3X   |  0x%3X  |",
                    MCP251XFD_FIFONames[zFIFO],
                    (unsigned int)(listFIFO[z].Size + 1),
                    MCP251XFD_PayloadToByte(listFIFO[z].Payload),
                    YesNoNames[TS],
                    (unsigned int)RI->ByteInObject,
                    (unsigned int)RI->ByteInFIFO,
                    (unsigned int)RI->RAMStartAddress,
                    (unsigned int)(RI->RAMStartAddress + RI->ByteInFIFO));
                break;
            }

    //--- Show table closing ---
    FURI_LOG_I(
        TAG, "      ---------------------------------------------------------------------------");
}

//=============================================================================
// Show device Filter configuration
//=============================================================================
const char MCP251XFD_FilterNames[MCP251XFD_FILTER_COUNT][9] = {
    "Filter 0", "Filter 1", "Filter 2", "Filter 3", "Filter 4", "Filter 5", "Filter 6", "Filter 7",
    "Filter 8", "Filter 9", "Filter10", "Filter11", "Filter12", "Filter13", "Filter14", "Filter15",
    "Filter16", "Filter17", "Filter18", "Filter19", "Filter20", "Filter21", "Filter22", "Filter23",
    "Filter24", "Filter25", "Filter26", "Filter27", "Filter28", "Filter29", "Filter30", "Filter31",
};

static char GetCorrespondingCharOf(uint32_t acceptanceID, uint32_t acceptanceMask, uint8_t bit) {
    if((acceptanceMask & (1 << bit)) > 0) // Corresponding bit Mask set ?
    {
        return ((acceptanceID & (1 << bit)) > 0) ? '1' :
                                                   '0'; // Put the Address Acceptance actual bit
    } else
        return '.'; // Else set a '.' to indicate "don't care"
}

void show_device_filter_configuration(MCP251XFD_Filter* listFilter, size_t count, bool UseSID11) {
    if(listFilter == NULL) return;
#define ADDR_BITMAP_MAX (MCP251XFD_EID_Size + 1 + MCP251XFD_SID_Size)

    char AcceptBitMap[ADDR_BITMAP_MAX + 1]; // 30bits + /0
    AcceptBitMap[ADDR_BITMAP_MAX] = 0; // Set the end char

    //--- Show table header ---
    FURI_LOG_I(TAG, "      --------------------------------------------------------");
    FURI_LOG_I(TAG, "      |   Name   | Addr: SID            EID       | Point To |");
    FURI_LOG_I(TAG, "      --------------------------------------------------------");

    //--- Show Filters infos ---
    for(size_t z = 0; z < count; z++) {
        for(int32_t zChar = ADDR_BITMAP_MAX; --zChar >= 0;)
            AcceptBitMap[zChar] = ' '; // Clear bitmap

        switch(listFilter[z].Match) {
        case MCP251XFD_MATCH_ONLY_SID: {
            for(int32_t zChar = (MCP251XFD_SID_Size + 1); --zChar >= 0;) {
                if(UseSID11 && (zChar == MCP251XFD_SID_Size))
                    AcceptBitMap[MCP251XFD_SID_Size - zChar] = GetCorrespondingCharOf(
                        listFilter[z].AcceptanceID, listFilter[z].AcceptanceMask, zChar);
                else {
                    if(!UseSID11 && (zChar == MCP251XFD_SID_Size))
                        AcceptBitMap[MCP251XFD_SID_Size - zChar] = ' ';
                    else
                        AcceptBitMap[MCP251XFD_SID_Size - zChar] = GetCorrespondingCharOf(
                            listFilter[z].AcceptanceID, listFilter[z].AcceptanceMask, zChar);
                }
            }
        } break;
        case MCP251XFD_MATCH_ONLY_EID:
        /*{
        for (int32_t zChar = EID_Size; --zChar >= 0;)
        AcceptBitMap[SID_Size+EID_Size-zChar] = GetCorrespondingCharOf(listFilter[z].AcceptanceID, listFilter[z].AcceptanceMask, zChar);
      }
      break;*/
        case MCP251XFD_MATCH_SID_EID: {
            for(int32_t zChar = (MCP251XFD_SID_Size + MCP251XFD_EID_Size + 1); --zChar >= 0;) {
                if(UseSID11 && (zChar == (MCP251XFD_SID_Size + MCP251XFD_EID_Size)))
                    AcceptBitMap[MCP251XFD_SID_Size + MCP251XFD_EID_Size - zChar] =
                        GetCorrespondingCharOf(
                            listFilter[z].AcceptanceID, listFilter[z].AcceptanceMask, zChar);
                else {
                    if(!UseSID11 && (zChar == (MCP251XFD_SID_Size + MCP251XFD_EID_Size)))
                        AcceptBitMap[MCP251XFD_SID_Size + MCP251XFD_EID_Size - zChar] = ' ';
                    else
                        AcceptBitMap[MCP251XFD_SID_Size + MCP251XFD_EID_Size - zChar] =
                            GetCorrespondingCharOf(
                                listFilter[z].AcceptanceID, listFilter[z].AcceptanceMask, zChar);
                }
            }
        } break;
        }
        FURI_LOG_I(
            TAG,
            "      | %s | %s -> %s  |",
            MCP251XFD_FilterNames[listFilter[z].Filter],
            AcceptBitMap,
            MCP251XFD_FIFONames[listFilter[z].PointTo]);
    }

    //--- Show table closing ---
    FURI_LOG_I(TAG, "      --------------------------------------------------------");
}

//**********************************************************************************************************************************************************
typedef union {
    unsigned int Uint32[MCP251XFD_CONTROLLER_SFR_SIZE / sizeof(unsigned int)];
    uint8_t Bytes[MCP251XFD_CONTROLLER_SFR_SIZE / sizeof(uint8_t)];
} ControllerSFRData;

void show_device_show_mcp251xfd_sfr_reg(MCP251XFD* pComp) {
    ControllerSFRData CSD;
    eERRORRESULT Error = MCP251XFD_ReadData(
        pComp, MCP251XFD_CONTROLLER_SFR_ADDR, &CSD.Bytes[0], MCP251XFD_CONTROLLER_SFR_SIZE);
    if(Error == ERR_OK) {
        FURI_LOG_I(TAG, "%s: MCP251XFD Special Function Registers:", CANx);
        FURI_LOG_I(TAG, "%s:   OSC     = 0x%08X", CANx, CSD.Uint32[0]);
        FURI_LOG_I(TAG, "%s:   IOCON   = 0x%08X", CANx, CSD.Uint32[1]);
        FURI_LOG_I(TAG, "%s:   CRC     = 0x%08X", CANx, CSD.Uint32[2]);
        FURI_LOG_I(TAG, "%s:   ECCCON  = 0x%08X", CANx, CSD.Uint32[3]);
        FURI_LOG_I(TAG, "%s:   ECCSTAT = 0x%08X", CANx, CSD.Uint32[4]);
        FURI_LOG_I(TAG, "%s:   DEVID   = 0x%08X", CANx, CSD.Uint32[5]);
    } else
        show_device_error(pComp, Error);
}

#define CAN_SFR_SIZE (16 * sizeof(unsigned int))

typedef union {
    unsigned int Uint32[CAN_SFR_SIZE / sizeof(unsigned int)];
    uint8_t Bytes[CAN_SFR_SIZE / sizeof(uint8_t)];
} CANControllerSFRData;

void show_device_show_mcp251xfd_can_sfr_reg(MCP251XFD* pComp) {
    CANControllerSFRData CCSD;
    eERRORRESULT Error =
        MCP251XFD_ReadData(pComp, MCP251XFD_CAN_CONTROLLER_ADDR, &CCSD.Bytes[0], CAN_SFR_SIZE);
    if(Error == ERR_OK) {
        FURI_LOG_I(TAG, "%s: CAN Controller Special Function Registers:", CANx);
        FURI_LOG_I(TAG, "%s:   C1CON    = 0x%08X", CANx, CCSD.Uint32[0]);
        FURI_LOG_I(TAG, "%s:   C1NBTCFG = 0x%08X", CANx, CCSD.Uint32[1]);
        FURI_LOG_I(TAG, "%s:   C1DBTCFG = 0x%08X", CANx, CCSD.Uint32[2]);
        FURI_LOG_I(TAG, "%s:   C1TDC    = 0x%08X", CANx, CCSD.Uint32[3]);
        FURI_LOG_I(TAG, "%s:   C1TBC    = 0x%08X", CANx, CCSD.Uint32[4]);
        FURI_LOG_I(TAG, "%s:   C1TSCON  = 0x%08X", CANx, CCSD.Uint32[5]);
        FURI_LOG_I(TAG, "%s:   C1VEC    = 0x%08X", CANx, CCSD.Uint32[6]);
        FURI_LOG_I(TAG, "%s:   C1INT    = 0x%08X", CANx, CCSD.Uint32[7]);
        FURI_LOG_I(TAG, "%s:   C1RXIF   = 0x%08X", CANx, CCSD.Uint32[8]);
        FURI_LOG_I(TAG, "%s:   C1TXIF   = 0x%08X", CANx, CCSD.Uint32[9]);
        FURI_LOG_I(TAG, "%s:   C1RXOVIF = 0x%08X", CANx, CCSD.Uint32[10]);
        FURI_LOG_I(TAG, "%s:   C1TXATIF = 0x%08X", CANx, CCSD.Uint32[11]);
        FURI_LOG_I(TAG, "%s:   C1TXREQ  = 0x%08X", CANx, CCSD.Uint32[12]);
        FURI_LOG_I(TAG, "%s:   C1TREC   = 0x%08X", CANx, CCSD.Uint32[13]);
        FURI_LOG_I(TAG, "%s:   C1BDIAG0 = 0x%08X", CANx, CCSD.Uint32[14]);
        FURI_LOG_I(TAG, "%s:   C1BDIAG1 = 0x%08X", CANx, CCSD.Uint32[15]);
    } else
        show_device_error(pComp, Error);
}

#define CAN_SFR_FIFO_SIZE ((3 + 1 + 3 + 3 * 31) * sizeof(unsigned int))

typedef union {
    unsigned int Uint32[CAN_SFR_FIFO_SIZE / sizeof(unsigned int)];
    uint8_t Bytes[CAN_SFR_FIFO_SIZE / sizeof(uint8_t)];
} FIFOSFRData;

void show_device_show_mcp251xfd_fifo_reg(MCP251XFD* pComp) {
    FIFOSFRData FSD;
    eERRORRESULT Error =
        MCP251XFD_ReadData(pComp, RegMCP251XFD_CiTEFCON, &FSD.Bytes[0], CAN_SFR_FIFO_SIZE);
    if(Error == ERR_OK) {
        FURI_LOG_I(TAG, "%s: CAN Controller FIFOs:", CANx);
        FURI_LOG_I(
            TAG,
            "%s:   TEF   : C1TEFCON    = 0x%08X ; C1TEFSTA    = 0x%08X ; C1TEFUA    = 0x%08X",
            CANx,
            FSD.Uint32[0],
            FSD.Uint32[1],
            FSD.Uint32[2]);
        FURI_LOG_I(
            TAG,
            "%s:   TXQ   : C1TXQCON    = 0x%08X ; C1TXQSTA    = 0x%08X ; C1TXQUA    = 0x%08X",
            CANx,
            FSD.Uint32[4],
            FSD.Uint32[5],
            FSD.Uint32[6]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO1 : C1FIFOCON1  = 0x%08X ; C1FIFOSTA1  = 0x%08X ; C1FIFOUA1  = 0x%08X",
            CANx,
            FSD.Uint32[7],
            FSD.Uint32[8],
            FSD.Uint32[9]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO2 : C1FIFOCON2  = 0x%08X ; C1FIFOSTA2  = 0x%08X ; C1FIFOUA2  = 0x%08X",
            CANx,
            FSD.Uint32[10],
            FSD.Uint32[11],
            FSD.Uint32[12]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO3 : C1FIFOCON3  = 0x%08X ; C1FIFOSTA3  = 0x%08X ; C1FIFOUA3  = 0x%08X",
            CANx,
            FSD.Uint32[13],
            FSD.Uint32[14],
            FSD.Uint32[15]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO4 : C1FIFOCON4  = 0x%08X ; C1FIFOSTA4  = 0x%08X ; C1FIFOUA4  = 0x%08X",
            CANx,
            FSD.Uint32[16],
            FSD.Uint32[17],
            FSD.Uint32[18]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO5 : C1FIFOCON5  = 0x%08X ; C1FIFOSTA5  = 0x%08X ; C1FIFOUA5  = 0x%08X",
            CANx,
            FSD.Uint32[19],
            FSD.Uint32[20],
            FSD.Uint32[21]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO6 : C1FIFOCON6  = 0x%08X ; C1FIFOSTA6  = 0x%08X ; C1FIFOUA6  = 0x%08X",
            CANx,
            FSD.Uint32[22],
            FSD.Uint32[23],
            FSD.Uint32[24]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO7 : C1FIFOCON7  = 0x%08X ; C1FIFOSTA7  = 0x%08X ; C1FIFOUA7  = 0x%08X",
            CANx,
            FSD.Uint32[25],
            FSD.Uint32[26],
            FSD.Uint32[27]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO8 : C1FIFOCON8  = 0x%08X ; C1FIFOSTA8  = 0x%08X ; C1FIFOUA8  = 0x%08X",
            CANx,
            FSD.Uint32[28],
            FSD.Uint32[29],
            FSD.Uint32[30]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO9 : C1FIFOCON9  = 0x%08X ; C1FIFOSTA9  = 0x%08X ; C1FIFOUA9  = 0x%08X",
            CANx,
            FSD.Uint32[31],
            FSD.Uint32[32],
            FSD.Uint32[33]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO10: C1FIFOCON10 = 0x%08X ; C1FIFOSTA10 = 0x%08X ; C1FIFOUA10 = 0x%08X",
            CANx,
            FSD.Uint32[34],
            FSD.Uint32[35],
            FSD.Uint32[36]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO11: C1FIFOCON11 = 0x%08X ; C1FIFOSTA11 = 0x%08X ; C1FIFOUA11 = 0x%08X",
            CANx,
            FSD.Uint32[37],
            FSD.Uint32[38],
            FSD.Uint32[39]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO12: C1FIFOCON12 = 0x%08X ; C1FIFOSTA12 = 0x%08X ; C1FIFOUA12 = 0x%08X",
            CANx,
            FSD.Uint32[40],
            FSD.Uint32[41],
            FSD.Uint32[42]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO13: C1FIFOCON13 = 0x%08X ; C1FIFOSTA13 = 0x%08X ; C1FIFOUA13 = 0x%08X",
            CANx,
            FSD.Uint32[43],
            FSD.Uint32[44],
            FSD.Uint32[45]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO14: C1FIFOCON14 = 0x%08X ; C1FIFOSTA14 = 0x%08X ; C1FIFOUA14 = 0x%08X",
            CANx,
            FSD.Uint32[46],
            FSD.Uint32[47],
            FSD.Uint32[48]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO15: C1FIFOCON15 = 0x%08X ; C1FIFOSTA15 = 0x%08X ; C1FIFOUA15 = 0x%08X",
            CANx,
            FSD.Uint32[49],
            FSD.Uint32[50],
            FSD.Uint32[51]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO16: C1FIFOCON16 = 0x%08X ; C1FIFOSTA16 = 0x%08X ; C1FIFOUA16 = 0x%08X",
            CANx,
            FSD.Uint32[52],
            FSD.Uint32[53],
            FSD.Uint32[54]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO17: C1FIFOCON17 = 0x%08X ; C1FIFOSTA17 = 0x%08X ; C1FIFOUA17 = 0x%08X",
            CANx,
            FSD.Uint32[55],
            FSD.Uint32[56],
            FSD.Uint32[57]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO18: C1FIFOCON18 = 0x%08X ; C1FIFOSTA18 = 0x%08X ; C1FIFOUA18 = 0x%08X",
            CANx,
            FSD.Uint32[58],
            FSD.Uint32[59],
            FSD.Uint32[60]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO19: C1FIFOCON19 = 0x%08X ; C1FIFOSTA19 = 0x%08X ; C1FIFOUA19 = 0x%08X",
            CANx,
            FSD.Uint32[61],
            FSD.Uint32[62],
            FSD.Uint32[63]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO20: C1FIFOCON20 = 0x%08X ; C1FIFOSTA20 = 0x%08X ; C1FIFOUA20 = 0x%08X",
            CANx,
            FSD.Uint32[64],
            FSD.Uint32[65],
            FSD.Uint32[66]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO21: C1FIFOCON21 = 0x%08X ; C1FIFOSTA21 = 0x%08X ; C1FIFOUA21 = 0x%08X",
            CANx,
            FSD.Uint32[67],
            FSD.Uint32[68],
            FSD.Uint32[69]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO22: C1FIFOCON22 = 0x%08X ; C1FIFOSTA22 = 0x%08X ; C1FIFOUA22 = 0x%08X",
            CANx,
            FSD.Uint32[70],
            FSD.Uint32[71],
            FSD.Uint32[72]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO23: C1FIFOCON23 = 0x%08X ; C1FIFOSTA23 = 0x%08X ; C1FIFOUA23 = 0x%08X",
            CANx,
            FSD.Uint32[73],
            FSD.Uint32[74],
            FSD.Uint32[75]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO24: C1FIFOCON24 = 0x%08X ; C1FIFOSTA24 = 0x%08X ; C1FIFOUA24 = 0x%08X",
            CANx,
            FSD.Uint32[76],
            FSD.Uint32[77],
            FSD.Uint32[78]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO25: C1FIFOCON25 = 0x%08X ; C1FIFOSTA25 = 0x%08X ; C1FIFOUA25 = 0x%08X",
            CANx,
            FSD.Uint32[79],
            FSD.Uint32[80],
            FSD.Uint32[81]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO26: C1FIFOCON26 = 0x%08X ; C1FIFOSTA26 = 0x%08X ; C1FIFOUA26 = 0x%08X",
            CANx,
            FSD.Uint32[82],
            FSD.Uint32[83],
            FSD.Uint32[84]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO27: C1FIFOCON27 = 0x%08X ; C1FIFOSTA27 = 0x%08X ; C1FIFOUA27 = 0x%08X",
            CANx,
            FSD.Uint32[85],
            FSD.Uint32[86],
            FSD.Uint32[87]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO28: C1FIFOCON28 = 0x%08X ; C1FIFOSTA28 = 0x%08X ; C1FIFOUA28 = 0x%08X",
            CANx,
            FSD.Uint32[88],
            FSD.Uint32[89],
            FSD.Uint32[90]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO29: C1FIFOCON29 = 0x%08X ; C1FIFOSTA29 = 0x%08X ; C1FIFOUA29 = 0x%08X",
            CANx,
            FSD.Uint32[91],
            FSD.Uint32[92],
            FSD.Uint32[93]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO30: C1FIFOCON30 = 0x%08X ; C1FIFOSTA30 = 0x%08X ; C1FIFOUA30 = 0x%08X",
            CANx,
            FSD.Uint32[94],
            FSD.Uint32[95],
            FSD.Uint32[96]);
        FURI_LOG_I(
            TAG,
            "%s:   FIFO31: C1FIFOCON31 = 0x%08X ; C1FIFOSTA31 = 0x%08X ; C1FIFOUA31 = 0x%08X",
            CANx,
            FSD.Uint32[97],
            FSD.Uint32[98],
            FSD.Uint32[99]);
    } else
        show_device_error(pComp, Error);
}

#define CAN_SFR_FILTER_SIZE ((8 + 2 * 32) * sizeof(unsigned int))

typedef union {
    unsigned int Uint32[CAN_SFR_FILTER_SIZE / sizeof(unsigned int)];
    uint8_t Bytes[CAN_SFR_FILTER_SIZE / sizeof(uint8_t)];
} FilterSFRData;

void show_device_show_mcp251xfd_filter_reg(MCP251XFD* pComp) {
    FIFOSFRData FSD;
    eERRORRESULT Error =
        MCP251XFD_ReadData(pComp, RegMCP251XFD_CiFLTCON0, &FSD.Bytes[0], CAN_SFR_FILTER_SIZE);
    if(Error == ERR_OK) {
        FURI_LOG_I(TAG, "%s: CAN Controller Filters:", CANx);
        FURI_LOG_I(
            TAG,
            "%s:   Filter0 : C1FLTCON0 = 0x%02X ; C1FLTOBJ0  = 0x%08X ; C1MASK0  = 0x%08X",
            CANx,
            FSD.Bytes[0],
            FSD.Uint32[8],
            FSD.Uint32[9]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter1 : C1FLTCON0 = 0x%02X ; C1FLTOBJ1  = 0x%08X ; C1MASK1  = 0x%08X",
            CANx,
            FSD.Bytes[1],
            FSD.Uint32[10],
            FSD.Uint32[11]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter2 : C1FLTCON0 = 0x%02X ; C1FLTOBJ2  = 0x%08X ; C1MASK2  = 0x%08X",
            CANx,
            FSD.Bytes[2],
            FSD.Uint32[12],
            FSD.Uint32[13]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter3 : C1FLTCON0 = 0x%02X ; C1FLTOBJ3  = 0x%08X ; C1MASK3  = 0x%08X",
            CANx,
            FSD.Bytes[3],
            FSD.Uint32[14],
            FSD.Uint32[15]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter4 : C1FLTCON1 = 0x%02X ; C1FLTOBJ4  = 0x%08X ; C1MASK4  = 0x%08X",
            CANx,
            FSD.Bytes[4],
            FSD.Uint32[16],
            FSD.Uint32[17]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter5 : C1FLTCON1 = 0x%02X ; C1FLTOBJ5  = 0x%08X ; C1MASK5  = 0x%08X",
            CANx,
            FSD.Bytes[5],
            FSD.Uint32[18],
            FSD.Uint32[19]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter6 : C1FLTCON1 = 0x%02X ; C1FLTOBJ6  = 0x%08X ; C1MASK6  = 0x%08X",
            CANx,
            FSD.Bytes[6],
            FSD.Uint32[20],
            FSD.Uint32[21]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter7 : C1FLTCON1 = 0x%02X ; C1FLTOBJ7  = 0x%08X ; C1MASK7  = 0x%08X",
            CANx,
            FSD.Bytes[7],
            FSD.Uint32[22],
            FSD.Uint32[23]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter8 : C1FLTCON2 = 0x%02X ; C1FLTOBJ8  = 0x%08X ; C1MASK8  = 0x%08X",
            CANx,
            FSD.Bytes[8],
            FSD.Uint32[24],
            FSD.Uint32[25]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter9 : C1FLTCON2 = 0x%02X ; C1FLTOBJ9  = 0x%08X ; C1MASK9  = 0x%08X",
            CANx,
            FSD.Bytes[9],
            FSD.Uint32[26],
            FSD.Uint32[27]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter10: C1FLTCON2 = 0x%02X ; C1FLTOBJ10 = 0x%08X ; C1MASK10 = 0x%08X",
            CANx,
            FSD.Bytes[10],
            FSD.Uint32[28],
            FSD.Uint32[29]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter11: C1FLTCON2 = 0x%02X ; C1FLTOBJ11 = 0x%08X ; C1MASK11 = 0x%08X",
            CANx,
            FSD.Bytes[11],
            FSD.Uint32[30],
            FSD.Uint32[31]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter12: C1FLTCON3 = 0x%02X ; C1FLTOBJ12 = 0x%08X ; C1MASK12 = 0x%08X",
            CANx,
            FSD.Bytes[12],
            FSD.Uint32[32],
            FSD.Uint32[33]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter13: C1FLTCON3 = 0x%02X ; C1FLTOBJ13 = 0x%08X ; C1MASK13 = 0x%08X",
            CANx,
            FSD.Bytes[13],
            FSD.Uint32[34],
            FSD.Uint32[35]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter14: C1FLTCON3 = 0x%02X ; C1FLTOBJ14 = 0x%08X ; C1MASK14 = 0x%08X",
            CANx,
            FSD.Bytes[14],
            FSD.Uint32[36],
            FSD.Uint32[37]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter15: C1FLTCON3 = 0x%02X ; C1FLTOBJ15 = 0x%08X ; C1MASK15 = 0x%08X",
            CANx,
            FSD.Bytes[15],
            FSD.Uint32[38],
            FSD.Uint32[39]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter16: C1FLTCON4 = 0x%02X ; C1FLTOBJ16 = 0x%08X ; C1MASK16 = 0x%08X",
            CANx,
            FSD.Bytes[16],
            FSD.Uint32[40],
            FSD.Uint32[41]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter17: C1FLTCON4 = 0x%02X ; C1FLTOBJ17 = 0x%08X ; C1MASK17 = 0x%08X",
            CANx,
            FSD.Bytes[17],
            FSD.Uint32[42],
            FSD.Uint32[43]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter18: C1FLTCON4 = 0x%02X ; C1FLTOBJ18 = 0x%08X ; C1MASK18 = 0x%08X",
            CANx,
            FSD.Bytes[18],
            FSD.Uint32[44],
            FSD.Uint32[45]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter19: C1FLTCON4 = 0x%02X ; C1FLTOBJ19 = 0x%08X ; C1MASK19 = 0x%08X",
            CANx,
            FSD.Bytes[19],
            FSD.Uint32[46],
            FSD.Uint32[47]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter20: C1FLTCON5 = 0x%02X ; C1FLTOBJ20 = 0x%08X ; C1MASK20 = 0x%08X",
            CANx,
            FSD.Bytes[20],
            FSD.Uint32[48],
            FSD.Uint32[49]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter21: C1FLTCON5 = 0x%02X ; C1FLTOBJ21 = 0x%08X ; C1MASK21 = 0x%08X",
            CANx,
            FSD.Bytes[21],
            FSD.Uint32[50],
            FSD.Uint32[51]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter22: C1FLTCON5 = 0x%02X ; C1FLTOBJ22 = 0x%08X ; C1MASK22 = 0x%08X",
            CANx,
            FSD.Bytes[22],
            FSD.Uint32[52],
            FSD.Uint32[53]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter23: C1FLTCON5 = 0x%02X ; C1FLTOBJ23 = 0x%08X ; C1MASK23 = 0x%08X",
            CANx,
            FSD.Bytes[23],
            FSD.Uint32[54],
            FSD.Uint32[55]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter24: C1FLTCON6 = 0x%02X ; C1FLTOBJ24 = 0x%08X ; C1MASK24 = 0x%08X",
            CANx,
            FSD.Bytes[24],
            FSD.Uint32[56],
            FSD.Uint32[57]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter25: C1FLTCON6 = 0x%02X ; C1FLTOBJ25 = 0x%08X ; C1MASK25 = 0x%08X",
            CANx,
            FSD.Bytes[25],
            FSD.Uint32[58],
            FSD.Uint32[59]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter26: C1FLTCON6 = 0x%02X ; C1FLTOBJ26 = 0x%08X ; C1MASK26 = 0x%08X",
            CANx,
            FSD.Bytes[26],
            FSD.Uint32[60],
            FSD.Uint32[61]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter27: C1FLTCON6 = 0x%02X ; C1FLTOBJ27 = 0x%08X ; C1MASK27 = 0x%08X",
            CANx,
            FSD.Bytes[27],
            FSD.Uint32[62],
            FSD.Uint32[63]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter28: C1FLTCON7 = 0x%02X ; C1FLTOBJ28 = 0x%08X ; C1MASK28 = 0x%08X",
            CANx,
            FSD.Bytes[28],
            FSD.Uint32[64],
            FSD.Uint32[65]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter29: C1FLTCON7 = 0x%02X ; C1FLTOBJ29 = 0x%08X ; C1MASK29 = 0x%08X",
            CANx,
            FSD.Bytes[29],
            FSD.Uint32[66],
            FSD.Uint32[67]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter30: C1FLTCON7 = 0x%02X ; C1FLTOBJ30 = 0x%08X ; C1MASK30 = 0x%08X",
            CANx,
            FSD.Bytes[30],
            FSD.Uint32[68],
            FSD.Uint32[69]);
        FURI_LOG_I(
            TAG,
            "%s:   Filter31: C1FLTCON7 = 0x%02X ; C1FLTOBJ31 = 0x%08X ; C1MASK31 = 0x%08X",
            CANx,
            FSD.Bytes[31],
            FSD.Uint32[70],
            FSD.Uint32[71]);
    } else
        show_device_error(pComp, Error);
}

//**********************************************************************************************************************************************************
//=============================================================================
// Show the current error of the device
//=============================================================================
void show_device_error(MCP251XFD* pComp, eERRORRESULT error) {
    UNUSED(pComp);
    char* pStr = NULL;
    /*  switch (error)
  {
#   define X(a, b, c) case a: pStr = (char*)c; break;
    ERRORS_TABLE
#   undef X
  }*/

    if(error < ERR__ERRORS_MAX) FURI_LOG_E(TAG, "error: %d", error);
    if(pStr != NULL)
        FURI_LOG_E(TAG, "%s: Device error: %s", CANx, pStr);
    else
        FURI_LOG_E(TAG, "%s: Device error: Unknown error (%u)", CANx, (unsigned int)error);
}
