

//===========TYPE============
enum type:byte
{
  Piezo    = 0,
  Switch   = 1,
  HHC      = 2,
  HH       = 3,
  HHs      = 4,
  YSwitch  = 5,
  Disabled = 15
};
//============================

//===========CURVE============
enum curve:byte
{
  Linear  = 0,
  Exp     = 1,
  Log     = 2,
  Sigma   = 3,
  Flat    = 4
};
//============================

//===========TIME============
enum state:byte
{
  Normal_Time     = 0,
  Scan_Time       = 1,
  Mask_Time       = 2,
  Retrigger_Time  = 3,
  Piezo_Time      = 4,
  Switch_Time     = 5,
  Choke_Time      = 6,
  Footsplash_Time = 7,
  Footclose_Time  = 8,
  Scanretrigger_Time = 9
};
//============================

//===========================
//   DEFAULT PIN
//===========================
const byte DP_SNAREHEAD = 0x04;
const byte DP_SNARERIM  = 0x06;
const byte DP_KICK      = 0x07;
const byte DP_EFFECT    = 0x05;
const byte DP_HHBOW     = 0x03;
const byte DP_HHEDGE    = 0x00;
const byte DP_HHC       = 0x01;
const byte DP_HHC_RING  = 0x02;

const byte DP_RIDEBOW   = 0x0C;
const byte DP_RIDEEDGE  = 0x0E;
const byte DP_TOM1HEAD  = 0x0F;
const byte DP_TOM1RIM   = 0x0D;
const byte DP_TOM2HEAD  = 0x0B;
const byte DP_TOM2RIM   = 0x08;
const byte DP_CRASHBOW  = 0x09;
const byte DP_CRASHEDGE = 0x0A;

const byte DP_TOM3HEAD  = 0x14;
const byte DP_TOM3RIM   = 0x16;
const byte DP_TOM4HEAD  = 0x17;
const byte DP_TOM4RIM   = 0x15;
const byte DP_CRASH2BOW = 0x13;
const byte DP_CRASH2EDGE= 0x10;
const byte DP_CHINA     = 0x11;
const byte DP_SPLASH    = 0x12;

//===========================
//   PIN 
//===========================
class pin
{
  public:
  
  pin()
  {    
    Type=Disabled;
    //State;
    Note=0;
    
    Gain=20;
    #if ENABLE_CHANNEL
    Channel=9;
    #endif
    Thresold=20;
    ScanTime=10;
    MaskTime=50;
    Retrigger=30;

    Curve=Linear;
    CurveForm=110;
    Xtalk=0;
    XtalkGroup=0;

    //Time;
    MaxReading=-1;
    
  }

  void set(byte pin)
  {
    Time=TIMEFUNCTION;
    this->Time=Time+this->MaskTime;
    
    switch(pin)
    {
      case DP_SNAREHEAD:
        this->Type=Piezo;
        this->Note=38;
      break;
      case DP_KICK:
        this->Type=Piezo;
        this->Note=35;
      break;
      case DP_HHC:
        this->Type=HHC;
        this->Thresold=10;
      break;
    }
  }

  byte Type:4; //type
  byte State:4; //state
    
  byte Note;
  union
  {
    byte ChokeNote;
    byte Gain;
  };
  
  #if ENABLE_CHANNEL
  byte Channel;
  #endif
  
  byte Thresold;
  byte ScanTime;
  byte MaskTime;
  byte Retrigger;
  
  byte CurveForm;
  byte Curve:4;
  byte XtalkGroup:4;
  byte Xtalk;

  unsigned long Time;
  int MaxReading;
  int yn_1;
  
} Pin[NPin];
//===========================


