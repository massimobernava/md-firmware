#if MENU
//==============================
//    MENU beta      
//==============================
void Menu()
{
  int btnOk = digitalRead(6);
  int btnA = digitalRead(7);
  int btnB = digitalRead(5);
    
  if(btnOk!=btnOk_Last && btnOk==HIGH) { eMenuChange=(eMenuChange+1)%3; Changed=true;}
    
  if(Changed) 
  {
      lcd.clear();
      lcd.noAutoscroll();
         
      //MENU=1
      lcd.setCursor(0,0);
      if(eMenuValue==0) MenuString("MODE",4,eMenuChange==0);
      else if(eMenuValue==1) MenuString("General",7,eMenuChange==0);
      else if(eMenuValue>=2 && eMenuValue<50)
      {
        char cPin[]="Pin 000";
        cPin[6]+=(eMenuValue-2)%10;
        cPin[5]+=(eMenuValue-2)/10;
        MenuString(cPin,7,eMenuChange==0);
      }
      else if (eMenuValue==50) MenuString("LOG",3,eMenuChange==0);
      else if (eMenuValue==51) { MenuString("---",3,eMenuChange==0);  lcd.setCursor(16,1);  lcd.autoscroll(); }
      
      //MENU=2
      lcd.setCursor(0,1);
      if(eMenuValue==0) 
      {
        switch(Mode)
        {
          case OffMode: MenuString("OFF",3,eMenuChange==1); break;
          case StandbyMode: MenuString("StandBy",7,eMenuChange==1); break;
          case MIDIMode: MenuString("MIDI",4,eMenuChange==1); break;
          case ToolMode: MenuString("Tool",4,eMenuChange==1); break;
        }
      }
      else if(eMenuValue==1)
      {
        switch(eMenuGeneral)
        {
          case 0: MenuString("Delay",5,eMenuChange==1); break;
          case 1: MenuString("XTalk",5,eMenuChange==1); break;
        }
      }
      else if(eMenuValue>=2 && eMenuValue<50)
      {
        switch(eMenuPin)
        {
          case 0: MenuString("Note",4,eMenuChange==1); break;
          case 1: MenuString("Thresold",8,eMenuChange==1); break;
          case 2: MenuString("ScanTime",8,eMenuChange==1); break;
          case 3: MenuString("MaskTime",8,eMenuChange==1); break;
          case 4: MenuString("Retrig",7,eMenuChange==1); break;
          case 5: MenuString("Curve",5,eMenuChange==1); break;
          case 6: MenuString("CurveF",6,eMenuChange==1); break;
          case 7: MenuString("XTalk",5,eMenuChange==1); break;
          case 8: MenuString("XTalkG",6,eMenuChange==1); break;
          case 9: MenuString("Type",4,eMenuChange==1); break;
          case 10: MenuString("CkNote",6,eMenuChange==1); break;
          case 11: MenuString("Dual",4,eMenuChange==1); break;
          case 12: MenuString("DualNote",8,eMenuChange==1); break;
          case 13: MenuString("DThres",9,eMenuChange==1); break;
          case 14: MenuString("Channel",7,eMenuChange==1); break;
        }
      }
      else if(eMenuValue==50)
     {
       switch(eMenuLog)
       {
         case 0: MenuString("Pin",3,eMenuChange==1); break;
         case 1: MenuString("Thresold",8,eMenuChange==1); break;
       }
     }
    
      //MENU=3
      lcd.setCursor(11,1);
      if(eMenuValue==1)
      {
        if(eMenuGeneral==0) MenuString(delayTime,3,eMenuChange==2);
        else if(eMenuGeneral==1) MenuString(GeneralXtalk,2,eMenuChange==2);
      }
      else if(eMenuValue>=2  && eMenuValue<50)
      {
        if(eMenuPin==0) MenuString(NoteSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==1) MenuString(ThresoldSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==2) MenuString(ScanTimeSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==3) MenuString(MaskTimeSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==4) MenuString(RetriggerSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==5) MenuString(CurveSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==6) MenuString(CurveFormSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==7) MenuString(XtalkSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==8) MenuString(XtalkGroupSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==9) MenuString(TypeSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==10) MenuString(ChokeNoteSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==11) MenuString(DualSensor[eMenuValue-2],4,eMenuChange==2);
        //else if(eMenuPin==12) MenuString(DualNoteSensor[eMenuValue-2],4,eMenuChange==2);
        //else if(eMenuPin==13) MenuString(DualThresoldSensor[eMenuValue-2],4,eMenuChange==2);
        else if(eMenuPin==14) MenuString(ChannelSensor[eMenuValue-2],4,eMenuChange==2);
      }
      else if(eMenuValue==50)
      {
       switch(eMenuLog)
       {
         case 0: MenuString(LogPin,4,eMenuChange==2);break;
         case 1: MenuString(LogThresold,4,eMenuChange==2);break;
       }
      }
        
      Changed=false;
  }
    
  btnOk_Last=btnOk;
    
  //CONTROL
    
  if(btnA!=btnA_Last && btnA==HIGH)
  {
      Changed=true;
      //0=MODE,1=General,2-49=Pin
      if(eMenuChange==0) eMenuValue=(eMenuValue+1)%52;
      else if(eMenuChange==1)
      {
        if(eMenuValue==0) Mode=(Mode+1)%4; //MODE
        else if(eMenuValue==1) eMenuGeneral=(eMenuGeneral+1)%2;//General
        else if(eMenuValue>=2 && eMenuValue<50) eMenuPin=(eMenuPin+1)%15;//Pin
        else if(eMenuValue==50) eMenuLog=(eMenuLog+1)%2;//LOG
      }
      else if(eMenuChange==2)
      {
        if(eMenuValue==1)//General
        {
          /*if(eMenuGeneral==0) delayTime=(delayTime+1)%999;//Delay
          else*/ if(eMenuGeneral==1) GeneralXtalk=(GeneralXtalk+1)%8;//XTalk
        }
        else if(eMenuValue>=2  && eMenuValue<50)//Pin
        {
          switch(eMenuPin)
          {
            case 0: NoteSensor[eMenuValue-2]=(NoteSensor[eMenuValue-2]+1)%256;break;//Note
            case 1: ThresoldSensor[eMenuValue-2]=(ThresoldSensor[eMenuValue-2]+1)%256;break;//Thresold
            case 2: ScanTimeSensor[eMenuValue-2]=(ScanTimeSensor[eMenuValue-2]+1)%256;break;//ScanTime
            case 3: MaskTimeSensor[eMenuValue-2]=(MaskTimeSensor[eMenuValue-2]+1)%256;break;//MaskTime
            case 4: RetriggerSensor[eMenuValue-2]=(RetriggerSensor[eMenuValue-2]+1)%256;break;//Retrigger
            case 5: CurveSensor[eMenuValue-2]=(CurveSensor[eMenuValue-2]+1)%256;break;//Curve
            case 6: CurveFormSensor[eMenuValue-2]=(CurveFormSensor[eMenuValue-2]+1)%256;break;//CurveForm
            case 7: XtalkSensor[eMenuValue-2]=(XtalkSensor[eMenuValue-2]+1)%256;break;//XTalk
            case 8: XtalkGroupSensor[eMenuValue-2]=(XtalkGroupSensor[eMenuValue-2]+1)%256; break;//XTalkGroup
            case 9: TypeSensor[eMenuValue-2]=(TypeSensor[eMenuValue-2]+1)%128; break;//Type
            case 10: ChokeNoteSensor[eMenuValue-2]=(ChokeNoteSensor[eMenuValue-2]+1)%256; break;//ChokeNote
            //case 11: DualSensor[eMenuValue-2]=(DualSensor[eMenuValue-2]+1)%256; break;//Dual
            //case 12: DualNoteSensor[eMenuValue-2]=(DualNoteSensor[eMenuValue-2]+1)%256; break;//DualNote
            //case 13: DualThresoldSensor[eMenuValue-2]=(DualThresoldSensor[eMenuValue-2]+1)%256; break;//DualThresold
            case 14: ChannelSensor[eMenuValue-2]=(ChannelSensor[eMenuValue-2]+1)%256; break;//Channel
          }
        }
        else if(eMenuValue==50) //LOG
        {
          switch(eMenuLog)
          {
            case 0: LogPin=(LogPin+1)%49; break;
            case 1: LogThresold=(LogThresold+1)%256; break;
          }
        }
      }
  }
    
  if(btnB!=btnB_Last && btnB==HIGH)
  {
      Changed=true;
      if(eMenuChange==0) eMenuValue=(eMenuValue-1)>-1?eMenuValue-1:51;
      else if(eMenuChange==1)
      {
        if(eMenuValue==0) Mode=Mode-1>-1?Mode-1:3; //MODE
        else if(eMenuValue==1) eMenuGeneral=eMenuGeneral-1>-1?eMenuGeneral-1:1; //General
        else if(eMenuValue>=2 && eMenuValue<50) eMenuPin=eMenuPin-1>-1?eMenuPin-1:14; //Pin
        else if(eMenuValue==50) eMenuLog=eMenuLog-1>-1?eMenuLog-1:1;//Log
      }
      else if(eMenuChange==2)
      {
        if(eMenuValue==1)//General
        {
          /*if(eMenuGeneral==0) delayTime=delayTime-1>-1?delayTime-1:999;//Delay
          else*/ if(eMenuGeneral==1) GeneralXtalk=(GeneralXtalk-1>-1)?GeneralXtalk-1:7;//General XTalk
        }
        else if(eMenuValue>=2  && eMenuValue<50)//Pin
        {
          switch(eMenuPin)
          {
            case 0: NoteSensor[eMenuValue-2]=NoteSensor[eMenuValue-2]-1>-1?NoteSensor[eMenuValue-2]-1:255; break;//Note
            case 1: ThresoldSensor[eMenuValue-2]=ThresoldSensor[eMenuValue-2]-1>-1?ThresoldSensor[eMenuValue-2]-1:255;break;//Thresold
            case 2: ScanTimeSensor[eMenuValue-2]=ScanTimeSensor[eMenuValue-2]-1>-1?ScanTimeSensor[eMenuValue-2]-1:255;break;//ScanTime
            case 3: MaskTimeSensor[eMenuValue-2]=MaskTimeSensor[eMenuValue-2]-1>-1?MaskTimeSensor[eMenuValue-2]-1:255;break;//MaskTime
            case 4: RetriggerSensor[eMenuValue-2]=RetriggerSensor[eMenuValue-2]-1>-1?RetriggerSensor[eMenuValue-2]-1:255;break;//Retrigger
            case 5: CurveSensor[eMenuValue-2]=CurveSensor[eMenuValue-2]-1>-1?CurveSensor[eMenuValue-2]-1:255;break;//Curve
            case 6: CurveFormSensor[eMenuValue-2]=CurveFormSensor[eMenuValue-2]-1>-1?CurveFormSensor[eMenuValue-2]-1:255;break;//CurveForm
            case 7: XtalkSensor[eMenuValue-2]=XtalkSensor[eMenuValue-2]-1>-1?XtalkSensor[eMenuValue-2]-1:255;break;//XTalk
            case 8: XtalkGroupSensor[eMenuValue-2]=XtalkGroupSensor[eMenuValue-2]-1>-1?XtalkGroupSensor[eMenuValue-2]-1:255;break;//XTalkGroup
            case 9: TypeSensor[eMenuValue-2]=TypeSensor[eMenuValue-2]-1>-1?TypeSensor[eMenuValue-2]-1:127;break;//Type
            case 10: ChokeNoteSensor[eMenuValue-2]=ChokeNoteSensor[eMenuValue-2]-1>-1?ChokeNoteSensor[eMenuValue-2]-1:255;break;//ChokeNote
            //case 11: DualSensor[eMenuValue-2]=DualSensor[eMenuValue-2]-1>-1?DualSensor[eMenuValue-2]-1:255;break;//Dual
            //case 12: DualNoteSensor[eMenuValue-2]=DualNoteSensor[eMenuValue-2]-1>-1?DualNoteSensor[eMenuValue-2]-1:255;break;//DualNote
            //case 13: DualThresoldSensor[eMenuValue-2]=DualThresoldSensor[eMenuValue-2]-1>-1?DualThresoldSensor[eMenuValue-2]-1:255;break;//DualThresold
            case 14: ChannelSensor[eMenuValue-2]=ChannelSensor[eMenuValue-2]-1>-1?ChannelSensor[eMenuValue-2]-1:255;break;//Channel
          }
        
        }
        else if(eMenuValue==50)
        {
          switch(eMenuLog)
          {
            case 0: LogPin=LogPin-1>-1?LogPin-1:48; break;
            case 1: LogThresold=LogThresold-1>-1?LogThresold-1:255; break;
          }
        }
      }
  }
  btnA_Last=btnA;
  btnB_Last=btnB;
}   

void MenuString(String str,bool sel)
{
  char cStr[str.length()];
  str.toCharArray(cStr,str.length());
  MenuString(cStr,str.length(),sel);
}
void MenuString(int inter,int sz,bool sel)
{
  String sInter=String(inter);
  char cInter[sz];
  sInter.toCharArray(cInter,sz);
  MenuString(cInter,sInter.length(),sel);
}

void MenuString(char* str,int sz,bool sel)
{
  char ret[17]="                ";
  if(sel) ret[0]='<';
  int i=0;
  while(i<sz) ret[i+1]=str[i++];
  if(sel) ret[i+2]='>';
 lcd.print(ret);
}

#endif
