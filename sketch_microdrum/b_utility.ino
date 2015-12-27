

//=================================
//   MENU LOG
//=================================
#if MENU_LOG
byte log_state=0;
byte log_Nmax=0;  //Max noise

unsigned long log_T1=0; //Start hit time
//unsigned long log_T2=0;
unsigned long log_Tmax=0;  //Max hit time
int log_Vmax=0;   //Max hit value

//byte log_note=0;
//byte log_oldState=0;
//short log_Show=0;

/*unsigned long log_T80=0;
unsigned long log_T70=0;
unsigned long log_T60=0;*/
unsigned long log_T50=0;  //Half value hit time
/*unsigned long log_T40=0;
unsigned long log_T30=0;
unsigned long log_T20=0;*/

unsigned long d_hsum=0;  //Sum Tmax
unsigned long d_tsum=0;  //Sum T50
unsigned long d_hsum2=0;  //Sum Tmax^2
unsigned long d_tsum2=0;  //Sum T50^2
byte d_tnum=0;  //Hit number
byte d_rmin=0;
int d_vmax=0; //Total max hit value
int d_vmin=1024;  //Total min hit value
int d_vmean=0;  //Total min hit value
#endif
//=================================

//===============================
//   LOG
//===============================
byte LogPin=0xFF;
byte LogThresold=0xFF;

short N=0;//Unsent log

bool Diagnostic=false;
//===============================

//===============================
//  PRESCALER
//===============================
// Maximum sampling frequency    // Resolution
enum Prescaler {
  Prescaler_2 = B00000000, // 16 MHz / 2 = 8 MHz            //
  Prescaler_4 = B00000010, // 16 MHz / 4 = 4 MHz            // ~5.9
  Prescaler_8 = B00000011, // 16 MHz / 8 = 2 MHz            // ~7.4
  Prescaler_16 = B00000100, // 16 MHz / 16 = 1 MHz           // ~8.6
  Prescaler_32 = B00000101, // 16 MHz / 32 = 500 kHz         // ~8.9
  Prescaler_64 = B00000110, // 16 MHz / 64 = 250 kHz         // ~9.0
  Prescaler_128 = B00000111, // 16 MHz / 128 = 125 kHz        // ~9.1
};

inline void setPrescaler(int prescaler) {
  ADCSRA &= B11111000;
  ADCSRA |= prescaler;
}
//===============================

//==============================
//   PROFILING 
//==============================
#define PROFA TimeProfA=micros();
#define PROFB TimeProf+=(micros()-TimeProfA); NProf++;
#if USE_PROFILER
unsigned long TimeProfA;
unsigned long TimeProf=0;
unsigned long NProf=0;
#endif
//==============================

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
#if USE_PROFILER
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


