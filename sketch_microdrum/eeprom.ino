
//==============================
//    EEPROM
//==============================
void LoadAllEEPROM()
{
  byte Version=EEPROM.read(1);
  
  if(Version==8 /*&& Version!=255*/)
  {
    LoadGeneralEEPROM(0);
    LoadGeneralEEPROM(2);
    LoadGeneralEEPROM(3);
    
    for(int h=0;h<12;h++)
      LoadHHEEPROM(h);
    
    for(int i=0;i<48;i++)
      for(int j=0;j<14;j++)
        LoadEEPROM(i,j);
  }
}

void LoadEEPROM(byte Pin,byte Param)
{
  byte Value=EEPROM.read(100+(Pin*16)+Param); 
  if(Value>127) Value=127;
  ExecCommand(0x03,Pin,Param,Value);
}

void SaveEEPROM(byte Pin,byte Param,byte Value)
{
  ExecCommand(0x03,Pin,Param,Value);
  EEPROM.write(100+(Pin*16)+Param, Value);
}

void SaveGeneralEEPROM(byte Param,byte Value)
{
  ExecCommand(0x03,0x7E,Param,Value);
  EEPROM.write(Param,Value);
}

void SaveHHEEPROM(byte Param,byte Value)
{
  ExecCommand(0x03,0x4C,Param,Value);
  EEPROM.write(50+Param,Value); 
}

void LoadGeneralEEPROM(byte Param)
{
  /*delayTime=EEPROM.read(0);
  //1=Version
  //2=NSensor
  GeneralXtalk=EEPROM.read(3);*/
  
  byte Value=EEPROM.read(Param);
  if(Value>127) Value=127;
  ExecCommand(0x03,0x7E,Param,Value);
}

void LoadHHEEPROM(byte Param)
{
  byte Value=EEPROM.read(50+Param);
  if(Value>127) Value=127;
  ExecCommand(0x03,0x4C,Param,Value);
}
