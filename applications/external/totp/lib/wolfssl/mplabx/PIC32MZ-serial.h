void _mon_putc(char c);

#define BAUD_GEN(sysclk, baud) ((sysclk / (16 * baud)) - 1)

#ifdef MICROCHIP_PIC32
#if defined (__32MZ2048ECH144__) || defined(__32MZ2048ECM144__) || defined(__32MZ2048EFM144__)
    /* Code generated from Harmony example then exported using Window -> PIC32 Memory View -> Configuration Bits into system_config.h */
    #define SYS_CLK_FREQ                    200000000ul
    #define SYS_CLK_BUS_PERIPHERAL_2        100000000ul

    /* PIC32MZ2048EFM144 Configuration Bit Settings */

    /*** DEVCFG0 ***/
    #pragma config DEBUG =      OFF
    #pragma config JTAGEN =     OFF
    #pragma config ICESEL =     ICS_PGx2
    #pragma config TRCEN =      OFF
    #pragma config BOOTISA =    MIPS32
    #pragma config FECCCON =    OFF_UNLOCKED
    #pragma config FSLEEP =     OFF
    #pragma config DBGPER =     PG_ALL
    #pragma config SMCLR =      MCLR_NORM
    #pragma config SOSCGAIN =   GAIN_2X
    #pragma config SOSCBOOST =  ON
    #pragma config POSCGAIN =   GAIN_2X
    #pragma config POSCBOOST =  ON
    #pragma config EJTAGBEN =   NORMAL
    #pragma config CP =         OFF

    /*** DEVCFG1 ***/
    #pragma config FNOSC =      SPLL
    #pragma config DMTINTV =    WIN_127_128
    #pragma config FSOSCEN =    OFF
    #pragma config IESO =       OFF
    #pragma config POSCMOD =    EC
    #pragma config OSCIOFNC =   OFF
    #pragma config FCKSM =      CSECME
    #pragma config WDTPS =      PS1048576
    #pragma config WDTSPGM =    STOP
    #pragma config FWDTEN =     OFF
    #pragma config WINDIS =     NORMAL
    #pragma config FWDTWINSZ =  WINSZ_25
    #pragma config DMTCNT =     DMT31
    #pragma config FDMTEN =     OFF

    /*** DEVCFG2 ***/
    #pragma config FPLLIDIV =   DIV_3
    #pragma config FPLLRNG =    RANGE_5_10_MHZ
    #pragma config FPLLICLK =   PLL_POSC
    #pragma config FPLLMULT =   MUL_50
    #pragma config FPLLODIV =   DIV_2
    #pragma config UPLLFSEL =   FREQ_24MHZ

    /*** DEVCFG3 ***/
    #pragma config USERID =     0xffff
    #pragma config FMIIEN =     ON
    #pragma config FETHIO =     ON
    #pragma config PGL1WAY =    ON
    #pragma config PMDL1WAY =   ON
    #pragma config IOL1WAY =    ON
    #pragma config FUSBIDIO =   ON

    /*** BF1SEQ0 ***/
    #pragma config TSEQ =       0x0000
    #pragma config CSEQ =       0xffff

    /* #pragma config statements should precede project file includes. */
    /* Use project enums instead of #define for ON and OFF. */

    #include <xc.h>
#endif
#endif

static void init_serial(unsigned int sysClk) {
#ifdef MICROCHIP_PIC32
#if defined (__32MZ2048ECH144__) || defined(__32MZ2048ECM144__) || defined(__32MZ2048EFM144__)
    /* This is for pin B14 which is connected to the USB to UART connector J11 located under Ethernet connector */

    /* Setup UART2 */
#ifdef SYS_CLK_BUS_PERIPHERAL_2
    U2BRG = BAUD_GEN(SYS_CLK_BUS_PERIPHERAL_2, 115200);
#else
    if (sysClk > 100000000)
        sysClk /= 2;
    U2BRG = BAUD_GEN(sysClk, 115200);
#endif
    ANSELBCLR = 0x4000;
    ANSELGCLR = 0x0040;
    RPB14R = 0x02;
    U2RXR = 0x01;
    U2MODE = 0x8000;
    U2STA = 0x400;
#endif
#endif
    (void)sysClk;
}
