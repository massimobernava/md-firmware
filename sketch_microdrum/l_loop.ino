
//==============================
//    FASTSCAN
//==============================
#define fastScan(sensor,count) { byte pin=count+(sensor<<3); if(Pin[pin].Type!=Disabled) { Pin[pin].scan(sensor,count); if(Pin[pin].State==Scan_Time) { Pin[pin].scan(sensor,count); Pin[pin].scan(sensor,count); Pin[pin].scan(sensor,count);}}}

//==============================
//     LOOP
//==============================
void loop()
{
  //simpleSysex(0xFF,Mode,0x00,0x00);
  Input();
  #if USE_LCD
  Menu();
  #endif
  
  if(Mode==Off)
  {
    delay(100);
    return;
  }

//---------------------------------------------------------------------------------------
//      LÊ OS 16 PINOS ANALOGICOS DO ARDUINO MEGA SEM MULTIPLEX E PINOS DIGITAIS
//---------------------------------------------------------------------------------------
#if MEGA
 // --- Uso dos Pinos Digitais para choke e padas adicionais ---
 #if USE_DIG 
  // Lê os pinos digitais
  #if TEENSY
  Choke_A.update();
  Choke_B.update();
  Choke_C.update();
  Choke_R.update();
  AUX_A.update();
  AUX_B.update();
  AUX_C.update();
  AUX_D.update();
  AUX_E.update();
  AUX_F.update();
  AUX_G.update();

    if (Choke_A.fallingEdge()) {usbMIDI.sendNoteOn(Choke1, 127, (0x90 | 144));}
    if (Choke_B.fallingEdge()) {usbMIDI.sendNoteOn(Choke2, 127, (0x90 | 144));}
    if (Choke_C.fallingEdge()) {usbMIDI.sendNoteOn(Choke3, 127, (0x90 | 144));}
    if (Choke_R.fallingEdge()) {usbMIDI.sendNoteOn(ChokeRide, 127, (0x90 | 144));}
    if (AUX_A.fallingEdge()) {usbMIDI.sendNoteOn(Aux1, 127, (0x90 | 144));}
    if (AUX_B.fallingEdge()) {usbMIDI.sendNoteOn(Aux2, 127, (0x90 | 144));}
    if (AUX_C.fallingEdge()) {usbMIDI.sendNoteOn(Aux3, 127, (0x90 | 144));}
    if (AUX_D.fallingEdge()) {usbMIDI.sendNoteOn(Aux4, 127, (0x90 | 144));}
    if (AUX_E.fallingEdge()) {usbMIDI.sendNoteOn(Aux5, 127, (0x90 | 144));}
    if (AUX_F.fallingEdge()) {usbMIDI.sendNoteOn(Aux6, 127, (0x90 | 144));}
    if (AUX_G.fallingEdge()) {usbMIDI.sendNoteOn(Aux7, 127, (0x90 | 144));}

    if (Choke_A.risingEdge()) {usbMIDI.sendNoteOff(Choke1, 0, (0x90 | 128));}
    if (Choke_B.risingEdge()) {usbMIDI.sendNoteOff(Choke2, 0, (0x90 | 128));}
    if (Choke_C.risingEdge()) {usbMIDI.sendNoteOff(Choke3, 0, (0x90 | 128));}
    if (Choke_R.risingEdge()) {usbMIDI.sendNoteOff(ChokeRide, 0, (0x90 | 128));}
    if (AUX_A.risingEdge()) {usbMIDI.sendNoteOff(Aux1, 0, (0x90 | 128));}
    if (AUX_B.risingEdge()) {usbMIDI.sendNoteOff(Aux2, 0, (0x90 | 128));}
    if (AUX_C.risingEdge()) {usbMIDI.sendNoteOff(Aux3, 0, (0x90 | 128));}
    if (AUX_D.risingEdge()) {usbMIDI.sendNoteOff(Aux4, 0, (0x90 | 128));}
    if (AUX_E.risingEdge()) {usbMIDI.sendNoteOff(Aux5, 0, (0x90 | 128));}
    if (AUX_F.risingEdge()) {usbMIDI.sendNoteOff(Aux6, 0, (0x90 | 128));}
    if (AUX_G.risingEdge()) {usbMIDI.sendNoteOff(Aux7, 0, (0x90 | 128));}
    
  #else
  
  // CHOKE PRATO 01
  currentSwitchState = digitalRead(Choke1_Pin);   if( currentSwitchState == LOW && Choke1_State == HIGH )/*push*/MIDI_TX(0x90 | 144,  Choke1, 127);
  if( currentSwitchState == HIGH && Choke1_Pin == LOW ) /*release*/ MIDI_TX(0x90 | 128, Choke1, 127); Choke1_State = currentSwitchState;
  
  // CHOKE PRATO 02
  currentSwitchState = digitalRead(Choke2_Pin);   if( currentSwitchState == LOW && Choke2_State == HIGH )/*push*/MIDI_TX(0x90 | 144,  Choke2, 127);
  if( currentSwitchState == HIGH && Choke2_Pin == LOW ) /*release*/ MIDI_TX(0x90 | 128, Choke2, 127); Choke2_State = currentSwitchState;
  
  // CHOKE PRATO 03
  currentSwitchState = digitalRead(Choke3_Pin);   if( currentSwitchState == LOW && Choke3_State == HIGH )/*push*/MIDI_TX(0x90 | 144,  Choke3, 127);
  if( currentSwitchState == HIGH && Choke3_Pin == LOW ) /*release*/ MIDI_TX(0x90 | 128, Choke3, 127); Choke3_State = currentSwitchState;
  
  // CHOKE RIDE
    currentSwitchState = digitalRead(ChokeRide_Pin);   if( currentSwitchState == LOW && ChokeRide_State == HIGH )/*push*/MIDI_TX(0x90 | 144,  ChokeRide, 127);
  if( currentSwitchState == HIGH && ChokeRide_Pin == LOW ) /*release*/ MIDI_TX(0x90 | 128, ChokeRide, 127); ChokeRide_State = currentSwitchState;
  
  // AUX 1
  currentSwitchState = digitalRead(Aux1_Pin);   if( currentSwitchState == LOW && Aux1_State == HIGH )/*push*/MIDI_TX(0x90 | 144,  Aux1, 127);
  if( currentSwitchState == HIGH && Aux1_State == LOW ) /*release*/ MIDI_TX(0x90 | 128, Aux1, 127); Aux1_State = currentSwitchState;
  
  // AUX 2
  currentSwitchState = digitalRead(Aux2_Pin);   if( currentSwitchState == LOW && Aux2_State == HIGH )/*push*/MIDI_TX(0x90 | 144,  Aux2, 127);
  if( currentSwitchState == HIGH && Aux2_State == LOW ) /*release*/ MIDI_TX(0x90 | 128, Aux2, 127); Aux2_State = currentSwitchState;

  // AUX 3
  currentSwitchState = digitalRead(Aux3_Pin);   if( currentSwitchState == LOW && Aux3_State == HIGH )/*push*/MIDI_TX(0x90 | 144,  Aux3, 127);
  if( currentSwitchState == HIGH && Aux3_State == LOW ) /*release*/ MIDI_TX(0x90 | 128, Aux3, 127); Aux3_State = currentSwitchState;
  Aux3_State = currentSwitchState;

  // AUX 4
  currentSwitchState = digitalRead(Aux4_Pin);   if( currentSwitchState == LOW && Aux4_State == HIGH )/*push*/MIDI_TX(0x90 | 144,  Aux4, 127);
  if( currentSwitchState == HIGH && Aux4_State == LOW ) /*release*/ MIDI_TX(0x90 | 128, Aux4, 127); Aux4_State = currentSwitchState;

  // AUX 5
  currentSwitchState = digitalRead(Aux5_Pin);   if( currentSwitchState == LOW && Aux5_State == HIGH )/*push*/MIDI_TX(0x90 | 144,  Aux5, 127);
  if( currentSwitchState == HIGH && Aux5_State == LOW ) /*release*/ MIDI_TX(0x90 | 128, Aux5, 127); Aux5_State = currentSwitchState;
  
  // AUX 6
  currentSwitchState = digitalRead(Aux6_Pin);   if( currentSwitchState == LOW && Aux6_State == HIGH )/*push*/MIDI_TX(0x90 | 144,  Aux6, 127);
  if( currentSwitchState == HIGH && Aux6_State == LOW ) /*release*/ MIDI_TX(0x90 | 128, Aux6, 127); Aux6_State = currentSwitchState;
    
  // AUX 7
  currentSwitchState = digitalRead(Aux7_Pin); if( currentSwitchState == LOW && Aux7_State == HIGH ) /*push*/ MIDI_TX(0x90 | 144,  Aux7, 127);
  if( currentSwitchState == HIGH && Aux7_State == LOW ) /*release*/ MIDI_TX(0x90 | 128, Aux7, 127); Aux7_State = currentSwitchState;
#endif
#endif
// --- Lê os pinos analógicos

  for(byte Sensor=0;Sensor<NPin;Sensor++)
  {
    if(Sensor != (10 || 11)); // comment out
    {
   if (Pin[Sensor].Type != Disabled)
   {
     Pin[Sensor].scan(Sensor,0);
     if (Pin[Sensor].State == Scan_Time)
     {
      Pin[Sensor].scan(Sensor,0);
      Pin[Sensor].scan(Sensor,0);
      Pin[Sensor].scan(Sensor,0);
      }
   }
   Pin[Sensor].play(Sensor,&Pin[DualSensor(Sensor)]);
   #if USE_LCD
      /*if (Pin[Sensor].State == Scan_Time)  
      {
        lcd.setCursor(Sensor, 1);
        lcd.print((char) spChar[Sensor]);
        tmChar[Sensor] = millis(); 
      }
      if (Pin[Sensor].State == Normal_Time)
      {
        if ((millis() - tmChar[Sensor]) > 1000)
        {
          lcd.setCursor(Sensor,1);
          lcd.print(' ');
        }
      }*/
    }
  }
   #endif
  
#else

  // --- Lê os multiplex / Microdrum original
   
  //==========UNROLLING======
  //{0, 1, 3, 2, 6, 7, 5, 4}
  //=========================
  
  //0
  fastWrite(2,0);/*fastWrite(3,0);fastWrite(4,0);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,0); }
  
  //1
  /*fastWrite(2,0);fastWrite(3,0);*/fastWrite(4,1);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,1); }
  
  //3
  /*fastWrite(2,0);*/fastWrite(3,1);/*fastWrite(4,1);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,3); }
  
  //2
  /*fastWrite(2,0);fastWrite(3,1);*/fastWrite(4,0);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,2); }
  
  //6
  fastWrite(2,1);/*fastWrite(3,1);fastWrite(4,0);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,6); }
  
  //7
  /*fastWrite(2,1);fastWrite(3,1);*/fastWrite(4,1);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,7); }
  
  //5
  /*fastWrite(2,1);*/fastWrite(3,0);/*fastWrite(4,1);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,5); }
  
  //4
  /*fastWrite(2,1);fastWrite(3,0);*/fastWrite(4,0);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastScan(Sensor,4); }
  
  //===============================
  if (Mode==Standby) return;
  //===============================

  //Time=TIMEFUNCTION;
  for(byte i=0;i<(NSensor*8);i++)
  {
    Pin[i].play(i,&Pin[DualSensor(i)]);
  }
#endif
  //RESET XTALK
  for(int i=0;i<8;i++)
    MaxMultiplexerXtalk[i]=-1;
  for(int i=0;i<NXtalkGroup;i++)
    MaxXtalkGroup[i]=-1;
    //RESET XTALK
  //for(int i=0;i<8;i++)
   // MaxMultiplexerXtalk[i]=MaxXtalkGroup[i]=-1;
}

//==============================
//    LOGTOOL
//==============================
void LogTool(int yn_0,byte MulSensor)
{
  #if MENU_LOG
  //FASE 1: autodeterminazione del rumore massimo Nmax in un intervallo fisso di 20s
  if(log_state==0)//FASE 1.a: AVVIO
  {
    log_T1=TIMEFUNCTION;  //Start hit time
    log_Nmax=yn_0;        //Max noise
    log_state=1;

    //V2
    Pin[MulSensor].Gain=64;
    Pin[MulSensor].CurveForm=32;
    Pin[MulSensor].Curve=Linear;
       
    d_hsum=0;
    d_tsum=0;
    d_tnum=0;     //Hit number
    //d_rmin=127;
    d_vmax=0;
    d_vmin=1024;
    //V2 d_vmean=0;
      
    N=0;
      
    //WAIT
  #if USE_LCD  
    DrawLog(0);
  #endif
  }
  else if(log_state==1)//FASE 1.b: ANALISI
  {
    if(yn_0>log_Nmax) { log_Nmax=yn_0*1.2; 
    #if USE_LCD
      DrawLog(4);//Determiniamo qual'è il massimo valore di input a riposo
    #endif
    }
    if((log_T1+20000)<TIMEFUNCTION) //20sec
    {
      log_state=2;
      #if USE_LCD
      if(d_tnum<25) 
         DrawLog(1); //25 hit
      else 
       DrawLog(2);
      #endif  
    }
  }
  else if(log_state==2) //FASE 1.c: FINE
  {
    //FASE 2: analisi del segnale per 1s
    if(yn_0>log_Nmax) //Il segnale inizia al superamento della soglia
    {
      #if USE_LCD
        DrawLog(0);//Wait
      #endif  
      N++; //Numero di letture del segnale
      log_Tmax=log_T1=TIMEFUNCTION; //FASE 2.a: inizio dell'analisi del singolo colpo
      log_state=3;
      log_Vmax=yn_0;
    }
  }
  else if(log_state==3)
  {
    if(yn_0>log_Nmax)
    {
      N++;
      //log_T2=TIMEFUNCTION; //Dopo quanto tempo scende sotto la soglia
      if(yn_0>log_Vmax)  //Salita: il segnale aumenta di intensità, cerchiamo il max
      {
        log_Vmax=yn_0;          //Valore massimo
        log_Tmax=TIMEFUNCTION;  //Istante di realizzazione del valore massimo
        log_T50=0;
      }
      else //Discesa: il segnale diminuisce di intensità, cerchiamo di determinare l'intervallo di dimezzamento
      {
        if(yn_0>(int)((float)log_Vmax*0.5))
          log_T50=TIMEFUNCTION;
      }
    }
      
    //1 sec
    if((TIMEFUNCTION-log_T1) > 1000)//FASE 2.b: fine dell'analisi del singolo colpo
    {
      log_state=4;
      //N=0;
        
      //if((log_Tmax-log_T1) > d_tmax) d_tmax=(log_Tmax-log_T1);
      d_hsum+=(log_T50==0?0:(log_T50-log_Tmax));
      d_tsum+=(log_Tmax-log_T1);
      //V2 d_hsum2+=(log_T50==0?0:log_T50-log_Tmax)*(log_T50==0?0:log_T50-log_Tmax);
      //V2 d_tsum2+=(log_Tmax-log_T1)*(log_Tmax-log_T1);
      d_tnum++; //Hit number
        
      //V2 if(((log_Vmax-log_Nmax)/N)<d_rmin) d_rmin=((log_Vmax-log_Nmax)/N);
      if(log_Vmax>d_vmax) d_vmax=log_Vmax;
      if(log_Vmax<d_vmin) d_vmin=log_Vmax;
      //V2 d_vmean+=log_Vmax;
        
      #if USE_LCD
      if(d_tnum<25) 
         DrawLog(1);
      else if(d_tnum<50) 
         DrawLog(2);
      #endif    
    }
  }
  else if(log_state==4) //FASE 3: report
  {
    //Se è finita la prima parte dell'analisi (d_tnum==25) : determinazione del valore di Gain
    //Se è finita la seconda parte dell'analisi (d_tnum==50) : determinazione degli altri parametri
    //Altrimenti ricomincia la fase 2

    if(d_tnum==25)
    {
      //V2
      
      Pin[MulSensor].Gain=(16.0/(float)d_vmin)*64.0;  //V2
      if (Pin[MulSensor].Gain <16) Pin[MulSensor].Gain=16;
      Pin[MulSensor].Thresold=((float)(d_vmin-log_Nmax))*((float)Pin[MulSensor].Gain/64.0);
      Pin[MulSensor].CurveForm=((32.0*64.0)/(float)Pin[MulSensor].Gain)-1;
        
      log_T1=TIMEFUNCTION;
      log_Nmax=0;
      log_state=1;    //ritorna a FASE 1.b: ANALISI
      
      d_hsum=0;
      d_tsum=0;
      //V2 d_hsum2=0;
      //V2 d_tsum2=0;
        
      //V2 d_rmin=127;
      d_vmax=0;
      
      N=0;
      #if USE_LCD
       DrawLog(0);//WAIT
      #endif 
    }
    else if(d_tnum==50)
    {
      //Determiniamo i restanti parametri e concludiamo l'analisi
       
      //V2: Pin[MulSensor].Thresold=log_Nmax;
      Pin[MulSensor].ScanTime=((float)d_tsum/25.0);//V2 +(sqrt((25*d_tsum2)-(d_tsum*d_tsum))/25);
      Pin[MulSensor].MaskTime=((float)d_hsum/25.0);//V2+(sqrt((25*d_hsum2)-(d_hsum*d_hsum))/25);
      Pin[MulSensor].Retrigger= (d_vmax*8.0)/((float)d_hsum/25.0); //V2: d_rmin;
      Pin[MulSensor].CurveForm= max(Pin[MulSensor].CurveForm,(1024.0/(float)d_vmax)*32); //Eventuale correzione
      Pin[MulSensor].Curve=Linear;
       
      #if USE_LCD
        DrawLog(3);
      #endif  
      log_state=5; //END
    }
    else if(yn_0>log_Nmax) //FASE 2.a: INIZIO
    {
      log_Tmax=log_T1=TIMEFUNCTION;
      log_state=3;
      log_Vmax=yn_0;
      //log_note=0;

      N=0;   
      #if USE_LCD
        DrawLog(0);
      #endif  
    }
  }
  #else
    N++;
    if(yn_0>=(LogThresold*2))
    SendLog(MulSensor,N,yn_0,Pin[MulSensor].useCurve(),Pin[MulSensor].MaxReading,Pin[MulSensor].State);
  #endif  
}



