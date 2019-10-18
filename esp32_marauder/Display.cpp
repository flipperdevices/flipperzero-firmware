#include "Display.h"

/*
Big thanks to bodmer for having great TFT and JPEG libraries

https://github.com/bodmer
*/

Display::Display()
{
}

void Display::main()
{
  return;
}

// Function to prepare the display and the menus
void Display::RunSetup()
{
  run_setup = false;

  // Need to declare new
  display_buffer = new SimpleList<String>();
  
  tft.init();
  tft.setRotation(0); // Portrait

  tft.setCursor(0, 0);
  //tft.setFreeFont(&FreeMonoBold9pt7b);

  // Calibration data
  //uint16_t calData[5] = { 390, 3516, 253, 3520, 7 }; tft.setRotation(1); // Portrait
  uint16_t calData[5] = { 275, 3494, 361, 3528, 4 }; // tft.setRotation(0); // Portrait
  tft.setTouch(calData);

  //tft.fillScreen(TFT_BLACK);
  clearScreen();

  
  // Initialize file system
  // This should probably have its own class
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }


  // Draw the title screen
  drawJpeg("/marauder3L.jpg", 0 , 0);     // 240 x 320 image

  //showCenterText(version_number, 250);
  tft.drawCentreString(version_number, 120, 250, 2);

  digitalWrite(TFT_BL, HIGH);

  delay(5000);
}

void Display::twoPartDisplay(String center_text)
{
  tft.setTextColor(TFT_BLACK, TFT_YELLOW);
  tft.fillRect(0,16,240,144, TFT_YELLOW);
  tft.drawCentreString(center_text,120,82,1);
}

void Display::touchToExit()
{
  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  tft.fillRect(0,16,240,16, TFT_LIGHTGREY);
  tft.drawCentreString("Touch screen to exit",120,16,2);
}


// Function to just draw the screen black
void Display::clearScreen()
{
  Serial.println("clearScreen()");
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
        tft.print(display_buffer->shift());
        printing = false;
        delay(print_delay_2);
      }
      
      blank[(18+(yStart - TOP_FIXED_AREA) / TEXT_HEIGHT)%19] = xPos;
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
  Serial.println("initScrollValues()");
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

  for(int i = 0; i < 18; i++) blank[i] = 0;
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
  Serial.println("setupScrollAread()");
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
    Serial.println("Jpeg file format not supported!");
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
  Serial.println("// Generated by a JPEGDecoder library example sketch:");
  Serial.println("// https://github.com/Bodmer/JPEGDecoder");
  Serial.println("");
  Serial.println("#if defined(__AVR__)");
  Serial.println("  #include <avr/pgmspace.h>");
  Serial.println("#endif");
  Serial.println("");
  Serial.print  ("const uint8_t ");
  while (*filename != '.') Serial.print(*filename++);
  Serial.println("[] PROGMEM = {"); // PROGMEM added for AVR processors, it is ignored by Due

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
  Serial.println("SPIFFS files found:");

  fs::Dir dir = SPIFFS.openDir("/"); // Root directory
  String  line = "=====================================";

  Serial.println(line);
  Serial.println("  File name               Size");
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
  Serial.println("SPIFFS files found:");

  Serial.printf("Listing directory: %s\n", "/");
  String  line = "=====================================";

  Serial.println(line);
  Serial.println("  File name               Size");
  Serial.println(line);

  fs::File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
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

// End SPIFFS_functions
