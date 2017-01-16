
//                             24.03.2009
#ifndef __HDD_LG_MAP_HDD__
#define __HDD_LG_MAP_HDD__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_map_hdd ==================================
int  View_Map_HDD(void);                                     //Карта занятости HDD
BOOL Register_MapHDDWin(void);                               //Регистрация окна

static HWND MapHDD_WinM, MapHDD_WinM1;
static char *MapWM_Name = "MapM_HDD";
static char *MapW1_Name = "Map1_HDD";
#define xM 4                                                 //Размер квадратика
#define yM 4                                                 //Размер квадратика
static int nG;                                               //Число прямоугольников по горизонтали
static int NumCl_Bl;                                         //Число кластеров в одном квадратике
static int numBl;                                            //Сколько получится квадратиков
static int numErr;                                           //Число ошибок
static short *Map;                                           //Память под число блоков
static HWND hLegend1, hLegend2, hLegend3, hLegend4, hLegend5;
static COLORREF ColFu = RGB(0, 0, 180);                      //Цвет полностью занятого блока
static COLORREF Col_P = RGB(0, 255, 255);                    //Цвет частично занятого/свободдного блока
static COLORREF ColFr = RGB(0, 180, 0);                      //Цвет полностью свободного блока
static COLORREF ColEr = RGB(255, 0, 0);                      //Цвет блока с ошибкой
static DWORD numCl_Err;                                      //Число занятых кластеров не числящихся в каталогах
#define IDC_VOST 3000

//------------------------------------------------------------------------------

static int Init_Map(void)
{
   RECT WRect;                                               //Размеры рабочей области окна
   GetClientRect(MapHDD_WinM1, &WRect);                      //Получили размеры рабочей области окна

   int nZ_FAT = maxZapFAT1 - 2;                              //Минус признак
   nG = WRect.right / (xM + 1);                              //Число прямоугольников по горизонтали
   int nV = WRect.bottom / (yM + 1);                         //Число прямоугольников по вертикали
   NumCl_Bl = (nZ_FAT + nV * nG - 1)/ (nV * nG);             //Число кластеров в одном квадратике
   numBl = nZ_FAT / NumCl_Bl;                                //Сколько получится квадратиков
   Map = (short *)MyAllocMem(numBl * sizeof(short));         //Память под число блоков
   if(Map == NULL)  return -1;

   numErr = 0;                                               //Число ошибок
#if defined TEST_FAT1                                        //Проверка FAT1
   DWORD *FAT_ = cTestFAT1 + 2;
#else
   DWORD *FAT_ = FAT1 + 2;
#endif
   numCl_Err = 0;                                            //Число занятых кластеров не числящихся в каталогах
   for(int i=0; i<nZ_FAT; i++)                               //По всем записям FAT
   {  if(*(FAT_ + i) == 0) continue;                         //Изначально Пустое значение и счетчик не меняется
//    if(*(FAT_ + i) == 1) continue;                         //Признак, что данный элемент используется
      numCl_Err++;                                           //Число занятых кластеров без "хозяина"
      int n = i / NumCl_Bl;                                  //Индекс блока в карте HDD
      if(*(FAT_ + i) > maxZapFAT1 && *(FAT_ + i) != 0x0FFFFFFF) //Ошибочное значение
      { numErr++; *(Map + n) = -1; continue; }               //Признак ошибки
      if(*(Map + n) != -1)                                   //Ошибки в данном блоке ранее необнаружено
        (*(Map + n))++;                                      //Увеличиваем счетчик
   }
   return 0;
}

//------------------------------------------------------------------------------

static void Create_MapHDD(HDC DC)                            //Карта занятости HDD
{
   if(Map == NULL) return;                                   //Нет карты HDD
   int y = 1;
   int x = 1;
   for(int i=0,j=1; i<numBl; i++,j++)
   {  COLORREF Col = Col_P;                                  //Цвет частично занятого/свободдного блока
      if(*(Map + i) == NumCl_Bl) Col = ColFu;                //Цвет полностью занятого блока
      if(*(Map + i) == 0)        Col = ColFr;                //Цвет полностью свободного блока
      if(*(Map + i) == -1)       Col = ColEr;                //Цвет блока с ошибкой
#if defined TEST_FAT1                                        //Проверка FAT1
      if(*(Map + i) != 0)        Col = ColEr;                //Не нулевой блок это ошибка
#endif
      HPEN Pen = CreatePen(PS_SOLID, 1, Col);
      HPEN OldPen = SelectPen(DC, Pen);
      HBRUSH FBrush = CreateSolidBrush(Col);
      HBRUSH OldBrush = SelectBrush(DC, FBrush);
      Rectangle(DC, x, y, x+xM, y+yM);
      SelectObject(DC, OldBrush);                            //Восстановили старую кисть
      DeleteObject(FBrush);                                  //Удалили новую кисть
      SelectPen(DC, OldPen);                                 //Восстановили старое перо
      DeleteObject(Pen);                                     //Удалили новое перо
      x += xM + 1;
      if(j == nG)
      {  j = 0;
         x = 1;
         y += yM + 1;
      }
   }
}

//------------------------------------------------------------------------------

static void MapHDDWin1_OnPaint(HWND hwnd)
{
   PAINTSTRUCT PaintStruct;                                  //Рабочая структура и контекст
   HDC PaintDC = BeginPaint(hwnd, &PaintStruct);
   Create_MapHDD(PaintDC);                                   //Карта занятости HDD
   EndPaint(hwnd, &PaintStruct);
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL MapHDDWinM_OnCreate(HWND hwnd, CREATESTRUCT FAR *lpCreateStruct)
{
   RECT WRect;                                               //Размеры рабочей области окна
   GetClientRect(hwnd, &WRect);                              //Получили размеры рабочей области окна
   int sw_xs = 4;
   int sw_ys = 4;
   int sw_ls = WRect.right - 2 * sw_xs;
   int sw_hs = WRect.bottom - 2 * sw_ys - 24 - 4;
   int sw_y = sw_hs + sw_ys + 4;

   MapHDD_WinM1 = CreateWindowEx(WS_EX_CLIENTEDGE, MapW1_Name, NULL,
                     WS_CHILD | WS_VISIBLE,
                     sw_xs, sw_ys, sw_ls, sw_hs, hwnd, HMENU(-1),
                     MainInst, NULL);
   HWND hClose = CreateWindow("button", (Lan+6)->msg,
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xs+sw_ls-80, sw_y, 80, 24, hwnd, HMENU(IDOK),
                     MainInst, NULL);
   SendMessage(hClose, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
#if defined TEST_FAT1_VOST                                 //Проверка и восстановление FAT1 на потерянные кластеры
   HWND hVost = CreateWindow("button", "Вост.", //(Lan+6)->msg,
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xs+sw_ls-80-50, sw_y, 40, 24, hwnd, HMENU(IDC_VOST),
                     MainInst, NULL);
   SendMessage(hVost, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
#endif
   if(Init_Map() < 0) return FALSE;
   char s1[200], s2[200], s3[200], s4[200], s5[200];
   SIZE SizeTxt1, SizeTxt2, SizeTxt3, SizeTxt4, SizeTxt5;
   wsprintf(s1, "%s: %3d", (Lan+136)->msg, NumCl_Bl);        //Число кластеров в одном блоке
   wsprintf(s2, "   %s   ", (Lan+125)->msg);                 //Занято
   wsprintf(s3, "   %s / %s   ", (Lan+125)->msg, (Lan+126)->msg); //Занято/Свободно
   wsprintf(s4, "   %s   ", (Lan+126)->msg);                 //Свободно
   wsprintf(s5, "   %s = %d  ", (Lan+137)->msg, numErr);     //Ошибки FAT
#if defined TEST_FAT1                                        //Проверка FAT1
   char s11[200], s12[200], s13[200];
   SIZE SizeTxt11, SizeTxt12, SizeTxt13;
   wsprintf(s11, "   %s   ", "Нет ошибок");                  //Нет ошибок
   wsprintf(s12, "   %s   ", "Потерянные кластеры");         //Потерянные кластеры
   double maxSize = double(maxZapFAT1 - 1) /1024.0/1024.0/1024.0 * sCl_B;
//   double usedSize = double(writeCl) /1024.0/1024.0/1024.0 * sCl_B;
//   double freeSize = double(max_ZapFAT - 1 - writeCl) /1024.0/1024.0/1024.0 * sCl_B;

//   int lostCl = writeCl+1 - sumClTestFAT1;                   //Если кластеры потеряны в результате сбоев в FAT
//   double lostSize = double(lostCl) /1024.0/1024.0/1024.0 * sCl_B;
//   sprintf(s13, "   %s: %d (%.2lf Gb) (%.1lf\%)", "Потеряно кластеров", lostCl, lostSize, lostSize/maxSize*100.0);

   double lostSize = double(numCl_Err) /1024.0/1024.0/1024.0 * sCl_B;
   sprintf(s13, "   %s: %d (%.2lf Gb) (%.1lf\%)", "Потеряно кластеров", numCl_Err, lostSize, lostSize/maxSize*100.0);
#endif
   HDC DC = GetDC(hClose);
   GetTextExtentPoint32(DC, s1, lstrlen(s1), &SizeTxt1);     //Число кластеров в одном блоке
   GetTextExtentPoint32(DC, s2, lstrlen(s2), &SizeTxt2);     //Занято
   GetTextExtentPoint32(DC, s3, lstrlen(s3), &SizeTxt3);     //Занято/Свободно
   GetTextExtentPoint32(DC, s4, lstrlen(s4), &SizeTxt4);     //Свободно
   GetTextExtentPoint32(DC, s5, lstrlen(s5), &SizeTxt5);     //Ошибки FAT
#if defined TEST_FAT1                                        //Проверка FAT1
   GetTextExtentPoint32(DC, s11, lstrlen(s11), &SizeTxt11);  //Число кластеров в одном блоке
   GetTextExtentPoint32(DC, s12, lstrlen(s12), &SizeTxt12);  //Занято
   GetTextExtentPoint32(DC, s13, lstrlen(s13), &SizeTxt13);  //Занято/Свободно
#endif
   ReleaseDC(hClose, DC);
   hLegend1 = CreateWindow("static", s1,
         //          WS_BORDER |                             //Черная рамка толщиной в пиксель
                     WS_CHILD | WS_VISIBLE,
                     sw_xs, sw_y+4, SizeTxt1.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend1, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
#if defined TEST_FAT1                                        //Проверка FAT1
   hLegend4 = CreateWindow("static", s11,
              //     WS_BORDER |                             //Черная рамка толщиной в пиксель
                     SS_CENTER | WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx, sw_y+4, SizeTxt11.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend4, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   hLegend5 = CreateWindow("static", s12,
              //     WS_BORDER |                             //Черная рамка толщиной в пиксель
                     SS_CENTER | WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx+SizeTxt11.cx, sw_y+4, SizeTxt12.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend5, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   HWND hLostCl = CreateWindow("static", s13,
            //       WS_BORDER |                             //Черная рамка толщиной в пиксель
                     WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx+SizeTxt11.cx+SizeTxt12.cx, sw_y+4, SizeTxt13.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLostCl, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
#else
   hLegend2 = CreateWindow("static", s2,
          //         WS_BORDER |                             //Черная рамка толщиной в пиксель
                     SS_CENTER | WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx, sw_y+4, SizeTxt2.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend2, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   hLegend3 = CreateWindow("static", s3,
          //         WS_BORDER |                             //Черная рамка толщиной в пиксель
                     SS_CENTER | WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx+SizeTxt2.cx, sw_y+4, SizeTxt3.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend3, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   hLegend4 = CreateWindow("static", s4,
          //         WS_BORDER |                             //Черная рамка толщиной в пиксель
                     SS_CENTER | WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx+SizeTxt2.cx+SizeTxt3.cx, sw_y+4, SizeTxt4.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend4, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   if(numErr == 0) return TRUE;
   hLegend5 = CreateWindow("static", s5,
          //         WS_BORDER |                             //Черная рамка толщиной в пиксель
                     SS_CENTER | WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx+SizeTxt2.cx+SizeTxt3.cx+SizeTxt4.cx, sw_y+4, SizeTxt5.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend5, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
#endif
   return TRUE;
}

//------------------------------------------------------------------------------

#pragma argsused
static void MapHDDWinM_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT CodeNotify)
{
   switch(id)
   {
#if defined TEST_FAT1_VOST                                 //Проверка и восстановление FAT1 на потерянные кластеры
      case IDC_VOST: Vost_FAT1();                          //Чистка FAT1 от потерянных кластеров
                     break;
#endif
      case IDOK:  DestroyWindow(hwnd);
                  break;
   }
}

//------------------------------------------------------------------------------

#pragma argsused
static LRESULT CALLBACK WndProc_MapHDD_W1(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   switch(Msg)
   {
      HANDLE_MSG(hwnd, WM_PAINT, MapHDDWin1_OnPaint);
      default: return DefWindowProc(hwnd, Msg, wParam, lParam);
  }
}

//------------------------------------------------------------------------------

#pragma argsused
static void MapHDDWinM_OnDestroy(HWND hwnd)
{
#if defined TEST_FAT1                                        //Проверка FAT1
   MyFreeMem(reinterpret_cast<void**>(&cTestFAT1));
#endif
   MyFreeMem(reinterpret_cast<void**>(&Map));                                   //Память под карту HDD
   MapHDD_WinM = NULL;
}

//------------------------------------------------------------------------------

#pragma argsused
static LRESULT CALLBACK WndProc_MapHDD_WM(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   switch(Msg)
   {
      case WM_CTLCOLORSTATIC:
        if(HWND(lParam) == hLegend2)
        {  SetTextColor((HDC)wParam, RGB(255, 255, 255));
           SetBkColor((HDC)wParam, ColFu);                   //Цвет полностью занятого блока
           return (BOOL)FonBrush;
        }
        if(HWND(lParam) == hLegend3)
        {  //SetTextColor((HDC)wParam, Col4);
           SetBkColor((HDC)wParam, Col_P);                   //Цвет частично занятого/свободдного блока
           return (BOOL)FonBrush;
        }
        if(HWND(lParam) == hLegend4)
        {  //SetTextColor((HDC)wParam, Col4);
           SetBkColor((HDC)wParam, ColFr);                   //Цвет полностью свободного блока
           return (BOOL)FonBrush;
        }
        if(HWND(lParam) == hLegend5)
        {  SetTextColor((HDC)wParam, RGB(255, 255, 255));
           SetBkColor((HDC)wParam, ColEr);                   //Цвет блока с ошибкой
           return (BOOL)FonBrush;
        }
        return TRUE;

      HANDLE_MSG(hwnd, WM_CREATE,  MapHDDWinM_OnCreate);
      HANDLE_MSG(hwnd, WM_COMMAND, MapHDDWinM_OnCommand);
      HANDLE_MSG(hwnd, WM_DESTROY, MapHDDWinM_OnDestroy);
      default: return DefWindowProc(hwnd, Msg, wParam, lParam);
  }
}

//------------------------------------------------------------------------------

#define sw_ys  24                                            //Позиция верхней границы окна дерева !!! Эта же константа есть в модуле вывода карты

HWND Create_MapHDDWin(void)                                  //Создание окна
{
   RECT WRect;                                               //Размеры рабочей области окна
   GetWindowRect(MainWin, &WRect);                           //Получили размеры рабочей области окна
   int STit  = GetSystemMetrics(SM_CYCAPTION);               //Высота заголовка
   int SizeBordX = GetSystemMetrics(SM_CXFIXEDFRAME);        //Размер бордюра
   int SizeBordY = GetSystemMetrics(SM_CYFIXEDFRAME);        //Размер бордюра
   int WinScrS_x = WRect.right - WRect.left - 2 - 2*SizeBordX;
   int WinScrS_y = WRect.bottom - WRect.top - 2 - STit - 2*SizeBordY - sw_ys;
   int Win_ScrSt_x = WRect.left + 1 + SizeBordX;
   int Win_ScrSt_y = WRect.top + 1 + STit + SizeBordY + sw_ys;
   MapHDD_WinM = CreateWindow(MapWM_Name, MapWM_Name,
                          WS_POPUP | WS_VISIBLE | WS_OVERLAPPED | WS_THICKFRAME,
                          Win_ScrSt_x, Win_ScrSt_y, WinScrS_x, WinScrS_y,
                          MainWin, NULL, MainInst, NULL);
   return MapHDD_WinM;
}

//------------------------------------------------------------------------------

int  View_Map_HDD(void)                                      //Карта занятости HDD
{
   if(maxZapFAT1 == 0xFFFFFFFF) return 0;                    //Диск не найден, размеры неизвестны
   if(MapHDD_WinM != NULL) return 0;                         //Окно уже на экране
#if defined TEST_FAT1                                        //Проверка FAT1
   if(Ctrl_FAT1() < 0) return -1;                            //Проверка структуры служебных записей
   int ret = 0;
   if(Create_MapHDDWin() == NULL) ret = -1;                  //Создание окна
   return ret;
#else
   if(Create_MapHDDWin() == NULL) return -1;                 //Создание окна
   return 0;
#endif
}

//------------------------------------------------------------------------------

BOOL Register_MapHDDWin(void)                                //Регистрация окна
{
   WNDCLASS WndClass;

   WndClass.style         = CS_HREDRAW | CS_VREDRAW;         //Внешний вид окна
   WndClass.lpfnWndProc   = WndProc_MapHDD_WM;               //Имя оконной процедуры
   WndClass.cbClsExtra    = 0;
   WndClass.cbWndExtra    = 0;
   WndClass.hInstance     = MainInst;
   WndClass.hIcon         = NULL;
   WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);     //Форма курсора
   WndClass.hbrBackground = FonBrush;                        //Цвет окна
   WndClass.lpszMenuName  = NULL;                            //Меню окна
   WndClass.lpszClassName = MapWM_Name;                      //Имя класа окна
   if(!RegisterClass(&WndClass)) return FALSE;

   WndClass.style         = CS_HREDRAW | CS_VREDRAW;         //Внешний вид окна
   WndClass.lpfnWndProc   = WndProc_MapHDD_W1;               //Имя оконной процедуры
   WndClass.cbClsExtra    = 0;
   WndClass.cbWndExtra    = 0;
   WndClass.hInstance     = MainInst;
   WndClass.hIcon         = NULL;
   WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);     //Форма курсора
//   WndClass.hbrBackground = GetStockBrush(WHITE_BRUSH);     //Цвет окна
   WndClass.hbrBackground = FonBrush;//GetStockBrush(LTGRAY_BRUSH);   //Цвет окна
   WndClass.lpszMenuName  = NULL;                            //Меню окна
   WndClass.lpszClassName = MapW1_Name;                      //Имя класа окна
   return RegisterClass(&WndClass);
}

#endif



