//==============================
//    SETTING
//==============================
void SendPinSetting(int Pin,int Set)
{
  int Value=0;
  switch(Set)
  {
    case 0x00: //NOTE
      Value=NoteSensor[Pin];
    break;
    case 0x01: //THRESOLD
      Value=ThresoldSensor[Pin];
    break;
    case 0x02: //SCANTIME
      Value=ScanTimeSensor[Pin];
    break;
    case 0x03: //MASKTIME
      Value=MaskTimeSensor[Pin];
    break;
    case 0x04: //RETRIGGER
      Value=RetriggerSensor[Pin];
    break;
    case 0x05: //CURVE
      Value=CurveSensor[Pin];
    break;
    case 0x06: //XTALK
      Value=XtalkSensor[Pin];
    break;
    case 0x07: //XTALKGROUP
      Value=XtalkGroupSensor[Pin];
    break;
    case 0x08: //CURVEFORM
      Value=CurveFormSensor[Pin];
    break;
    case 0x09: //CHOKE
      Value=ChokeNoteSensor[Pin];
      break;
    case 0x0A: //DUAL
      Value=DualSensor[Pin];
      break;
    case 0x0B: //DUALNOTE
      Value=DualNoteSensor[Pin];
      break;
    case 0x0C: //DUALTHRESOLD
      Value=DualThresoldSensor[Pin];
      break;     
    case 0x0D: //TYPE
      Value=TypeSensor[Pin];
      break;
    case 0x0E: //CHANNEL
      Value=ChannelSensor[Pin];
      break; 
    case 0x7F://All 
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
      simpleSysex(0x02,Pin,0x0A,DualSensor[Pin]);//DUAL
      simpleSysex(0x02,Pin,0x0B,DualNoteSensor[Pin]);//DUALNOTE
      simpleSysex(0x02,Pin,0x0C,DualThresoldSensor[Pin]);//DUALTHRESOLD
      simpleSysex(0x02,Pin,0x0D,TypeSensor[Pin]);//TYPE
      simpleSysex(0x02,Pin,0x0E,ChannelSensor[Pin]);//CHANNEL
      return;
    break;
  } 
 
  simpleSysex(0x02,Pin,Set,Value); 
}

void SendHHSetting(int Set)
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
   else Value==HHFootThresoldSensor[Set-10];
  
   simpleSysex(0x02,0x4C,Set,Value);
}

void SendGeneralSetting(int Set)
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
      Value=MaxNSensor;
      break;
    
    case 0x03: //GeneralXtalk
      Value=GeneralXtalk;
     break;
     
     case 0x7F://All 
       simpleSysex(0x02,0x7E,0x00,(byte)(delayTime/2));//Delay
       //simpleSysex(0x02,0x7E,0x01,(byte)GetHHC());//HHC1
       simpleSysex(0x02,0x7E,0x02,(byte)MaxNSensor);//NSensor
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
        /*randomSeed(millis());
        CheckLicense();*/
      break;
      
      case 0x01: //SetMode
        Serial.flush();
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
        if(Data1==0x7F)
        {
          SendGeneralSetting(Data2);
          SendHHSetting(Data2);
          for(int s=0;s<NPin;s++)
            SendPinSetting(s,Data2);
        }
        else if(Data1==0x7E) //General
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
            case 0x00: delayTime=Data3*2; break; //DELAY
            //case 0x01: int h=GetHHC(); if(h!=127) TypeSensor[h]=0; TypeSensor[Data3]=2; /*HHControl1=Data3;*/ break; //HHC1
            case 0x02: MaxNSensor=Data3; break; //MaxNSensor
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
            case 0x00: NoteSensor[Data1]=Data3; break; //NOTE
            case 0x01: ThresoldSensor[Data1]=Data3; break; //THRESOLD
            case 0x02: ScanTimeSensor[Data1]=Data3; break; //SCANTIME
            case 0x03: MaskTimeSensor[Data1]=Data3; break; //MASKTIME
            case 0x04: RetriggerSensor[Data1]=Data3; break; //RETRIGGER
            case 0x05: CurveSensor[Data1]=Data3; break; //CURVE
            case 0x06: XtalkSensor[Data1]=Data3; break; //XTALK
            case 0x07: XtalkGroupSensor[Data1]=Data3; break; //XTALKGROUP
            case 0x08: CurveFormSensor[Data1]=Data3; break; //CURVEFORM
            case 0x09: ChokeNoteSensor[Data1]=Data3; break; //CHOKE
            case 0x0A: DualSensor[Data1]=Data3; break; //DUAL
            case 0x0B: DualNoteSensor[Data1]=Data3; break; //DUALNOTE
            case 0x0C: DualThresoldSensor[Data1]=Data3; break; //DUALTHRESOLD
            case 0x0D: TypeSensor[Data1]=Data3; break; //TYPE
            case 0x0E: ChannelSensor[Data1]=Data3; break; //CHANNEL
                         
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
        if(Data1==0)
        {
          TimeProf=0;
          NProf=0;
        }
        else if (Data1==1)
        {
         SendProfiling(); 
        }
      break;
      case 0x6E:
        LogPin=Data1;
        LogThresold=Data2;
        N=0;
      break;
      case 0x6F:
        Diagnostic=(Data1==1);
      break;
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
      
      case 0x61:
#if defined(__AVR__)
        simpleSysex(0x61,Data1,Data2,EEPROM.read((Data1*256)+Data2));
#endif
      break;
      
      case 0x7F: //RESET
        Serial.flush();
        Mode=OffMode;
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
  if(Serial.peek()!=0xF0) Serial.read();
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
}
