
//                                14.03.2009
#ifndef __HDD_LG_WRITE_1__
#define __HDD_LG_WRITE_1__

#include "from_hdd_lg_to_pc.h"

#if defined WRITE_YES                                        //����� ������ ��������

//============================ hdd_lg_write_1 ==================================
int  File_ToHDD(void);                                       //������ ������ �� ���� LG
int  New_Folder_ToHDD(void);                                 //C������� ����� �� ����� LG
int  Folder_ToHDD(void);                                     //������ ����� �� ���� LG
     char *spisF;                                            //������ ������ ������

static BYTE bufIO[sCl_B];                                    //������ ��� ���� �������
static int numF_Sp;                                          //����� ���� � ������
static DWORD all_nCl;                                        //����� ����� ��������� � ������
static int writeNewCl;                                       //����� ���������� ���������
static DWORD *pozNameF;                                      //������� �������� � ������ ����� � ������

//------------------------------------------------------------------------------

static int MaloMesta(DWORD nCl)
{
   char ss1[200], ss2[200];
   wsprintf(ss1, "     %s   %s %s.", (Lan+48)->msg, FileSize_Txt(DWORDLONG(maxZapFAT1 - 1 - writeCl) * sCl_B), (Lan+47)->msg);
   wsprintf(ss2, "     %s %s %s.", (Lan+49)->msg, FileSize_Txt(DWORDLONG(nCl) * sCl_B), (Lan+47)->msg);
   return Error3((Lan+190)->msg, ss1, ss2);                  //"������ ����������, �� HDD LG ��� ���������� �����."},
}

//------------------------------------------------------------------------------

static int WriteFile_ToHDD(char *NameF, char *msg)           //������ ����� �� HDD ���������
{
   DWORD nb;

   if(OpenInFilePC(NameF) < 0) return -1;                    //������� ������� ���� ��� ������ � ����������
   DWORD wSizeB = sCl_B;                                     //����� ���� ������ ����� ����� ���� � ��������
   LONGLONG SizeToEnd = Size_inF.QuadPart;                   //����� ���� ������� �������� ��������
   DWORD numCl = DWORD((Size_inF.QuadPart + sCl_B - 1) / sCl_B);//����� ��������� ����������� ��� ���������� ����� ������� �������
   if(numCl > maxZapFAT1 - 1 - writeCl)
      return MaloMesta(numCl);                               //"������ ����������, �� HDD LG ��� ���������� �����."},
   if(numCl > 0)
      InitProgressBar(numCl, msg);
   DWORD n, nCl = 1;                                         //����� ���������� ����������� ��������
   for(DWORD i=0; i<numCl; i++)                              //�� ����� ���������
   {  for(n=nCl+1; n<maxZapFAT1; n++)                        //�������� FAT ��� ������ ������� ���������� ��������
        if(*(c_FAT1 + n) == 0) break;
      if(n >= maxZapFAT1)
         return Error1((Lan+190)->msg);                      //"������ ����������, �� HDD LG ��� ���������� �����."},
      if(wSizeB > SizeToEnd)
      {  wSizeB = DWORD(SizeToEnd);                          //������ ������� ������ ������ ������� ��������
         ZeroMemory(bufIO, sCl_B);
      }
      if(ReadFile(inFile, bufIO, wSizeB, &nb, NULL) == false || nb != wSizeB)
         return ErrorSys1((Lan+87)->msg);                    //"������ ��� ������ �����."
#if !defined EMULATOR_HDD                                    //��� ������ ���������
      if(WriteClast1P(n, bufIO) < 0) return -1;              //������ ��������
#endif
      if(nCl > 1)
        *(c_FAT1 + nCl) = n;                                 //������ �� ��������� ������� � ������� ���������
      else nCl_1 = n;                                        //����� �������� ������ �����
      nCl = n;                                               //��������� ���������� �������
      SizeToEnd -= wSizeB;                                   //����� ���� ������� �������� ���������
      if(ProgressBar(wSizeB) < 0) return -1;                 //�������� ����� ������ ��������
   }
   *(c_FAT1 + nCl) = 0x0FFFFFFF;                             //������� ����� �������
   CloseFile(&inFile);
   writeNewCl += numCl;                                      //����� ���������� ���������
   return Change_Dir_For_File(NameF);                        //��������� �������� (������ ������ �����)
}

//------------------------------------------------------------------------------

static int CopyOne_FileToHDD(char *NameF)                    //������ ������������� ����� �� HDD ���������
{
   //���� ���������, ��� ������ ����� � ����� ���
   c_FAT1 = (DWORD *)MyAllocMem(Size_FAT1);                  //������ ��� FAT
   if(c_FAT1 == NULL)  return -1;
   CopyMemory(c_FAT1, FAT1, Size_FAT1);                      //����������� ���������� FAT1
   if(Load_Dir() < 0) return -1;                             //�������� �������� ��������
   if(WriteFile_ToHDD(NameF, (Lan+24)->msg) < 0) return -1;  //������ ����� �� HDD ���������
#if !defined EMULATOR_HDD                                    //����� ���������
   if(Save_FAT1() < 0) return -1;                            //���������� ����������� FAT ������� �������
   if(Save_Dir() < 0) return -1;                             //���������� �������� ��������
   if(Change_Sec67(-writeNewCl) < 0) return -1;              //�������� ��������� � ���������� ������� 67
#endif
   writeCl += writeNewCl;                                    //����� ���������� ���������
   ViewSize();                                               //������ ��������� ������������
   return 0;
}

//------------------------------------------------------------------------------

static int CopyOneFileToHDD(char *NameF)                     //������ ������������� ����� �� HDD ���������
{
   writeNewCl = 0;                                           //����� ���������� ���������
   InitProgressBar = InitProgressBar1;
   ProgressBar = ProgressBar1;
   Close_ProgressBar = Close_ProgressBar1;
   int ret = CopyOne_FileToHDD(NameF);                       //������ ������������� ����� �� HDD ���������
   Close_ProgressBar();
   MyFreeMem(&(void*)c_FAT1);
   MyFreeMem(&(void*)spisF);
   CloseFile(&inFile);
   return ret;
}

//------------------------------------------------------------------------------

static int Work_Spis(char *SpisF, int sm)                      //�������� ������ ������
{
   numF_Sp = 0;                                              //����� ���� � ������
   pozNameF[numF_Sp] = sm;                                   //������� �������� � ������ ������ (�������� � ����� ������� �����)
   for(;;)                                                   //�� ������ ������
   {  int L = lstrlen(SpisF + pozNameF[numF_Sp]);            //����� �������� � ����� �����
      if(L == 0) break;                                      //������ ��� ��� ����� ������
      numF_Sp++;
      if(numF_Sp >= MAX_NAME_S)                              //������������ ����� ������
        return Error1((Lan+182)->msg);                       //"����� ������ ��� ����������� ��������� ����������� ���������.");
      pozNameF[numF_Sp] = pozNameF[numF_Sp-1] + L + 1;       //������� �������� � ������ ������ (�������� � ����� ���������� �����)
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Mk_AllSize(char *SpisF)                           //���������� ������ ����� ��������� ������
{
   char NameF[260];
   all_nCl = 0;
   for(int i=0; i<numF_Sp; i++)                              //�� ����� ���� � ������
   {  lstrcpy(NameF, SpisF);                                 //��������� ��� ��������
      lstrcat(NameF, "\\");                                  //�������� �����������
      lstrcat(NameF, SpisF + pozNameF[i]);                   //�������� ��� �����
      int ret = OpenInFilePC(NameF);                         //������� ������� ���� ��� ������ � ����������
      CloseFile(&inFile);
      if(ret < 0) return -1;
      all_nCl += DWORD((Size_inF.QuadPart + sCl_B - 1) / sCl_B);//����� ��������� ����������� ��� ���������� ���� ������
   }
   if(all_nCl > maxZapFAT1 - 1 - writeCl)
      return MaloMesta(all_nCl);                             //"������ ����������, �� HDD LG ��� ���������� �����."},
   return 0;
}

//------------------------------------------------------------------------------

static int CopyAll_FileToHDD(char *SpisF)                    //������ ������ ����� �� HDD ���������
{
   char NameF[260], Ss[100];
   //���� ���������, ��� ������ ����� � ����� ���
   InitProgressBar2_2(all_nCl, "");                          //������� ��������� ��� ���������� ������
   c_FAT1 = (DWORD *)MyAllocMem(Size_FAT1);                  //������ ��� FAT
   if(c_FAT1 == NULL)  return -1;
   CopyMemory(c_FAT1, FAT1, Size_FAT1);                      //����������� ���������� FAT1
   if(Load_Dir() < 0) return -1;                             //�������� �������� ��������
   for(int i=0; i<numF_Sp; i++)                              //�� ����� ���� � ������
   {  lstrcpy(NameF, SpisF);                                 //��������� ��� ��������
      lstrcat(NameF, "\\");                                  //�������� �����������
      lstrcat(NameF, SpisF + pozNameF[i]);                   //�������� ��� �����
      sprintf(Ss, "%s:   %d  ( %d )", (Lan+24)->msg, i+1, numF_Sp);
      int ret = WriteFile_ToHDD(NameF, Ss);                  //������ ����� �� HDD ���������
      CloseFile(&inFile);
      if(ret < 0) return -1;
      Close_ProgressBar();
   }
#if !defined EMULATOR_HDD                                    //����� ���������
   if(Save_FAT1() < 0) return -1;                            //���������� ����������� FAT ������� �������
   if(Save_Dir() < 0) return -1;                             //���������� �������� ��������
   if(Change_Sec67(-writeNewCl) < 0) return -1;              //�������� ��������� � ���������� ������� 67
#endif
   writeCl += writeNewCl;                                    //����� ���������� ���������
   ViewSize();                                               //������ ��������� ������������
   return 0;
}

//------------------------------------------------------------------------------

static int Copy_File_ToHDD(int sm)                           //������ ���������� ������ �� HDD ���������
{
   writeNewCl = 0;                                           //����� ���������� ���������
   if(Work_Spis(spisF, sm) < 0) return -1;                   //�������� ������ ���������� ������ �� HDD ���������
   if(Mk_AllSize(spisF) < 0) return -1;                      //���������� ������ ����� ��������� ������
   InitProgressBar = InitProgressBar1_2;                     //������� ��������� ��� ������ ����� �� ������
   ProgressBar = ProgressBar2;
   Close_ProgressBar = Close_ProgressBar1_2;
   int ret = CopyAll_FileToHDD(spisF);                       //������ ������ ����� �� HDD ���������
   Close_ProgressBar2_2();
   MyFreeMem(&(void*)c_FAT1);
   MyFreeMem(&(void*)spisF);
   return ret;
}

//------------------------------------------------------------------------------

int File_ToHDD(void)                                         //������ ������ �� ���� LG
{
   int sm;
   spisF = (char *)MyAllocMem(MAX_SP + MAX_NAME_S*sizeof(DWORD));
   if(spisF == NULL) return -1;                              //��� ������
   pozNameF = (DWORD*)(spisF + MAX_SP);                      //������� �������� � ������ ����� � ������
   *spisF = 0;
   int ret = GetSpNameInFile(spisF, MAX_SP, &sm);            //���� ������ ���� ��� ������
   if(ret != 0)  return ret;
   if(*(spisF+sm-1) != 0)                                    //�������� ��� ������������ ��� ��� ������
      return CopyOneFileToHDD(spisF);                        //������ ������������� ����� �� HDD ���������
   return Copy_File_ToHDD(sm);                               //������ ���������� ������ �� HDD ���������
}

//------------------------------------------------------------------------------

static int CreateNewFolder(void)                             //�������� ����� ����� �� HDD ���������
{
   DWORD nCl = 2;
   for(; nCl<maxZapFAT1; nCl++)                              //�������� FAT ��� ������ ������� ���������� ��������
      if(*(c_FAT1 + nCl) == 0) break;
   if(nCl >= maxZapFAT1)
      return Error1((Lan+190)->msg);                         //"������ ����������, �� HDD LG ��� ���������� �����."},
   ZeroMemory(bufIO, sCl_B);                                 //��� ��������
   SYSTEMTIME sysTime;
   GetLocalTime(&sysTime);
   One_Str_Cat *Kat = (One_Str_Cat *)bufIO;                  //�������
   *(DWORD *)(Kat+0)->Name = 0x00002E00;                     //��� ���� ����� �.�. ������� �������� ��������
   *(DWORD *)(Kat+1)->Name = 0x2E002E00;                     //��� ��� ����� �.�. ������� ����������� ��������
   (Kat+0)->pf.type = (Kat+1)->pf.type = 48;                 //��� �����
   (Kat+0)->pf.ClSt = nCl_1 = nCl;                           //����� �������� �������� ��������
   (Kat+1)->pf.ClSt = ClStDir;                               //����� �������� ������������� ��������
   (Kat+0)->pf.Sec  = (Kat+1)->pf.Sec  = sysTime.wSecond;
   (Kat+0)->pf.Min  = (Kat+1)->pf.Min  = sysTime.wMinute;
   (Kat+0)->pf.Hour = (Kat+1)->pf.Hour = sysTime.wHour;
   (Kat+0)->pf.Day  = (Kat+1)->pf.Day  = sysTime.wDay;
   (Kat+0)->pf.Mon  = (Kat+1)->pf.Mon  = sysTime.wMonth;
   (Kat+0)->pf.Year = (Kat+1)->pf.Year = sysTime.wYear;
#if !defined EMULATOR_HDD                                    //����� ���������
   if(WriteClast1P(nCl, bufIO) < 0) return -1;               //������ ��������
#endif
   *(c_FAT1 + nCl) = 0x0FFFFFFF;                             //������� ����� �������
   return Change_Dir_For_Folder(NameFoFi);                   //��������� �������� (������ ����� �����)
}

//------------------------------------------------------------------------------

static int Create_New_Folder_ToHDD(void)                     //C������� ����� �� ����� LG
{
   if(*NameFoFi == 0)
     return Error1((Lan+184)->msg);                          //"������������ ���"
   c_FAT1 = (DWORD *)MyAllocMem(Size_FAT1);                  //������ ��� FAT
   if(c_FAT1 == NULL)  return -1;
   CopyMemory(c_FAT1, FAT1, Size_FAT1);                      //����������� ���������� FAT1
   if(Load_Dir() < 0) return -1;                             //�������� �������� ��������
   if(CreateNewFolder() < 0) return -1;                      //�������� ����� ����� �� HDD ���������
#if !defined EMULATOR_HDD                                    //����� ���������
   if(Save_FAT1() < 0) return -1;                            //���������� ����������� FAT ������� �������
   if(Save_Dir() < 0) return -1;                             //���������� �������� ��������
   if(Change_Sec67(-1) < 0) return -1;                       //�������� ��������� � ���������� ������� 67
#endif
   writeCl++;                                                //����� ���������� ���������
   ViewSize();                                               //������ ��������� ������������
   MyFreeMem(&(void*)c_FAT1);
   MyFreeMem(&(void*)spisF);
   return 0;
}

//------------------------------------------------------------------------------

int New_Folder_ToHDD(void)                                   //C������� ����� �� ����� LG
{
   if(DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_NAME),
                MainWin, Dlg_NewName, LPARAM(0)) == IDCANCEL) return -1;
   return Create_New_Folder_ToHDD();                         //C������� ����� �� ����� LG
}

//------------------------------------------------------------------------------

typedef char OneExt[5];
static OneExt ExtEn[] =  { ".mp3", ".wma",
                           ".jpg",
                           ".avi", ".srt", ".smi", ".sub", ".txt", ".mpg", ".divx" };
static int nExt[3] = {2, 1, 7};                              //����� ���������� � ������
static int pExt[3] = {0, 2, 3};                              //������� ������� ���������� � ������

static int Make_Spis(int *n)                                 //�������� ������ ������
{
   char NameF[256];
   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT];

   WIN32_FIND_DATA Data;
   HANDLE File;

   int l = lstrlen(spisF) + 1;
   lstrcpy(NameF, spisF);
   lstrcat(NameF, "\\*.*");
   if((File = FindFirstFile(NameF, &Data)) == INVALID_HANDLE_VALUE)  return -1; //���� �� ������ (����� ������)
   for(;;)
   {
      fnsplit(Data.cFileName, Driv, Dir, Name, Ext);
      if(!(lstrcmp(Data.cFileName, ".") == 0 ||              //��� �����  "." ��� �� ����������
           lstrcmp(Data.cFileName, "..") == 0))              //��� �����  ".." ��� �� ����������
      {  //lstrcpy(NameF+l, Data.cFileName);                 //������� ������ ��� � �����
         if((Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) //������� ��� ��� ��������� �����
         {
   //���� ������ ����� ���� ������ �����, �� ��������� ��� ��� ���������� �����, �.�. ������ ��� ������
         }
         else
         {  OneExt *ExtE = ExtEn + pExt[prFolder];
            for(int i=0; i<nExt[prFolder]; i++)
              if(lstrcmpi(ExtE[i], Ext) == 0)
              {  lstrcpy(spisF+l, Data.cFileName);
                 l += lstrlen(spisF+l) + 1;
                 (*n)++;
                 break;
              }
         }
      }
      if(FindNextFile(File, &Data) == 0)
        if(GetLastError() == ERROR_NO_MORE_FILES) break;
   }
   FindClose(File);
   *(spisF+l) = 0;
   return 0;
}

//------------------------------------------------------------------------------

int Folder_ToHDD(void)                                       //������ ����� �� ���� LG
{
   char NameDir[256], Driv[MAXDRIVE], Dir[MAXPATH], Ext[MAXEXT];

   if(Get_Name_Dir((Lan+186)->msg, NameDir, 1) < 0) return -1;//������ ����� �����
   fnsplit(NameDir, Driv, Dir, NameFoFi, Ext);
   lstrcat(NameFoFi, Ext);                                   //� ����� ���������� ��� ��� ��������
   if(Create_New_Folder_ToHDD() < 0) return -1;              //C������� ����� �� ����� LG
   ClStDir = nCl_1;                                          //����� �������� �������� � ������� ����������� ������, �.�. ����� ��������� �������
   spisF = (char *)MyAllocMem(MAX_SP + MAX_NAME_S * sizeof(DWORD));
   if(spisF == NULL) return -1;                              //��� ������
   pozNameF = (DWORD*)(spisF + MAX_SP);                      //������� �������� � ������ ����� � ������
   lstrcpy(spisF, NameDir);
   int sm = lstrlen(spisF) + 1;
   int n = 0;
   if(Make_Spis(&n) < 0) return -1;                          //�������� ������ ������
   if(n == 0) return 0;                                      //��� �� ������ ����� � ������
   return Copy_File_ToHDD(sm);                               //������ ���������� ������ �� HDD ���������
}

#endif
#endif

