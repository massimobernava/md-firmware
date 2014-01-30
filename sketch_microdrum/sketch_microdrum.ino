//=====================================================================================
//=>            microDRUM firmware v1.1 beta8
//=>              www.microdrum.net
//=>               CC BY-NC-SA 3.0
//=>
//=> Massimo Bernava
//=> massimo.bernava@gmail.com
//=> 2013-01-30
//=====================================================================================

//========CONFIGURE=============
#define MENU 0
#define PROF 0
#define VERYFASTADC 1
#define RASPBERRY 0
//Dopo andrà in Thresold
#define HHCTHRESOLD 10
#define TEST 0
#define LICENSE 1
//==============================

#if defined(__arm__) 
/* Use 24LC256 EEPROM to save settings */
#include <Wire.h>
#elif defined(__AVR__) 
#include <EEPROM.h>
#endif

#include <math.h>

#if MENU
#include <LiquidCrystal.h>
#endif

#define OffMode     0
#define StandbyMode 1
#define MIDIMode    2
#define ToolMode    3

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define TIMEFUNCTION millis() //NOT micros() (thresold error)

#define fastCheckMulti(n1,n2) if(TypeSensor[n2+(n1<<3)]!=127) { CheckMulti(n1,n2); if(MaxReadingSensor[n2+(n1<<3)]>0) { CheckMulti(n1,n2); CheckMulti(n1,n2); CheckMulti(n1,n2);}}

#if defined(__arm__) 
#define fastWrite(_pin_, _state_) digitalWrite(_pin_, _state_);
#elif defined(__AVR__) 
//#define fastWrite(_pin_, _state_) ( _pin_ < 8 ? (_state_ ?  PORTD |= 1 << _pin_ : PORTD &= ~(1 << _pin_ )) : (_state_ ?  PORTB |= 1 << (_pin_ -8) : PORTB &= ~(1 << (_pin_ -8)  )))
#define fastWrite(_pin_, _state_) (_state_ ?  PORTD |= 1 << _pin_ : PORTD &= ~(1 << _pin_ ))
#endif
#define fastNoteOn(_channel,_note,_velocity) {Serial.write(0x90 | _channel);Serial.write(_note);Serial.write(_velocity);}

unsigned long Time;

#if LICENSE
byte LicenseData[]={0,0};
const byte MaxNSensor=6;
#endif

//Log
byte LogPin=0xFF;
byte LogThresold=0xFF;
int N=0;//Unsent log
bool Diagnostic=false;

//Mode
byte Mode=OffMode;//StandbyMode;//

//===========PROFILING============
#define PROFA TimeProfA=micros();
#define PROFB TimeProf+=(micros()-TimeProfA); NProf++;
#if PROF
unsigned long TimeProfA;
unsigned long TimeProf=0;
unsigned long NProf=0;
#endif
//=================================

//===General================
const int delayTime=20;
byte GeneralXtalk=0;

const byte NPin=48;
byte NSensor=2;
//===========================

//===HiHat==================
byte HHNoteSensor[]     = {20,50,80,100};
byte HHThresoldSensor[] = {54,55,56,57};
byte HHFootNoteSensor[] = {59,48};
byte HHFootThresoldSensor[] = {127,127};
//===========================

//===Xtalk===================
byte NXtalkGroup=8;//Max number XTalk groups
byte MaxXtalkGroup[] = {255,255,255,255,255,255,255,255};
int XtalkThresold[]  = {8,7,6,5,4,3,2,1};
//===========================

                   //PIN 00,01,02,03,04,05,06,07,  08,09,10,11,12,13,14,15,  16,17,18,19,20,21,22,23,   24,25,26,27,28,29,30,  31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47
byte NoteSensor[]     = {35,36,44,38,71,72,69,75,  67,68,65,67,7 ,71,46,79,  38,42,45,62,69,71, 4,75,   38,42,65,67,69,71,73,  75,38,42,65,67,69,71,73,75,38,42,65,67,69,71,73,75};
byte ThresoldSensor[] = {35,35,80,65,35,35,35,80,  35,35,35,35,99,35,35,80,  80,35,40,40,99,35,35,35,   35,35,35,35,35,35,255, 35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35};
byte ScanTimeSensor[] = {25,25,25,40,25,25,25,25,  25,25,25,25,60,25,25,25,  25,25,25,25,25,25,25,25,   25,25,25,25,25,25,25,  25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25};
byte MaskTimeSensor[] = {30,30,30,50,30,30,30,30,  30,30,30,30,99,30,30,30,  30,30,30,30,30,30,30,30,   30,30,30,30,30,30,30,  30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
byte RetriggerSensor[]= {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte CurveSensor[]    = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte CurveFormSensor[]   = {128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128,  128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128};
byte XtalkSensor[]       = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte XtalkGroupSensor[]  = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};

//0=Piezo 1=Switch 2=HHC 3=HH 4=HHs 5=YSwitch 127=Disabled
byte TypeSensor[]  = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,2,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte ChokeNoteSensor[]   = {127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,78  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127};
//127=Disabled
byte DualSensor[]    = {127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127};
//byte DualNoteSensor[]=  {0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0};
//byte DualThresoldSensor[]={0,0,0,0,0,0,0,0                ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0};
byte ChannelSensor[]=     {9,9,9,9,9,9,9,9                ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9};

byte ZeroCountSensor[]  = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
unsigned long TimeSensor[]= {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
int MaxReadingSensor[] = {-1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1};
int MaxRetriggerSensor[]= {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};

int yn_1[NPin];

//===========Pearson================== 
#if LICENSE
const byte Permutation[] = { 0x72, 0x32 , 0x25 , 0x64 , 0x64 , 0x4f , 0x1e , 0x26 , 0x2a , 0x74 , 0x37 , 0x09 , 0x57 , 0x02 , 0x28 , 0x08 , 0x14 , 0x23 , 0x49 , 0x10 , 0x62 , 0x02 , 0x1e , 0x7e , 0x5d , 0x1b , 0x27 , 0x76 , 0x7a , 0x76 , 0x05 , 0x2e };
#endif
//====================================

//==============================
//    MENU
//==============================
#if MENU
int eMenuChange=0;
int eMenuValue=0;
int eMenuGeneral=0;
int eMenuPin=0;
int eMenuLog=0;
int btnOk_Last=LOW;
int btnA_Last=LOW;
int btnB_Last=LOW;
bool Changed=true;

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

byte *getChar(int n, byte newChar[]) 
{
  int i;
  
  for (i = 0; i < 7-n; i++)
    newChar[i] = B00000;
    
  for (i = 7-n; i < 8; i++)
    newChar[i] = B11111;
  return newChar;

}
#endif
//==============================

//==============================
//     SETUP
//==============================
void setup()
{

  //LicenseData[0]=random(128);
  //LicenseData[1]=random(128);
  #if TEST
    simpleSysex(0x77,0x01,0x01,0x01);
  #endif
  
  #if MENU
  byte newChar[8];
  for (int i = 0; i < 8; i++)
    lcd.createChar(i, getChar(i, newChar));
  #endif

  pinMode(2, OUTPUT);    // s0
  pinMode(3, OUTPUT);    // s1
  pinMode(4, OUTPUT);    // s2

  Time=TIMEFUNCTION;
  for (int count=0; count<NPin; count++)
    TimeSensor[count]=Time+MaskTimeSensor[count];

  #if MENU
  pinMode(7, INPUT);
  pinMode(6, INPUT);
  pinMode(5, INPUT);
  #endif
  
  #if RASPBERRY
  Serial.begin(115200);    //Raspberry Pi
  #else
  Serial.begin(31250);      // MIDI
  #endif
  
  Serial.flush();
  
  #if defined(__AVR__) 
  analogReference(DEFAULT);
  #endif
  
  LoadAllEEPROM();
  
  #if defined(__AVR__)
  #if FASTADC
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  #endif
  
  #if VERYFASTADC
  // set prescale to 8
  cbi(ADCSRA,ADPS2) ;
  sbi(ADCSRA,ADPS1) ;
  sbi(ADCSRA,ADPS0) ;
  #endif
  
  #if VERYVERYFASTADC
  // set prescale to 4
  cbi(ADCSRA,ADPS2) ;
  sbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  #endif
  #elif defined(__arm__) 
  REG_ADC_MR = (REG_ADC_MR & 0xFFF0FFFF) | 0x00030000; //0x00020000;
  #endif
  
  #if MENU
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("microDRUM v1.1");
  #endif
  
  fastWrite(3,0);fastWrite(4,0);
  
}


