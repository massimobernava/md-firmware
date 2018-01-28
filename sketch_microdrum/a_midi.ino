
//==============================
//    MIDI
//==============================

#if TEENSY
#if ENABLE_CHANNEL
  #define fastNoteOn(_channel,_note,_velocity) { usbMIDI.sendNoteOn(_note, _velocity, 0x90 | _channel);          usbMIDI.sendNoteOff(_note, _velocity, 0x90 | _channel);   HWSERIAL.write(0x90 | _channel);  HWSERIAL.write(_note); HWSERIAL.write(_velocity); /*note off*/ HWSERIAL.write(0x90 | _channel);  HWSERIAL.write(_note); HWSERIAL.write(0);       /*Serial.println(String("Note On:  ch=") + _channel + ", note=" + _note + ", velocity=" + _velocity); Serial.println();*/}
  #define fastMidiCC(_channel,_number,_value) {  usbMIDI.sendControlChange(_number, _value, (_channel));                                                                   HWSERIAL.write((0xB0 | _channel)); HWSERIAL.write(_number); HWSERIAL.write(_value);  /*Serial.println(String("Note On:  ch=") + _channel + ", note=" + _number + ", value=" + _value);Serial.println();*/}
#else
  #define fastNoteOn(_channel,_note,_velocity) { usbMIDI.sendNoteOn(_note, _velocity, 0x90 | 0x09);              usbMIDI.sendNoteOff(_note, _velocity, 0x90 | 0x09);       HWSERIAL.write(0x90 | 0x09); HWSERIAL.write(_note); HWSERIAL.write(_velocity);       /*Serial.println("channel---note---velocity "); Serial.print(0x90 | _channel); Serial.print(_note); Serial.println(_velocity); Serial.println();*/}
  #define fastMidiCC(_channel,_number,_value) {  usbMIDI.sendControlChange(_number, _value, (0xB0 | 0x09));                                                                HWSERIAL.write((0xB0 | 0x09)); HWSERIAL.write(_number); HWSERIAL.write(_value); }
#endif
#else
#if ENABLE_CHANNEL
  #define fastNoteOn(_channel,_note,_velocity) { Serial.write(0x90 | _channel);Serial.write(_note);Serial.write(_velocity); }
  #define fastMidiCC(_channel,_number,_value) { Serial.write((0xB0 | _channel)); Serial.write(_number); Serial.write(_value); }
#else
  #define fastNoteOn(_channel,_note,_velocity) { Serial.write(0x90 | 0x09); Serial.write(_note); Serial.write(_velocity);}
  #define fastMidiCC(_channel,_number,_value) { Serial.write((0xB0 | 0x09)); Serial.write(_number); Serial.write(_value); }
#endif
#endif

//=============SYSEX=========
void simpleSysex(byte cmd,byte data1,byte data2,byte data3)
{
#if USE_LCD
  if(cmd==0x6F) DrawDiagnostic(data1,data2);
  #endif
              //      ID    cmd data1 data2 data3
  byte simpleSysex[]= {0x77, 0,  0,    0,    0   };

#if TEENSY
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

  #else
  
  Serial.write(0xF0);
  Serial.write(0x77);
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
  Serial.write(data3);
  Serial.write(0xF7);
  #endif
}

void Sysex(byte cmd,byte* message,byte size)
{
  #if TEENSY
  HWSERIAL.write(0xF0);
  HWSERIAL.write(0x77);
  HWSERIAL.write(cmd);
  HWSERIAL.write(size);
  HWSERIAL.write(message, size);
  HWSERIAL.write(0xF7);
  #else
  Serial.write(0xF0);
  Serial.write(0x77);
  Serial.write(cmd);
  Serial.write(size);
  Serial.write(message, size);
  Serial.write(0xF7);
  #endif
}
//============================

#if NO_MUX
void MIDI_TX(byte MESSAGE, byte PITCH, byte VELOCITY)
{
  //status = MESSAGE + midichannel;
  Serial.write((MESSAGE));
  Serial.write(PITCH);
  Serial.write(VELOCITY);
}

void noteOn(byte channel, byte note, byte velocity) {
    Serial.write(0x90 | channel);
    Serial.write(note);
    Serial.write(velocity);
}
#endif
