
//==============================
//    MENU
//==============================
#if USE_LCD

#if USE_595
  int bttnAPin = 5;
  int bttnBPin = 6;
  
#elif ENCODER
  byte val; 
  byte encoder0PinA = 5;
  byte encoder0PinB = 6;
  byte encoder0Btn1 = 7; 
  byte encoder0Pos = 0;
  byte encoder0PinALast = LOW;
  byte n = LOW;
  #define RESETDELAY 3000
  
#else
  int bttnAPin = 11;
  int bttnBPin = 12;
#endif

#define HOLDDELAY 500
#define DEBOUNCEDELAY 50
byte eMenuSelect=0;
byte eMenuPage=0;
byte eMenuGeneral=0;
byte eMenuPin=0;
//byte eMenuLog=0;
byte btnB_State=0;
byte btnA_State=0;
unsigned long btnA_Time=0;
unsigned long btnB_Time=0;
//byte btnA_Click=0;
//bool Changed=true;
  
#if USE_I2C_LCD
  LiquidCrystal_I2C lcd(0x3F,20,4);
#elif USE_595
LiquidCrystal595 lcd(10, 12, 13);
#else
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);
#endif

#endif
//==============================

#if MENU_LOG
#define S_HITSOFT PSTR("HIT SOFT")
#define S_HITHARD PSTR("HIT HARD")
#define S_WAIT PSTR("WAIT...")
#define S_END PSTR("END")
#define S_NOISE PSTR("NOISE..")
#endif

#if USE_LCD
#define ADD(x) Pin[eMenuPage-2].x=(Pin[eMenuPage-2].x+1)%128;
#define TADD(x,t) Pin[eMenuPage-2].x=(t)((Pin[eMenuPage-2].x+1)%128);
#define SUB(x) Pin[eMenuPage-2].x=Pin[eMenuPage-2].x-1>-1?Pin[eMenuPage-2].x-1:127;
#define TSUB(x,t) Pin[eMenuPage-2].x=(t)(Pin[eMenuPage-2].x-1>-1?Pin[eMenuPage-2].x-1:127);
#define SAVE(x) SaveEEPROM(eMenuPage-2,x);

#define S_MODE PSTR("MODE")
#define S_GENERAL PSTR("General")
#define S_PIN PSTR("PIN")
#define S_LOGDIS PSTR("TOOL MODE DISABLED")

//#define S_NULL PSTR("---")
#define S_OFF PSTR("OFF")
#define S_STANDBY PSTR("Standby")
#define S_MIDI PSTR("MIDI")
#define S_TOOL PSTR("Tool")
#define S_DELAY PSTR("Delay")
#define S_XTALK PSTR("XTalk")

#define S_HHCT100 PSTR("HHC T100")
#define S_HHCT75 PSTR("HHC T75")
#define S_HHCT50 PSTR("HHC T50")
#define S_HHCT25 PSTR("HHC T25")

#define S_NSENSOR PSTR("Sensor N")

#define S_KIT PSTR("Kit")
#define S_BOXER PSTR("Boxer")
#define S_FAT_STACKS PSTR("FatStacks")
#define S_BOMBASTIX PSTR("Bombastix")
#define S_SALAMANDER PSTR("Salamander")

#define S_LIN PSTR("LIN")
#define S_EXP PSTR("EXP")
#define S_LOG PSTR("LOG")
#define S_SGM PSTR("SGM")
#define S_FLT PSTR("FLT")

#define S_NOTE PSTR("Note")
#define S_THRESOLD PSTR("Thresold")
#define S_SCANTIME PSTR("ScanTime")
#define S_MASKTIME PSTR("MaskTime")
#define S_RETRIG PSTR("Retrig")
#define S_CURVE PSTR("Curve")
#define S_CURVEF PSTR("CurveF")
#define S_XTALKG PSTR("XTalkG")
#define S_TYPE PSTR("Type")
#define S_CKNOTE PSTR("CkNote")
#define S_GAIN PSTR("Gain")
#define S_DUAL PSTR("Dual")
#define S_CHANNEL PSTR("Channel")

#define S_PIEZO PSTR("Piezo")
#define S_SWITCH PSTR("Switch")
#define S_HHC PSTR("HHC")
#define S_HH PSTR("HH")
#define S_HHS PSTR("HHs")
#define S_YSWITCH PSTR("YSwitch")
#define S_DISABLED PSTR("Disabled")

//DEFAULT NAME STRING
#define S_SNAREHEAD PSTR("Snare Head")
#define S_SNARERIM PSTR("Snare Rim")
#define S_EFFECT PSTR("Effect")
#define S_KICK PSTR("Kick")
#define S_HHC PSTR("HHC")
#define S_HHBOW PSTR("HH Bow")
#define S_HHEDGE PSTR("HH Edge")
/*added cause wasn't included*/ #define S_HHC_RING PSTR("HHC Ring")
#define S_TOM1HEAD PSTR("Tom1 Head")
#define S_TOM1RIM PSTR("Tom1 Rim")
#define S_TOM2HEAD PSTR("Tom2 Head")
#define S_TOM2RIM PSTR("Tom2 Rim")
#define S_CRASHBOW PSTR("Crash Bow")
#define S_CRASHEDGE PSTR("Crash Edge")
/*added cause wasn't included*/ #define S_CRASH2BOW PSTR("Crash 2 Bow")
/*added cause wasn't included*/ #define S_CRASH2EDGE PSTR("Crash 2 EDdge")
#define S_RIDEBOW PSTR("Ride Bow")
#define S_RIDEEDGE PSTR("Ride Edge")
#define S_TOM3HEAD PSTR("Tom3 Head")
#define S_TOM3RIM PSTR("Tom3 Rim")
#define S_TOM4HEAD PSTR("Tom4 Head")
#define S_TOM4RIM PSTR("Tom4 Rim")
#define S_EXTRA1 PSTR("Extra 1")
#define S_EXTRA2 PSTR("Extra 2")
#define S_CHINA PSTR("China")
#define S_SPLASH PSTR("Splash")

//==============================
//    MENU     
//==============================
void Menu()
{
  unsigned long now = millis();
  
  #if ENCODER
 /* #if TEENSY
  Bounce btnA = Bounce(encoder0Btn1, 50);
  btnA.update();
  
  if(btnA.fallingEdge() && (now> btnA_Time + RESETDELAY)) {
    softReset();
    }
    
   else if (btnA.fallingEdge()) {
      btnA_Time=now;
      eMenuSelect=(eMenuSelect+1)%3; 
      Draw();
  }

  n = digitalRead(encoder0PinA);
   if ((encoder0PinALast == LOW) && (n == HIGH)) {
     if (digitalRead(encoder0PinB) == LOW) {
   
        Down(); Draw();}
     
     else {
        Up(); Draw();}
   }
   encoder0PinALast = n;
   
  #else
  */
  byte btnA= digitalRead(encoder0Btn1);
  
  if(btnA_State==0) {
    if(btnA==LOW)
    {
      btnA_State=1;
      btnA_Time=now;
    }
  }
  else  if(btnA_State==1) {
    if(btnA==HIGH)
    {
      btnA_State=2;
    }
    else if((btnA==LOW) && (now> btnA_Time + RESETDELAY))  //do a soft reset
    {
      softReset();
      btnA_State=3;
    }
  }
  else if(btnA_State==2) {
    if(now>btnA_Time + DEBOUNCEDELAY)   //change set of setting
    {
      eMenuSelect=(eMenuSelect+1)%3; 
      Draw();
    }
    btnA_State=0;
  }
  else if(btnA_State==3) {
    if(btnA==HIGH)
    {
      btnA_State=0;
    }
  }

  n = digitalRead(encoder0PinA);
   if ((encoder0PinALast == LOW) && (n == HIGH)) {
     if (digitalRead(encoder0PinB) == LOW) {
   
       /*encoder0Pos--;*/ Down(); Draw();}
     
     else {
       /*encoder0Pos++;*/ Up(); Draw();}
   }
   encoder0PinALast = n;
//#endif
#else
  
  byte btnB = digitalRead(bttnBPin);
  byte btnA = digitalRead(bttnAPin);
  
  if(btnB==LOW && btnA==LOW) softReset();
  
  if(btnA_State==0) {
    if(btnA==LOW)
    {
      btnA_State=1;
      btnA_Time=now;
    }
  }
  else  if(btnA_State==1) {
    if(btnA==HIGH)
    {
      btnA_State=2;
    }
    else if((btnA==LOW) && (now> btnA_Time + HOLDDELAY))
    {
      eMenuSelect=(eMenuSelect+1)%3; 
      Draw();
      btnA_State=3;
    }
  }
  else if(btnA_State==2) {
    if(now>btnA_Time + DEBOUNCEDELAY)
    {
      Up();
      Draw();
    }
    btnA_State=0;
  }
  else if(btnA_State==3) {
    if(btnA==HIGH)
    {
      btnA_State=0;
    }
  }
  
  
  if(btnB_State==0) {
    if(btnB==LOW)
    {
      btnB_State=1;
      btnB_Time=now;
    }
  }
  else  if(btnB_State==1) {
    if(btnB==HIGH)
    {
      btnB_State=2;
    }
    else if((btnB==LOW) && (now> btnB_Time + HOLDDELAY))
    {
      eMenuSelect=(eMenuSelect-1)>-1?eMenuSelect-1:2; 
      Draw();
      btnB_State=3;
    }
  }
  else if(btnB_State==2) {
    if(now>btnB_Time + DEBOUNCEDELAY)
    {
      Down();
      Draw();
    }
    btnB_State=0;
  }
  else if(btnB_State==3) {
    if(btnB==HIGH)
    {
      btnB_State=0;
    }
  }
  #endif
}   

//==============================
//    UP
//==============================

void Up()
{
  if(eMenuSelect==0) 
  { 
    eMenuPage=(eMenuPage+1)%51; 
    LogPin=eMenuPage-2; 
    #if MENU_LOG
    log_state=0;
    #endif
  }
  else if(eMenuSelect==1)
  {
    if(eMenuPage==0) Mode=(mode)(((int)Mode+1)%4); //MODE
    else if(eMenuPage==1) eMenuGeneral=(eMenuGeneral+1)%8;//General
    else if(eMenuPage>=2 && eMenuPage<50) eMenuPin=(eMenuPin+1)%12;//Pin
    else if(eMenuPage==50) eMenuPage=(eMenuPage+1)%51;//eMenuLog=(eMenuLog+1)%2;//LOG
  }
  else if(eMenuSelect==2)
  {
    if(eMenuPage==1)//General
    {
          /*if(eMenuGeneral==0) delayTime=(delayTime+1)%999;//Delay
          else*/ if(eMenuGeneral==1) GeneralXtalk=(GeneralXtalk+1)%8;//XTalk
          #if USE_WAVTRIGGER
          else if(eMenuGeneral==2) kit=(kit+1)%4;
          #endif
          else if(eMenuGeneral==3) { HHThresoldSensor[0]=(HHThresoldSensor[0]+1)%128; SaveHHEEPROM(0x04,HHThresoldSensor[0]); }
          else if(eMenuGeneral==4) { HHThresoldSensor[1]=(HHThresoldSensor[1]+1)%128; SaveHHEEPROM(0x05,HHThresoldSensor[1]); }
          else if(eMenuGeneral==5) { HHThresoldSensor[2]=(HHThresoldSensor[2]+1)%128; SaveHHEEPROM(0x06,HHThresoldSensor[2]); }
          else if(eMenuGeneral==6) { HHThresoldSensor[3]=(HHThresoldSensor[3]+1)%128; SaveHHEEPROM(0x07,HHThresoldSensor[3]); }
          else if(eMenuGeneral==7) { NSensor=(NSensor+1)%6; SaveGeneralEEPROM(0x02); }
          //else if(eMenuGeneral==8) { Soundbank = (Soundbank + 1)%10;}
        }
        else if(eMenuPage>=2  && eMenuPage<50)//Pin
        {
          switch(eMenuPin)
          {
            case 0: ADD(Note); SAVE(NOTE); break;//Note
            case 1: ADD(Thresold); SAVE(THRESOLD); break;//Thresold
            case 2: ADD(ScanTime); SAVE(SCANTIME); break;//ScanTime
            case 3: ADD(MaskTime); SAVE(MASKTIME); break;//MaskTime
            case 4: ADD(Retrigger); SAVE(RETRIGGER); break;//Retrigger
            case 5: TADD(Curve,curve); SAVE(CURVE); break;//Curve
            case 6: ADD(CurveForm); SAVE(CURVEFORM); break;//CurveForm
            case 7: ADD(Xtalk); SAVE(XTALK); break;//XTalk
            case 8: ADD(XtalkGroup); SAVE(XTALKGROUP); break;//XTalkGroup
            case 9: Pin[eMenuPage-2].Type=(type)((Pin[eMenuPage-2].Type+1)%128); SAVE(TYPE); break;//Type
            case 10: ADD(ChokeNote); SAVE(CHOKENOTE); break;//ChokeNote
            #if ENABLE_CHANNEL
            case 11:  ADD(Channel); SAVE(CHANNEL); break;//Channel
            #endif
          }
        }
        else if(eMenuPage==50) //LOG
        {
          /*switch(eMenuLog)
          {
            case 0: LogPin=(LogPin+1)%49; break;
            case 1: LogThresold=(LogThresold+1)%256; break;
          }*/
          eMenuPage=(eMenuPage+1)%51;
        }
      }
 }
 
 //==============================
//    DRAW
//==============================
void Draw()
{
  lcd.clear();
  //lcd.noAutoscroll();
  
  Diagnostic=false;
  
      //MENU=1
      lcd.setCursor(0,0);
      if(eMenuPage==0) MenuString(S_MODE,eMenuSelect==0);
      else if(eMenuPage==1) MenuString(S_GENERAL,eMenuSelect==0);
      else if(eMenuPage>=2 && eMenuPage<50)
      {
        /*char cPin[]="Pin 000";
        cPin[6]+=(eMenuPage-2)%10;
        cPin[5]+=(eMenuPage-2)/10;*/
        #if USE_DEFAULT_NAME
        DefaultPrintName((eMenuPage-2),false);
        #else
        MenuString(S_PIN,false);
        #endif
        MenuInt((eMenuPage-2),eMenuSelect==0);

      }
      else if (eMenuPage==50) {if(Mode==Tool) Diagnostic=true; else MenuString(S_LOGDIS,false);}//MenuString(S_LOG,eMenuSelect==0);
      //else if (eMenuPage==51) { MenuString(S_NULL,eMenuSelect==0);  lcd.setCursor(16,1);  lcd.autoscroll(); }

      
      //MENU=2
      lcd.setCursor(0,1);
      if(eMenuPage==0) 
      {
        switch(Mode)
        {
          case Off: MenuString(S_OFF,eMenuSelect==1); break;
          case Standby: MenuString(S_STANDBY,eMenuSelect==1); break;
          case MIDI: MenuString(S_MIDI,eMenuSelect==1); break;
          case Tool: MenuString(S_TOOL,eMenuSelect==1); break;
        }
      }
      else if(eMenuPage==1)
      {
        switch(eMenuGeneral)
        {
          case 0: MenuString(S_DELAY,eMenuSelect==1); break;
          case 1: MenuString(S_XTALK,eMenuSelect==1); break;
          case 2: MenuString(S_KIT,eMenuSelect==1); break;
          case 3: MenuString(S_HHCT100,eMenuSelect==1); break;
          case 4: MenuString(S_HHCT75,eMenuSelect==1); break;
          case 5: MenuString(S_HHCT50,eMenuSelect==1); break;
          case 6: MenuString(S_HHCT25,eMenuSelect==1); break;
          case 7: MenuString(S_NSENSOR,eMenuSelect==1);break;
        }
      }
      else if(eMenuPage>=2 && eMenuPage<50)
      {
        if(Mode==Tool) 
          return;
              
        switch(eMenuPin)
        {
          case 0: MenuString(S_NOTE,eMenuSelect==1); break;
          case 1: MenuString(S_THRESOLD,eMenuSelect==1); break;
          case 2: MenuString(S_SCANTIME,eMenuSelect==1); break;
          case 3: MenuString(S_MASKTIME,eMenuSelect==1); break;
          case 4: MenuString(S_RETRIG,eMenuSelect==1); break;
          case 5: MenuString(S_CURVE,eMenuSelect==1); break;
          case 6: MenuString(S_CURVEF,eMenuSelect==1); break;
          case 7: MenuString(S_XTALK,eMenuSelect==1); break;
          case 8: MenuString(S_XTALKG,eMenuSelect==1); break;
          case 9: MenuString(S_TYPE,eMenuSelect==1); break;
          case 10:
          if(Pin[eMenuPage-2].Type==Piezo)
            MenuString(S_GAIN,eMenuSelect==1);
          else
            MenuString(S_CKNOTE,eMenuSelect==1); 
 
          break;
          //case 11: MenuString(S_DUAL,eMenuSelect==1); break;
          #if ENABLE_CHANNEL
          case 11: MenuString(S_CHANNEL,eMenuSelect==1); break;
          #endif
        }
      }
      /*else if(eMenuPage==50)
     {
       switch(eMenuLog)
       {
         case 0: MenuString(S_PIN,eMenuSelect==1); break;
         case 1: MenuString(S_THRESOLD,eMenuSelect==1); break;
       }
     }*/
    
      //MENU=3
      lcd.setCursor(11,1);
      if(eMenuPage==1)
      {
        if(eMenuGeneral==0) MenuInt(delayTime,eMenuSelect==2);
        else if(eMenuGeneral==1) MenuInt(GeneralXtalk,eMenuSelect==2);
        #if USE_WAVTRIGGER
        else if(eMenuGeneral==2) 
        {
          lcd.setCursor(5,1);
          if(kit==0) MenuString(S_FAT_STACKS,eMenuSelect==2);
          else if(kit==1) MenuString(S_BOXER,eMenuSelect==2);
          else if(kit==2) MenuString(S_BOMBASTIX,eMenuSelect==2);
          else if(kit==3) MenuString(S_SALAMANDER,eMenuSelect==2);
        }
        #endif
        else if(eMenuGeneral==3) MenuInt(HHThresoldSensor[0],eMenuSelect==2);
        else if(eMenuGeneral==4) MenuInt(HHThresoldSensor[1],eMenuSelect==2);
        else if(eMenuGeneral==5) MenuInt(HHThresoldSensor[2],eMenuSelect==2);
        else if(eMenuGeneral==6) MenuInt(HHThresoldSensor[3],eMenuSelect==2);
        else if(eMenuGeneral==7) MenuInt(NSensor,eMenuSelect==2);
      }
      else if(eMenuPage>=2  && eMenuPage<50)
      {
        if(eMenuPin==0) MenuInt(Pin[eMenuPage-2].Note,eMenuSelect==2);
        else if(eMenuPin==1) MenuInt(Pin[eMenuPage-2].Thresold,eMenuSelect==2);
        else if(eMenuPin==2) MenuInt(Pin[eMenuPage-2].ScanTime,eMenuSelect==2);
        else if(eMenuPin==3) MenuInt(Pin[eMenuPage-2].MaskTime,eMenuSelect==2);
        else if(eMenuPin==4) MenuInt(Pin[eMenuPage-2].Retrigger,eMenuSelect==2);
        else if(eMenuPin==5)
       {
         switch(Pin[eMenuPage-2].Curve)
         {
           case 0: MenuString(S_LIN,eMenuSelect==2); break;
           case 1: MenuString(S_EXP,eMenuSelect==2); break;
           case 2: MenuString(S_LOG,eMenuSelect==2); break;
           case 3: MenuString(S_SGM,eMenuSelect==2); break;
           case 4: MenuString(S_FLT,eMenuSelect==2); break;
           default: MenuInt(Pin[eMenuPage-2].Curve,eMenuSelect==2);
         }
       }
        else if(eMenuPin==6) MenuInt(Pin[eMenuPage-2].CurveForm,eMenuSelect==2);
        else if(eMenuPin==7) MenuInt(Pin[eMenuPage-2].Xtalk,eMenuSelect==2);
        else if(eMenuPin==8) MenuInt(Pin[eMenuPage-2].XtalkGroup,eMenuSelect==2);
        else if(eMenuPin==9)
       {
        lcd.setCursor(6,1);
         switch(Pin[eMenuPage-2].Type)
         {
           case Piezo: MenuString(S_PIEZO,eMenuSelect==2); break;
           case Switch: MenuString(S_SWITCH,eMenuSelect==2); break;
           case HHC: MenuString(S_HHC,eMenuSelect==2); break;
           case HH: MenuString(S_HH,eMenuSelect==2); break;
           case HHs: MenuString(S_HHS,eMenuSelect==2); break;
           case YSwitch: MenuString(S_YSWITCH,eMenuSelect==2); break;
           case Disabled: MenuString(S_DISABLED,eMenuSelect==2); break;
         }
       }
        else if(eMenuPin==10) MenuInt(Pin[eMenuPage-2].ChokeNote,eMenuSelect==2);
        else if(eMenuPin==11) MenuInt(DualSensor(eMenuPage-2),eMenuSelect==2);
        #if ENABLE_CHANNEL
        else if(eMenuPin==12) MenuInt(Pin[eMenuPage-2].Channel,eMenuSelect==2);
        #endif
      }
      /*else if(eMenuPage==50)
      {
       switch(eMenuLog)
       {
         case 0: MenuString(LogPin,eMenuSelect==2);break;
         case 1: MenuString(LogThresold,eMenuSelect==2);break;
       }
      }*/
        
      //Changed=false;
 }
 
//==============================
//    DOWN
//==============================
 void Down()
 {
   
  if(eMenuSelect==0) { 
    eMenuPage=(eMenuPage-1)>-1?eMenuPage-1:50; 
    LogPin=eMenuPage-2; 
    #if MENU_LOG
    log_state=0;
    #endif
  }
  else if(eMenuSelect==1)
      {
        if(eMenuPage==0) Mode=(mode)((int)Mode-1>-1?(int)Mode-1:3); //MODE
        else if(eMenuPage==1) eMenuGeneral=eMenuGeneral-1>-1?eMenuGeneral-1:7; //General
        else if(eMenuPage>=2 && eMenuPage<50) eMenuPin=eMenuPin-1>-1?eMenuPin-1:11; //Pin
        else if(eMenuPage==50) eMenuPage=(eMenuPage-1)>-1?eMenuPage-1:50;//eMenuLog=eMenuLog-1>-1?eMenuLog-1:1;//Log
      }
      else if(eMenuSelect==2)
      {
        if(eMenuPage==1)//General
        {
          /*if(eMenuGeneral==0) delayTime=delayTime-1>-1?delayTime-1:999;//Delay
          else*/ if(eMenuGeneral==1) GeneralXtalk=(GeneralXtalk-1>-1)?GeneralXtalk-1:7;//General XTalk
          #if USE_WAVTRIGGER
          else if(eMenuGeneral==2) kit=(kit-1>-1)?kit-1:3;
          #endif
          else if(eMenuGeneral==3) { HHThresoldSensor[0]=((HHThresoldSensor[0]-1)>-1)?HHThresoldSensor[0]-1:127; SaveHHEEPROM(0x04,HHThresoldSensor[0]); }
          else if(eMenuGeneral==4) { HHThresoldSensor[1]=((HHThresoldSensor[1]-1)>-1)?HHThresoldSensor[1]-1:127; SaveHHEEPROM(0x05,HHThresoldSensor[1]); }
          else if(eMenuGeneral==5) { HHThresoldSensor[2]=((HHThresoldSensor[2]-1)>-1)?HHThresoldSensor[2]-1:127; SaveHHEEPROM(0x06,HHThresoldSensor[2]); }
          else if(eMenuGeneral==6) { HHThresoldSensor[3]=((HHThresoldSensor[3]-1)>-1)?HHThresoldSensor[3]-1:127; SaveHHEEPROM(0x07,HHThresoldSensor[3]); }
          else if(eMenuGeneral==7) { NSensor=((NSensor-1)>-1)?(NSensor-1):6; SaveGeneralEEPROM(0x02); }
          //else if(eMenuGeneral==8) { Soundbank = (Soundbank -1)%10;}
        }
        else if(eMenuPage>=2  && eMenuPage<50)//Pin
        {
          switch(eMenuPin)
          {
            case 0: SUB(Note); SAVE(NOTE); break;//Note
            case 1: SUB(Thresold); SAVE(THRESOLD); break;//Thresold
            case 2: SUB(ScanTime); SAVE(SCANTIME); break;//ScanTime
            case 3: SUB(MaskTime); SAVE(MASKTIME); break;//MaskTime
            case 4: SUB(Retrigger); SAVE(RETRIGGER); break;//Retrigger
            case 5: TSUB(Curve,curve); SAVE(CURVE); break;//Curve
            case 6: SUB(CurveForm); SAVE(CURVEFORM); break;//CurveForm
            case 7: SUB(Xtalk); SAVE(XTALK); break;//XTalk
            case 8: SUB(XtalkGroup); SAVE(XTALKGROUP); break;//XTalkGroup
            case 9: TSUB(Type,type); SAVE(TYPE); break;//Type
            case 10: SUB(ChokeNote); SAVE(CHOKENOTE); break;//ChokeNote
            #if ENABLE_CHANNEL
            case 11: SUB(Channel); SAVE(CHANNEL); break;//Channel
            #endif
          }
        }
        else if(eMenuPage==50)
        {
          /*switch(eMenuLog)
          {
            case 0: LogPin=LogPin-1>-1?LogPin-1:48; break;
            case 1: LogThresold=LogThresold-1>-1?LogThresold-1:255; break;
          }*/
          eMenuPage=(eMenuPage-1)>-1?eMenuPage-1:50;
        }
      }  
 }

#if MENU_LOG
void DrawLog(byte x)
{
   lcd.setCursor(0,1);
   if(x==0) { MenuString(S_WAIT,false); MenuInt(log_Vmax>>3,'(',')'); }
   else if(x==1) { MenuString(S_HITSOFT,false);MenuInt(d_tnum,'(',')');}
   else if(x==2) { MenuString(S_HITHARD,false);MenuInt(d_tnum,'(',')');}
   else if(x==3) MenuString(S_END,false);
   else if(x==4) { MenuString(S_NOISE,false); MenuInt(log_Nmax,'(',')'); }
   //MenuInt(d_tnum,'(',')');
   //MenuString(log_Vmax,'(',')');
  /*if(log_state==2) MenuString(log_Nmax,'n',' ');
  else
  { 
    if(x==0)
    {
      MenuString(log_Nmax,'T',' '); //-> Thresold
      MenuString((byte)(d_tsum/d_tnum),'S',' '); //-> ScanTime
      MenuString((byte)(d_hsum/d_tnum),'M',' ');//-> MaskTime

    }  
    else if(x==1)
    {
      MenuString(d_rmax,'R',' '); //-> Retrigger
      MenuString((40/d_vmin)*64,'G',' '); //-> Gain
      MenuString((1024/d_vmax)*64,'F',' '); //-> FormFactor
    }
    else if(x==2)
    {
      MenuString(d_vmin,'v',' ');
      MenuString(log_Tmax-log_T1,'t',' ');  
      MenuString(log_T50==0?0:log_T50-log_Tmax,'h',' ');
    }*/
    /*else if(x==2)
    {
      MenuString(log_T70==0?0:log_T70-log_Tmax,'b',' ');
      MenuString(log_T60==0?0:log_T60-log_Tmax,'c',' ');
      MenuString(log_T50==0?0:log_T50-log_Tmax,'d',' ');
    }
    else if(x==3)
    {
      MenuString(log_T40==0?0:log_T40-log_Tmax,'e',' ');
      MenuString(log_T30==0?0:log_T30-log_Tmax,'f',' ');
      MenuString(log_T20==0?0:log_T20-log_Tmax,'g',' ');
    }
  }*/
}

#endif

//========================================
//   PRINTNAME
//========================================
#if USE_DEFAULT_NAME
void DefaultPrintName(byte pin,bool sel)
{
  switch(pin)
  {
    case DP_SNAREHEAD: MenuString(S_SNAREHEAD,sel); break;
    case DP_SNARERIM: MenuString(S_SNARERIM,sel); break;
    case DP_KICK: MenuString(S_KICK,sel); break;
    case DP_HHC: MenuString(S_HHC,sel); break;
    case DP_HHBOW: MenuString(S_HHBOW,sel); break;

    //Added these cause default names was not complete
    case DP_HHC_RING: MenuString(S_HHC_RING,sel); break;
    case DP_CHINA: MenuString(S_CHINA,sel); break;
    case DP_SPLASH: MenuString(S_SPLASH,sel); break;
    case DP_CRASH2BOW: MenuString(S_CRASH2BOW,sel); break;
    case DP_CRASH2EDGE: MenuString(S_CRASH2EDGE,sel); break;
    
    #if NO_MUX
    case DP_EXTRA1: MenuString(S_EXTRA1,sel); break;
    case DP_EXTRA2: MenuString(S_EXTRA2,sel); break;
    #else
    case DP_HHEDGE: MenuString(S_HHEDGE,sel); break;
    case DP_EFFECT: MenuString(S_EFFECT,sel); break;
    case DP_CRASHBOW: MenuString(S_CRASHBOW,sel); break;
    #endif
    case DP_CRASHEDGE: MenuString(S_CRASHEDGE,sel); break;
    case DP_RIDEBOW: MenuString(S_RIDEBOW,sel); break;
    case DP_RIDEEDGE: MenuString(S_RIDEEDGE,sel); break;
    case DP_TOM1HEAD: MenuString(S_TOM1HEAD,sel); break;
    case DP_TOM2HEAD: MenuString(S_TOM2HEAD,sel); break;
    case DP_TOM3HEAD: MenuString(S_TOM3HEAD,sel); break;
    case DP_TOM4HEAD: MenuString(S_TOM4HEAD,sel); break;

    #if NO_MUX
    case DP_CHINA: MenuString(S_CHINA,sel); break;
    case DP_SPLASH: MenuString(S_SPLASH,sel); break;
    #else
    case DP_TOM1RIM: MenuString(S_TOM1RIM,sel); break; 
    case DP_TOM2RIM: MenuString(S_TOM2RIM,sel); break;
    case DP_TOM3RIM: MenuString(S_TOM3RIM,sel); break; 
    case DP_TOM4RIM: MenuString(S_TOM4RIM,sel); break;
    #endif
    
    default: MenuString(S_PIN,sel); break;
  }
}
#endif

void DrawDiagnostic(byte i,byte val)
{
  if(i>31) return;
  
  lcd.setCursor(i%16,i/16);
  lcd.print(val/16);
  
}
void MenuInt(int inter,bool sel) //sz si puo togliere
{
  if(sel) MenuInt(inter,'<','>');
  else MenuInt(inter,' ',' ');
}

void MenuInt(int inter,char A,char B) //sz si puo togliere
{
  if(inter>999) B='e';
  
  char cInter[]=" 000 ";
  cInter[0]=A; cInter[4]=B;
  cInter[3]+=inter%10;
  cInter[2]+=(inter%100)/10;
  cInter[1]+=(inter%1000)/100;
  //lcd.print(cInter);
  byte i=0;
  while(i<5)
  {
    lcd.print(cInter[i]);
    ++i;
  }
}
void MenuString(const PROGMEM char *s,bool sel)
{
  char c;
  if(sel) lcd.print('<');
  while ((c = pgm_read_byte_near(s++)) != 0)
     lcd.print(c);
  if(sel) lcd.print('>');
}
#endif
