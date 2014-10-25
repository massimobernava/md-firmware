#if MENU_LOG
#define S_HITSOFT PSTR("HIT SOFT")
#define S_HITHARD PSTR("HIT HARD")
#define S_WAIT PSTR("WAIT...")
#define S_END PSTR("END")
#endif

#if MENU
#define ADD(x) x[eMenuPage-2]=(x[eMenuPage-2]+1)%128;
#define SUB(x) x[eMenuPage-2]=x[eMenuPage-2]-1>-1?x[eMenuPage-2]-1:127;
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

#define S_KIT PSTR("Kit")
#define S_BOXER PSTR("Boxer")
#define S_FAT_STACKS PSTR("FatStacks")
#define S_BOMBASTIX PSTR("Bombastix")

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

//==============================
//    MENU     
//==============================
void Menu()
{
  byte btnB = digitalRead(6);
  byte btnA = digitalRead(7);
  unsigned long now = millis();
  
  if(btnB==HIGH && btnA==HIGH) softReset();
  
  if(btnA_State==0) {
    if(btnA==HIGH)
    {
      btnA_State=1;
      btnA_Time=now;
    }
  }
  else  if(btnA_State==1) {
    if(btnA==LOW)
    {
      btnA_State=2;
    }
    else if((btnA==HIGH) && (now> btnA_Time + HOLDDELAY))
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
    if(btnA==LOW)
    {
      btnA_State=0;
    }
  }
  
  
  if(btnB_State==0) {
    if(btnB==HIGH)
    {
      btnB_State=1;
      btnB_Time=now;
    }
  }
  else  if(btnB_State==1) {
    if(btnB==LOW)
    {
      btnB_State=2;
    }
    else if((btnB==HIGH) && (now> btnB_Time + HOLDDELAY))
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
    if(btnB==LOW)
    {
      btnB_State=0;
    }
  }
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
    if(eMenuPage==0) Mode=(Mode+1)%4; //MODE
    else if(eMenuPage==1) eMenuGeneral=(eMenuGeneral+1)%7;//General
    else if(eMenuPage>=2 && eMenuPage<50) eMenuPin=(eMenuPin+1)%12;//Pin
    else if(eMenuPage==50) eMenuPage=(eMenuPage+1)%51;//eMenuLog=(eMenuLog+1)%2;//LOG
  }
  else if(eMenuSelect==2)
  {
    if(eMenuPage==1)//General
    {
          /*if(eMenuGeneral==0) delayTime=(delayTime+1)%999;//Delay
          else*/ if(eMenuGeneral==1) GeneralXtalk=(GeneralXtalk+1)%8;//XTalk
          #if WAVTRIGGER
          else if(eMenuGeneral==2) kit=(kit+1)%3;
          #endif
          else if(eMenuGeneral==3) { HHThresoldSensor[0]=(HHThresoldSensor[0]+1)%128; SaveHHEEPROM(0x04,HHThresoldSensor[0]); }
          else if(eMenuGeneral==4) { HHThresoldSensor[1]=(HHThresoldSensor[1]+1)%128; SaveHHEEPROM(0x05,HHThresoldSensor[1]); }
          else if(eMenuGeneral==5) { HHThresoldSensor[2]=(HHThresoldSensor[2]+1)%128; SaveHHEEPROM(0x06,HHThresoldSensor[2]); }
          else if(eMenuGeneral==6) { HHThresoldSensor[3]=(HHThresoldSensor[3]+1)%128; SaveHHEEPROM(0x07,HHThresoldSensor[3]); }
        }
        else if(eMenuPage>=2  && eMenuPage<50)//Pin
        {
          switch(eMenuPin)
          {
            case 0: ADD(NoteSensor); SAVE(NOTE); break;//Note
            case 1: ADD(ThresoldSensor); SAVE(THRESOLD); break;//Thresold
            case 2: ADD(ScanTimeSensor); SAVE(SCANTIME); break;//ScanTime
            case 3: ADD(MaskTimeSensor); SAVE(MASKTIME); break;//MaskTime
            case 4: ADD(RetriggerSensor); SAVE(RETRIGGER); break;//Retrigger
            case 5: ADD(CurveSensor); SAVE(CURVE); break;//Curve
            case 6: ADD(CurveFormSensor); SAVE(CURVEFORM); break;//CurveForm
            case 7: ADD(XtalkSensor); SAVE(XTALK); break;//XTalk
            case 8: ADD(XtalkGroupSensor); SAVE(XTALKGROUP); break;//XTalkGroup
            case 9: TypeSensor[eMenuPage-2]=(TypeSensor[eMenuPage-2]+1)%128; SAVE(TYPE); break;//Type
            case 10: ADD(ChokeNoteSensor); SAVE(CHOKENOTE); break;//ChokeNote
            #if ENABLE_CHANNEL
            case 11:  ADD(ChannelSensor); SAVE(CHANNEL); break;//Channel
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
  lcd.noAutoscroll();
  
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
        #if WAVTRIGGER
        wtPrintName((eMenuPage-2),false);
        #else
        MenuString(S_PIN,false);
        #endif
        MenuString((eMenuPage-2),eMenuSelect==0);

      }
      else if (eMenuPage==50) {if(Mode==ToolMode) Diagnostic=true; else MenuString(S_LOGDIS,false);}//MenuString(S_LOG,eMenuSelect==0);
      //else if (eMenuPage==51) { MenuString(S_NULL,eMenuSelect==0);  lcd.setCursor(16,1);  lcd.autoscroll(); }

      
      //MENU=2
      lcd.setCursor(0,1);
      if(eMenuPage==0) 
      {
        switch(Mode)
        {
          case OffMode: MenuString(S_OFF,eMenuSelect==1); break;
          case StandbyMode: MenuString(S_STANDBY,eMenuSelect==1); break;
          case MIDIMode: MenuString(S_MIDI,eMenuSelect==1); break;
          case ToolMode: MenuString(S_TOOL,eMenuSelect==1); break;
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
        }
      }
      else if(eMenuPage>=2 && eMenuPage<50)
      {
        if(Mode==ToolMode) 
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
          if(TypeSensor[eMenuPage-2]==PIEZO)
            MenuString(S_GAIN,eMenuSelect==1);
          else
            MenuString(S_CKNOTE,eMenuSelect==1); 
 
          break;
          //case 11: MenuString(S_DUAL,eMenuSelect==1); break;
          case 11: MenuString(S_CHANNEL,eMenuSelect==1); break;
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
        if(eMenuGeneral==0) MenuString(delayTime,eMenuSelect==2);
        else if(eMenuGeneral==1) MenuString(GeneralXtalk,eMenuSelect==2);
        #if WAVTRIGGER
        else if(eMenuGeneral==2) 
        {
          lcd.setCursor(5,1);
          if(kit==0) MenuString(S_FAT_STACKS,eMenuSelect==2);
          else if(kit==1) MenuString(S_BOXER,eMenuSelect==2);
          else if(kit==2) MenuString(S_BOMBASTIX,eMenuSelect==2);
        }
        #endif
        else if(eMenuGeneral==3) MenuString(HHThresoldSensor[0],eMenuSelect==2);
        else if(eMenuGeneral==4) MenuString(HHThresoldSensor[1],eMenuSelect==2);
        else if(eMenuGeneral==5) MenuString(HHThresoldSensor[2],eMenuSelect==2);
        else if(eMenuGeneral==6) MenuString(HHThresoldSensor[3],eMenuSelect==2);
      }
      else if(eMenuPage>=2  && eMenuPage<50)
      {
        if(eMenuPin==0) MenuString(NoteSensor[eMenuPage-2],eMenuSelect==2);
        else if(eMenuPin==1) MenuString(ThresoldSensor[eMenuPage-2],eMenuSelect==2);
        else if(eMenuPin==2) MenuString(ScanTimeSensor[eMenuPage-2],eMenuSelect==2);
        else if(eMenuPin==3) MenuString(MaskTimeSensor[eMenuPage-2],eMenuSelect==2);
        else if(eMenuPin==4) MenuString(RetriggerSensor[eMenuPage-2],eMenuSelect==2);
        else if(eMenuPin==5)
       {
         switch(CurveSensor[eMenuPage-2])
         {
           case 0: MenuString(S_LIN,eMenuSelect==2); break;
           case 1: MenuString(S_EXP,eMenuSelect==2); break;
           case 2: MenuString(S_LOG,eMenuSelect==2); break;
           case 3: MenuString(S_SGM,eMenuSelect==2); break;
           case 4: MenuString(S_FLT,eMenuSelect==2); break;
           default: MenuString(CurveSensor[eMenuPage-2],eMenuSelect==2);
         }
       }
        else if(eMenuPin==6) MenuString(CurveFormSensor[eMenuPage-2],eMenuSelect==2);
        else if(eMenuPin==7) MenuString(XtalkSensor[eMenuPage-2],eMenuSelect==2);
        else if(eMenuPin==8) MenuString(XtalkGroupSensor[eMenuPage-2],eMenuSelect==2);
        else if(eMenuPin==9)
       {
         switch(TypeSensor[eMenuPage-2])
         {
           case 0: MenuString(S_PIEZO,eMenuSelect==2); break;
           case 1: MenuString(S_SWITCH,eMenuSelect==2); break;
           case 2: MenuString(S_HHC,eMenuSelect==2); break;
           case 3: MenuString(S_HH,eMenuSelect==2); break;
           case 4: MenuString(S_HHS,eMenuSelect==2); break;
           case 5: MenuString(S_YSWITCH,eMenuSelect==2); break;
           case 127: MenuString(S_DISABLED,eMenuSelect==2); break;
         }
       }
        else if(eMenuPin==10) MenuString(ChokeNoteSensor[eMenuPage-2],eMenuSelect==2);
        else if(eMenuPin==11) MenuString(DualSensor(eMenuPage-2),eMenuSelect==2);
        #if ENABLE_CHANNEL
        else if(eMenuPin==12) MenuString(ChannelSensor[eMenuPage-2],eMenuSelect==2);
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
        if(eMenuPage==0) Mode=Mode-1>-1?Mode-1:3; //MODE
        else if(eMenuPage==1) eMenuGeneral=eMenuGeneral-1>-1?eMenuGeneral-1:6; //General
        else if(eMenuPage>=2 && eMenuPage<50) eMenuPin=eMenuPin-1>-1?eMenuPin-1:11; //Pin
        else if(eMenuPage==50) eMenuPage=(eMenuPage-1)>-1?eMenuPage-1:50;//eMenuLog=eMenuLog-1>-1?eMenuLog-1:1;//Log
      }
      else if(eMenuSelect==2)
      {
        if(eMenuPage==1)//General
        {
          /*if(eMenuGeneral==0) delayTime=delayTime-1>-1?delayTime-1:999;//Delay
          else*/ if(eMenuGeneral==1) GeneralXtalk=(GeneralXtalk-1>-1)?GeneralXtalk-1:7;//General XTalk
          #if WAVTRIGGER
          else if(eMenuGeneral==2) kit=(kit-1>-1)?kit-1:2;
          #endif
          else if(eMenuGeneral==3) { HHThresoldSensor[0]=((HHThresoldSensor[0]-1)>-1)?HHThresoldSensor[0]-1:127; SaveHHEEPROM(0x04,HHThresoldSensor[0]); }
          else if(eMenuGeneral==4) { HHThresoldSensor[1]=((HHThresoldSensor[1]-1)>-1)?HHThresoldSensor[1]-1:127; SaveHHEEPROM(0x05,HHThresoldSensor[1]); }
          else if(eMenuGeneral==5) { HHThresoldSensor[2]=((HHThresoldSensor[2]-1)>-1)?HHThresoldSensor[2]-1:127; SaveHHEEPROM(0x06,HHThresoldSensor[2]); }
          else if(eMenuGeneral==6) { HHThresoldSensor[3]=((HHThresoldSensor[3]-1)>-1)?HHThresoldSensor[3]-1:127; SaveHHEEPROM(0x07,HHThresoldSensor[3]); }
        }
        else if(eMenuPage>=2  && eMenuPage<50)//Pin
        {
          switch(eMenuPin)
          {
            case 0: SUB(NoteSensor); SAVE(NOTE); break;//Note
            case 1: SUB(ThresoldSensor); SAVE(THRESOLD); break;//Thresold
            case 2: SUB(ScanTimeSensor); SAVE(SCANTIME); break;//ScanTime
            case 3: SUB(MaskTimeSensor); SAVE(MASKTIME); break;//MaskTime
            case 4: SUB(RetriggerSensor); SAVE(RETRIGGER); break;//Retrigger
            case 5: SUB(CurveSensor); SAVE(CURVE); break;//Curve
            case 6: SUB(CurveFormSensor); SAVE(CURVEFORM); break;//CurveForm
            case 7: SUB(XtalkSensor); SAVE(XTALK); break;//XTalk
            case 8: SUB(XtalkGroupSensor); SAVE(XTALKGROUP); break;//XTalkGroup
            case 9: SUB(TypeSensor); SAVE(TYPE); break;//Type
            case 10: SUB(ChokeNoteSensor); SAVE(CHOKENOTE); break;//ChokeNote
            #if ENABLE_CHANNEL
            case 11: SUB(ChannelSensor); SAVE(CHANNEL); break;//Channel
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
   if(x==0) MenuString(S_WAIT,false);
   else if(x==1) MenuString(S_HITSOFT,false);
   else if(x==2) MenuString(S_HITHARD,false);
   else if(x==3) MenuString(S_END,false);
   MenuString(d_tnum,'(',')');
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

void DrawDiagnostic(byte i,byte val)
{
  if(i>31) return;
  
  lcd.setCursor(i%16,i/16);
  lcd.print(val/16);
  
}
void MenuString(int inter,bool sel) //sz si puo togliere
{
  if(sel) MenuString(inter,'<','>');
  else MenuString(inter,' ',' ');
}

void MenuString(int inter,char A,char B) //sz si puo togliere
{
  if(inter>999) B='e';
  
  char cInter[]=" 000 ";
  cInter[0]=A; cInter[4]=B;
  cInter[3]+=inter%10;
  cInter[2]+=(inter%100)/10;
  cInter[1]+=(inter%1000)/100;
  lcd.print(cInter);
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
