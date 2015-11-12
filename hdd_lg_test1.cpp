
//                               10.07.2007
#ifndef __HDD_LG_TEST1__
#define __HDD_LG_TEST1__

#include "from_hdd_lg_to_pc.h"

#if defined TEST_LG1

//============================ hdd_lg_test1 ====================================
int  Read_Test1(void);                                       //�������� ������ � �����

static DWORD numClPointMEDIA;                                //����� �������� � ������ '.' � ����� MEDIA
static int Read_One_Dir_T(DWORD nClast, int pr);             //������ � ������������ ������ ��������

//------------------------------------------------------------------------------

static int Ctrl_FAT_For_File_T(DWORD clSt, LONGLONG SizeF)   //�������� ������������ ����� ����� � FAT
{
   DWORD nCl = clSt;                                         //������� ����� �������� ����� ������� �������� �����
   DWORD wSizeB = sCl_B;                                     //����� ������������ ���� ����� ����� ���� � ��������
   LONGLONG SizeToEnd = SizeF;                               //����� ���� ������� �������� � �������
   DWORD numCl = DWORD((SizeF + sCl_B - 1) / sCl_B);         //����� ��������� ����������� ��� ���������� ����� ������� �������
   for(DWORD i=0; i<numCl; i++)                              //�� ����� ��������� ������� ����� ��� ������� �����
   {  if(*(FAT1 + nCl) == 0)
      {  Add_Spis(".....���������� �������������� �������� FAT � ������ �� ������� �����."); break; }
      if(nCl == 0x0FFFFFFF)
      {  Add_Spis(".....���������� ������ ������� ����� ������� FAT."); break; }
      if(wSizeB > SizeToEnd) wSizeB = DWORD(SizeToEnd);      //������ ������� ������ ������ ������� ��������
      SizeToEnd -= wSizeB;                                   //����� ���� ������� �������� ��������
      nCl = *(FAT1 + nCl);                                    //����� ���������� ��������
      if(nCl > maxZapFAT1 && nCl != 0x0FFFFFFF)
      {  Add_Spis(".....����� �������� � ������� FAT ��������� ���������� ��������."); break; }
      if(SizeToEnd == 0 && nCl != 0x0FFFFFFF)
     {  Add_Spis(".....������ ����� ��������� ��������, � ������� ����� ������� FAT �� ������."); break; }
   }
   return 0;
}

//------------------------------------------------------------------------------

static void View_Str_Dir_Hex(int n, One_Str_Cat *Kat)        //����� ������ �������� � ���������������� ����
{
   char *Adr, Ss[300];

   sprintf(Ss, "....������ �������� %d..............................................", n+1);
   Add_Spis(Ss);
   BYTE *bb = (BYTE*)(Kat + n);
   for(int i=0; i<128; i+=32)                                //����� ����������������� ����� ����� ������ ��������
   {   Adr = Ss;
       for(int j=i; j<i+32; j++)
       {   Adr += sprintf(Adr, " %02X", bb[j]);
           if((j+1) % 8 == 0)
             Adr += sprintf(Adr, "   ");
       }
       *(Adr + 1) = 0;
       Add_Spis(Ss);                                         //���������� ������ � ������
   }
}

//------------------------------------------------------------------------------

static const DWORD Point1[] = { 0x00002E00, 0x20002E00 };    //��� ����� (� ��������� ����� ������ ���� - �������)

static int View_Dir_T(BYTE *buff, int pr)                    //�������� ����� ����������� ��������
{
   char *Adr, Ss[300];

   One_Str_Cat *Kat = (One_Str_Cat *)buff;
   for(int n=0; n<4*sClSec; n++)                             //� ������ ������� 4 ������
   {  if((Kat + n)->pf.type == 0) return 1;                  //����� ��������
//    if((Kat + n)->Name[0] == 0 && (Kat + n)->Name[1] == 0) return 1; //����� ��������

//      View_Str_Dir_Hex(n, Kat);                            //����� ������ �������� � ���������������� ����

      DWORD NamePoint = *((DWORD*)&(Kat + n)->Name);
      int prPoint1 = (NamePoint == *(Point1 + pr_tRec)) ? 1 : 0;//��� ���� ����� �.�. ������� �������� ��������
      int prPoint2 = (NamePoint == 0x2E002E00) ? 1 : 0;      //��� ��� ����� �.�. ������� ����������� ��������
      WORD NameDel = *((WORD*)&(Kat + n)->Name);
//    if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0) continue; //��� ��������� ��� (������ ����� ����� � � ����� �������� ������ ����)
      if((Kat + n)->pf.type != 32 && (Kat + n)->pf.type != 48)
         return Error1("����������� ��� ������ � ��������");
      char nam[128], ext[128];
      if(pr_tRec == 0)                                       //������� ��������� 0 ��� 1 (��������� �����)
      {  UnicodeToAnsi((Kat + n)->Name, nam, 41);            //������������� ��� �� UNICODE
         UnicodeToAnsi((Kat + n)->Ext, ext, 4);              //������������� ���������� �� UNICODE
      }
      if(pr_tRec == 1)                                       //������� ��������� 0 ��� 1 (��������� �����)
      {  UnicodeToAnsi((Kat + n)->Name, nam, 45);            //������������� ��� � ���������� �� UNICODE
         lstrcpy(ext, nam + 42);                             //������� ����������
         if(*ext == ' ') *ext = 0;                           //���� ���������� ���, �� ��������
         for(int i=41; i>0; i--)                             //�������� ����� ����� �� �������� ������ ������ �������
         {  if(*(nam + i) != ' ') break;                     //��������� ����� ����� ����� ���������� �������
            *(nam + i) = 0;
         }
      }
//    if(strcmp(ext, "idx") == 0) continue;

      FillMemory(Ss, sizeof(Ss), '_');
      if((Kat + n)->pf.type == 32)                           //��� ��� �����
      {  if(*ext != 0) sprintf(Ss+2*pr, "%03d. %s.%s", n+1, nam, ext);
         else strcpy(Ss+2*pr, nam);
      }
      if((Kat + n)->pf.type == 48)                           //��� ��� �����
      {  if(*ext != 0) sprintf(Ss+2*pr, "%03d.  [%s.%s]", n+1, nam, ext);
         else sprintf(Ss+2*pr, "%03d. [%s]", n+1, nam);
         if((Kat + n)->pf.SizeF != 0)
            return Error1("������ �������� ���������� ��� ����� ����� ������ � ������.");
      }
      int l = lstrlen(Ss);
      for(int i=l; i<50; i++) Ss[i] = ' ';
      Adr = Ss + 50;
      if((Kat + n)->pf.type == 32)
           Adr += sprintf(Adr, "%18.0lf", double((Kat + n)->pf.SizeF));
      else Adr += sprintf(Adr, "                  ");
      Adr += sprintf(Adr, "%10d  ", (Kat + n)->pf.ClSt);
      Adr += sprintf(Adr, "%02d-%02d-%04d  %02d:%02d:%02d  ", (Kat + n)->pf.Day, (Kat + n)->pf.Mon, (Kat + n)->pf.Year,
                                                     (Kat + n)->pf.Hour, (Kat + n)->pf.Min, (Kat + n)->pf.Sec);
//    BYTE *b = (BYTE*)&(Kat + n)->pf.LL1;
//    Adr += sprintf(Adr, "| %02X%02X%02X%02X %02X%02X%02X%02X ||", b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
      Adr += sprintf(Adr, "%16.0lf |", double((Kat + n)->pf.lL1));
      if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0)       //��� ��������� ��� (������ ����� ����� � � ����� �������� ������ ����)
         Adr += sprintf(Adr, "  �������");
      Add_Spis(Ss);                                          //���������� ������ � ������
      if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0) continue; //��� ��������� ��� (������ ����� ����� � � ����� �������� ������ ����)
      if((Kat + n)->pf.type == 48)                           //��� ��� �����
      {  if(pr == 0 && prPoint1 == 1) numClPointMEDIA = (Kat + n)->pf.ClSt; //����� �������� � ������ '.' � ����� MEDIA
         if(pr == 0) continue;
         if(prPoint1 == 1) continue;                         //������� '.'  ���� �� ����������
         if(prPoint2 == 1) continue;                         //������� '..'  ���� �� ����������
         if(Read_One_Dir_T((Kat + n)->pf.ClSt, pr+1) < 0) return -1;//������ � ������������ ������ ��������
      }
      if((Kat + n)->pf.type == 32)                           //��� ��� �����
        Ctrl_FAT_For_File_T((Kat + n)->pf.ClSt, (Kat + n)->pf.SizeF); //�������� ������������ ����� ����� � FAT
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir_T_(BYTE *buff, DWORD nClast, int pr) //�������� ������ � ������������ ������ ��������
{
   char Ss[300];
   DWORD nSector = Start_SecDir1 + (nClast - 1) * sClSec;    //������� ������� �������� �������� MEDIA
   sprintf(Ss, " ********** ������� %d.  ��������� ������ %d  (0x%08X) **********", nClast, nSector, nSector);
   Add_Spis(Ss);                                             //���������� ������ � ������
   if(ReadClast1_P(nSector, buff) < 0) return -1;            //������ ��������
// if(View_Dir_T(buff, pr) < 0) return -1;                   //����� ����������� ��������
   View_Dir_T(buff, pr);                                     //����� ����������� ��������
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir_T(DWORD nClast, int pr)              //������ � ������������ ������ ��������
{
   if(nClast == 0 || nClast >= maxZapFAT1)
   {  Add_Spis((Lan+100)->msg);                              //���������� ������ � ������
      return Error1((Lan+100)->msg);                         //return Error1("������������ ����� �������� ��������.");
   }
   if(nClast != 1 && *(FAT1 + nClast) == 0)                   //
      return Error1((Lan+18)->msg);                          //return Error1("���������� �������������� �������� FAT � ������ �� ������� ��������.");
   if(nClast != 1 && *(FAT1 + nClast) != 0x0FFFFFFF)
      return Error1((Lan+17)->msg);                          //return Error1("������ ������ �������� ��������� ������ ��������.");
   BYTE *buff = (BYTE *)MyAllocMem(sCl_B);                   //������ ��� ���� �������
   if(buff == NULL)  return -1;
   int ret = Read_One_Dir_T_(buff, nClast, pr);              //������ � ������������ ������ ��������
   MyFreeMem(&(void*)buff);
   return ret;
}

//------------------------------------------------------------------------------

static int Read_Dir_T(void)                                  //�������� x����� ��������
{
   Read_One_Dir_T(1, 0);                                     //������ � ������������ �������� MEDIA
   if(stCl_Media2 != 0)                                      //������� ������ ������ ����� ����� MEDIA
   { Add_Spis("������ ������� �������� ����� MEDIA");
     Read_One_Dir_T(stCl_Media2, 0);                         //������ � ������������ ������ ����� ����� MEDIA
   }
   Read_One_Dir_T(2, 1);                                     //������ � ������������ ��������� ���������
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir_T_Sec_(BYTE *buff, DWORD nSec, int pr) //�������� ������ � ������������ ������ ��������
{
   char Ss[300];
   double nClast = (double(nSec) - Start_SecDir1) / sClSec + 1;
   sprintf(Ss, " ********** ������� %.2lf.  ��������� ������ %d  (0x%08X) **********", nClast, nSec, nSec);
   Add_Spis(Ss);                                             //���������� ������ � ������
   if(ReadClast1_P(nSec, buff) < 0) return -1;
   if(View_Dir_T(buff, pr) < 0) return -1;                   //����� ����������� ��������
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir_T_Sec(DWORD nSec, int pr)            //������ � ������������ ������ ��������
{
   BYTE *buff = (BYTE *)MyAllocMem(sCl_B);                   //������ ��� ���� �������
   if(buff == NULL)  return -1;
   int ret = Read_One_Dir_T_Sec_(buff, nSec, pr);            //������ � ������������ ������ ��������
   MyFreeMem(&(void*)buff);
   return ret;
}

//------------------------------------------------------------------------------

#if defined TEST_LG1_AND_TS                                  //������ ������ � ��������� ���� � ����� TimeShift
#define NDW_S 16                                             //����� �������� � ������
static int View_Folder_Point0(BYTE *buff)                    //����� ����������� �������� [.] �� ����� MEDIA
{
   char Ss[1024];

   Add_Spis("C���������� �������� [.] �� ����� MEDIA");
   int n = sCl_B / sizeof(DWORD);                            //����� DWORD � ��������
   DWORD *DW = (DWORD*)buff;                                 //������� ��� �������� DWORD
   InitProgressBar1(n/NDW_S, "������������ �������");
   for(int i=0; i<n; i+=NDW_S)                               //�� ����� ����� � NDW_S ����������
   {  char *Adr = Ss;
      for(int j=0; j<NDW_S; j++)
      { //SWAP32(DW+i+j);
        Adr += sprintf(Adr, "%12d", *(DW+i+j));
//      if(j != 2) continue;
        WORD *W = (WORD*)(DW+i+j);
        Adr += sprintf(Adr, "(%6d %6d) ", *W, *(W+1));
        Adr += sprintf(Adr, "[%8X] ", *(DW+i+j));
      }
      *Adr = 0;
      Add_Spis(Ss);                                          //���������� ������ � ������
      if(ProgressBar1(sSecB) < 0)                            //�������� ����� ������ ��������
      {  Close_ProgressBar1();  return -1;   }
   }
   Close_ProgressBar1();
   return 0;
}
#endif

//------------------------------------------------------------------------------

#if defined TEST_LG1_AND_TS                                  //������ ������ � ��������� ���� � ����� TimeShift
#define NB_S 48                                              //����� �������� � ������
static int View_Folder_Point1(BYTE *buff)                    //����� ����������� �������� [.] �� ����� MEDIA � ���������� ����
{
   char Ss[1024];

   Add_Spis("C���������� �������� [.] �� ����� MEDIA");
   int n = sCl_B;
   BYTE *B = (BYTE*)buff;                                    //������� ��� �������� DWORD
   InitProgressBar1(n/NB_S, "������������ �������");
   for(int i=0; i<n; i+=NB_S)                                //�� ����� ����� � NB_S ����������
   {  char *Adr = Ss;
      for(int j=0; j<NB_S; j++)
        Adr += sprintf(Adr, "%3d ", *(B+i+j));
      *Adr = 0;
      Add_Spis(Ss);                                          //���������� ������ � ������
      if(ProgressBar1(sSecB) < 0)                            //�������� ����� ������ ��������
      {  Close_ProgressBar1();  return -1;   }
   }
   Close_ProgressBar1();
   return 0;
}
#endif

//-------------------------------------------------------------------------------

#if defined TEST_LG1_AND_TS                                  //������ ������ � ��������� ���� � ����� TimeShift
static void View_Cl_Hex(BYTE *buff, int pr)                  //����� ����������� �������� � ���������������� ����
{
   BYTE *t_buff = buff;
   InitProgressBar1(sClSec, "������������ �����");
   for(int i=0; i<sClSec; i++)                               //�� ����� �������� � ��������
   {  View_Sec_Hex(t_buff, pr);                              //����� ����������� �������
      t_buff += sSecB;                                       //��������� ����� ������ �� ������ �������
      Add_Spis("----------------------------------------------------------------------------");
      if(ProgressBar1(sSecB) < 0)                            //�������� ����� ������ ��������
      {  Close_ProgressBar1();  return;   }
   }
   Close_ProgressBar1();
}
#endif

//------------------------------------------------------------------------------

#if defined TEST_LG1_AND_TS                                  //������ ������ � ��������� ���� � ����� TimeShift
static int ViewSpecCl(DWORD nCl, int pr)                     //������������ ��������
{
   BYTE buff[sCl_B];
   char Ss[300];

   DWORD nSector = (nCl - 1) * sClSec + Start_SecDir1;
   sprintf(Ss, " ********** ������� %d.  ��������� ������ %d  (0x%08X) **********", nCl, nSector, nSector);
   Add_Spis(Ss);                                             //���������� ������ � ������
   if(ReadClast1_P(nSector, buff) < 0) return -1;            //������ ��������
   if(pr == 1)
     View_Cl_Hex(buff, pr);                                  //����� ����������� ��������  � ���������������� ����
   if(pr == 2)
     View_Folder_Point0(buff);                               //����� ����������� �������� [.] �� ����� MEDIA
   if(pr == 3)
     View_Folder_Point1(buff);                               //����� ����������� �������� [.] �� ����� MEDIA � ���������� ����
   return 0;
}
#endif

//------------------------------------------------------------------------------

int  Read_Test1(void)                                        //�������� ������ � �����
{
   if(Read_Dir_T() < 0) return -1;                           //�������� ������ ��������
#if defined TEST_LG1_AND_TS                                  //������ ������ � ��������� ���� � ����� TimeShift
// ViewSpecCl(numClPointMEDIA, 1);                           //����� ����������� �������� [.] �� ����� MEDIA � ���������������� ����
   ViewSpecCl(numClPointMEDIA, 2);                           //����� ����������� �������� [.] �� ����� MEDIA
// ViewSpecCl(numClPointMEDIA, 3);                           //����� ����������� �������� [.] �� ����� MEDIA � ���������� ����
#endif
   return 0;
}

#endif

#endif

