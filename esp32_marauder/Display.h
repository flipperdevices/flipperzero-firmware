#ifndef Display_h
#define Display_h


#include <FS.h>
#include <functional>
#include <JPEGDecoder.h>
#include <SimpleList.h>
#include <SPI.h>
#include "SPIFFS.h"
#include <TFT_eSPI.h>
//#include "Free_Fonts.h"


#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define TEXT_HEIGHT 16 // Height of text to be printed and scrolled
#define BOT_FIXED_AREA 0 // Number of lines in bottom fixed area (lines counted from bottom of screen)
#define TOP_FIXED_AREA 16 // Number of lines in top fixed area (lines counted from top of screen)
#define YMAX 320 // Bottom of screen area
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))
//#define MENU_FONT NULL
#define MENU_FONT &FreeMono9pt7b // Winner
//#define MENU_FONT &FreeMonoBold9pt7b
//#define MENU_FONT &FreeSans9pt7b
//#define MENU_FONT &FreeSansBold9pt7b


class Display
{
  private:
    
    bool run_setup = true;
    
    // For the byte we read from the serial port
    byte data = 0;
    
    // A few test variables used during debugging
    boolean change_colour = 1;
    boolean selected = 1;

    //void addNodes(Menu* menu, String name, Menu* child, std::function<void()> callable);
    //void changeMenu(Menu* menu);
    //void showMenuList(Menu* menu, int layer);

  public:
    Display();
    TFT_eSPI tft = TFT_eSPI();
    String version_number = "v0.1";

    bool printing = false;
    bool loading = false;
    bool tteBar = false;

    int TOP_FIXED_AREA_2 = 32;
    int print_delay_1, print_delay_2 = 10;

    //Menu* current_menu;
    
    //Menu mainMenu;
    //Menu wifiMenu;
    //Menu bluetoothMenu;

    SimpleList<String>* display_buffer;

    // The initial y coordinate of the top of the bottom text line
    uint16_t yDraw = YMAX - BOT_FIXED_AREA - TEXT_HEIGHT;

    // Keep track of the drawing x coordinate
    uint16_t xPos = 0;

    // The initial y coordinate of the top of the scrolling area
    uint16_t yStart = TOP_FIXED_AREA;
    // yArea must be a integral multiple of TEXT_HEIGHT
    uint16_t yArea = YMAX - TOP_FIXED_AREA - BOT_FIXED_AREA;

    // We have to blank the top line each time the display is scrolled, but this takes up to 13 milliseconds
    // for a full width line, meanwhile the serial buffer may be filling... and overflowing
    // We can speed up scrolling of short text lines by just blanking the character we drew
    int blank[19]; // We keep all the strings pixel lengths to optimise the speed of the top line blanking
    
    void clearScreen();
    void displayBuffer(bool do_clear = false);
    void drawJpeg(const char *filename, int xpos, int ypos);
    void getTouchWhileFunction(bool pressed);
    void initScrollValues(bool tte = false);
    void jpegInfo();
    void jpegRender(int xpos, int ypos);
    void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
    void listFiles();
    void main();
    void RunSetup();
    void scrollAddress(uint16_t vsp);
    int scroll_line(uint32_t color);
    void setupScrollArea(uint16_t tfa, uint16_t bfa);
    void showCenterText(String text, int y);
    void touchToExit();
    void twoPartDisplay(String center_text);
};
#endif
