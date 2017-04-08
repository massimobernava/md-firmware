
//==============================
//     SETUP
//==============================
void setup()
{
  HWSERIAL.begin(31250);

  #if SERIALSPEED
    Serial.begin(115200);    //Serial
  #else
    Serial.begin(31250);      // MIDI
  #endif
  Serial.flush();

  #if SAM2695
  synth.midiReset();
  synth.setReverb(fluxCHANNEL,5,255,100); // A Plate Reverb with maximum effect level
  synth.setChannelVolume(fluxCHANNEL, 127); // max. channel volume
  synth.setMasterVolume(255);  // max. master volume
  #endif

//------------------------------------
//    MULTIPLEX RETIRADO SE MEGA
//------------------------------------ 

#if MEGA
#if USE_DIG
//Ativa os pinos digitais
 pinMode(Choke1_Pin, INPUT_PULLUP);
 pinMode(Choke2_Pin, INPUT_PULLUP);
 pinMode(Choke3_Pin, INPUT_PULLUP);
 pinMode(ChokeRide_Pin, INPUT_PULLUP);
 pinMode(Aux1_Pin, INPUT_PULLUP);
 pinMode(Aux2_Pin, INPUT_PULLUP);
 pinMode(Aux3_Pin, INPUT_PULLUP);
 pinMode(Aux4_Pin, INPUT_PULLUP);
 pinMode(Aux5_Pin, INPUT_PULLUP);
 pinMode(Aux6_Pin, INPUT_PULLUP);
 pinMode(Aux7_Pin, INPUT_PULLUP);
 #endif
#else
  pinMode(2, OUTPUT);    // s0
  pinMode(3, OUTPUT);    // s1
  pinMode(4, OUTPUT);    // s2
  #if DEBUG
Serial.print("Multiplexer on.");
  #endif
#endif
//------------------------------------

  GlobalTime=TIMEFUNCTION;
  for (int count=0; count<NPin; count++)
  {
    Pin[count].set(count);
    //Pin[count].Time=Time+Pin[count].MaskTime;
    //MaxReadingSensor[count]=-1;
    //MaxRetriggerSensor[count]=0;//0xFF;
  }
  
  #if TEENSY
  pinMode(LEDpin,OUTPUT);
  #endif
  
  #if USE_LCD
  #if MEGA
    pinMode(3, INPUT_PULLUP);   //No VCC needed, only signal and GND
    pinMode(2, INPUT_PULLUP);   //No VCC needed, only signal and GND
  #elif ENCODER
    pinMode (encoder0PinA,INPUT);
    pinMode (encoder0PinB,INPUT);
    pinMode (encoder0Btn1,INPUT_PULLUP);
  #else
    pinMode(6, INPUT_PULLUP);   //No VCC needed, only signal and GND
    pinMode(5, INPUT_PULLUP);   //No VCC needed, only signal and GND
  #endif
  #endif

  #if USE_WAVTRIGGER
    mySerial.begin(57600);//31250);
  #endif

  #if USE_PISERIAL
    mySerial.begin(57600);
  #endif
   
  #if defined(__AVR__) 
    analogReference(DEFAULT);
  #endif

  LoadAllEEPROM();

  #if defined(__AVR__)

  #if FASTADC
    setPrescaler(Prescaler_16);
  #endif

  #if VERYFASTADC
    setPrescaler(Prescaler_8);
  #endif

  #if VERYVERYFASTADC
    setPrescaler(Prescaler_4);
  #endif

  #elif defined(__arm__) 
//  REG_ADC_MR = (REG_ADC_MR & 0xFFF0FFFF) | 0x00030000; //0x00020000;
    //analogReference(DEFAULT);   might be changed in the future
  #endif

  #if USE_LCD
    #if USE_I2C_LCD
      lcd.init(); // initialize the lcd
      lcd.backlight();
      lcd.noBacklight();
    #elif USE_595
      lcd.setLED2Pin(HIGH);
      lcd.begin(16, 2);
      lcd.setCursor(0,0);
     #else
      lcd.begin(16, 2);
      lcd.setCursor(0,0);
      #endif

    #if NANO
      MenuString(PSTR("nanoDRUM v1.4"),false);
    #elif MEGA
      {
        //lcd.clear();
        //lcd.setCursor(0,1);
        //lcd.print("microDrum");
        MenuString(PSTR("TeensyMegaDRUM  v1.4"),false);
        }
    #else
      MenuString(PSTR("microDRUM v1.4"),false);
    #endif
  #endif

  #if MEGA
  //
  #else
    fastWrite(3,0);fastWrite(4,0);
  #endif

  #if USE_LCD
  #if MEGA
    /*lcd.createChar(0, lcdSnare);
    lcd.createChar(1, lcdTom);
    lcd.createChar(2, lcdCymball);
    lcd.createChar(3, lcdHH);
    lcd.createChar(4, lcdKick);*/
    #endif
  #endif

  blink();  blink();  blink();

}
