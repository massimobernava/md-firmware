
//=================================
//   DUAL SENSOR
//=================================
#define DualSensor(i) (_DualSensor[(i)&0x07]+((i)&0xF8))
const byte _DualSensor[]    = {3,2,1,0,6,7,4,5};
//=================================

//===========CURVE============
const float _Exp[9]={2.33, 3.85, 6.35, 10.48, 17.28, 28.5, 46.99 , 77.47, 127.74}; //e^((x/256)+0,85)
const float _Log[9]={0, 83.67, 98.23, 106.74, 112.78, 117.47, 121.30 , 124.53, 127.34}; //21*ln(0,42*x)
const float _Sigma[9]={2.28, 6.02, 15.13, 34.15, 63.5, 92.84, 111.86 , 120.97, 127.71}; //128/(1+e^((512-x)/128))
const float _Flat[9]={0, 32.86, 46.42, 55.82, 64.0, 72.17, 81.57 , 95.13, 127}; //64-16*ln((1024/x)-1)

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

//--------------------------------------------
//   DEFINIR MEGA em sketch_microdrum.ino
//   para usar 16 pinos sem multiplex
//--------------------------------------------

#if MEGA

//*** Compat�vel com Megadrum

const byte DP_HHC       = 0x00;
const byte DP_SNAREHEAD = 0x01;
const byte DP_SNARERIM  = 0x02;
const byte DP_KICK      = 0x03;
const byte DP_TOM1HEAD  = 0x04;
const byte DP_TOM2HEAD  = 0x05;
const byte DP_TOM3HEAD  = 0x06;
const byte DP_TOM4HEAD  = 0x07;
const byte DP_EXTRA1    = 0x08;
const byte DP_EXTRA2    = 0x09; //end of teensy inputs 
const byte DP_HHBOW     = 0x0A;
const byte DP_CHINA     = 0x0B;
const byte DP_SPLASH    = 0x0C;
const byte DP_CRASHEDGE = 0x0D;
const byte DP_RIDEBOW   = 0x0E;
const byte DP_RIDEEDGE  = 0x0F;

#else

//*** Pinos Padr�es do Microdrum

const byte DP_SNAREHEAD = 0x04;   // - tip
const byte DP_SNARERIM  = 0x06;
const byte DP_KICK      = 0x07;   // - tip
const byte DP_EFFECT    = 0x05;
const byte DP_HHBOW     = 0x03;
const byte DP_HHEDGE    = 0x00;   // - tip
const byte DP_HHC       = 0x01;
const byte DP_HHC_RING  = 0x02;   //might delete this. Don't know what it is really.

const byte DP_RIDEBOW   = 0x0C;   // pin4         pin2
const byte DP_RIDEEDGE  = 0x0E;   // pin6 - tip   pin4
const byte DP_TOM1HEAD  = 0x0F;   // pin7 - tip
const byte DP_TOM1RIM   = 0x0D;   // pin5
const byte DP_TOM2HEAD  = 0x0B;   // pin3 - tip
const byte DP_TOM2RIM   = 0x08;   // pin0         pin1
const byte DP_CRASHBOW  = 0x09;   // pin1         pin0
const byte DP_CRASHEDGE = 0x0A;   // pin2 - tip   pin6

const byte DP_TOM3HEAD  = 0x14;   // pin4 - tip
const byte DP_TOM3RIM   = 0x16;   // pin6
const byte DP_TOM4HEAD  = 0x17;   // pin7 - tip
const byte DP_TOM4RIM   = 0x15;   // pin5
const byte DP_CRASH2BOW = 0x13;   // pin3
const byte DP_CRASH2EDGE= 0x10;   // pin0 - tip
const byte DP_CHINA     = 0x11;   // pin1 - tip
const byte DP_SPLASH    = 0x12;   // pin2 - tip
#endif

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
    Channel = 10; if (Channel>=15) {Channel = 0;}
    #endif
    
    Thresold=30;
    ScanTime=10;
    MaskTime=30;
    Retrigger=30;

    Curve=Linear;
    CurveForm=110;
    Xtalk=0;
    XtalkGroup=0;

    //Time;
    MaxReading=-1;
    
  }

  //===============================
  //   SET
  //===============================
  void set(byte pin)
  {
    Time=TIMEFUNCTION;
    this->Time=Time+this->MaskTime;
//    #if MEGA
//    this->_pin=pin;
//    #endif
    
    
    switch(pin)
    {
      case DP_SNAREHEAD:
        this->Type=Piezo;
        this->Note=38;
      break;
      case DP_SNARERIM:
        this->Type=Piezo;
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

      #if TEENSY
      case DP_HHEDGE:       this->Type=Piezo; this->Note=42;  break;
      case DP_RIDEBOW:      this->Type=Piezo; this->Note=53;  break;
      case DP_RIDEEDGE:     this->Type=Piezo; this->Note=51;  break;
      case DP_TOM1HEAD:     this->Type=Piezo; this->Note=48;  break;
      case DP_TOM1RIM:      this->Type=Piezo; this->Note=10;  break;
      case DP_TOM2HEAD:     this->Type=Piezo; this->Note=47;  break;
      case DP_TOM2RIM:      this->Type=Piezo; this->Note=10;  break;
      case DP_CRASHBOW:     this->Type=Piezo; this->Note=10;  break;
      case DP_CRASHEDGE:    this->Type=Piezo; this->Note=49;  break;
      case DP_TOM3HEAD:     this->Type=Piezo; this->Note=43;  break;
      case DP_TOM3RIM:      this->Type=Piezo; this->Note=10;  break;
      case DP_TOM4HEAD:     this->Type=Piezo; this->Note=41;  break;
      case DP_TOM4RIM:      this->Type=Piezo; this->Note=10;  break;
      case DP_CRASH2BOW:    this->Type=Piezo; this->Note=10;  break;
      case DP_CRASH2EDGE:   this->Type=Piezo; this->Note=57;  break;
      case DP_CHINA:        this->Type=Piezo; this->Note=52;  break;
      case DP_SPLASH:       this->Type=Piezo; this->Note=55;  break;
      #endif
    }
  }
  
  //===============================
  //   SCAN
  //===============================
  void scan(byte sensor,byte count)
  {
    byte pin = count+(sensor<<3);
    int yn_0 = -1;
    
    //===============================
    //        HHC
    //===============================
//    #if MEGA
//    if(Type==HHC) { scanHHC(_pin,analogRead(_pin)/8); return; }
//    #else
      analogReadAveraging(2);
    if(Type==HHC) { scanHHC(pin,analogRead(sensor)/8); return; }
//    #endif
    
    //===============================
    //        Switch
    //===============================
    if(Type==Switch)
    {
//      #if MEGA
//      yn_0 = analogRead(_pin);
//      #else
      analogReadAveraging(2);
      yn_0 =  analogRead(sensor);
//      #endif
      
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
          if(yn_0>=Thresold*10 && yn_1>=Thresold*10 ) 
            State=Normal_Time;
        }
      }
    }
    //===============================
    //        YSwitch
    //===============================
    else if(Type==YSwitch)
    { 
      analogReadAveraging(2);
      yn_0 = analogRead(sensor);
      
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
          if(yn_0>CurveForm*4) MaxReading = MaxReading; //???
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
//      #if MEGA
//      yn_0 = 0.5 + ((float)analogRead(sensor)*(float)Gain)/64.0;
//      #else   
      yn_0 = 0.5 + ((float)ANALOGREAD(sensor,pin)*(float)Gain)/64.0;
//      #endif
        
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
  
  //===============================
  //   PLAY   
  //===============================
  void play(byte i,pin* dual)
  {
    //===============================
    //        Disabled
    //===============================
    if(Type==Disabled) return;

    //===============================
    //        HHC
    //===============================
    if(Type==HHC) //HH???
    {
      if(State==Footsplash_Time)
      {
        if (Mode==MIDI) fastNoteOn(Channel,HHFootNoteSensor[0],127);//127 
        
        #if DEBUG 
        Serial.println("HHC Footsplash"); 
        #endif
        
        State=Normal_Time;
      }
      else if(State==Footclose_Time)
      {
        if (Mode==MIDI) fastNoteOn(Channel,HHFootNoteSensor[1],127);

        #if DEBUG 
        Serial.println("HHC Footsplash"); 
        #endif 
        
        State=Normal_Time;
      }
      return;
    }
   
    if(State==Normal_Time || State==Scan_Time || State==Mask_Time || State==Retrigger_Time) return;

    //=============XTALK==================
    if(Mode==Tool && Diagnostic==true) playTOOL(i,dual);
    else
    {
      if(MaxMultiplexerXtalk[i%8]!=-1 && MaxMultiplexerXtalk[i%8]>(2*MaxReading))  //Multiplexer XTalk
      {
        State=Normal_Time;
        return;
      }
      if(MaxXtalkGroup[XtalkGroup]!=-1 && MaxXtalkGroup[XtalkGroup]>(MaxReading+(64-Xtalk)*4))
      {
        State=Normal_Time;
        return;
      }  
      playMIDI(i,dual);

    }
  }
  //===============================
  //   PLAYTOOL
  //===============================
  void playTOOL(byte i,pin* dual)
  {
    //===============================
    //        Switch
    //===============================

    if(Type==Switch)
    { 
      simpleSysex(0x6F,i,MaxReading,0);
    
      if(State==Switch_Time)
      {   
        State=Mask_Time;
        MaxReading = -1;
      }
      return;
    }
    //===============================
    //          YSwitch
    //===============================
    if(Type==YSwitch)
    {
      simpleSysex(0x6F,i,MaxReading,0);
      MaxReading = -1;
      return;
    }
    //===============================
    //        Piezo, HH
    //===============================
    if (/*(Time-TimeSensor[i]) >= ScanTimeSensor[i]*/ State==Piezo_Time)
    {          
      //Piezo
      if(Type==Piezo)
      {
        simpleSysex(0x6F,i,useCurve(),0);
        
        State=Mask_Time;
              
        //Piezo-Switch
        if(dual->Type==Switch && dual->State==Switch_Time )
        {
              simpleSysex(0x6F,DualSensor(i),127,0);

              dual->State=Mask_Time;
              dual->MaxReading = -1;
         }
      }
      else //HH========================================
        simpleSysex(0x6F,i,useCurve(),0);
               
      MaxReading = -1;
    }
  }

  //===============================
  //   PLAYMIDI
  //===============================
  void playMIDI(byte i,pin* dual)
  {
    //===============================
    //        Single Switch
    //===============================
    if(Type==Switch)
    { 
      /*if(TypeSensor[DualSensor(i)]==SWITCH) //Switch-Switch
      { 
        fastNoteOn(ChannelSensor[i],NoteSensor[i],MaxReadingSensor[i]/8);
    
        #if WAVTRIGGER
        wavTrigger(i,MaxReadingSensor[i]/8);
        #endif
            
        MaxReadingSensor[i] = -1;
      }
      else if(TypeSensor[DualSensor(i)]==Disabled) //Switch-Disabled
      {
      
      }*/
      if(State==Switch_Time)
      {
        fastNoteOn(Channel,Note,127);

        #if DEBUG
        Serial.println("Switch");
        #endif
        
        #if USE_WAVTRIGGER
        wavTrigger(i,126);//MaxReadingSensor[i]*18);
        #endif
        #if USE_PISERIAL
        piNote(Note,126);
        #endif
          
        State=Mask_Time;
        MaxReading = Retrigger;
      }
      else if(State==Choke_Time) //Choke
      {
        fastNoteOn(Channel,ChokeNote,127);

        #if DEBUG
        Serial.println("Choke");
        #endif
        
        #if USE_WAVTRIGGER
        wavChoke(i);
        #endif
      
        State = Mask_Time;
        MaxReading = Retrigger;
      }
      return;
    }
    //===============================
  
    GlobalTime=TIMEFUNCTION;
  
    //===============================
    //          YSwitch
    //===============================
    if(Type==YSwitch)
    {
      if(MaxReading <= 512)
      {
        fastNoteOn(Channel,Note,min(127,MaxReading*8));

        #if DEBUG
        Serial.println("YSwitch");
        #endif
        
      }
      else
        fastNoteOn(Channel,DualSensor(i),min(127,(MaxReading-512)*8)); //Dual??? ChokeNote
          
      if(DualSensor(i)!=127)//Dual???
      {
        dual->MaxReading=-1;
        dual->Time=GlobalTime - dual->ScanTime;
      }
    
      MaxReading = -1;
      return;
    }
    //===============================
    //    Piezo
    //====================================================================
    if (State==Piezo_Time)
    {          
      //Piezo
      if(Type==Piezo)
      {
        byte v=useCurve();

        #if DEBUG
        Serial.println("Piezo");
        #endif
          
        #if USE_WAVTRIGGER
        wavTrigger(i,v);
        #endif
        
        #if USE_PISERIAL
        piNote(Note,v);
        #endif
        
        #if SAM2695
        //if(Channel==10){
        Serial.println("sent to SAM!")
        synth.noteOn(fluxCHANNEL, Note, v);      // play a note
        synth.noteOff(fluxCHANNEL, Note);         // send note off
        /*}
        else  {
        synth.noteOn(Channel, Note, v);      // play a note
        synth.noteOff(Channel, Note);         // send note off
        }*/
        #endif 
        
        fastNoteOn(Channel,Note,v);

        
        State=Mask_Time;
              
        //Piezo-Switch
        if(dual->Type==Switch && dual->State==Switch_Time )
        {
              fastNoteOn(dual->Channel,dual->Note,127);
    
              #if USE_WAVTRIGGER
              wavTrigger(DualSensor(i),126);
              #endif
              #if USE_PISERIAL
              piNote(DualSensor(i),126);
              #endif
              
              dual->State=Mask_Time;
         }
         /*
         else if(TypeSensor[DualSensor(i)]==PIEZO && MaxReadingSensor[DualSensor(i)]> ThresoldSensor[DualSensor(i)]) //Piezo-Piezo
        {
            byte v=UseCurve(CurveSensor[DualSensor(i)],MaxReadingSensor[DualSensor(i)],CurveFormSensor[DualSensor(i)]);
            #if WAVTRIGGER
             wavTrigger(DualSensor(i),v);
            #endif
            fastNoteOn(ChannelSensor[DualSensor(i)],NoteSensor[DualSensor(i)],v);
            
            MaxReadingSensor[DualSensor(i)]=-1;  //Dual XTalk
          
        }*/
      }
      else //HH========================================
      {
      	//??? dual != hhc
        byte note=Note;
        //if(ZeroCountSensor[HHC]>DualThresoldSensor[i])//DUAL
        if(dual->MaxReading>dual->Thresold)
          note=ChokeNote;
        else if(dual->MaxReading>HHThresoldSensor[3])
          note=HHNoteSensor[3];
        else if(dual->MaxReading>HHThresoldSensor[2])
          note=HHNoteSensor[2];
        else if(dual->MaxReading>HHThresoldSensor[1])
          note=HHNoteSensor[1];
        else if(dual->MaxReading>HHThresoldSensor[0])
          note=HHNoteSensor[0];

        fastNoteOn(Channel,note,useCurve());

        #if DEBUG
        Serial.println("HH");
        #endif
        
      }//HH=======================
    }
  }
  //=======================
  byte Type:4; //type
  byte State:4; //state
  byte _pin;
    
  byte Note;
  union
  {
    byte ChokeNote;
    byte Gain;
  };
  
  #if ENABLE_CHANNEL
  byte Channel=10;
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

  //===============================
  //   USECURVE
  //===============================
  byte useCurve()
  {
    int ret=0;
    //float Xn=(float)Value;
    float f=((float)CurveForm)/32.0;//[1;127]->[0.;4.0]
    
    if(Curve==Linear)
    {
      ret=0.5 + ((float)MaxReading*f/8.0);
    }
    else
    {
      int i=MaxReading/128;
      int m=MaxReading % 128;
    
      switch(Curve)
      {
        case Exp: ret = 0.5 + (((float)m*(_Exp[i+1]-_Exp[i])/128.0) + _Exp[i])*f; break;
        case Log: ret = 0.5 + (((float)m*(_Log[i+1]-_Log[i])/128.0) + _Log[i])*f; break;
        case Sigma: ret = 0.5 + (((float)m*(_Sigma[i+1]-_Sigma[i])/128.0) + _Sigma[i])*f; break;
        case Flat: ret = 0.5 + (((float)m*(_Flat[i+1]-_Flat[i])/128.0) + _Flat[i])*f; break;
      
        default: ret = i*16; break;//Test mode
      }
    }
  
    if(ret<=0) return 0;
    if(ret>=127) return 127;//127
    return ret;
  }

  //===============================
  //   SCANHHC
  //===============================
  void scanHHC(byte pin,byte sensorReading)
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

          #if DEBUG
          Serial.println("HHC");
          #endif
          
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
