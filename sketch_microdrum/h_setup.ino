//==============================
//     SETUP
//==============================
void setup()
{
  pinMode(13, OUTPUT);   //LED
  
  pinMode(2, OUTPUT);    // s0
  pinMode(3, OUTPUT);    // s1
  pinMode(4, OUTPUT);    // s2

  GlobalTime=TIMEFUNCTION;
  for (int count=0; count<NPin; count++)
  {
    Pin[count].set(count);
    //Pin[count].Time=Time+Pin[count].MaskTime;
    //MaxReadingSensor[count]=-1;
    //MaxRetriggerSensor[count]=0;//0xFF;
  }
  
  #if USE_LCD
    pinMode(7, INPUT);
    pinMode(6, INPUT);
  #endif
  
  #if SERIALSPEED
    Serial.begin(115200);    //Serial
  #else
    Serial.begin(31250);      // MIDI
  #endif
  
  Serial.flush();
  
  #if USE_WAVTRIGGER
    mySerial.begin(57600);//31250);
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
    REG_ADC_MR = (REG_ADC_MR & 0xFFF0FFFF) | 0x00030000; //0x00020000;
  #endif
  
  #if USE_LCD
    #if USE_595
      lcd.setLED2Pin(HIGH);
    #endif
    // set up the LCD's number of columns and rows: 
    lcd.begin(16, 2);
    // Print a message to the LCD.
    #if NANO
      MenuString(PSTR("nanoDRUM v1.4"),false);
    #else
      MenuString(PSTR("microDRUM v1.4"),false);
    #endif
  #endif
  
  fastWrite(3,0);fastWrite(4,0);
  
}
