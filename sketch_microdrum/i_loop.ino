
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     LOOP
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void loop()
{
  //simpleSysex(0xFF,Mode,0x00,0x00);
  Input();
  #if USE_LCD
  Menu();
  #endif
  

  if(Mode==Off)
  {
    delay(100);
    return;
  }


  //==========UNROLLING======
  //{0, 1, 3, 2, 6, 7, 5, 4}
  //=========================
  
  //0
  fastWrite(2,0);/*fastWrite(3,0);fastWrite(4,0);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,0); }
  
  //1
  /*fastWrite(2,0);fastWrite(3,0);*/fastWrite(4,1);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,1); }

  //3
  /*fastWrite(2,0);*/fastWrite(3,1);/*fastWrite(4,1);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,3); }
  
  //2
  /*fastWrite(2,0);fastWrite(3,1);*/fastWrite(4,0);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,2); }

  //6
  fastWrite(2,1);/*fastWrite(3,1);fastWrite(4,0);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,6); }
  
  //7
  /*fastWrite(2,1);fastWrite(3,1);*/fastWrite(4,1);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,7); }
  
  //5
  /*fastWrite(2,1);*/fastWrite(3,0);/*fastWrite(4,1);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,5); }
  
  //4
  /*fastWrite(2,1);fastWrite(3,0);*/fastWrite(4,0);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,4); }

  //===============================
  if (Mode==Standby) return;
  //===============================

  //Time=TIMEFUNCTION;
  for(byte i=0;i<(NSensor*8);i++)
  {
    byte TS=Pin[i].Type;
    //===============================
    //        Disabled, HHC
    //===============================
    if(TS==Disabled || TS==HHC) continue;
 
    //===============================
    //        HH
    //===============================
    if(TS==HH)
    {
      byte Hhc=DualSensor(i);
      
      //Foot Splash
      if(Pin[Hhc].State==Footsplash_Time)
      {
        if (Mode==MIDI) fastNoteOn(Pin[i].Channel,HHFootNoteSensor[0],127);//127
        Pin[Hhc].State=Normal_Time;
      }
      else if(Pin[Hhc].State==Footclose_Time)
      {
        if (Mode==MIDI) fastNoteOn(Pin[i].Channel,HHFootNoteSensor[1],127);
        Pin[Hhc].State=Normal_Time;
      }
      continue;
    }
    //===============================
    
    byte SS=Pin[i].State;
    if(SS==Normal_Time || SS==Scan_Time || SS==Mask_Time || SS==Retrigger_Time) continue;

    //=============XTALK==================
    if(Mode==Tool && Diagnostic==true) PlaySensorTOOLMode(i);
    else
    {
      if(MaxMultiplexerXtalk[i%8]!=-1 && MaxMultiplexerXtalk[i%8]>(2*Pin[i].MaxReading))  //Multiplexer XTalk
      {
        Pin[i].State=Normal_Time;
        continue;
      }
      if(MaxXtalkGroup[Pin[i].XtalkGroup]!=-1 && MaxXtalkGroup[Pin[i].XtalkGroup]>(Pin[i].MaxReading+(64-Pin[i].Xtalk)*4))
      {
        Pin[i].State=Normal_Time;
        continue;
      }  
      PlaySensorMIDIMode(i);

    }
  }
  //RESET XTALK
  for(int i=0;i<8;i++)
    MaxMultiplexerXtalk[i]=MaxXtalkGroup[i]=-1;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    PLAYSENSOR MIDIMODE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void PlaySensorMIDIMode(byte i)
{
  //===============================
  //        Single Switch
  //===============================
  if(Pin[i].Type==Switch)
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
    if(Pin[i].State==Switch_Time)
    {
      fastNoteOn(Pin[i].Channel,Pin[i].Note,127);//MaxReadingSensor[i]*18);
    
      #if USE_WAVTRIGGER
      wavTrigger(i,126);//MaxReadingSensor[i]*18);
      #endif
           
      Pin[i].State=Mask_Time;
      Pin[i].MaxReading = Pin[i].Retrigger;
    }
    else if(Pin[i].State==Choke_Time) //Choke
    {
      fastNoteOn(Pin[i].Channel,Pin[i].ChokeNote,127);
      
      #if USE_WAVTRIGGER
      wavChoke(i);
      #endif
      
      Pin[i].State = Mask_Time;
      Pin[i].MaxReading = Pin[i].Retrigger;
    }
    return;
  }
  //===============================
  
  Time=TIMEFUNCTION;
  
  //===============================
  //          YSwitch
  //===============================
  if(Pin[i].Type==YSwitch)
  {
    if(Pin[i].MaxReading <= 512)
    {
      fastNoteOn(Pin[i].Channel,Pin[i].Note,min(127,Pin[i].MaxReading*8));
    }
    else
      fastNoteOn(Pin[i].Channel,DualSensor(i),min(127,(Pin[i].MaxReading-512)*8));
          
    if(DualSensor(i)!=127)//Dual
    {
      Pin[DualSensor(i)].MaxReading=-1;
      Pin[DualSensor(i)].Time=Time-Pin[DualSensor(i)].ScanTime;
    }
    
    Pin[i].MaxReading = -1;
    return;
  }
  //===============================
  
  //====================================================================
  if (/*(Time-TimeSensor[i]) >= ScanTimeSensor[i]*/ Pin[i].State==Piezo_Time)
  {          
      //Piezo
      if(/*DualSensor(i)!=127 &&*/ Pin[i].Type==Piezo)
      {
        byte v=UseCurve(Pin[i].Curve,Pin[i].MaxReading,Pin[i].CurveForm);
          
        #if USE_WAVTRIGGER
        wavTrigger(i,v);
        #endif
          
        fastNoteOn(Pin[i].Channel,Pin[i].Note,v);
        
        Pin[i].State=Mask_Time;
              
        //Piezo-Switch
        if(Pin[DualSensor(i)].Type==Switch && Pin[DualSensor(i)].State==Switch_Time )
        {
              fastNoteOn(Pin[DualSensor(i)].Channel,Pin[DualSensor(i)].Note,127);
    
              #if USE_WAVTRIGGER
              wavTrigger(DualSensor(i),126);
              #endif
            
              Pin[DualSensor(i)].State=Mask_Time;
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
        byte Note=Pin[i].Note;
        byte Hhc=DualSensor(i);
             
             //if(ZeroCountSensor[HHC]>DualThresoldSensor[i])//DUAL
             if(Pin[Hhc].MaxReading>Pin[Hhc].Thresold)
               Note=Pin[i].ChokeNote;
             else if(Pin[Hhc].MaxReading>HHThresoldSensor[3])
               Note=HHNoteSensor[3];
             else if(Pin[Hhc].MaxReading>HHThresoldSensor[2])
               Note=HHNoteSensor[2];
             else if(Pin[Hhc].MaxReading>HHThresoldSensor[1])
               Note=HHNoteSensor[1];
             else if(Pin[Hhc].MaxReading>HHThresoldSensor[0])
               Note=HHNoteSensor[0];

        fastNoteOn(Pin[i].Channel,Note,UseCurve(Pin[i].Curve,Pin[i].MaxReading,Pin[i].CurveForm));
      }//HH=======================
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    FASTCHECKMULTI
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void fastCheckMulti(byte n1,byte n2)
{
  if(Pin[n2+(n1<<3)].Type!=Disabled) 
  {
    CheckMulti(n1,n2); 
    if(Pin[n2+(n1<<3)].State==Scan_Time) 
    { 
      CheckMulti(n1,n2); CheckMulti(n1,n2); CheckMulti(n1,n2);
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    CHECKMULTI
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CheckMulti(byte Sensor,byte count)
{
  byte MulSensor=count+(Sensor<<3);
  //if(TypeSensor[MulSensor]==127/*Disabled*/) return;

  //Time=TIMEFUNCTION;
  //int sensorReading = analogRead(Sensor); 
  int yn_0 = -1;

  //===============================
  //        HHC
  //===============================
  if(Pin[MulSensor].Type==HHC) { CheckHHControl(MulSensor,analogRead(Sensor)/8); return; }
    
  //===============================
  //        Switch
  //===============================
  if(Pin[MulSensor].Type==Switch)
  {
    yn_0 = analogRead(Sensor);
    
     //DrawDiagnostic(MulSensor,yn_0/8);
    if(Pin[MulSensor].State==Normal_Time) 
    {
      if(yn_0<Pin[MulSensor].Thresold*10 && Pin[MulSensor].yn_1<Pin[MulSensor].Thresold*10 )  
      {
        //DrawDiagnostic(MulSensor,0);
        Pin[MulSensor].State=Scan_Time;
        Pin[MulSensor].MaxReading=0;
      }
    }
    else if(Pin[MulSensor].State==Scan_Time) 
    {      
      if(yn_0<Pin[MulSensor].Thresold*10 && Pin[MulSensor].yn_1<Pin[MulSensor].Thresold*10 )  
        Pin[MulSensor].MaxReading=Pin[MulSensor].MaxReading+1;
      else
      {
        if(Pin[MulSensor].MaxReading>Pin[MulSensor].ScanTime)
          Pin[MulSensor].State=Switch_Time;
        else
          Pin[MulSensor].State=Normal_Time;
      }
      
      if(Pin[MulSensor].MaxReading>Pin[MulSensor].MaskTime)
        Pin[MulSensor].State=Choke_Time;
    }
    else if(Pin[MulSensor].State==Mask_Time)  
    { 
      //if(ZeroCountSensor[MulSensor]>0) DrawDiagnostic(MulSensor,ZeroCountSensor[MulSensor]*16);
      if(Pin[MulSensor].MaxReading > 0)
      {
        Pin[MulSensor].MaxReading=Pin[MulSensor].MaxReading-1;
        //DrawDiagnostic(MulSensor,128);
      }
      else
      {
        if(yn_0>=Pin[MulSensor].Thresold*10 && Pin[MulSensor].yn_1>=Pin[MulSensor].Thresold*10 ) 
          Pin[MulSensor].State=Normal_Time;
      }
    }
  }
  //===============================
  //        YSwitch
  //===============================
  else if(Pin[MulSensor].Type==YSwitch)
  {
    yn_0 = analogRead(Sensor);
    
    if(yn_0<Pin[MulSensor].Thresold*4 )
    {
      Pin[MulSensor].State=Scan_Time;
      
      if(Pin[MulSensor].MaxReading<=Pin[MulSensor].MaskTime) 
        Pin[MulSensor].MaxReading=Pin[MulSensor].MaxReading+1;
    }
    else
    {
      if(Pin[MulSensor].MaxReading!=255 && Pin[MulSensor].MaxReading>Pin[MulSensor].ScanTime) //SwitchTime
      {
        ///DUAL
        //if(yn_0>DualThresoldSensor[MulSensor]*4)
        if(yn_0>Pin[MulSensor].CurveForm*4)
          Pin[MulSensor].MaxReading = Pin[MulSensor].MaxReading;
        else
          Pin[MulSensor].MaxReading = 512+Pin[MulSensor].MaxReading;
      }
      Pin[MulSensor].State=Switch_Time; 
    }
  }
  //===============================
  //        Piezo, HH
  //===============================
  else
  {
    Time=TIMEFUNCTION;
    
    if(Pin[MulSensor].State==Mask_Time)  
    { 
      if ((Time-Pin[MulSensor].Time)>Pin[MulSensor].MaskTime)
      {
        Pin[MulSensor].State = Retrigger_Time;
        Pin[MulSensor].Time = Time;
      }
    }
    
    yn_0 = 0.5 + ((float)analogRead(Sensor)*(float)Pin[MulSensor].Gain)/64.0;
    
        
    if(Pin[MulSensor].State==Retrigger_Time)
    {
      int MaxRetriggerSensor=Pin[MulSensor].MaxReading-((Time-Pin[MulSensor].Time)*(Pin[MulSensor].Retrigger+1)/16);
      if(MaxRetriggerSensor>0)
      {
          if((yn_0 - Pin[MulSensor].yn_1)> Pin[MulSensor].Thresold && yn_0 > MaxRetriggerSensor)
          {
            Pin[MulSensor].State = Scan_Time;
            Pin[MulSensor].Time = Time;
            Pin[MulSensor].MaxReading = 0;
          }
      }
      else
        Pin[MulSensor].State = Normal_Time;
    }
    else if(Pin[MulSensor].State==Normal_Time) 
    {
      if((yn_0 - Pin[MulSensor].yn_1)> Pin[MulSensor].Thresold) 
      {
        Pin[MulSensor].State = Scan_Time;
        Pin[MulSensor].Time = Time;
        Pin[MulSensor].MaxReading = 0;
      }
    }
    else if(Pin[MulSensor].State==Scan_Time) 
    {
      if ((Time-Pin[MulSensor].Time) < Pin[MulSensor].ScanTime)
      {
        if(yn_0 > Pin[MulSensor].MaxReading)
        {
          Pin[MulSensor].MaxReading = yn_0;
        
          if(MaxXtalkGroup[Pin[MulSensor].XtalkGroup]==-1 || MaxXtalkGroup[Pin[MulSensor].XtalkGroup]<yn_0) //MaxGroup
            MaxXtalkGroup[Pin[MulSensor].XtalkGroup]=yn_0;
          
          if(MaxMultiplexerXtalk[count]==-1 || MaxMultiplexerXtalk[count]<yn_0)
            MaxMultiplexerXtalk[count]=yn_0;
        }
      }
      else
        Pin[MulSensor].State=Piezo_Time;
    }
  }
  
  //====================================
  //TOOL
  //====================================
  if(Mode==Tool && LogPin==MulSensor) LogTool(yn_0,MulSensor);
  //====================================
  
  Pin[MulSensor].yn_1=yn_0;

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    CHECKHHCONTROL
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CheckHHControl(byte HHControl,byte sensorReading)
{
  if ((Time-Pin[HHControl].Time) > Pin[HHControl].MaskTime)
  {
    if(sensorReading>(/*yn_1*/Pin[HHControl].MaxReading+Pin[HHControl].Thresold) || sensorReading<(/*yn_1*/Pin[HHControl].MaxReading-Pin[HHControl].Thresold))
    {
      if (Mode==MIDI)
      {
        #if USE_WAVTRIGGER
        wavTriggerHHC(sensorReading);
        #endif
        
        fastMidiCC(Pin[HHControl].Channel,Pin[HHControl].Note,sensorReading);
      }
      else if(Mode==Tool && Diagnostic==true)
        simpleSysex(0x6F,HHControl,sensorReading,0);
        
      float m=(((float)Pin[HHControl].MaxReading-(float)sensorReading)/((float)Pin[HHControl].Time-(float)Time))*100;

      Pin[HHControl].MaxReading=sensorReading;//LastReading
        
      //Foot Splash
      if(m>0 && m>HHFootThresoldSensor[0])
        Pin[HHControl].State=Footsplash_Time;

      //FootClose
      else if(m<0 && -m>HHFootThresoldSensor[1])
        Pin[HHControl].State=Footclose_Time;

      Pin[HHControl].Time=Time;
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    USECURVE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//const float X[9]={0, 128, 256, 384, 512, 640, 768, 896, 1024};
const float _Exp[9]={2.33, 3.85, 6.35, 10.48, 17.28, 28.5, 46.99 , 77.47, 127.74}; //e^((x/256)+0,85)
const float _Log[9]={0, 83.67, 98.23, 106.74, 112.78, 117.47, 121.30 , 124.53, 127.34}; //21*ln(0,42*x)
const float _Sigma[9]={2.28, 6.02, 15.13, 34.15, 63.5, 92.84, 111.86 , 120.97, 127.71}; //128/(1+e^((512-x)/128))
const float _Flat[9]={0, 32.86, 46.42, 55.82, 64.0, 72.17, 81.57 , 95.13, 127}; //64-16*ln((1024/x)-1)

byte UseCurve(byte Curve,int Value,byte Form)
{
  int ret=0;
  //float Xn=(float)Value;
  float f=((float)Form)/32.0;//[1;127]->[0.;4.0]
    
  if(Curve==0)
  {
    ret=0.5 + ((float)Value*f/8.0);
  }
  else
  {
    int i=Value/128;
    int m=Value % 128;
    
    switch(Curve)
    {
      case 1: ret = 0.5 + (((float)m*(_Exp[i+1]-_Exp[i])/128.0) + _Exp[i])*f; break;
      case 2: ret = 0.5 + (((float)m*(_Log[i+1]-_Log[i])/128.0) + _Log[i])*f; break;
      case 3: ret = 0.5 + (((float)m*(_Sigma[i+1]-_Sigma[i])/128.0) + _Sigma[i])*f; break;
      case 4: ret = 0.5 + (((float)m*(_Flat[i+1]-_Flat[i])/128.0) + _Flat[i])*f; break;
      
      default: ret = i*16; break;//Test mode
    }
  }
  
  if(ret<=0) return 0;
  if(ret>=127) return 127;//127
  return ret;
}





