
//                                    07.12.2008
#ifndef __HDD_LG_FUNC_IO__
#define __HDD_LG_FUNC_IO__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_func_io ==================================
void ClearDopDlg(void);                                      //������� ���������� ��������������� �������
int  SetInFilePointer(LONGLONG Poz);                         //��������� ������� ��������� � ����� (�� ���������� �����)
void CloseFile(HANDLE *File);                                //������� ���� (���������� ����)
int  GetName_OutFile(char *NameF);                           //���� ����� ��� ������ �����
int  CtrlFileYesNo(char *NameF);                             //�������� ������� �����
int  DiskFreeSpaceEx_F(char *Name_Dir, DWORDLONG Size);      //�������� ���������� �����
int  Open_OutFileAlw(char *Name, HANDLE *File);              //�������� ��������� �����
//int  Open_FileR(char *Name, HANDLE *File, int *nn);          //�������� ������������� �����
//11int  Open_OutFile512(char *Name, HANDLE *File);          //�������� ��������� �����
int  Open_OutFile(char *Name, HANDLE *File);                 //�������� ��������� �����
int  DiskFreeSpaceEx(char *NameF, DWORDLONG Size);           //�������� ���������� �����
     ModifDialog ModDlg;                                     //��� ����������� �������
     WIN32_FIND_DATA DataF;                                  //��������� ����� ���� �� ����

//------------------------------------------------------------------------------

void ClearDopDlg(void)                                       //������� ���������� ��������������� �������
{
   ModDlg.Flags = 0;                                         //�������������� �����
   ModDlg.fnHook = 0;                                        //������� ��������� ��������������� �������
   ModDlg.DopDlgName = 0;                                    //�������������� ������ ������� ����������� � ��������
}

//------------------------------------------------------------------------------

int SetInFilePointer(LONGLONG Poz)                           //��������� ������� ��������� � ����� (�� ���������� �����)
{
   LARGE_INTEGER TPoz;

   TPoz.QuadPart = Poz;
   TPoz.u.LowPart = SetFilePointer(hDrive, TPoz.u.LowPart, &TPoz.u.HighPart, FILE_BEGIN);
   if(TPoz.u.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
      return ErrorSys1((Lan+41)->msg);                       //"������ ��� ���������������� �� �����."
   return 0;
}

//------------------------------------------------------------------------------

void CloseFile(HANDLE *File)                                 //������� ���� (���������� ����)
{
   if(*File == 0) return;
   CloseHandle(*File);
   *File = 0;
}

//------------------------------------------------------------------------------

int GetName_OutFile(char *NameF)                             //���� ����� ��� ������ �����
{
   OPENFILENAME ofn;
   char DirName[260];                                        //��������� ���� ��� ������ ������
   char FileTitle[260];                                      //��������� ����
   char Filter[260];                                         //������� ������
   char *Title = (Lan+42)->msg;                              //"������� ��� ������������� �����"
   char NameFile[260];
   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT];

   strcpy(NameFile, NameF);                                  //��������� ���� ��� ������ ������
   *DirName = 0;                                             //�� ������ ���� ����� ������
   if(*NameFile != 0)                                        //��� ��� ����
   {  fnsplit(NameFile, Driv, Dir, Name, Ext);
      fnmerge(DirName, Driv, Dir, NULL, NULL);
      fnmerge(NameFile, NULL, NULL, Name, Ext);
      if(*Name == '*') *Name = 0;                            //���� ��� ����������� �����
   }
   sprintf(Filter, "%s *%s|*%s|%s (*.*)|*.*", (Lan+43)->msg, Ext, Ext, (Lan+44)->msg); //������� ������ //sprintf(Filter, "����� *%s|*%s|��� ����� (*.*)|*.*", Ext, Ext);
   for(int i=0; Filter[i]!='\0'; i++)                        //�������� ����������� 0
     if(Filter[i] == '|') Filter[i] = '\0';
   ZeroMemory(&ofn, sizeof(OPENFILENAME));                   //��������� ������
   ofn.lStructSize = sizeof(OPENFILENAME);                   //����� ���������
   ofn.hwndOwner = MainWin;                                  //�������� �������
   ofn.hInstance = MainInst;                                 //������������� ��������� ��������� ��������
   ofn.lpstrFilter = Filter;                                 //���� ��������������� ������
//     lpstrCustomFilter                                     //����������� �������
//     nMaxCustFilter                                        //����� ������������ �������
   ofn.nFilterIndex = 1;                                     //������ ��� ������ � ���������
   ofn.lpstrFile = NameFile;                                 //��� ����� � ������ ������
   ofn.nMaxFile = sizeof(NameFile);                          //����� ���� ����� �����
   ofn.lpstrFileTitle = FileTitle;                           //����� ���������
   ofn.nMaxFileTitle = sizeof(FileTitle);                    //
   ofn.lpstrInitialDir = DirName;                            //��������� ������� ������
   ofn.lpstrTitle = Title;
   ofn.Flags = ModDlg.Flags | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
// ofn.nFileOffset;                                          //�������� � ������ lpstrFile
// ofn.nFileExtension;                                       //�������� � ������ lpstrFile
// ofn.lpstrDefExt = ".*";
// ofn.lCustData;                                            //��� ������� ������������
   ofn.lpfnHook = ModDlg.fnHook;                             //������� ��������� ��������������� �������
   ofn.lpTemplateName = ModDlg.DopDlgName;                   //�������������� ������ ������� ����������� � ��������
   if(GetSaveFileName(&ofn) == FALSE || *NameFile == 0)      //������� �������
   {   *NameF = 0;    return -1;  }
   strcpy(NameF, NameFile);                                  //��������� ��� �����
   return 0;
}

//------------------------------------------------------------------------------

int CtrlFileYesNo(char *NameF)                               //�������� ������� �����
{
// WIN32_FIND_DATA DataF;
   HANDLE  File;

   if((File = FindFirstFile(NameF, &DataF)) == INVALID_HANDLE_VALUE) return 0; //����� ���                                              //���� �� ������
   FindClose(File);
   return 1;                                                 //���� ����
}

//------------------------------------------------------------------------------

int Open_OutFileAlw(char *Name, HANDLE *File)                //�������� ��������� �����
{
  *File = CreateFile(Name,                                   //����� ������ � ������ �����
                     GENERIC_WRITE,                          //������ ������� � �����
                     0,                                      //����������� ������ �� ������� �������
                     NULL,                                   //����� ��������� � ����������� ������ ����
                     CREATE_ALWAYS,                          //��� �������� �����
                     FILE_ATTRIBUTE_NORMAL,                  //�������� �����
                     NULL);                                  //������������ ��� ��������� ������
   if(*File == INVALID_HANDLE_VALUE)
   {  *File = NULL;
      return ErrorSys2((Lan+82)->msg, Name);                 //"������ ��� �������� ��������� �����"
   }
   return 0;
}

//------------------------------------------------------------------------------
/*
int Open_FileR(char *Name, HANDLE *File, int *nn)            //�������� ������������� �����
{
  *File = CreateFile(Name,                                   //����� ������ � ������ �����
                     GENERIC_READ | GENERIC_WRITE,           //������ ������� � �����
                     0,                                      //����������� ������ �� ������� �������
                     NULL,                                   //����� ��������� � ����������� ������ ����
                     OPEN_EXISTING,                          //��� �������� �����
                     FILE_ATTRIBUTE_NORMAL,                  //�������� �����
                     NULL);                                  //������������ ��� ��������� ������
   if(*File == INVALID_HANDLE_VALUE)
   {  *File = NULL;
      if(GetLastError() == 32)                               //���� ��� �����
      { (*nn)++;
        Sleep(250);                                          //�������� �� 250 ��
        if(*nn < 100)                                        //�������, ��� �� 25 ������ ���� �����������
           return Open_FileR(Name, File, nn);                //��� ��� �������� ������������� �����
      }
      return ErrorSys2((Lan+82)->msg, Name);                 //"������ ��� �������� ��������� �����"
   }
   return 0;
}
*/
//------------------------------------------------------------------------------

//11int Open_OutFile512(char *Name, HANDLE *File)                //�������� ��������� �����
int Open_OutFile(char *Name, HANDLE *File)                //�������� ��������� �����
{
  *File = CreateFile(Name,                                   //����� ������ � ������ �����
                     GENERIC_WRITE,                          //������ ������� � �����
                     0,                                      //����������� ������ �� ������� �������
                     NULL,                                   //����� ��������� � ����������� ������ ����
                     CREATE_NEW,                             //��� �������� ����� (�� ������ ����)
//11                 FILE_FLAG_NO_BUFFERING |
           //        FILE_FLAG_SEQUENTIAL_SCAN |
                     FILE_ATTRIBUTE_NORMAL,                  //�������� �����
                     NULL);                                  //������������ ��� ��������� ������
   if(*File == INVALID_HANDLE_VALUE)
   {  *File = NULL;
      return ErrorSys2((Lan+82)->msg, Name);                 //"������ ��� �������� ��������� �����"
   }
   return 0;
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL CALLBACK Dlg_FileSize(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch(Message)
   {
      case WM_INITDIALOG:
             CenterDlg(hDlg, 0);                             //������������� ���� ������� � ������� ����
             SetWindowText(hDlg, (Lan+36)->msg);             //������ ����� ���������  ��������!
             SetDlgItemText(hDlg, IDC_STATICTEXT1, (Lan+50)->msg); //�� ����� ��������� ��� ������ ������������ ���������� �����.
             SetDlgItemText(hDlg, IDC_STATICTEXT3, (Lan+48)->msg); //��������
             SetDlgItemText(hDlg, IDC_STATICTEXT4, FileSize_Txt(*(DWORDLONG*)(lParam)));
             SetDlgItemText(hDlg, IDC_STATICTEXT5, (Lan+47)->msg); //����
             SetDlgItemText(hDlg, IDC_STATICTEXT6, (Lan+49)->msg); //���������
             SetDlgItemText(hDlg, IDC_STATICTEXT7, FileSize_Txt(*((DWORDLONG*)(lParam) + 1)));
             SetDlgItemText(hDlg, IDC_STATICTEXT8, (Lan+47)->msg); //����
             SetDlgItemText(hDlg, IDC_STATICTEXT9, (Lan+52)->msg); //������ ������� ������ ���� ?
             SetDlgItemText(hDlg, IDOK, (Lan+45)->msg);            //��
             SetDlgItemText(hDlg, IDCANCEL, (Lan+51)->msg);        //���
             return TRUE;
      case WM_CTLCOLORSTATIC:
             if(GetDlgCtrlID(HWND(lParam)) == IDC_STATICTEXT1)
             {  SetTextColor((HDC)wParam, RGB(255, 0, 0));
                SetBkColor((HDC)wParam, FonLTGRAY);
                return (BOOL)FonBrush;
             }
             return TRUE;
      case WM_COMMAND:
             switch(LOWORD(wParam))
             {  case IDOK:
                case IDCANCEL: EndDialog(hDlg, LOWORD(wParam));    //������� ���� �� ���� �����
                               return TRUE;
             }
             break;
   }
   return FALSE;
}

//------------------------------------------------------------------------------

int DiskFreeSpaceEx(char *NameF, DWORDLONG Size)             //�������� ���������� �����
{
    ULARGE_INTEGER FreeBytes, TotalBytes, TotalFreeBytes;
    char   Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT];
    char   Path[MAXPATH];

    fnsplit(NameF, Driv, Dir, Name, Ext);
    lstrcpy(Path, Driv);
    lstrcat(Path, "\\");

    if(GetDiskFreeSpaceEx(Path, &FreeBytes,                  //����� ��������� ��������� ���� (������������ ������ �����)
                                &TotalBytes,                 //����� ����� ���� �� �����
                                &TotalFreeBytes) == FALSE)   //����� ����� ��������� ���� �� ����� (����� ���� ��������� ����)
       return ErrorSys1((Lan+46)->msg);                      //"��������� ������ ��� ������� ������� �����."
    if(FreeBytes.QuadPart < Size)
    {  DWORDLONG size[2];
       size[0] = FreeBytes.QuadPart;
       size[1] = Size;
       if(DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_FILE_SIZE),
          MainWin, Dlg_FileSize, LPARAM(size)) == IDCANCEL) return -1;
       return 1;
    }
    if(Size <= 0xFFFFFFFF-2) return 0;                       //���������� ������ ��� FAT32 � ��� ���� ������

    char FileSystemName[260];
    GetVolumeInformation(Path, NULL, 0, NULL, 0, NULL,
                         FileSystemName, sizeof(FileSystemName));
    if(lstrcmp(FileSystemName, "FAT32") != 0) return 0;
  //if(MsgYesNo3("��� ������ ������ ���� � �������� �������� FAT32",
  //             "������������ ������ ����� � ������� 4�� - 2",
  //             "������ ������� ������ ���� ?") == 'N') return -1;
    if(MsgYesNo3((Lan+53)->msg, (Lan+54)->msg, (Lan+55)->msg) == 'N') return -1;
    return 1;                                                //����� ���
}

//------------------------------------------------------------------------------

int DiskFreeSpaceEx_F(char *Name_Dir, DWORDLONG Size)        //�������� ���������� �����
{
   ULARGE_INTEGER FreeBytes, TotalBytes, TotalFreeBytes;
   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT], Path[MAXPATH];
   fnsplit(Name_Dir, Driv, Dir, Name, Ext);
   lstrcpy(Path, Driv);
   lstrcat(Path, "\\");
   if(GetDiskFreeSpaceEx(Path, &FreeBytes, &TotalBytes, &TotalFreeBytes) == FALSE)//����� ����� ��������� ���� �� ����� (����� ���� ��������� ����)
       return ErrorSys1((Lan+46)->msg);                      //"��������� ������ ��� ������� ������� �����."

   if(TotalFreeBytes.QuadPart >= Size)  return 0;
   DWORDLONG size[2];
   size[0] = TotalFreeBytes.QuadPart;
   size[1] = Size;
   if(DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_FILE_SIZE),
          MainWin, Dlg_FileSize, LPARAM(size)) == IDCANCEL) return -1;
   return 1;
}

#endif

