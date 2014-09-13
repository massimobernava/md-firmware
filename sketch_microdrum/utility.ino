
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

//==============================
//    SENDPROFILING
//==============================
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
   //autodeterminazione del rumore massimo Nmax in un intervallo fisso di 5s
    if(log_state==0)
    {
      log_T1=TIMEFUNCTION;
      log_Nmax=yn_0;
      log_state=1;
    }
    else if(log_state==1)
    {
      if(yn_0>log_Nmax) log_Nmax=yn_0;
      if((log_T1+10000)<TIMEFUNCTION) log_state=2;
    }
    else if(log_state==2)
    {
      DrawLog(0);
      if(yn_0>log_Nmax)
      {
        log_T1=TIMEFUNCTION;
        log_state=3;
        log_Vmax=0;
        log_note=0;
      }
    }
    else if(log_state==3)
    {
      if(log_oldState==0 && StateSensor[MulSensor]==1) log_note++;
      log_oldState=StateSensor[MulSensor];
    
      if(yn_0>log_Nmax)
      {
        DrawLog(0);
        log_T2=TIMEFUNCTION;
        if(yn_0>log_Vmax)
        {
          log_Vmax=yn_0;
          log_Tmax=log_T2;

          log_T80=0;
          log_T70=0;
          log_T60=0;
          log_T50=0;
          log_T40=0;
          log_T30=0;
          log_T20=0;
        }
        else
        {
          //if(((yn_0-log_VMax)/(log_T2-log_Tmax))>log_Dmax)
          //  log_Dmax=((yn_0-log_VMax)/(log_T2-log_Tmax));
          if(yn_0>(int)((float)log_Vmax*0.8))
            log_T80=log_T2;
          else if(yn_0>(int)((float)log_Vmax*0.7))
            log_T70=log_T2;
          else if(yn_0>(int)((float)log_Vmax*0.6))
            log_T60=log_T2;
          else if(yn_0>(int)((float)log_Vmax*0.5))
            log_T50=log_T2;
          else if(yn_0>(int)((float)log_Vmax*0.4))
            log_T40=log_T2;
          else if(yn_0>(int)((float)log_Vmax*0.3))
            log_T30=log_T2;
          else if(yn_0>(int)((float)log_Vmax*0.2))
            log_T20=log_T2;
          }
      }
      if((TIMEFUNCTION-log_T1)>1000)
      {
        log_state=4;
        N=0;
      }
    }
    else if(log_state==4)
    {
      if(N==0) DrawLog(0);
      else if(N==2000) DrawLog(1);
      else if(N==4000) DrawLog(2);
      else if(N==6000) DrawLog(3);
    
      N++;
      if(N==8000) N=0;
      if(yn_0>log_Nmax)
      {
        log_T1=TIMEFUNCTION;
        log_state=3;
        log_Vmax=0;
        log_note=0;
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
  /*
  if ((Time-TimeSensor[i]) >= ScanTimeSensor[i] )
  {         
    //if (MaxReadingSensor[i] > ThresoldSensor[i])
    {
      //Dual
      if(TypeSensor[i]==PIEZO)
      {
        //Piezo-Piezo
        if(TypeSensor[DualSensor(i)]==PIEZO) //Piezo-Piezo
        {
          //DUAL
          if(MaxReadingSensor[DualSensor(i)]>MaxReadingSensor[i])
          {
            MaxReadingSensor[i]=-1;
            return;
          }
          else
          {
            simpleSysex(0x6F,i,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),0);
            MaxReadingSensor[DualSensor(i)]=-1;  //Dual XTalk
          }
        }
        else if(TypeSensor[DualSensor(i)]==SWITCH)//Piezo-Switch
        {
          //Se lo switch è stato attivato questo viene inibito altrimenti suona come un piezo normale
          if(MaxReadingSensor[DualSensor(i)]<0)
          {
            simpleSysex(0x6F,i,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),0);
            //simpleSysex(0x6F,DualSensor(i),ZeroCountSensor[DualSensor(i)],0);
            MaxReadingSensor[i] = -1;
            
            //STOP SWITCH
            MaxReadingSensor[DualSensor(i)]=-1;
            StateSensor[DualSensor(i)]=0;
          } 
          else
           {
             simpleSysex(0x6F,DualSensor(i),UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),0);
             
             MaxReadingSensor[i] = -1;
             //Lo mettiamo in mask
             TimeSensor[i]=Time-ScanTimeSensor[i];
            
            //STOP SWITCH
            MaxReadingSensor[DualSensor(i)]=-1;
            StateSensor[DualSensor(i)]=NORMAL_TIME;
           }   
          return;
        }
        else
          simpleSysex(0x6F,i,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),0);
      }
      else //Mono========================================
      {
        simpleSysex(0x6F,i,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),0);
      }//Mono=======================
    }//Thresold
   
    MaxReadingSensor[i] = -1;
  }//ScanTime*/
}

//==============================
//    SOFTRESET
//==============================
void softReset() {
  #if defined(__AVR__)
  asm volatile (" jmp 0");
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


