#if 0
/*
  Display "flicker free" scrolling text and updating number

  Example for library:
  https://github.com/Bodmer/TFT_eSPI

  The sketch has been tested on a 320x240 ILI9341 based TFT, it
  could be adapted for other screen sizes.

  A Sprite is notionally an invisible graphics screen that is
  kept in the processors RAM. Graphics can be drawn into the
  Sprite just as it can be drawn directly to the screen. Once
  the Sprite is completed it can be plotted onto the screen in
  any position. If there is sufficient RAM then the Sprite can
  be the same size as the screen and used as a frame buffer.

  The Sprite occupies (2 * width * height) bytes.

  On a ESP8266 Sprite sizes up to 128 x 160 can be accommodated,
  this size requires 128*160*2 bytes (40kBytes) of RAM, this must be
  available or the processor will crash. You need to make the sprite
  small enough to fit, with RAM spare for any "local variables" that
  may be needed by your sketch and libraries.

  Created by Bodmer 15/11/17

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  #########################################################################
*/

// Size of sprite image for the scrolling text, this requires ~14 Kbytes of RAM
#define IWIDTH 240
#define IHEIGHT 30

// Pause in milliseconds to set scroll speed
#define WAIT 0

#include <TFT_eSPI.h> // Include the graphics library (this includes the sprite functions)

void build_banner(String msg, int xpos);
void numberBox(int num, int x, int y);
unsigned int rainbow(byte value);

TFT_eSPI    tft = TFT_eSPI();         // Create object "tft"

TFT_eSprite img = TFT_eSprite(&tft);  // Create Sprite object "img" with pointer to "tft" object
//                                    // the pointer is used by pushSprite() to push it onto the TFT

// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------
void setup(void) {
  tft.init();
  tft.setRotation(0);

  tft.fillScreen(TFT_BLUE);
}

// -------------------------------------------------------------------------
// Main loop
// -------------------------------------------------------------------------
void loop() {

  while (1)
  {
    // Create the sprite and clear background to black
    img.createSprite(IWIDTH, IHEIGHT);
    //img.fillSprite(TFT_BLACK); // Optional here as we fill the sprite later anyway

    for (int pos = IWIDTH; pos > 0; pos--)
    {
      build_banner("Hello World", pos);
      img.pushSprite(0, 0);

      build_banner("TFT_eSPI sprite" , pos);
      img.pushSprite(0, 50);

      delay(WAIT);
    }

    // Delete sprite to free up the memory
    img.deleteSprite();

    // Create a sprite of a different size
    numberBox(random(100), 60, 100);

  }
}

// #########################################################################
// Build the scrolling sprite image from scratch, draw text at x = xpos
// #########################################################################

void build_banner(String msg, int xpos)
{
  int h = IHEIGHT;

  // We could just use fillSprite(color) but lets be a bit more creative...

  // Fill with rainbow stripes
  while (h--) img.drawFastHLine(0, h, IWIDTH, rainbow(h * 4));

  // Draw some graphics, the text will apear to scroll over these
  img.fillRect  (IWIDTH / 2 - 20, IHEIGHT / 2 - 10, 40, 20, TFT_YELLOW);
  img.fillCircle(IWIDTH / 2, IHEIGHT / 2, 10, TFT_ORANGE);

  // Now print text on top of the graphics
  img.setTextSize(1);           // Font size scaling is x1
  img.setTextFont(4);           // Font 4 selected
  img.setTextColor(TFT_BLACK);  // Black text, no background colour
  img.setTextWrap(false);       // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  img.setCursor(xpos, 2);  // Print text at xpos
  img.print(msg);

  img.setCursor(xpos - IWIDTH, 2); // Print text at xpos - sprite width
  img.print(msg);
}

// #########################################################################
// Create sprite, plot graphics in it, plot to screen, then delete sprite
// #########################################################################
void numberBox(int num, int x, int y)
{
  // Create a sprite 80 pixels wide, 50 high (8kbytes of RAM needed)
  img.createSprite(80, 50);

  // Fill it with black
  img.fillSprite(TFT_BLACK);

  // Draw a backgorund of 2 filled triangles
  img.fillTriangle(  0, 0,  0, 49, 40, 25, TFT_RED);
  img.fillTriangle( 79, 0, 79, 49, 40, 25, TFT_DARKGREEN);

  // Set the font parameters
  img.setTextSize(1);           // Font size scaling is x1
  img.setFreeFont(&FreeSerifBoldItalic24pt7b);  // Select free font
  img.setTextColor(TFT_WHITE);  // White text, no background colour

  // Set text coordinate datum to middle centre
  img.setTextDatum(MC_DATUM);

  // Draw the number in middle of 80 x 50 sprite
  img.drawNumber(num, 40, 25);

  // Push sprite to TFT screen CGRAM at coordinate x,y (top left corner)
  img.pushSprite(x, y);

  // Delete sprite to free up the RAM
  img.deleteSprite();
}


// #########################################################################
// Return a 16 bit rainbow colour
// #########################################################################
unsigned int rainbow(byte value)
{
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = red through to 127 = blue

  byte red   = 0; // Red is the top 5 bits of a 16 bit colour value
  byte green = 0;// Green is the middle 6 bits
  byte blue  = 0; // Blue is the bottom 5 bits

  byte sector = value >> 5;
  byte amplit = value & 0x1F;

  switch (sector)
  {
    case 0:
      red   = 0x1F;
      green = amplit;
      blue  = 0;
      break;
    case 1:
      red   = 0x1F - amplit;
      green = 0x1F;
      blue  = 0;
      break;
    case 2:
      red   = 0;
      green = 0x1F;
      blue  = amplit;
      break;
    case 3:
      red   = 0;
      green = 0x1F - amplit;
      blue  = 0x1F;
      break;
  }

  return red << 11 | green << 6 | blue;
}

#elif 1
#include <FS.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".
#define CALIBRATION_FILE "/TouchCalData3"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

// Keypad start position, key sizes and spacing
#define KEY_X 40 // Centre of key
#define KEY_Y 96
#define KEY_W 62 // Width and height
#define KEY_H 30
#define KEY_SPACING_X 18 // X and Y gap
#define KEY_SPACING_Y 20
#define KEY_TEXTSIZE 1                     // Font size multiplier

// Using two fonts since numbers are nice when bold
#define LABEL1_FONT &FreeSansOblique12pt7b // Key label font 1
#define LABEL2_FONT &FreeSansBold12pt7b    // Key label font 2

// Numeric display box size and location
#define DISP_X 1
#define DISP_Y 10
#define DISP_W 238
#define DISP_H 50
#define DISP_TSIZE 3
#define DISP_TCOLOR TFT_CYAN

char ssid[] = "henryTestAp";          // change xxxx to Wi-Fi SSID
char password[] = ""; // change xxxx to Wi-Fi password
char server[] = "api.open-meteo.com";

int count = 99;
WiFiClient client;
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
DynamicJsonDocument jsonDoc(9216); // JSON formatted data

unsigned int chkTime = 0;
uint16_t x, y;
int screenFlag = 0;
int touchFlag = 0;

void touch_calibrate();
void checkTime();
void getWeather();
void connect();
void firstScreen();
void secondScreen();

void setup()
{
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0);
  touch_calibrate();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting...");
  }

  Serial.println("Connected");

  tft.fillScreen(TFT_BLACK);

  // tft.fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
  tft.drawRect(1, 1, tft.width() - 1, tft.height() - 1, TFT_WHITE);
}

void loop(void)
{
    checkTime();
    if(tft.getTouch(&x, &y) > 0 && touchFlag == 0) // if screen pressed
    {
      touchFlag = 1;
      count = 99;
      screenFlag = 1 - screenFlag;
    }
}

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  if (!SPIFFS.begin())
  {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  if (SPIFFS.exists(CALIBRATION_FILE))
  {
    Serial.println("exists");
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      Serial.println("open file");
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f)
      {
        Serial.println("read file");
        if (f.readBytes((char *)calData, 14) == 14)
        {
          Serial.println("read 14");
          calDataOK = 1;
        }
        f.close();
      }
    }
  }

  if (calDataOK & !REPEAT_CAL)
  {
    // calibration data valid
    tft.setTouch(calData);
  }
  else
  {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated!");
    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL)
    {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);

    // Store the data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f)
    {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }

  }
}

void getWeather()
{
  connect();
  if(screenFlag == 0)
    firstScreen();
  else
    secondScreen();
  
  touchFlag = 0;
}

String output;
void connect() // function for Wi-Fi connection
{
  client.connect(server, 80);                                                                        // connect to server on port 80
  client.println("GET /v1/forecast?latitude=40.39&longitude=-80.16&current_weather=true&hourly=temperature_2m HTTP/1.1"); // send HTTP request
  client.println("Host: api.open-meteo.com");
  client.println("User-Agent: ESP8266/0.1");
  client.println("Connection: close");
  client.println();
  client.find("\r\n\r\n");

  if (client.println() == 0)
  {
    Serial.println("HTTP request failed");
    return;
  }

  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  Serial.println(status);

  // essential instruction
  DeserializationError error = deserializeJson(jsonDoc, client);
  serializeJson(jsonDoc, output);
  Serial.println(output);
}

void checkTime()
{
  if(count > 4)
  {
    getWeather();
    count = 0;
  }
  else if((millis() - chkTime) > 60000)
  {
    chkTime = millis();
    count ++;
  }

}

void firstScreen()
{

}

void secondScreen()
{

}

#elif 0
/*
  The TFT_eSPI library incorporates an Adafruit_GFX compatible
  button handling class, this sketch is based on the Arduin-o-phone
  example.

  This example diplays a keypad where numbers can be entered and
  send to the Serial Monitor window.

  The sketch has been tested on the ESP8266 (which supports SPIFFS)

  The minimum screen size is 320 x 240 as that is the keypad size.

  TOUCH_CS and SPI_TOUCH_FREQUENCY must be defined in the User_Setup.h file
  for the touch functions to do anything.
*/

// The SPIFFS (FLASH filing system) is used to hold touch screen
// calibration data

#include "FS.h"

#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <WiFi.h>
#include <ArduinoJson.h>

char ssid[] = "henryTestAp";          // change xxxx to Wi-Fi SSID
char password[] = ""; // change xxxx to Wi-Fi password
char server[] = "api.open-meteo.com";

WiFiClient client;
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
DynamicJsonDocument jsonDoc(8192); // JSON formatted data

// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".
#define CALIBRATION_FILE "/TouchCalData3"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL true

// Keypad start position, key sizes and spacing
#define KEY_X 40 // Centre of key
#define KEY_Y 96
#define KEY_W 62 // Width and height
#define KEY_H 30
#define KEY_SPACING_X 18 // X and Y gap
#define KEY_SPACING_Y 20
#define KEY_TEXTSIZE 1                     // Font size multiplier

// Using two fonts since numbers are nice when bold
#define LABEL1_FONT &FreeSansOblique12pt7b // Key label font 1
#define LABEL2_FONT &FreeSansBold12pt7b    // Key label font 2

// Numeric display box size and location
#define DISP_X 1
#define DISP_Y 10
#define DISP_W 238
#define DISP_H 50
#define DISP_TSIZE 3
#define DISP_TCOLOR TFT_CYAN

// Number length, buffer for storing it and character index
#define NUM_LEN 12
char numberBuffer[NUM_LEN + 1] = "";
uint8_t numberIndex = 0;

// We have a status line for messages
#define STATUS_X 120 // Centred on this
#define STATUS_Y 65

void drawKeypad();
void touch_calibrate();
void status(const char *msg);

// Create 15 keys for the keypad
char keyLabel[15][5] = {"New", "Del", "Send", "1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "0", "#"};
uint16_t keyColor[15] = {TFT_RED, TFT_DARKGREY, TFT_DARKGREEN,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE};

// Invoke the TFT_eSPI button class and create all the button objects
TFT_eSPI_Button key[15];

String output;
void connect() // function for Wi-Fi connection
{
  client.connect(server, 80);                                                                        // connect to server on port 80
  client.println("GET /v1/forecast?latitude=40.39&longitude=-80.16&hourly=temperature_2m HTTP/1.1"); // send HTTP request
  client.println("Host: api.open-meteo.com");
  client.println("User-Agent: ESP8266/0.1");
  client.println("Connection: close");
  client.println();
  client.find("\r\n\r\n");

  if (client.println() == 0)
  {
    Serial.println("HTTP request failed");
    return;
  }

  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  Serial.println(status);

  // essential instruction
  DeserializationError error = deserializeJson(jsonDoc, client);
  serializeJson(jsonDoc, output);
  Serial.println(output);
}
//------------------------------------------------------------------------------------------

void setup()
{
  // Use serial port
  Serial.begin(115200);

  WiFi.begin(ssid, password); // initialise Wi-Fi and wait
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("...");
    delay(500); // for WiFi connection
  }
  Serial.println("Connected");
  // Initialise the TFT screen
  tft.init();

  // Set the rotation before we calibrate
  tft.setRotation(0);

  connect();

  // Calibrate the touch screen and retrieve the scaling factors
  touch_calibrate();

  // Clear the screen
  tft.fillScreen(TFT_BLACK);

  // Draw keypad background
  tft.fillRect(0, 0, 240, 320, TFT_DARKGREY);

  // Draw number display area and frame
  tft.fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_BLACK);
  tft.drawRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_WHITE);

  // Draw keypad
  drawKeypad();
}

//------------------------------------------------------------------------------------------

void loop(void)
{
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  // Pressed will be set true is there is a valid touch on the screen
  bool pressed = tft.getTouch(&t_x, &t_y);

  // / Check if any key coordinate boxes contain the touch coordinates
  for (uint8_t b = 0; b < 15; b++)
  {
    if (pressed && key[b].contains(t_x, t_y))
    {
      key[b].press(true); // tell the button it is pressed
    }
    else
    {
      key[b].press(false); // tell the button it is NOT pressed
    }
  }

  // Check if any key has changed state
  for (uint8_t b = 0; b < 15; b++)
  {

    if (b < 3)
      tft.setFreeFont(LABEL1_FONT);
    else
      tft.setFreeFont(LABEL2_FONT);

    if (key[b].justReleased())
      key[b].drawButton(); // draw normal

    if (key[b].justPressed())
    {
      key[b].drawButton(true); // draw invert

      // if a numberpad button, append the relevant # to the numberBuffer
      if (b >= 3)
      {
        if (numberIndex < NUM_LEN)
        {
          numberBuffer[numberIndex] = keyLabel[b][0];
          numberIndex++;
          numberBuffer[numberIndex] = 0; // zero terminate
        }
        status(""); // Clear the old status
      }

      // Del button, so delete last char
      if (b == 1)
      {
        numberBuffer[numberIndex] = 0;
        if (numberIndex > 0)
        {
          numberIndex--;
          numberBuffer[numberIndex] = 0; //' ';
        }
        status(""); // Clear the old status
      }

      if (b == 2)
      {
        status("Sent value to serial port");
        Serial.println(numberBuffer);
      }
      // we dont really check that the text field makes sense
      // just try to call
      if (b == 0)
      {
        status("Value cleared");
        numberIndex = 0;               // Reset index to 0
        numberBuffer[numberIndex] = 0; // Place null in buffer
      }

      // Update the number display field
      tft.setTextDatum(TL_DATUM);       // Use top left corner as text coord datum
      tft.setFreeFont(&FreeSans18pt7b); // Choose a nicefont that fits box
      tft.setTextColor(DISP_TCOLOR);    // Set the font colour

      // Draw the string, the value returned is the width in pixels
      int xwidth = tft.drawString(numberBuffer, DISP_X + 4, DISP_Y + 12);

      // Now cover up the rest of the line up by drawing a black rectangle.  No flicker this way
      // but it will not work with italic or oblique fonts due to character overlap.
      tft.fillRect(DISP_X + 4 + xwidth, DISP_Y + 1, DISP_W - xwidth - 5, DISP_H - 2, TFT_BLACK);

      delay(10); // UI debouncing
    }
  }
}

//------------------------------------------------------------------------------------------

void drawKeypad()
{
  // Draw the keys
  for (uint8_t row = 0; row < 5; row++)
  {
    for (uint8_t col = 0; col < 3; col++)
    {
      uint8_t b = col + row * 3;

      if (b < 3)
        tft.setFreeFont(LABEL1_FONT);
      else
        tft.setFreeFont(LABEL2_FONT);

      key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
                        keyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
    }
  }
}

//------------------------------------------------------------------------------------------

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin())
  {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE))
  {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f)
      {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL)
  {
    // calibration data valid
    tft.setTouch(calData);
  }
  else
  {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL)
    {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f)
    {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

//------------------------------------------------------------------------------------------

// Print something in the mini status bar
void status(const char *msg)
{
  tft.setTextPadding(240);
  // tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.setTextFont(0);
  tft.setTextDatum(TC_DATUM);
  tft.setTextSize(1);
  tft.drawString(msg, STATUS_X, STATUS_Y);
}

//------------------------------------------------------------------------------------------
#elif 0
/*
 An example showing rainbow colours on a 3.0 or 3.2" TFT LCD screen
 and to show basic examples of font use.

 This sketch uses the GLCD, 2, 4, 6 fonts only.

 Make sure all the required fonts are loaded by editing the
 User_Setup.h file in the TFT_eSPI library folder.


  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  ######           TO SELECT THE FONTS AND PINS YOU USE              ######
  #########################################################################
 */

#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

unsigned long targetTime = 0;
byte red = 31;
byte green = 0;
byte blue = 0;
byte state = 0;
unsigned int colour = red << 11; // Colour order is RGB 5+6+5 bits each
void rainbow_fill();

void setup(void)
{
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);

  targetTime = millis() + 1000;
}

void loop()
{

  if (targetTime < millis())
  {
    targetTime = millis() + 10000;

    rainbow_fill(); // Fill the screen with rainbow colours

    // The standard AdaFruit font still works as before
    tft.setTextColor(TFT_BLACK); // Background is not defined so it is transparent
    tft.setCursor(100, 5);
    tft.setTextFont(1); // Select font 1 which is the Adafruit GLCD font
    tft.print("Original Adafruit font!");

    // The new larger fonts do not need to use the .setCursor call, coords are embedded
    tft.setTextColor(TFT_BLACK); // Do not plot the background colour
    // Overlay the black text on top of the rainbow plot (the advantage of not drawing the backgorund colour!)
    tft.drawCentreString("Font size 2", 160, 14, 2);        // Draw text centre at position 120, 14 using font 2
    tft.drawCentreString("Font size 4", 160, 30, 4);        // Draw text centre at position 120, 30 using font 4
    tft.drawCentreString("12.34", 160, 54, 6);              // Draw text centre at position 120, 54 using font 6
    tft.drawCentreString("12.34 is in font 6", 160, 92, 2); // Draw text centre at position 120, 92 using font 2
    // Note the x, y position is the top left corner of the first character printed!

    // draw a floating point number
    float pi = 3.14159;                                     // Value to print
    int precision = 3;                                      // Number of digits after decimal point
    int xpos = 130;                                         // x position
    int ypos = 110;                                         // y position
    int font = 2;                                           // font number 2
    xpos += tft.drawFloat(pi, precision, xpos, ypos, font); // Draw rounded number and return new xpos delta for next print position
    tft.drawString(" is pi", xpos, ypos, font);             // Continue printing from new x position

    tft.setTextSize(1); // We are using a text size multiplier of 1

    tft.setTextColor(TFT_BLACK);   // Set text colour to black, no background (so transparent)
    tft.setCursor(76, 150, 4);     // Set cursor to x = 76, y = 150 and use font 4
    tft.println("Transparent..."); // As we use println, the cursor moves to the next line

    tft.setCursor(70, 175);                 // Set cursor to x = 70, y = 175
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // Set text colour to white and background to black
    tft.println("White on black");

    tft.setTextFont(4);     // Select font 4 without moving cursor
    tft.setCursor(00, 210); // Set cursor to x = 90, y = 210 without changing the font
    tft.setTextColor(TFT_WHITE);

    // By using the print class we can use all the formatting features like printing HEX
    tft.print(57005, HEX);   // Cursor does no move to next line
    tft.println(48879, HEX); // print and move cursor to next line

    tft.setTextColor(TFT_GREEN, TFT_BLACK); // This time we will use green text on a black background
    tft.setTextFont(2);                     // Select font 2
    // Text will wrap to the next line if needed, by luck it breaks the lines at a space..
    tft.println(" Ode to a Small Lump of Green Putty I Found in My Armpit One Midsummer Morning ");

    tft.drawCentreString("34.56", 160, 300, 7);              // Draw text centre at position 120, 54 using font 6
    tft.drawCentreString("34.56 is in font 7", 160, 350, 2); // Draw text centre at position 120, 92 using font 2

    tft.drawCentreString("78.90", 160, 370, 8);              // Draw text centre at position 120, 54 using font 6
    tft.drawCentreString("78.90 is in font 8", 160, 450, 2); // Draw text centre at position 120, 92 using font 2
  }
}

// Fill screen with a rainbow pattern
void rainbow_fill()
{
  // The colours and state are not initialised so the start colour changes each time the funtion is called

  for (int i = 479; i > 0; i--)
  {
    // Draw a vertical line 1 pixel wide in the selected colour
    tft.drawFastHLine(0, i, tft.width(), colour); // in this example tft.width() returns the pixel width of the display
    // This is a "state machine" that ramps up/down the colour brightnesses in sequence
    switch (state)
    {
    case 0:
      green++;
      if (green == 64)
      {
        green = 63;
        state = 1;
      }
      break;
    case 1:
      red--;
      if (red == 255)
      {
        red = 0;
        state = 2;
      }
      break;
    case 2:
      blue++;
      if (blue == 32)
      {
        blue = 31;
        state = 3;
      }
      break;
    case 3:
      green--;
      if (green == 255)
      {
        green = 0;
        state = 4;
      }
      break;
    case 4:
      red++;
      if (red == 32)
      {
        red = 31;
        state = 5;
      }
      break;
    case 5:
      blue--;
      if (blue == 255)
      {
        blue = 0;
        state = 0;
      }
      break;
    }
    colour = red << 11 | green << 5 | blue;
  }
}

#elif 0
#include <Arduino.h>
#define DEEP_SLEEP_TOUCH_WAKEUP

#ifdef DEEP_SLEEP_TOUCH_WAKEUP
#define Threshold 40 /* Greater the value, more the sensitivity */

RTC_DATA_ATTR int bootCount = 0;
touch_pad_t touchPin;
/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

/*
Method to print the touchpad by which ESP32
has been awaken from sleep
*/
void print_wakeup_touchpad(){
  touchPin = esp_sleep_get_touchpad_wakeup_status();

  switch(touchPin)
  {
    case 0  : Serial.println("Touch detected on GPIO 4"); break;
    case 1  : Serial.println("Touch detected on GPIO 0"); break;
    case 2  : Serial.println("Touch detected on GPIO 2"); break;
    case 3  : Serial.println("Touch detected on GPIO 15"); break;
    case 4  : Serial.println("Touch detected on GPIO 13"); break;
    case 5  : Serial.println("Touch detected on GPIO 12"); break;
    case 6  : Serial.println("Touch detected on GPIO 14"); break;
    case 7  : Serial.println("Touch detected on GPIO 27"); break;
    case 8  : Serial.println("Touch detected on GPIO 33"); break;
    case 9  : Serial.println("Touch detected on GPIO 32"); break;
    default : Serial.println("Wakeup not by touchpad"); break;
  }
}

void callback(){
  //placeholder callback function
}

#endif
//   Diagnostic test for the displayed colour order
//
// Written by Bodmer 17/2/19 for the TFT_eSPI library:
// https://github.com/Bodmer/TFT_eSPI

/*
 Different hardware manufacturers use different colour order
 configurations at the hardware level.  This may result in
 incorrect colours being displayed.

 Incorrectly displayed colours could also be the result of
 using the wrong display driver in the library setup file.

 Typically displays have a control register (MADCTL) that can
 be used to set the Red Green Blue (RGB) colour order to RGB
 or BRG so that red and blue are swapped on the display.

 This control register is also used to manage the display
 rotation and coordinate mirroring. The control register
 typically has 8 bits, for the ILI9341 these are:

 Bit Function
 7   Mirror Y coordinate (row address order)
 6   Mirror X coordinate (column address order)
 5   Row/column exchange (for rotation)
 4   Refresh direction (top to bottom or bottom to top in portrait orientation)
 3   RGB order (swaps red and blue)
 2   Refresh direction (top to bottom or bottom to top in landscape orientation)
 1   Not used
 0   Not used

 The control register bits can be written with this example command sequence:

    tft.writecommand(TFT_MADCTL);
    tft.writedata(0x48);          // Bits 6 and 3 set

 0x48 is the default value for ILI9341 (0xA8 for ESP32 M5STACK)
 in rotation 0 orientation.

 Another control register can be used to "invert" colours,
 this swaps black and white as well as other colours (e.g.
 green to magenta, red to cyan, blue to yellow).

 To invert colours insert this line after tft.init() or tft.begin():

    tft.invertDisplay( invert ); // Where invert is true or false

*/

#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

void setup(void)
{
  Serial.begin(115200);

  tft.init();

  tft.fillScreen(TFT_BLACK);
#if 1
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);
  Serial.println(tft.width());
  Serial.println(tft.height());
  // Set "cursor" at top left corner of display (0,0) and select font 4
  tft.setCursor(0, 4, 4);

  // Set the font colour to be white with a black background
  tft.setTextColor(TFT_WHITE);

  // We can now plot text on screen using the "print" class
  tft.println(" Initialised default\n");
  tft.println(" White text");

  tft.setTextColor(TFT_RED);
  tft.println(" Red text");

  tft.setTextColor(TFT_GREEN);
  tft.println(" Green text");

  tft.setTextColor(TFT_BLUE);
  tft.println(" Blue text");
#endif
  //delay(5000);
#ifdef DEEP_SLEEP_TOUCH_WAKEUP
//Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32 and touchpad too
  print_wakeup_reason();
  print_wakeup_touchpad();

  //Setup interrupt on Touch Pad 0 (GPIO4)
  touchAttachInterrupt(T0, callback, Threshold);

  //Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();

  //Go to sleep now
  Serial.println("Going to sleep now");
  //esp_deep_sleep_start();
  Serial.println("This will never be printed");
#endif
}

void loop()
{
#if 0
  tft.invertDisplay(false); // Where i is true or false

  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);

  tft.setCursor(0, 4, 4);

  tft.setTextColor(TFT_WHITE);
  tft.println(" Invert OFF\n");

  tft.println(" White text");

  tft.setTextColor(TFT_RED);
  tft.println(" Red text");

  tft.setTextColor(TFT_GREEN);
  tft.println(" Green text");

  tft.setTextColor(TFT_BLUE);
  tft.println(" Blue text");

  delay(5000);

  // Binary inversion of colours
  tft.invertDisplay(true); // Where i is true or false

  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);

  tft.setCursor(0, 4, 4);

  tft.setTextColor(TFT_WHITE);
  tft.println(" Invert ON\n");

  tft.println(" White text");

  tft.setTextColor(TFT_RED);
  tft.println(" Red text");

  tft.setTextColor(TFT_GREEN);
  tft.println(" Green text");

  tft.setTextColor(TFT_BLUE);
  tft.println(" Blue text");
#endif
  delay(5000);
}

#else
/*
 Adapted from the Adafruit graphicstest sketch, see original header at end
 of sketch.

 This sketch uses the GLCD font (font 1) only.

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
*/

#include "SPI.h"
#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();

unsigned long total = 0;
unsigned long tn = 0;

unsigned long testFillScreen()
{
  unsigned long start = micros();
  tft.fillScreen(TFT_BLACK);
  tft.fillScreen(TFT_RED);
  tft.fillScreen(TFT_GREEN);
  tft.fillScreen(TFT_BLUE);
  tft.fillScreen(TFT_BLACK);
  return micros() - start;
}

unsigned long testText()
{
  tft.fillScreen(TFT_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(TFT_RED);
  tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  // tft.setTextColor(TFT_GREEN,TFT_BLACK);
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}

unsigned long testLines(uint16_t color)
{
  unsigned long start, t;
  int x1, y1, x2, y2,
      w = tft.width(),
      h = tft.height();

  tft.fillScreen(TFT_BLACK);

  x1 = y1 = 0;
  y2 = h - 1;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6)
    tft.drawLine(x1, y1, x2, y2, color);
  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6)
    tft.drawLine(x1, y1, x2, y2, color);
  t = micros() - start; // fillScreen doesn't count against timing
  yield();
  tft.fillScreen(TFT_BLACK);

  x1 = w - 1;
  y1 = 0;
  y2 = h - 1;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6)
    tft.drawLine(x1, y1, x2, y2, color);
  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6)
    tft.drawLine(x1, y1, x2, y2, color);
  t += micros() - start;
  yield();
  tft.fillScreen(TFT_BLACK);

  x1 = 0;
  y1 = h - 1;
  y2 = 0;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6)
    tft.drawLine(x1, y1, x2, y2, color);
  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6)
    tft.drawLine(x1, y1, x2, y2, color);
  t += micros() - start;
  yield();
  tft.fillScreen(TFT_BLACK);

  x1 = w - 1;
  y1 = h - 1;
  y2 = 0;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6)
    tft.drawLine(x1, y1, x2, y2, color);
  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6)
    tft.drawLine(x1, y1, x2, y2, color);
  yield();
  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2)
{
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(TFT_BLACK);
  start = micros();
  for (y = 0; y < h; y += 5)
    tft.drawFastHLine(0, y, w, color1);
  for (x = 0; x < w; x += 5)
    tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

unsigned long testRects(uint16_t color)
{
  unsigned long start;
  int n, i, i2,
      cx = tft.width() / 2,
      cy = tft.height() / 2;

  tft.fillScreen(TFT_BLACK);
  n = min(tft.width(), tft.height());
  start = micros();
  for (i = 2; i < n; i += 6)
  {
    i2 = i / 2;
    tft.drawRect(cx - i2, cy - i2, i, i, color);
  }

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2)
{
  unsigned long start, t = 0;
  int n, i, i2,
      cx = tft.width() / 2 - 1,
      cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  n = min(tft.width(), tft.height());
  for (i = n - 1; i > 0; i -= 6)
  {
    i2 = i / 2;
    start = micros();
    tft.fillRect(cx - i2, cy - i2, i, i, color1);
    t += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx - i2, cy - i2, i, i, color2);
  }

  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color)
{
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(TFT_BLACK);
  start = micros();
  for (x = radius; x < w; x += r2)
  {
    for (y = radius; y < h; y += r2)
    {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color)
{
  unsigned long start;
  int x, y, r2 = radius * 2,
            w = tft.width() + radius,
            h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for (x = 0; x < w; x += r2)
  {
    for (y = 0; y < h; y += r2)
    {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testTriangles()
{
  unsigned long start;
  int n, i, cx = tft.width() / 2 - 1,
            cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  n = min(cx, cy);
  start = micros();
  for (i = 0; i < n; i += 5)
  {
    tft.drawTriangle(
        cx, cy - i,     // peak
        cx - i, cy + i, // bottom left
        cx + i, cy + i, // bottom right
        tft.color565(0, 0, i));
  }

  return micros() - start;
}

unsigned long testFilledTriangles()
{
  unsigned long start, t = 0;
  int i, cx = tft.width() / 2 - 1,
         cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  start = micros();
  for (i = min(cx, cy); i > 10; i -= 5)
  {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                     tft.color565(0, i, i));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                     tft.color565(i, i, 0));
  }

  return t;
}

unsigned long testRoundRects()
{
  unsigned long start;
  int w, i, i2,
      cx = tft.width() / 2 - 1,
      cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  w = min(tft.width(), tft.height());
  start = micros();
  for (i = 0; i < w; i += 6)
  {
    i2 = i / 2;
    tft.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(i, 0, 0));
  }

  return micros() - start;
}

unsigned long testFilledRoundRects()
{
  unsigned long start;
  int i, i2,
      cx = tft.width() / 2 - 1,
      cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  start = micros();
  for (i = min(tft.width(), tft.height()); i > 20; i -= 6)
  {
    i2 = i / 2;
    tft.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(0, i, 0));
    yield();
  }

  return micros() - start;
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("");
  Serial.println("");
  Serial.println("TFT_eSPI library test!");

  tft.init();

  tn = micros();
  tft.fillScreen(TFT_BLACK);

  yield();
  Serial.println(F("Benchmark                Time (microseconds)"));

  yield();
  Serial.print(F("Screen fill              "));
  yield();
  Serial.println(testFillScreen());
  // total+=testFillScreen();
  // delay(500);

  yield();
  Serial.print(F("Text                     "));
  yield();
  Serial.println(testText());
  // total+=testText();
  // delay(3000);

  yield();
  Serial.print(F("Lines                    "));
  yield();
  Serial.println(testLines(TFT_CYAN));
  // total+=testLines(TFT_CYAN);
  // delay(500);

  yield();
  Serial.print(F("Horiz/Vert Lines         "));
  yield();
  Serial.println(testFastLines(TFT_RED, TFT_BLUE));
  // total+=testFastLines(TFT_RED, TFT_BLUE);
  // delay(500);

  yield();
  Serial.print(F("Rectangles (outline)     "));
  yield();
  Serial.println(testRects(TFT_GREEN));
  // total+=testRects(TFT_GREEN);
  // delay(500);

  yield();
  Serial.print(F("Rectangles (filled)      "));
  yield();
  Serial.println(testFilledRects(TFT_YELLOW, TFT_MAGENTA));
  // total+=testFilledRects(TFT_YELLOW, TFT_MAGENTA);
  // delay(500);

  yield();
  Serial.print(F("Circles (filled)         "));
  yield();
  Serial.println(testFilledCircles(10, TFT_MAGENTA));
  // total+= testFilledCircles(10, TFT_MAGENTA);

  yield();
  Serial.print(F("Circles (outline)        "));
  yield();
  Serial.println(testCircles(10, TFT_WHITE));
  // total+=testCircles(10, TFT_WHITE);
  // delay(500);

  yield();
  Serial.print(F("Triangles (outline)      "));
  yield();
  Serial.println(testTriangles());
  // total+=testTriangles();
  // delay(500);

  yield();
  Serial.print(F("Triangles (filled)       "));
  yield();
  Serial.println(testFilledTriangles());
  // total += testFilledTriangles();
  // delay(500);

  yield();
  Serial.print(F("Rounded rects (outline)  "));
  yield();
  Serial.println(testRoundRects());
  // total+=testRoundRects();
  // delay(500);

  yield();
  Serial.print(F("Rounded rects (filled)   "));
  yield();
  Serial.println(testFilledRoundRects());
  // total+=testFilledRoundRects();
  // delay(500);

  yield();
  Serial.println(F("Done!"));
  yield();
  // Serial.print(F("Total = ")); Serial.println(total);

  // yield();Serial.println(millis()-tn);
}

void loop(void)
{
  for (uint8_t rotation = 0; rotation < 4; rotation++)
  {
    tft.setRotation(rotation);
    testText();
    delay(2000);
  }
}

/***************************************************
  Original Adafruit text:

  This is an example sketch for the Adafruit 2.2" SPI display.
  This library works with the Adafruit 2.2" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/1480

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#endif
