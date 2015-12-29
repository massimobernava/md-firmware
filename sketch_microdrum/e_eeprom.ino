
//==============================
//    EEPROM
//==============================

#if defined(__arm__) 
//TODO: https://github.com/sebnil/DueFlashStorage
#endif

void LoadAllEEPROM()
{
  #if defined(__AVR__)
  byte Version=EEPROM.read(1);
  
  if(Version==8 /*&& Version!=255*/)
  {
    LoadGeneralEEPROM(0);
    LoadGeneralEEPROM(2);
    LoadGeneralEEPROM(3);
    
    for(int h=0;h<12;h++)
      LoadHHEEPROM(h);
    
    for(int i=0;i<NPin;i++)
      for(int j=0;j<14;j++)
        LoadEEPROM(i,j);
  }
  else
  {
    SaveGeneralEEPROM(0);
    SaveGeneralEEPROM(2);
    SaveGeneralEEPROM(3);
    
    for(int h=0;h<12;h++)
      SaveHHEEPROM(h);
    
    for(int i=0;i<NPin;i++)
      for(int j=0;j<14;j++)
        SaveEEPROM(i,j);
        
    EEPROM.write(1, 8);
  }
  #endif
}

void LoadEEPROM(byte Pin,byte Param)
{
  #if defined(__AVR__)
  byte Value=EEPROM.read(100+(Pin*16)+Param); 
  if(Value>127) Value=127;
  ExecCommand(0x03,Pin,Param,Value);
  #endif
}

void SaveEEPROM(byte Pin,byte Param,byte Value)
{
  #if defined(__AVR__)
  ExecCommand(0x03,Pin,Param,Value);
  EEPROM.write(100+(Pin*16)+Param, Value);
  #endif
}

void SaveEEPROM(byte Pin,byte Param)
{
  #if defined(__AVR__)
  EEPROM.write(100+(Pin*16)+Param, GetPinSetting(Pin,Param));
  #endif
}

void SaveGeneralEEPROM(byte Param,byte Value)
{
  #if defined(__AVR__)
  ExecCommand(0x03,0x7E,Param,Value);
  EEPROM.write(Param,Value);
  #endif
}

void SaveGeneralEEPROM(byte Param)
{
  #if defined(__AVR__)
  byte Value=0;
  switch(Param)
  {
    case 0x00: //Delay
      Value=delayTime/2;
    break;

    case 0x02: //NSensor
      Value=NSensor;
      break;
    
    case 0x03: //GeneralXtalk
      Value=GeneralXtalk;
     break;
  }
  if(Value>127) Value=127;
  EEPROM.write(Param,Value);
  #endif
}

void SaveHHEEPROM(byte Param,byte Value)
{
  #if defined(__AVR__)
  ExecCommand(0x03,0x4C,Param,Value);
  EEPROM.write(50+Param,Value); 
  #endif
}

void SaveHHEEPROM(byte Param)
{
  #if defined(__AVR__)
  byte Value=0;
  
  if(Param<4)Value=HHNoteSensor[Param];
   else if(Param<8)Value=HHThresoldSensor[Param-4];
   else if(Param<10) Value=HHFootNoteSensor[Param-8];
   else Value=HHFootThresoldSensor[Param-10];
   
  if(Value>127) Value=127;
  EEPROM.write(50+Param,Value); 
  #endif
}

void LoadGeneralEEPROM(byte Param)
{
  /*delayTime=EEPROM.read(0);
  //1=Version
  //2=NSensor
  GeneralXtalk=EEPROM.read(3);*/
  #if defined(__AVR__)
  byte Value=EEPROM.read(Param);
  if(Value>127) Value=127;
  ExecCommand(0x03,0x7E,Param,Value);
  #endif
}

void LoadHHEEPROM(byte Param)
{
  #if defined(__AVR__)
  byte Value=EEPROM.read(50+Param);
  if(Value>127) Value=127;
  ExecCommand(0x03,0x4C,Param,Value);
  #endif
}
