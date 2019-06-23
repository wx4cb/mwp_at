#ifndef __TFT_H__
#define __TFT_H__
#include <TimerOne.h>

#define TFT_UPDATE_FREQ 250000

#include <TFT_HX8357.h>
#include <User_Setup.h>
#include "bitmap_functions.h"
#include "Free_Fonts.h" // Include the header file attached to this sketch
#include "lang.h"

extern const char __PROGNAME__[];
extern const char __COPYLEFT__[];
extern const char __PROGVERS__[];

// SERVO Data
extern  int pan_current;
extern  int tilt_current;
extern  int pan_target;
extern  int tilt_target;
extern  int lasttarget;
extern  int pan_servo_error;
extern  int tilt_servo_error;

// GPS Data
extern  char gps_date_str[12];
extern  char gps_time_str[12];
extern  char gps_lat_str[12];
extern  char gps_lon_str[12];
extern float  gps_lat, gps_lon, gps_altitude, gps_course, gps_speed;
extern unsigned long gps_age, gps_time, gps_chars, gps_date_raw, gps_time_raw;
extern unsigned short gps_sentences, gps_failed, gps_passed;
extern unsigned int gps_sats, gps_hdop;

// These are used when calling drawBMP() function
#define BU_BMP 1 // Temporarily flip the TFT coords for standard Bottom-Up bit maps
#define TD_BMP 0 // Draw inverted Top-Down bitmaps in standard coord frame

#define TFT_HARLEQUIN ConvertRGB(0x3F, 0xFF, 0x00)
// Invoke library
TFT_HX8357 tft = TFT_HX8357();

static int TFT_WIDTH = tft.width();
static int TFT_HEIGHT = tft.height();
static int TFT_CENTER_X = 240;
static int TFT_CENTER_Y = 160;
static int TFT_FONTSIZE = 15;
static bool TFT_FIRST_UPDATE = true;

static long UpdateMillis = 0;

void UpdateTFT(void);
void SetupTFT(void);
void DoSplash(void);
void TFT_Print(int x, int y, char txt[], int color, int back_color, int txtsize);
void TFT_Rotate(int);
void TFT_DrawGrid (int);
word ConvertRGB( byte , byte , byte );

void UpdateTFT(void) {

  char buf[80];

  // Turn off interrupts
  noInterrupts();

  if (TFT_FIRST_UPDATE) {
    tft.fillScreen(TFT_BLACK);

    Serial.println("Aattch TFT Update to timer");
    Timer1.initialize(TFT_UPDATE_FREQ);
    Timer1.attachInterrupt(UpdateTFT); // blinkLED to run every 0.15 seconds    tft.fillScreen(TFT_BLACK);

    tft.drawRoundRect(0, 0, tft.width(), tft.height(), 10, TFT_CYAN);
    // Draw Title
    TFT_FIRST_UPDATE = false;
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setFreeFont(FSS12);

    tft.drawCentreString(__PROGNAME__, TFT_CENTER_X, 6, 1); // Next size up font 2
    tft.setTextDatum(ML_DATUM);
    tft.setFreeFont(FSS9);
    //TFT_Print(15, 400, __COPYLEFT__, TFT_WHITE, TFT_BLACK, 1);

    tft.drawString(__COPYLEFT__, 30, 320, GFXFF); // Next size up font 2

    tft.setTextFont(2);
    //TFT_DrawGrid(25);

    tft.setTextDatum(MC_DATUM); // Centre text on x,y position

    // Draw icons
    int ypos = 35;
    int xpos = 30;
    drawRAW("uas.dat", 343, 45, 100, 72);
    // box sections
    tft.drawRoundRect(xpos - 20, 30, tft.width() - 22, 100, 5, TFT_CYAN);

    // Print Labels
    TFT_Print(xpos, ypos, LANG_LATITUDE, TFT_HARLEQUIN, TFT_BLACK, 2);
    TFT_Print(xpos, ypos += TFT_FONTSIZE, LANG_LONGITUDE, TFT_HARLEQUIN, TFT_BLACK, 2);
    TFT_Print(xpos, ypos += TFT_FONTSIZE, LANG_HEADING, TFT_HARLEQUIN, TFT_BLACK, 2);

    // Do Lower Box
    drawRAW("gs.dat", 350, 170, 100, 75);

    tft.drawRoundRect(xpos - 20, 135, tft.width() - 22, 155, 5, TFT_CYAN);
    ypos = 140;
    TFT_Print(xpos, ypos, LANG_LATITUDE, TFT_HARLEQUIN, TFT_BLACK, 2);
    TFT_Print(xpos, ypos += TFT_FONTSIZE, LANG_LONGITUDE, TFT_HARLEQUIN, TFT_BLACK, 2);
    TFT_Print(xpos, ypos += TFT_FONTSIZE, LANG_CURRENT, TFT_HARLEQUIN, TFT_BLACK, 2);
    TFT_Print(xpos, ypos += TFT_FONTSIZE, LANG_TARGET, TFT_HARLEQUIN, TFT_BLACK, 2);
    TFT_Print(xpos, ypos += TFT_FONTSIZE, LANG_ERROR, TFT_HARLEQUIN, TFT_BLACK, 2);
  }

  /*
     Draw the actual values
  */
  // Upper Box
  int ypos = 35;
  int xpos = 165;
  TFT_Print(xpos, ypos, "gps_lat_str", TFT_ORANGE, TFT_BLACK, 2);
  TFT_Print(xpos, ypos += TFT_FONTSIZE, "gps_lon_str", TFT_ORANGE, TFT_BLACK, 2);
  TFT_Print(xpos, ypos += TFT_FONTSIZE, "Heading", TFT_ORANGE, TFT_BLACK, 2);


  // Lower Box
  ypos = 140;
  TFT_Print(xpos, ypos, gps_lat_str, TFT_ORANGE, TFT_BLACK, 2);
  TFT_Print(xpos, ypos += TFT_FONTSIZE, gps_lon_str, TFT_ORANGE, TFT_BLACK, 2);

  sprintf(buf, "%d Degrees    ", pan_current);
  TFT_Print(xpos, ypos += TFT_FONTSIZE, buf, TFT_ORANGE, TFT_BLACK, 2);

  sprintf(buf, "%d Degrees    ", pan_target);
  TFT_Print(xpos, ypos += TFT_FONTSIZE, buf, TFT_ORANGE, TFT_BLACK, 2);

  sprintf(buf, "%d Degrees    ", pan_servo_error);
  if (pan_servo_error >= -3 && pan_servo_error >= 0 && pan_servo_error <= 3) {
    TFT_Print(xpos, ypos += TFT_FONTSIZE, buf, TFT_GREEN, TFT_BLACK, 2);
  } else {
    TFT_Print(xpos, ypos += TFT_FONTSIZE, buf, TFT_RED, TFT_BLACK, 2);
  }


  // Print GPS Sats in bottom right
  sprintf(buf, "%s %s", gps_date_str, gps_time_str);
  TFT_Print(300, 320, buf, TFT_GREEN, TFT_BLACK, 1);

  // Turn back on interrupts
  interrupts();
}

void InitTFT(void) {
  Serial.println("Init TFT");
  tft.begin();

  // Set display orientation - USB Top Left
  TFT_Rotate(3);

  Serial.print("TFT Dimensions: ");
  Serial.print(tft.width());
  Serial.print("x");
  Serial.println(tft.height());

  // Show Splash Screen
  DoSplash();

  tft.fillScreen(TFT_BLACK);

  // Set startup fontsize etc
  tft.setFreeFont(FSB12);
  tft.setTextDatum(MC_DATUM); // Centre text on x,y position
  TFT_FONTSIZE = tft.fontHeight(GFXFF);
  tft.println();
}

void DoSplash(void) {
  // Draw splash BMP
  // Draw bmp image top left corner at x,y = 0,0
  // Image must fit (one day I will add clipping... but it will slow things down)

  drawRAW("splash.dat", 0, 0, 480, 320);

  tft.setCursor(0, 0, 2);
  int txtlen = strlen(__PROGVERS__) / 2;
  TFT_Print(TFT_CENTER_Y + 50, 300, __PROGVERS__, TFT_YELLOW, TFT_BLACK, 1);

  delay(500);

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 2);
}

void TFT_Print(int x, int y, char txt[], int color, int back_color, int txtsize) {
  // Make sure we aren't drawing off the end of the screen
  if (y > tft.height() - TFT_FONTSIZE) {
    y = tft.height() - TFT_FONTSIZE;
  }
  if (x >= tft.width() - TFT_FONTSIZE) {
    x = tft.width() - TFT_FONTSIZE;
  }

  if (y < 0) {
    y = 0;
  }
  if (x < 0) {
    x = 0;
  }
  tft.setCursor(x, y);
  tft.setTextColor(color, back_color);
  tft.setTextSize(txtsize);
  tft.print(txt);
}

void TFT_Rotate(int dir) {
  // horizontal - USB to the left
  tft.setRotation(dir);
  // Update display dimensions
  TFT_WIDTH = tft.width();
  TFT_HEIGHT = tft.height();

}

void TFT_DrawGrid (int gridspacing) { // routine to generate a square grid on screen x pixels per side
  tft.setCursor(0, 0);
  int ScreenMax = tft.width(); // manually ammended, could be included in function as a parameter
  // vertical lines
  for (int i = 0; i < ScreenMax + 1; i += gridspacing) {
    if (1i == 0 || i == 50 || i == 100 || i == 80 || i == 150 || i == 200 || i == 250 || i == 300 || i == 350) {
      tft.drawLine(i, 0, i, ScreenMax - 1, TFT_RED);
    } else {
      tft.drawLine(i, 0, i, ScreenMax - 1, TFT_WHITE);
    }
  } // End if
  // horizontal lines
  for (int i = 0; i < ScreenMax + 1; i += gridspacing) {
    tft.drawLine(0, i, ScreenMax - 1, i, TFT_WHITE);
  } // End if
  delay (10000);
}// End grid

// Convert an RGB colour to TFT colour
word ConvertRGB( byte R, byte G, byte B)
{
  return ( ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3) );
}

void print_float_at(float val, int x, int y)
{
  char buf[10];
  int16_t x1, y1, w, h;
  dtostrf(val, 5, 1, buf);   //e.g. 123.4
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setCursor(x, y);
  tft.print(buf);
}
#endif
