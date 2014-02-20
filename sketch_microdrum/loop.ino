
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     LOOP
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void loop()
{
  #if TEST
    simpleSysex(0x77,0x01,0x02,0x01);
  #endif
  
  //simpleSysex(0xFF,Mode,0x00,0x00);
  Input();
  #if MENU
  Menu();
  #endif
  
  #if LICENSE
  if(Mode==OffMode)
  {
    //HANDSHAKE
    CheckLicense();
    delay(100);
    //Mode=StandbyMode;
    //NSensor=2;
    return;
  }
  #endif

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
  if (Mode==StandbyMode) return;
  //===============================

  #if TEST
    simpleSysex(0x77,0x01,0x02,0x02);
  #endif
  
  
  //Time=TIMEFUNCTION;
  for(byte i=0;i<(NSensor*8);i++)
  {
    byte TS=TypeSensor[i];
    //===============================
    //        Disabled, HHC
    //===============================
    if(TS==127/*Disabled*/ || TS==2/*HHC*/) continue;
 
    //===============================
    //        HH
    //===============================
    if(TS==3/*HH*/)
    {
      byte HHC=DualSensor[i];
      
      //Foot Splash
      if(MaxReadingSensor[HHC]>0 && MaxReadingSensor[HHC]>HHFootThresoldSensor[0])
      {
        if (Mode==MIDIMode) noteOn(ChannelSensor[i],HHFootNoteSensor[0],127);//127
        MaxReadingSensor[HHC]=0;
      }
      //FootClose
      else if(MaxReadingSensor[HHC]<0 && -MaxReadingSensor[HHC]>HHFootThresoldSensor[1])
      {
        if (Mode==MIDIMode) noteOn(ChannelSensor[i],HHFootNoteSensor[1],127);
        MaxReadingSensor[HHC]=0;
      }
    }
    //===============================
      
    //==============SWITCH CHOKE=================
    if((TS==1 /*Switch*/ || TS==5 /*YSwitch*/)&& ZeroCountSensor[i]>MaskTimeSensor[i]/*ChokeTime*/ && ZeroCountSensor[i]!=255) //Choke
    {
      if (Mode==MIDIMode) noteOn(ChannelSensor[i],ChokeNoteSensor[i],127);//127 //In caso di problemi si può impostare i al posto di 127 in modo da sapere quale pin da problemi
      ZeroCountSensor[i]=255;
      
      continue;
    }
    //===============================
    
    if(MaxReadingSensor[i] <0) continue; //OTTIMIZZA

    //=============XTALK==================
    if(Mode==ToolMode && Diagnostic==true) PlaySensorTOOLMode(i);
    else
    {
      if(MaxMultiplexerXtalk[i%8]!=-1 && MaxMultiplexerXtalk[i%8]>(2*MaxReadingSensor[i]))  //Multiplexer XTalk
      {
        MaxReadingSensor[i]=-1;
        continue;
      }
      if(MaxXtalkGroup[XtalkGroupSensor[i]]!=-1 && MaxXtalkGroup[XtalkGroupSensor[i]]>(MaxReadingSensor[i]+(64-XtalkSensor[i])*4))
      {
        MaxReadingSensor[i]=-1;
        continue;
      }  
      PlaySensorMIDIMode(i);

    }
  }
  //RESET XTALK
  for(int i=0;i<8;i++)
    MaxMultiplexerXtalk[i]=MaxXtalkGroup[i]=-1;
    
  #if TEST
    simpleSysex(0x77,0x01,0x02,0x03);
  #endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    PLAYSENSOR MIDIMODE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void PlaySensorMIDIMode(byte i)
{
  //===============================
  //         Switch
  //===============================
  if(TypeSensor[i]==1)
  {
    //DUAL
    /*if(DualSensor[i]!=127 && MaxReadingSensor[DualSensor[i]]>(DualThresoldSensor[i]*4))
      noteOn(ChannelSensor[i],DualNoteSensor[i],MaxReadingSensor[i]/8); //Dual
    else*/
      noteOn(ChannelSensor[i],NoteSensor[i],MaxReadingSensor[i]/8);
    
    MaxReadingSensor[i] = -1;
    return;
  }
  //===============================
  
  Time=TIMEFUNCTION;
  
  //===============================
  //          YSwitch
  //===============================
  if(TypeSensor[i]==5)
  {
    if(MaxReadingSensor[i] <= 512)
      noteOn(ChannelSensor[i],NoteSensor[i],min(127,MaxReadingSensor[i]*8));
    else
      //noteOn(ChannelSensor[i],DualNoteSensor[i],min(127,(MaxReadingSensor[i]-512)*8));//DUAL
      noteOn(ChannelSensor[i],DualSensor[i],min(127,(MaxReadingSensor[i]-512)*8));
          
    if(DualSensor[i]!=127)//Dual
    {
      MaxReadingSensor[DualSensor[i]]=-1;
      TimeSensor[DualSensor[i]]=Time-ScanTimeSensor[DualSensor[i]];
    }
    
    MaxReadingSensor[i] = -1;
    return;
  }
  //===============================
  
  //====================================================================
  if ((Time-TimeSensor[i]) >= ScanTimeSensor[i] /*|| TypeSensor[i]==1*/)
  {    
    //if (MaxReadingSensor[i] > ThresoldSensor[i])//ATTENZIONE SECONDO ME E' RIDONDANTE
    {
      //Dual
      if(DualSensor[i]!=127 && TypeSensor[i]!=3/*HH*/)
      {
        //Piezo-Piezo
        if(TypeSensor[DualSensor[i]]==0) //Piezo-Piezo
        {
          //DUAL
          if(MaxReadingSensor[DualSensor[i]]>MaxReadingSensor[i])
          {
            MaxReadingSensor[i]=-1;
            return;
          }
          else
          {
            noteOn(ChannelSensor[i],NoteSensor[i],UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]));
            MaxReadingSensor[DualSensor[i]]=-1;  //Dual XTalk
          }
          /*if(MaxReadingSensor[i]> (DualThresoldSensor[i]*4) && MaxReadingSensor[DualSensor[i]]<=(DualThresoldSensor[DualSensor[i]]*4))
            noteOn(ChannelSensor[i],NoteSensor[i],UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i] ));
          else if(MaxReadingSensor[i]<= (DualThresoldSensor[i]*4) && MaxReadingSensor[DualSensor[i]]>(DualThresoldSensor[DualSensor[i]]*4))
            noteOn(ChannelSensor[i],NoteSensor[DualSensor[i]],UseCurve(CurveSensor[DualSensor[i]],MaxReadingSensor[DualSensor[i]],CurveFormSensor[DualSensor[i]] ));
          else if(MaxReadingSensor[i]> (DualThresoldSensor[i]*4) && MaxReadingSensor[DualSensor[i]]>(DualThresoldSensor[DualSensor[i]]*4))
            noteOn(ChannelSensor[i],DualNoteSensor[i],UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i] ));
          else if(MaxReadingSensor[i]<= (DualThresoldSensor[i]*4) && MaxReadingSensor[DualSensor[i]]<=(DualThresoldSensor[DualSensor[i]]*4))
            noteOn(ChannelSensor[i],DualNoteSensor[DualSensor[i]],UseCurve(CurveSensor[DualSensor[i]],MaxReadingSensor[DualSensor[i]],CurveFormSensor[DualSensor[i]] ));
            */
        }
        else if(TypeSensor[DualSensor[i]]==1)//Piezo-Switch
        {
          if(MaxReadingSensor[DualSensor[i]]<0 || ZeroCountSensor[DualSensor[i]]>0)
          {
            noteOn(ChannelSensor[i],NoteSensor[i],UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]));
            MaxReadingSensor[i] = -1;
          } 
          else
           {
             MaxReadingSensor[i] = -1;
             //Lo mettiamo in mask
             TimeSensor[i]=Time-ScanTimeSensor[i];
           }   
          return;
        }
        else
          noteOn(ChannelSensor[i],NoteSensor[i],UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]));
      }
      else //Mono========================================
      {
          if(TypeSensor[i]==3/*HH*/)
          {
             byte Note=NoteSensor[i];
             byte HHC=DualSensor[i];
             
             //if(ZeroCountSensor[HHC]>DualThresoldSensor[i])//DUAL
             if(ZeroCountSensor[HHC]>HHCTHRESOLD)
               Note=ChokeNoteSensor[i];
             else if(ZeroCountSensor[HHC]>HHThresoldSensor[3])
               Note=HHNoteSensor[3];
             else if(ZeroCountSensor[HHC]>HHThresoldSensor[2])
               Note=HHNoteSensor[2];
             else if(ZeroCountSensor[HHC]>HHThresoldSensor[1])
               Note=HHNoteSensor[1];
             else if(ZeroCountSensor[HHC]>HHThresoldSensor[0])
               Note=HHNoteSensor[0];

             noteOn(ChannelSensor[i],Note,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]));

          }
          else
             noteOn(ChannelSensor[i],NoteSensor[i],UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]));
      }//Mono=======================
    }//Thresold
               
    MaxReadingSensor[i] = -1;
  }//ScanTime
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

  byte State=0;
  
  //===============================
  //        HHC
  //===============================
  if(TypeSensor[MulSensor]==2/*HHC*/) { CheckHHControl(MulSensor,analogRead(Sensor)/8); return; }
    
  //===============================
  //        Switch
  //===============================
  if(TypeSensor[MulSensor]==1)
  {
    yn_0 = analogRead(Sensor);
      
    if(yn_0==0 /*&& yn_1[MulSensor]==0 && yn_2[MulSensor]==0*/ /*ZZZ*/)  
    {
      if(ZeroCountSensor[MulSensor]!=255) ZeroCountSensor[MulSensor]=ZeroCountSensor[MulSensor]+1;
    }
    else
    {
      if(ZeroCountSensor[MulSensor]!=255 && ZeroCountSensor[MulSensor]>ScanTimeSensor[MulSensor]) //SwitchTime
      {
        MaxReadingSensor[MulSensor] = ZeroCountSensor[MulSensor]*100;
      }
      ZeroCountSensor[MulSensor]=0; 
    }
  }
  //===============================
  //        YSwitch
  //===============================
  else if(TypeSensor[MulSensor]==5)
  {
    yn_0 = analogRead(Sensor);
    
    if(yn_0<ThresoldSensor[MulSensor]*4 )
    {
      if(ZeroCountSensor[MulSensor]!=255) ZeroCountSensor[MulSensor]=ZeroCountSensor[MulSensor]+1;
    }
    else
    {
      if(ZeroCountSensor[MulSensor]!=255 && ZeroCountSensor[MulSensor]>ScanTimeSensor[MulSensor]) //SwitchTime
      {
        ///DUAL
        //if(yn_0>DualThresoldSensor[MulSensor]*4)
        if(yn_0>CurveFormSensor[MulSensor]*4)
          MaxReadingSensor[MulSensor] = ZeroCountSensor[MulSensor];
        else
          MaxReadingSensor[MulSensor] = 512+ZeroCountSensor[MulSensor];
      }
      ZeroCountSensor[MulSensor]=0; 
    }
  }
  //===============================
  //        Piezo, HH
  //===============================
  else //ATTENZIONE QUESTO POTREBBE DIVENTARE IL PRIMO IF DATO CHE E' IL PIU' PROBABILE
  {
    Time=TIMEFUNCTION;
    if ((Time-TimeSensor[MulSensor]) < ScanTimeSensor[MulSensor])
    {
      State=1;//ScanTime
      yn_0 = (analogRead(Sensor)*(int)ChokeNoteSensor[MulSensor])/64;
      
      if(yn_0 > MaxReadingSensor[MulSensor])
      {
        MaxReadingSensor[MulSensor] = yn_0;
        
	MaxRetriggerSensor[MulSensor]=(yn_0 > RetriggerSensor[MulSensor])?(yn_0 - RetriggerSensor[MulSensor]):0;
        
        if(MaxXtalkGroup[XtalkGroupSensor[MulSensor]]==-1 || MaxXtalkGroup[XtalkGroupSensor[MulSensor]]<yn_0) //MaxGroup
          MaxXtalkGroup[XtalkGroupSensor[MulSensor]]=yn_0;
          
        if(MaxMultiplexerXtalk[count]==-1 || MaxMultiplexerXtalk[count]<yn_0)
           MaxMultiplexerXtalk[count]=yn_0;
      }
    } 
    else
    {
      State=2;//MaskTime
      if ((Time-TimeSensor[MulSensor])>MaskTimeSensor[MulSensor])
      {
        yn_0 = (analogRead(Sensor)*(int)ChokeNoteSensor[MulSensor])/64;
        if((Time-TimeSensor[MulSensor])<2*MaskTimeSensor[MulSensor])
        {
          State=3;//RetriggerTime
          if((yn_0 - yn_1[MulSensor])> ThresoldSensor[MulSensor] && yn_0 > MaxRetriggerSensor[MulSensor])
          {
            State=1;//ScanTime
            TimeSensor[MulSensor]=Time;
            MaxReadingSensor[MulSensor] = yn_0;
          }
        }
        else
        {
          State=0;//NormalTime
          if((yn_0 - yn_1[MulSensor])> ThresoldSensor[MulSensor]) { TimeSensor[MulSensor]=Time; MaxReadingSensor[MulSensor] = yn_0; State=1;}//ScanTime
        }
      }
      //else MaxReadingSensor[MulSensor] = -1;
    }
  }
  
  //====================================
  //TOOL
  //====================================
  if(Mode==ToolMode && LogPin==MulSensor)
  {
    N++;
    if(yn_0>=(LogThresold*2)) 
      SendLog(MulSensor,N,yn_0,MaxRetriggerSensor[MulSensor],MaxReadingSensor[MulSensor],State);
  }
  //====================================
  
  yn_1[MulSensor]=yn_0;

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    CHECKHHCONTROL
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CheckHHControl(byte HHControl,byte sensorReading)
{
   if ((Time-TimeSensor[HHControl]) > MaskTimeSensor[HHControl])
    {
      if(sensorReading>(/*yn_1*/ZeroCountSensor[HHControl]+HHCTHRESOLD) || sensorReading<(/*yn_1*/ZeroCountSensor[HHControl]-HHCTHRESOLD))
      {
        if (Mode==MIDIMode) midiCC(ChannelSensor[HHControl],NoteSensor[HHControl],sensorReading);
        
        float m=((float)ZeroCountSensor[HHControl]-(float)sensorReading)/((float)TimeSensor[HHControl]-(float)Time);

        MaxReadingSensor[HHControl]=m*100;
               
        ZeroCountSensor[HHControl]=sensorReading;//LastReading
        //yn_1[HHControl]=sensorReading;
        
        TimeSensor[HHControl]=Time;
      }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    USECURVE beta version
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
byte UseCurve(byte Curve,int Value,byte Form)
{
  int ret=0;
 
  float x=(float)Value;
  float f=((float)Form)/64.0;//[1;127]->[0.;2.0]

  switch(Curve)
  {
    //[0-1023]x[0-127]
    case 0: ret=x*f/8.0; break;
    case 1: ret=(127.0/(exp(2.0*f)-1))*(exp(f*x/512.0)-1.0);break; //Exp 4*(exp(x/256)-1)
    case 2: ret=log(1.0+(f*x/128.0))*(127.0/log((8*f)+1));break; //Log 64*log(1+x/128)
    case 3: ret=(127.0/(1.0+exp(f*(512.0-x)/64.0)));break; //Sigma
    case 4: ret=(64.0-((8.0/f)*log((1024/(1+x))-1)));break; //Flat
  }
  if(ret<=0) return 0;
  if(ret>=127) return 127;//127
  return ret;
}




