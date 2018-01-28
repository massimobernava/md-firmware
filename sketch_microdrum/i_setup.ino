
//==============================
//     SETUP
//==============================
void setup()
{
  #if TEENSY
  HWSERIAL.begin(31250);
  #endif
  
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
//    MULTIPLEX RETIRADO SE NO_MUX
//------------------------------------ 

 #if USE_DIG
 //Initializing digital pins. Pullup not necessary, but is simpler. Not sure if long cables effect it or not.
 pinMode(Choke1_Pin, INPUT_PULLUP);
 pinMode(Aux1_Pin, INPUT_PULLUP);
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
  
  pinMode(LEDpin,OUTPUT);
  
  #if USE_LCD
  #if NO_MUX
    pinMode(3, INPUT_PULLUP);   //For buttons. No VCC needed, only signal and GND
    pinMode(2, INPUT_PULLUP);   //For buttons. No VCC needed, only signal and GND
  #elif ENCODER
    pinMode (encoder0PinA,INPUT);
    pinMode (encoder0PinB,INPUT);
    pinMode (encoder0Btn1,INPUT);
  #else
    pinMode(6, INPUT_PULLUP);   //For buttons. No VCC needed, only signal and GND
    pinMode(5, INPUT_PULLUP);   //For buttons. No VCC needed, only signal and GND
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
  #endif

  #if USE_LCD
    #if USE_I2C_LCD
      lcd.init(); // initialize the lcd
      lcd.backlight();
      lcd.noAutoscroll();
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
    #else
      MenuString(PSTR("microDRUM v1.4"),false);
    #endif
  #endif

  #if NO_MUX
  // Do a backflip or something
  #else
    pinMode(2, OUTPUT);    // s0
    pinMode(3, OUTPUT);    // s1
    pinMode(4, OUTPUT);    // s2

      #if DEBUG
      Serial.print("Multiplexer on.");
      #endif
      
    fastWrite(3,0);fastWrite(4,0);
  #endif

  blink();  blink();  blink();
}
