
//                                25.01.2009
#ifndef __HDD_LG_WRITE_IO__
#define __HDD_LG_WRITE_IO__

#include "from_hdd_lg_to_pc.h"

#if defined WRITE_YES                                        //����� ������ ��������

//============================ hdd_lg_write_io =================================
int  GetSpNameInFile(char *Spis, int N, int *sm);            //���� ������ ���� ��� ������
int  OpenInFilePC(char *NameF);                              //������� ������� ���� ��� ������ � ����������
int  WriteClast1P(DWORD nClast, BYTE *buff);                 //������ ��������
int  Save_FAT1(void);                                        //���������� ����������� FAT ������� �������
void Ansi_To_Unicode(char *inANSI, WCHAR *outUNI, int maxN); //�������������� ������ �������� � Unicode
int  Load_Dir(void);                                         //�������� �������� ��������
int  Change_Dir_For_File(char *NameF);                       //��������� �������� (�������� ������ �����)
int  Change_Dir_For_Folder(char *NameF);                     //��������� �������� (�������� ����� �����)
int  Save_Dir(void);                                         //���������� �������� ��������
     DWORD *c_FAT1;                                          //����� ������� FAT ������� �������
     HANDLE inFile;
     ULARGE_INTEGER Size_inF;                                //������ ����� �.�. > 4 ��
     int nCl_1;                                              //����� �������� ������ ����� ��� ����� �����
     BYTE bufDir[sCl_B];                                     //������ ��� ���� �������

static FILETIME Time_inF;                                    //���� � ����� �������� �����
static int ns_Dir;                                           //����� ��������� ������ ��������
static One_Str_Cat *s_Kat;                                   //�������

//------------------------------------------------------------------------------

int GetSpNameInFile(char *Spis, int N, int *sm)              //���� ������ ���� ��� ������
{
   OPENFILENAME ofn;
   char DirName[260];                                        //��������� ���� ��� ������ ������
   char FileTitle[256];                                      //��������� ����
   char Filter[256];                                         //������� ������

   *DirName = 0;                                             //�� ������ ���� ����� ������
   lstrcpy(Filter, (Lan+43)->msg);
   switch(prFolder)
   {  case 0: lstrcat(Filter, " mp3, wma|*.mp3;*.wma;|");  break;
      case 1: lstrcat(Filter, " jpg|*.jpg|");  break;
      case 2: //lstrcat(Filter, " avi|*.avi|");  break;
              lstrcat(Filter, " avi, srt, smi, sub, txt, mpg, divx|*.avi;*.srt;*.smi;*.sub;*.txt;*.mpg;*.divx;"); break;
   }
   for(int i=0; Filter[i]!='\0'; i++)                        //�������� ����������� 0
     if(Filter[i] == '|') Filter[i] = '\0';
   ZeroMemory(&ofn, sizeof(OPENFILENAME));                   //��������� ������
   ofn.lStructSize = sizeof(OPENFILENAME);                   //����� ���������
   ofn.hwndOwner = MainWin;                                  //�������� �������
   ofn.hInstance = MainInst;                                 //������������� ��������� ��������� ��������
   ofn.lpstrFilter = Filter;                                 //���� ��������������� ������
// ofn.lpstrCustomFilter                                     //����������� �������
// ofn.nMaxCustFilter                                        //����� ������������ �������
   ofn.nFilterIndex = 1;//prFolder + 1;                      //������ ��� ������ � ���������
   ofn.lpstrFile = Spis;                                     //��� ����� � ������ ������
   ofn.nMaxFile = N;                                         //����� ���� ����� �����
   ofn.lpstrFileTitle = FileTitle;                           //������� � ��� ����� � ������ ������
   ofn.nMaxFileTitle = sizeof(FileTitle);                    //����� ����
   ofn.lpstrInitialDir = DirName;                            //��������� ������� ������
   ofn.lpstrTitle = (Lan+174)->msg;
   ofn.Flags = OFN_ALLOWMULTISELECT |
               OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
   if(GetOpenFileName(&ofn) == FALSE) return -1;
   if(*Spis == 0) return -1;
   *sm = ofn.nFileOffset;                                    //�������� � ����� �����
   return 0;
}
//------------------------------------------------------------------------------

int OpenInFilePC(char *NameF)                                  //������� ������� ���� ��� ������ � ����������
{
   Size_inF.QuadPart = 0;
   inFile = CreateFile(NameF, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(inFile == INVALID_HANDLE_VALUE)
      return ErrorSys2(NameF, (Lan+84)->msg);                //"������ ��� �������� ����� ��� ������.");
   Size_inF.u.LowPart = GetFileSize(inFile, &Size_inF.u.HighPart);
   if(Size_inF.u.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
      return ErrorSys2(NameF, (Lan+85)->msg);                //"������ ��� ������� ������� �����."
   if(Size_inF.QuadPart == 0)
      return Error2(NameF, (Lan+86)->msg);                   //"����������� ����� ������ �����."
   GetFileTime(inFile, NULL, NULL, &Time_inF);               //���� ��������� ������
   return 0;
}

//------------------------------------------------------------------------------

#if !defined EMULATOR_HDD                                    //��� ������ ���������
int WriteClast1P(DWORD nClast, BYTE *buff)                   //������ ��������
{
   DWORD nb;
   if(MaxClast < nClast) MaxClast = nClast;                  //����� ������ �������� �������� ��������������� ��� ������
   DWORD nSector = Start_SecDir1 + (nClast - 1) * sClSec;    //����� ������� �� ������ ��������
   LONGLONG Poz = LONGLONG(sSecB) * nSector;
   if(SetInFilePointer(Poz) < 0) return -1;                  //��������� ������� ��������� �� �����
   if(WriteFile(hDrive, buff, sCl_B, &nb, NULL) == FALSE || nb != sCl_B)
     return ErrorSys1((Lan+175)->msg);                       //"������ ��� ������ ��������."
   return 0;
}
#endif

//-------------------------------------------------------------------------------

#if !defined EMULATOR_HDD                                    //��� ������ ���������
int Save_FAT1(void)                                          //���������� ����������� FAT ������� �������
{
   //�������� ����� ������ �� ��� FAT, � ������ ������������ ������� ��� ���� ������������ ����� FAT (��������� ��������)
   //����������� ������ FAT � ��������� � �������, ��������� ������ � ������ ����������� �������������
   DWORD nb;
   LONGLONG Poz = LONGLONG(sSecB) * Start_SecFAT1;
   if(SetInFilePointer(Poz) < 0) return -1;                  //��������� ������� ��������� � �����
   if(WriteFile(hDrive, c_FAT1, Size_FAT1, &nb, NULL) == FALSE || nb != Size_FAT1)
      return ErrorSys1((Lan+176)->msg);                      //"������ ��� ������ FAT."
   CopyMemory(FAT1, c_FAT1, Size_FAT1);                      //����������� ���������� FAT1
   return 0;
}
#endif

//------------------------------------------------------------------------------

void Ansi_To_Unicode(char *inANSI, WCHAR *outUNI, int maxN)  //�������������� ������ �������� � Unicode
{
   int n = lstrlen(inANSI);
   MultiByteToWideChar(CP_ACP, 0, inANSI, n, outUNI, maxN);
   for(int i=0; i<n; i++) SWAP16((WORD *)(outUNI + i));      //� �������� ����� ������������
   for(int i=n; i<maxN; i++) *(WORD *)(outUNI + i) = 0;      //�������� �����
}

//------------------------------------------------------------------------------

static void NameToKat(char *NameF, char *ExtF, One_Str_Cat *Kat) //������������ ����� �����
{
   int n = lstrlen(NameF);
   if(n > 38)
   {  *(NameF + 38) = 0;                                     //������� ���
       NumMsg38++;
   }
   Ansi_To_Unicode(NameF, Kat->Name, 40);                    //������������� ��� �� UNICODE
   if(*ExtF == '.')                                          //���� ����������
      Ansi_To_Unicode(ExtF+1, Kat->Ext, 4);                  //������������� ���������� �� UNICODE
   else
      for(int i=0; i<4; i++) *(WORD *)(Kat->Ext + i) = 0;    //�������� ��������������
}

//------------------------------------------------------------------------------

static int Add_Item_To_Tree(char *NameF)                     //���������� ������ � ������
{
   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT];

   fnsplit(NameF, Driv, Dir, Name, Ext);
   if((s_Kat+ns_Dir)->pf.type == 48)                         //��� �����
   {  lstrcat(Name, Ext);                                    //� ����� ���������� ��� ��� ��������
      *Ext = *(Ext+1) = 0;
   }
   int l = lstrlen(Name);
   if(l > 38) *(Name + 38) = 0;
   return AddItemToTree_forWrite(Name, Ext+1, &((s_Kat+ns_Dir)->pf));//���������� ����� ������ � ������
}

//------------------------------------------------------------------------------

int Load_Dir(void)                                           //�������� �������� ��������
{
   //������ �������� �������� (�������� ����� � �� ������������ �, � ��� ����� �������� ������ �������� ������)
   //���������� �������� (���������� ������ ������ �����) � � ��� ����� �������� ������ ��������
   DWORD nSector = Start_SecDir1 + (ClStDir - 1) * sClSec;   //����� ������� �� ������ ��������
   if(ReadClast1_P(nSector, bufDir) < 0) return -1;          //������ ��������
   s_Kat = (One_Str_Cat *)bufDir;
   int prPoint = 0;                                          //������� ������� ��������� � ������ ��� ������� ������������ ���������
   ns_Dir = 0;                                               //����� ��������� ������ ��������
   for(; ns_Dir<4*sClSec; ns_Dir++)                          //�� ���� ��������� ������� � ����� �������� �������� (� ������ ������� 4 ������)
   {  if((s_Kat + ns_Dir)->pf.type == 0) break;              //����� ������� ��������
      DWORD NamePoint = *((DWORD*)&(s_Kat + ns_Dir)->Name);
      if(NamePoint == 0x00002E00)                            //��� ���� ����� �.�. ������� �������� ��������
      { if(ns_Dir == 0) prPoint++; continue; }
      if(NamePoint == 0x2E002E00)                            //��� ��� ����� �.�. ������� ����������� ��������
      { if(ns_Dir == 1) prPoint++; continue; }
      WORD NameDel = *((WORD*)&(s_Kat + ns_Dir)->Name);
      if(NameDel == 0xE500 && (s_Kat + ns_Dir)->pf.ClSt == 0) break;//��� ��������� ��� �.�. ��������� ������
   }
   if(prPoint != 2)
      return Error1((Lan+96)->msg);                          //"�������� ��������� ��������."
   if(ns_Dir < 2 || ns_Dir >= 4*sClSec)
      return Error1((Lan+181)->msg);                         //"� �������� ��� ����� ��� ����� ������."
   return 0;
}

//------------------------------------------------------------------------------

int Change_Dir_For_File(char *NameF)                         //��������� �������� (�������� ������ �����)
{
   for(; ns_Dir<4*sClSec; ns_Dir++)                          //�� ���� ��������� ������� � ����� �������� �������� (� ������ ������� 4 ������)
   {  if((s_Kat + ns_Dir)->pf.type == 0) break;              //����� ������� ��������
      WORD NameDel = *((WORD*)&(s_Kat + ns_Dir)->Name);
      if(NameDel == 0xE500 && (s_Kat + ns_Dir)->pf.ClSt == 0) break;//��� ��������� ��� �.�. ��������� ������
   }
   if(ns_Dir >= 4*sClSec)
      return Error1((Lan+181)->msg);                         //"� �������� ��� ����� ��� ����� ������."
   ZeroMemory(s_Kat+ns_Dir, sizeof(One_Str_Cat));            //��� ��������
   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT];

   fnsplit(NameF, Driv, Dir, Name, Ext);
   NameToKat(Name, Ext, s_Kat+ns_Dir);                       //������������ ����� ����� � Unicode
   SYSTEMTIME sysTime;
   FILETIME TimeF;                                           //���� � ����� �������� �����
   FileTimeToLocalFileTime(&Time_inF, &TimeF);
   FileTimeToSystemTime(&TimeF, &sysTime);
   (s_Kat+ns_Dir)->pf.type = 32;                             //��� ����
   (s_Kat+ns_Dir)->pf.ClSt = nCl_1;                          //����� �������� ������ �����
   (s_Kat+ns_Dir)->pf.Sec = sysTime.wSecond;
   (s_Kat+ns_Dir)->pf.Min = sysTime.wMinute;
   (s_Kat+ns_Dir)->pf.Hour = sysTime.wHour;
   (s_Kat+ns_Dir)->pf.Day = sysTime.wDay;
   (s_Kat+ns_Dir)->pf.Mon = sysTime.wMonth;
   (s_Kat+ns_Dir)->pf.Year = sysTime.wYear;
   (s_Kat+ns_Dir)->pf.SizeF = Size_inF.QuadPart;
   Add_Item_To_Tree(NameF);                                  //���������� ������ � ������
   return 0;
}

//------------------------------------------------------------------------------

int Change_Dir_For_Folder(char *NameF)                       //��������� �������� (�������� ����� �����)
{
   for(; ns_Dir<4*sClSec; ns_Dir++)                          //�� ���� ��������� ������� � ����� �������� �������� (� ������ ������� 4 ������)
   {  if((s_Kat + ns_Dir)->pf.type == 0) break;              //����� ������� ��������
      WORD NameDel = *((WORD*)&(s_Kat + ns_Dir)->Name);
      if(NameDel == 0xE500 && (s_Kat + ns_Dir)->pf.ClSt == 0) break;//��� ��������� ��� �.�. ��������� ������
   }
   if(ns_Dir >= 4*sClSec)
      return Error1((Lan+181)->msg);                         //"� �������� ��� ����� ��� ����� ������."
   ZeroMemory(s_Kat+ns_Dir, sizeof(One_Str_Cat));            //��� ��������
   NameToKat(NameF, "", s_Kat+ns_Dir);                       //������������ ����� ����� � Unicode
   SYSTEMTIME sysTime;
   GetLocalTime(&sysTime);
   (s_Kat+ns_Dir)->pf.type = 48;                             //��� �����
   (s_Kat+ns_Dir)->pf.ClSt = nCl_1;                          //����� �������� ������ �����
   (s_Kat+ns_Dir)->pf.Sec = sysTime.wSecond;
   (s_Kat+ns_Dir)->pf.Min = sysTime.wMinute;
   (s_Kat+ns_Dir)->pf.Hour = sysTime.wHour;
   (s_Kat+ns_Dir)->pf.Day = sysTime.wDay;
   (s_Kat+ns_Dir)->pf.Mon = sysTime.wMonth;
   (s_Kat+ns_Dir)->pf.Year = sysTime.wYear;
   Add_Item_To_Tree(NameF);                                  //���������� ������ � ������
   return 0;
}

//------------------------------------------------------------------------------

#if !defined EMULATOR_HDD                                    //��� ������ ���������
int Save_Dir(void)                                           //���������� �������� ��������
{
   if(WriteClast1P(ClStDir, bufDir) < 0) return -1;          //������ �������� ��������
   //����������� ������ � ��������� � ��������, ��������� ������ � ������ ����������� �������������
   return 0;
}

#endif

#endif
#endif

