 
//==============================================================================
//=>                microDRUM/nanoDRUM firmware v1.4.0                        <= 
//=>                        www.microdrum.net                                 <=
//=>                         CC BY-NC-SA 3.0                                  <=
//=>                                                                          <=
//=> Massimo Bernava                                                          <=
//=> massimo.bernava@gmail.com                                                <=
//=> 2016-02-10                                                               <=
//=>                                                                          <=
//=> MEGA adaptations by (ADAPTADO PARA O ARDUINO MEGA SEM USO DOS MULTIPLEX) <=
//=> PEDRO HENRIQUE KNUP                MARCOS CESAR MONECCHI                 <=
//=> facebook.com/pedroknup             22/06/2016                            <=
//=> 06/06/2016                         Revisão 0.4                           <=
//=>                                                                          <=
//=> Revisions for Teenssy/other add-ons by                                   <=
//=> Wesley Saunders                                                          <=
//=> Forum: WesleyS                                                           <=
//=> 3/29/2017                                                                <=
//==============================================================================


//========CONFIGURE=============
#define USE_LCD           1     // Use LCD
#define USE_595           0     // 1 with nanoDrum and v0.8
#define USE_I2C_LCD       1     // Serial LCD interface (20,4)
#define ENCODER           1     // Use rotary encoder + pushbutton instead of 2 pushbuttons, for faster naviagtion
#define USE_PROFILER      0     // Use Profiler (only for Debug)
#define FASTADC           1     // FASTADC = Prescaler_16, VERYFASTADC = Prescaler_8, VERYVERYFASTADC = Prescaler_4
#define SERIALSPEED       1     // 1 = 115200, 0 = 31250 (MIDI) , picoMIDI use 115200 therefore with nanoDrum and v0.8 use 1
#define USE_DEFAULT_NAME  1     // Use Default Name for pins in LCD Menu
#define USE_PISERIAL      0     // Use Raspberry Pi serial
#define USE_WAVTRIGGER    0     // Use WavTrigger
#define WT_16             0     // WT_16 = 16 Wav Trigger Input , WT_24 = 24 Wav Trigger Input
#define ENABLE_CHANNEL    1     // Enable MIDI Channel
#define MENU_LOG          0     // Enable Auto Tune (only with LCD)
#define NANO              0     // 1 = nanoDrum , 0 = microDrum
#define MEGA              0     // 1 = Usa Arduino Mega sem multiplex, 0 = microdrum
#define USE_DIG           0     // 1 = Usa os pinos digitais, 0 = Desabilitado
#define DEBUG             1     // Serial debug
#define TEENSY            1     // code optimized for teensy
#define SAM2695           0     // SAM2695 midi synthesizer on pin 8
//==============================

#if defined(__arm__) 
  #include <EEPROM.h> // Teensy uses EEPROM like normal
  #define HWSERIAL Serial1
#elif defined(__AVR__) 
  #include <EEPROM.h>
#endif

#if TEENSY
#include <Bounce.h>
int LEDpin = 13;
#endif

#if SAM2695
#include "Fluxamasynth.h"
Fluxamasynth synth;    // create a synth object
int fluxCHANNEL = 9;              // MIDI channel number
#endif

#if defined(__MK20DX256__)
#define RESTART_ADDR       0xE000ED0C
#define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))
#endif

#if defined(__AVR__) 
#if USE_WAVTRIGGER
  #include <SoftwareSerial.h>
#endif
#if USE_PISERIAL
  #include <SoftwareSerial.h>
#endif
#endif
//#include <math.h>

#if USE_LCD
  #if USE_I2C_LCD
    #if TEENSY  
    #include <i2c_t3.h>   
    #else
    #include <Wire.>
    #endif
    #include <LiquidCrystal_I2C.h>
  #elif USE_595
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
  #define fastNoteOn(_channel,_note,_velocity) { usbMIDI.sendNoteOn(_note, _velocity, 0x90 | _channel);          usbMIDI.sendNoteOff(_note, _velocity, 0x90 | _channel);   HWSERIAL.write(0x90 | _channel);  HWSERIAL.write(_note); HWSERIAL.write(_velocity);  Serial.println("channel---note---velocity "); Serial.print(_channel); Serial.print(_note); Serial.println(_velocity); Serial.println();}
  #define fastMidiCC(_channel,_number,_value) {  usbMIDI.sendControlChange(_number, _value, (_channel));                                                            HWSERIAL.write((0xB0 | _channel)); HWSERIAL.write(_number); HWSERIAL.write(_value);  Serial.println("channel---note---velocity "); Serial.print(_channel); Serial.print(_number); Serial.println(_value);Serial.println();}
#else
  #define fastNoteOn(_channel,_note,_velocity) { usbMIDI.sendNoteOn(_note, _velocity, 0x90 | 0x09);              usbMIDI.sendNoteOff(_note, _velocity, 0x90 | 0x09);        HWSERIAL.write(0x90 | 0x09); HWSERIAL.write(_note); HWSERIAL.write(_velocity);  Serial.println("channel---note---velocity "); Serial.print(0x90 | _channel); Serial.print(_note); Serial.println(_velocity); Serial.println();}
  #define fastMidiCC(_channel,_number,_value) {  usbMIDI.sendControlChange(_number, _value, (0xB0 | 0x09));                                                                 HWSERIAL.write((0xB0 | 0x09)); HWSERIAL.write(_number); HWSERIAL.write(_value); }
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
mode Mode=MIDI;
unsigned long GlobalTime;
//=================================

//===SETTING================
//===General================

const int delayTime=6;
byte GeneralXtalk=0;
#if NANO    // Verifica o modelo de arduino e seta o numero de pinos: NANO= 24, MEGA=16, OUTROS=48
const byte NPin=24;
#elif MEGA
const byte NPin=16;
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


  
#if MEGA
#if USE_DIG
 #if TEENSY

  // Define as notas do chocke //
  #define Choke1 78
  #define Choke2 80   
  #define Choke3 82
  #define ChokeRide 63
 
  // Define as Notas dos Plugs Digitais Auxiliares //
  #define Aux1 47
  #define Aux2 47
  #define Aux3 47
  #define Aux4 47
  #define Aux5 47
  #define Aux6 47
  #define Aux7 47

   //Define os Pinos Digitais dos Chokes
 #define Choke1_Pin 4 
 #define Choke2_Pin 5
 #define Choke3_Pin 6
 #define ChokeRide_Pin 7

 //Define os Pinos Digitais Auxiliares
 #define Aux1_Pin 8
 #define Aux2_Pin 9
 #define Aux3_Pin 10
 #define Aux4_Pin 11
 #define Aux5_Pin 12
 #define Aux6_Pin 31  // will work, but need to be changed
 #define Aux7_Pin 32  // will work, but need to be changed
 
 #include <Bounce.h>
  
  Bounce Choke_A = Bounce(Choke1_Pin, 5);
  Bounce Choke_B = Bounce(Choke2_Pin, 5);  // 5 = 5 ms debounce time
  Bounce Choke_C = Bounce(Choke3_Pin, 5);  // which is appropriate for good
  Bounce Choke_R = Bounce(ChokeRide_Pin, 5);  // quality mechanical pushbuttons

  Bounce AUX_A = Bounce(Aux1_Pin, 5);
  Bounce AUX_B = Bounce(Aux2_Pin, 5);  // if a button is too "sensitive"
  Bounce AUX_C = Bounce(Aux3_Pin, 5);  // to rapid touch, you can
  Bounce AUX_D = Bounce(Aux4_Pin, 5);  // increase this time.
  Bounce AUX_E = Bounce(Aux5_Pin, 5);
  Bounce AUX_F = Bounce(Aux6_Pin, 5);
  Bounce AUX_G = Bounce(Aux7_Pin, 5);

 #else

 // Define as notas do chocke //
 #define choke1 78
 #define choke2 80   
 #define choke3 82
 #define chokeRide 63
 
 // Define as Notas dos Plugs Digitais Auxiliares //
 #define Aux1 47
 #define Aux2 47
 #define Aux3 47
 #define Aux4 47
 #define Aux5 47
 #define Aux6 47
 #define Aux7 47                                  

 //Define os Pinos Digitais dos Chokes
 #define Choke1_Pin 4 
 #define Choke2_Pin 5
 #define Choke3_Pin 6
 #define ChokeRide_Pin 7

 //Define os Pinos Digitais Auxiliares
 #define Aux1_Pin 8
 #define Aux2_Pin 9
 #define Aux3_Pin 24
 #define Aux4_Pin 25
 #define Aux5_Pin 26
 #define Aux6_Pin 27
 #define Aux7_Pin 28

//Define o estado dos pinos digitais
 int Choke1_State = LOW;
 int Choke2_State = LOW; 
 int Choke3_State = LOW;
 int ChokeRide_State = LOW;
 int Aux1_State = LOW;
 int Aux2_State = LOW;
 int Aux3_State = LOW;
 int Aux4_State = LOW;
 int Aux5_State = LOW;
 int Aux6_State = LOW;
 int Aux7_State = LOW; 
 
 int currentSwitchState = LOW;
#endif
#endif

#if USE_LCD
//Define os caracteres especiais que serão apresentados na tela ao se tocar

//spChar define a matriz de 16 posições onde se deve colocar o tipo de sensor (kick, snare, etc)
//de acordo com o seu esquema

 #define chSnare  0
 #define chTom    1
 #define chCymbal 2
 #define chHH     3
 #define chKick   4
 
byte spChar[16] = {chHH,chSnare,chSnare,chKick,chTom,chTom,chTom,chTom,chCymbal,chCymbal,chHH,chCymbal,chCymbal,chCymbal,chCymbal,chCymbal};
long tmChar[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

byte lcdSnare[8] = {
  0b00000,
  0b01110,
  0b10001,
  0b11111,
  0b11011,
  0b11111,
  0b01110,
  0b00000
};
byte lcdTom[8] = {
  0b00000,
  0b01110,
  0b10001,
  0b10001,
  0b11011,
  0b11111,
  0b01110,
  0b00000
};
byte lcdCymball[8] = {
  0b00000,
  0b00001,
  0b01110,
  0b01100,
  0b01010,
  0b10010,
  0b00010,
  0b00000
};
byte lcdHH[8] = {
  0b00000,
  0b00100,
  0b11011,
  0b00100,
  0b11011,
  0b00100,
  0b00100,
  0b00000
};
byte lcdKick[8] = {
  0b00000,
  0b00100,
  0b00010,
  0b00001,
  0b00011,
  0b11101,
  0b11111,
  0b00000
};
#endif
#endif

