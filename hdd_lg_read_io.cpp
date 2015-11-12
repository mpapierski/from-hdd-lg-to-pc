
//                                    12.02.2009
#ifndef __HDD_LG_READ_IO__
#define __HDD_LG_READ_IO__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_read_io ==================================
int  ReadSec(BYTE *buff);                                    //������ �������
int  ReadClast1(BYTE *buff);                                 //������ �������� ������� ������� ������� � �������� �������
int  ReadClast1_P(DWORD nSector, BYTE *buff);                //������ �������� ������� ������� ������� � ���������� �������
int  ReadClast2(BYTE *buff);                                 //������ �������� ������� ������� ������� � �������� �������
int  ReadClast2_P(DWORD nSector, BYTE *buff);                //������ �������� ������� ������� ������� � ���������� �������
int  Load_FAT1(void);                                         //�������� FAT ������� �������
int  Load_FAT2(void);                                        //�������� FAT ������� �������
int  FindHDD_LG(void);                                       //����� ����� �� ��������� LG
     DWORD stCl_Media2;                                      //������� ������ ������ ����� ����� MEDIA

//------------------------------------------------------------------------------

int ReadSec(BYTE *buff)                                      //������ �������
{
   DWORD nb;
   if(ReadFile(hDrive, buff, sSecB, &nb, NULL) == FALSE || nb != sSecB)
   {  ErrorSys1((Lan+9)->msg);                               //������ ��� ������ �������.
      CloseFile(&hDrive);                                    //������� ������� ����
      return -1;
   }
   return 0;
}

//------------------------------------------------------------------------------

int ReadClast1(BYTE *buff)                                   //������ ��������
{
   DWORD nb;
   if(ReadFile(hDrive, buff, sCl_B, &nb, NULL) == FALSE || nb != sCl_B)
   {  ErrorSys1((Lan+10)->msg);                              //ErrorSys1("������ ��� ������ ��������.");
      CloseFile(&hDrive);                                    //������� ������� ����
      return -1;
   }
   return 0;
}

//------------------------------------------------------------------------------

int  ReadClast1_P(DWORD nSector, BYTE *buff)                 //������ ��������
{
   LONGLONG Poz = LONGLONG(sSecB) * nSector;
#if defined EMULATOR_HDD
   if(ReadClastEmul(Poz, buff) < 0) return -1;               //������ �������� ����������
#else
   if(SetInFilePointer(Poz) < 0) return -1;                  //��������� ������� ��������� � �����
   if(ReadClast1(buff) < 0) return -1;                       //������ ��������
#endif
   return 0;
}

//------------------------------------------------------------------------------

int ReadClast2(BYTE *buff)                                   //������ �������� ������� �������
{
   DWORD nb;
   if(ReadFile(hDrive, buff, sCl2_B, &nb, NULL) == FALSE || nb != sCl2_B)
   {  ErrorSys1((Lan+10)->msg);                              //ErrorSys1("������ ��� ������ ��������.");
      CloseFile(&hDrive);                                    //������� ������� ����
      return -1;
   }
   return 0;
}

//------------------------------------------------------------------------------

int  ReadClast2_P(DWORD nSector, BYTE *buff)                 //������ ��������
{
   LONGLONG Poz = LONGLONG(sSecB) * nSector;
#if defined EMULATOR_HDD
   if(ReadClast2Emul(Poz, buff) < 0) return -1;              //������ �������� ����������
#else
   if(SetInFilePointer(Poz) < 0) return -1;                  //��������� ������� ��������� � �����
   if(ReadClast2(buff) < 0) return -1;                       //������ ��������
#endif
   return 0;
}

//------------------------------------------------------------------------------
/*
static int Ctrl_FAT_For_File(DWORD clSt, LONGLONG SizeF)     //�������� ������������ ����� ����� � FAT
{
   DWORD nCl = clSt;                                         //������� ����� �������� ����� ������� �������� �����
   DWORD wSizeB = sCl_B;                                     //����� ������������ ���� ����� ����� ���� � ��������
   LONGLONG SizeToEnd = SizeF;                               //����� ���� ������� �������� � �������
   DWORD numCl = DWORD((SizeF + sCl_B - 1) / sCl_B);         //����� ��������� ����������� ��� ���������� ����� ������� �������
   for(DWORD i=0; i<numCl; i++)                              //�� ����� ��������� ������� ����� ��� ������� �����
   {  if(nCl == 0x0FFFFFFF)
        return Error1((Lan+12)->msg);                        //return Error1("���������� ������ ������� ����� ������� FAT.");
      if(*(FAT + nCl) == 0)
        return Error1((Lan+11)->msg);                        //return Error1("���������� �������������� �������� FAT � ������ �� ������� �����.");
      if(wSizeB > SizeToEnd) wSizeB = DWORD(SizeToEnd);      //������ ������� ������ ������ ������� ��������
      SizeToEnd -= wSizeB;                                   //����� ���� ������� �������� ��������
      nCl = *(FAT + nCl);                                    //����� ���������� ��������
      if(nCl > num_ZapFAT && nCl != 0x0FFFFFFF)
        return Error1((Lan+13)->msg);                        //return Error2("����� �������� � ������� FAT", "��������� ���������� ��������.");
      if(SizeToEnd == 0 && nCl != 0x0FFFFFFF)
        return Error1((Lan+14)->msg);                        //return Error2("������ ����� ��������� ��������,", "� ������� ����� ������� FAT �� ������.");
   }
   return 0;
}
*/

//------------------------------------------------------------------------------

static int ReadFAT1(void)                                    //������ FAT ������� �������
{
   DWORD nb;
   if(ReadFile(hDrive, FAT1, Size_FAT1, &nb, NULL) == FALSE || nb != Size_FAT1)
   {   ErrorSys1((Lan+22)->msg);                             //ErrorSys1("������ ��� ������ FAT.");
       CloseFile(&hDrive);                                   //������� ������� ����
       return -1;
   }
   return 0;
}

//------------------------------------------------------------------------------

static int ReadFAT2(void)                                    //������ FAT ������� �������
{
   DWORD nb;
   if(ReadFile(hDrive, FAT2, Size_FAT2, &nb, NULL) == FALSE || nb != Size_FAT2)
   {   ErrorSys1((Lan+22)->msg);                             //ErrorSys1("������ ��� ������ FAT.");
       CloseFile(&hDrive);                                   //������� ������� ����
       return -1;
   }
   return 0;
}

//-------------------------------------------------------------------------------

int Load_FAT1(void)                                          //�������� FAT ������� �������
{
#if defined OUT_TEST
   char Ss[300];
   wsprintf(Ss, "Load FAT 1 (sector %d, size (sectors) %d)", Start_SecFAT1, num_SecFAT1);
   Add_SpecSpis(Ss);
#endif
   FAT1 = (DWORD *)MyAllocMem(Size_FAT1);                    //������ ��� FAT
   if(FAT1 == NULL)  return -1;
   LONGLONG Poz = LONGLONG(sSecB) * Start_SecFAT1;
#if defined EMULATOR_HDD
   if(ReadFATEmul(Poz) < 0) return -1;                       //������ FAT ����������
#else
   if(SetInFilePointer(Poz) < 0) return -1;                  //��������� ������� ��������� � �����
   if(ReadFAT1() < 0) return -1;                             //������ FAT
#endif
#if defined OUT_TEST
   View_HEX_Any((BYTE*)FAT1, Size_FAT1);
#endif
   if(*(FAT1 + 0) != 0x00FFFFF8)
      return Error1((Lan+23)->msg);                          //return Error1("����������� ������ FAT.");
   stCl_Media2 = *(FAT1 + 1);                                //������� ������ ������ ����� ����� MEDIA
   //����������� ����� ���� �� 5, �.�. �������� 6 ���������
   //����� ���� �� ��������� ��� ��������� ������� ���������
   if(stCl_Media2 == 0x0FFFFFFF) stCl_Media2 = 0;            //������ ����� ����, �� ����� ����� �� ������
   if(stCl_Media2 == 0xFFFFFFFF) stCl_Media2 = 0;            //��������� ��������� �����
   return 0;
}

//------------------------------------------------------------------------------

#if defined OUT_TEST
static int Find_FAT2(void)                                   //����� � ������ FAT ������� �������
{
   char Ss[300];
   sprintf(Ss, "Not found FAT2 in sector %d", Start_SecFAT2);
   Add_SpecSpis(Ss);

   DWORD nb;
   InitProgressBar = InitProgressBar1;
   ProgressBar = ProgressBar1;
   Close_ProgressBar = Close_ProgressBar1;
   int numTestSec = Start_SecDir1 - Start_SecFAT1 - num_SecFAT1 - num_SecFAT2; //����� �������� ������� ���� ����������������
   DWORD Sec[128];                                           //������ ������ �������
   int pr = -1;
   LONGLONG Poz = LONGLONG(sSecB) * (Start_SecFAT1 + num_SecFAT1); //����� �� ������ FAT ������� �������
   if(SetInFilePointer(Poz) < 0) return -1;                  //��������� ������� ��������� � �����
   InitProgressBar(numTestSec, (Lan+21)->msg);               //InitProgressBar(nCycl, "������ ��������� ����������");
   for(int i=0; i<numTestSec; i++)
   {  if(ReadFile(hDrive, &Sec, sSecB, &nb, NULL) == FALSE || nb != sSecB)
      {  ErrorSys1((Lan+22)->msg);                           //ErrorSys1("������ ��� ������ FAT.");
         CloseFile(&hDrive);                                 //������� ������� ����
         return -1;
      }
      if(ProgressBar(sSecB) < 0)                             //�������� ����� ������ ��������
      {  Close_ProgressBar();  return -1;   }
      if(*(Sec + 0) != 0x00FFFFF8 || *(Sec + 1) != 0) continue; //���� ������ �� �������� ������� FAT2
      pr = i;
      break;
   }
   Close_ProgressBar();
   if(pr == -1) return -1;
   DWORD stSec_FAT2 = pr + Start_SecFAT1 + num_SecFAT1;
   LONGLONG Poz_St = LONGLONG(sSecB) * stSec_FAT2;
   if(SetInFilePointer(Poz_St) < 0) return -1;               //��������� ������� ��������� � �����
   if(ReadFAT2() < 0) return -1;                             //������ FAT ������� �������
   wsprintf(Ss, "Load FAT 2 (sector %d, size (sectors) %d)", stSec_FAT2, num_SecFAT2);
   Add_SpecSpis(Ss);
   return 0;
}
#endif

//-------------------------------------------------------------------------------

int Load_FAT2(void)                                          //�������� FAT ������� �������
{
#if defined OUT_TEST
   char Ss[300];
   wsprintf(Ss, "Load FAT 2 (sector %d, size (sectors) %d)", Start_SecFAT2, num_SecFAT2);
   Add_SpecSpis(Ss);
#endif
   FAT2 = (DWORD *)MyAllocMem(Size_FAT2);                     //������ ��� FAT
   if(FAT2 == NULL)  return -1;
   LONGLONG Poz = LONGLONG(sSecB) * Start_SecFAT2;
#if defined EMULATOR_HDD
   if(ReadFAT2Emul(Poz) < 0) return -1;                      //������ FAT ����������
#else
   if(Start_SecFAT2 < Start_SecFAT1 + num_SecFAT1 ||
      Start_SecFAT2 >  Start_SecDir1)
  #if defined OUT_TEST
      if(Find_FAT2() < 0) return -1;                         //����� � ������ FAT ������� �������
  #else
      return Error1((Lan+135)->msg);                         //return Error1("������������ ����� ������� ������ FAT2.");
  #endif
   if(SetInFilePointer(Poz) < 0) return -1;                  //��������� ������� ��������� � �����
   if(ReadFAT2() < 0) return -1;                             //������ FAT ������� �������
#endif
#if defined OUT_TEST
   if(*(FAT2 + 0) != 0x00FFFFF8 || *(FAT2 + 1) != 0)         //���� FAT ������� ������� �� ������
      if(Find_FAT2() < 0) return -1;                         //����� � ������ FAT ������� �������
   View_HEX_Any((BYTE*)FAT2, Size_FAT2);
   if(*(FAT2 + 0) != 0x00FFFFF8 || *(FAT2 + 1) != 0)
      Error1((Lan+23)->msg);                                 //return Error1("����������� ������ FAT.");
#else
   if(*(FAT2 + 0) != 0x00FFFFF8 || *(FAT2 + 1) != 0)
      return Error1((Lan+23)->msg);                          //return Error1("����������� ������ FAT.");
#endif
   return 0;
}

//------------------------------------------------------------------------------

#define MAX_CHAR 40                                          //��������� ����� ������

static int ErrorSys1_New(char *Msg, DWORD Code)
{
   char Ss[2048], MsgErr[512];

   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, Code,
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                 MsgErr, 512, NULL);
   int n = lstrlen(MsgErr);                                  //����� �������� � ��������� ���������
   int m = MAX_CHAR;                                         //����� ������
   if(n > m)
   {  for(;;)
      { int i;
        for(i=m; i<n; i++)                                   //����� ����� �� ��������� �����
          if(*(MsgErr+i) == ' ')                             //����� ������ �� ��������
          {  *(MsgErr+i) = '\n';
             m = i + MAX_CHAR + 1;
             break;
          }
        if(n - m < MAX_CHAR || i >= n) break;
      }
   }
   wsprintf(Ss, "%s\n\n%s:\n%s", Msg, (Lan+37)->msg, MsgErr);               //wsprintf(Ss, "%s\n\n��������� ���������:\n%s", Msg, MsgErr);
   MessageBox(MainWin, Ss, (Lan+35)->msg, MB_TOPMOST | MB_OK | MB_ICONSTOP);//MessageBox(MainWin, Ss, "������", MB_TOPMOST | MB_OK | MB_ICONSTOP);
#if defined OUT_TEST
   Add_SpecSpis(Ss);
#endif
   return -1;
}

//------------------------------------------------------------------------------

static int ReadSec0(BYTE *buff)                              //������ �������
{
   DWORD nb;
   if(ReadFile(hDrive, buff, sSecB, &nb, NULL) == FALSE || nb != sSecB)
   {  DWORD Code = GetLastError();
      if(Code != ERROR_NOT_READY)
         ErrorSys1_New((Lan+9)->msg, Code);                  //������ ��� ������ �������
#if defined OUT_TEST
      else                                                   //���������� �� ������������
         Add_SpecSpis("The device is not ready. ");
#endif
      CloseFile(&hDrive);                                    //������� ������� ����
      return -1;
   }
   return 0;
}

//------------------------------------------------------------------------------

#if defined OUT_TEST
static void ReadAnySec(void)
{
   SEC_0 anySec;
   LONGLONG Poz = LONGLONG(sSecB) * 67;
   SetInFilePointer(Poz);
   ReadSec((BYTE *)&anySec);                                 //������ �������
   Add_SpecSpis("Sector 67");
   View_HEX_Any((BYTE *)&anySec, 512);
   ReadSec((BYTE *)&anySec);                                 //������ �������
   Add_SpecSpis("Sector 68");
   View_HEX_Any((BYTE *)&anySec, 512);
   Poz = LONGLONG(sSecB) * 70;
   SetInFilePointer(Poz);
   ReadSec((BYTE *)&anySec);                                 //������ �������
   Add_SpecSpis("Sector 70");
   View_HEX_Any((BYTE *)&anySec, 512);
   ReadSec((BYTE *)&anySec);                                 //������ �������
   Add_SpecSpis("Sector 71");
   View_HEX_Any((BYTE *)&anySec, 512);
}
#endif

//-------------------------------------------------------------------------------

int FindHDD_LG(void)                                         //����� ����� �� ��������� LG
{
   hDrive = NULL;
#if defined OUT_TEST
   Add_SpecSpis("Find HDD from LG");
#endif
#if defined HDD_LG
   for(int drive=HDD_LG; drive<32; drive++)                  //�� ���� ��������� ���������� ����������� ������� � ����������
#else
   for(int drive=0; drive<32; drive++)                       //�� ���� ��������� ���������� �����������
#endif
   {  char driveName[256];                                   //��� ����������� ����������
      sprintf(driveName, "\\\\.\\PhysicalDrive%d", drive);
#if defined OUT_TEST
      Add_SpecSpis(driveName);
#endif
      hDrive = CreateFile(driveName, GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
//                        FILE_FLAG_NO_BUFFERING |
                          0, NULL);
#if defined OUT_TEST
      if(hDrive == INVALID_HANDLE_VALUE)                     //���������� �� ������������
         Add_SpecSpis("Error Drive");
#endif
      if(hDrive == INVALID_HANDLE_VALUE) continue;           //���������� �� ������������
      SEC_0 Sec0;
      if(ReadSec0((BYTE*)&Sec0) < 0) continue;               //������ �������, ���� ������ �� ��������, �� �� ����������� ������
      SEC_63 Sec63, Sec64;
      LONGLONG Poz = LONGLONG(sSecB) * 63;
      if(SetInFilePointer(Poz) < 0)  //return -1;            //��������� ������� ��������� � �����
      {  CloseFile(&hDrive);  continue;  }                   //������-�� �� ���������
      if(ReadSec((BYTE *)&Sec63) < 0) continue;              //������ �������
      if(strncmp(Sec63.Met, "LGEINC  ", 8) != 0 ||
         strncmp(Sec63.Name, "VOLUMELABE FAT32   ", 19) != 0)
#if defined OUT_TEST
      {  Add_SpecSpis("No HDD from LG");
#else
      {
#endif
         CloseFile(&hDrive);  continue;  }
#if defined OUT_TEST
      Add_SpecSpis("HDD from LG found");
      Add_SpecSpis("Sector 63");
      View_HEX_Any((BYTE *)&Sec63, 512);
#endif
      if(Sec63.nf != 1 || Sec63.F8 != 0xF8 || Sec63.SecPerCl != sClSec)
#if defined OUT_TEST
        Error1((Lan+19)->msg);                               //Error1("������ ������� ���� ������ �������� ������� �� ��, ��� �� �� ��������� LG.");
#else
        return Error1((Lan+19)->msg);                        //return Error1("������ ������� ���� ������ �������� ������� �� ��, ��� �� �� ��������� LG.");
#endif
      Start_SecFAT1 = 63 + Sec63.numRezSec;                  //������� ������ ������� FAT ������� �������
      Start_SecDir1 = Start_SecFAT1 + Sec63.nSecDir;         //������� ������ ������� ��������
      num_SecFAT1 = Sec63.numSecFAT;                         //����� �������� � FAT
      Size_FAT1 = num_SecFAT1 * sSecB;                       //����� ���� � FAT
//??  maxZapFAT1 = sSecB / 4 * num_SecFAT1;                  //������������ ����� ������� � FAT (�� 4 �����)
      maxZapFAT1 = Sec63.numSecPart / sClSec;                //������������ ����� ������� � FAT (�� 4 �����)
      if(ReadSec((BYTE *)&Sec64) < 0) return -1;             //������ �������
#if defined OUT_TEST
      Add_SpecSpis("Sector 64");
      View_HEX_Any((BYTE *)&Sec64, 512);
#endif
      if(strncmp(Sec64.Met, "LGEINC  ", 8) != 0 ||
         strncmp(Sec64.Name, "VOLUMELABE FAT32   ", 19) != 0)
      {  CloseFile(&hDrive);  continue;  }
      if(Sec64.nf != 1 || Sec64.F8 != 0xF8 || Sec64.SecPerCl != sClSec2)
        Error1((Lan+19)->msg);                               //return Error1("������ ������� ���� ������ �������� ������� �� ��, ��� �� �� ��������� LG.");
      Start_SecDir2 = Sec63.numSecPart + 63 + Sec64.numRezSec + Sec64.nSecDir; //����� ������� ������ �������� �� ������ �������
      num_SecFAT2 = Sec64.numSecFAT;                         //����� �������� � FAT
      Size_FAT2 = num_SecFAT2 * sSecB;                       //����� ���� � FAT
      maxZapFAT2 = sSecB / 4 * num_SecFAT2;                  //������������ ����� ������� � FAT2 (�� 4 �����)
      Start_SecFAT2 = Start_SecFAT1 + Sec0.numClP1;          //������� ������ ������� FAT2
//��� �������� ������� BIG LBA ����� ����� ��������� ������ � ������� 268435456+64 (0x10000000+64)
//� �������� ����������� �� ����������� � �������� 64
#if defined OUT_TEST
      Add_SpecSpis("Sector 0");
      View_HEX_Any((BYTE *)&Sec0, 512);
      ReadAnySec();
#endif
      break;
   }
   if(hDrive == NULL || hDrive == INVALID_HANDLE_VALUE)
      return Error1((Lan+20)->msg);                          //return Error1("�� ������ ������� ���� �� ��������� LG.");
   return 0;
}

#endif

