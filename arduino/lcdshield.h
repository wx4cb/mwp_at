#ifndef __LCDSHIELD_H__
#define __LCDSHIELD_H__
#include <LiquidCrystal.h>
//#include <HMC5883L.h>

//extern HMC5883L compass;
//extern Vector norm;

/*
   LCD Sheild Definitions and rudimentary menu/page system
*/
// define some values used by the panel and buttons
int lcd_key     = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

/*
   select the pins used on the LCD panel
*/
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

/*
   Menu System Defs
*/
// screen page
#define PAGE_BASE 0
#define PAGE_MAX 1
int LCDPage = 0;

// Text for LCD Lines
#define LCDBUFSIZ 16

// Page Addresses
#define GPSPAGE PAGE_BASE
#define MAGPAGE PAGE_BASE + 1


/*
   read the buttons
*/
int read_LCD_buttons()
{
  int adc_key_in = 0;

  adc_key_in = analogRead(0);      // read the value from the sensor
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
  // For V1.1 us this threshold
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 250)  return btnUP;
  if (adc_key_in < 450)  return btnDOWN;
  if (adc_key_in < 650)  return btnLEFT;
  if (adc_key_in < 850)  return btnSELECT;

  return btnNONE;  // when all others fail, return this...
}


/*
    Setup the LCD
*/
int SetupLCDShield() {

  // Init LCD
  lcd.begin(16, 2);             // start the library
  lcd.setCursor(0, 0);          // set the cursor to 0,0
}

// Function to pad spaces to unused part of a message and null terminate it.
char makeLCDString(char str[])// Simple C string function
{
  char spaces[] = "                ";     // 16 spaces
  int end = strlen(str);
  strcat(str, &spaces[end]);

  return(str);
}

/*
 * Write each line out on the LCD
 */
void LCDPrint2Line(char line1[], char line2[]) {
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

/*
   Process Menu
*/
int ProcessMenu(void) {
  char str_x[6];
  char str_y[6];
  char str_z[6];
  char LCDText[2][LCDBUFSIZ+1];

 
  //LCDText[0][0] = "\0";
  //LCDText[1][0] = "\0";
 
  switch (LCDPage) {
    case GPSPAGE:
      strncpy(LCDText[0], "GPS               ", LCDBUFSIZ);
      strncpy(LCDText[1], "INOP               ", LCDBUFSIZ);
      break;

    case MAGPAGE:
      /* dtostrf(norm.XAxis, 4, 1, str_x);
        dtostrf(norm.YAxis, 4, 1, str_y);
        dtostrf(norm.ZAxis, 4, 1, str_z);
        snprintf(LCDText[0], LCDBUFSIZ, "Compass");
        snprintf(LCDText[1], LCDBUFSIZ, "INOP");
        snprintf(LCDText[1], LCDBUFSIZ, "%s %s %s", str_x, str_y, str_z);

      */

      strncpy(LCDText[0], "COMPASS", LCDBUFSIZ);
      strncpy(LCDText[1], "INOP", LCDBUFSIZ);
      break;

    default:
      break;
  }

  lcd.setCursor(0, 0);
  lcd.print(LCDText[0]);
  lcd.setCursor(0, 1);
  lcd.print(LCDText[1]);


  // Read switch
  lcd_key = read_LCD_buttons();  // read the buttons
  switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
    case btnRIGHT:
      //Serial.println("RIGHT");
      lcd.clear();
      LCDPage = PAGE_MAX;
      break;
    case btnLEFT:
      //Serial.println("LEFT");
      lcd.clear();
      LCDPage = PAGE_BASE;
      break;
    case btnUP:
      //Serial.println("UP");
      lcd.clear();
      LCDPage--;
      if (LCDPage < PAGE_BASE) {
        LCDPage = PAGE_BASE;
      }
      break;
    case btnDOWN:
      lcd.clear();
      //Serial.println("DOWN");
      LCDPage++;
      if (LCDPage > PAGE_MAX) {
        LCDPage = PAGE_MAX;
      }
      break;
    case btnSELECT:
      //Serial.println("SELECT");
      break;
    case btnNONE:
      //Serial.println("NONE");
      break;
  }
}

#endif
