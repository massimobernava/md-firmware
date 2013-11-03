
//==============================
//    SENDLOG
//==============================
void SendLog(byte Sensor,int N,int Reading,int Y0,int MaxReading,byte State)
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
  
  buf[7] = (byte)Reading;
  buf[8] = (byte)(Reading>>8);
  
  buf[9] = (byte)Y0;
  buf[10] = (byte)(Y0>>8);
  
  buf[11] = (byte)MaxReading;
  buf[12] = (byte)(MaxReading>>8);
  
  buf[13] = State;
  
  Sysex(0x6E,buf,14);
  
  
  #if MENU
  if(menuEnabled && eMenuValue==51)
  {
   int lev=Y0/16;
   lcd.write(lev);
  }
  #endif
}

//==============================
//    SENDPROFILING
//==============================
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


//==============================
//    LICENSE
//==============================
void CheckLicense()
{
  //LicenseHash=PearsonHash(Data,2);
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

//==============================
//    PLAYSENSOR TOOLMODE
//==============================
void PlaySensorTOOLMode(int i)
{
  //===============================
  //        Switch
  //===============================
  if(TypeSensor[i]==1)
  {
    simpleSysex(0x6F,i,MaxReadingSensor[i],0);
    MaxReadingSensor[i] = -1;
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
  //====================================================================
  if ((Time-TimeSensor[i]) >= ScanTimeSensor[i] /*|| TypeSensor[i]==1*/)
  {         
    if (MaxReadingSensor[i] > ThresoldSensor[i])
    {
      //Dual
      if(DualSensor[i]!=127 && TypeSensor[i]!=3/*HH*/ && TypeSensor[i]!=4/*HHs*/)
      {
        //Piezo-Piezo
        if(TypeSensor[DualSensor[i]]==0) //Piezo-Piezo
        {
          //Suona solo il primo dei due piezo che arriva, il secondo viene inibito
          if(MaxReadingSensor[i]> (DualThresoldSensor[i]*4) && MaxReadingSensor[DualSensor[i]]<=(DualThresoldSensor[DualSensor[i]]*4))
            simpleSysex(0x6F,i,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),0);
          else if(MaxReadingSensor[i]<= (DualThresoldSensor[i]*4) && MaxReadingSensor[DualSensor[i]]>(DualThresoldSensor[DualSensor[i]]*4))
            simpleSysex(0x6F,DualSensor[i],UseCurve(CurveSensor[DualSensor[i]],MaxReadingSensor[DualSensor[i]],CurveFormSensor[DualSensor[i]]),0);
          else if(MaxReadingSensor[i]> (DualThresoldSensor[i]*4) && MaxReadingSensor[DualSensor[i]]>(DualThresoldSensor[DualSensor[i]]*4))
            simpleSysex(0x6F,i,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),0);
          else if(MaxReadingSensor[i]<= (DualThresoldSensor[i]*4) && MaxReadingSensor[DualSensor[i]]<=(DualThresoldSensor[DualSensor[i]]*4))
            simpleSysex(0x6F,DualSensor[i],UseCurve(CurveSensor[DualSensor[i]],MaxReadingSensor[DualSensor[i]],CurveFormSensor[DualSensor[i]]),0);
        }
        else //Piezo-Switch
        {
          //Se lo switch è stato attivato questo viene inibito altrimenti suona come un piezo normale
          if(MaxReadingSensor[DualSensor[i]]<0 || ZeroCountSensor[DualSensor[i]]>0)
          {
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
      }
      else //Mono========================================
      {
        simpleSysex(0x6F,i,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),0);
      }//Mono=======================
    }//Thresold
   
    MaxReadingSensor[i] = -1;
  }//ScanTime
}

//==============================
//    SOFTRESET
//==============================
void softReset() { asm volatile (" jmp 0"); /*wdt_enable(WDTO_30MS);*/ }


//==============================
//    FREERAM
//==============================
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

