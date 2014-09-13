
//==============================
//    EEPROM
//==============================

#if defined(__arm__) 
/* These two functions help us write to the 24LC256 EEPROM chip */
#define EEPROM_ADDR 0x50
void EEPROM_write(unsigned int addr,byte data) {
  int rdata = data;
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((int)(addr >> 8));       // MSB
  Wire.write((int)(addr & 0xFF));     // LSB
  Wire.write(rdata);
  Wire.endTransmission();
  //Serial.print("EEPROM write: addr: ");
  //Serial.print(addr);
  //Serial.print(" ");
  //Serial.println(data);
  delay(5);
}

byte EEPROM_read(unsigned int addr) {
  byte data = 0xFF;
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((int)(addr >> 8));       // MSB
  Wire.write((int)(addr & 0xFF));     // LSB
  Wire.endTransmission();
  Wire.requestFrom(EEPROM_ADDR,1);
  if (Wire.available()) data = Wire.read();
  //Serial.print("EEPROM read: addr: ");
  //Serial.print(addr);
  //Serial.print(" ");
  //Serial.println(data);
  delay(5);
  return data;
}
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

void SaveHHEEPROM(byte Param,byte Value)
{
  #if defined(__AVR__)
  ExecCommand(0x03,0x4C,Param,Value);
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
