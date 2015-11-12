
//                                    21.11.2009
#ifndef __HDD_LG_FUNC_IO_1__
#define __HDD_LG_FUNC_IO_1__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_func_io_1 ================================
//11int  OpenOutFile(char *Name, LONGLONG Size, HANDLE *File);   //�������� ��������� �����
void Ctrl_Name_Ch(char *Name);                               //�������� ����� �� ������������ ������� � ������ ������������ ��������
int  OpenOutFileName_New(char *Name, LONGLONG Size, HANDLE *File);//�������� ��������� �����
int  OpenOutFileName_Yes(char *Name, LONGLONG Size, HANDLE *File);//�������� ��������� �����
     int prCopy;                                             //������� �������� ��� ��������� �����������
     char ndS[9] = { "/\\:*?\"<>|" };                        //�������� ������������ ��������
//   int  pr_ndS;                                            //������� ����������� ������ ������������� �������. 1-���� ������

//------------------------------------------------------------------------------

#pragma argsused
static BOOL CALLBACK Dlg_Warning(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch(Message)
   {
      case WM_INITDIALOG:
             CenterDlg(hDlg, -80);                           //������������� ���� ������� � ������� ����
             SetWindowText(hDlg, (Lan+36)->msg);             //������ ����� ���������
             OutNameDlg(hDlg, IDC_STATICTEXT1, LPSTR(lParam));     //�������� ����� �����
             SetDlgItemText(hDlg, IDC_STATICTEXT2, (Lan+65)->msg);
             SetDlgItemText(hDlg, IDC_REWRITE, (Lan+66)->msg);
             SetDlgItemText(hDlg, IDC_NEW, (Lan+67)->msg);
             SetDlgItemText(hDlg, IDCANCEL, (Lan+68)->msg);
             return TRUE;
      case WM_CTLCOLORSTATIC:
             if(GetDlgCtrlID(HWND(lParam)) == IDC_STATICTEXT2)
             {  SetTextColor((HDC)wParam, RGB(255, 0, 0));
                SetBkColor((HDC)wParam, FonLTGRAY);
                return (BOOL)FonBrush;
             }
             return TRUE;
      case WM_COMMAND:
           switch(LOWORD(wParam))
           {  case IDC_REWRITE:
              case IDC_NEW:
              case IDCANCEL:    EndDialog(hDlg, LOWORD(wParam));  //������� ���� �� ���� �����
                                return TRUE;
           }
           break;
   }
   return FALSE;
}

//------------------------------------------------------------------------------

static int Ctrl1FileYes(char *NameF)                         //�������� ������� �����
{
   if(CtrlFileYesNo(NameF) == 0) return 0;                   //����� ��� - �������� ������� �����
   int ret = DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_F_YESNO),
                   MainWin, Dlg_Warning, LPARAM(NameF));
   if(ret == IDCANCEL) { *NameF = 0;  return -1; }           //������ ������
   if(ret == IDC_NEW) return 1;                              //������ ������ ���� ������ ����� �����
   if(DeleteFile(NameF) != 0) return 0;
   return ErrorSys2((Lan+56)->msg, NameF);                   //"������ ��� �������� �����"
}

//------------------------------------------------------------------------------

static void TimeFileCopy(SYSTEMTIME *sysTime)                //����� ������ �����
{
   sysTime->wYear = (aTree+indF)->pf.Year;
   sysTime->wMonth = (aTree+indF)->pf.Mon;
   sysTime->wDay = (aTree+indF)->pf.Day;
   sysTime->wHour = (aTree+indF)->pf.Hour;
   sysTime->wMinute = (aTree+indF)->pf.Min;
   sysTime->wSecond = (aTree+indF)->pf.Sec;
   sysTime->wDayOfWeek = 0;
   sysTime->wMilliseconds = 0;
}

//------------------------------------------------------------------------------

static int Comp_TimeF(void)                                  //��������� ������� �������� ������
{
   SYSTEMTIME sysTime;
   FILETIME fTime, flTime;
   TimeFileCopy(&sysTime);                                   //����� ������ �����
   SystemTimeToFileTime(&sysTime, &flTime);                  //������������� �����
   LocalFileTimeToFileTime(&flTime, &fTime);
   return (CompareFileTime(&DataF.ftLastWriteTime, &fTime) == -1) ? IDOK : IDC_SCIP; //�������� � ����������� �� ������� ������ �����
}

//------------------------------------------------------------------------------

static int Mk_NewName(char *NameF)                           //�������������� ������������ ������ �����
{
   char Driv[MAXDRIVE+3], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT];
   fnsplit(NameF, Driv, Dir, Name, Ext);
   int l = lstrlen(Name);
   for(DWORD i=1; i<0xFFFFFFFF; i++)
   {  sprintf(Name+l, ".%03d", i);
      fnmerge(NameF, Driv, Dir, Name, Ext);
      if(CtrlFileYesNo(NameF) == 0) return 0;                //����� ��� - �������� ������� �����
  }
  return 0;
}

//------------------------------------------------------------------------------

static void View_FilePar(HWND hDlg, SYSTEMTIME *sysTime, DWORDLONG SizeF, int IDC1, int IDC2)
{
   char Ss[100], dd[48], sDate[64], sTime[64];

   GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, sysTime, NULL, sDate, 64);
   GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, sysTime, NULL, sTime, 64);
   sprintf(dd, "%12.0f", double(SizeF));
   wsprintf(Ss, "%s  ", Char_Dig_p(dd, 15));
   SetDlgItemText(hDlg, IDC1, Ss);
   wsprintf(Ss, "%s   %s", sDate, sTime);
   SetDlgItemText(hDlg, IDC2, Ss);
}

//------------------------------------------------------------------------------

static void ViewOldFilePar(HWND hDlg)                        //��������� ������� �����
{
   FILETIME TimeFile;
   ULARGE_INTEGER SizeFile;
   SYSTEMTIME sysTime;
   SizeFile.u.HighPart = DataF.nFileSizeHigh;
   SizeFile.u.LowPart = DataF.nFileSizeLow;
   FileTimeToLocalFileTime(&DataF.ftLastWriteTime, &TimeFile);
   FileTimeToSystemTime(&TimeFile, &sysTime);
   View_FilePar(hDlg, &sysTime, SizeFile.QuadPart, IDC_STATICTEXT4, IDC_STATICTEXT6);
}

//------------------------------------------------------------------------------

static void ViewNewFilePar(HWND hDlg)                        //��������� ������ �����
{
   SYSTEMTIME sysTime;
   TimeFileCopy(&sysTime);                                   //����� ������ �����
   View_FilePar(hDlg, &sysTime, (aTree+indF)->pf.SizeF, IDC_STATICTEXT8, IDC_STATICTEXT10);
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL CALLBACK Dlg_FileYes(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch(Message)
   {  case WM_INITDIALOG:
             CenterDlg(hDlg, 0);                             //������������� ���� ������� � ������� ����
             SetWindowText(hDlg, (Lan+36)->msg);             //������ ����� ���������
             OutNameDlg(hDlg, IDC_STATICTEXT1, LPSTR(lParam)); //�������� ����� �����
             SetDlgItemText(hDlg, IDC_STATICTEXT2, (Lan+65)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT3, (Lan+156)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT5, (Lan+47)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT7, (Lan+157)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT9, (Lan+47)->msg);
             SetDlgItemText(hDlg, IDOK,            (Lan+164)->msg);
             SetDlgItemText(hDlg, IDC_REP_ALL,     (Lan+158)->msg);
             SetDlgItemText(hDlg, IDC_REP_OLD,     (Lan+159)->msg);
             SetDlgItemText(hDlg, IDC_SCIP,        (Lan+160)->msg);
             SetDlgItemText(hDlg, IDC_SCIP_ALL,    (Lan+161)->msg);
             SetDlgItemText(hDlg, IDC_RENAME,      (Lan+163)->msg);
             SetDlgItemText(hDlg, IDC_REN_AUTO,    (Lan+162)->msg);
             SetDlgItemText(hDlg, IDCANCEL,        (Lan+68)->msg);
             ViewOldFilePar(hDlg);                           //��������� ������� �����
             ViewNewFilePar(hDlg);                           //��������� ������ �����
             return TRUE;
      case WM_CTLCOLORSTATIC:
             if(GetDlgCtrlID(HWND(lParam)) == IDC_STATICTEXT2)
             {  SetTextColor((HDC)wParam, RGB(255, 0, 0));
                SetBkColor((HDC)wParam, FonLTGRAY);
                return (BOOL)FonBrush;
             }
             return TRUE;
      case WM_COMMAND:
           switch(LOWORD(wParam))
           {  case IDC_REP_ALL:  prCopy = 1;  break;         //������ ������ �������� ���
              case IDC_REP_OLD:  prCopy = 2;  break;         //������ ������ �������� ������
              case IDC_SCIP_ALL: prCopy = 3;  break;         //������ ������ ���������� ���
              case IDC_REN_AUTO: prCopy = 4;  break;         //������ ������ ��������������� �������������
              case IDC_SCIP:                                 //������ ������ ����������
              case IDOK:                                     //������ ������ ��������
              case IDC_RENAME:                               //������ ������ ��������������
              case IDCANCEL: prCopy = 0;  break;             //������ ������ �������� �����
           }
           EndDialog(hDlg, LOWORD(wParam));                  //������� ���� �� �����
           return TRUE;
   }
   return FALSE;
}

//------------------------------------------------------------------------------

static int CtrlFileYes(char *NameF)                          //�������� ������� �����
{
   int ret;
   if(CtrlFileYesNo(NameF) == 0) return 0;                   //����� ��� - �������� ������� �����
   if(prCopy == 0)
        ret = DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_FILE_YES),
                      MainWin, Dlg_FileYes, LPARAM(NameF));
   switch(prCopy)                                             //������� �������� ��� ��������� �����������
   {  case 0: break;
      case 1: ret = IDOK; break;                             //������ ������ �������� ���
      case 2: ret = Comp_TimeF(); break;                     //������ ������ �������� ������
      case 3: ret = IDC_SCIP; break;                         //������ ������ ���������� ���
      case 4: return Mk_NewName(NameF);                      //������ ������ ��������������� �������������
   }
   switch (ret)
   {  case IDCANCEL: *NameF = 0;  return -1;                 //������ ������ �����������
      case IDC_SCIP: *NameF = 0;  return -2;                 //������ ������ ����������
      case IDC_RENAME: return 1;                             //������ ������ ��������������
      case IDOK:                                             //������ ������ ��������
                 if(DeleteFile(NameF) != 0) return 0;
                 return ErrorSys2((Lan+56)->msg, NameF);     //"������ ��� �������� �����"
   }
   return -1;
}

//------------------------------------------------------------------------------

void Ctrl_Name_Ch(char *Name)                                //�������� ����� �� ������������ ������� � ������ ������������ ��������
{
   for(int i=0; *(Name+i)!=0; i++)
     for(DWORD j=0; j<sizeof(ndS); j++)
        if(*(Name + i) == *(ndS + j))
        {  *(Name + i) = '_';
//         pr_ndS = 1;                                       //������� ����������� ������ ������������� �������
        }
}

//------------------------------------------------------------------------------

//11int OpenOutFile(char *Name, LONGLONG Size, HANDLE *File)     //�������� ��������� �����
int OpenOutFileName_New(char *Name, LONGLONG Size, HANDLE *File) //�������� ��������� �����
{
   int ret;
   for(;;)
   {  if(GetName_OutFile(Name) < 0) return -1;               //���� ����� ��� ������ �����
      if((ret = Ctrl1FileYes(Name)) < 0) return ret;         //�������� ������� ����� � ��������� ������
      if(ret == 1) continue;                                 //��������� ���� ������ �����
      if((ret = DiskFreeSpaceEx(Name, Size)) < 0)return -1;  //������ (���� ������� == 1, �� ����� ���)
      if(ret == 0) break;                                    //���� ����� ��� ������
   }
//11return Open_OutFile512(Name, File);                      //�������� ��������� �����
   return Open_OutFile(Name, File);                          //�������� ��������� �����
}

//------------------------------------------------------------------------------

//11 int OpenOutFileNameYes(char *Name, LONGLONG Size, HANDLE *File)//�������� ��������� �����
int OpenOutFileName_Yes(char *Name, LONGLONG Size, HANDLE *File)//�������� ��������� �����
{
   int ret, pr = 0;
   for(;;)
   {  if(pr != 0)                                            //��� ������ ������� ��� �� ���������
        if(GetName_OutFile(Name) < 0) return -1;             //���� ����� ��� ������ �����
      pr = 1;                                                //��� ������������� ������� ����������� ���
      if((ret = CtrlFileYes(Name)) < 0) return ret;          //�������� ������� ����� � ��������� ������
      if(ret == 1) continue;                                 //��������� ���� ������ �����
      if((ret = DiskFreeSpaceEx(Name, Size)) < 0)return -1;  //������   (���� ������� == 1, �� ����� ���)
      if(ret == 0) break;                                    //���� ����� ��� ������
   }
//11   return Open_OutFile512(Name, File);                   //�������� ��������� �����
   return Open_OutFile(Name, File);                          //�������� ��������� �����
}

#endif

