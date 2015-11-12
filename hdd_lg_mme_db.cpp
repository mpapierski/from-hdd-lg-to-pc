
//                               11.08.2008
#ifndef __HDD_LG_MME_DB__
#define __HDD_LG_MME_DB__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_mme_db ===================================
int  CtrlSize_MME(DWORD Delta);                              //�������� �������� � ������� MME
int  CtrlRecoder(void);                                      //������������� ���������
int  Read_Dir_Part2_MME(void);                               //������ �������� ������� ������� � ����� MME
int  ReadMME_DB(void);                                       //������ ����� ���� �������
int  Update_MME_DB(void);                                    //������������ ������� ������������� ����
     BYTE *MMe;                                              //���������� ����� ����
     DWORD SizeMME;                                          //����� ���� � ����� MME
     TABL_MME *tabMME;                                       //������� ������������ ����
     DWORD numNam;                                           //����� ���� � ������� MME (������ ����� Title ��-�� ������� ������)
     TABL_MME *tabMME_Part;                                  //������� ���� ����������������� Title
     DWORD numNam_Part;                                      //����� ���� � ������� ���� ����������������� Title

static BYTE *endMME;                                         //����� ����� ������� � ������� MME
static BYTE *at_MME;                                         //����� ������ �������� �������� ������ � ������� MME
static int idRec;                                            //������������� ���������
static char Title[128];                                      //��� ����� ����������� � ����� MEDIA
static char nameTV[8];

//------------------------------------------------------------------------------

int CtrlSize_MME(DWORD Delta)                                //�������� �������� � ������� MME
{
   at_MME += Delta;                                          //����� ������ ���������� �������� ������
   if(at_MME > endMME)                                       //����� �� ������� ������� MME
     return Error4((Lan+107)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg);//return Error1("��������� ����� MME.DB ���������� ������ ���������.");
   return 0;
}

//------------------------------------------------------------------------------

static int Ctrl_Recoder(int d, int e)                        //������������� ���������
{
   if(*(MMe + d+0) != 0 || *(MMe + d+1) != 0x41 || *(MMe + d+2) != 0 || *(MMe + d+3) != 0x3A ||
      *(MMe + d+4) != 0 || *(MMe + d+5) != 0x5C || *(MMe + e+0) != 0 || *(MMe + e+1) != 0x2E ||
      *(MMe + e+2) != 0 || *(MMe + e+3) != 0x73 || *(MMe + e+4) != 0 || *(MMe + e+5) != 0x74 ||
      *(MMe + e+6) != 0 || *(MMe + e+7) != 0x72 ||
      *(MMe + d+64) != 0 || *(MMe + d+65) != 0x41 || *(MMe + d+66) != 0 || *(MMe + d+67) != 0x3A ||
      *(MMe + d+68) != 0 || *(MMe + d+69) != 0x5C || *(MMe + e+64) != 0 || *(MMe + e+65) != 0x2E ||
      *(MMe + e+66) != 0 || *(MMe + e+67) != 0x69 || *(MMe + e+68) != 0 || *(MMe + e+69) != 0x64 ||
      *(MMe + e+70) != 0 || *(MMe + e+71) != 0x78 ) return 0;
   return 1;
}

//------------------------------------------------------------------------------

int CtrlRecoder(void)                                        //������������� ���������
{
   if(MMe == NULL) return -1;
   if(Ctrl_Recoder(300, 346) == 1) return 0;                 //������������� ���������
   if(Ctrl_Recoder(308, 358) == 1) return 1;                 //������������� ���������
   if(Ctrl_Recoder(316, 366) == 1) return (2 + Conf.typeRec);//������������� ���������
   if(Ctrl_Recoder(324, 374) == 1) return 4;                 //������������� ���������
   if(Ctrl_Recoder(332, 382) == 1) return 5;                 //������������� ���������
   return -1;
}

//-------------------------------------------------------------------------------

static void Recalc_Par(WCHAR *wTitle, char *name_TV, DATE_T2 *DaT2,
                       WORD *nPart, DATE_T1 *DaT1)           //�������������� ������
{
   char Title0[128];                                         //��� ����� ����������� � ����� MEDIA
   UnicodeToAnsi(wTitle, Title0, 33);                        //������������� ��� Title
   for(int j=0; j<6; j++) *(nameTV + j) = *(name_TV + j);
   for(int j=4; j>0; j--)                                    //�������� ���� ��������� ��������
   {  if(*(nameTV + j) != ' ') break;                        //������ �� ������
      *(nameTV + j) = 0;                                     //������ ������
   }
   SWAP16(nPart);                                            //����� ������ � ������ Title
   SWAP16(&DaT2->Year);
   DaT2->Year = WORD(DaT2->Year - (DaT2->Year / 100) * 100); //�������� ��� �����
   SWAP16(&DaT1->year);
   SWAP16(&DaT1->mon);
   SWAP16(&DaT1->day);
   SWAP16(&DaT1->hour);
   wsprintf(Title, "%02d%02d%02d%02d%02d %s", DaT2->Year, DaT2->Mon, DaT2->Day, DaT2->Hour, DaT2->Min, Title0);
// wsprintf(cData, "%02d%02d%02d%02d%02d%02d %s", DaT2->Year, DaT2->Mon, DaT2->Day, DaT2->Hour, DaT2->Min, DaT2->Sec, Title0);
}

//-------------------------------------------------------------------------------

char *Qu[6][5] = { { "HQ", "SQ", "LQ", "--", "--" },         //�������� 0
                   { "EQ", "LQ", "SQ", "HQ", "--" },         //�������� 1
                   { "EP", "LP", "SP", "XP", "--" },         //�������� 2
                   { "MLP", "EP", "LP", "SP", "XP" },        //�������� 3
                   { "MLP", "EP", "LP", "SP", "XP" },        //�������� 4
                   { "MLP", "EP", "LP", "SP", "XP" },        //�������� 5
                 };
#pragma argsused
static int MakeNewName(DWORD t, int n, WORD nPart, BYTE Q, BYTE nAV, DATE_T1 *DaT1)//�������� ����� � ��������� ��� � ������� ���� �� ����� MME
{
   char nameIn[8];
   ONE_NAME *aNam = (ONE_NAME *)at_MME;                      //����� ������ ������� �� ����� ����� ������� Title
   if(CtrlSize_MME(sizeof(ONE_NAME)) < 0) return -1;         //�������� �������� � ������� MME
//??SWAP64(&DWORDLONG(aNam->sizeF));
   SWAP64((DWORDLONG*)&aNam->sizeF);
   (tabMME+numNam)->SizeF = aNam->sizeF;                     //������� ������ �����
   SWAP32(&aNam->timeLong);
   (tabMME+numNam)->timeLong = aNam->timeLong;               //������� ������������ ������� � �����
   UnicodeToAnsi(aNam->Name1+3, (tabMME+numNam)->NameF, 27); //������� ������������� ��� ����� � ����� MEDIA (������ ������� A:\)
   *strchr((tabMME+numNam)->NameF, '.') = 0;                 //�������� ����������
   if(Q > 4) Q = 4;
   if(nAV == 119) sprintf(nameIn, "DVD");                    //����������� ��������� �������
   else  if(nAV == 0) sprintf(nameIn, "%s", nameTV);
         else sprintf(nameIn, "AV%1d", nAV);
   lstrcpy((tabMME+numNam)->aName, Title);                   //�������������� ��� �����
   lstrcpy((tabMME+numNam)->Qual, Qu[idRec][Q]);
   lstrcpy((tabMME+numNam)->nameIn, nameIn);
   (tabMME+numNam)->numPart = 0;                             //����� �����
   (tabMME+numNam)->dt = *DaT1;                              //���� � ����� ������ ������ �����
   (tabMME+numNam)->prYes = 0;                               //�������, ��� ��� ������ (1- ���� � � mme.db � � ��������)
   (tabMME+numNam)->indFolder = 0xFFFF;
   (tabMME+numNam)->nPart = nPart;
   (tabMME+numNam)->indTitle = WORD(t+1);                    //���������� ����� Title
   if(nPart > 1)                                             //���� ������ ����� �����
   { (tabMME+numNam)->numPart = WORD(n + 1);                 //����� �����
     (tabMME+numNam)->indFolder = WORD(numNam_Part);         //������ ������ � ������� ���� ����� ������������� Title
     if(n == 0)                                              //������ ����� �������������� Title
     { *(tabMME_Part+numNam_Part) = *(tabMME+numNam);        //������� ���� ����������������� Title
       numNam_Part++;                                        //����� ���� � ������� ���� ����������������� Title
     }
     else
     { (tabMME_Part+numNam_Part-1)->SizeF += aNam->sizeF;    //��������� ������
       (tabMME_Part+numNam_Part-1)->timeLong += aNam->timeLong;//��������� ������������ ������� � �����
     }
   }
   if(++numNam >= MAX_NAME_MME)
      return Error1((Lan+106)->msg);                         //return Error1("����� ������� � ����� MME ��������� ����������� ���������.");
   return 0;
}

//-------------------------------------------------------------------------------

static int Make_Tab_Name_MME0(DWORD NumT)                    //�������� ������� ���� �� ����� MME
{
   for(DWORD t=0; t<NumT; t++)                               //�� ����� ����� MME
   {  PSP_TIT0 *aTit = (PSP_TIT0 *)at_MME;                   //����� ������ ��������� ��������� ������
      if(CtrlSize_MME(sizeof(PSP_TIT0)) < 0) return -1;      //�������� �������� � ������� MME
      Recalc_Par(aTit->Title, aTit->nameTV, &aTit->DaT2, &aTit->nPart, &aTit->DaT1);
      for(int n=0; n<aTit->nPart; n++)                       //�� ����� ������ � ����� Title
         if(MakeNewName(t, n, aTit->nPart, aTit->Q, aTit->nAV, &aTit->DaT1) < 0) return -1; //�������� ����� � ��������� ��� � ������� ���� �� ����� MME
//??  END_REC *eRec = (END_REC *)atMME;
//??  atMME += sizeof(END_REC);
//??  if(eRec->nM == 0)  continue;
//??  SWAP16(&eRec->nM);
//??  atMME += eRec->nM * sizeof(ONE_MET);                   //���������� ��� ������ � ������
   }
   return 0;
}

//-------------------------------------------------------------------------------

static int Make_Tab_Name_MME1(DWORD NumT)                    //�������� ������� ���� �� ����� MME
{
   for(DWORD t=0; t<NumT; t++)                               //�� ����� ����� MME
   {  PSP_TIT1 *aTit = (PSP_TIT1 *)at_MME;                   //����� ������ ��������� ��������� ������
      if(CtrlSize_MME(sizeof(PSP_TIT1)) < 0) return -1;      //�������� �������� � ������� MME
      Recalc_Par(aTit->Title, aTit->nameTV, &aTit->DaT2, &aTit->nPart, &aTit->DaT1);
      for(int n=0; n<aTit->nPart; n++)                       //�� ����� ������
         if(MakeNewName(t, n, aTit->nPart, aTit->Q, aTit->nAV, &aTit->DaT1) < 0) return -1; //�������� ����� � ��������� ��� � ������� ���� �� ����� MME
      END_REC *eRec = (END_REC *)at_MME;
      if(CtrlSize_MME(sizeof(END_REC)) < 0) return -1;       //�������� �������� � ������� MME
      if(eRec->numMet == 0)  continue;                       //����� ���
      SWAP16(&eRec->numMet);
      if(CtrlSize_MME(eRec->numMet * sizeof(ONE_MET)) < 0) return -1;//���������� ��� ������ � ������
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Make_Tab_Name_MME2(DWORD NumT)                    //�������� ������� ���� �� ����� MME
{
   for(DWORD t=0; t<NumT; t++)                               //�� ����� ����� MME
   {  PSP_TIT2 *aTit = (PSP_TIT2 *)at_MME;                   //����� ������ ��������� ��������� ������
      if(CtrlSize_MME(sizeof(PSP_TIT2)) < 0) return -1;      //�������� �������� � ������� MME
      Recalc_Par(aTit->Title, aTit->nameTV, &aTit->DaT2, &aTit->nPart, &aTit->DaT1);
      for(int n=0; n<aTit->nPart; n++)                       //�� ����� ������ � ����� Title
         if(MakeNewName(t, n, aTit->nPart, aTit->Q, aTit->nAV, &aTit->DaT1) < 0) return -1; //�������� ����� � ��������� ��� � ������� ���� �� ����� MME
      END_REC *eRec = (END_REC *)at_MME;
      if(CtrlSize_MME(sizeof(END_REC)) < 0) return -1;       //�������� �������� � ������� MME
      if(eRec->numMet != 0)
      {  SWAP16(&eRec->numMet);
         if(CtrlSize_MME(eRec->numMet * sizeof(ONE_MET)) < 0) return -1;//���������� ��� ������ � ������
      }
      END0_ZAP_MME *aEnd0 = (END0_ZAP_MME *)at_MME;          //����� ������ ��������� ��������� ������
      if(CtrlSize_MME(sizeof(END0_ZAP_MME)) < 0) return -1;  //�������� �������� � ������� MME
      SWAP16(&aEnd0->NN);
      if(CtrlSize_MME(aEnd0->NN * sizeof(END1_ZAP_MME)) < 0) return -1;//���������� ��� ������ �� ���� � ���
   }
   return 0;
}

//-------------------------------------------------------------------------------

static int  Make_Tab_Name_MME4(DWORD NumT)                   //�������� ������� ���� �� ����� MME
{
   for(DWORD t=0; t<NumT; t++)                               //�� ����� ����� MME
   {  PSP_TIT4 *aTit = (PSP_TIT4 *)at_MME;                   //����� ������ ��������� ��������� ������
      if(CtrlSize_MME(sizeof(PSP_TIT4)) < 0) return -1;      //�������� �������� � ������� MME
      Recalc_Par(aTit->Title, aTit->nameTV, &aTit->DaT2, &aTit->nPart, &aTit->DaT1);
      for(int n=0; n<aTit->nPart; n++)                       //�� ����� ������ � ����� Title
         if(MakeNewName(t, n, aTit->nPart, aTit->Q, aTit->nAV, &aTit->DaT1) < 0) return -1; //�������� ����� � ��������� ��� � ������� ���� �� ����� MME
      END_REC *eRec = (END_REC *)at_MME;
      if(CtrlSize_MME(sizeof(END_REC)) < 0) return -1;       //�������� �������� � ������� MME
      if(eRec->numMet != 0)
      {  SWAP16(&eRec->numMet);
         if(CtrlSize_MME(eRec->numMet * sizeof(ONE_MET)) < 0) return -1;//���������� ��� ������ � ������
      }
      END0_ZAP_MME *aEnd0 = (END0_ZAP_MME *)at_MME;          //����� ������ ��������� ��������� ������
      if(CtrlSize_MME(sizeof(END0_ZAP_MME)) < 0) return -1;  //�������� �������� � ������� MME
      SWAP16(&aEnd0->NN);
      if(CtrlSize_MME(aEnd0->NN * sizeof(END1_ZAP_MME)) < 0) return -1;//���������� ��� ������ �� ���� � ���
   }
   return 0;
}

//-------------------------------------------------------------------------------

static int Make_Tab_Name_MME5(DWORD NumT)                    //�������� ������� ���� �� ����� MME
{
   for(DWORD t=0; t<NumT; t++)                               //�� ����� ����� MME
   {  PSP_TIT5 *aTit = (PSP_TIT5 *)at_MME;                   //����� ������ ��������� ��������� ������
      if(CtrlSize_MME(sizeof(PSP_TIT5)) < 0) return -1;      //�������� �������� � ������� MME
      Recalc_Par(aTit->Title, aTit->nameTV, &aTit->DaT2, &aTit->nPart, &aTit->DaT1);
      for(int n=0; n<aTit->nPart; n++)                       //�� ����� ������ � ����� Title
         if(MakeNewName(t, n, aTit->nPart, aTit->Q, aTit->nAV, &aTit->DaT1) < 0) return -1; //�������� ����� � ��������� ��� � ������� ���� �� ����� MME
      END_REC *eRec = (END_REC *)at_MME;
      if(CtrlSize_MME(sizeof(END_REC)) < 0) return -1;       //�������� �������� � ������� MME
      if(eRec->numMet != 0)
      {  SWAP16(&eRec->numMet);
         if(CtrlSize_MME(eRec->numMet * sizeof(ONE_MET)) < 0) return -1;//���������� ��� ������ � ������
      }
      END0_ZAP_MME *aEnd0 = (END0_ZAP_MME *)at_MME;          //����� ������ ��������� ��������� ������
      if(CtrlSize_MME(sizeof(END0_ZAP_MME)) < 0) return -1;  //�������� �������� � ������� MME
      SWAP16(&aEnd0->NN);
      if(CtrlSize_MME(aEnd0->NN * sizeof(END1_ZAP_MME)) < 0) return -1;//���������� ��� ������ �� ���� � ���
   }
   return 0;
}

//-------------------------------------------------------------------------------

static int Make_Tab_Name_MME(void)                           //�������� ������� ���� �� ����� MME
{
   tabMME = (TABL_MME *)MyAllocMem((MAX_NAME_MME+256) * sizeof(TABL_MME));//������� ������������ ����
   if(tabMME == NULL)  return -1;
   tabMME_Part = tabMME+MAX_NAME_MME;                        //������� ���� ����������������� Title
   numNam_Part = 0;                                          //����� ���� � ������� ���� ����������������� Title
   at_MME = MMe;
   PSP_MME *pspMME = (PSP_MME *)at_MME;                      //����� ������ �������� ����� MME
   if(CtrlSize_MME(sizeof(PSP_MME)) < 0) return -1;          //�������� �������� � ������� MME
   SWAP32(&pspMME->NumT);                                    //����� Title � ����� MME
   if(pspMME->NumT == 0) return 0;                           //��� �� ����� ������ � ����
   idRec = CtrlRecoder();                                    //������������� ���������
   if(idRec < 0)
      return Error4((Lan+107)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg); //return Error1("��� ��������� ���������.");
   switch(idRec)
   { case 0: if(Make_Tab_Name_MME0(pspMME->NumT) < 0) return -1; break;
     case 1: if(Make_Tab_Name_MME1(pspMME->NumT) < 0) return -1; break;
     case 2:
     case 3: if(Make_Tab_Name_MME2(pspMME->NumT) < 0) return -1; break;
     case 4: if(Make_Tab_Name_MME4(pspMME->NumT) < 0) return -1; break;
     case 5: if(Make_Tab_Name_MME5(pspMME->NumT) < 0) return -1; break;
   }
   if(at_MME != endMME)
      return Error4((Lan+107)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg);//return Error1("��������� ����� MME.DB ���������� ������ ���������.");
   return 0;
}

//------------------------------------------------------------------------------

int Read_File_MME(DWORD ClSt)                                //������ ����� ����
{
   DWORD numCl = (SizeMME + sCl2_B - 1) / sCl2_B;            //����� ��������� ����������� ��� ���������� ����� ������� �������
   DWORD sizeF = numCl * sCl2_B;                             //����� ��� ������ �������
   MMe = (BYTE *)MyAllocMem(sizeF);                          //������ ���� ����
   if(MMe == NULL)  return -1;
   endMME = MMe + SizeMME;                                   //����� ����� ������� � ������� MME
   BYTE *aMME = MMe;
   DWORD wSizeB = sCl2_B;                                    //����� ������������ ���� ����� ����� ���� � ��������
   DWORD nCl = ClSt;                                         //������� ����� �������� ����� ������� �������� �����
   DWORD SizeToEnd = SizeMME;                                //����� ���� ������� �������� ��������
   for(DWORD i=0; i<numCl; i++)                              //�� ����� ���������
   {  if(nCl == 0x0FFFFFFF)
        return Error1((Lan+12)->msg);                        //return Error1("���������� ������ ������� ����� ������� FAT.");
      if(*(FAT2 + nCl) == 0)
        return Error1((Lan+11)->msg);                        //return Error1("���������� �������������� �������� FAT � ������ �� ������� �����.");
      if(wSizeB > SizeToEnd) wSizeB = DWORD(SizeToEnd);      //������ ������� ������ ������ ������� ��������
      DWORD nSector = Start_SecDir2 + (nCl - 1) * sClSec2;   //������ ������ �������� ��������
#if defined OUT_TEST
      char Ss[300];
      wsprintf(Ss, "Load Dir (claster2 %d, sector %d)", nCl, nSector);
      Add_SpecSpis(Ss);
#endif
      if(ReadClast2_P(nSector, aMME) < 0) return -1;         //������ ��������
#if defined OUT_TEST
      View_HEX_Any(aMME, sCl2_B);
#endif
      aMME += sCl2_B;
      SizeToEnd -= wSizeB;                                   //����� ���� ������� �������� ��������
      nCl = *(FAT2 + nCl);                                   //����� ���������� ��������
      if(nCl > maxZapFAT2 && nCl != 0x0FFFFFFF)
        return Error1((Lan+13)->msg);                        //"����� �������� ��������� ���������� ��������."
      if(SizeToEnd <= 0 && nCl != 0x0FFFFFFF)
        return Error1((Lan+29)->msg);                        //return Error2("�������� ���� ��������� ������� �������,", "� ������� ����� ������� FAT �� ������.");
   }
   return 0;
}

//------------------------------------------------------------------------------

int Read_Dir_Part2_MME(void)                                 //������ �������� ������� ������� � ����� MME
{
   BYTE buff[sCl2_B];

   SizeMME = 0;                                              //������� ����� ����� (��� ������� ����������)
   tabMME = NULL;                                            //������� ������������ ����
   numNam = 0;                                               //����� ���� � ������� MME
#if defined OUT_TEST
   DWORD nClast = 1;
   DWORD nSector = Start_SecDir2 + (nClast-1) * sClSec;      //����� ������� �� ������ ��������
   char Ss[300];
   wsprintf(Ss, "Load Dir (claster2 %d, sector %d)", nClast, nSector);
   Add_SpecSpis(Ss);
#endif
   if(ReadClast2_P(Start_SecDir2, buff) < 0) return -1;      //������ ��������
#if defined OUT_TEST
   View_HEX_Any(buff, sCl2_B);
#endif
   One_Str_Cat *Kat = (One_Str_Cat *)buff;
   for(DWORD n=0; n<4 * sClSec2; n++)                          //� ������ ������� 4 ������
   {  if((Kat + n)->pf.type == 0) return 0;                  //����� �������� � ���� �� ������
//      if((Kat + n)->pf.type == 0xFFFF) continue;             //����� �������� � ���� �� ������
//    if(n == 0) return 0;                                   //��� ������� � ����� MEDIA
//    else return Error4((Lan+15)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg); //"����������� ��� ������ � ������ ��������."
      WORD NameDel = *((WORD*)&(Kat + n)->Name);
      if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0) continue; //��� ��������� ���
      if((Kat + n)->pf.type != 32 && (Kat + n)->pf.type != 48)
        return Error4((Lan+15)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg); //"����������� ��� ������ � ������ ��������."
      int prRec = 0;
      if(*(Kat->Name+14) == 0x2000) prRec = 1;               //��������� ����� (� ��� ������� � �����)
      char nam[128], ext[128];
      if(prRec == 0)                                         //������� ��������� 0 ��� 1
      {  UnicodeToAnsi(Kat->Name, nam, 41);                  //������������� ���
         UnicodeToAnsi(Kat->Ext, ext, 4);                    //������������� ����������
      }
      if(prRec == 1)                                         //������� ��������� 0 ��� 1 (��������� �����)
      {  UnicodeToAnsi(Kat->Name, nam, 45);                  //������������� ���
         lstrcpy(ext, nam + 42);
         if(*(ext+2) == ' ') *(ext+2) = 0;                   //� ������ �������� ����� ���� ������ ���������� DB (2 �������)
         for(int i=41; i>0; i--)
         {  if(*(nam + i) != ' ') break;
            *(nam + i) = 0;
         }
      }
      if(lstrcmp(nam, "MME") != 0) continue;                 //��� �� ����
//� ����� ����� "BACKUP" ���
//    if(lstrcmp(nam, "BACKUP") != 0) continue;              //��� �� ����
      if(lstrcmp(ext, "DB") != 0) continue;                  //��� �� ����
      SizeMME = DWORD((Kat + n)->pf.SizeF);                  //����� ���� � ����� MME
      if(SizeMME == 0) return 0;                             //������� ����� �����
      return Read_File_MME((Kat + n)->pf.ClSt);              //������ ����� ����
   }
   return 0;                                                 //���� �� ������
}

//------------------------------------------------------------------------------

#if defined VIEW_TAB_MME
static void View_Tab_MME_DB(void)                            //������������ �������
{
   char Ss[1024];
   for(DWORD i=0; i<numNam; i++)
   {  int tlHour = BYTE((tabMME + i)->timeLong / 3600);
      int M1 = (tabMME + i)->timeLong - tlHour * 3600;
      int tlMin = BYTE(M1 / 60);
      int tlSec = BYTE(M1 - tlMin * 60);
      sprintf(Ss, "%4d. %-024s (n=%5d) %-040s %-08s %-08s %12.0lf [%02d:%02d:%02d] R %02d-%02d-%04d %02d:%02d:%02d",
                  i+1, (tabMME + i)->NameF, (tabMME + i)->numPart, (tabMME + i)->aName,
                  (tabMME + i)->Qual, (tabMME + i)->nameIn, double((tabMME + i)->SizeF),
                  tlHour, tlMin, tlSec,
                  (tabMME + i)->dt.day, (tabMME + i)->dt.mon, (tabMME + i)->dt.year,
                  (tabMME + i)->dt.hour, (tabMME + i)->dt.min, (tabMME + i)->dt.sec);
      Add_Spis(Ss);                                          //���������� ������ � ������
   }
}
#endif

//------------------------------------------------------------------------------

int ReadMME_DB(void)                                         //������ ����� ���� �������
{
   if(Conf.altName == 1) return 0;                           //�� ���������� ������������� �����
   if(Read_Dir_Part2_MME() < 0) return -1;                   //������ �������� ������� ������� � ����� MME
   if(SizeMME == 0) return 0;                                //������� ����� �����
#if !defined OUT_TEST1 && !defined TEST_LG2_MME
   int ret = Make_Tab_Name_MME();                            //�������� ������� ���� �� ����� MME
   MyFreeMem(&(void*)MMe);
   if(ret < 0)
   {  MyFreeMem(&(void*)tabMME);
      return -1;
   }
  #if defined VIEW_TAB_MME
   View_Tab_MME_DB();                                        //������������ �������
  #endif
#endif
   return 0;
}

//------------------------------------------------------------------------------

int Update_MME_DB(void)                                      //������������ ������� ������������� ����
{
   MyFreeMem(&(void*)MMe);
   MyFreeMem(&(void*)tabMME);                                //������� ������������ ����
   return ReadMME_DB();                                      //������ ����� ���� �������
}

#endif

