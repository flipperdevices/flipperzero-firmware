#include "Display.h"


/*
Big thanks to bodmer for having great TFT and JPEG libraries

https://github.com/bodmer
*/

//PROGMEM lv_obj_t * slider_label;
//PROGMEM lv_obj_t * ta1;
//PROGMEM lv_obj_t * ta2;

Display::Display()
{
}

// Function to prepare the display and the menus
void Display::RunSetup()
{
  run_setup = false;

  // Need to declare new
  display_buffer = new LinkedList<String>();
  
  tft.init();
  tft.setRotation(0); // Portrait

  tft.setCursor(0, 0);
  //tft.setFreeFont(&FreeMonoBold9pt7b);

  // Calibration data
  //uint16_t calData[5] = { 390, 3516, 253, 3520, 7 }; tft.setRotation(1); // Portrait

  #ifdef TFT_SHIELD
    uint16_t calData[5] = { 275, 3494, 361, 3528, 4 }; // tft.setRotation(0); // Portrait with TFT Shield
    Serial.println(F("Using TFT Shield"));
  #else if defined(TFT_DIY)
    uint16_t calData[5] = { 339, 3470, 237, 3438, 2 }; // tft.setRotation(0); // Portrait with DIY TFT
    Serial.println(F("Using TFT DIY"));
  #endif
  tft.setTouch(calData);

  //tft.fillScreen(TFT_BLACK);
  clearScreen();

  Serial.println("SPI_FREQUENCY: " + (String)SPI_FREQUENCY);
  Serial.println("SPI_READ_FREQUENCY: " + (String)SPI_READ_FREQUENCY);
  Serial.println("SPI_TOUCH_FREQUENCY: " + (String)SPI_TOUCH_FREQUENCY);

  
  // Initialize file system
  // This should probably have its own class
  if (!SPIFFS.begin()) {
    Serial.println(F("SPIFFS initialisation failed!"));
    while (1) yield(); // Stay here twiddling thumbs waiting
  }

  //this->initLVGL();


  // Draw the title screen
  //drawJpeg("/marauder3L.jpg", 0 , 0);     // 240 x 320 image

  //showCenterText(version_number, 250);
  //tft.drawCentreString(version_number, 120, 250, 2);

  //digitalWrite(TFT_BL, HIGH);

  //delay(5000);

  #ifdef KIT
    pinMode(KIT_LED_BUILTIN, OUTPUT);
  #endif
}

/* Interrupt driven periodic handler */
/*
void Display::lv_tick_handler()
{
  lv_tick_inc(LVGL_TICK_PERIOD);
}*/

/* Display flushing */
/*
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  extern Display display_obj;
  uint16_t c;

  display_obj.tft.startWrite();
  display_obj.tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1));
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      display_obj.tft.writeColor(c, 1);
      color_p++;
    }
  }
  display_obj.tft.endWrite();
  lv_disp_flush_ready(disp);
}


bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
  extern Display display_obj;
  
  uint16_t touchX, touchY;

  bool touched = display_obj.tft.getTouch(&touchX, &touchY, 600);

  if(!touched)
  {
    return false;
  }

  if(touchX>WIDTH_1 || touchY > HEIGHT_1)
  {
    Serial.println("Y or y outside of expected parameters..");
    Serial.print("y:");
    Serial.print(touchX);
    Serial.print(" x:");
    Serial.print(touchY);
  }
  else
  {

    data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 

    //if(data->state == LV_INDEV_STATE_PR) touchpad_get_xy(&last_x, &last_y);
   
    data->point.x = touchX;
    data->point.y = touchY;

    Serial.print("Data x");
    Serial.println(touchX);
    
    Serial.print("Data y");
    Serial.println(touchY);

  }

  return false;
}*/

void Display::tftDrawGraphObjects(byte x_scale)
{
  //draw the graph objects
  tft.fillRect(11, 5, x_scale+1, 120, TFT_BLACK); // positive start point
  tft.fillRect(11, 121, x_scale+1, 119, TFT_BLACK); // negative start point
  tft.drawFastVLine(10, 5, 230, TFT_WHITE); // y axis
  tft.drawFastHLine(10, HEIGHT_1 - 1, 310, TFT_WHITE); // x axis
  tft.setTextColor(TFT_YELLOW); tft.setTextSize(1); // set parameters for y axis labels
  //tft.setCursor(3, 116); tft.print(midway);  // "0" at center of ya axis
  tft.setCursor(3, 6); tft.print("+"); // "+' at top of y axis
  tft.setCursor(3, 228); tft.print("0"); // "-" at bottom of y axis
}

void Display::tftDrawEapolColorKey()
{
  //Display color key
  tft.setTextSize(1); tft.setTextColor(TFT_WHITE);
  tft.fillRect(14, 0, 15, 8, TFT_CYAN); tft.setCursor(30, 0); tft.print(" - EAPOL"); 
}

void Display::tftDrawColorKey()
{
  //Display color key
  tft.setTextSize(1); tft.setTextColor(TFT_WHITE);
  tft.fillRect(14, 0, 15, 8, TFT_GREEN); tft.setCursor(30, 0); tft.print(" - Beacons"); 
  tft.fillRect(14, 8, 15, 8, TFT_RED); tft.setCursor(30, 8); tft.print(" - Deauths");
  tft.fillRect(14, 16, 15, 8, TFT_BLUE); tft.setCursor(30, 16); tft.print(" - Probes");
}

void Display::tftDrawXScaleButtons(byte x_scale)
{
  tft.drawFastVLine(234, 0, 20, TFT_WHITE);
  tft.setCursor(208, 21); tft.setTextColor(TFT_WHITE); tft.setTextSize(1); tft.print("X Scale:"); tft.print(x_scale);

  key[0].initButton(&tft, // x - box
                        220,
                        10, // x, y, w, h, outline, fill, text
                        20,
                        20,
                        TFT_BLACK, // Outline
                        TFT_CYAN, // Fill
                        TFT_BLACK, // Text
                        "-",
                        2);
  key[1].initButton(&tft, // x + box
                        249,
                        10, // x, y, w, h, outline, fill, text
                        20,
                        20,
                        TFT_BLACK, // Outline
                        TFT_CYAN, // Fill
                        TFT_BLACK, // Text
                        "+",
                        2);

  key[0].setLabelDatum(1, 5, MC_DATUM);
  key[1].setLabelDatum(1, 5, MC_DATUM);

  key[0].drawButton();
  key[1].drawButton();
}

void Display::tftDrawYScaleButtons(byte y_scale)
{
  tft.drawFastVLine(290, 0, 20, TFT_WHITE);
  tft.setCursor(265, 21); tft.setTextColor(TFT_WHITE); tft.setTextSize(1); tft.print("Y Scale:"); tft.print(y_scale);

  key[2].initButton(&tft, // y - box
                        276,
                        10, // x, y, w, h, outline, fill, text
                        20,
                        20,
                        TFT_BLACK, // Outline
                        TFT_MAGENTA, // Fill
                        TFT_BLACK, // Text
                        "-",
                        2);
  key[3].initButton(&tft, // y + box
                        305,
                        10, // x, y, w, h, outline, fill, text
                        20,
                        20,
                        TFT_BLACK, // Outline
                        TFT_MAGENTA, // Fill
                        TFT_BLACK, // Text
                        "+",
                        2);

  key[2].setLabelDatum(1, 5, MC_DATUM);
  key[3].setLabelDatum(1, 5, MC_DATUM);

  key[2].drawButton();
  key[3].drawButton();
}

void Display::tftDrawChannelScaleButtons(int set_channel)
{
  tft.drawFastVLine(178, 0, 20, TFT_WHITE);
  tft.setCursor(145, 21); tft.setTextColor(TFT_WHITE); tft.setTextSize(1); tft.print("Channel:"); tft.print(set_channel);

  key[4].initButton(&tft, // channel - box
                        164,
                        10, // x, y, w, h, outline, fill, text
                        20,
                        20,
                        TFT_BLACK, // Outline
                        TFT_BLUE, // Fill
                        TFT_BLACK, // Text
                        "-",
                        2);
  key[5].initButton(&tft, // channel + box
                        193,
                        10, // x, y, w, h, outline, fill, text
                        20,
                        20,
                        TFT_BLACK, // Outline
                        TFT_BLUE, // Fill
                        TFT_BLACK, // Text
                        "+",
                        2);

  key[4].setLabelDatum(1, 5, MC_DATUM);
  key[5].setLabelDatum(1, 5, MC_DATUM);

  key[4].drawButton();
  key[5].drawButton();
}

void Display::tftDrawExitScaleButtons()
{
  //tft.drawFastVLine(178, 0, 20, TFT_WHITE);
  //tft.setCursor(145, 21); tft.setTextColor(TFT_WHITE); tft.setTextSize(1); tft.print("Channel:"); tft.print(set_channel);

  key[6].initButton(&tft, // Exit box
                        137,
                        10, // x, y, w, h, outline, fill, text
                        20,
                        20,
                        TFT_ORANGE, // Outline
                        TFT_RED, // Fill
                        TFT_BLACK, // Text
                        "X",
                        2);

  key[6].setLabelDatum(1, 5, MC_DATUM);

  key[6].drawButton();
}

void Display::twoPartDisplay(String center_text)
{
  tft.setTextColor(TFT_BLACK, TFT_YELLOW);
  tft.fillRect(0,16,HEIGHT_1,144, TFT_YELLOW);
  //tft.drawCentreString(center_text,120,82,1);
  tft.setTextWrap(true);
  tft.setFreeFont(NULL);
  //showCenterText(center_text, 82);
  //tft.drawCentreString(center_text,120,82,1);
  tft.setCursor(0, 82);
  tft.println(center_text);
  tft.setFreeFont(MENU_FONT);
  tft.setTextWrap(false);
}

void Display::touchToExit()
{
  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  tft.fillRect(0,32,HEIGHT_1,16, TFT_LIGHTGREY);
  tft.drawCentreString("Touch screen to exit",120,32,2);
}


// Function to just draw the screen black
void Display::clearScreen()
{
  Serial.println(F("clearScreen()"));
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
}

void Display::displayBuffer(bool do_clear)
{
  if (this->display_buffer->size() > 0)
  {
    delay(1);
    
    while (display_buffer->size() > 0)
    {
      xPos = 0;
      if ((display_buffer->size() > 0) && (!loading))
      {
        printing = true;
        delay(print_delay_1);
        yDraw = scroll_line(TFT_RED);
        tft.setCursor(xPos, yDraw);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.print(display_buffer->shift());
        printing = false;
        delay(print_delay_2);
      }
      if (!tteBar)
        blank[(18+(yStart - TOP_FIXED_AREA) / TEXT_HEIGHT)%19] = xPos;
      else
        blank[(18+(yStart - TOP_FIXED_AREA_2) / TEXT_HEIGHT)%19] = xPos;
    }
  }
}

void Display::showCenterText(String text, int y)
{
  tft.setCursor((SCREEN_WIDTH - (text.length() * 6)) / 2, y);
  tft.println(text);
}


void Display::initScrollValues(bool tte)
{
  Serial.println(F("initScrollValues()"));
  yDraw = YMAX - BOT_FIXED_AREA - TEXT_HEIGHT;

  xPos = 0;

  if (!tte)
  {
    yStart = TOP_FIXED_AREA;

    yArea = YMAX - TOP_FIXED_AREA - BOT_FIXED_AREA;
  }
  else
  {
    yStart = TOP_FIXED_AREA_2;

    yArea = YMAX - TOP_FIXED_AREA_2 - BOT_FIXED_AREA;
  }

  for(uint8_t i = 0; i < 18; i++) blank[i] = 0;
}



// Function to execute hardware scroll for TFT screen
int Display::scroll_line(uint32_t color) {
  //Serial.println("scroll_line()");
  int yTemp = yStart; // Store the old yStart, this is where we draw the next line
  // Use the record of line lengths to optimise the rectangle size we need to erase the top line

  // Check if we have the "touch to exit bar"
  if (!tteBar)
  {
    tft.fillRect(0,yStart,blank[(yStart-TOP_FIXED_AREA)/TEXT_HEIGHT],TEXT_HEIGHT, color);
  
    // Change the top of the scroll area
    yStart+=TEXT_HEIGHT;
    // The value must wrap around as the screen memory is a circular buffer
    if (yStart >= YMAX - BOT_FIXED_AREA) yStart = TOP_FIXED_AREA + (yStart - YMAX + BOT_FIXED_AREA);
  }
  else
  {
    tft.fillRect(0,yStart,blank[(yStart-TOP_FIXED_AREA_2)/TEXT_HEIGHT],TEXT_HEIGHT, color);
  
    // Change the top of the scroll area
    yStart+=TEXT_HEIGHT;
    // The value must wrap around as the screen memory is a circular buffer
    if (yStart >= YMAX - BOT_FIXED_AREA) yStart = TOP_FIXED_AREA_2 + (yStart - YMAX + BOT_FIXED_AREA);
  }
  // Now we can scroll the display
  scrollAddress(yStart);
  return  yTemp;
}


// Function to setup hardware scroll for TFT screen
void Display::setupScrollArea(uint16_t tfa, uint16_t bfa) {
  Serial.println(F("setupScrollArea()"));
  Serial.println("   tfa: " + (String)tfa);
  Serial.println("   bfa: " + (String)bfa);
  Serial.println("yStart: " + (String)this->yStart);
  tft.writecommand(ILI9341_VSCRDEF); // Vertical scroll definition
  tft.writedata(tfa >> 8);           // Top Fixed Area line count
  tft.writedata(tfa);
  tft.writedata((YMAX-tfa-bfa)>>8);  // Vertical Scrolling Area line count
  tft.writedata(YMAX-tfa-bfa);
  tft.writedata(bfa >> 8);           // Bottom Fixed Area line count
  tft.writedata(bfa);
}


void Display::scrollAddress(uint16_t vsp) {
  tft.writecommand(ILI9341_VSCRSADD); // Vertical scrolling pointer
  tft.writedata(vsp>>8);
  tft.writedata(vsp);
}




// JPEG_functions
void Display::drawJpeg(const char *filename, int xpos, int ypos) {

  // Open the named file (the Jpeg decoder library will close it after rendering image)
  fs::File jpegFile = SPIFFS.open( filename, "r");    // File handle reference for SPIFFS

  //ESP32 always seems to return 1 for jpegFile so this null trap does not work
  if ( !jpegFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  // Use one of the three following methods to initialise the decoder,
  // the filename can be a String or character array type:

  boolean decoded = JpegDec.decodeFsFile(filename);  // or pass the filename (leading / distinguishes SPIFFS files)

  if (decoded) {
    // print information about the image to the serial port
    jpegInfo();

    // render the image onto the screen at given coordinates
    jpegRender(xpos, ypos);
  }
  else {
    Serial.println(F("Jpeg file format not supported!"));
  }
}

void Display::setupDraw() {
  this->tft.drawLine(0, 0, 10, 0, TFT_MAGENTA);
  this->tft.drawLine(0, 0, 0, 10, TFT_GREEN);
  this->tft.drawLine(0, 0, 0, 0, TFT_CYAN);
}

uint16_t xlast;
uint16_t ylast;
uint32_t AH;
void Display::drawStylus()
{
  uint16_t x = 0, y = 0; // To store the touch coordinates

  // Pressed will be set true is there is a valid touch on the screen
  boolean pressed = tft.getTouch(&x, &y);

  if ((x <= 10) && (y <= 10) && (pressed)) {
    Serial.println(F("Exit draw function"));
    this->draw_tft = false;
    this->exit_draw = true;
    return;
  }

  // Draw a white spot at the detected coordinates
  if (pressed) {
    //    tft.fillCircle(x, y, 2, TFT_WHITE);
    if ( xlast > 0 && ylast > 0 ) {
      uint16_t the_color = TFT_WHITE;
      uint16_t wd = 1;
      int xlast2;
      int ylast2;
      int x2;
      int y2;
      int n;
      int n2 = -wd;
      xlast2 = xlast - wd;
      x2 = x - wd;
      for (n = -wd; n <= wd; n++) {
        ylast2 = ylast + n;
        y2 = y + n;
        tft.drawLine(xlast2, ylast2, x2, y2, the_color);
      }
      for (n2 = -wd; n2 <= wd; n2++) {
        xlast2 = xlast + n2;
        x2 = x + n2;
        tft.drawLine(xlast2, ylast2, x2, y2, the_color);
      }
      for (n = wd; n >= -wd; n--) {
        ylast2 = ylast + n;
        y2 = y + n;
        tft.drawLine(xlast2, ylast2, x2, y2, the_color);
      }
      for (n2 = wd; n2 >= -wd; n2--) {
        xlast2 = xlast + n2;
        x2 = x + n2;
        tft.drawLine(xlast2, ylast2, x2, y2, the_color);
      }
//      tft.drawLine(xlast, ylast, x, y, TFT_WHITE);
    }
    xlast = x;
    ylast = y;
    AH = 0;
    //Serial.print("x,y = ");
    //Serial.print(x);
    //Serial.print(",");
    //Serial.println(y);
  } else if ( AH < 5 ) {
    AH++;
  } else if ( AH == 5 ) {
    xlast = 0;
    ylast = 0;
  }
}

//====================================================================================
//   Decode and render the Jpeg image onto the TFT screen
//====================================================================================
void Display::jpegRender(int xpos, int ypos) {

  // retrieve infomration about the image
  uint16_t  *pImg;
  int16_t mcu_w = JpegDec.MCUWidth;
  int16_t mcu_h = JpegDec.MCUHeight;
  int32_t max_x = JpegDec.width;
  int32_t max_y = JpegDec.height;

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  int32_t min_w = minimum(mcu_w, max_x % mcu_w);
  int32_t min_h = minimum(mcu_h, max_y % mcu_h);

  // save the current image block size
  int32_t win_w = mcu_w;
  int32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while ( JpegDec.readSwappedBytes()) { // Swapped byte order read

    // save a pointer to the image block
    pImg = JpegDec.pImage;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;  // Calculate coordinates of top left corner of current MCU
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      for (int h = 1; h < win_h-1; h++)
      {
        memcpy(pImg + h * win_w, pImg + (h + 1) * mcu_w, win_w << 1);
      }
    }

    // draw image MCU block only if it will fit on the screen
    if ( mcu_x < tft.width() && mcu_y < tft.height())
    {
      // Now push the image block to the screen
      tft.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
    }

    else if ( ( mcu_y + win_h) >= tft.height()) JpegDec.abort();

  }

  // calculate how long it took to draw the image
  drawTime = millis() - drawTime; // Calculate the time it took
}

//====================================================================================
//   Print information decoded from the Jpeg image
//====================================================================================
void Display::jpegInfo() {
/*
  Serial.println("===============");
  Serial.println("JPEG image info");
  Serial.println("===============");
  Serial.print  ("Width      :"); Serial.println(JpegDec.width);
  Serial.print  ("Height     :"); Serial.println(JpegDec.height);
  Serial.print  ("Components :"); Serial.println(JpegDec.comps);
  Serial.print  ("MCU / row  :"); Serial.println(JpegDec.MCUSPerRow);
  Serial.print  ("MCU / col  :"); Serial.println(JpegDec.MCUSPerCol);
  Serial.print  ("Scan type  :"); Serial.println(JpegDec.scanType);
  Serial.print  ("MCU width  :"); Serial.println(JpegDec.MCUWidth);
  Serial.print  ("MCU height :"); Serial.println(JpegDec.MCUHeight);
  Serial.println("===============");
  Serial.println("");
  */
}

//====================================================================================
//   Open a Jpeg file and send it to the Serial port in a C array compatible format
//====================================================================================
void createArray(const char *filename) {

  // Open the named file
  fs::File jpgFile = SPIFFS.open( filename, "r");    // File handle reference for SPIFFS
  //  File jpgFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library

  if ( !jpgFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  uint8_t data;
  byte line_len = 0;
  Serial.println("");
  Serial.println(F("// Generated by a JPEGDecoder library example sketch:"));
  Serial.println(F("// https://github.com/Bodmer/JPEGDecoder"));
  Serial.println("");
  Serial.println(F("#if defined(__AVR__)"));
  Serial.println(F("  #include <avr/pgmspace.h>"));
  Serial.println(F("#endif"));
  Serial.println("");
  Serial.print  (F("const uint8_t "));
  while (*filename != '.') Serial.print(*filename++);
  Serial.println(F("[] PROGMEM = {")); // PROGMEM added for AVR processors, it is ignored by Due

  while ( jpgFile.available()) {

    data = jpgFile.read();
    Serial.print("0x"); if (abs(data) < 16) Serial.print("0");
    Serial.print(data, HEX); Serial.print(",");// Add value and comma
    line_len++;
    if ( line_len >= 32) {
      line_len = 0;
      Serial.println();
    }

  }

  Serial.println("};\r\n");
  jpgFile.close();
}

// End JPEG_functions

// SPIFFS_functions

#ifdef ESP8266
void Display::listFiles(void) {
  Serial.println();
  Serial.println(F("SPIFFS files found:"));

  fs::Dir dir = SPIFFS.openDir("/"); // Root directory
  String  line = "=====================================";

  Serial.println(line);
  Serial.println(F("  File name               Size"));
  Serial.println(line);

  while (dir.next()) {
    String fileName = dir.fileName();
    Serial.print(fileName);
    int spaces = 21 - fileName.length(); // Tabulate nicely
    while (spaces--) Serial.print(" ");

    fs::File f = dir.openFile("r");
    String fileSize = (String) f.size();
    spaces = 10 - fileSize.length(); // Tabulate nicely
    while (spaces--) Serial.print(" ");
    Serial.println(fileSize + " bytes");
  }

  Serial.println(line);
  Serial.println();
  delay(1000);
}
#endif

//====================================================================================

#ifdef ESP32

void Display::listFiles(void) {
  listDir(SPIFFS, "/", 0);
}

void Display::listDir(fs::FS &fs, const char * dirname, uint8_t levels) {

  Serial.println();
  Serial.println(F("SPIFFS files found:"));

  Serial.printf("Listing directory: %s\n", "/");
  String  line = "=====================================";

  Serial.println(line);
  Serial.println(F("  File name               Size"));
  Serial.println(line);

  fs::File root = fs.open(dirname);
  if (!root) {
    Serial.println(F("Failed to open directory"));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F("Not a directory"));
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      int spaces = 20 - fileName.length(); // Tabulate nicely
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 10 - fileSize.length(); // Tabulate nicely
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }

  Serial.println(line);
  Serial.println();
  delay(1000);
}
#endif


void Display::updateBanner(String msg)
{
  this->img.deleteSprite();
  
  this->img.setColorDepth(8);

  this->img.createSprite(SCREEN_WIDTH, TEXT_HEIGHT);

  this->buildBanner(msg, current_banner_pos);

  this->img.pushSprite(0, STATUS_BAR_WIDTH);

  current_banner_pos--;

  if (current_banner_pos <= 0)
    current_banner_pos = SCREEN_WIDTH + 2;
}


void Display::buildBanner(String msg, int xpos)
{
  int h = TEXT_HEIGHT;

  // We could just use fillSprite(color) but lets be a bit more creative...

  // Fill with rainbow stripes
  //while (h--) img.drawFastHLine(0, h, SCREEN_WIDTH, 255);

  // Draw some graphics, the text will apear to scroll over these
  //img.fillRect  (SCREEN_WIDTH / 2 - 20, TEXT_HEIGHT / 2 - 10, 40, 20, TFT_YELLOW);
  //img.fillCircle(SCREEN_WIDTH / 2, TEXT_HEIGHT / 2, 10, TFT_ORANGE);

  // Now print text on top of the graphics
  img.setTextSize(2);           // Font size scaling is x1
  img.setTextFont(0);           // Font 4 selected
  img.setTextColor(TFT_WHITE);  // Black text, no background colour
  img.setTextWrap(false);       // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  img.setCursor(xpos, 2);  // Print text at xpos
  img.print(msg);

  img.setCursor(xpos - SCREEN_WIDTH, 2); // Print text at xpos - sprite width
  img.print(msg);
}

/*
void Display::initLVGL() {
  tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);
  
  lv_init();

  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = WIDTH_1;
  disp_drv.ver_res = HEIGHT_1;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);             
  indev_drv.type = LV_INDEV_TYPE_POINTER;    
  indev_drv.read_cb = my_touchpad_read;      
  lv_indev_drv_register(&indev_drv);         
}


void Display::deinitLVGL() {
  Serial.println(F("Deinit LVGL"));
  //lv_deinit();
}



void Display::joinWiFiGFX(){

  // Create one text area
  ta1 = lv_textarea_create(lv_scr_act(), NULL);
  lv_textarea_set_one_line(ta1, true);
  lv_obj_set_width(ta1, LV_HOR_RES / 2 - 20);
  lv_obj_set_pos(ta1, 5, 20);
  //lv_ta_set_cursor_type(ta, LV_CURSOR_BLOCK);
  lv_textarea_set_text(ta1, "");
  lv_obj_set_event_cb(ta1, ta_event_cb);

  // Create first label
  lv_obj_t * ssid_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(ssid_label, "SSID:");
  lv_obj_align(ssid_label, ta1, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

  // Create second text area
  ta2 = lv_textarea_create(lv_scr_act(), ta1);
  //lv_textarea_set_pwd_mode(ta2, true); // This shit makes it so backspace does not work
  //lv_textarea_set_pwd_show_time(ta2, 1000);
  lv_textarea_set_cursor_hidden(ta2, true);
  lv_obj_align(ta2, NULL, LV_ALIGN_IN_TOP_RIGHT, -5, 20);

  // Create second label
  lv_obj_t * pw_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(pw_label, "Password:");
  lv_obj_align(pw_label, ta2, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

  // Create a keyboard and apply the styles
  kb = lv_keyboard_create(lv_scr_act(), NULL);
  lv_obj_set_size(kb, LV_HOR_RES, LV_VER_RES / 2);
  lv_obj_set_event_cb(kb, keyboard_event_cb);

  // Focus it on one of the text areas to start
  lv_keyboard_set_textarea(kb, ta1);
  lv_keyboard_set_cursor_manage(kb, true);
  
}


void keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event){
  extern Display display_obj;
  lv_keyboard_def_event_cb(kb, event);
  if(event == LV_EVENT_APPLY){
    printf("LV_EVENT_APPLY\n");
    //String ta1_text = lv_textarea_get_text(lv_keyboard_get_textarea(kb));
    String ta1_text = lv_textarea_get_text(ta1);
    String ta2_text = lv_textarea_get_text(ta2);
    Serial.println(ta1_text);
    Serial.println(ta2_text);
    //joinWiFi(ta1_text, ta2_text);
  }else if(event == LV_EVENT_CANCEL){
    printf("LV_EVENT_CANCEL\n");
    //lv_textarea_set_text(lv_keyboard_get_textarea(kb), "");
    display_obj.deinitLVGL();
    display_obj.exit_draw = true; // set everything back to normal
  }
}


void ta_event_cb(lv_obj_t * ta, lv_event_t event)
{
  if(event == LV_EVENT_CLICKED) {
    if(kb != NULL)
      lv_keyboard_set_textarea(kb, ta);
  }

  //else if(event == LV_EVENT_INSERT) {
  //  const char * str = lv_event_get_data();
  //  if(str[0] == '\n') {
  //    printf("Ready\n");
  //  }
  //}
}*/

void Display::main(uint8_t scan_mode)
{  
  if ((scan_mode == LV_JOIN_WIFI) ||
      (scan_mode == LV_ADD_SSID))
    lv_task_handler();
  return;
}
// End SPIFFS_functions
