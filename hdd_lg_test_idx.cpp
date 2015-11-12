
//                                    11.08.2008
#ifndef __HDD_LG_TEST_IDX__
#define __HDD_LG_TEST_IDX__

#include "from_hdd_lg_to_pc.h"

#if defined TEST_IDX

//============================ hdd_lg_test_idx =================================
int Read_File_idx(void);

typedef struct { DWORD A[12];} FILE_IDX;
static FILE_IDX *inBufIdx;
static DWORD inSizeIdx;

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
   inSizeIdx = GetFileSize(*inFile, NULL);
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
   lstrcpy(Filter, "����� idx|*.idx|��� ����� (*.*)|*.*|");
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
   ofn.lpstrDefExt = "idx";                                  //���������� �� ���������
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

//-------------------------------------------------------------------------------

int Read_File_idx(void)
{
   HANDLE inFile;
   char Name_F_IDX[260];
   DWORD nb;
   if(GetName_InFile(Name_F_IDX) < 0) return -1;              //���� ����� ��������������� �����
   if(OpenInFile(Name_F_IDX, &inFile) < 0) return -1;         //�������� �������� �����
   inBufIdx = (FILE_IDX *)MyAllocMem(inSizeIdx);
   if(inBufIdx == NULL) return -1;
   if(ReadFile(inFile, inBufIdx, inSizeIdx, &nb, NULL) == FALSE || nb != inSizeIdx)
      return ErrorSys2("������ ��� ������ ��������� �����", Name_F_IDX);
   CloseFile(&inFile);                                       //������� ������� ����

   char Ss[1024], *as;
   FILE_IDX *aIn = inBufIdx;
   for(DWORD i=0; i<inSizeIdx / sizeof(FILE_IDX); i++, aIn++)
   {   as = Ss;
       for(DWORD j=0; j<12; j++)
       {  SWAP32(&(aIn->A[j]));
          as += sprintf(as, " %08X", aIn->A[j]);
       }
       *as = 0;
       Add_Spis(Ss);                                             //���������� ������ � ������
   }
   return 0;
}

#endif

#endif

