//==============================
//    MIDI
//==============================
void noteOn(byte channel, byte note, byte velocity) {

    //OTTIMIZZA
   // fastWrite(ledPin,HIGH);
    Serial.write(0x90 | channel);//Ottimizzazione
    Serial.write(note);
    Serial.write(velocity);
    //digitalWrite(ledPin,LOW);
    //fastWrite(ledPin,LOW);
}

void noteOff(byte channel, byte note, byte velocity) {
  midiMsg( (0x80 | channel), note, velocity);
}

void midiCC(byte channel, byte number, byte value) {
  //midiMsg( (0xB0 | channel), number, value);
    Serial.write((0xB0 | channel));//Ottimizzazione
    Serial.write(number);
    Serial.write(value);
}

void midiMsg(byte cmd, byte data1, byte data2) {
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
}
//SYSEX========================================
void simpleSysex(byte cmd,byte data1,byte data2,byte data3)
{
  //OTTIMIZZA
  Serial.write(0xF0);
  Serial.write(0x77);
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
  Serial.write(data3);
  Serial.write(0xF7);
}
void Sysex(byte cmd,byte* message,byte size)
{
  Serial.write(0xF0);
  Serial.write(0x77);
  Serial.write(cmd);
  Serial.write(size);
  Serial.write(message, size);
  Serial.write(0xF7);
}
//============================
