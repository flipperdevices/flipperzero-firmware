// This header file contains a list of user setup files and defines which one the
// compiler uses when the IDE performs a Verify/Compile or Upload.
//
// Users can create configurations for different Espressif boards and TFT displays.
// This makes selecting between hardware setups easy by "uncommenting" one line.

// The advantage of this hardware configuration method is that the examples provided
// with the library should work with different setups immediately without any other
// changes being needed. It also improves the portability of users sketches to other
// hardware configurations and compatible libraries.
//
// Create a shortcut to this file on your desktop to permit quick access for editing.
// Re-compile and upload after making and saving any changes to this file.

// Customised User_Setup files are stored in the "User_Setups" folder.

#ifndef USER_SETUP_LOADED //  Lets PlatformIO users define settings in
                          //  platformio.ini, see notes in "Tools" folder.

// Only ONE line below should be uncommented.  Add extra lines and files as needed.

//#include <User_Setup.h>           // Default setup is root library folder

//#include <User_Setup_og_marauder.h>
#include <User_Setup_marauder_mini.h>

//#include <User_Setups/Setup1_ILI9341.h>  // Setup file configured for my ILI9341
//#include <User_Setups/Setup2_ST7735.h>   // Setup file configured for my ST7735
//#include <User_Setups/Setup3_ILI9163.h>  // Setup file configured for my ILI9163
//#include <User_Setups/Setup4_S6D02A1.h>  // Setup file configured for my S6D02A1
//#include <User_Setups/Setup5_RPi_ILI9486.h>        // Setup file configured for my stock RPi TFT
//#include <User_Setups/Setup6_RPi_Wr_ILI9486.h>     // Setup file configured for my modified RPi TFT
//#include <User_Setups/Setup7_ST7735_128x128.h>     // Setup file configured for my ST7735 128x128 display
//#include <User_Setups/Setup8_ILI9163_128x128.h>    // Setup file configured for my ILI9163 128x128 display
//#include <User_Setups/Setup9_ST7735_Overlap.h>     // Setup file configured for my ST7735
//#include <User_Setups/Setup10_RPi_touch_ILI9486.h> // Setup file configured for ESP8266 and RPi TFT with touch

//#include <User_Setups/Setup11_RPi_touch_ILI9486.h> // Setup file configured for ESP32 and RPi TFT with touch
//#include <User_Setups/Setup12_M5Stack.h>           // Setup file for the ESP32 based M5Stack
//#include <User_Setups/Setup13_ILI9481_Parallel.h>  // Setup file for the ESP32 with parallel bus TFT
//#include <User_Setups/Setup14_ILI9341_Parallel.h>  // Setup file for the ESP32 with parallel bus TFT
//#include <User_Setups/Setup15_HX8357D.h>           // Setup file configured for HX8357D (untested)
//#include <User_Setups/Setup16_ILI9488_Parallel.h>  // Setup file for the ESP32 with parallel bus TFT
//#include <User_Setups/Setup17_ePaper.h>            // Setup file for any Waveshare ePaper display
//#include <User_Setups/Setup18_ST7789.h>            // Setup file configured for ST7789

//#include <User_Setups/Setup19_RM68140_Parallel.h>	 // Setup file configured for RM68140 with parallel bus

//#include <User_Setups/Setup20_ILI9488.h>           // Setup file for ESP8266 and ILI9488 SPI bus TFT
//#include <User_Setups/Setup21_ILI9488.h>           // Setup file for ESP32 and ILI9488 SPI bus TFT

//#include <User_Setups/Setup22_TTGO_T4.h>           // Setup file for ESP32 and TTGO T4 (BTC) ILI9341 SPI bus TFT
//#include <User_Setups/Setup23_TTGO_TM.h>           // Setup file for ESP32 and TTGO TM ST7789 SPI bus TFT
//#include <User_Setups/Setup24_ST7789.h>            // Setup file configured for ST7789 240 x 240
//#include <User_Setups/Setup25_TTGO_T_Display.h>    // Setup file for ESP32 and TTGO T-Display ST7789V SPI bus TFT
//#include <User_Setups/Setup26_TTGO_T_Wristband.h>  // Setup file for ESP32 and TTGO T-Wristband ST7735 SPI bus TFT

//#include <User_Setups/Setup27_RPi_ST7796_ESP32.h>    // ESP32   RPi MHS-4.0 inch Display-B
//#include <User_Setups/Setup28_RPi_ST7796_ESP8266.h>  // ESP8266 RPi MHS-4.0 inch Display-B

//#include <User_Setups/Setup29_ILI9341_STM32.h>          // Setup for Nucleo board
//#include <User_Setups/Setup30_ILI9341_Parallel_STM32.h> // Setup for Nucleo board and parallel display
//#include <User_Setups/Setup31_ST7796_Parallel_STM32.h>  // Setup for Nucleo board and parallel display
//#include <User_Setups/Setup32_ILI9341_STM32F103.h>      // Setup for "Blue Pill"

//#include <User_Setups/Setup43_ST7735.h>            // Setup file configured for my ST7735S 80x160

//#include <User_Setups/Setup135_ST7789.h>           // Setup file for ESP8266 and ST7789 135 x 240 TFT

//#include <User_Setups/SetupX_Template.h>


#endif // USER_SETUP_LOADED



/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//     DON'T TINKER WITH ANY OF THE FOLLOWING LINES, THESE ADD THE TFT DRIVERS     //
//       AND ESP8266 PIN DEFINITONS, THEY ARE HERE FOR BODMER'S CONVENIENCE!       //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////


// Identical looking TFT displays may have a different colour ordering in the 16 bit colour
#define TFT_BGR 0   // Colour order Blue-Green-Red
#define TFT_RGB 1   // Colour order Red-Green-Blue

// Legacy setup support, RPI_DISPLAY_TYPE replaces RPI_DRIVER
#if defined (RPI_DRIVER)
  #if !defined (RPI_DISPLAY_TYPE)
    #define RPI_DISPLAY_TYPE
  #endif
#endif

// Legacy setup support, RPI_ILI9486_DRIVER form is deprecated
// Instead define RPI_DISPLAY_TYPE and also define driver (e.g. ILI9486_DRIVER) 
#if defined (RPI_ILI9486_DRIVER)
  #if !defined (ILI9486_DRIVER)
    #define ILI9486_DRIVER
  #endif
  #if !defined (RPI_DISPLAY_TYPE)
    #define RPI_DISPLAY_TYPE
  #endif
#endif

// Load the right driver definition - do not tinker here !
#if   defined (ILI9341_DRIVER)
     #include <TFT_Drivers/ILI9341_Defines.h>
     #define  TFT_DRIVER 0x9341
#elif defined (ST7735_DRIVER)
     #include <TFT_Drivers/ST7735_Defines.h>
     #define  TFT_DRIVER 0x7735
#elif defined (ILI9163_DRIVER)
     #include <TFT_Drivers/ILI9163_Defines.h>
     #define  TFT_DRIVER 0x9163
#elif defined (S6D02A1_DRIVER)
     #include <TFT_Drivers/S6D02A1_Defines.h>
     #define  TFT_DRIVER 0x6D02
#elif defined (ST7796_DRIVER)
      #include "TFT_Drivers/ST7796_Defines.h"
      #define  TFT_DRIVER 0x7796
#elif defined (ILI9486_DRIVER)
     #include <TFT_Drivers/ILI9486_Defines.h>
     #define  TFT_DRIVER 0x9486
#elif defined (ILI9481_DRIVER)
     #include <TFT_Drivers/ILI9481_Defines.h>
     #define  TFT_DRIVER 0x9481
#elif defined (ILI9488_DRIVER)
     #include <TFT_Drivers/ILI9488_Defines.h>
     #define  TFT_DRIVER 0x9488
#elif defined (HX8357D_DRIVER)
     #include "TFT_Drivers/HX8357D_Defines.h"
     #define  TFT_DRIVER 0x8357
#elif defined (EPD_DRIVER)
     #include "TFT_Drivers/EPD_Defines.h"
     #define  TFT_DRIVER 0xE9D
#elif defined (ST7789_DRIVER)
     #include "TFT_Drivers/ST7789_Defines.h"
     #define  TFT_DRIVER 0x7789
#elif defined (R61581_DRIVER)
     #include "TFT_Drivers/R61581_Defines.h"
     #define  TFT_DRIVER 0x6158
#elif defined (ST7789_2_DRIVER)
     #include "TFT_Drivers/ST7789_2_Defines.h"
     #define  TFT_DRIVER 0x778B
#elif defined (RM68140_DRIVER)
     #include "TFT_Drivers/RM68140_Defines.h"
     #define  TFT_DRIVER 0x6814
                              // <<<<<<<<<<<<<<<<<<<<<<<< ADD NEW DRIVER HERE
                              // XYZZY_init.h and XYZZY_rotation.h must also be added in TFT_eSPI.c
#elif defined (XYZZY_DRIVER)
     #include "TFT_Drivers/XYZZY_Defines.h"
     #define  TFT_DRIVER 0x0000
#else
     #define  TFT_DRIVER 0x0000
#endif


// These are the pins for ESP8266 boards
//      Name   GPIO    NodeMCU      Function
#define PIN_D0  16  // GPIO16       WAKE
#define PIN_D1   5  // GPIO5        User purpose
#define PIN_D2   4  // GPIO4        User purpose
#define PIN_D3   0  // GPIO0        Low on boot means enter FLASH mode
#define PIN_D4   2  // GPIO2        TXD1 (must be high on boot to go to UART0 FLASH mode)
#define PIN_D5  14  // GPIO14       HSCLK
#define PIN_D6  12  // GPIO12       HMISO
#define PIN_D7  13  // GPIO13       HMOSI  RXD2
#define PIN_D8  15  // GPIO15       HCS    TXD0 (must be low on boot to enter UART0 FLASH mode)
#define PIN_D9   3  //              RXD0
#define PIN_D10  1  //              TXD0

#define PIN_MOSI 8  // SD1          FLASH and overlap mode
#define PIN_MISO 7  // SD0
#define PIN_SCLK 6  // CLK
#define PIN_HWCS 0  // D3

#define PIN_D11  9  // SD2
#define PIN_D12 10  // SD4
