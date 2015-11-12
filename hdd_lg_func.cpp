
//                                16.04.2010
#ifndef __HDD_LG_FUNC__
#define __HDD_LG_FUNC__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_func =====================================
int  CreateMyFont(void);                                     //Создание фонта
char *Char_Dig_p(char *CharDig, int N);                      //Преобразование символьного числа в разрядку
char *Char_Dig_p_n(char *CharDig, int N, int n);             //Преобразование символьного числа в разрядку с "n" знаками после запятой
char *FileSize_Txt(DWORDLONG Size);                          //Размер файла в текст
void CenterDlg(HWND hDlg, int yPoz);                         //Центрирование окна диалога в главном окне
void OutNameDlg(HWND hDlg, int ID_T, char *inText);          //Усечение имени
void UnicodeToAnsi(WCHAR *inUNI, char *outANSI, int maxN);   //Преобразование Unicode в символы
void Open_Wait_Found_HDD(void);                              //Окошко со словами подождите
void Close_Wait_Found_HDD(void);                             //Окошко со словами подождите
int  Ctrl_Adm(void);                                         //Проверка наличия прав администратора
void About(void);
void AddToSpecSpis_VerWin(void);                             //Запись в дамп версии Windows

static HWND hWaitDlg;

//------------------------------------------------------------------------------

int CreateMyFont(void)                                       //Создание фонта
{
   LOGFONT LogFont;

   memset(&LogFont, 0, sizeof(LOGFONT));                     //Заполнили нулями
// LogFont.lfCharSet = DEFAULT_CHARSET;
   LogFont.lfHeight = 16;
   strcpy(LogFont.lfFaceName, "MS Sans Serif");
   MyFont = CreateFontIndirect(&LogFont);
   if(MyFont == NULL)
      return Error1((Lan+7)->msg);                           //"Ошибка при создании шрифта."

   memset(&LogFont, 0, sizeof(LOGFONT));                     //Заполнили нулями
   LogFont.lfCharSet = DEFAULT_CHARSET;
   LogFont.lfHeight = 12;
   strcpy(LogFont.lfFaceName, "Arial");
   MyFontSm = CreateFontIndirect(&LogFont);
   if(MyFontSm == NULL)
      return Error1((Lan+7)->msg);                           //"Ошибка при создании шрифта."

   memset(&LogFont, 0, sizeof(LOGFONT));                     //Заполнили нулями
   LogFont.lfCharSet = DEFAULT_CHARSET;
   LogFont.lfHeight = 15;   LogFont.lfPitchAndFamily = FF_MODERN;                     //Моноширинный
   strcpy(LogFont.lfFaceName, "Courier New");
   MyFont_Cur = CreateFontIndirect(&LogFont);
   if(MyFont_Cur == NULL)
      return Error1((Lan+7)->msg);                           //"Ошибка при создании шрифта."
   return 0;
}

//------------------------------------------------------------------------------

char *Char_Dig_p(char *CharDig, int N)                       //Преобразование символьного числа в разрядку (целого)
{                                                            //N-общее число символов в выходной строке
   static char Ss[100];                                      //Символьное число в разрядку
   int l = lstrlen(CharDig);                                 //Число символов во входной строке
   for(int i=0; i<N; i++) *(Ss + i) = ' ';                   //Очистили строку
   *(Ss + N) = 0;                                            //Отметили конец строки
   for(int i=1,k=N-1,j=l-1; i<=l; i++,k--,j--)               //Пересылка символов с вставкой пробела
   {  if(k < 0) break;
      *(Ss + k) = *(CharDig + j);
      if(i % 3 == 0) k--;
   }
   return Ss;
}

//------------------------------------------------------------------------------

char *Char_Dig_p_n(char *CharDig, int N, int n)              //Преобразование символьного числа в разрядку с "n" знаками после запятой
{                                                            //N-общее число символов в выходной строке
   static char Ss[100];                                      //Символьное число в разрядку
   int i, j, k;

   *Ss = 0;
   if(N >= 99)  { Error1("Char_Dig"); return Ss; }
   int l = lstrlen(CharDig);                                 //Число символов в исходной строке
   int m = n;                                                //Число символов которые не разряжаются (n-после запятой + точка)
   if(n != 0) m++;
   if(m > l) m = l;
   if(l+(l-m)/3 >= 99)  { Error1("Char_Dig"); return Ss; }
   for(i=0; i<N; i++) *(Ss + i) = ' ';                       //Очистили строку
   *(Ss + N) = 0;                                            //Отметили конец строки
   for(i=1,j=l-1,k=N-1; i<=m; i++,j--,k--)                   //Переслали неизменяемую часть числа (n-после запятой + точка)
     *(Ss + k) = *(CharDig + j);
   for(i=1; i<=l-m; i++,j--,k--)                             //Пересылка оставшихся символов с вставкой пробела
   {  if(k < 0) break;
      *(Ss + k) = *(CharDig + j);
      if(i % 3 == 0) k--;
   }
   if(Ss[k+1] == '-' && Ss[k+2] == ' ')                      //Проверяем, чтобы знак числа не оторвался от первой цифры
   {  Ss[k+1] = ' '; Ss[k+2] = '-'; }
   return Ss;
}

//------------------------------------------------------------------------------

char *FileSize_Txt(DWORDLONG Size)                           //Размер файла в текст
{
   char dd[48], *Adr;

   sprintf(dd, "%16.0f", double(Size));
   Adr = Char_Dig_p(dd, 22);                                  //Преобразование символьного числа в разрядку
   return Adr;
}

//------------------------------------------------------------------------------

void CenterDlg(HWND hDlg, int yPoz)                          //Центрирование окна диалога в главном окне
{
   RECT RectD, RectW;
   GetWindowRect(MainWin, &RectW);
   GetWindowRect(hDlg, &RectD);
   int xCent = (RectW.right + RectW.left) / 2;               //Центр главного окна
   int yCent = (RectW.bottom + RectW.top) / 2;               //Центр главного окна
   int xDlg =  xCent - (RectD.right - RectD.left) / 2;
   int yDlg =  yCent - (RectD.bottom - RectD.top) / 2;
   SetWindowPos(hDlg, HWND_TOP, xDlg, yDlg + yPoz, 0, 0, SWP_NOSIZE);
}

//------------------------------------------------------------------------------

void Out_NameDlg(HWND hDlg, int ID_T, char *inText, char *ShortTxt, char **ATxt) //Усечение имени
{
   static char *StartTxt = "w:\\...";                        //Довесок к усеченному тексту (не менять static)
   SIZE SizeTxt;                                             //Размер изображения текста
   RECT WRect;                                               //Структура с координатами окна
   if(*inText == 0)
   {  *ShortTxt = 0;
      *ATxt = ShortTxt;
      return;
   }
   lstrcpy(ShortTxt+6, inText);                              //Tекст для вывода
   *StartTxt = *inText;                                      //Переслали букву диска
   HWND hWnd = GetDlgItem(hDlg, ID_T);                       //Идентификатор окна для текста
   HDC hDC = GetDC(hWnd);                                    //Контекст окна для текста
   GetClientRect(hWnd, &WRect);                              //Получили размеры окна
//Переодически строка становится больше окна. Возможно надо выбирать эмперический коэффициент из величины текста
// int LogSize = MulDiv(WRect.right-10, GetDeviceCaps(hDC, LOGPIXELSX), 72);
   int LogSize = int(1.25 * (WRect.right+1));                //Эмпирический коэффициент соотношения размеров окна и размеров текта
   *ATxt = ShortTxt+6;                                       //Адрес начала текста
   int Len = lstrlen(*ATxt);                                 //Число символов в тексте
   GetTextExtentPoint32(hDC, *ATxt, Len, &SizeTxt);
   if(SizeTxt.cx > LogSize)                                  //Весь текст не поместится в окно
   {  GetTextExtentPoint32(hDC, StartTxt, 6, &SizeTxt);      //Размер довеска текста
      LogSize -= SizeTxt.cx;                                 //Уменьшили размер окна на величину довеска
      for(;Len>0; *(*ATxt)++,Len--)                          //Пока не вычислим длину хвоста имени
      {  GetTextExtentPoint32(hDC, *ATxt, Len, &SizeTxt);
         if(SizeTxt.cx < LogSize) break;                     //Если остаток текста поместится в окно
      }
      if(*ATxt > ShortTxt+6)                                 //Произошло отсечение имени
      {  *ATxt -= 6;                                         //Место для довеска
         CopyMemory(*ATxt, StartTxt, 6);                     //Переслали довесок
      }
   }
   ReleaseDC(hWnd, hDC);                                     //Освободили контекст
}

//------------------------------------------------------------------------------

void OutNameDlg(HWND hDlg, int ID_T, char *inText)           //Усечение имени
{
   char ShortTxt[260+6];
   char *ATxt;
   Out_NameDlg(hDlg, ID_T, inText, ShortTxt, &ATxt);         //Усечение имени
   SendDlgItemMessage(hDlg, ID_T, WM_SETTEXT, 0, LPARAM(ATxt));
}

//------------------------------------------------------------------------------

void UnicodeToAnsi(WCHAR *inUNI, char *outANSI, int maxN)    //Преобразование Unicode в символы
{
   char strANSI[256];
   for(int i=0; i<maxN; i++) SWAP16((WORD *)(inUNI + i));    //В каталоге байты переставлены
// WideCharToMultiByte(CP_ACP, 0, inUNI, -1, outANSI, 100, NULL, NULL);  //Непосредственный адрес не работает
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
             CenterDlg(hDlg, -90);                           //Центрирование окна диалога в главном окне
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

void Open_Wait_Found_HDD(void)                               //Окошко со словами подождите
{
   hWaitDlg = CreateDialog(MainInst, MAKEINTRESOURCE(IDD_WAIT), MainWin, Dlg_Wait);
}

//------------------------------------------------------------------------------

void Close_Wait_Found_HDD(void)                              //Окошко со словами подождите
{
   if(hWaitDlg != NULL) EndDialog(hWaitDlg, 0);
   hWaitDlg = NULL;
}

//------------------------------------------------------------------------------

int Ctrl_Adm(void)                                           //Проверка наличия прав администратора
{
#if !defined EMULATOR_HDD
   HANDLE hToken;
   LUID TakeOwnershipValue;
   TOKEN_PRIVILEGES tkp;
   if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
      return Error2((Lan+26)->msg, "(code -10)");            //return Error2("Ошибка при проверке уровня доступа", "(code -10)");
   if(!LookupPrivilegeValue((LPSTR)NULL, SE_TAKE_OWNERSHIP_NAME, &TakeOwnershipValue))
      return Error2((Lan+26)->msg, "(code -11)");            //return Error2("Ошибка при проверке уровня доступа", "(code -11)");
   tkp.PrivilegeCount = 1;
   tkp.Privileges[0].Luid = TakeOwnershipValue;
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
   AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL);
   if(GetLastError() != ERROR_SUCCESS)
      return Error1((Lan+25)->msg);                          //return Error1("Данная программа не работает если у текущего пользователя нет прав администратора.");
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
               CenterDlg(hDlg, 0);                           //Центрирование окна диалога в главном окне
               if(Conf.nLanguage == 0)                       //Признак языка по умолчанию 0-русский,1-другой
               {  SetWindowText(hDlg, "О программе");        //Вывели новый заголовок
                  SetDlgItemText(hDlg, IDC_STATICTEXT1, "Программа копирования файлов с HDD извлеченного из рекордера LG");
                  SetDlgItemText(hDlg, IDC_STATICTEXT2, "Автор Кудряшов В. ( Val238 ),  г. Москва");
                  SetDlgItemText(hDlg, IDC_STATICTEXT6, "Версия");
                  SetDlgItemText(hDlg, IDCANCEL, "Закрыть");
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
void AddToSpecSpis_VerWin(void)                              //Запись в дамп версии Windows
{
   HKEY hKey;
   char key[260], Ss[300];
   DWORD nb = sizeof(key);
   DWORD Type = REG_SZ;                                      //Тип - нуль терминированная строка
   int n;
   
   if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                   0, KEY_READ, &hKey) != ERROR_SUCCESS)     //Ключ не найден
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

