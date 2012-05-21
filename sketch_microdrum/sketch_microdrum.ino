//=====================================================================================
//=>            microDRUM firmware v1.1 beta1
//=>              www.microdrum.net
//=>               CC BY-NC-SA 3.0
//=>
//=> Massimo Bernava
//=> massimo.bernava@gmail.com
//=> 2012-5-21
//=====================================================================================

#include <EEPROM.h>
#include <math.h>
#include <LiquidCrystal.h>

#define OffMode     0
#define StandbyMode 1
#define MIDIMode    2
#define ToolMode    3

#define VERYVERYFASTADC 1
//#define FASTADC 1

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define TIMEFUNCTION millis()

//===========PROFILING============
#define PROFA TimeProfA=micros();
#define PROFB TimeProf+=(micros()-TimeProfA); NProf++;

unsigned long TimeProfA;
unsigned long TimeProf=0;
unsigned long NProf=0;
//=================================

#define fastWrite(_pin_, _state_) ( _pin_ < 8 ? (_state_ ?  PORTD |= 1 << _pin_ : PORTD &= ~(1 << _pin_ )) : (_state_ ?  PORTB |= 1 << (_pin_ -8) : PORTB &= ~(1 << (_pin_ -8)  )))
#define fastNoteOn(_channel,_note,_velocity) {Serial.write(0x90 | _channel);Serial.write(_note);Serial.write(_velocity);}

unsigned long Time;

//License
byte LicenseData[]={0,0};

//Log
byte LogPin=0xFF;
byte LogThresold=0xFF;
int N=0;
bool Diagnostic=false;

//Mode
byte Mode=OffMode;

//===General================
int delayTime=20;
byte GeneralXtalk=0;

const byte NPin=48;
byte NSensor=2;
byte MaxNSensor=6;
//===========================

//===HiHat==================
byte HHNoteSensor[]     = {20,50,80,100};
byte HHThresoldSensor[] = {54,55,56,57};
byte HHFootNoteSensor[] = {59,48};
byte HHFootThresoldSensor[] = {127,127};
//===========================

//===Xtalk===================
byte NXtalkGroup=8;//Numero massimo di gruppi per XTalk
byte MaxXtalkGroup[] = {255,255,255,255,255,255,255,255};
int XtalkThresold[]  = {8,7,6,5,4,3,2,1};
//===========================

                   //PIN 00,01,02,03,04,05,06,  07,08,09,10,11,12  ,13,14,15,16,17,18,19,20,21,  22,23,24,25,26,27,28,29,30,  31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47
byte NoteSensor[]     = {35,36,44,38,71,72,69,  75,67,68,65,67,7   ,71,46,79,38,42,45,62,69,71,   4,75,38,42,65,67,69,71,73,  75,38,42,65,67,69,71,73,75,38,42,65,67,69,71,73,75};
byte ThresoldSensor[] = {35,35,80,65,35,35,35, 80,35,35,35,35,255 ,35,35,80,80,35,40,40,200,35,  35,35,35,35,35,35,35,35,255, 35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35};
byte ScanTimeSensor[] = {25,25,25,40,25,25,25,  25,25,25,25,25,60  ,25,25,25,25,25,25,25,25,25,   25,25,25,25,25,25,25,25,25,  25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25};
byte MaskTimeSensor[] = {30,30,30,50,30,30,30,  30,30,30,30,30,190 ,30,30,30,30,30,30,30,30,30,  30,30,30,30,30,30,30,30,30,  30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
byte RetriggerSensor[]= {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte CurveSensor[]    = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte CurveFormSensor[]   = {128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128,  128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128};
byte XtalkSensor[]       = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte XtalkGroupSensor[]  = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};

//0=Piezo 1=Switch 2=HHC 3=HH 4=HHs 5=YSwitch 127=Disabled
byte TypeSensor[]  = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,2,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte ChokeNoteSensor[]   = {127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,78  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127};
//127=Disabled
byte DualSensor[]    = {127,127,3,2,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127};
byte DualNoteSensor[]=  {0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0};
byte DualThresoldSensor[]={0,0,0,0,0,0,0,0                ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0                  ,0,0,0,0,0,0,0,0};
byte ChannelSensor[]=     {9,9,9,9,9,9,9,9                ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9};

byte ZeroCountSensor[]  = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
unsigned long TimeSensor[]= {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
int MaxReadingSensor[] = {-1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1};
int MaxRetriggerSensor[]= {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};

int yn_1[]= {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
int yn_2[]= {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};

//===========Pearson================== 
const byte Permutation[] = { 0x72, 0x32 , 0x25 , 0x64 , 0x64 , 0x4f , 0x1e , 0x26 , 0x2a , 0x74 , 0x37 , 0x09 , 0x57 , 0x02 , 0x28 , 0x08 , 0x14 , 0x23 , 0x49 , 0x10 , 0x62 , 0x02 , 0x1e , 0x7e , 0x5d , 0x1b , 0x27 , 0x76 , 0x7a , 0x76 , 0x05 , 0x2e };
//====================================

//==============================
//    MENU
//==============================
bool menuEnabled=false;
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
//==============================

//==============================
//     SETUP
//==============================
void setup()
{
  
  LicenseData[0]=random(128);
  LicenseData[1]=random(128);
  
  byte newChar[8];
  for (int i = 0; i < 8; i++)
    lcd.createChar(i, getChar(i, newChar));


  pinMode(2, OUTPUT);    // s0
  pinMode(3, OUTPUT);    // s1
  pinMode(4, OUTPUT);    // s2

  Time=TIMEFUNCTION;
  for (int count=0; count<NPin; count++)
    TimeSensor[count]=Time+MaskTimeSensor[count];

  //pinMode(ledPin, OUTPUT);  // Disabled with LCD
  pinMode(7, INPUT);
  pinMode(6, INPUT);
  pinMode(5, INPUT);
  
  Serial.begin(31250);      // MIDI
  Serial.flush();
  
  analogReference(DEFAULT);
  //digitalWrite(ledPin,LOW);
  
  LoadAllEEPROM();
  
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
  
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("microDRUM v1.1");
  
}

