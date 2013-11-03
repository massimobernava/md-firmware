//=====================================================================================
//=>            microDRUM firmware v1.1 beta3
//=>              www.microdrum.net
//=>               CC BY-NC-SA 3.0
//=>
//=> Massimo Bernava
//=> massimo.bernava@gmail.com
//=> 2013-11-2
//=====================================================================================

//========CONFIGURE=============
#define MENU 0
#define VERYVERYFASTADC 1
//#define FASTADC
#define RASPBERRY 0
//Da 1 a 9
#define BUTTERWORTH 6
//==============================

#if defined(__arm__) 

// Arduino Due Board follows

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

//===========PROFILING============
#define PROFA TimeProfA=micros();
#define PROFB TimeProf+=(micros()-TimeProfA); NProf++;

unsigned long TimeProfA;
unsigned long TimeProf=0;
unsigned long NProf=0;
//=================================

#define fastCheckMulti(n1,n2) if(TypeSensor[n2+(n1<<3)]!=127) CheckMulti(n1,n2)

#if defined(__arm__) 
#define fastWrite(_pin_, _state_) digitalWrite(_pin_, _state_);
#elif defined(__AVR__) 
#define fastWrite(_pin_, _state_) ( _pin_ < 8 ? (_state_ ?  PORTD |= 1 << _pin_ : PORTD &= ~(1 << _pin_ )) : (_state_ ?  PORTB |= 1 << (_pin_ -8) : PORTB &= ~(1 << (_pin_ -8)  )))
#endif
#define fastNoteOn(_channel,_note,_velocity) {Serial.write(0x90 | _channel);Serial.write(_note);Serial.write(_velocity);}

unsigned long Time;


//High pass butterworth filter order=1 alpha1=0.06 
class filter
{
	public:
		filter()
		{
			v[0]=0;
			v[1]=0;
		}
	private:
		short v[2];
	public:
		short step(short x)
		{
			v[0] = v[1];
                        
                        //0.01
                        #if BUTTERWORTH==1                        
       			long tmp = ((((x * 2033254L) >>  1)	//= (   9.6953125291e-1 * x)
				+ ((v[0] * 1969357L) >> 1)	//+(  0.9390625058*v[0])
				)+524288) >> 20; // round and downshift fixed point /1048576 
                        //0.02
                        #elif BUTTERWORTH==2
          		long tmp = ((((x * 1973020L) >>  1)	//= (   9.4080929618e-1 * x)
          			+ ((v[0] * 1848888L) >> 1)	//+(  0.8816185924*v[0])
				)+524288) >> 20; // round and downshift fixed point /1048576

                        //0.03
                        #elif BUTTERWORTH==3
        		long tmp = ((((x * 1916034L) >>  1)	//= (   9.1363597299e-1 * x)
        			+ ((v[0] * 1734915L) >> 1)	//+(  0.8272719460*v[0])
				)+524288) >> 20; // round and downshift fixed point /1048576

                        //0.04
                        #elif BUTTERWORTH==4
                        long tmp = ((((x * 1861935L) >>  1)	//= (   8.8783975552e-1 * x)
				+ ((v[0] * 1626718L) >> 1)	//+(  0.7756795110*v[0])
				)+524288) >> 20; // round and downshift fixed point /1048576

                        //0.05
                        #elif BUTTERWORTH==5
                        long tmp = ((((x * 1810411L) >>  1)	//= (   8.6327126400e-1 * x)
				+ ((v[0] * 1523670L) >> 1)	//+(  0.7265425280*v[0])
				)+524288) >> 20; // round and downshift fixed point /1048576

                        //0.06
                        #elif BUTTERWORTH==6
			long tmp = ((((x * 1761188L) >>  1)	//= (   8.3979964911e-1 * x)
				+ ((v[0] * 1425223L) >> 1)	//+(  0.6795992982*v[0])
				)+524288) >> 20; // round and downshift fixed point /1048576

                        //0.07
                        #elif BUTTERWORTH==7
                        long tmp = ((((x * 1714023L) >>  1)	//= (   8.1730964877e-1 * x)
				+ ((v[0] * 1330893L) >> 1)	//+(  0.6346192975*v[0])
				)+524288) >> 20; // round and downshift fixed point /1048576
                        
                        //0.08
                        #elif BUTTERWORTH==8
                        long tmp = ((((x * 1668702L) >>  1)	//= (   7.9569917570e-1 * x)
				+ ((v[0] * 1240252L) >> 1)	//+(  0.5913983514*v[0])
				)+524288) >> 20; // round and downshift fixed point /1048576

                        //0.09
                        #elif BUTTERWORTH==9
                        long tmp = ((((x * 1625036L) >>  1)	//= (   7.7487732610e-1 * x)
				+ ((v[0] * 1152919L) >> 1)	//+(  0.5497546522*v[0])
				)+524288) >> 20; // round and downshift fixed point /1048576
                        #endif
                        
			v[1]= (short)tmp;
			return (short)((
				 (v[1] - v[0]))); // 2^
		}
};


//License
byte LicenseData[]={0,0};

//Log
byte LogPin=0xFF;
byte LogThresold=0xFF;
int N=0;//Unsent log
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
byte NXtalkGroup=8;//Max number XTalk groups
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

//int yn_1[]= {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};
//int yn_2[]= {0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0  ,0,0,0,0,0,0,0,0};

filter f[NPin];

//===========Pearson================== 
const byte Permutation[] = { 0x72, 0x32 , 0x25 , 0x64 , 0x64 , 0x4f , 0x1e , 0x26 , 0x2a , 0x74 , 0x37 , 0x09 , 0x57 , 0x02 , 0x28 , 0x08 , 0x14 , 0x23 , 0x49 , 0x10 , 0x62 , 0x02 , 0x1e , 0x7e , 0x5d , 0x1b , 0x27 , 0x76 , 0x7a , 0x76 , 0x05 , 0x2e };
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

  LicenseData[0]=random(128);
  LicenseData[1]=random(128);
  
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
  #endif
  
  #if MENU
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("microDRUM v1.1");
  #endif
  
  fastWrite(3,0);fastWrite(4,0);
  
}

