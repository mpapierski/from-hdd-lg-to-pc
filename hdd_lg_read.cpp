
//                              03.11.2009
#ifndef __HDD_LG_READ__
#define __HDD_LG_READ__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_read =====================================
int  Update_Tree(void);                                      //������������ ������ ������ � ����� ��� ��������� ��������
int  Read_HDD(void);                                         //����� HDD �� ��������� LG � �������� ��������� ����������
     HANDLE hDrive;                                          //��������� �� ����
     DWORD *FAT1;                                            //������� FAT ������� �������
     DWORD Start_SecDir1;                                    //������� ������ ������� �������� ������� �������
     DWORD Start_SecFAT1;                                    //������� ������ ������� FAT ������� �������
     DWORD num_SecFAT1;                                      //����� �������� � FAT
     DWORD Size_FAT1;                                        //����� ���� � FAT
     DWORD maxZapFAT1 = 0xFFFFFFFF;                          //������������ ����� ������� � FAT
     DWORD *FAT2;                                            //������� FAT ������� �������
     DWORD Start_SecDir2;                                    //����� ������� ������ �������� �� ������ �������
     DWORD Start_SecFAT2;                                    //������� ������ ������� FAT ������� �������
     DWORD num_SecFAT2;                                      //����� �������� � FAT
     DWORD Size_FAT2;                                        //����� ���� � FAT ������� �������
     DWORD maxZapFAT2;                                       //������������ ����� ������� � FAT ������� �������
     int   indTabMME;                                        //������ � ������� MME.DB ��� �������� ����� � ����� MEDIA
     DWORD writeCl;                                          //����� ������� ���������
     int prMEDIA;                                            //������� �������������� �����
     int pr_tRec;                                            //������� ��������� 0 ��� 1 (��������� �����)

static int Read_One_Dir(DWORD nClast, int pr);               //������ � ������������ ������ ��������
typedef struct { DWORD nCl; char  nam[48]; } FOLDER_INFO;
#define MAX_FOLDER 4                                         //������������ ����� ��������� ����� ��� ��������� ���� 1
static FOLDER_INFO F_Inf[MAX_FOLDER];                        //��������� ���� ��������� ����� ��� ��������� ���� 1
static int numFolder;                                        //����� ��������� ����� ��� ��������� ���� 1
static int nCl_3Cat;                                         //����� �������� � ����� �������� ����������

//------------------------------------------------------------------------------

static int CreateName(char *nam, char *ext, One_Str_Cat *Kat)//������������ ����� �����
{
   if(pr_tRec == 0)                                          //������� ��������� 0 ��� 1 (��������� �����)
   {  UnicodeToAnsi(Kat->Name, nam, 41);                     //������������� ��� �� UNICODE
      UnicodeToAnsi(Kat->Ext, ext, 4);                       //������������� ���������� �� UNICODE
   }
   if(pr_tRec == 1)                                          //������� ��������� 0 ��� 1 (��������� �����)
   {  UnicodeToAnsi(Kat->Name, nam, 45);                     //������������� ��� � ���������� �� UNICODE
      lstrcpy(ext, nam + 42);                                //������� ����������
      if(*ext == ' ') *ext = 0;                              //���� ���������� ���, �� ��������
      for(int i=41; i>0; i--)                                //�������� ����� ����� �� �������� ������ ������ �������
      {  if(*(nam + i) != ' ') break;                        //��������� ����� ����� ����� ���������� �������
         *(nam + i) = 0;
      }
   }
   if(nCl_3Cat == -1 &&  prMEDIA == 1 && Kat->pf.type == 48 &&
      lstrcmp(nam, "MEDIA") == 0 && *ext == 0)
   {  nCl_3Cat = Kat->pf.ClSt;                               //����� �������� � ����� �������� ����������
      return 1;                                              //���������� ����� � ��������� MEDIA
   }
/*
   if(prM == 2 && n == 2 && lstrcmp(nam, "JUKEBOX") != 0)
     return Error2((Lan+96)->msg, (Lan+100)->msg);           //return Error2("�������� ��������� ��������.", "�� ������� ����� MEDIA.");
   if(prM == 2 && n == 3 && lstrcmp(nam, "VIDEO") != 0)
     return Error2((Lan+96)->msg, (Lan+101)->msg);           //return Error2("�������� ��������� ��������.", "�� ������� ����� MEDIA.");
   if(prM == 2 && n == 4 && lstrcmp(nam, "PHOTO") != 0)
       return Error2((Lan+96)->msg, (Lan+102)->msg);         //return Error2("�������� ��������� ��������.", "�� ������� ����� MEDIA.");
*/
   if(Kat->pf.type == 48)                                    //��� ��� �����
      if(Kat->pf.SizeF != 0)
         return Error1((Lan+16)->msg);                       //return Error1("������ �������� ���������� ��� ����� ����� ����������� � ������.");
   return 0;
}

//------------------------------------------------------------------------------

static const DWORD Point1[] = { 0x00002E00, 0x20002E00 };    //��� ����� (� ��������� ����� ������ ���� - �������)

static int View_Dir(BYTE *buff, int prM)                     //����� ����������� ��������
{
   One_Str_Cat *Kat = (One_Str_Cat *)buff;
   int prPoint = 0;                                          //������� ������� ��������� � ������ ��� ������� ������������ ���������
   prMEDIA = prM;                                            //������� �������������� �����
   if(prM == 0) prMEDIA = 1;                                 //��������� ��� ������� �������� ����� MEDIA
   for(DWORD n=0; n<4*sClSec; n++)                             //� ������ ������� 4 ������
   {  if((Kat + n)->pf.type == 0) return 1;                  //����� ��������
      if((Kat + n)->pf.type != 32 && (Kat + n)->pf.type != 48)
        return Error1((Lan+15)->msg);                        //return Error1("����������� ��� ������ � ������ ��������.");
      DWORD NamePoint = *((DWORD*)&(Kat + n)->Name);
      if(NamePoint == *(Point1 + pr_tRec))                   //��� ���� ����� �.�. ������� �������� ��������
      { if(n == 0) prPoint++; continue; }
      if(NamePoint == 0x2E002E00)                            //��� ��� ����� �.�. ������� ����������� ��������
      { if(n == 1) prPoint++; continue; }
      if(prM > 1 && prPoint != 2)                            //��� �������� ����� MEDIA ������ ���������� � ����� � ���� �����
        return Error1((Lan+96)->msg);                        //return Error1("�������� ��������� ��������.");
      WORD NameDel = *((WORD*)&(Kat + n)->Name);
#if !defined VIEW_DEL                                        //����� ��������� ������ � �����
      if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0) continue; //��� ��������� ���
#else
//      *((WORD*)&(Kat + n)->Name) = 0xE600;
#endif
      writeCl += DWORD(((Kat + n)->pf.type == 48) ? 1 : ((Kat + n)->pf.SizeF + sCl_B - 1) / sCl_B);
      char nam[128], ext[128];
      int ret = CreateName(nam, ext, Kat + n);               //������������ ����� �����
      if(ret == 1) continue;                                 //����� ��� ��� �� ����������  //���������� ����� � ��������� MEDIA
      if(ret < 0) return -1;
      int lPr;                                               //������� ������ ������
      if(prM <= 1)                                           //������� ������ ������� �������� (MEDIA ��� JUKEBOX)
      {  if(pr_tRec == 0)                                    //������� ��������� 0 ��� 1 (��������� �����)
         {  //??if(n == 0) lPr = 1;                              //������� ������� ������ MEDIA (��� MEDIA �������� � ������ ������)
            //??else       lPr = 2;                              //��������� ����� � ����� MEDIA
            //??if(prM == 0) lPr = 2;                            //��� ������� �������� ����� MEDIA ������ ������ �������
            lPr = 2;                                         //��������� ����� � ����� MEDIA
         }
         if(pr_tRec == 1)                                    //������� ��������� 0 ��� 1 (��������� �����)
         {  if((Kat + n)->pf.type == 48)                     //������� ������� ������ � ����� ������ ��������
            {  (F_Inf + numFolder)->nCl = (Kat + n)->pf.ClSt;//��������� ���������� �� ��������� �����
               lstrcpy((F_Inf + numFolder)->nam, nam);
               if(numFolder+1 >= MAX_FOLDER)                 //����� ��������� �����
               {  Error1((Lan+99)->msg);                     //Error1("������������ ����� ����� �������� ������.");
                  continue;
               }
               numFolder++;                                  //����� ��������� �����
               continue;
            }
            else  lPr = 2;                                   //��������� ����� � ����� MEDIA
         }
      }
      else                                                   //������ � ����������� ������� � ����������
        if(prM == 2) lPr = 1;                                //��������� ��� ������� ��������
        else lPr = prM - 1;                                  //������� ������ ����������� ����� � ������
      if(AddItemToTree(nam, ext, &((Kat + n)->pf), lPr) < 0) return -1; //�������� ������ � ������
      if(prM > 1)                                            //��� �� ������� MEDIA � ������ ����� ���� ��������� �����
        if((Kat + n)->pf.type == 48)                         //��� ��� ��������� �����
           if(Read_One_Dir((Kat + n)->pf.ClSt, prM+1) < 0) return -1;  //������ � ������������ ����� ��������� �����
//??  if((Kat + n)->pf.Type == 32)                           //��� ��� �����
//??      Ctrl_FAT_For_File((Kat + n)->pf.ClSt, (Kat + n)->pf.SizeF); //�������� ������������ ����� ����� � FAT
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir_(BYTE *buff, DWORD nClast, int pr)   //������ � ������������ ������ ��������
{
   DWORD nSector = Start_SecDir1 + (nClast - 1) * sClSec;    //����� ������� �� ������ ��������
   if(ReadClast1_P(nSector, buff) < 0) return -1;            //������ ��������
#if defined OUT_TEST
   View_HEX_Any(buff, sCl_B);
#endif
   if(pr == 1)                                               //��� ����� ������ ������� MEDIA
   {  if(*(DWORDLONG*)buff      == 0 && *(DWORDLONG*)(buff+ 8) == 0 &&//�������� ������� �����
         *(DWORDLONG*)(buff+16) == 0 && *(DWORDLONG*)(buff+24) == 0 &&//� ���� ����� ���, �� ������
         *(DWORDLONG*)(buff+32) == 0 && *(DWORDLONG*)(buff+40) == 0)
        return Error1((Lan+101)->msg);                       //return Error1("�� ������� ����� �� ���������� �� ����� ������.");
      pr_tRec = 0;                                           //������� ��������� 0 ��� 1 (��������� �����)
      DWORDLONG *dwBuf = (DWORDLONG*)buff;
      for(DWORD n=0; n<4*sClSec; n++)                          //� ������ ������� 4 ������ (�������� ����� ��������)
	  {  if(*dwBuf == 0x4900440045004D00i64) break;             //������� ��������� 0 ��� 1      //MEDIA
		 if(*dwBuf == 0x45004B0055004A00i64) { pr_tRec = 1; break; }//������� ��������� 0 ��� 1 //JKEBOX
         if(*dwBuf == 0x0000000000000000i64) break;             //������� ����� �������
         dwBuf += 128 / sizeof(DWORDLONG);
      }
      if(pr_tRec == 1)
      {  if(Conf.WriteYes == 1)                              //��� ������ ����� ������� ������
         {  EnableWindow(hWriteFi, false);
            EnableWindow(hWriteFo, false);
            EnableWindow(hNew_Fo, false);
            EnableWindow(hRenFi, false);
            EnableWindow(hDelFi, false);
            EnableWindow(hCorr67, false);
         }
         Conf.WriteYes = 0;                                  //��� ������ ����� ������� ������
      }
      PAR_FILE pf;
      ZeroMemory(&pf, sizeof(PAR_FILE));
      pf.type = 48;                                          //�������, ��� ��� �����
      AddItemToTree("MEDIA", NULL, &pf, 1);                  //�������� ������ � ������ (����� ������ �������� ������)
   }
   if(View_Dir(buff, pr) < 0)                                //����� ����������� ��������
      Error2((Lan+97)->msg, (Lan+98)->msg);                  //Error2("������� ������ � ��������� ������� �� ����� LG.". "������ ����� � ��������� �������� �������� ����������.");
   if(nClast != 1 && *(FAT1 + nClast) != 0x0FFFFFFF)
   {  Error1((Lan+17)->msg);                                 //Error1("������ ������ �������� ��������� ������ ��������.");
      Error2((Lan+97)->msg, (Lan+98)->msg);                  //Error2("������� ������ � ��������� ������� �� ����� LG.". "������ ����� � ��������� �������� �������� ����������.");
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir(DWORD nClast, int prM)               //������ � ������������ ������ ��������
{
#if defined OUT_TEST
   DWORD nSector = Start_SecDir1 + (nClast-1) * sClSec;      //����� ������� �� ������ ��������
   char Ss[300];
   wsprintf(Ss, "Load Dir (claster %d, sector %d)", nClast, nSector);
   Add_SpecSpis(Ss);
#endif
   if(nClast == 0 || nClast >= maxZapFAT1)
      return Error1((Lan+100)->msg);                         //return Error1("������������ ����� �������� ��������.");
   if(nClast != 1 && *(FAT1 + nClast) == 0)
      return Error1((Lan+18)->msg);                          //return Error1("���������� �������������� �������� FAT � ������ �� ������� ��������.");
   BYTE *buff = (BYTE *)MyAllocMem(sCl_B);                   //������ ��� ����� ��������� � ��������
   if(buff == NULL)  return -1;
   int ret = Read_One_Dir_(buff, nClast, prM);               //������ � ������������ ������ ��������
   MyFreeMem(&(void*)buff);
   return ret;
}

//------------------------------------------------------------------------------

static int Read_AllDir(void)                                 //������ ���� ���������
{
   nCl_3Cat = -1;                                            //����� �������� � ����� �������� ����������
   numEl_Tree = 0;                                           //����� ���������� � ������ ����
   num_Sel = 0;                                              //����� ��������� ���� ��� ��������� ������
   size_Sel = 0;                                             //��������� ������ ��������� ������
   for(int i=0; i<MAX_U; i++)
     hPrev[i] = NULL;                                        //������ ������� ����������� ������
   PAR_FILE pf;
   ZeroMemory(&pf, sizeof(PAR_FILE));
   pf.type = 48;                                             //�������, ��� ��� �����
   AddItemToTree("HDD_LG", NULL, &pf, 0);                    //�������� ������ � ������ (����� ������ �������� ������)
   numFolder = 0;                                            //����� ��������� �����
   if(Read_One_Dir(1, 1) < 0) return -1;                     //������ � ������������ ������ ����� MEDIA
   if(stCl_Media2 != 0)                                      //������� ������ ������ ����� ����� MEDIA
     if(Read_One_Dir(stCl_Media2, 0) < 0) return -1;         //������ � ������������ ������ ����� ����� MEDIA
#if !defined OUT_TEST
   ViewAbsendName();                                         //����� ���� �������������� � ����� MME.DB, �� ������������� � ��������
#endif
   if(pr_tRec == 0 && nCl_3Cat != -1)                        //���� ����� MEDIA
     if(Read_One_Dir(nCl_3Cat, 2) < 0) return -1;            //������ � ������������ ������ �������� (JUKEBOX, VIDEO � FOTO)
   if(pr_tRec == 1 && numFolder > 0)                         //C�������� ����� ����������
     for(int i=0; i<numFolder; i++)
     {  AddItemToTree((F_Inf+i)->nam, NULL, &pf, 1);         //�������� ������ � ������ (����� ������ �������� ������)
        if(Read_One_Dir((F_Inf+i)->nCl, 2) < 0) return -1;   //������ � ������������ ������ �������� (JUKEBOX, VIDEO � FOTO)
     }
   Expand_Tree(0);                                           //���������� ������ �� ������ (�������) ������
   return 0;
}

//-------------------------------------------------------------------------------

static int My_Read_HDD(void)                                 //����� HDD �� ��������� LG � �������� ��������� ����������
{
#if defined EMULATOR_HDD
   if(FindHDD_LG_Emul() < 0) return -1;                      //����� ����� �� ��������� LG ����������
#else
   if(FindHDD_LG() < 0) return -1;                           //����� ����� �� ��������� LG (���������)
#endif
   if(Load_FAT1() < 0) return -1;                            //�������� FAT ������� �������
   if(Load_FAT2() >= 0)                                      //�������� FAT ������� �������
      ReadMME_DB();                                          //������ ����� ���� ������� (������ 3,0���� ������ FAT32)
   if(Read_AllDir() < 0) return -1;                          //������ ���� ���������
   if(Conf.altName == 0 && Conf.ViewPart == 0 && tabMME != NULL)//����� �������������� �����, ����� ����������������� ���� ��� ����� � ���� ������� ���� �� MME.DB
     RemakeTree();                                           //���������������� ������ � ������ ������������� Titles
#if defined TEST_67_68_70_71                                 //�������
   if(Read_Sec_67_68_70_71() < 0) return -1;                 //������ ����������� ��������
#endif
#if defined TEST_LG1
   if(Read_Test1() < 0) return -1;                           //�������� ������ � �����
#endif
#if defined TEST_LG2_MME
   if(Read_Test2() < 0) return -1;                           //�������� ������ � �����
#endif
   return 0;
}

//------------------------------------------------------------------------------

static int Update_Tree_(void)                                //������������ ������ ������ � ����� ��� ��������� ��������
{
   TreeView_DeleteAllItems(hwndTree);                        //������� ������ ��� �������� ������
   if(Read_AllDir() < 0) return -1;                          //������ ���� ���������
   if(Conf.altName == 0 && Conf.ViewPart == 0 && tabMME != NULL)//����� �������������� �����, ����� ����������������� ���� ��� ����� � ���� ������� ���� �� MME.DB
     RemakeTree();                                           //���������������� ������ � ������ ������������� Titles
   return 0;
}

//------------------------------------------------------------------------------

int Update_Tree(void)                                        //������������ ������ ������ � ����� ��� ��������� ��������
{
   writeCl = 0;                                              //����� ������� ���������
   SetCursor(LoadCursor(NULL, IDC_WAIT));                    //����� ������� (����)
   int ret = Update_Tree_();                                 //������������ ������ ������ � ����� ��� ��������� ��������
   SetWindowText(hNumSel, "");
   SetWindowText(hSizeSel, "");
   EnableWindow(hClear, false);
   SetCursor(LoadCursor(NULL, IDC_ARROW));                   //����� ������� (�������)
   return ret;
}

//------------------------------------------------------------------------------

int Read_HDD(void)                                           //����� HDD �� ��������� LG � �������� ��������� ����������
{
   writeCl = 0;                                              //����� ������� ���������
   aTree = (OneStrTree *)MyAllocMem(MAX_NAME * sizeof(OneStrTree));//������ ��� ������
   if(aTree == NULL)  return -1;
#if !defined TEST_LG1 && !defined TEST_LG2_MME
   SetCursor(LoadCursor(NULL, IDC_WAIT));                    //����� ������� (����)
   Open_Wait_Found_HDD();                                    //������ �� ������� ���������
#endif
   int ret = My_Read_HDD();
#if !defined TEST_LG1 && !defined TEST_LG2_MME
   Close_Wait_Found_HDD();                                   //������ �� ������� ���������
   SetCursor(LoadCursor(NULL, IDC_ARROW));                   //����� ������� (�������)
#endif
#if defined OUT_TEST
   Add_SpecSpis("End of dump");
#endif
#if defined FIND_NO_ZERO_SEC                                 //����� �� ������� ��������
   FindNoZeroSec();                                          //����� �� ������� �������� � �������� ������
#endif
   ViewSize();                                               //����� ��������� ������������
   return ret;
}

#endif

