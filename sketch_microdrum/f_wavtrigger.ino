
//==============================
//    WAVTRIGGER: http://robertsonics.com/wav-trigger/
//==============================
#if USE_WAVTRIGGER

#define MAX_POLYPHONIC 15
#define VOLUME_SCALE 3

const byte FAT_STACKS = 0;
const byte BOXER = 1;
const byte BOMBASTIX = 2;
const byte CUSTOM = 3;
const byte FIXED = 4;

byte kit=BOMBASTIX;

#if defined(__arm__) 
  #define mySerial Serial2
#elif defined(__AVR__) 
  SoftwareSerial mySerial(6, 5); // RX, TX
#endif

/*
A1 Snare         6/4
B1 Kick/CowBell  5/7
C1 HH            0/3
D1 HHC           2/1

A2 Ride          14/12
B2 Tom1/2        13/15
C2 Tom3/4        8/11
D2 Crash         10/9

A3 Ride2         22/20
B3 Ride2         21/23
C3 Crash2        16/19
D3 China/Splash  18/17
*/

//WAV TRIGGER PIN DEFAULT
#define WTP_SNAREHEAD 0x04
#define WTP_SNARERIM 0x06
#define WTP_KICK 0x07
#define WTP_EFFECT 0x05
#define WTP_HHBOW 0x03
#define WTP_HHEDGE 0x00
#define WTP_HHC 0x01
#define WTP_HHC_RING 0x02

//WAV TRIGGER PIN 16 INPUT
#if WT_16
#define WTP_RIDEBOW 0x0C
#define WTP_RIDEEDGE 0x0E
#define WTP_TOM1HEAD 0x0F
#define WTP_TOM2HEAD 0x0D
#define WTP_TOM3HEAD 0x0B
#define WTP_TOM4HEAD 0x08
#define WTP_CRASHBOW 0x09
#define WTP_CRASHEDGE 0x0A
#endif

//WAV TRIGGER PIN 24 INPUT
#if WT_24
#define WTP_RIDEBOW 0x0C
#define WTP_RIDEEDGE 0x0E
#define WTP_TOM1HEAD 0x0F
#define WTP_TOM1RIM 0x0D
#define WTP_TOM2HEAD 0x0B
#define WTP_TOM2RIM 0x08
#define WTP_CRASHBOW 0x09
#define WTP_CRASHEDGE 0x0A

#define WTP_TOM3HEAD
#define WTP_TOM3RIM
#define WTP_TOM4HEAD
#define WTP_TOM4RIM
#define WTP_CRASH2BOW
#define WTP_CRASH2EDGE
#define WTP_CHINA
#define WTP_SPLASH
#endif


//WAV TRIGGER SOUND
#define WTS_HHBOW_CLOSED 0x00
#define WTS_HHBOW_OPEN25 0x01
#define WTS_HHBOW_OPEN50 0x02
#define WTS_HHBOW_OPEN75 0x03
#define WTS_HHBOW_OPEN 0x04
#define WTS_HHEDGE_CLOSED 0x05
#define WTS_HHEDGE_OPEN25 0x06
#define WTS_HHEDGE_OPEN50 0x07
#define WTS_HHEDGE_OPEN75 0x08
#define WTS_HHEDGE_OPEN 0x09
#define WTS_HHFOOT 0x0A
#define WTS_HHSPLASH 0x0B
#define WTS_SNARE_HEAD 0x0C
#define WTS_SNARE_RIM 0x0D
#define WTS_KICK 0x0E
#define WTS_TOM1_HEAD 0x0F
#define WTS_TOM1_RIM 0x10
#define WTS_TOM2_HEAD 0x11
#define WTS_TOM2_RIM 0x12
#define WTS_TOM3_HEAD 0x13
#define WTS_TOM3_RIM 0x14
#define WTS_TOM4_HEAD 0x15
#define WTS_TOM4_RIM 0x16
#define WTS_RIDE_EDGE 0x17
#define WTS_RIDE_BOW 0x18
#define WTS_RIDE_BELL 0x19
#define WTS_CRASH1_EDGE 0x1A
#define WTS_CRASH1_BOW 0x1B
#define WTS_CRASH2_EDGE 0x1C
#define WTS_CRASH2_BOW 0x1D
#define WTS_EFFECT1 0x1E
#define WTS_EFFECT2 0x1F

//========================================
// GENERAL
//========================================
short Poly[MAX_POLYPHONIC];
byte LastPoly=0;
byte WT_HHC=0;

//========================================
// INTERFACE
//========================================
void wavChoke(byte pin) 
{
    switch(kit)
    {
      case FAT_STACKS: wavChoke_FS(pin); break;
      case BOXER: wavChoke_BX(pin); break;
      case BOMBASTIX: wavChoke_BM(pin); break;
      case CUSTOM: wavChoke_Custom(pin); break;
      case FIXED: wavChoke_Fixed(pin); break;
    }
}

void wavTriggerHHC(byte hhc) {
 
  if(hhc<12)
  {
    wavChoke(WTP_HHBOW); 
  }
  WT_HHC=hhc;
  
}

void wavTrigger(byte pin, byte velocity) {

    switch(kit)
    {
      case FAT_STACKS: wavTrigger_FS(pin,velocity);break;
      case BOXER: wavTrigger_BX(pin,velocity); break;
      case BOMBASTIX: wavTrigger_BM(pin,velocity); break;
      case CUSTOM: wavTrigger_Custom(pin,velocity); break;
      case FIXED: wavTrigger_Fixed(pin,velocity); break;
    }
}

short Pin2Sound(byte pin)
{
  switch(pin)
    {
      case WTP_KICK: return WTS_KICK;
      case WTP_EFFECT: return WTS_EFFECT1; 
      case WTP_SNAREHEAD: return WTS_SNARE_HEAD;
      case WTP_SNARERIM: return WTS_SNARE_RIM;
      case WTP_TOM1HEAD: return WTS_TOM1_HEAD;
      case WTP_TOM2HEAD: return WTS_TOM2_HEAD;
      case WTP_TOM3HEAD: return WTS_TOM3_HEAD;
      case WTP_TOM4HEAD: return WTS_TOM4_HEAD;
      
      case WTP_RIDEBOW: return WTS_RIDE_BOW;
      case WTP_RIDEEDGE: return WTS_RIDE_EDGE;
      case WTP_CRASHBOW: return WTS_CRASH1_BOW;
      case WTP_CRASHEDGE: return WTS_CRASH1_EDGE;
      case WTP_HHBOW: if(Pin[WTP_HHC].Type==HHC)
                      { 
                        if(WT_HHC>HHThresoldSensor[0]) { return WTS_HHBOW_OPEN;}
                        else if(WT_HHC>HHThresoldSensor[1]) { return WTS_HHBOW_OPEN75; }
                        else if(WT_HHC>HHThresoldSensor[2]) { return WTS_HHBOW_OPEN50; }
                        else if(WT_HHC>HHThresoldSensor[3]) { return WTS_HHBOW_OPEN25; }
                        else { return WTS_HHBOW_CLOSED;}
                      }
                      break;
      default: return 0;
      
    }
}
const byte dNOTEON=1;
const byte dDELAY=2;

const byte DEMO_LENGTH = 10;
const byte demo_song[] PROGMEM={ dNOTEON,WTP_SNAREHEAD,120,
                         dDELAY,255,
                         dNOTEON,WTP_KICK,120,
                         dDELAY,255};
void demo(byte repeat)
{
  while(repeat--)
  for(byte d=0;d<DEMO_LENGTH;d++)
  {
    if(pgm_read_byte_near(demo_song+d)==dNOTEON)
    {
      wavTrigger(pgm_read_byte_near(demo_song+d+1),pgm_read_byte_near(demo_song+d+2));
      d+=2;
    }
    else if(pgm_read_byte_near(demo_song+d)==dDELAY)
    {
      d++;
      delay(pgm_read_byte_near(demo_song+d));
    }
  }
  
  
}
//========================================
//Custom
//========================================

byte RR_VL[32]={
                   0x64,0x64,0x64,0x64,0x64,
                   0x64,0x64,0x64,0x64,0x64,
                   0x63,0x61, //HH
		   0x67,0x64, //SNARE
		   0x63,//KICK
		   0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,//TOM
		   0x63,0x63,0x63,//RIDE
		   0x63,0x63,//CRASH
		   0x63,0x63,//CRASH
		   0x63,0x63//EFFECT
                  };

void wavChoke_Custom(byte pin)
{
  //ferma tutti i suoni appartenetnti a quel pin, possibilmente con un fade
  /*byte from=0,to=0;
  switch(pin)
  {
    case WTP_CRASHEDGE: wavTriggerFade(StartWav_BX[WTS_CRASH1_EDGE],CountWav_BX[WTS_CRASH1_EDGE],ROUND_ROBIN_BX); wavTriggerFade(StartWav_BX[WTS_CRASH1_BOW],CountWav_BX[WTS_CRASH1_BOW],ROUND_ROBIN_BX); break;
    case WTP_RIDEEDGE:  wavTriggerFade(StartWav_BX[WTS_RIDE_EDGE],CountWav_BX[WTS_RIDE_EDGE],ROUND_ROBIN_BX); wavTriggerFade(StartWav_BX[WTS_RIDE_BOW],CountWav_BX[WTS_RIDE_BOW],ROUND_ROBIN_BX); break;
    case WTP_HHBOW: wavTriggerFade(StartWav_BX[WTS_HHBOW_OPEN],CountWav_BX[WTS_HHBOW_OPEN],ROUND_ROBIN_BX); wavTriggerFade(StartWav_BX[WTS_HHBOW_OPEN75],CountWav_BX[WTS_HHBOW_OPEN75],ROUND_ROBIN_BX); wavTriggerFade(StartWav_BX[WTS_HHBOW_OPEN50],CountWav_BX[WTS_HHBOW_OPEN50],ROUND_ROBIN_BX); wavTriggerFade(StartWav_BX[WTS_HHBOW_OPEN25],CountWav_BX[WTS_HHBOW_OPEN25],ROUND_ROBIN_BX);break;
  }*/
}

void wavTrigger_Custom(byte pin, byte velocity)
{
  byte sound=Pin2Sound(pin);
  if(RR_VL[sound]<0x10) return;
  
  short start=0;

  for(byte i=0;i<sound;i++)
    start+=(RR_VL[i]>>4)*(RR_VL[i]&0xF);
		
  byte rr=RR_VL[sound]>>4;
  byte vlevel=RR_VL[sound]&0x0F;
    
  //int n=start+(vlevel*velocity/128)+random(0,rr)*vlevel;//OLD SOUNd->RR->LEVEL
  int n=start+(vlevel*velocity/128)*rr+random(0,rr);//NEW SOUND->LEVEL->RR
  
  int vol= -0.5 + (float)((float)velocity-127.0)/VOLUME_SCALE;
  wavTriggerPlay(n,vol); 
}

//========================================
//Fixed
//========================================
const byte ROUND_ROBIN = 5;
const byte V_LEVEL = 6;

void wavChoke_Fixed(byte pin)
{
  
}

void wavTrigger_Fixed(byte pin, byte velocity)
{
  byte sound=Pin2Sound(pin);
  short start=sound*ROUND_ROBIN*V_LEVEL;
  
  int n=start+(V_LEVEL*velocity/128)+random(0,ROUND_ROBIN)*V_LEVEL;
    
  int vol= -0.5 + (float)((float)velocity-127.0)/VOLUME_SCALE;
  wavTriggerPlay(n,vol); 
}

//========================================
//Analogue Drums
//========================================

byte RR=0;  //General RoundRobin Index

//========================================
//Analogue Drums - Boxer
//========================================

const byte ROUND_ROBIN_BX= 6;

const short StartWav_BX[32]={361,265,289,337,313, //HH TT/TC/TL/TS/TO
                    217,121,145,193,169, //HH ET/EC/EL/ES/EO
                    247,241, //PD - FS
                    //457,499, //SNARE1 CN/XS
                    523,565, //SNARE2 CN/XS
                    //589,631, //SNARE3 CN/XS
                    385,//KICK
		    691,691,727,727,763,763,763,763,//TOM
		    439,439,421,//RIDE EG/BW/BL
		    37,1,//CRASH EG/BL
		    97,61,//CRASH EG/BL
		    655,673};//EFFECT

const byte CountWav_BX[32]={4,4,4,4,4,
                   4,4,4,4,4,
                   3,1, //HH
		   7,4, //SNARE
		   6,//KICK
		   6,6,6,6,6,6,6,6,//TOM
		   3,3,3,//RIDE
		   3,3,//CRASH
		   3,3,//CRASH
		   3,3};//EFFECT

void wavChoke_BX(byte pin)
{
  //ferma tutti i suoni appartenetnti a quel pin, possibilmente con un fade 
  switch(pin)
  {
    case WTP_CRASHEDGE: wavTriggerFade(StartWav_BX[WTS_CRASH1_EDGE],CountWav_BX[WTS_CRASH1_EDGE],ROUND_ROBIN_BX); wavTriggerFade(StartWav_BX[WTS_CRASH1_BOW],CountWav_BX[WTS_CRASH1_BOW],ROUND_ROBIN_BX); break;
    case WTP_RIDEEDGE:  wavTriggerFade(StartWav_BX[WTS_RIDE_EDGE],CountWav_BX[WTS_RIDE_EDGE],ROUND_ROBIN_BX); wavTriggerFade(StartWav_BX[WTS_RIDE_BOW],CountWav_BX[WTS_RIDE_BOW],ROUND_ROBIN_BX); break;
    case WTP_HHBOW: wavTriggerFade(StartWav_BX[WTS_HHBOW_OPEN],CountWav_BX[WTS_HHBOW_OPEN],ROUND_ROBIN_BX); wavTriggerFade(StartWav_BX[WTS_HHBOW_OPEN75],CountWav_BX[WTS_HHBOW_OPEN75],ROUND_ROBIN_BX); wavTriggerFade(StartWav_BX[WTS_HHBOW_OPEN50],CountWav_BX[WTS_HHBOW_OPEN50],ROUND_ROBIN_BX); wavTriggerFade(StartWav_BX[WTS_HHBOW_OPEN25],CountWav_BX[WTS_HHBOW_OPEN25],ROUND_ROBIN_BX);break;
  }
}

void wavTrigger_BX(byte pin, byte velocity)
{
    short start=0;
    byte count=0;
  
    //start=StartWav_BX[Pin2Sound(pin)];
    //count=CountWav_BX[Pin2Sound(pin)];
  
    switch(pin)
    {
      case WTP_KICK: start=StartWav_BX[WTS_KICK]; count=CountWav_BX[WTS_KICK]; break;
      case WTP_SNAREHEAD: start=StartWav_BX[WTS_SNARE_HEAD]; count=CountWav_BX[WTS_SNARE_HEAD]; break;
      case WTP_SNARERIM: start=StartWav_BX[WTS_SNARE_RIM]; count=CountWav_BX[WTS_SNARE_RIM]; break;
      case WTP_TOM1HEAD: start=StartWav_BX[WTS_TOM1_HEAD]; count=CountWav_BX[WTS_TOM1_HEAD]; break;
      case WTP_TOM2HEAD: start=StartWav_BX[WTS_TOM2_HEAD]; count=CountWav_BX[WTS_TOM2_HEAD]; break;
      case WTP_TOM3HEAD: start=StartWav_BX[WTS_TOM3_HEAD]; count=CountWav_BX[WTS_TOM3_HEAD]; break;
      case WTP_TOM4HEAD: start=StartWav_BX[WTS_TOM4_HEAD]; count=CountWav_BX[WTS_TOM4_HEAD]; break;
      
      #if WT_24
      case WTP_TOM1RIM: start=StartWav_BX[WTS_TOM1_RIM]; count=CountWav_BX[WTS_TOM1_RIM]; break;
      case WTP_TOM2RIM: start=StartWav_BX[WTS_TOM2_RIM]; count=CountWav_BX[WTS_TOM2_RIM]; break;
      #endif
      
      case WTP_RIDEBOW: start=StartWav_BX[WTS_RIDE_BOW]; count=CountWav_BX[WTS_RIDE_BOW]; break;
      case WTP_RIDEEDGE: start=StartWav_BX[WTS_RIDE_EDGE]; count=CountWav_BX[WTS_RIDE_EDGE]; break;
      case WTP_CRASHBOW: start=StartWav_BX[WTS_CRASH1_BOW]; count=CountWav_BX[WTS_CRASH1_BOW]; break;
      case WTP_CRASHEDGE: start=StartWav_BX[WTS_CRASH1_EDGE]; count=CountWav_BX[WTS_CRASH1_EDGE]; break;
      case WTP_HHBOW: if(Pin[WTP_HHC].Type==HHC)
                      { 
                        if(WT_HHC>48) {start=StartWav_BX[WTS_HHBOW_OPEN]; count=CountWav_BX[WTS_HHBOW_OPEN];}
                        else if(WT_HHC>36) {start=StartWav_BX[WTS_HHBOW_OPEN75]; count=CountWav_BX[WTS_HHBOW_OPEN75];}
                        else if(WT_HHC>24) {start=StartWav_BX[WTS_HHBOW_OPEN50]; count=CountWav_BX[WTS_HHBOW_OPEN50];}
                        else if(WT_HHC>12) {start=StartWav_BX[WTS_HHBOW_OPEN25]; count=CountWav_BX[WTS_HHBOW_OPEN25];}
                        else {start=StartWav_BX[WTS_HHBOW_CLOSED]; count=CountWav_BX[WTS_HHBOW_CLOSED];}
                      }
                      break;
      default: return;
      
    }
    //Analogue Drums
    int n=start+/*floor*/(count*velocity/128)+RR*count;
    int vol= -0.5 + (float)((float)velocity-127.0)/VOLUME_SCALE;
    wavTriggerPlay(n,vol);
    RR=(RR+1)%ROUND_ROBIN_BX;
    
    //2Box
    //int n=start+/*floor*/(count*velocity/127)/*+random*/;
}

//========================================
//Analogue Drums - Fat Stacks
//========================================

const byte ROUND_ROBIN_FS=7;

const short StartWav_FS[32]={526,365,400,484,442, //HH TT/TC/TL/TS/TO
                    295,127,169,253,211, //HH ET/EC/EL/ES/EO
                    344,337, //PD - FS
                    680,736, //SNARE
                    568,//KICK
		    792,792,834,834,876,876,918,918,//TOM
		    659,638,617,//RIDE EG/BW/BL
		    50,29,//CRASH EG/BL
		    99,78,//CRASH EG/BL
		    1,764};//EFFECT

const byte CountWav_FS[32]={6,5,6,6,6,
                   6,6,6,6,6,
                   3,1, //HH
		   8,4, //SNARE
		   7,//KICK
		   6,6,6,6,6,6,6,6,//TOM
		   3,3,3,//RIDE
		   3,3,//CRASH
		   3,3,//CRASH
		   4,4};//EFFECT

void wavChoke_FS(byte pin)
{
  //ferma tutti i suoni appartenetnti a quel pin, possibilmente con un fade 
  switch(pin)
  {
    case WTP_CRASHEDGE: wavTriggerFade(StartWav_FS[WTS_CRASH1_EDGE],CountWav_FS[WTS_CRASH1_EDGE],ROUND_ROBIN_FS); wavTriggerFade(StartWav_FS[WTS_CRASH1_BOW],CountWav_FS[WTS_CRASH1_BOW],ROUND_ROBIN_FS); break;
    case WTP_RIDEEDGE:  wavTriggerFade(StartWav_FS[WTS_RIDE_EDGE],CountWav_FS[WTS_RIDE_EDGE],ROUND_ROBIN_FS); wavTriggerFade(StartWav_FS[WTS_RIDE_BOW],CountWav_FS[WTS_RIDE_BOW],ROUND_ROBIN_FS); break;
    case WTP_HHBOW: wavTriggerFade(StartWav_FS[WTS_HHBOW_OPEN],CountWav_FS[WTS_HHBOW_OPEN],ROUND_ROBIN_FS); wavTriggerFade(StartWav_FS[WTS_HHBOW_OPEN75],CountWav_FS[WTS_HHBOW_OPEN75],ROUND_ROBIN_FS); wavTriggerFade(StartWav_FS[WTS_HHBOW_OPEN50],CountWav_FS[WTS_HHBOW_OPEN50],ROUND_ROBIN_FS); wavTriggerFade(StartWav_FS[WTS_HHBOW_OPEN25],CountWav_FS[WTS_HHBOW_OPEN25],ROUND_ROBIN_FS);break;
  }
}

void wavTrigger_FS(byte pin, byte velocity) 
{    
    short start=0;
    byte count=0;
  
    switch(pin)
    {
      case WTP_KICK: start=StartWav_FS[WTS_KICK]; count=CountWav_FS[WTS_KICK]; break;
      case WTP_SNAREHEAD: start=StartWav_FS[WTS_SNARE_HEAD]; count=CountWav_FS[WTS_SNARE_HEAD]; break;
      case WTP_SNARERIM: start=StartWav_FS[WTS_SNARE_RIM]; count=CountWav_FS[WTS_SNARE_RIM]; break;
      case WTP_TOM1HEAD: start=StartWav_FS[WTS_TOM1_HEAD]; count=CountWav_FS[WTS_TOM1_HEAD]; break;
      case WTP_TOM2HEAD: start=StartWav_FS[WTS_TOM2_HEAD]; count=CountWav_FS[WTS_TOM2_HEAD]; break;
      case WTP_TOM3HEAD: start=StartWav_FS[WTS_TOM3_HEAD]; count=CountWav_FS[WTS_TOM3_HEAD]; break;
      case WTP_TOM4HEAD: start=StartWav_FS[WTS_TOM4_HEAD]; count=CountWav_FS[WTS_TOM4_HEAD]; break;
      
      #if WT_24
      case WTP_TOM1RIM: start=StartWav_FS[WTS_TOM1_RIM]; count=CountWav_FS[WTS_TOM1_RIM]; break;
      case WTP_TOM2RIM: start=StartWav_FS[WTS_TOM2_RIM]; count=CountWav_FS[WTS_TOM2_RIM]; break;
      #endif
      
      case WTP_RIDEBOW: start=StartWav_FS[WTS_RIDE_BOW]; count=CountWav_FS[WTS_RIDE_BOW]; break;
      case WTP_RIDEEDGE: start=StartWav_FS[WTS_RIDE_EDGE]; count=CountWav_FS[WTS_RIDE_EDGE]; break;
      case WTP_CRASHBOW: start=StartWav_FS[WTS_CRASH1_BOW]; count=CountWav_FS[WTS_CRASH1_BOW]; break;
      case WTP_CRASHEDGE: start=StartWav_FS[WTS_CRASH1_EDGE]; count=CountWav_FS[WTS_CRASH1_EDGE]; break;
      case WTP_HHBOW: if(Pin[WTP_HHC].Type==HHC)
                      { 
                        if(WT_HHC>48) {start=StartWav_FS[WTS_HHBOW_OPEN]; count=CountWav_FS[WTS_HHBOW_OPEN];}
                        else if(WT_HHC>36) {start=StartWav_FS[WTS_HHBOW_OPEN75]; count=CountWav_FS[WTS_HHBOW_OPEN75];}
                        else if(WT_HHC>24) {start=StartWav_FS[WTS_HHBOW_OPEN50]; count=CountWav_FS[WTS_HHBOW_OPEN50];}
                        else if(WT_HHC>12) {start=StartWav_FS[WTS_HHBOW_OPEN25]; count=CountWav_FS[WTS_HHBOW_OPEN25];}
                        else {start=StartWav_FS[WTS_HHBOW_CLOSED]; count=CountWav_FS[WTS_HHBOW_CLOSED];}
                      }
                      break;
      default: return;
      
    }
    //Analogue Drums
    int n=start+/*floor*/(count*velocity/128)+RR*count;
    int vol= -0.5 + (float)((float)velocity-127.0)/VOLUME_SCALE;
    wavTriggerPlay(n,vol);
    RR=(RR+1)%ROUND_ROBIN_FS;
    
    //2Box
    //int n=start+/*floor*/(count*velocity/127)/*+random*/;
}

//========================================
//Analogue Drums - Bombastix
//========================================

const byte ROUND_ROBIN_BM= 6;

const short StartWav_BM[32]={457,361,385,433,409, //HH TT/TC/TL/TS/TO
                    313,217,241,289,265, //HH ET/EC/EL/ES/EO
                    342,337, //PD - FS
                    //661,703, //SNARE1 CN/XS
                    727,769, //SNARE2 CN/XS
                    //793,835, //SNARE3 CN/XS
                    //481,//KICK1
                    517,//KICK2
		    859,859,895,895,931,931,931,931,//TOM
		    601,577,553,//RIDE1 EG/BW/BL
                    //655,631,607,//RIDE2 EG/BW/BL
		    49,25,//CRASH1 EG/BL
		    97,73,//CRASH2 EG/BL
                    //145,121,//CRASH3 EG/BL
		    //193,169,//CRASH4 EG/BL
		    1,1};//EFFECT

const byte CountWav_BM[32]={4,4,4,4,4,
                   4,4,4,4,4,
                   3,1, //HH
		   7,4, //SNARE
		   6,//KICK
		   6,6,6,6,6,6,6,6,//TOM
		   1,4,4,//RIDE
		   4,4,//CRASH
		   4,4,//CRASH
		   4,4};//EFFECT

void wavChoke_BM(byte pin)
{
  //ferma tutti i suoni appartenetnti a quel pin, possibilmente con un fade 
  switch(pin)
  {
    case WTP_CRASHEDGE: wavTriggerFade(StartWav_BM[WTS_CRASH1_EDGE],CountWav_BM[WTS_CRASH1_EDGE],ROUND_ROBIN_BM); wavTriggerFade(StartWav_BM[WTS_CRASH1_BOW],CountWav_BM[WTS_CRASH1_BOW],ROUND_ROBIN_BM); break;
    case WTP_RIDEEDGE:  wavTriggerFade(StartWav_BM[WTS_RIDE_EDGE],CountWav_BM[WTS_RIDE_EDGE],ROUND_ROBIN_BM); wavTriggerFade(StartWav_BM[WTS_RIDE_BOW],CountWav_BM[WTS_RIDE_BOW],ROUND_ROBIN_BM); break;
    case WTP_HHBOW: wavTriggerFade(StartWav_BM[WTS_HHBOW_OPEN],CountWav_BM[WTS_HHBOW_OPEN],ROUND_ROBIN_BM); wavTriggerFade(StartWav_BM[WTS_HHBOW_OPEN75],CountWav_BM[WTS_HHBOW_OPEN75],ROUND_ROBIN_BM); wavTriggerFade(StartWav_BM[WTS_HHBOW_OPEN50],CountWav_BM[WTS_HHBOW_OPEN50],ROUND_ROBIN_BM); wavTriggerFade(StartWav_BM[WTS_HHBOW_OPEN25],CountWav_BM[WTS_HHBOW_OPEN25],ROUND_ROBIN_BM);break;
  }
}

void wavTrigger_BM(byte pin, byte velocity)
{  
    short start=0;
    byte count=0;
  
    switch(pin)
    {
      case WTP_KICK: start=StartWav_BM[WTS_KICK]; count=CountWav_BM[WTS_KICK]; break;
      case WTP_EFFECT: start=StartWav_BM[WTS_EFFECT1]; count=CountWav_BM[WTS_EFFECT1]; break; 
      case WTP_SNAREHEAD: start=StartWav_BM[WTS_SNARE_HEAD]; count=CountWav_BM[WTS_SNARE_HEAD]; break;
      case WTP_SNARERIM: start=StartWav_BM[WTS_SNARE_RIM]; count=CountWav_BM[WTS_SNARE_RIM]; break;
      case WTP_TOM1HEAD: start=StartWav_BM[WTS_TOM1_HEAD]; count=CountWav_BM[WTS_TOM1_HEAD]; break;
      case WTP_TOM2HEAD: start=StartWav_BM[WTS_TOM2_HEAD]; count=CountWav_BM[WTS_TOM2_HEAD]; break;
      case WTP_TOM3HEAD: start=StartWav_BM[WTS_TOM3_HEAD]; count=CountWav_BM[WTS_TOM3_HEAD]; break;
      case WTP_TOM4HEAD: start=StartWav_BM[WTS_TOM4_HEAD]; count=CountWav_BM[WTS_TOM4_HEAD]; break;
      
      #if WT_24
      case WTP_TOM1RIM: start=StartWav_BM[WTS_TOM1_RIM]; count=CountWav_BM[WTS_TOM1_RIM]; break;
      case WTP_TOM2RIM: start=StartWav_BM[WTS_TOM2_RIM]; count=CountWav_BM[WTS_TOM2_RIM]; break;
      #endif
      
      case WTP_RIDEBOW: start=StartWav_BM[WTS_RIDE_BOW]; count=CountWav_BM[WTS_RIDE_BOW]; break;
      case WTP_RIDEEDGE: start=StartWav_BM[WTS_RIDE_EDGE]; count=CountWav_BM[WTS_RIDE_EDGE]; break;
      case WTP_CRASHBOW: start=StartWav_BM[WTS_CRASH1_BOW]; count=CountWav_BM[WTS_CRASH1_BOW]; break;
      case WTP_CRASHEDGE: start=StartWav_BM[WTS_CRASH1_EDGE]; count=CountWav_BM[WTS_CRASH1_EDGE]; break;
      case WTP_HHBOW: if(Pin[WTP_HHC].Type==HHC)
                      { 
                        if(WT_HHC>HHThresoldSensor[0]) {start=StartWav_BM[WTS_HHBOW_OPEN]; count=CountWav_BM[WTS_HHBOW_OPEN];}
                        else if(WT_HHC>HHThresoldSensor[1]) {start=StartWav_BM[WTS_HHBOW_OPEN75]; count=CountWav_BM[WTS_HHBOW_OPEN75];}
                        else if(WT_HHC>HHThresoldSensor[2]) {start=StartWav_BM[WTS_HHBOW_OPEN50]; count=CountWav_BM[WTS_HHBOW_OPEN50];}
                        else if(WT_HHC>HHThresoldSensor[3]) {start=StartWav_BM[WTS_HHBOW_OPEN25]; count=CountWav_BM[WTS_HHBOW_OPEN25];}
                        else {start=StartWav_BM[WTS_HHBOW_CLOSED]; count=CountWav_BM[WTS_HHBOW_CLOSED];}
                      }
                      break;
      default: return;
      
    }
    //Analogue Drums
    //FORSE UNO SWITCH ELIMINEREBBE I PROBLEMI DI APPROSSIMAZIONE
    int n=start+/*floor*/(count*velocity/128)+RR*count;
    int vol= -0.5 + (float)((float)velocity-127.0)/VOLUME_SCALE;
    wavTriggerPlay(n,vol);
    RR=(RR+1)%ROUND_ROBIN_BM;
    
    //2Box
    //int n=start+/*floor*/(count*velocity/127)/*+random*/;
}

//========================================
//  DRIVER
//========================================

void wavTriggerPlay(int n, int vol)
{
  //VOLUME track(0x01, 0x00) volume(0x00, 0x00) : 0xf0, 0xaa, 0×09, 0×08, 0×01, 0×00, 0×00, 0×00, 0×55 
  //PLAY track(0x0a, 0x00) : 0xf0, 0xaa, 0×08, 0×03, 0×01, 0x0a, 0×00, 0×55
    
  //VOLUME
  mySerial.write(0xF0);
  mySerial.write(0xAA);
  mySerial.write(0x09);
  mySerial.write(0x08);
  mySerial.write((byte)(255 & n));//TRACK
  mySerial.write((byte)(n>>8));//TRACK>>8
  mySerial.write((byte)(255 & vol));//VOLUME
  mySerial.write((byte)(vol>>8));//VOLUME>>8
  mySerial.write(0x55);
    
  //PLAY
  mySerial.write(0xF0);
  mySerial.write(0xAA);
  mySerial.write(0x08);
  mySerial.write(0x03);
  mySerial.write(0x01);
  mySerial.write((byte)(255 & n));//TRACK
  mySerial.write((byte)(n>>8));//TRACK>>8
  mySerial.write(0x55);
    
  Poly[LastPoly]=n;
  LastPoly=(LastPoly+1)%MAX_POLYPHONIC;
}

void wavTriggerFade(short start, byte count, byte rr)
{
  //FADE : 0xf0, 0xaa, 0x0C, 0x0a, 0×03, 0×00, 0×00, 0×00, 0xe8, 0×03, 0×00, 0×55
  //Data = Track Number (2 bytes), Target Volume (2 bytes, signed int, -70dB to +10dB), Milliseconds (2 bytes), StopFlag (1 byte)
	    //VOLUME

  for(byte i=0;i<MAX_POLYPHONIC;i++)
    if(Poly[i]>=start && Poly[i]<start+rr*count)
    {
      /*mySerial.write(0xF0);
      mySerial.write(0xAA);
      mySerial.write(0x0C);
      mySerial.write(0x0A);
      mySerial.write((byte)(255 & (Poly[i])));//TRACK
      mySerial.write((byte)((Poly[i])>>8));//TRACK>>8
      mySerial.write((byte)0x00);//VOLUME
      mySerial.write((byte)0x00);//VOLUME>>8
      mySerial.write((byte)0x00);//TIME
      mySerial.write((byte)0x00);
      mySerial.write(0x01);//STOP
      mySerial.write(0x55);*/
        
       //STOP
      mySerial.write(0xF0);
      mySerial.write(0xAA);
      mySerial.write(0x08);
      mySerial.write(0x03);
      mySerial.write(0x04);
      mySerial.write((byte)(255 & Poly[i]));//TRACK
      mySerial.write((byte)(Poly[i]>>8));//TRACK>>8
      mySerial.write(0x55);
     }
}

#endif
