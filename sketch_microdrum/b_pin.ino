

//=================================
//   DUAL SENSOR
//=================================
#define DualSensor(i) (_DualSensor[(i)&0x07]+((i)&0xF8))
//127=Disabled
const byte _DualSensor[]    = {3,2,1,0,6,7,4,5};
//=================================

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
  
  //===========PIN============
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

  //===========SET============
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
      case DP_SNARERIM:
        this->Type=Switch;
        this->Note=39;
      break;
      case DP_KICK:
        this->Type=Piezo;
        this->Note=36;
      break;
      case DP_HHC:
        this->Type=HHC;
        this->Thresold=10;
      break;
    }
    
  }
  
  //===========SCAN============
  void scan(byte sensor,byte count)
  {
    byte pin = count+(sensor<<3);
    int yn_0 = analogRead(sensor);
    
    //===============================
    //        HHC
    //===============================
    if(Type==HHC) { scanHH(pin,yn_0/8); return; }
    
    //===============================
    //        Switch
    //===============================
    if(Type==Switch)
    {
      //DrawDiagnostic(MulSensor,yn_0/8);
      if(State==Normal_Time) 
      {
        if(yn_0<Thresold*10 && yn_1<Thresold*10 )  
        {
          //DrawDiagnostic(pin,0);
          State=Scan_Time;
          MaxReading=0;
        }
      }
      else if(State==Scan_Time) 
      {      
        if(yn_0<Thresold*10 && yn_1<Thresold*10 ) MaxReading=MaxReading+1;
        else
        {
          if(MaxReading>ScanTime) State=Switch_Time;
          else State=Normal_Time;
        }
      
        if(MaxReading>MaskTime) State=Choke_Time;
      }
      else if(State==Mask_Time)  
      { 
        //if(ZeroCountSensor[MulSensor]>0) DrawDiagnostic(MulSensor,ZeroCountSensor[MulSensor]*16);
        if(MaxReading > 0)
        {
          MaxReading=MaxReading-1;
          //DrawDiagnostic(MulSensor,128);
        }
        else
        {
          if(yn_0>=Thresold*10 && yn_1>=Thresold*10 ) State=Normal_Time;
        }
      }
    }
    //===============================
    //        YSwitch
    //===============================
    else if(Type==YSwitch)
    {
      if(yn_0<Thresold*4 )
      {
        State=Scan_Time;
      
        if(MaxReading<=MaskTime) MaxReading=MaxReading+1;
      }
      else
      {
        if(MaxReading!=255 && MaxReading>ScanTime) //SwitchTime
        {
          ///DUAL
          //if(yn_0>DualThresoldSensor[MulSensor]*4)
          if(yn_0>CurveForm*4) MaxReading = MaxReading; //?
          else MaxReading = 512+MaxReading;
        }
        State=Switch_Time; 
      }
    }
    //===============================
    //        Piezo, HH
    //===============================
    else
    {
      GlobalTime=TIMEFUNCTION;
    
      if(State==Mask_Time)  
      { 
        if ((GlobalTime-Time)>MaskTime)
        {
          State = Retrigger_Time;
          Time = GlobalTime;
        }
      }
    
      yn_0 = 0.5 + ((float)yn_0*(float)Gain)/64.0;
    
        
      if(State==Retrigger_Time)
      {
        int MaxRetriggerSensor=MaxReading-((GlobalTime-Time)*(Retrigger+1)/16);
        if(MaxRetriggerSensor>0)
        {
          if((yn_0 - yn_1)> Thresold && yn_0 > MaxRetriggerSensor)
          {
            State = Scan_Time;
            Time = GlobalTime;
            MaxReading = 0;
          }
        }
        else
          State = Normal_Time;
      }
      else if(State==Normal_Time) 
      {
        if((yn_0 - yn_1)> Thresold) 
        {
          State = Scan_Time;
          Time = GlobalTime;
          MaxReading = 0;
        }
      }
      else if(State==Scan_Time) 
      {
        if ((GlobalTime-Time) < ScanTime)
        {
          if(yn_0 > MaxReading)
          {
            MaxReading = yn_0;
        
            if(MaxXtalkGroup[XtalkGroup]==-1 || MaxXtalkGroup[XtalkGroup]<yn_0) //MaxGroup
              MaxXtalkGroup[XtalkGroup]=yn_0;
          
            if(MaxMultiplexerXtalk[count]==-1 || MaxMultiplexerXtalk[count]<yn_0)
              MaxMultiplexerXtalk[count]=yn_0;
          }
        }
        else
          State=Piezo_Time;
      }
    }
  
    //====================================
    //TOOL
    //====================================
    if(Mode==Tool && LogPin==pin) LogTool(yn_0,pin);
    //====================================
  
    yn_1=yn_0;
  }
  
  //===========PLAY============
  void play(byte pin)
  {

  }
  
  //=======================
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

  private:
  
  void scanHH(byte pin,byte sensorReading)
  {
    if ((GlobalTime-Time) > MaskTime)
    {
      if(sensorReading>(/*yn_1*/MaxReading+Thresold) || sensorReading<(/*yn_1*/MaxReading-Thresold))
      {
        if (Mode==MIDI)
        {
          #if USE_WAVTRIGGER
          wavTriggerHHC(sensorReading);
          #endif
        
          fastMidiCC(Channel,Note,sensorReading);
        }
        else if(Mode==Tool && Diagnostic==true)
          simpleSysex(0x6F,pin,sensorReading,0);
        
        float m=(((float)MaxReading-(float)sensorReading)/((float)Time-(float)GlobalTime))*100;

        MaxReading=sensorReading;//LastReading
        
        //Foot Splash
        if(m>0 && m>HHFootThresoldSensor[0])
          State=Footsplash_Time;

        //FootClose
        else if(m<0 && -m>HHFootThresoldSensor[1])
          State=Footclose_Time;

        Time=GlobalTime;
      }
    }
  }
} Pin[NPin];
//===========================


