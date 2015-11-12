
//                                16.04.2010
#ifndef __HDD_LG_FUNC__
#define __HDD_LG_FUNC__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_func =====================================
int  CreateMyFont(void);                                     //�������� �����
char *Char_Dig_p(char *CharDig, int N);                      //�������������� ����������� ����� � ��������
char *Char_Dig_p_n(char *CharDig, int N, int n);             //�������������� ����������� ����� � �������� � "n" ������� ����� �������
char *FileSize_Txt(DWORDLONG Size);                          //������ ����� � �����
void CenterDlg(HWND hDlg, int yPoz);                         //������������� ���� ������� � ������� ����
void OutNameDlg(HWND hDlg, int ID_T, char *inText);          //�������� �����
void UnicodeToAnsi(WCHAR *inUNI, char *outANSI, int maxN);   //�������������� Unicode � �������
void Open_Wait_Found_HDD(void);                              //������ �� ������� ���������
void Close_Wait_Found_HDD(void);                             //������ �� ������� ���������
int  Ctrl_Adm(void);                                         //�������� ������� ���� ��������������
void About(void);
void AddToSpecSpis_VerWin(void);                             //������ � ���� ������ Windows

static HWND hWaitDlg;

//------------------------------------------------------------------------------

int CreateMyFont(void)                                       //�������� �����
{
   LOGFONT LogFont;

   memset(&LogFont, 0, sizeof(LOGFONT));                     //��������� ������
// LogFont.lfCharSet = DEFAULT_CHARSET;
   LogFont.lfHeight = 16;
   strcpy(LogFont.lfFaceName, "MS Sans Serif");
   MyFont = CreateFontIndirect(&LogFont);
   if(MyFont == NULL)
      return Error1((Lan+7)->msg);                           //"������ ��� �������� ������."

   memset(&LogFont, 0, sizeof(LOGFONT));                     //��������� ������
   LogFont.lfCharSet = DEFAULT_CHARSET;
   LogFont.lfHeight = 12;
   strcpy(LogFont.lfFaceName, "Arial");
   MyFontSm = CreateFontIndirect(&LogFont);
   if(MyFontSm == NULL)
      return Error1((Lan+7)->msg);                           //"������ ��� �������� ������."

   memset(&LogFont, 0, sizeof(LOGFONT));                     //��������� ������
   LogFont.lfCharSet = DEFAULT_CHARSET;
   LogFont.lfHeight = 15;   LogFont.lfPitchAndFamily = FF_MODERN;                     //������������
   strcpy(LogFont.lfFaceName, "Courier New");
   MyFont_Cur = CreateFontIndirect(&LogFont);
   if(MyFont_Cur == NULL)
      return Error1((Lan+7)->msg);                           //"������ ��� �������� ������."
   return 0;
}

//------------------------------------------------------------------------------

char *Char_Dig_p(char *CharDig, int N)                       //�������������� ����������� ����� � �������� (������)
{                                                            //N-����� ����� �������� � �������� ������
   static char Ss[100];                                      //���������� ����� � ��������
   int l = lstrlen(CharDig);                                 //����� �������� �� ������� ������
   for(int i=0; i<N; i++) *(Ss + i) = ' ';                   //�������� ������
   *(Ss + N) = 0;                                            //�������� ����� ������
   for(int i=1,k=N-1,j=l-1; i<=l; i++,k--,j--)               //��������� �������� � �������� �������
   {  if(k < 0) break;
      *(Ss + k) = *(CharDig + j);
      if(i % 3 == 0) k--;
   }
   return Ss;
}

//------------------------------------------------------------------------------

char *Char_Dig_p_n(char *CharDig, int N, int n)              //�������������� ����������� ����� � �������� � "n" ������� ����� �������
{                                                            //N-����� ����� �������� � �������� ������
   static char Ss[100];                                      //���������� ����� � ��������
   int i, j, k;

   *Ss = 0;
   if(N >= 99)  { Error1("Char_Dig"); return Ss; }
   int l = lstrlen(CharDig);                                 //����� �������� � �������� ������
   int m = n;                                                //����� �������� ������� �� ����������� (n-����� ������� + �����)
   if(n != 0) m++;
   if(m > l) m = l;
   if(l+(l-m)/3 >= 99)  { Error1("Char_Dig"); return Ss; }
   for(i=0; i<N; i++) *(Ss + i) = ' ';                       //�������� ������
   *(Ss + N) = 0;                                            //�������� ����� ������
   for(i=1,j=l-1,k=N-1; i<=m; i++,j--,k--)                   //��������� ������������ ����� ����� (n-����� ������� + �����)
     *(Ss + k) = *(CharDig + j);
   for(i=1; i<=l-m; i++,j--,k--)                             //��������� ���������� �������� � �������� �������
   {  if(k < 0) break;
      *(Ss + k) = *(CharDig + j);
      if(i % 3 == 0) k--;
   }
   if(Ss[k+1] == '-' && Ss[k+2] == ' ')                      //���������, ����� ���� ����� �� ��������� �� ������ �����
   {  Ss[k+1] = ' '; Ss[k+2] = '-'; }
   return Ss;
}

//------------------------------------------------------------------------------

char *FileSize_Txt(DWORDLONG Size)                           //������ ����� � �����
{
   char dd[48], *Adr;

   sprintf(dd, "%16.0f", double(Size));
   Adr = Char_Dig_p(dd, 22);                                  //�������������� ����������� ����� � ��������
   return Adr;
}

//------------------------------------------------------------------------------

void CenterDlg(HWND hDlg, int yPoz)                          //������������� ���� ������� � ������� ����
{
   RECT RectD, RectW;
   GetWindowRect(MainWin, &RectW);
   GetWindowRect(hDlg, &RectD);
   int xCent = (RectW.right + RectW.left) / 2;               //����� �������� ����
   int yCent = (RectW.bottom + RectW.top) / 2;               //����� �������� ����
   int xDlg =  xCent - (RectD.right - RectD.left) / 2;
   int yDlg =  yCent - (RectD.bottom - RectD.top) / 2;
   SetWindowPos(hDlg, HWND_TOP, xDlg, yDlg + yPoz, 0, 0, SWP_NOSIZE);
}

//------------------------------------------------------------------------------

void Out_NameDlg(HWND hDlg, int ID_T, char *inText, char *ShortTxt, char **ATxt) //�������� �����
{
   static char *StartTxt = "w:\\...";                        //������� � ���������� ������ (�� ������ static)
   SIZE SizeTxt;                                             //������ ����������� ������
   RECT WRect;                                               //��������� � ������������ ����
   if(*inText == 0)
   {  *ShortTxt = 0;
      *ATxt = ShortTxt;
      return;
   }
   lstrcpy(ShortTxt+6, inText);                              //T���� ��� ������
   *StartTxt = *inText;                                      //��������� ����� �����
   HWND hWnd = GetDlgItem(hDlg, ID_T);                       //������������� ���� ��� ������
   HDC hDC = GetDC(hWnd);                                    //�������� ���� ��� ������
   GetClientRect(hWnd, &WRect);                              //�������� ������� ����
//������������ ������ ���������� ������ ����. �������� ���� �������� ������������ ����������� �� �������� ������
// int LogSize = MulDiv(WRect.right-10, GetDeviceCaps(hDC, LOGPIXELSX), 72);
   int LogSize = int(1.25 * (WRect.right+1));                //������������ ����������� ����������� �������� ���� � �������� �����
   *ATxt = ShortTxt+6;                                       //����� ������ ������
   int Len = lstrlen(*ATxt);                                 //����� �������� � ������
   GetTextExtentPoint32(hDC, *ATxt, Len, &SizeTxt);
   if(SizeTxt.cx > LogSize)                                  //���� ����� �� ���������� � ����
   {  GetTextExtentPoint32(hDC, StartTxt, 6, &SizeTxt);      //������ ������� ������
      LogSize -= SizeTxt.cx;                                 //��������� ������ ���� �� �������� �������
      for(;Len>0; *(*ATxt)++,Len--)                          //���� �� �������� ����� ������ �����
      {  GetTextExtentPoint32(hDC, *ATxt, Len, &SizeTxt);
         if(SizeTxt.cx < LogSize) break;                     //���� ������� ������ ���������� � ����
      }
      if(*ATxt > ShortTxt+6)                                 //��������� ��������� �����
      {  *ATxt -= 6;                                         //����� ��� �������
         CopyMemory(*ATxt, StartTxt, 6);                     //��������� �������
      }
   }
   ReleaseDC(hWnd, hDC);                                     //���������� ��������
}

//------------------------------------------------------------------------------

void OutNameDlg(HWND hDlg, int ID_T, char *inText)           //�������� �����
{
   char ShortTxt[260+6];
   char *ATxt;
   Out_NameDlg(hDlg, ID_T, inText, ShortTxt, &ATxt);         //�������� �����
   SendDlgItemMessage(hDlg, ID_T, WM_SETTEXT, 0, LPARAM(ATxt));
}

//------------------------------------------------------------------------------

void UnicodeToAnsi(WCHAR *inUNI, char *outANSI, int maxN)    //�������������� Unicode � �������
{
   char strANSI[256];
   for(int i=0; i<maxN; i++) SWAP16((WORD *)(inUNI + i));    //� �������� ����� ������������
// WideCharToMultiByte(CP_ACP, 0, inUNI, -1, outANSI, 100, NULL, NULL);  //���������������� ����� �� ��������
// WideCharToMultiByte(CP_ACP, 0, inUNI, -1, strANSI, 100, NULL, NULL);
   WideCharToMultiByte(CP_ACP, 0, inUNI, maxN, strANSI, 100, NULL, NULL);
   for(int i=0; i<maxN; i++)
   {  *(outANSI + i) = *(strANSI + i);
      if(*(outANSI + i) == 0) return;
   }
   *(outANSI + maxN) = 0;
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL CALLBACK Dlg_Wait(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch(Message)
   {  case WM_INITDIALOG:
             CenterDlg(hDlg, -90);                           //������������� ���� ������� � ������� ����
             SetDlgItemText(hDlg, IDC_STATICTEXT1, (Lan+57)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT2, (Lan+58)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT3, (Lan+59)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT4, (Lan+60)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT5, (Lan+61)->msg);
             return TRUE;
//      case WM_CTLCOLORSTATIC:
//            if(GetDlgCtrlID(HWND(lParam)) == IDC_STATICTEXT1)
//            {  SetTextColor((HDC)wParam, RGB(0, 0, 180));
//               SetBkColor((HDC)wParam, FonLTGRAY);
//               return (BOOL)FonBrush;
//            }
//            return TRUE;
   }
   return FALSE;
}

//------------------------------------------------------------------------------

void Open_Wait_Found_HDD(void)                               //������ �� ������� ���������
{
   hWaitDlg = CreateDialog(MainInst, MAKEINTRESOURCE(IDD_WAIT), MainWin, Dlg_Wait);
}

//------------------------------------------------------------------------------

void Close_Wait_Found_HDD(void)                              //������ �� ������� ���������
{
   if(hWaitDlg != NULL) EndDialog(hWaitDlg, 0);
   hWaitDlg = NULL;
}

//------------------------------------------------------------------------------

int Ctrl_Adm(void)                                           //�������� ������� ���� ��������������
{
#if !defined EMULATOR_HDD
   HANDLE hToken;
   LUID TakeOwnershipValue;
   TOKEN_PRIVILEGES tkp;
   if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
      return Error2((Lan+26)->msg, "(code -10)");            //return Error2("������ ��� �������� ������ �������", "(code -10)");
   if(!LookupPrivilegeValue((LPSTR)NULL, SE_TAKE_OWNERSHIP_NAME, &TakeOwnershipValue))
      return Error2((Lan+26)->msg, "(code -11)");            //return Error2("������ ��� �������� ������ �������", "(code -11)");
   tkp.PrivilegeCount = 1;
   tkp.Privileges[0].Luid = TakeOwnershipValue;
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
   AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL);
   if(GetLastError() != ERROR_SUCCESS)
      return Error1((Lan+25)->msg);                          //return Error1("������ ��������� �� �������� ���� � �������� ������������ ��� ���� ��������������.");
#endif
   return 0;
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL CALLBACK AboutDlg(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   char NumVer[260];
   switch(Message)
   {  case WM_INITDIALOG:
               CenterDlg(hDlg, 0);                           //������������� ���� ������� � ������� ����
               if(Conf.nLanguage == 0)                       //������� ����� �� ��������� 0-�������,1-������
               {  SetWindowText(hDlg, "� ���������");        //������ ����� ���������
                  SetDlgItemText(hDlg, IDC_STATICTEXT1, "��������� ����������� ������ � HDD ������������ �� ��������� LG");
                  SetDlgItemText(hDlg, IDC_STATICTEXT2, "����� �������� �. ( Val238 ),  �. ������");
                  SetDlgItemText(hDlg, IDC_STATICTEXT6, "������");
                  SetDlgItemText(hDlg, IDCANCEL, "�������");
               }
               wsprintf(NumVer, "%s    ( %s )", NVER_TXT, __DATE__);
               SetDlgItemText(hDlg, IDC_STATICTEXT7, NumVer);
               return TRUE;
      case WM_COMMAND:
               if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
               {  EndDialog(hDlg, IDOK);
                  return TRUE;
               }
               break;
   }
   return FALSE;
}

//------------------------------------------------------------------------------

void About(void)
{
   DialogBox(MainInst, MAKEINTRESOURCE(IDD_DLG_ABOUT), MainWin, AboutDlg);
}

//------------------------------------------------------------------------------

#if defined OUT_TEST
void AddToSpecSpis_VerWin(void)                              //������ � ���� ������ Windows
{
   HKEY hKey;
   char key[260], Ss[300];
   DWORD nb = sizeof(key);
   DWORD Type = REG_SZ;                                      //��� - ���� ��������������� ������
   int n;
   
   if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                   0, KEY_READ, &hKey) != ERROR_SUCCESS)     //���� �� ������
   {  Add_SpecSpis("Windows ???");
      return;
   }
   *Ss = 0;
   if(RegQueryValueEx(hKey, "ProductName", NULL, &Type, LPBYTE(key), &nb) == ERROR_SUCCESS)
      n = wsprintf(Ss, " %s", key);
   if(RegQueryValueEx(hKey, "CSDVersion", NULL, &Type, LPBYTE(key), &nb) == ERROR_SUCCESS)
      wsprintf(Ss+n, " %s", key);
   Add_SpecSpis(Ss);
}
#endif

#endif

