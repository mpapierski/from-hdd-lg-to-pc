
//                                    11.08.2008
#ifndef __HDD_LG_EMULATOR__
#define __HDD_LG_EMULATOR__

#include "from_hdd_lg_to_pc.h"

#if defined EMULATOR_HDD

//============================ hdd_lg_emulator =================================
int ReadClastEmul(DWORDLONG Poz, BYTE *buff);                //������ �������� ����������
int ReadClast2Emul(DWORDLONG Poz, BYTE *buff);               //������ �������� ����������
int ReadFATEmul(DWORDLONG Poz);                              //������ FAT ����������
int ReadFAT2Emul(DWORDLONG Poz);                             //������ FAT ����������
int Read_Sec_Emul(int Num, BYTE* Sec);                       //������ ������� �� ������ ����������
int FindHDD_LG_Emul(void);                                   //����� ����� �� ��������� LG ����������
    char *inBuf;
    char NameFDump[260];                                     //��� ����� � ������

static DWORD inSize;
static char *Adr, *AdrEnd;

//------------------------------------------------------------------------------

static int OpenInFile(char *NameF, HANDLE *inFile)                           //�������� �����
{
   *inFile = CreateFile(NameF,                                //����� ������ � ������ �����
                       GENERIC_READ,                         //������ ������� � �����
                       FILE_SHARE_READ,                      //����������� ������ ����� ������ ������ ����
                       NULL,                                 //����� ��������� � ����������� ������ ����
                       OPEN_EXISTING,                        //��� �������� �����
                       FILE_ATTRIBUTE_NORMAL,                //�������� �����
                       NULL);                                //������������ ��� ��������� ������
   if(*inFile == INVALID_HANDLE_VALUE)
     return ErrorSys2("������ ��� �������� �����", NameF);
   inSize = GetFileSize(*inFile, NULL);
   return 0;
}

//------------------------------------------------------------------------------

static int GetName_InFile(char *NameF)                       //���� ����� ��������������� �����
{
   OPENFILENAME ofn;
   char DirName[260];                                        //��������� ���� ��� ������ ������
   char FileTitle[256];                                      //��������� ����
   char Filter[256];                                         //������� ������
   char NameFile[260];

   strcpy(NameFile, NameF);                                  //��������� ���� ��� ������ ������
   *DirName = 0;                                             //�� ������ ���� ����� ������
   if(*NameFile != 0)                                        //��� ��� ����
   {  char *A;
      lstrcpy(DirName, NameFile);                            //��������� ���� ��� ������ ������
      //������� ������� ������� ������ ���
      A = strrchr(DirName, '\\');                            //����� ��������� �����������
      if(A != NULL) *A = 0;
      else *DirName = 0;                                     //��� ����� �����
   }
   *NameFile = 0;
   lstrcpy(Filter, "����� �����|*.txt|��� ����� (*.*)|*.*|");
   for(int i=0; Filter[i]!=0; i++)                           //�������� ����������� 0
     if(Filter[i] == '|') Filter[i] = 0;
   ZeroMemory(&ofn, sizeof(OPENFILENAME));                   //��������� ������
   ofn.lStructSize = sizeof(OPENFILENAME);                   //����� ���������
   ofn.hwndOwner = MainWin;                                  //�������� �������
   ofn.hInstance = MainInst;                                 //������������� ��������� ��������� ��������
   ofn.lpstrFilter = Filter;                                 //���� ��������������� ������
// ofn.lpstrCustomFilter                                     //����������� �������
// ofn.nMaxCustFilter                                        //����� ������������ �������
   ofn.nFilterIndex = 1;                                     //������ ��� ������ � ���������
   ofn.lpstrFile = NameFile;                                 //��� ����� � ������ ������
   ofn.nMaxFile = sizeof(NameFile);                          //����� ���� ����� �����
   ofn.lpstrFileTitle = FileTitle;                           //������� � ��� ����� � ������ ������
   ofn.nMaxFileTitle = sizeof(FileTitle);                    //����� ����
   ofn.lpstrInitialDir = DirName;                            //��������� ������� ������
   ofn.lpstrTitle = "������� ��� ����� �����";
   ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
// ofn.Flags |= OFN_ENABLESIZING;                            //����� ������ ������ ���� �������
// ofn.nFileOffset;                                          //�������� � ������ lpstrFile
// ofn.nFileExtension;                                       //�������� � ������ lpstrFile
   ofn.lpstrDefExt = "txt";                                  //���������� �� ���������
// ofn.lCustData;                                            //��� ������� ������������
//   ofn.lpfnHook = ModDlg.fnHook;                           //������� ��������� ��������������� �������
//   ofn.lpTemplateName = ModDlg.DopDlgName;                 //�������������� ������ ������� ����������� � ��������
//   if(NumWinNT != 0)                                       //0-������� Win95/98 ��� ����� NT
//     ofn.FlagsEx = OFN_EX_NOPLACESBAR;                     //������� ������ �����
   if(GetOpenFileName(&ofn) == FALSE) return -1;
   if(*NameFile == 0) return -1;
   strcpy(NameF, NameFile);                                  //��������� ��� �����
   return 0;
}

//------------------------------------------------------------------------------

static int FindStr(char *msg)                                //����� ������ ������������ �� ����� msg
{
   int n = lstrlen(msg);
   Adr = inBuf;
   for(;;)                                                   //�� ����� �����
   {  char *NewAdr = strchr(Adr, '\0');
      if(NewAdr > AdrEnd || NewAdr == NULL) return -1;       //������ ��������� � ������ ��� �� ����� ������ ������
      if(strncmp(Adr, msg, n) == 0) return 0;
      Adr = NewAdr + 1;
   }
}

//------------------------------------------------------------------------------

static int Read_Dump(BYTE *buf, DWORD size)
{
   int num, ahex;
   ZeroMemory(buf, size);                                    //�������� ����� �� ������ ���������� ����
   char *NewAdr = strchr(Adr, '\0');
   if(NewAdr > AdrEnd || NewAdr == NULL) return -1;          //������ ��������� � ������ ��� �� ����� ������ ������
   Adr = NewAdr + 1;
   DWORD nn = 0;
   for(int n=1, hex=0; ; n++,hex+=128)                       //�� ����� �����
   {  NewAdr = strchr(Adr, '\0');
      if(NewAdr > AdrEnd || NewAdr == NULL) return -1;       //������ ��������� � ������ ��� �� ����� ������ ������
      if(lstrcmp(Adr, "All bytes == 0\r") == 0) return 0;    //��������� ����� ����
      sscanf(Adr, "%d: %x:", &num, &ahex);
      if(n != num && ahex != hex)
        return Error2("��������� ������� ���������� �����.", "��������.");
      Adr += 14;
      for(int j=1; j<=128; j++)
      {  if(strncmp(Adr, "(0)", 3) == 0) return 0;           //����� ��� ����
         DWORD OneB;
         sscanf(Adr, "%x", &OneB);
         *(buf + nn) = BYTE(OneB);
         nn++;
         if(nn > size)
           return Error2("������������ ������.", "��������.");
         Adr += 3;
         if((j % 8) == 0) Adr++;
      }
      Adr = NewAdr + 1;
      if(nn == size) return 0;
   }
}

//------------------------------------------------------------------------------

static int Load_Dump(void)                                   //�������� �����
{
   HANDLE inFile;
   DWORD nb;
   if(GetName_InFile(NameFDump) < 0) return -1;              //���� ����� ��������������� �����
   if(OpenInFile(NameFDump, &inFile) < 0) return -1;         //�������� �������� �����
   inBuf = (char *)MyAllocMem(inSize);
   if(inBuf == NULL) return -1;
   if(ReadFile(inFile, inBuf, inSize, &nb, NULL) == FALSE || nb != inSize)
      return ErrorSys2("������ ��� ������ ��������� �����", NameFDump);
   CloseFile(&inFile);                                       //������� ������� ����
   AdrEnd = inBuf + inSize;
   Adr = inBuf;
   for(;;)                                                   //�� ����� �����
   {  char *NewAdr = strchr(Adr, '\n');
      if(NewAdr > AdrEnd || NewAdr == NULL) break;           //������ ��������� � ������ ��� �� ����� ������ ������
      *NewAdr = 0;                                           //���������� ������
      Adr = NewAdr + 1;
   }
   return 0;
}

//------------------------------------------------------------------------------

int ReadClastEmul(DWORDLONG Poz, BYTE *buff)                 //������ �������� ����������
{
   DWORD nSector = DWORD(Poz / sSecB);
   DWORD nClast = (nSector - Start_SecDir1) / sClSec + 1;
   char Ss[300];
   wsprintf(Ss, "Load Dir (claster %d, sector %d)", nClast, nSector);
   if(FindStr(Ss) < 0)
//     return Error2("�� ������� ������", Ss);
   {  Error3("�� ������� ������", Ss, "��������.");  return 0;  }
   if(Read_Dump(buff, sCl_B) < 0) return -1;
   return 0;
}

//------------------------------------------------------------------------------

int ReadClast2Emul(DWORDLONG Poz, BYTE *buff)                //������ �������� ����������
{
   DWORD nSector = DWORD(Poz / sSecB);
   DWORD nClast = (nSector - Start_SecDir2) / sClSec2 + 1;
   char Ss[300];
   wsprintf(Ss, "Load Dir (claster2 %d, sector %d)", nClast, nSector);
   if(FindStr(Ss) < 0)
//     return Error2("�� ������� ������", Ss);
   { Error3("�� ������� ������", Ss, "��������.");  return 0; }
   if(Read_Dump(buff, sCl2_B) < 0) return -1;
   return 0;
}

//------------------------------------------------------------------------------

#pragma argsused
int ReadFATEmul(DWORDLONG Poz)                               //������ FAT ����������
{
   if(FindStr("Load FAT 1") < 0) return -1;
   if(Read_Dump((BYTE*)FAT1, Size_FAT1) < 0) return -1;
   return 0;
}

//------------------------------------------------------------------------------

#pragma argsused
int ReadFAT2Emul(DWORDLONG Poz)                               //������ FAT ����������
{
   if(FindStr("Load FAT 2") < 0) return -1;
   if(Read_Dump((BYTE*)FAT2, Size_FAT2) < 0) return -1;
   return 0;
}

//-------------------------------------------------------------------------------

int Read_Sec_Emul(int Num, BYTE* Sec)                        //������ ������� �� ������ ����������
{
   char Ss[100];
   sprintf (Ss, "Sector %d", Num);
   if(FindStr(Ss) < 0) return -1;
   if(Read_Dump(Sec, 512) < 0) return -1;
   return 0;
}

//-------------------------------------------------------------------------------

int FindHDD_LG_Emul(void)                                    //����� ����� �� ��������� LG ����������
{
#if !defined EMULATOR_HDD_AND_COPY                           //����� ��������� � ��������� �����������
   EnableWindow(hCopy, FALSE);
#endif
   if(Load_Dump() < 0) return -1;                            //�������� �����

   SEC_0 Sec0;
// if(FindStr("Sector 0") < 0) return -1;
// if(Read_Dump((BYTE*)&Sec0, 512) < 0) return -1;
   if(Read_Sec_Emul(0, (BYTE*)&Sec0) < 0) return -1;         //������ ������� �� ������ ����������
#if defined OUT_TEST
   Add_SpecSpis("Find HDD from LG emulator");
#endif
   SEC_63 Sec63;
   if(FindStr("Sector 63") < 0) return -1;
   if(Read_Dump((BYTE*)&Sec63, 512) < 0) return -1;
   if(strncmp(Sec63.Met, "LGEINC  ", 8) != 0 ||
      strncmp(Sec63.Name, "VOLUMELABE FAT32   ", 19) != 0)
     return Error2("���������� ������� 63 �� ������������� �����������.", "��������.");
#if defined OUT_TEST
   Add_SpecSpis("HDD from LG found");
   Add_SpecSpis("Sector 63");
   View_HEX_Any((BYTE *)&Sec63, 512);
#endif
   if(Sec63.nf != 1 || Sec63.F8 != 0xF8 || Sec63.SecPerCl != sClSec)
     return Error1((Lan+19)->msg);                           //return Error1("������ ������� ���� ������ �������� ������� �� ��, ��� �� �� ��������� LG.");

   Start_SecFAT1 = 63 + Sec63.numRezSec;                     //������� ������ ������� FAT
   Start_SecDir1 = Start_SecFAT1 + Sec63.nSecDir;            //������� ������ ������� ��������
   num_SecFAT1 = Sec63.numSecFAT;                            //����� �������� � FAT
   Size_FAT1 = num_SecFAT1 * sSecB;                          //����� ���� � FAT
// maxZapFAT1 = sSecB / 4 * num_SecFAT1;                     //����� ������� � FAT (�� 4 �����)
   maxZapFAT1 = Sec63.numSecPart / sClSec;                   //������������ ����� ������� � FAT (�� 4 �����)
   SEC_63 Sec64;
   if(FindStr("Sector 64") < 0) return -1;
   if(Read_Dump((BYTE*)&Sec64, 512) < 0) return -1;
#if defined OUT_TEST
   Add_SpecSpis("Sector 64");
   View_HEX_Any((BYTE *)&Sec64, 512);
#endif
   if(strncmp(Sec64.Met, "LGEINC  ", 8) != 0 ||
      strncmp(Sec64.Name, "VOLUMELABE FAT32   ", 19) != 0)
     return Error2("���������� ������� 64 �� ������������� �����������.", "��������.");
   Start_SecDir2 = Sec63.numSecPart + 63 + Sec64.numRezSec + Sec64.nSecDir; //����� ������� ������ �������� �� ������ �������
   num_SecFAT2 = Sec64.numSecFAT;                            //����� �������� � FAT
   Size_FAT2 = num_SecFAT2 * sSecB;                          //����� ���� � FAT
   maxZapFAT2 = sSecB / 4 * num_SecFAT2;                     //����� ������� � FAT (�� 4 �����)
   Start_SecFAT2 = Start_SecFAT1 + Sec0.numClP1;             //������� ������ ������� FAT2
   return 0;
}

#endif

#endif

