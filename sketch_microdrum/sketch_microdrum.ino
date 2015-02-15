
//=====================================================================================
//=>            microDRUM firmware v1.2.3
//=>              www.microdrum.net
//=>               CC BY-NC-SA 3.0
//=>
//=> Massimo Bernava
//=> massimo.bernava@gmail.com
//=> 2014-11-29
//=====================================================================================

//========CONFIGURE=============
#define MENU 1
#define PROF 0
#define FASTADC 1
#define SERIALSPEED 0
//Dopo andrà in Thresold
#define HHCTHRESOLD 10
#define LICENSE 0
#define WAVTRIGGER 1
#define WT_16 1
//#define WT_24 1
#define ENABLE_CHANNEL 0
#define MENU_LOG 1
//==============================

#if defined(__arm__) 
/* Use 24LC256 EEPROM to save settings */
#include <Wire.h>
#elif defined(__AVR__) 
#include <EEPROM.h>
#endif
#if WAVTRIGGER
#include <SoftwareSerial.h>
#endif
//#include <math.h>

#if MENU
#include <LiquidCrystal.h>
#endif

//===========MODE============
#define OffMode     0
#define StandbyMode 1
#define MIDIMode    2
#define ToolMode    3
//============================

//===========SETTING============
#define NOTE       0x00
#define THRESOLD   0x01
#define SCANTIME   0x02
#define MASKTIME   0x03
#define RETRIGGER  0x04
#define CURVE      0x05
#define XTALK      0x06
#define XTALKGROUP 0x07
#define CURVEFORM  0x08
#define CHOKENOTE  0x09
#define DUAL       0x0A
#define TYPE       0x0D
#define CHANNEL    0x0E
//===============================

//===========TYPE============
#define PIEZO    0
#define SWITCH   1
#define HHC      2
#define HH       3
#define HHS      4
#define YSwitch  5
#define Disabled 127
//============================

//===========TIME============
#define NORMAL_TIME        0
#define SCAN_TIME          1
#define MASK_TIME          2
#define RETRIGGER_TIME     3
#define PIEZO_TIME         4
#define SWITCH_TIME        5
#define CHOKE_TIME         6
#define FOOTSPLASH_TIME    7
#define FOOTCLOSE_TIME     8
#define SCANRETRIGGER_TIME 9
//===========================



// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define TIMEFUNCTION millis() //NOT micros() (thresold error)

#define fastCheckMulti(n1,n2) if(TypeSensor[n2+(n1<<3)]!=Disabled) { CheckMulti(n1,n2); if(StateSensor[n2+(n1<<3)]==SCAN_TIME) { CheckMulti(n1,n2); CheckMulti(n1,n2); CheckMulti(n1,n2);}}


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

#define DualSensor(i) (_DualSensor[(i)&0x07]+((i)&0xF8))
//127=Disabled
const byte _DualSensor[]    = {3,2,1,0,6,7,4,5};

unsigned long Time;

#if LICENSE
byte LicenseData[]={0,0};
const byte MaxNSensor=6;
#endif

//==========LOG====================
byte LogPin=0xFF;
byte LogThresold=0xFF;

short N=0;//Unsent log

bool Diagnostic=false;

#if MENU_LOG
byte log_state=0;

byte log_Nmax=0;  //Max noise


unsigned long log_T1=0; //Start hit time
//unsigned long log_T2=0;
unsigned long log_Tmax=0;  //Max hit time
int log_Vmax=0;   //Max hit value


//byte log_note=0;
//byte log_oldState=0;
//short log_Show=0;

/*unsigned long log_T80=0;
unsigned long log_T70=0;
unsigned long log_T60=0;*/
unsigned long log_T50=0;  //Half value hit time
/*unsigned long log_T40=0;
unsigned long log_T30=0;
unsigned long log_T20=0;*/

unsigned long d_hsum=0;  //Sum Tmax
unsigned long d_tsum=0;  //Sum T50
unsigned long d_hsum2=0;  //Sum Tmax^2
unsigned long d_tsum2=0;  //Sum T50^2
byte d_tnum=0;  //Hit number
byte d_rmin=0;
int d_vmax=0; //Total max hit value
int d_vmin=1024;  //Total min hit value
int d_vmean=0;  //Total min hit value

#endif

//=================================

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
const int delayTime=10;
byte GeneralXtalk=0;

const byte NPin=48;
byte NSensor=2;
//===========================

//===HiHat==================
byte HHNoteSensor[]     = {20,50,80,100};
byte HHThresoldSensor[] = {48,36,24,12};
byte HHFootNoteSensor[] = {59,48};
byte HHFootThresoldSensor[] = {127,127};
//===========================

//===Xtalk===================
const byte NXtalkGroup=8;//Max number XTalk groups
int MaxXtalkGroup[] = {-1,-1,-1,-1,-1,-1,-1,-1};
int MaxMultiplexerXtalk[]={-1,-1,-1,-1,-1,-1,-1,-1};
//int XtalkThresold[]  = {8,7,6,5,4,3,2,1};
//===========================

                   //PIN 00,01,02,03,04,05,06,07,  08,09,10,11,12,13,14,15,  16,17,18,19,20,21,22,23,   24,25,26,27,28,29,30,  31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47
byte NoteSensor[]     = {35,36,44,38,71,72,69,75,  67,68,65,67,7 ,71,46,79,  38,42,45,62,69,71, 4,75,   38,42,65,67,69,71,73,  75,38,42,65,67,69,71,73,75,38,42,65,67,69,71,73,75};
byte ThresoldSensor[] = {35,35,80,65,35,35,35,80,  35,35,35,35,99,35,35,80,  80,35,40,40,99,35,35,35,   35,35,35,35,35,35,255, 35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35};
byte ScanTimeSensor[] = {25,25,25,40,25,25,25,25,  25,25,25,25,60,25,25,25,  25,25,25,25,25,25,25,25,   25,25,25,25,25,25,25,  25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25};
byte MaskTimeSensor[] = {30,30,30,50,30,30,30,30,  30,30,30,30,99,30,30,30,  30,30,30,30,30,30,30,30,   30,30,30,30,30,30,30,  30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
byte RetriggerSensor[]= {55,55,55,50,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte CurveSensor[]    = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte CurveFormSensor[]   = {128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128,  128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128  ,128,128,128,128,128,128,128,128};
byte XtalkSensor[]       = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte XtalkGroupSensor[]  = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};

//0=Piezo 1=Switch 2=HHC 3=HH 4=HHs 5=YSwitch 127=Disabled
byte TypeSensor[]  = {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,2,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
byte ChokeNoteSensor[]   = {127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,78  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127  ,127,127,127,127,127,127,127,127};

#if ENABLE_CHANNEL
byte ChannelSensor[]=     {9,9,9,9,9,9,9,9                ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9                  ,9,9,9,9,9,9,9,9};
#endif

byte StateSensor[NPin];
unsigned long TimeSensor[NPin];
int MaxReadingSensor[NPin];

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
#define HOLDDELAY 500
#define DEBOUNCEDELAY 50
byte eMenuSelect=0;
byte eMenuPage=0;
byte eMenuGeneral=0;
byte eMenuPin=0;
//byte eMenuLog=0;
byte btnB_State=0;
byte btnA_State=0;
unsigned long btnA_Time=0;
unsigned long btnB_Time=0;
//byte btnA_Click=0;
//bool Changed=true;

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

/*byte level1[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B11111
};*/

#endif
//==============================

#if WAVTRIGGER

#define FAT_STACKS 0
#define BOXER 1
#define BOMBASTIX 2

byte kit=2;

SoftwareSerial mySerial(6, 5); // RX, TX
#endif

//==============================
//     SETUP
//==============================
void setup()
{

  #if LICENSE
  randomSeed(millis());
  LicenseData[0]=random(128);
  LicenseData[1]=random(128);
  #endif

  pinMode(2, OUTPUT);    // s0
  pinMode(3, OUTPUT);    // s1
  pinMode(4, OUTPUT);    // s2

  Time=TIMEFUNCTION;
  for (int count=0; count<NPin; count++)
  {
    TimeSensor[count]=Time+MaskTimeSensor[count];
    MaxReadingSensor[count]=-1;
    //MaxRetriggerSensor[count]=0;//0xFF;
  }

  #if MENU
  pinMode(7, INPUT);
  pinMode(6, INPUT);
  #endif
  
  #if SERIALSPEED
  Serial.begin(115200);    //Serial
  #else
  Serial.begin(31250);      // MIDI
  #endif
  
  Serial.flush();
  
  #if WAVTRIGGER
  mySerial.begin(57600);//31250);
  #endif
  
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
  MenuString(PSTR("microDRUM v1.2"),false);
  #endif
  
  fastWrite(3,0);fastWrite(4,0);
  
}


