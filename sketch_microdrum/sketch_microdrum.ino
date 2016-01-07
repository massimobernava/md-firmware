 
//==============================================================================
//=>                microDRUM/nanoDRUM firmware v1.4.0                        <= 
//=>                        www.microdrum.net                                 <=
//=>                         CC BY-NC-SA 3.0                                  <=
//=>                                                                          <=
//=> Massimo Bernava                                                          <=
//=> massimo.bernava@gmail.com                                                <=
//=> 2016-01-07                                                               <=
//==============================================================================

//========CONFIGURE=============
#define USE_LCD           1     // Use LCD
#define USE_595           1     // 1 with nanoDrum and v0.8
#define USE_PROFILER      0     // Use Profiler (only for Debug)
#define FASTADC           1     // FASTADC = Prescaler_16, VERYFASTADC = Prescaler_8, VERYVERYFASTADC = Prescaler_4
#define SERIALSPEED       1     // 1 = 115200, 0 = 31250 (MIDI) , picoMIDI use 115200 therefore with nanoDrum and v0.8 use 1
#define USE_DEFAULT_NAME  1     // Use Default Name for pin in LCD Menu
#define USE_WAVTRIGGER    1     // Use WavTrigger
#define WT_16             1     // WT_16 = 16 Wav Trigger Input , WT_24 = 24 Wav Trigger Input
#define ENABLE_CHANNEL    0     // Enable MIDI Channel
#define MENU_LOG          1     // Enable Auto Tune (only with LCD)
#define NANO              1     // 1 = nanoDrum , 0 = microDrum
//==============================

#if defined(__arm__) 
  //TODO: https://github.com/sebnil/DueFlashStorage
#elif defined(__AVR__) 
  #include <EEPROM.h>
#endif

#if USE_WAVTRIGGER
  #include <SoftwareSerial.h>
#endif
//#include <math.h>

#if USE_LCD
  #if USE_595
    #include "LiquidCrystal595.h"
  #else
    #include <LiquidCrystal.h>
  #endif
#endif

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define TIMEFUNCTION millis() //NOT micros() (thresold error)
#define ANALOGREAD(s,p) analogRead(s)

#if defined(__arm__) 
  #define fastWrite(_pin_, _state_) digitalWrite(_pin_, _state_);
#elif defined(__AVR__) 
//#define fastWrite(_pin_, _state_) ( _pin_ < 8 ? (_state_ ?  PORTD |= 1 << _pin_ : PORTD &= ~(1 << _pin_ )) : (_state_ ?  PORTB |= 1 << (_pin_ -8) : PORTB &= ~(1 << (_pin_ -8)  )))
  #define fastWrite(_pin_, _state_) (_state_ ?  PORTD |= 1 << _pin_ : PORTD &= ~(1 << _pin_ ))
#endif

#if ENABLE_CHANNEL
  #define fastNoteOn(_channel,_note,_velocity) { Serial.write(0x90 | _channel);Serial.write(_note);Serial.write(_velocity); }
  #define fastMidiCC(_channel,_number,_value) { Serial.write((0xB0 | _channel)); Serial.write(_number); Serial.write(_value); }
#else
  #define fastNoteOn(_channel,_note,_velocity) { Serial.write(0x90 | 0x09); Serial.write(_note); Serial.write(_velocity);}
  #define fastMidiCC(_channel,_number,_value) { Serial.write((0xB0 | 0x09)); Serial.write(_number); Serial.write(_value); }
#endif

//===========MODE============
enum mode:byte
{
  Off     = 0,
  Standby = 1,
  MIDI    = 2,
  Tool    = 3
};
//============================

//===GLOBAL========================
mode Mode=Off;
unsigned long GlobalTime;
//=================================

//===SETTING================
//===General================
const int delayTime=10;
byte GeneralXtalk=0;
#if NANO
const byte NPin=24;
#else
const byte NPin=48;
#endif
byte NSensor=2;
//===========================

//===HiHat==================
byte HHNoteSensor[]     = {20,50,80,100};
byte HHThresoldSensor[] = {48,36,24,12};
byte HHFootNoteSensor[] = {59,48};
byte HHFootThresoldSensor[] = {127,127};
//===========================

//===Xtalk===================
const byte NXtalkGroup=4;//Max number XTalk groups
int MaxXtalkGroup[NXtalkGroup] = {-1};
int MaxMultiplexerXtalk[8]={-1};
//===========================




