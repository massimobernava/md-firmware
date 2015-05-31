//==============================
//    SENDLOG
//==============================
void SendLog(byte Sensor,int N,int Y0,int MaxRetrigger,int MaxReading,byte State)
{
  //Time               4 bytes
  //Sensor             1 byte
  //N                  2 bytes
  //Reading            2 bytes
  //Y0                 2 bytes
  //MaxReading         2 bytes
  //State (Scan,Mask)  1 byte
  
  byte buf[19]; // Work only with Arduino serial (MIDI>127)
  
  buf[0] = 0xF0;
  buf[1] = 0x77;
  buf[2] = 0x6E;
  buf[3] =19;
  
  buf[4] = (byte) Time;
  buf[5] = (byte) (Time >> 8);
  buf[6] = (byte) (Time >> 16);
  buf[7] = (byte) (Time >> 24);
  
  buf[8] = Sensor;

  buf[9] = (byte)N;
  buf[10] = (byte)(N>>8);
  
  buf[11] = (byte)MaxRetrigger;
  buf[12] = (byte)(MaxRetrigger>>8);
  
  buf[13] = (byte)Y0;
  buf[14] = (byte)(Y0>>8);
  
  buf[15] = (byte)MaxReading;
  buf[16] = (byte)(MaxReading>>8);
  
  buf[17] = State;
  
  buf[18] = 0xF7;
  
  #if USB_MIDI
   usbMIDI.sendSysEx(19, buf);  
   #else
   Serial1.write(19,buf);
  #endif
  
  #if Serial_Debug
  Serial.println("Sendlog ");
   for(int i=0; i<20; i++)
    {Serial.print(buf[i]); Serial.print(" ");}
    Serial.println();
  #endif
}

/*void SendLog(byte Sensor,int N,int Y0,int MaxRetrigger,int MaxReading,byte State)
{
  //Time               4 bytes
  //Sensor             1 byte
  //N                  2 bytes
  //Reading            2 bytes
  //Y0                 2 bytes
  //MaxReading         2 bytes
  //State (Scan,Mask)  1 byte
  
  byte buf[14]; // Work only with Arduino serial (MIDI>127)
  
  buf[0] = (byte) Time;
  buf[1] = (byte) (Time >> 8);
  buf[2] = (byte) (Time >> 16);
  buf[3] = (byte) (Time >> 24);
  
  buf[4] = Sensor;

  buf[5] = (byte)N;
  buf[6] = (byte)(N>>8);
  
  buf[7] = (byte)MaxRetrigger;
  buf[8] = (byte)(MaxRetrigger>>8);
  
  buf[9] = (byte)Y0;
  buf[10] = (byte)(Y0>>8);
  
  buf[11] = (byte)MaxReading;
  buf[12] = (byte)(MaxReading>>8);
  
  buf[13] = State;
  
  Sysex(0x6E,buf,14);
  
}
*/

//==============================
//    SENDPROFILING
//==============================
#if PROF
void SendProfiling()
{
  byte buf[13];
  
  
  buf[0] = 0xF0;
  buf[1] = 0x77;
  buf[3] = 0x6D;
  buf[4] = 14;    //not sure if I need this
  buf[5] = (byte) TimeProf;
  buf[6] = (byte) (TimeProf >> 8);
  buf[7] = (byte) (TimeProf >> 16);
  buf[8] = (byte) (TimeProf >> 24);
  
  buf[9] = (byte) NProf;
  buf[10] = (byte) (NProf >> 8);
  buf[11] = (byte) (NProf >> 16);
  buf[12] = (byte) (NProf >> 24);
  
  buf[13] = 0xF7;
  
  #if USB_MIDI
   usbMIDI.sendSysEx(13, buf);
   #else
   Serial1.write(13,buf);
  #endif
  
  #if Serial_Debug
  Serial.println("Profiling");
   for(int i=0; i<20; i++)
    {Serial.print(buf[i]); Serial.print(" ");}
    Serial.println();
  #endif
}
#endif

/*
#if PROF
void SendProfiling()
{
  byte buf[8];
  
  buf[0] = (byte) TimeProf; //Work only with Arduino serial (MIDI>127)
  buf[1] = (byte) (TimeProf >> 8);
  buf[2] = (byte) (TimeProf >> 16);
  buf[3] = (byte) (TimeProf >> 24);
  
  buf[4] = (byte) NProf;
  buf[5] = (byte) (NProf >> 8);
  buf[6] = (byte) (NProf >> 16);
  buf[7] = (byte) (NProf >> 24);
  
  Sysex(0x6D,buf,8);
}
#endif
*/

//==============================
//    LICENSE
//==============================
#if LICENSE
void CheckLicense()
{
  //byte LicenseHash=PearsonHash(LicenseData,2);
  simpleSysex(0x60,LicenseData[0],LicenseData[1],0x00);
}
byte PearsonHash(byte* in,byte size)
{
 byte h=0;
 for(int i=0;i<size;i++)
 {
   byte index=(byte)(h^in[i]);
   //simpleSysex(0x61,index,0x00,0x00);
   h=Permutation[index%32];
 } 
 return h;
}
#endif

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
        ChokeNoteSensor[MulSensor]=(40.0/(float)(d_vmean/25))*64.0; //GAIN
        if (ChokeNoteSensor[MulSensor] <16) ChokeNoteSensor[MulSensor]=16;
        
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
       ThresoldSensor[MulSensor]=log_Nmax;
       ScanTimeSensor[MulSensor]=(d_tsum/25)+(sqrt((25*d_tsum2)-(d_tsum*d_tsum))/25);
       MaskTimeSensor[MulSensor]=(d_hsum/25)+(sqrt((25*d_hsum2)-(d_hsum*d_hsum))/25);
       RetriggerSensor[MulSensor]=d_rmin;
       CurveFormSensor[MulSensor]=(1024/d_vmax)*32;
       CurveSensor[MulSensor]=0;
       
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
    SendLog(MulSensor,N,yn_0,MaxRetriggerSensor[MulSensor],MaxReadingSensor[MulSensor],StateSensor[MulSensor]);
  #endif  
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
  if(TypeSensor[i]==SWITCH)
  { 
    simpleSysex(0x6F,i,MaxReadingSensor[i],0);
    
    if(StateSensor[i]==SWITCH_TIME)
    {   
      StateSensor[i]=MASK_TIME;
      MaxReadingSensor[i] = -1;
    }
    return;
  }
  //===============================
  //          YSwitch
  //===============================
  if(TypeSensor[i]==5)
  {
    simpleSysex(0x6F,i,MaxReadingSensor[i],0);
    MaxReadingSensor[i] = -1;
    return;
  }
  //===============================
  //        Piezo, HH
  //===============================
  if (/*(Time-TimeSensor[i]) >= ScanTimeSensor[i]*/ StateSensor[i]==PIEZO_TIME)
  {          
      //Piezo
      if(/*DualSensor(i)!=127 &&*/ TypeSensor[i]==PIEZO)
      {
        simpleSysex(0x6F,i,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),0);
        
        StateSensor[i]=MASK_TIME;
              
        //Piezo-Switch
        if(TypeSensor[DualSensor(i)]==SWITCH && StateSensor[DualSensor(i)]==SWITCH_TIME )
        {
              simpleSysex(0x6F,DualSensor(i),127,0);

              StateSensor[DualSensor(i)]=MASK_TIME;
              MaxReadingSensor[DualSensor(i)] = -1;
         }
      }
      else //HH========================================
        simpleSysex(0x6F,i,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),0);
               
    MaxReadingSensor[i] = -1;
  }
}

//==============================
//    SOFTRESET           
//==============================
void softReset() {
  #if defined(__AVR__)
  asm volatile (" jmp 0");
  #elif defined(__MK20DX256__)
  WRITE_RESTART(0x5FA0004);
  #endif 
//wdt_enable(WDTO_30MS);
}


//==============================
//    FREERAM
//==============================
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


