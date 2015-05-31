/* -----------------------------------------------------------------------------------
 * $Author: robaby@gmail.com $
 * $Date: 2012-04-08 23:54:07 +0100 (Sun, 08 Apr 2012) $
 * $Revision: 4 $
 * ----------------------------------
 * 
 * Full Information:  
 *    Code and Breadboarding: http://rowansimms.com/article.php/lcd-hookup-in-seconds
 *    Make your own Shield:   http://rowansimms.com/article.php/lcd-hookup-in-seconds-shield
 *
 * Adaption of the LiquidCrystal library shipped with Arduino 22, 
 * now updated for Arduino 1.0.
 * Code originally developed by Steve Hobley - February 2011
 *      updates and maintenance by Rowan Simms   code@rowansimms.com
 *
 * Changes Log:
 * v1.0
 *    - Now works with Arduino 1.0 (not backwards compatible)
 *    - Re-ordered Shift Register Pinouts to allow for better prototyping
 *
 *
 * ---Shift Register 74HC595---
 * [SR Pin 14 (DS)]    to Arduino pin - [datapin]
 * [SR Pin 12 (ST_CP)] to Arduino pin - [latchpin]
 * [SR Pin 11 (SH_CP)] to Arduino pin - [clockpin]
 * Black wire to Ground
 * Red wire to +5v
 *
 * -----Shift Reg to LCD--------
 * SR Pin 15  - ENABLE        10000000
 * SR Pin 1   - D7            00000010
 * SR Pin 2   - D6            00000100
 * SR Pin 3   - D5            00001000
 * SR Pin 4   - D4            00010000
 * SR Pin 5   - MOSFET / LED1 00100000
 * SR Pin 6   - LED 2         01000000
 * SR Pin 7   - RS            00000001
 *
 * -----------------------------------------------------------------------------------
 */
// 595 mappings - LED1 is also the backlight controller

#define ENABLE_PIN  B00000001
#define RS_PIN		B10000000
#define LED1_PIN    B00100000
#define LED2_PIN    B01000000
#define DATABITS	B00011110
#define PIN_D4		B00010000
#define PIN_D5		B00001000
#define PIN_D6		B00000100
#define PIN_D7		B00000010

#include "LiquidCrystal595.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LiquidCrystal595::LiquidCrystal595(uint8_t datapin, uint8_t latchpin, uint8_t clockpin)
{
  init(datapin, latchpin, clockpin);
}

// Performs the shift, MSB first 
void LiquidCrystal595::shift595()
{
    digitalWrite(_latchpin, LOW);
    shiftOut(_datapin, _clockpin, MSBFIRST, _register);  
    digitalWrite(_latchpin, HIGH);
}

void LiquidCrystal595::init(uint8_t datapin, uint8_t latchpin, uint8_t clockpin)
{
  _datapin  = datapin;
  _latchpin = latchpin;
  _clockpin = clockpin;

   pinMode(_datapin, OUTPUT);
   pinMode(_latchpin, OUTPUT);
   pinMode(_clockpin, OUTPUT);
   
   _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  
  begin(16, 1);  
}

void LiquidCrystal595::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) 
{
  
  if (lines > 1) 
  {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  delayMicroseconds(50000); 
  // Now we pull both RS and R/W low to begin commands
  
  setRSPin(LOW);
  setEPin(LOW);
  shift595();
  
  //digitalWrite(_rs_pin, LOW);
  //digitalWrite(_enable_pin, LOW);
  
  //if (_rw_pin != 255) { 
  //  digitalWrite(_rw_pin, LOW);
  //}
  
  //put the LCD into 4 bit or 8 bit mode
  if (! (_displayfunction & LCD_8BITMODE)) 
  {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
    delayMicroseconds(4500); // wait min 4.1ms

    // second try
    write4bits(0x03);
    delayMicroseconds(4500); // wait min 4.1ms
    
    // third go!
    write4bits(0x03); 
    delayMicroseconds(150);

    // finally, set to 4-bit interface
    write4bits(0x02); 
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(4500);  // wait more than 4.1ms

    // second try
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(150);

    // third go
    command(LCD_FUNCTIONSET | _displayfunction);
  }

  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);  

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);

}

/********** high level commands, for the user! */
void LiquidCrystal595::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal595::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal595::setCursor(uint8_t col, uint8_t row)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal595::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal595::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal595::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal595::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal595::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal595::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal595::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal595::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal595::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal595::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal595::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal595::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal595::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}

/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal595::command(uint8_t value) {
  send(value, LOW);
}

inline size_t LiquidCrystal595::write(uint8_t value) {
  send(value, HIGH);
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void LiquidCrystal595::send(uint8_t value, uint8_t mode) 
{
  setRSPin(mode);
  shift595();
  
  //digitalWrite(_rs_pin, mode);

  // if there is a RW pin indicated, set it low to Write
  //if (_rw_pin != 255) { 
  //  digitalWrite(_rw_pin, LOW);
  //}
  
  if (_displayfunction & LCD_8BITMODE) 
  {
    write8bits(value); 
  } 
  else 
  {
    write4bits(value>>4);
    write4bits(value);
  }
}

void LiquidCrystal595::pulseEnable(void) 
{
   // LOW / HIGH / LOW of ENABLE_PIN
  setEPin(LOW);   // LOW
  shift595();
  
  delayMicroseconds(1);
  setEPin(HIGH);    // HIGH
  shift595();
  
  delayMicroseconds(1); // enable pulse must be >450ns
  
  setEPin(LOW);   // LOW
  shift595();
  delayMicroseconds(100);  // commands need > 37us to settle
}

void LiquidCrystal595::write4bits(uint8_t value) 
{
  int val_nibble= value & 0x0F;  //clean the value.  (unnecessary)
	
  setD4Pin(val_nibble & 01);
  val_nibble >>= 1;
  setD5Pin(val_nibble & 01);
  val_nibble >>= 1;
  setD6Pin(val_nibble & 01);
  val_nibble >>= 1;
  setD7Pin(val_nibble & 01);
  
  pulseEnable();
}

void LiquidCrystal595::write8bits(uint8_t value) 
{
  return;
  // Should not be used
}

// Accessor functions --------------------------------------------------------
void LiquidCrystal595::setLED1Pin(uint8_t pinValue)
{
	if (pinValue == HIGH)
	{
		_register |= LED1_PIN;    // HIGH
	}
	else
	{
		_register &= ~LED1_PIN;   // LOW
	}
}

void LiquidCrystal595::setLED2Pin(uint8_t pinValue)
{
	if (pinValue == HIGH)
	{
		_register |= LED2_PIN;    // HIGH
	}
	else
	{
		_register &= ~LED2_PIN;   // LOW
	}
}

void LiquidCrystal595::setEPin(uint8_t pinValue)
{
	if (pinValue == HIGH)
	{
		_register |= ENABLE_PIN;    // HIGH
	}
	else
	{
		_register &= ~ENABLE_PIN;   // LOW
	}
}

void LiquidCrystal595::setD4Pin(uint8_t pinValue)
{
	if (pinValue == HIGH)
	{
		_register |= PIN_D4;    // HIGH
	}
	else
	{
		_register &= ~PIN_D4;   // LOW
	}
}

void LiquidCrystal595::setD5Pin(uint8_t pinValue)
{
	if (pinValue == HIGH)
	{
		_register |= PIN_D5;    // HIGH
	}
	else
	{
		_register &= ~PIN_D5;   // LOW
	}
}
void LiquidCrystal595::setD6Pin(uint8_t pinValue)
{
	if (pinValue == HIGH)
	{
		_register |= PIN_D6;    // HIGH
	}
	else
	{
		_register &= ~PIN_D6;   // LOW
	}
}
void LiquidCrystal595::setD7Pin(uint8_t pinValue)
{
	if (pinValue == HIGH)
	{
		_register |= PIN_D7;    // HIGH
	}
	else
	{
		_register &= ~PIN_D7;   // LOW
	}
}

void LiquidCrystal595::setRSPin(uint8_t pinValue)
{
	if (pinValue == HIGH)
	{
		_register |= RS_PIN;    // HIGH
	}
	else
	{
		_register &= ~RS_PIN;   // LOW
	}
}
