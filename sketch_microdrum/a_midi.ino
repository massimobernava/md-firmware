
//==============================
//    MIDI
//==============================

//=============SYSEX=========
void simpleSysex(byte cmd,byte data1,byte data2,byte data3)
{
#if USE_LCD
  if(cmd==0x6F) DrawDiagnostic(data1,data2);
  #endif
              //      ID    cmd data1 data2 data3
  byte simpleSysex[]= {0x77, 0,  0,    0,    0   };

  simpleSysex[1] = cmd;
  simpleSysex[2] = data1;
  simpleSysex[3] = data2;
  simpleSysex[4] = data3;

  usbMIDI.sendSysEx(5, simpleSysex);
  
  delay(5);
  
  HWSERIAL.write(0xF0);
  HWSERIAL.write(0x77);
  HWSERIAL.write(cmd);
  HWSERIAL.write(data1);
  HWSERIAL.write(data2);
  HWSERIAL.write(data3);
  HWSERIAL.write(0xF7);
}

void Sysex(byte cmd,byte* message,byte size)
{
  HWSERIAL.write(0xF0);
  HWSERIAL.write(0x77);
  HWSERIAL.write(cmd);
  HWSERIAL.write(size);
  HWSERIAL.write(message, size);
  HWSERIAL.write(0xF7);
}
//============================

#if MEGA
void MIDI_TX(byte MESSAGE, byte PITCH, byte VELOCITY)
{
  //status = MESSAGE + midichannel;
  Serial.write((MESSAGE));
  Serial.write(PITCH);
  Serial.write(VELOCITY);
}
#endif
