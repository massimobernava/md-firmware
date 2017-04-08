
//===========SETTING============
const byte NOTE      = 0x00;
const byte THRESOLD  = 0x01;
const byte SCANTIME  = 0x02;
const byte MASKTIME  = 0x03;
const byte RETRIGGER = 0x04;
const byte CURVE     = 0x05;
const byte XTALK     = 0x06;
const byte XTALKGROUP= 0x07;
const byte CURVEFORM = 0x08;
const byte CHOKENOTE = 0x09;
const byte DUAL      = 0x0A;
const byte TYPE      = 0x0D;
const byte CHANNEL   = 0x0E;
//===============================


//==============================
//    SETTING
//==============================
void SendPinSetting(byte pin,byte Set)
{
  if(Set==0x7F)//All
  { 
      simpleSysex(0x02,pin,0x00,Pin[pin].Note);//NOTE
      simpleSysex(0x02,pin,0x01,Pin[pin].Thresold);//THRESOLD
      simpleSysex(0x02,pin,0x02,Pin[pin].ScanTime);//SCANTIME
      simpleSysex(0x02,pin,0x03,Pin[pin].MaskTime);//MASKTIME
      simpleSysex(0x02,pin,0x04,Pin[pin].Retrigger);//RETRIGGER
      simpleSysex(0x02,pin,0x05,Pin[pin].Curve);//CURVE
      simpleSysex(0x02,pin,0x06,Pin[pin].Xtalk);//XTALK
      simpleSysex(0x02,pin,0x07,Pin[pin].XtalkGroup);//XTALKGROUP
      simpleSysex(0x02,pin,0x08,Pin[pin].CurveForm);//CURVEFORM
      simpleSysex(0x02,pin,0x09,Pin[pin].ChokeNote);//CHOKE
      simpleSysex(0x02,pin,0x0A,DualSensor(pin));//DUAL
      //simpleSysex(0x02,Pin,0x0B,DualNoteSensor[Pin]);//DUALNOTE
      //simpleSysex(0x02,Pin,0x0C,DualThresoldSensor[Pin]);//DUALTHRESOLD
      simpleSysex(0x02,pin,0x0D,Pin[pin].Type);//TYPE
      #if ENABLE_CHANNEL
      simpleSysex(0x02,pin,0x0E,Pin[pin].Channel);//CHANNEL
      #endif
      return;
  } 
 
  simpleSysex(0x02,pin,Set,GetPinSetting(pin,Set)); 
}

byte GetPinSetting(byte pin,byte Set)
{
  byte Value=0;
  switch(Set)
  {
    case NOTE:
      Value=Pin[pin].Note;
    break;
    case THRESOLD:
      Value=Pin[pin].Thresold;
    break;
    case SCANTIME:
      Value=Pin[pin].ScanTime;
    break;
    case MASKTIME:
      Value=Pin[pin].MaskTime;
    break;
    case RETRIGGER:
      Value=Pin[pin].Retrigger;
    break;
    case CURVE:
      Value=Pin[pin].Curve;
    break;
    case XTALK:
      Value=Pin[pin].Xtalk;
    break;
    case XTALKGROUP:
      Value=Pin[pin].XtalkGroup;
    break;
    case CURVEFORM:
      Value=Pin[pin].CurveForm;
    break;
    case CHOKENOTE:
      Value=Pin[pin].ChokeNote;
      break;
    case DUAL:
      Value=DualSensor(pin);
      break;    
    case TYPE:
      Value=Pin[pin].Type;
      break;
    case CHANNEL:
    #if ENABLE_CHANNEL
      Value=Pin[pin].Channel;
    #endif
      break; 
  } 
 
  return Value;
}

void SendHHSetting(byte Set)
{
    byte Value=0;
    if(Set==0x7F)
    {
      simpleSysex(0x02,0x4C,0x00,HHNoteSensor[0]);//HH NOTE A
      simpleSysex(0x02,0x4C,0x01,HHNoteSensor[1]);//HH NOTE B
      simpleSysex(0x02,0x4C,0x02,HHNoteSensor[2]);//HH NOTE C
      simpleSysex(0x02,0x4C,0x03,HHNoteSensor[3]);//HH NOTE D
      simpleSysex(0x02,0x4C,0x04,HHThresoldSensor[0]);//HH THRESOLD A
      simpleSysex(0x02,0x4C,0x05,HHThresoldSensor[1]);//HH THRESOLD B
      simpleSysex(0x02,0x4C,0x06,HHThresoldSensor[2]);//HH THRESOLD C
      simpleSysex(0x02,0x4C,0x07,HHThresoldSensor[3]);//HH THRESOLD D
      
      simpleSysex(0x02,0x4C,0x08,HHFootNoteSensor[0]);//HH FOOTSPLASH NOTE
      simpleSysex(0x02,0x4C,0x09,HHFootNoteSensor[1]);//HH FOOTCLOSE NOTE
      simpleSysex(0x02,0x4C,0x0A,HHFootThresoldSensor[0]);//HH FOOTSPLASH THRESOLD
      simpleSysex(0x02,0x4C,0x0B,HHFootThresoldSensor[1]);//HH FOOTCLOSE THRESOLD
      return;
    }
   else if(Set<4)Value=HHNoteSensor[Set];
   else if(Set<8)Value=HHThresoldSensor[Set-4];
   else if(Set<10) Value=HHFootNoteSensor[Set-8];
   else Value=HHFootThresoldSensor[Set-10];
  
   simpleSysex(0x02,0x4C,Set,Value);
}

void SendGeneralSetting(byte Set)
{
  byte Value=0;
   switch(Set)
  {
    case 0x00: //Delay
      Value=delayTime/2;
    break;
    
    /*case 0x01: //HHC1
      Value=GetHHC();
      break;*/
    
    case 0x02: //NSensor
      Value=NSensor;
      break;
    
    case 0x03: //GeneralXtalk
      Value=GeneralXtalk;
     break;
     
     case 0x7F://All 
       simpleSysex(0x02,0x7E,0x00,(byte)(delayTime/2));//Delay
       //simpleSysex(0x02,0x7E,0x01,(byte)GetHHC());//HHC1
       simpleSysex(0x02,0x7E,0x02,(byte)NSensor);//NSensor
       simpleSysex(0x02,0x7E,0x03,(byte)GeneralXtalk);//GeneralXtalk
       return;
     break;
  }
  simpleSysex(0x02,0x7E,Set,Value);
}
 
void ExecCommand(int Cmd,int Data1,int Data2,int Data3)
{
  switch(Cmd)
    {
      case 0x00: //AskMode
        simpleSysex(0x00,Mode,0x00,0x00);
      break;
      
      case 0x01: //SetMode
        Serial.flush();
        switch(Data1)
        {
           case Off: Mode=Off; break;
           case Standby: Mode=Standby; /*CheckLicense();*/ break;
           case MIDI: Mode=MIDI; break;
           case Tool: Mode=Tool; break;
        } 
        simpleSysex(0x01,Mode,0x00,0x00);
      break;
      
      case 0x02: //AskSetting: Data1: general, pad ecc... Data2:delay, note, ecc.. Data3:value
        /*if(Data1==0x7F)
        {
          SendGeneralSetting(Data2);
          SendHHSetting(Data2);
          for(int s=0;s<NPin;s++)
            SendPinSetting(s,Data2);
        }
        else*/ if(Data1==0x7E) //General
          SendGeneralSetting(Data2);
        else if(Data1==0x4C) //HH
          SendHHSetting(Data2);
        else
          SendPinSetting(Data1,Data2);
          
         simpleSysex(0x02,0x7F,0x7F,0x7F);//Fine trasmissioni
      break;
      
      case 0x03: //SetSetting: Data1: general, pad ecc... Data2:delay, note, ecc.. Data3:value
        if(Data1==0x7F) //Non utilizzato
        {

        }
        else if(Data1==0x7E) //General
        {
          switch(Data2)
          {
            //case 0x00: delayTime=Data3*2; break; //DELAY
            //case 0x01: int h=GetHHC(); if(h!=127) TypeSensor[h]=0; TypeSensor[Data3]=2; /*HHControl1=Data3;*/ break; //HHC1
            case 0x02: NSensor=Data3; break; //MaxNSensor
            case 0x03: GeneralXtalk=Data3; break; //GeneralXtalk
          }
        }
        else if(Data1==0x4C) //HH
        {
          switch(Data2)
          {
            case 0x00: HHNoteSensor[0]=Data3;break; //HH NOTE A
            case 0x01: HHNoteSensor[1]=Data3;break; //HH NOTE B
            case 0x02: HHNoteSensor[2]=Data3;break; //HH NOTE C
            case 0x03: HHNoteSensor[3]=Data3;break; //HH NOTE D
            case 0x04: HHThresoldSensor[0]=Data3;break; //HH Thresold A
            case 0x05: HHThresoldSensor[1]=Data3;break; //HH Thresold B
            case 0x06: HHThresoldSensor[2]=Data3;break; //HH Thresold C
            case 0x07: HHThresoldSensor[3]=Data3;break; //HH Thresold D
            
            case 0x08: HHFootNoteSensor[0]=Data3;break;//FootSplash Note
            case 0x09: HHFootNoteSensor[1]=Data3;break;//FootClose Note
            case 0x0A: HHFootThresoldSensor[0]=Data3;break;//FootSplash Thresold
            case 0x0B: HHFootThresoldSensor[1]=Data3;break;//FootClose Thresold
            
          }
        }
        else
        {
           switch(Data2)
          {
            case NOTE: Pin[Data1].Note=Data3; break;
            case THRESOLD: Pin[Data1].Thresold=Data3; break;
            case SCANTIME: Pin[Data1].ScanTime=Data3; break;
            case MASKTIME: Pin[Data1].MaskTime=Data3; break;
            case RETRIGGER: Pin[Data1].Retrigger=Data3; break;
            case CURVE: Pin[Data1].Curve=(curve)Data3; break;
            case XTALK: Pin[Data1].Xtalk=Data3; break;
            case XTALKGROUP: Pin[Data1].XtalkGroup=Data3; break;
            case CURVEFORM: Pin[Data1].CurveForm=Data3; break;
            case CHOKENOTE: Pin[Data1].ChokeNote=Data3; break;
            //case 0x0A: DualSensor[Data1]=Data3; break; //DUAL
            case TYPE: Pin[Data1].Type=(type)Data3; break;
            #if ENABLE_CHANNEL
            case CHANNEL: Pin[Data1].Channel=Data3; break;
            #endif
                         
          }
        }
      break;
      case 0x04: //SaveSetting: Data1: general, pad ecc... Data2:delay, note, ecc.. Data3:value
        if(Data1==0x7F) //Non utilizzato
        {

        }
        else if(Data1==0x7E) //General
        {
           SaveGeneralEEPROM(Data2,Data3);
        }
        else if(Data1==0x4C)
        {
           SaveHHEEPROM(Data2,Data3); 
        }
        else
        {
           SaveEEPROM(Data1,Data2,Data3);
        }
      break;
      case 0x6D:
      #if USE_PROFILER
        if(Data1==0)
        {
          TimeProf=0;
          NProf=0;
        }
        else if (Data1==1)
        {
         SendProfiling(); 
        }
      #endif
      break;
      case 0x6E:
        LogPin=Data1;
        LogThresold=Data2;
        N=0;
      break;
      case 0x6F:
        Diagnostic=(Data1==1);
      break;
      /*
      #if LICENSE
      case 0x60://License
        if(LicenseData[0]==Data1 && LicenseData[1]==Data2)
        {
          if(Data3==PearsonHash(LicenseData,2))
          {
            Mode=StandbyMode;
            NSensor=MaxNSensor;
            //digitalWrite(ledPin,HIGH);
          }
          else if(Data3==0x00)
          {
            Mode=StandbyMode;
            NSensor=2;
          }
          else
          {
            LicenseData[0]=random(128);
            LicenseData[1]=random(128);
            
            //CheckLicense();
          }
        }
      break;
      #endif
      */
      case 0x61:
#if defined(__AVR__)
        simpleSysex(0x61,Data1,Data2,EEPROM.read((Data1*256)+Data2));
#endif
      break;
      
      case 0x7F: //RESET
        Serial.flush();
        Mode=Off;
        softReset();
        //simpleSysex(0x7F,0x00,0x00,0x00);
      break;
    }
}

//==============================
//    INPUT
//==============================
void Input()
{
      //===HANDSHAKE======
      if(usbMIDI.read() && usbMIDI.getType() == 7)
      //===HANDSHAKE======
      { blink();
  
    int ID    = usbMIDI.getSysExArray()[1];
    int Cmd   = usbMIDI.getSysExArray()[2];
    int Data1 = usbMIDI.getSysExArray()[3];
    int Data2 = usbMIDI.getSysExArray()[4];
    int Data3 = usbMIDI.getSysExArray()[5];
    int End   = usbMIDI.getSysExArray()[6];
    
    #if DEBUG
    Serial.println("  Begin USB_INPUT: ");
    Serial.print("ID: "); Serial.print(ID); Serial.print("  Cmd: "); Serial.print(Cmd); delay(5);
    Serial.print("  Data1: "); Serial.print(Data1); Serial.print("  Data2: "); Serial.print(Data2); delay(5);
    Serial.print("  Data3: "); Serial.print(Data3); Serial.print("  End: "); Serial.print(End); delay(5);
    Serial.println("  END USB_INPUT ");
    #endif
    
  //===HANDSHAKE======
  while(HWSERIAL.peek()>=0 && HWSERIAL.peek()!=0xF0) HWSERIAL.read();
  //===HANDSHAKE======
  
  if (HWSERIAL.available() > 6)
  {
    byte Start=   HWSERIAL.read();
    byte ID=      HWSERIAL.read(); 
    int Cmd=      HWSERIAL.read();
    int Data1=    HWSERIAL.read();
    int Data2=    HWSERIAL.read();
    int Data3=    HWSERIAL.read();
    byte End=     HWSERIAL.read();
    
    ExecCommand(Cmd,Data1,Data2,Data3);
  }
}
}
