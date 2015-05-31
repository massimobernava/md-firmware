//=============SYSEX=========
void simpleSysex(byte cmd,byte data1,byte data2,byte data3)
{
               //      begin  ID    cmd data1 data2 data3 End
  byte simpleSysex[]= {0xF0,  0x77, 0,  0,    0,    0,    0xF7};

  simpleSysex[3] = cmd;
  simpleSysex[4] = data1;
  simpleSysex[5] = data2;
  simpleSysex[6] = data3;
  
  #if USB_MIDI
   usbMIDI.sendSysEx(7, simpleSysex);
   #else
   Serial1.write(7,simpleSysex);
  #endif
   
  #if MENU
  if(cmd==0x6F) DrawDiagnostic(data1,data2);
  #endif 
 
  #if Serial_Debug
   Serial.println("simpleSysex ");
    for(int i=0; i<8; i++)
    {Serial.print(simpleSysex[i]); Serial.print(" ");}
    Serial.println();
  #endif
}
/*
Sysex is hard coded into SendLog and PROF functions.
I might add it back, for the sake of possible future expansion of its function.
The origional SendLog and PROF functions would need to be added back.
If so here is the function:

void Sysex(byte cmd,byte* buf,byte size)
{
  if (size == 14) //SendLog
  { 
    byte message[]= {0xF0,0x77,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0xF7};
    message[2]=cmd;
    message[3]=size;
    for(int i=4; i<19; i++) {message[i]=buf[i-4];}// could be a problem
  
   #if USB_MIDI
    usbMIDI.sendSysEx(19, message);
    #else
    Serial1.write(19,message);
   #endif
  }
  
  else if (size == 8) //PROF
  { 
    byte message[]= {0xF0,0x77,0,0,1,1,1,1,1,1,1,1,0xF7};
    message[2]=cmd;
    message[3]=size;
    for(int i=4; i<14; i++) {message[i]=buf[i-4];}// could be a problem
  
   #if USB_MIDI
    usbMIDI.sendSysEx(13, message);
    #else
    Serial1.write(13,message);
   #endif
  }
  else
  {//error};
}
*/
