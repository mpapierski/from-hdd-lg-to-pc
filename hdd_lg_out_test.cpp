
//                               02.07.2007
#ifndef __HDD_LG_OUT_TEST__
#define __HDD_LG_OUT_TEST__

#include "from_hdd_lg_to_pc.h"

#if defined OUT_TEST

//============================ hdd_lg_out_test =================================
void Add_SpecSpis(char *Str);                                //Добавление строки в список
void View_HEX_Any(BYTE *buff, DWORD Size);                   //Вывод шеснадцатиричных кодов
HWND Create_SpecWin(void);                                   //Создание окна
BOOL Register_SpecWin(void);                                 //Регистрация окна

static HWND SpecWin;
static HWND hListBoxS;
static char *SW_Name = "SpTest";

//------------------------------------------------------------------------------

static int SaveDump(void)
{
   char Driv[MAXDRIVE], Dir[MAXDIR], Name[MAXFILE], Ext[MAXEXT];
   char NameF[260];

   SYSTEMTIME ST;
   GetLocalTime(&ST);
   if(GetModuleFileName(NULL, NameF, sizeof(NameF)) == 0) return -1;
   fnsplit(NameF, Driv, Dir, Name, Ext);                     //Разложили имя файла
   wsprintf(Name, "%04d_%02d_%02d_%02d_%02d_dump_for_val238", ST.wYear, ST.wMonth, ST.wDay, ST.wHour, ST.wMinute);
   fnmerge(NameF, Driv, Dir, Name, ".txt");                  //Получили имя файла
   FILE *PrgF;
   if((PrgF = fopen(NameF, "wt")) == NULL)
      return Error2(NameF, "Error open file.");
   char Str[4096];
   int num = SendMessage(hListBoxS, LB_GETCOUNT, 0, 0);
   for(int i=0; i<num; i++)
   {  SendMessage(hListBoxS, LB_GETTEXT, i, (LPARAM)Str);
      fprintf(PrgF, "%s\n", Str);
   }
   fclose(PrgF);
   char Msg[4096];
   sprintf(Msg, "File name:\n"
                 "%s.txt\n\n"
                 "Please, compress a file any archiver available at you\n"
                 "and send it for analysis Val238.\n\n"
                 "e-mail:  val238@mail.ru", Name);
   MessageBox(MainWin, Msg, "Attention!", MB_TOPMOST | MB_OK | MB_ICONWARNING | MB_APPLMODAL);
   return 0;
}

//------------------------------------------------------------------------------

void Add_SpecSpis(char *Str)                                 //Добавление строки в список
{
   if(hListBoxS == NULL) return;
   SendMessage(hListBoxS, LB_ADDSTRING, 0, (LPARAM)Str);
   int Poz = SendMessage(hListBoxS, LB_GETCOUNT, 0, 0);      
   SendMessage(hListBoxS,LB_SETCURSEL, Poz-1, 0);
// FormList->ListBox1->Update();
   UpdateWindow(hListBoxS);                                  //Восстановили окно после диалога выбора файла
}

//------------------------------------------------------------------------------

void View_HEX_Any(BYTE *buff, DWORD Size)                    //Вывод шеснадцатиричных кодов
{
   char Ss[4096];
   DWORD iEnd, Size1 = Size - 1;

   for(iEnd=Size-1; iEnd>0; iEnd--)                          //Поиск последнего значащего байта
     if(*(buff + iEnd) != 0) break;
   if(iEnd <= 0)
   { Add_SpecSpis("All bytes == 0");
     return;
   }
// if(iEnd > Size - 4)  iEnd = Size-1;                       //Нет смысла писать заменитель
   if(iEnd < Size1 && ((iEnd+1) % 128) == 0) iEnd++;         //Заменитель попал на конец строки
   for(DWORD i=0,n=1; i<=iEnd; i+=128,n++)
   {  char *Adr = Ss;
      sprintf(Adr, "%4d: %06X:", n, i);                      //Смещение первого байта строки в секторе
      Adr += 13;
      for(DWORD j=0; j<128; j++)
      {   sprintf(Adr, " %02X", *(buff+i+j));
          Adr += 3;
          if((j+1)%8 == 0)
          {  *Adr = ' ';   Adr++; }                          //Увеличили интервал
          if((i+j) >= iEnd && iEnd < Size1)
          { lstrcpy(Adr, " (0)");
            Adr += 4;
            break;
          }
      }
      *Adr = 0;
      Add_SpecSpis(Ss);
   }
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL SpecWin_OnCreate(HWND hwnd, CREATESTRUCT FAR *lpCreateStruct)
{
   RECT WRect;                                              //Размеры рабочей области окна
   GetClientRect(hwnd, &WRect);                             //Получили размеры рабочей области окна

   int sw_xs = 6;
   int sw_ys = 6;
   int sw_ls = WRect.right - 2 * sw_xs;
   int sw_hs = WRect.bottom - 2 * sw_ys - 30;
   int sw_x = (WRect.right + WRect.left - 300) / 2;
   int sw_y = sw_hs + 8;

   hListBoxS = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", NULL,
                    WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE,
                    sw_xs, sw_ys, sw_ls, sw_hs, hwnd, HMENU(IDC_LISTBOX),
                    MainInst, NULL);
   SendMessage(hListBoxS, WM_SETFONT, (WPARAM)MyFont_Cur, LPARAM(TRUE));
   SendMessage(hListBoxS, LB_SETHORIZONTALEXTENT, 10*2048, 0);

   HWND hSave = CreateWindow("button", "Save dump for compress and send to Val238",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_x, sw_y, 300, 24, hwnd, HMENU(IDOK),
                     MainInst, NULL);
   SendMessage(hSave, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   return TRUE;

}

//------------------------------------------------------------------------------

#pragma argsused
static void SpecWin_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT CodeNotify)
{
   switch(id)
   {  case IDOK:  SaveDump();
                  DestroyWindow(hwnd);
                  hListBoxS = NULL;
                  break;
   }
}

//------------------------------------------------------------------------------

#pragma argsused
static LRESULT CALLBACK WndProc_SpecW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   switch(Msg)
   {
      HANDLE_MSG(hwnd, WM_CREATE,  SpecWin_OnCreate);
      HANDLE_MSG(hwnd, WM_COMMAND, SpecWin_OnCommand);
      default: return DefWindowProc(hwnd, Msg, wParam, lParam);
  }
}

//------------------------------------------------------------------------------

HWND Create_SpecWin(void)                                    //Создание окна
{
   RECT WRect;                                               //Размеры рабочей области окна
   GetWindowRect(hwndTree, &WRect);                          //Получили размеры рабочей области окна
   int WinScrS_x = WRect.right - WRect.left - 16;
   int WinScrS_y = WRect.bottom - WRect.top - 16;
   int Win_ScrSt_x = WRect.left + 8;
   int Win_ScrSt_y = WRect.top + 8;

   SpecWin = CreateWindow(SW_Name, SW_Name,
                          WS_POPUP | WS_VISIBLE | WS_OVERLAPPED | WS_THICKFRAME,
                          Win_ScrSt_x, Win_ScrSt_y, WinScrS_x, WinScrS_y,
                          MainWin, NULL, MainInst, NULL);
   return SpecWin;
}

//------------------------------------------------------------------------------

BOOL Register_SpecWin(void)                                  //Регистрация окна
{
   WNDCLASS WndClass;

   WndClass.style         = CS_HREDRAW | CS_VREDRAW;         //Внешний вид окна
   WndClass.lpfnWndProc   = WndProc_SpecW;                   //Имя оконной процедуры
   WndClass.cbClsExtra    = 0;
   WndClass.cbWndExtra    = 0;
   WndClass.hInstance     = MainInst;
// WndClass.hIcon         = MyIcon;
   WndClass.hIcon         = LoadIcon(MainInst, NULL);
   WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);     //Форма курсора
   WndClass.hbrBackground = FonBrush;//GetStockBrush(LTGRAY_BRUSH);   //Цвет окна
   WndClass.lpszMenuName  = NULL;                            //Меню окна
   WndClass.lpszClassName = SW_Name;                         //Имя класа окна
   return RegisterClass(&WndClass);
}

#endif

#endif

