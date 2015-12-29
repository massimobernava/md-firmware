//==============================
//     LOOP
//==============================
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
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,0); }
  
  //1
  /*fastWrite(2,0);fastWrite(3,0);*/fastWrite(4,1);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,1); }

  //3
  /*fastWrite(2,0);*/fastWrite(3,1);/*fastWrite(4,1);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,3); }
  
  //2
  /*fastWrite(2,0);fastWrite(3,1);*/fastWrite(4,0);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,2); }

  //6
  fastWrite(2,1);/*fastWrite(3,1);fastWrite(4,0);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,6); }
  
  //7
  /*fastWrite(2,1);fastWrite(3,1);*/fastWrite(4,1);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,7); }
  
  //5
  /*fastWrite(2,1);*/fastWrite(3,0);/*fastWrite(4,1);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,5); }
  
  //4
  /*fastWrite(2,1);fastWrite(3,0);*/fastWrite(4,0);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,4); }

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
    MaxMultiplexerXtalk[i]=-1;
  for(int i=0;i<NXtalkGroup;i++)
    MaxXtalkGroup[i]=-1;
}

//==============================
//    FASTSCAN
//==============================

inline void fastScan(byte sensor,byte count)
{
  byte pin=count+(sensor<<3);
  if(Pin[pin].Type!=Disabled) 
  {
    Pin[pin].scan(sensor,count); 
    if(Pin[pin].State==Scan_Time) 
    { 
      Pin[pin].scan(sensor,count); Pin[pin].scan(sensor,count); Pin[pin].scan(sensor,count);
    }
  }
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
  
  GlobalTime=TIMEFUNCTION;
  
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
      Pin[DualSensor(i)].Time=GlobalTime-Pin[DualSensor(i)].ScanTime;
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

//==============================
//    PLAYSENSOR TOOLMODE
//==============================
void PlaySensorTOOLMode(byte i)
{
  //===============================
  //        Switch
  //===============================
  /*if(TypeSensor[i]==1)
  {
    simpleSysex(0x6F,i,MaxReadingSensor[i],0);
    MaxReadingSensor[i] = -1;
    return;
  }*/
  if(Pin[i].Type==Switch)
  { 
    simpleSysex(0x6F,i,Pin[i].MaxReading,0);
    
    if(Pin[i].State==Switch_Time)
    {   
      Pin[i].State=Mask_Time;
      Pin[i].MaxReading = -1;
    }
    return;
  }
  //===============================
  //          YSwitch
  //===============================
  if(Pin[i].Type==YSwitch)
  {
    simpleSysex(0x6F,i,Pin[i].MaxReading,0);
    Pin[i].MaxReading = -1;
    return;
  }
  //===============================
  //        Piezo, HH
  //===============================
  if (/*(Time-TimeSensor[i]) >= ScanTimeSensor[i]*/ Pin[i].State==Piezo_Time)
  {          
      //Piezo
      if(/*DualSensor(i)!=127 &&*/ Pin[i].Type==Piezo)
      {
        simpleSysex(0x6F,i,UseCurve(Pin[i].Curve,Pin[i].MaxReading,Pin[i].CurveForm),0);
        
        Pin[i].State=Mask_Time;
              
        //Piezo-Switch
        if(Pin[DualSensor(i)].Type==Switch && Pin[DualSensor(i)].State==Switch_Time )
        {
              simpleSysex(0x6F,DualSensor(i),127,0);

              Pin[DualSensor(i)].State=Mask_Time;
              Pin[DualSensor(i)].MaxReading = -1;
         }
      }
      else //HH========================================
        simpleSysex(0x6F,i,UseCurve(Pin[i].Curve,Pin[i].MaxReading,Pin[i].CurveForm),0);
               
    Pin[i].MaxReading = -1;
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    FASTCHECKMULTI
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*inline void fastCheckMulti(byte sensor,byte count)
{
  if(Pin[count+(sensor<<3)].Type!=Disabled) 
  {
    CheckMulti(sensor,count); 
    if(Pin[count+(sensor<<3)].State==Scan_Time) 
    { 
      CheckMulti(sensor,count); CheckMulti(sensor,count); CheckMulti(sensor,count);
    }
  }
}*/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    CHECKMULTI
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*void CheckMulti(byte Sensor,byte count)
{
  byte MulSensor=count+(Sensor<<3);
  //if(TypeSensor[MulSensor]==127) return;

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
    GlobalTime=TIMEFUNCTION;
    
    if(Pin[MulSensor].State==Mask_Time)  
    { 
      if ((GlobalTime-Pin[MulSensor].Time)>Pin[MulSensor].MaskTime)
      {
        Pin[MulSensor].State = Retrigger_Time;
        Pin[MulSensor].Time = GlobalTime;
      }
    }
    
    yn_0 = 0.5 + ((float)analogRead(Sensor)*(float)Pin[MulSensor].Gain)/64.0;
    
        
    if(Pin[MulSensor].State==Retrigger_Time)
    {
      int MaxRetriggerSensor=Pin[MulSensor].MaxReading-((GlobalTime-Pin[MulSensor].Time)*(Pin[MulSensor].Retrigger+1)/16);
      if(MaxRetriggerSensor>0)
      {
          if((yn_0 - Pin[MulSensor].yn_1)> Pin[MulSensor].Thresold && yn_0 > MaxRetriggerSensor)
          {
            Pin[MulSensor].State = Scan_Time;
            Pin[MulSensor].Time = GlobalTime;
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
        Pin[MulSensor].Time = GlobalTime;
        Pin[MulSensor].MaxReading = 0;
      }
    }
    else if(Pin[MulSensor].State==Scan_Time) 
    {
      if ((GlobalTime-Pin[MulSensor].Time) < Pin[MulSensor].ScanTime)
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
*/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    CHECKHHCONTROL
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*void CheckHHControl(byte HHControl,byte sensorReading)
{
  if ((GlobalTime-Pin[HHControl].Time) > Pin[HHControl].MaskTime)
  {
    if(sensorReading>(Pin[HHControl].MaxReading+Pin[HHControl].Thresold) || sensorReading<(Pin[HHControl].MaxReading-Pin[HHControl].Thresold))
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
        
      float m=(((float)Pin[HHControl].MaxReading-(float)sensorReading)/((float)Pin[HHControl].Time-(float)GlobalTime))*100;

      Pin[HHControl].MaxReading=sensorReading;//LastReading
        
      //Foot Splash
      if(m>0 && m>HHFootThresoldSensor[0])
        Pin[HHControl].State=Footsplash_Time;

      //FootClose
      else if(m<0 && -m>HHFootThresoldSensor[1])
        Pin[HHControl].State=Footclose_Time;

      Pin[HHControl].Time=GlobalTime;
    }
  }
}
*/
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


//==============================
//    LOGTOOL
//==============================
void LogTool(int yn_0,byte MulSensor)
{
   #if MENU_LOG
   //FASE 1: autodeterminazione del rumore massimo Nmax in un intervallo fisso di 20s
    if(log_state==0)//FASE 1: AVVIO
    {
      log_T1=TIMEFUNCTION;
      log_Nmax=yn_0;
      log_state=1;
      
      //ChokeNoteSensor[MulSensor]=64; //GAIN=64 attenzione pialla tutto
      //CurveFormSensor[MulSensor]=64;
      //CurveSensor[MulSensor]=0;
       
      d_hsum=0;
      d_tsum=0;
      d_tnum=0;
      d_rmin=127;
      d_vmax=0;
      d_vmin=1024;
      d_vmean=0;
      
      N=0;
      
      //WAIT
      DrawLog(0);

    }
    else if(log_state==1)//FASE 1: ANALISI
    {
      if(yn_0>log_Nmax) log_Nmax=yn_0;
      if((log_T1+20000)<TIMEFUNCTION) //20sec
      {
        log_state=2;
      
        if(d_tnum<25) DrawLog(1); //25 hit
        else DrawLog(2);
     }
    }
    else if(log_state==2) //FASE 1: FINE
    {
      //DrawLog(0);
      
      //FASE 2: analisi del segnale per 1s
      if(yn_0>log_Nmax)
      {
        //Wait
        DrawLog(0);
        N++;
        log_Tmax=log_T1=TIMEFUNCTION; //FASE 2: INIZIO
        log_state=3;
        log_Vmax=yn_0;
        //log_note=0;
      }
    }
    else if(log_state==3)
    {
      //if(log_oldState==0 && StateSensor[MulSensor]==SCAN_TIME) log_note++; //Numero di Scan
      //log_oldState=StateSensor[MulSensor];
    
      if(yn_0>log_Nmax)
      {
        //DrawLog(0);
        N++;
        //log_T2=TIMEFUNCTION; //Dopo quanto tempo scende sotto la soglia
        if(yn_0>log_Vmax)
        {
          log_Vmax=yn_0;
          log_Tmax=TIMEFUNCTION;

          /*log_T80=0;
          log_T70=0;
          log_T60=0;*/
          log_T50=0;
          /*log_T40=0;
          log_T30=0;
          log_T20=0;*/
        }
        else
        {
          /*if(yn_0>(int)((float)log_Vmax*0.8))
            log_T80=log_T2;
          else if(yn_0>(int)((float)log_Vmax*0.7))
            log_T70=log_T2;
          else if(yn_0>(int)((float)log_Vmax*0.6))
            log_T60=log_T2;
          else */
          if(yn_0>(int)((float)log_Vmax*0.5))
            log_T50=TIMEFUNCTION;
          /*else if(yn_0>(int)((float)log_Vmax*0.4))
            log_T40=log_T2;
          else if(yn_0>(int)((float)log_Vmax*0.3))
            log_T30=log_T2;
          else if(yn_0>(int)((float)log_Vmax*0.2))
            log_T20=log_T2;*/
          }
      }
      
      //1 sec
      if((TIMEFUNCTION-log_T1) > 1000)//FASE 2: FINE
      {
        log_state=4;
        //N=0;
        
        //if((log_Tmax-log_T1) > d_tmax) d_tmax=(log_Tmax-log_T1);
        d_hsum+=(log_T50==0?0:log_T50-log_Tmax);
        d_tsum+=(log_Tmax-log_T1);
        d_hsum2+=(log_T50==0?0:log_T50-log_Tmax)*(log_T50==0?0:log_T50-log_Tmax);
        d_tsum2+=(log_Tmax-log_T1)*(log_Tmax-log_T1);
        d_tnum++;
        
        if(((log_Vmax-log_Nmax)/N)<d_rmin) d_rmin=((log_Vmax-log_Nmax)/N);
        if(log_Vmax>d_vmax) d_vmax=log_Vmax;
        if(log_Vmax<d_vmin) d_vmin=log_Vmax;
        d_vmean+=log_Vmax;
        
        if(d_tnum<25) DrawLog(1);
        else if(d_tnum<50) DrawLog(2);
        
      }
    }
    else if(log_state==4) //FASE 3: report
    {
      /*if(log_Show==0) DrawLog(0);
      else if(log_Show==4000) DrawLog(1);
      else if(log_Show==8000) DrawLog(2);
      //else if(log_Show==12000) DrawLog(3);
    
      log_Show++;
      if(log_Show==12000) log_Show=0;*/
      
      if(d_tnum==25)
      {
        Pin[MulSensor].Gain=(40.0/(float)(d_vmean/25))*64.0;
        if (Pin[MulSensor].Gain <16) Pin[MulSensor].Gain=16;
        
        log_T1=TIMEFUNCTION;
        log_Nmax=0;
        log_state=1;
      
        d_hsum=0;
        d_tsum=0;
      
        d_hsum2=0;
        d_tsum2=0;
        
        d_rmin=127;
        d_vmax=0;
      
        N=0;
      
        //WAIT
        DrawLog(0);
      }
      else if(d_tnum==50)
     {
       Pin[MulSensor].Thresold=log_Nmax;
       Pin[MulSensor].ScanTime=(d_tsum/25)+(sqrt((25*d_tsum2)-(d_tsum*d_tsum))/25);
       Pin[MulSensor].MaskTime=(d_hsum/25)+(sqrt((25*d_hsum2)-(d_hsum*d_hsum))/25);
       Pin[MulSensor].Retrigger=d_rmin;
       Pin[MulSensor].CurveForm=(1024/d_vmax)*32;
       Pin[MulSensor].Curve=Linear;
       
       DrawLog(3);
       log_state=5; //END
     }
     else if(yn_0>log_Nmax) //FASE 2: INIZIO
      {
        log_Tmax=log_T1=TIMEFUNCTION;
        log_state=3;
        log_Vmax=yn_0;
        //log_note=0;

         N=0;
         
         DrawLog(0);
      }
    }
  #else
    N++;
    if(yn_0>=(LogThresold*2))
    SendLog(MulSensor,N,yn_0,UseCurve(Pin[MulSensor].Curve,Pin[MulSensor].MaxReading,Pin[MulSensor].CurveForm),Pin[MulSensor].MaxReading,Pin[MulSensor].State);
  #endif  
}




