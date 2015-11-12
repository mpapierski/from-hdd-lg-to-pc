
//                                    19.08.2008
#ifndef __HDD_LG_MK_NAME__
#define __HDD_LG_MK_NAME__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_mk_name ==================================
int  MakeOneStrForTree(char *oneStr, char *Name, char *Ext, PAR_FILE *pf);//�������� ������ �������� ��� ��������� � ������
int  ViewAbsendName(void);                                   //����� ���� �������������� � ����� MME.DB, �� ������������� � ��������

static int nErr1 = 0, nErr2 = 0, nErr3 = 0, nErr4 = 0;       //��� ����������� ����� ��������� �� ������
static char *strSize[] = { "", "Kb", "Mb", "Gb" };

//------------------------------------------------------------------------------

static int Ctrl_Get_Size_From_FAT(DWORD clSt, LONGLONG *SizeFromFAT)//���������� ������������ ����� ����� �� FAT
{
   DWORD nCl = clSt;                                         //������� ����� �������� ����� ������� �������� �����
   DWORD numCl = 0;                                          //����� ��������� ����������� � FAT ��� ������� �����
/*
   for(;; numCl++)                                           //���� �� ������� ���������
   {  if(nCl == 0x0FFFFFFF) break;                           //������ ������� ����� ������� FAT
      if(*(FAT + nCl) == 0) return -1;                       //����� ������� ���������
      nCl = *(FAT + nCl);                                    //����� ���������� ��������
      if(nCl > max_ZapFAT && nCl != 0x0FFFFFFF) return -2;   //����� �������� � ������� FAT ��������� ���������� ��������
   }
   *SizeFromFAT =  LONGLONG(numCl) * sCl_B;                  //����������� ��������� ������ ����� ��� ������� ����� ���������
   return 0;
*/
   for(;; numCl++)                                           //���� �� ������� ���������
   {  if(nCl == 0x0FFFFFFF) break;                           //������ ������� ����� ������� FAT
      if(*(FAT1 + nCl) == 0) return -1;                      //����� ������� ���������
      nCl = *(FAT1 + nCl);                                   //����� ���������� ��������
      if(nCl > maxZapFAT1 && nCl != 0x0FFFFFFF) return -2;   //����� �������� � ������� FAT ��������� ���������� ��������
     *SizeFromFAT =  LONGLONG(numCl) * sCl_B;                //����������� ��������� ������ ����� ��� ������� ����� ���������
   }
   *SizeFromFAT =  LONGLONG(numCl+1) * sCl_B;                //����������� ��������� ������ ����� ��� ������� ����� ���������
   return 0;
}

//------------------------------------------------------------------------------

static int ChangeNameInMEDIA(char *nam, char *ext, PAR_FILE *pf)//�������� ����� � ����� MEDIA
{
   int prF = -1;                                             //��� �����
   if(lstrcmp(ext, "idx") == 0) prF = 0;
   if(lstrcmp(ext, "str") == 0) prF = 1;                     //
   if((Conf.altName == 0 || Conf.ViewIDX == 0) && prF == 0) return 1; //�� ���������� �����  *.idx
   if((Conf.altName == 0 || Conf.Ren_STR == 0) && prF == 1)  //������ ���������� ������ *.str
       lstrcpy(ext, "vro");
   (aTree + numEl_Tree)->numPart = 0;                        //����� ����� �������������� Title
   (aTree + numEl_Tree)->indTitle = 0xFFFF;
   (aTree + numEl_Tree)->indFolder = 0;                      //������ �� ��� �����
   (aTree + numEl_Tree)->numCopy = 0;                        //������� ������������� ������
   (aTree + numEl_Tree)->prSel = 0;                          //������� ������ ������� �����(0-�� ������)

   indTabMME = -1;                                           //������ � ������� MME.DB ��� �������� ����� � ����� MEDIA
   if(Conf.altName == 0 && tabMME != NULL)                   //����� �������������� ����� � ���� ������� ���� �� MME.DB
   {  for(DWORD n=0; n<numNam; n++)                          //�� ����� ���� � ������� �� MME.DB
        if(lstrcmp(nam, (tabMME + n)->NameF) == 0)           //��������� ���� �� �������� � �� ����� ����. ����� �������
        {  (aTree + numEl_Tree)->numPart = (tabMME + n)->numPart;//����� ����� �������������� Title
           (aTree + numEl_Tree)->indTitle = (tabMME + n)->indTitle;
           if(pf->SizeF == (tabMME + n)->SizeF)              //������� ����� ������
           {  indTabMME = n;                                 //������ � ������� �� MME.DB ��� �������� ����� � ����� MEDIA
              (tabMME + n)->prYes = 1;                       //�������, ��� ��� ������ (1 - ���� � � MME.DB � � ��������)
              return 0;                                      //���� ������
           }
           else                                              //��� �����, �� ����� ������ ����������
           {  LONGLONG SizeFromFAT;
              if(Ctrl_Get_Size_From_FAT(pf->ClSt, &SizeFromFAT) < 0 ||//���������� ������������ ����� ����� �� FAT
                 pf->SizeF > SizeFromFAT)
              {  pf->SizeF = SizeFromFAT;//0;                //� ������ � ����������� � FAT ��������� ����� == 0
                 if(nErr2 == 0)                              //��������� �� ������ ��� ������
                   Error3((Lan+138)->msg, (Lan+139)->msg, (Lan+152)->msg);
                 nErr2++;                                    //����� �� ����������� ���������
                 pf->type = 34;                              //������� ��� ������ ����������� ������ E2
              }
              else
              {  if(nErr3 == 0)                              //��������� �� ������ ��� ������
                   Error3((Lan+141)->msg, (Lan+142)->msg, (Lan+143)->msg);
                 nErr3++;                                    //����� �� ����������� ���������
                 pf->type = 33;                              //������� ��� ������ ����������� ������ e1
              }
              indTabMME = n;                                 //������ � ������� MME.DB ��� �������� ����� � ����� MEDIA
              (tabMME + n)->prYes = 1;                       //�������, ��� ��� ������ (1 - ���� � � MME.DB � � ��������)
              return 0;
           }
        }
      //������ ��� � ����� �� ����� � ����� �������� ����
      if(nErr1 == 0)                                         //��������� �� ������ ��� ������
        Error4((Lan+108)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg);
      nErr1++;                                               //����� �� ����������� ���������
   }
   //���� ��� ����� MME.DB ��� ��� ������������� ��������� (������� � �������)
   if(Conf.SwapNPart == 0)                                   //�������� ������������ ������ ����� � ����� ����� �����
   {  char nPart[8];
      int iPart;
      MoveMemory(nPart, nam, 4);                             //��������� ������� ��������
      *(nPart + 4) = 0;
      sscanf(nPart, "%X", &iPart);
      strncpy(nam, nam + 4, 42);                             //��������� (��������) ��� ����� ��� �������� (���������� �����)
      if(Conf.ChangeNul == 0)// && prF == 1)                 //�������� ������ 0 �� _ � �������� ������
      {  for(int i=0; i<4; i++)
         {  if(*(nPart + i) != '0') break;                   //��������� �����
            if(*(nPart + i) == '0') *(nPart + i) = '_';
         }
         if(pf->lL2 != 0 && *(nPart + 3) == '_') *(nPart + 3) = '0'; //������������ 0 ��� ������ ����� ���� �� �����
      }
      if(!(Conf.NoViewOnePart == 0 && pf->lL2 == 0) ||       //0-�� ��������������� ��� ����� �����
         prF == 0 ||                                         //��� �� ���� *.std (*.vro)
         iPart != 0)                                         //���� ����� �����
      {  lstrcat(nam, ".");                                  //�������� ����� � �����
         strncat(nam, nPart, 4);                             //�������� ����� �����
      }
      LONGLONG SizeFromFAT;
      if(Ctrl_Get_Size_From_FAT(pf->ClSt, &SizeFromFAT) < 0 ||//���������� ������������ ����� ����� �� FAT
         pf->SizeF > SizeFromFAT)
      {  pf->SizeF = SizeFromFAT;//0;                        //� ������ � ����������� � FAT ��������� ����� == 0
         if(nErr2 == 0)                                      //��������� �� ������ ��� ������
           Error3((Lan+138)->msg, (Lan+139)->msg, (Lan+152)->msg);
         nErr2++;                                            //����� �� ����������� ���������
         pf->type = 34;                                      //������� ��� ������ ����������� ������ E2
      }
   }
   return 0;
}

//------------------------------------------------------------------------------

static void MkInformName(char *Name, char *Ext)
{
   if(indTabMME < 0) return;                                 //�� ������� ������������ ����
   TABL_MME *tMME = tabMME + indTabMME;                      //������ ������� ����������� � ������� �����
   int l = wsprintf(Name, "%s", tMME->aName);
   if(Conf.poz_In == 1)                                      //�������� ������: 0-� �������, 1-� �����, 2-�� ����������
     l += wsprintf(Name + l, ".%s", tMME->nameIn);
   if(Conf.poz_Ql == 1)                                      //�������� ������: 0-� �������, 1-� �����, 2-�� ����������
     l += wsprintf(Name + l, ".%s", tMME->Qual);
   if(tMME->numPart > 0)                                     //���� ������ ����� �����
     sprintf(Name + l, ".%03d", tMME->numPart);              //�������� ����� �����
   lstrcpy(Ext, "vro");
}

//------------------------------------------------------------------------------

int MakeOneStrForTree(char *oneStr, char *Name, char *Ext, PAR_FILE *pf)//�������� ������ �������� ��� ��������� � ������
{
   char size[32], *as;
   if(pf->type == 48)                                        //��� ��� �����
   {  sprintf(oneStr, "%s", Name);                           //��� ����� ������ �� ������
      return 0;
   }
   if(prMEDIA == 1)                                          //����� MEDIA
      if(ChangeNameInMEDIA(Name, Ext, pf) == 1) return 1;    //�������� ����� � ����� MEDIA
   if(prMEDIA == 1 && Conf.altName == 0 && tabMME != NULL)
      MkInformName(Name, Ext);                               //������������ �������������� �����
   if(*Ext != 0)                                             //�������� ���������� ���� ��� ����
   {  lstrcat(Name, ".");
      lstrcat(Name, Ext);
   }
   int l = sprintf(oneStr, "  %s", Name);
   for(int i=l; i<256; i++)                                  //��������� ����� ������ ���������
       *(oneStr + i) = ' ';
   switch(Conf.typeSize)                                     //0-�����, 1-������, 2-������, 3-������
   {  case 0: sprintf(size, "%12.0lf", double(pf->SizeF));
              as = Char_Dig_p(size, 15);  break;             //�������������� ����������� ����� � ��������
      case 1: sprintf(size, "%11.1lf", double(pf->SizeF)/1024.0);
              as = Char_Dig_p_n(size, 12, 1);  break;        //�������������� ����������� ����� � ��������
      case 2: sprintf(size, "%9.2lf", double(pf->SizeF)/1024.0/1024.0);
              as = Char_Dig_p_n(size, 9, 2);  break;         //�������������� ����������� ����� � ��������
      case 3: sprintf(size, "%7.3lf", double(pf->SizeF)/1024.0/1024.0/1024.0);
              as = Char_Dig_p_n(size, 6, 3);  break;         //�������������� ����������� ����� � ��������
   }
/*
   if(prMEDIA == 1 && Conf.altName == 0 && tabMME != NULL)
     if((tabMME + indTabMME)->dt.year !=  pf.Year ||
        (tabMME + indTabMME)->dt.mon  !=  pf.Mon  ||
        (tabMME + indTabMME)->dt.day  !=  pf.Day  ||
        (tabMME + indTabMME)->dt.hour !=  pf.Hour ||
        (tabMME + indTabMME)->dt.min  !=  pf.Min  ||
        (tabMME + indTabMME)->dt.sec  !=  pf.Sec)
     {
        char sf[260], sb[260], ss[260];
        wsprintf(sf, "� �����: %02d.%02d.%04d  %02d:%02d:%02d", pf.Day, pf.Mon, pf.Year, pf.Hour, pf.Min, pf.Sec);
        wsprintf(sb, "� ����:    %02d.%02d.%04d  %02d:%02d:%02d",
                    (tabMME + indTabMME)->dt.day, (tabMME + indTabMME)->dt.mon, (tabMME + indTabMME)->dt.year,
                    (tabMME + indTabMME)->dt.hour, (tabMME + indTabMME)->dt.min, (tabMME + indTabMME)->dt.sec);
        WORD wHour = WORD((tabMME + indTabMME)->timeLong / 3600);
        int M1 = (tabMME + indTabMME)->timeLong - wHour * 3600;
        WORD wMinute = WORD(M1 / 60);
        WORD wSecond = WORD(M1 - wMinute * 60);
        wsprintf(ss, "������������: %02d:%02d:%02d", wHour, wMinute, wSecond);
        Error4("������������ ���:", ss, sb, sf);
     }
*/
   char sDate[64], sTime[64], Pr = ' ';
   SYSTEMTIME sysTime;
   if(prMEDIA == 1 && Conf.altName == 0 &&                   //��� ����� MEDIA � ���� ������� �������������� �����
      tabMME != NULL && *(DWORD*)Ext == 0x006F7276 &&        //���� ������ MME � ���� � ����������� *.vro
      Conf.typeTime == 0 &&                                  //0-���������� ����� ������ ������, 1-����� �������� �����
      indTabMME != -1)                                       //�� ������� ������������ ����
   {  pf->Year = (tabMME + indTabMME)->dt.year;
	  pf->Mon = BYTE((tabMME + indTabMME)->dt.mon);
	  pf->Day = BYTE((tabMME + indTabMME)->dt.day);
	  pf->Hour = BYTE((tabMME + indTabMME)->dt.hour);
	  pf->Min = BYTE((tabMME + indTabMME)->dt.min);
	  pf->Sec = BYTE((tabMME + indTabMME)->dt.sec);
      Pr = 'R';
   }
   sysTime.wYear = (pf->Year == 0) ? WORD(1900) : pf->Year;
   sysTime.wMonth = (pf->Mon == 0) ? WORD(1) : pf->Mon;
   sysTime.wDay = (pf->Day == 0) ? WORD(1) : pf->Day;
   sysTime.wHour = pf->Hour;
   sysTime.wMinute = pf->Min;
   sysTime.wSecond = pf->Sec;
   sysTime.wDayOfWeek = 1;
   sysTime.wMilliseconds = 0;

   GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, sDate, 64);
   GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, &sysTime, NULL, sTime, 64);
/*
   int prNoDT = 0;                                           //������� ���������� ����/������� (���� �� �����������)
   if((sysTime.wYear == 2004 && sysTime.wMonth == 1 && sysTime.wDay == 1) ||
       sysTime.wYear >= 2055 || sysTime.wYear < 2004) prNoDT = 1;                 //������� ���������� ����/������� (���� �� �����������)
   if(prNoDT == 1)                                           //������� ���������� ����/������� (���� �� �����������)
   {  for(int i=0; i<sizeof(sDate); i++)
      {  if(*(sDate+i) == 0)  break;
         if(*(sDate+i) >= '0' && *(sDate+i) <= '9') *(sDate+i) = '-';
      }
      for(int i=0; i<sizeof(sTime); i++)
      {  if(*(sTime+i) == 0)  break;
         if(*(sTime+i) >= '0' && *(sTime+i) <= '9') *(sTime+i) = '-';
      }
   }
*/
   if(prMEDIA == 1 && Conf.altName == 0 &&                   //��� ����� MEDIA � ���� ������� �������������� �����
      tabMME != NULL && *(DWORD*)Ext == 0x006F7276 &&        //���� ������ MME � ���� � ����������� *.vro
      indTabMME != -1)                                       //�� ������� ������������ ����
   {  sysTime.wHour = WORD((tabMME + indTabMME)->timeLong / 3600);
      int M1 = (tabMME + indTabMME)->timeLong - sysTime.wHour * 3600;
      sysTime.wMinute = WORD(M1 / 60);
      sysTime.wSecond = WORD(M1 - sysTime.wMinute * 60);
      sysTime.wMilliseconds = 0;
      char sLTime[64];
      GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, &sysTime, NULL, sLTime, 64);
      if(Conf.ViewVRO == 1)                                  //0-��������� ���������� *.vro, 1-�� ����������
         for(int i=l-4; i<l; i++)                            //������ ���������� .vro � �����
             *(oneStr + i) = ' ';
      int n = (Conf.poz_Ql == 0) ? 59 : 63;
      if(Conf.poz_In == 0)                                   //�������� ������: 0-� �������, 1-� �����, 2-�� ����������
         sprintf(oneStr + n,  "%s        ", (tabMME + indTabMME)->nameIn);
      if(Conf.poz_Ql == 0)                                   //�������� ������: 0-� �������, 1-� �����, 2-�� ����������
         sprintf(oneStr + 66, "%s        ", (tabMME + indTabMME)->Qual);
      sprintf(oneStr+70, "%s %s  [%8s]  %c %10s  %8s", as, strSize[Conf.typeSize], sLTime, Pr, sDate, sTime);
   }
   else  sprintf(oneStr + 52, "%s %s  %10s  %8s", as, strSize[Conf.typeSize], sDate, sTime);
   return 0;
}

//------------------------------------------------------------------------------

static int MkAbsendName(char *oneStr, char *Name, PAR_FILE *pf)//�������� ������ �������� ��� ��������� � ������ ��� ������������� �����
{
   char size[32], *as;
   if(pf->type == 48)                                        //��� ��� �����
   {  sprintf(oneStr, "%s", Name);                           //��� ����� ������ �� ������
      return 0;
   }
   char Ext[8];
   MkInformName(Name, Ext);                                  //������������ �������������� �����
   int l = sprintf(oneStr, "  %s", Name);
   for(int i=l; i<256; i++)                                  //��������� ����� ������ ���������
       *(oneStr + i) = ' ';
   switch(Conf.typeSize)                                     //0-�����, 1-������, 2-������, 3-������
   {  case 0: sprintf(size, "%12.0lf", double(pf->SizeF));
              as = Char_Dig_p(size, 15);  break;             //�������������� ����������� ����� � ��������
      case 1: sprintf(size, "%11.1lf", double(pf->SizeF)/1024.0);
              as = Char_Dig_p_n(size, 12, 1);  break;        //�������������� ����������� ����� � ��������
      case 2: sprintf(size, "%9.2lf", double(pf->SizeF)/1024.0/1024.0);
              as = Char_Dig_p_n(size, 9, 2);  break;         //�������������� ����������� ����� � ��������
      case 3: sprintf(size, "%7.3lf", double(pf->SizeF)/1024.0/1024.0/1024.0);
              as = Char_Dig_p_n(size, 6, 3);  break;         //�������������� ����������� ����� � ��������
   }
   char sDate[64], sTime[64], Pr = 'R';
   SYSTEMTIME sysTime;
   pf->Year = (tabMME + indTabMME)->dt.year;
   pf->Mon = BYTE((tabMME + indTabMME)->dt.mon);
   pf->Day = BYTE((tabMME + indTabMME)->dt.day);
   pf->Hour = BYTE((tabMME + indTabMME)->dt.hour);
   pf->Min = BYTE((tabMME + indTabMME)->dt.min);
   pf->Sec = BYTE((tabMME + indTabMME)->dt.sec);
   sysTime.wYear = (pf->Year == 0) ? WORD(1900) : pf->Year;
   sysTime.wMonth = (pf->Mon == 0) ? WORD(1) : pf->Mon;
   sysTime.wDay = (pf->Day == 0) ? WORD(1) : pf->Day;
   sysTime.wHour = pf->Hour;
   sysTime.wMinute = pf->Min;
   sysTime.wSecond = pf->Sec;
   sysTime.wDayOfWeek = 1;
   sysTime.wMilliseconds = 0;

   GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, sDate, 64);
   GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, &sysTime, NULL, sTime, 64);
   sysTime.wHour = WORD((tabMME + indTabMME)->timeLong / 3600);
   int M1 = (tabMME + indTabMME)->timeLong - sysTime.wHour * 3600;
   sysTime.wMinute = WORD(M1 / 60);
   sysTime.wSecond = WORD(M1 - sysTime.wMinute * 60);
   sysTime.wMilliseconds = 0;
   char sLTime[64];
   GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, &sysTime, NULL, sLTime, 64);
   int n = (Conf.poz_Ql == 0) ? 59 : 63;
   if(Conf.poz_In == 0)                                   //�������� ������: 0-� �������, 1-� �����, 2-�� ����������
      sprintf(oneStr + n,  "%s        ", (tabMME + indTabMME)->nameIn);
   if(Conf.poz_Ql == 0)                                   //�������� ������: 0-� �������, 1-� �����, 2-�� ����������
      sprintf(oneStr + 66, "%s        ", (tabMME + indTabMME)->Qual);
   sprintf(oneStr+70, "%s %s  [%8s]  %c %10s  %8s", as, strSize[Conf.typeSize], sLTime, Pr, sDate, sTime);
   return 0;
}

//------------------------------------------------------------------------------

static int AddAbsendNameToTree(PAR_FILE *pf, int Level)      //���������� ����� ������ � ������
{
   TV_INSERTSTRUCT tvins;
   char oneStr[256], Name[256];

   MkAbsendName(oneStr, Name, pf);                           //�������� ������ �������� ��� ��������� � ������ ��� ������������� �����

   lstrcpy((aTree + numEl_Tree)->NameF, Name);
   (aTree + numEl_Tree)->pf = *pf;
   (aTree + numEl_Tree)->numPart = 0;                        //����� ����� �������������� Title
   (aTree + numEl_Tree)->indTitle = 0xFFFF;
   (aTree + numEl_Tree)->indFolder = 0;                      //������ �� ��� �����
   (aTree + numEl_Tree)->numCopy = 0;                        //������� ������������� ������
   (aTree + numEl_Tree)->prSel = 0;                          //������� ������ ������� �����(0-�� ������)

   tvins.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   tvins.item.pszText = oneStr;
   tvins.item.lParam = numEl_Tree;
   tvins.item.iImage = tvins.item.iSelectedImage = 9;        //������ ������
   if(Conf.PrSort == 0)                                      //0-���������� �� �����, 1-��� ����������
      tvins.hInsertAfter = TVI_SORT;
   else
      tvins.hInsertAfter = TVI_LAST;
   if(Level == 0) tvins.hParent = NULL;
   else tvins.hParent = hPrev[Level-1];
   hPrev[Level] = TreeView_InsertItem(hwndTree, &tvins);
   if(++numEl_Tree > MAX_NAME)
      return Error1((Lan+31)->msg);                          //return Error2("��������� ����� ���� ����� � ������ ��������� ����������� ���������.");
   return 0;
}

//------------------------------------------------------------------------------

int  ViewAbsendName(void)                                    //����� ���� �������������� � ����� MME.DB, �� ������������� � ��������
{
   PAR_FILE pf;

   if(tabMME == NULL) return 0;                              //������� �����������
   indTabMME = -1;                                           //������ � ������� MME.DB ��� �������� ����� � ����� MEDIA
   for(DWORD n=0; n<numNam; n++)                             //�� ����� ���� � ������� MME
   {   if((tabMME + n)->prYes == 1) continue;                //�������, ��� ��� ������ (1 - ���� � � mme.db � � ��������)
       if(nErr4 == 0)                                        //��������� �� ������ ��� ������
           Error3((Lan+144)->msg, (Lan+145)->msg, (Lan+140)->msg);
       nErr4++;                                              //����� �� ����������� ���������
       indTabMME = n;
       pf.type = 35;                                         //������� ����������� ������  (e3)
       pf.SizeF = 0;
       AddAbsendNameToTree(&pf, 2);                          //���������� ����� ������ � ������
   }
   return 0;
}

#endif

