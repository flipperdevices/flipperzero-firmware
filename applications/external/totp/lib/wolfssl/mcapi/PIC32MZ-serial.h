void _mon_putc(char c);

#define BAUD_GEN(sysclk, baud) ((sysclk / (16 * baud)) - 1)

#ifdef MICROCHIP_PIC32
#if defined (__32MZ2048ECH144__) || defined(__32MZ2048ECM144__) || defined(__32MZ2048EFM144__)
    /* Code generated from Harmony example then exported using Window -> PIC32 Memory View -> Configuration Bits into system_config.h */
    #define SYS_CLK_FREQ                    200000000ul
    #define SYS_CLK_BUS_PERIPHERAL_2        100000000ul

    // DEVCFG3
    #pragma config FMIIEN = ON              // Ethernet RMII/MII Enable (MII Enabled)
    #pragma config FETHIO = ON              // Ethernet I/O Pin Select (Default Ethernet I/O)
    #pragma config PGL1WAY = ON             // Permission Group Lock One Way Configuration (Allow only one reconfiguration)
    #pragma config PMDL1WAY = ON            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
    #pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow only one reconfiguration)
    #pragma config FUSBIDIO = ON            // USB USBID Selection (Controlled by the USB Module)

    // DEVCFG2
    #pragma config FPLLIDIV = DIV_1         // System PLL Input Divider (1x Divider)
    #pragma config FPLLRNG = RANGE_5_10_MHZ // System PLL Input Range (5-10 MHz Input)
    #pragma config FPLLICLK = PLL_FRC       // System PLL Input Clock Selection (FRC is input to the System PLL)
    #pragma config FPLLMULT = MUL_50        // System PLL Multiplier (PLL Multiply by 50)
    #pragma config FPLLODIV = DIV_2         // System PLL Output Clock Divider (2x Divider)
    #pragma config UPLLFSEL = FREQ_24MHZ    // USB PLL Input Frequency Selection (USB PLL input is 24 MHz)

    // DEVCFG1
    #pragma config FNOSC = SPLL             // Oscillator Selection Bits (System PLL)
    #pragma config DMTINTV = WIN_127_128    // DMT Count Window Interval (Window/Interval value is 127/128 counter value)
    #pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disable SOSC)
    #pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
    #pragma config POSCMOD = OFF            // Primary Oscillator Configuration (Primary osc disabled)
    #pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
    #pragma config FCKSM = CSECME           // Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Enabled)
    #pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
    #pragma config WDTSPGM = STOP           // Watchdog Timer Stop During Flash Programming (WDT stops during Flash programming)
    #pragma config WINDIS = NORMAL          // Watchdog Timer Window Mode (Watchdog Timer is in non-Window mode)
    #pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled)
    #pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window size is 25%)
    #pragma config DMTCNT = DMT31           // Deadman Timer Count Selection (2^31 (2147483648))
    #pragma config FDMTEN = OFF             // Deadman Timer Enable (Deadman Timer is disabled)

    // DEVCFG0
    #pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (Communicate on PGEC2/PGED2)

    // DEVCP0
    #pragma config CP = OFF                 // Code Protect (Protection Disabled)

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
