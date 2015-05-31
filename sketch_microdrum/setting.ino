
//==============================
//    SETTING
//==============================
void SendPinSetting(byte Pin,byte Set)
{
  if(Set==0x7F)//All
  { 
      simpleSysex(0x02,Pin,0x00,NoteSensor[Pin]);//NOTE
      simpleSysex(0x02,Pin,0x01,ThresoldSensor[Pin]);//THRESOLD
      simpleSysex(0x02,Pin,0x02,ScanTimeSensor[Pin]);//SCANTIME
      simpleSysex(0x02,Pin,0x03,MaskTimeSensor[Pin]);//MASKTIME
      simpleSysex(0x02,Pin,0x04,RetriggerSensor[Pin]);//RETRIGGER
      simpleSysex(0x02,Pin,0x05,CurveSensor[Pin]);//CURVE
      simpleSysex(0x02,Pin,0x06,XtalkSensor[Pin]);//XTALK
      simpleSysex(0x02,Pin,0x07,XtalkGroupSensor[Pin]);//XTALKGROUP
      simpleSysex(0x02,Pin,0x08,CurveFormSensor[Pin]);//CURVEFORM
      simpleSysex(0x02,Pin,0x09,ChokeNoteSensor[Pin]);//CHOKE
      simpleSysex(0x02,Pin,0x0A,DualSensor(Pin));//DUAL
      //simpleSysex(0x02,Pin,0x0B,DualNoteSensor[Pin]);//DUALNOTE
      //simpleSysex(0x02,Pin,0x0C,DualThresoldSensor[Pin]);//DUALTHRESOLD
      simpleSysex(0x02,Pin,0x0D,TypeSensor[Pin]);//TYPE
      simpleSysex(0x02,Pin,0x0E,ChannelSensor[Pin]);//CHANNEL
      return;
  } 
 
  simpleSysex(0x02,Pin,Set,GetPinSetting(Pin,Set)); 
}
byte GetPinSetting(byte Pin,byte Set)
{
  byte Value=0;
  switch(Set)
  {
    case NOTE:
      Value=NoteSensor[Pin];
    break;
    case THRESOLD:
      Value=ThresoldSensor[Pin];
    break;
    case SCANTIME:
      Value=ScanTimeSensor[Pin];
    break;
    case MASKTIME:
      Value=MaskTimeSensor[Pin];
    break;
    case RETRIGGER:
      Value=RetriggerSensor[Pin];
    break;
    case CURVE:
      Value=CurveSensor[Pin];
    break;
    case XTALK:
      Value=XtalkSensor[Pin];
    break;
    case XTALKGROUP:
      Value=XtalkGroupSensor[Pin];
    break;
    case CURVEFORM:
      Value=CurveFormSensor[Pin];
    break;
    case CHOKENOTE:
      Value=ChokeNoteSensor[Pin];
      break;
    case DUAL:
      Value=DualSensor(Pin);
      break;    
    case TYPE:
      Value=TypeSensor[Pin];
      break;
    case CHANNEL:
      Value=ChannelSensor[Pin];
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
        switch(Data1)
        {
           case OffMode: Mode=OffMode; break;
           case StandbyMode: Mode=StandbyMode; /*CheckLicense();*/ break;
           case MIDIMode: Mode=MIDIMode; break;
           case ToolMode: Mode=ToolMode; break;
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
            case NOTE: NoteSensor[Data1]=Data3; break;
            case THRESOLD: ThresoldSensor[Data1]=Data3; break;
            case SCANTIME: ScanTimeSensor[Data1]=Data3; break;
            case MASKTIME: MaskTimeSensor[Data1]=Data3; break;
            case RETRIGGER: RetriggerSensor[Data1]=Data3; break;
            case CURVE: CurveSensor[Data1]=Data3; break;
            case XTALK: XtalkSensor[Data1]=Data3; break;
            case XTALKGROUP: XtalkGroupSensor[Data1]=Data3; break;
            case CURVEFORM: CurveFormSensor[Data1]=Data3; break;
            case CHOKENOTE: ChokeNoteSensor[Data1]=Data3; break;
            //case 0x0A: DualSensor[Data1]=Data3; break; //DUAL
            case TYPE: TypeSensor[Data1]=Data3; break;
            case CHANNEL: ChannelSensor[Data1]=Data3; break;                         
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
      #if PROF
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
      case 0x61:
#if defined(__AVR__)
        simpleSysex(0x61,Data1,Data2,EEPROM.read((Data1*256)+Data2));
#endif
      break;
      
      case 0x7F: //RESET
        Mode=OffMode;
        softReset();
        //simpleSysex(0x7F,0x00,0x00,0x00);
      break;
    }
}

//==============================
//    INPUT
//==============================
/*void Input()
{
  //===HANDSHAKE======
  while(Serial.peek()>=0 && Serial.peek()!=0xF0) Serial.read();
  //===HANDSHAKE======
  
  if (Serial.available() > 6)
  {
    int Start=Serial.read();
    int ID=Serial.read(); 
    int Cmd=Serial.read();
    int Data1=Serial.read();
    int Data2=Serial.read();
    int Data3=Serial.read();
    int End=Serial.read();
    
    ExecCommand(Cmd,Data1,Data2,Data3);
  }
}*/
//==============================
//    USB INPUT
//==============================
void USB_Input()
{ 
  if(usbMIDI.read() && usbMIDI.getType() == 7)
  { qBlink(); delay(10); qBlink();
  
    int ID    = usbMIDI.getSysExArray()[1];
    int Cmd   = usbMIDI.getSysExArray()[2];
    int Data1 = usbMIDI.getSysExArray()[3];
    int Data2 = usbMIDI.getSysExArray()[4];
    int Data3 = usbMIDI.getSysExArray()[5];
    int End   = usbMIDI.getSysExArray()[6];
    
    ExecCommand(Cmd,Data1,Data2,Data3);

    
    #if Serial_Debug
    Serial.println("USB_Input "); // Recieved communication
    Serial.print(ID);
    Serial.print(" ");
    Serial.print(Cmd);
    Serial.print(" ");
    Serial.print(Data1);
    Serial.print(" ");
    Serial.print(Data2);
    Serial.print(" ");
    Serial.print(Data3);
    Serial.print(" ");
    Serial.print(End);
    Serial.println();
    #endif
   }
  }
