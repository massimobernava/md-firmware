 
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
//=> MEGA adaptations now named NO_MUX                                        <=
//=>                                                                          <=
//=> Revisions for Teensy by Wesley Saunders    Forum: WesleyS                <=
//==============================================================================

//========CONFIGURE=============
#define TEENSY            1     // Code optimized for Teensy
#define NANO              0     // 1 = nanoDrum , 0 = microDrum
#define NO_MUX            0     // 1 = Doesn't use multiplexers. Just set amount of analog inputs. See line 106 to set
#define USE_LCD           1     // Use LCD
#define USE_PROFILER      0     // Use Profiler (only for Debug)
#define FASTADC           0     // FASTADC = Prescaler_16, VERYFASTADC = Prescaler_8, VERYVERYFASTADC = Prescaler_4
#define SERIALSPEED       1     // 1 = 115200, 0 = 31250 (MIDI) , picoMIDI use 115200 therefore with nanoDrum and v0.8 use 1
#define USE_DEFAULT_NAME  1     // Use Default Name for pins in LCD Menu
#define USE_PISERIAL      0     // Use Raspberry Pi serial
#define USE_WAVTRIGGER    0     // Use WavTrigger
#define WT_16             0     // WT_16 = 16 Wav Trigger Input , WT_24 = 24 Wav Trigger Input
#define ENABLE_CHANNEL    0     // Enable MIDI Channel
#define USE_DIG           0     // Use digital pins as chokes/digital inputs. Must assign correct pins 
#define DEBUG             0     // Serial debug
#define SAM2695           0     // SAM2695 midi synthesizer on pin 8
//==============================

#if USE_LCD
#define MENU_LOG          0     // Enable Auto Tune (only with LCD)
#define USE_595           0     // 1 with nanoDrum and v0.8
#define USE_I2C_LCD       1     // Serial LCD interface (20,4)
#define ENCODER           1     // Use rotary encoder + pushbutton instead of 2 pushbuttons, for faster naviagtion
                                //If USE_595 and USE_I2C_LCD are both 0, normal LCD is used.
  #if USE_I2C_LCD
    #if TEENSY
      #include <LiquidCrystal_I2C_T.h>
        #else
      #include <LiquidCrystal_I2C.h>
    #endif
  #elif USE_595
    #include "LiquidCrystal595.h"
  #else
    #include <LiquidCrystal.h>
  #endif    
#endif

#if SAM2695
#include "Fluxamasynth.h"
Fluxamasynth synth;    // create a synth object
int fluxCHANNEL = 9;              // MIDI channel number
#endif

#if TEENSY
#define HWSERIAL Serial1
#endif

#if defined(__AVR__) 
#if USE_WAVTRIGGER
  #include <SoftwareSerial.h>
#endif
#if USE_PISERIAL
  #include <SoftwareSerial.h>
#endif
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

int delayTime=20;
byte GeneralXtalk=0;
#if NANO
const byte NPin=24;
#elif NO_MUX
const byte NPin=10; //Set to number of analog inputs on microcontroller
#elif TEENSY
const byte NPin=48;
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
