
//                                    04.11.2009
#if defined(__BORLANDC__)
#include <condefs.h>
#endif // defined(__BORLANDC__)

#include "from_hdd_lg_to_pc.h"

//==============================================================================
   HINSTANCE MainInst;                                       //Идентификатор приложения
   HWND  MainWin;                                            //Главное окно
   HWND  hwndTree;                                           //Окно дерева
   DWORD FonLTGRAY;                                          //Фоновый цвет
   HBRUSH FonBrush;                                          //Фоновая кисть
   HICON MyIcon;                                             //
   HFONT MyFont, MyFont_Cur, MyFontSm;
   HWND hCopy, hConf, hClose, hClear, hSizeHDD;              //Окна кнопок
   HWND hMap, hToTxt, hPrShift;                              //Окна кнопок
   HWND hNumSel, hSizeSel, hAbout;                           //Окна кнопок
   HWND hWriteFi, hWriteFo, hNew_Fo, hRenFi, hDelFi;         //Кнопки для записи
   HWND hCorr67;
   int prHDD_LG_Yes;                                         //Признак, присутствия HDD: 0-есть, 1-не найден
#if defined EMULATOR_HDD
   const char *NVER_TXT = N_VER_TXT "-emul";                  //Текст в "О Программе"
#else
   const char *NVER_TXT = N_VER_TXT;
#endif
#if defined OUT_TEST                                         //Заголовок программы
   char *AppName = " From_HDD_LG_to_PC (" N_VER_TXT ".dump." TEST_N_VER_TXT ") (The special test version with creation of a dump for Val238)";
#else
#if defined TEST_WIN
   char *AppName = " From_HDD_LG_to_PC TEST  TEST  TEST  TEST  TEST  TEST  TEST  TEST  TEST ";
#else
#if defined EMULATOR_HDD
   char *AppName = " From_HDD_LG_to_PC (" N_VER_TXT "-emul)";
#else
   char *AppName = " From_HDD_LG_to_PC (" N_VER_TXT ")";
#endif
#endif
#endif
static char *ClassName = "Val238";
#define KK 39
static BYTE MyName[] = { 'V'-KK, 'a'-KK, 'l'-KK, '2'-KK, '3'-KK, '8'-KK };
static HANDLE hMutex;

//------------------------------------------------------------------------------

#pragma argsused
static void M_Win_OnClose(HWND hwnd)                         //Если нажимается кнопка закрытия окна
{
   if(prProcCopy == FALSE)                                   //Признак процесса копирования
   {  Error1((Lan+105)->msg); return;  }                     //Error1("Для выхода из программы прервите процесс копирования.");
   DestroyWindow(hwnd);                                      //Разрушили окно
}

//------------------------------------------------------------------------------

#pragma argsused
static void M_Win_OnDestroy(HWND hwnd)
{
   if(MyFont != NULL) DeleteObject(MyFont);                  //Массив фонта
   if(MyFontSm != NULL) DeleteObject(MyFontSm);              //Массив фонта
   if(MyFont_Cur != NULL) DeleteObject(MyFont_Cur);          //Массив фонта
   if(FonBrush != NULL) DeleteObject(FonBrush);              //Фоновая кисть
   CloseFile(&hDrive);                                       //Закрыли входной файл
   MyFreeMem(reinterpret_cast<void **>(&FAT1));
   MyFreeMem(reinterpret_cast<void **>(&FAT2));
   MyFreeMem(reinterpret_cast<void **>(&aTree));
   MyFreeMem(reinterpret_cast<void **>(&MMe));
   MyFreeMem(reinterpret_cast<void **>(&tabMME));                                //Таблица соответствия имен
#if defined EMULATOR_HDD
   MyFreeMem(MyFreeMem(reinterpret_cast<void **>(&inBuf));
#endif
   PostQuitMessage(0);
}

//------------------------------------------------------------------------------
/*
int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
//The callback function must return a negative value if the first item should precede the second,
//a positive value if the first item should follow the second, or zero if the two items are equivalent.
//The lParam1 and lParam2 parameters correspond to the lParam member of the TV_ITEM structure
//for the two items being compared. The lParamSort parameter corresponds to the lParam member of the TV_SORTCB structure that was passed with the TVM_SORTCHILDRENCB message.
   return -1;
}
*/
//------------------------------------------------------------------------------

#define sw_d    6                                            //Расстояние от нижнего края окна до нижнего края кнопок
#define sw_hb  24                                            //Высота кнопок
#define sw_lx  100                                           //Ширина клавиш
#define sw_lx2 120                                           //Ширина клавиш
#define sw_dx  20                                            //Расстояние меду клавишами
#define sw_xs   6                                            //Позиция левой границы окна дерева
#define sw_ys  24                                            //Позиция верхней границы окна дерева !!! Эта же константа есть в модуле вывода карты

#pragma argsused
static BOOL M_Win_OnCreate(HWND hwnd, CREATESTRUCT FAR *lpCreateStruct)
{
   RECT WRect;                                               //Размеры рабочей области окна
   GetClientRect(hwnd, &WRect);                              //Получили размеры рабочей области окна
   if(CreateMyFont() < 0) return FALSE;                      //Создание логического фонта
   int sw_x1 = WRect.right / 2 - sw_lx / 2 - 2*sw_lx - 2*sw_dx;
   int sw_x2 = WRect.right / 2 - sw_lx / 2 - sw_lx - sw_dx;
   int sw_x3 = WRect.right / 2 - sw_lx / 2;
   int sw_x4 = WRect.right / 2 + sw_lx / 2 + sw_dx;
   int sw_x5 = WRect.right / 2 + sw_lx / 2 + sw_lx + 2*sw_dx;
#if defined WRITE_YES                                        //Режим записи разрешен
   int ddh2 = (Conf.WriteYes != 1) ? 0 : sw_hb;
   int sw_xx1 = WRect.right / 2 - sw_lx2 / 2 - 2*sw_lx2 - 2*sw_dx;
   int sw_xx2 = WRect.right / 2 - sw_lx2 / 2 - sw_lx2 - sw_dx;
   int sw_xx3 = WRect.right / 2 - sw_lx2 / 2;
   int sw_xx4 = WRect.right / 2 + sw_lx2 / 2 + sw_dx;
   int sw_xx5 = WRect.right / 2 + sw_lx2 / 2 + sw_lx2 + 2*sw_dx;
#else
   #define ddh2 0
#endif
   int sw_y1 = WRect.bottom - sw_hb - sw_d;
   int sw_ls = WRect.right - 2 * sw_xs;                      //Ширина окна дерева
   int sw_hs = WRect.bottom - sw_ys - sw_hb - 3 * sw_d - ddh2;//Высота окна дерева

#if defined WRITE_YES                                        //Режим записи разрешен
   int sw_y2 = WRect.bottom - sw_hb - 2*sw_d - ddh2;
#endif
   hSizeHDD = CreateWindowEx(WS_EX_STATICEDGE, "button", "", //Кнопка размеров диска
                    WS_CHILD | WS_VISIBLE,
                    sw_xs, sw_ys-22, 560, 22, hwnd, HMENU(IDC_SIZE_HDD),
                    MainInst, NULL);
   SendMessage(hSizeHDD, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hPrShift = CreateWindow("static", "",                     //Признак выбранной границы
//                    WS_BORDER |                              //Черная рамка толщиной в пиксель
                    SS_RIGHT | WS_CHILD | WS_VISIBLE,
                    sw_xs+sw_ls-200, sw_ys-18, 200, 18, hwnd, HMENU(IDC_PR_SHIFT),
                    MainInst, NULL);
   SendMessage(hPrShift, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hwndTree = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, "Tree View", //Окно дерева
                    TVS_HASBUTTONS |
                    TVS_HASLINES |
                    TVS_DISABLEDRAGDROP |
               //   TVS_FULLROWSELECT |
               //   TVS_SINGLEEXPAND |                       //При нажатии CTRL стрелки сдвигают список
               //   TVS_TRACKSELECT |                        //При движении мыши выводит горизонтальную подчеркивающую линию
                    TVS_LINESATROOT |
               //   TVS_CHECKBOXES |                         //Между иконкой и именем появляется ChekBox в котором можно ставить и убирать галочки
                    TVS_SHOWSELALWAYS | WS_VISIBLE | WS_CHILD | WS_BORDER,
                    sw_xs, sw_ys, sw_ls, sw_hs, hwnd, HMENU(IDC_TREEBOX),
                    MainInst, NULL);
   SendMessage(hwndTree, WM_SETFONT, (WPARAM)MyFont_Cur, LPARAM(TRUE));
// TV_SORTCB psort;
// psort.hParent = psort.lpfnCompare = CompareFunc;
// TreeView_SortChildrenCB(hwndTree, &psort, 0);

   hNumSel = CreateWindow("static", "",                      //Число выбранных файлов
//                    WS_BORDER |                              //Черная рамка толщиной в пиксель
                    WS_CHILD | WS_VISIBLE,
                    sw_xs, sw_y1-4, 160, 18, hwnd, HMENU(IDC_NUM_SEL),
                    MainInst, NULL);
   SendMessage(hNumSel, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   hSizeSel = CreateWindow("static", "",                     //Суммарный размер выбранных файлов
//                    WS_BORDER |                              //Черная рамка толщиной в пиксель
               //   WS_BORDER | SS_CENTER |                  //Черная рамка толщиной в пиксель
                    WS_CHILD | WS_VISIBLE,
                    sw_xs, sw_y1+12, 160, 18, hwnd, HMENU(IDC_SIZE_SEL),
                    MainInst, NULL);
   SendMessage(hSizeSel, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hCopy = CreateWindow("button", (Lan+4)->msg,              //"Копировать файл",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_x1, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDC_COPY),
                     MainInst, NULL);
   SendMessage(hCopy, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hClear = CreateWindow("button", (Lan+123)->msg,           //"Очистить",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
                     sw_x2, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDC_CLEAR),
                     MainInst, NULL);
   SendMessage(hClear, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hToTxt = CreateWindow("button", (Lan+146)->msg,           //Запись txt
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                     sw_x3, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDC_TO_TXT),
                     MainInst, NULL);
   SendMessage(hToTxt, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hConf = CreateWindow("button", (Lan+5)->msg,              //"Настройка",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_x4, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDC_CONFIG),
                     MainInst, NULL);
   SendMessage(hConf, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hClose = CreateWindow("button", (Lan+6)->msg,             //"Выход"
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_x5, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDCANCEL),
                     MainInst, NULL);
   SendMessage(hClose, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

#if defined WRITE_YES                                        //Режим записи разрешен
   if(Conf.WriteYes == 1)
   {
      hWriteFi = CreateWindow("button", (Lan+177)->msg,      //"Копирование файла на HDD LG"
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xx1, sw_y2, sw_lx2, sw_hb, hwnd, HMENU(IDC_WRITE_FI),
                     MainInst, NULL);
      SendMessage(hWriteFi, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

      hWriteFo = CreateWindow("button", (Lan+178)->msg,      //"Копирование папки на HDD LG"
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xx2, sw_y2, sw_lx2, sw_hb, hwnd, HMENU(IDC_WRITE_FO),
                     MainInst, NULL);
      SendMessage(hWriteFo, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

      hNew_Fo = CreateWindow("button", (Lan+179)->msg,       //"Создание папки на HDD LG"
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xx3, sw_y2, sw_lx2, sw_hb, hwnd, HMENU(IDC_NEW_FO),
                     MainInst, NULL);
      SendMessage(hNew_Fo, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

      hRenFi = CreateWindow("button", (Lan+163)->msg,       //"Переименовать"
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xx4, sw_y2, sw_lx2, sw_hb, hwnd, HMENU(IDC_REN_FI),
                     MainInst, NULL);
      SendMessage(hRenFi, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

      hDelFi = CreateWindow("button", (Lan+187)->msg,       //"Удалить"
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xx5, sw_y2, sw_lx2, sw_hb, hwnd, HMENU(IDC_DEL_FI),
                     MainInst, NULL);
      SendMessage(hDelFi, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
      hCorr67 = CreateWindow("button", (Lan+193)->msg,       //"Correction",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_MULTILINE,
                     WRect.right-64, sw_y2+4, 64, 16, hwnd, HMENU(IDC_CORR67),
                     MainInst, NULL);
      SendMessage(hCorr67, WM_SETFONT, (WPARAM)MyFontSm/*MyFont*/, LPARAM(TRUE));
   }
#endif

   char myName[7];
   for(int i=0; i<6; i++) *(myName + i) = char(*(MyName + i) + KK);
   *(myName + 6) = 0;
   hAbout = CreateWindowEx(WS_EX_STATICEDGE, "button", myName,//Val238
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                     WRect.right-50, WRect.bottom-22, 50, 22, hwnd, HMENU(IDC_ABOUT),
                     MainInst, NULL);
   SendMessage(hAbout, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   HBITMAP hBitMap;
   HIMAGELIST hImageList;                                    //Иконки для дерева файлов
   hImageList = ImageList_Create(16, 16, ILC_COLOR16, 15, 15);
   hBitMap = LoadBitmap(MainInst, MAKEINTRESOURCE(IDB_ICON));
   ImageList_Add(hImageList, hBitMap, NULL);                                                                                      // Macro: Attach the image, to the image list
   SendMessage(hwndTree, TVM_SETIMAGELIST, 0, (LPARAM)hImageList);
   return TRUE;
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL NoPosChanged(HWND hwnd, WINDOWPOS *Pos)          //Запрет изменения размеров и положения окна
{
   Pos->flags |= SWP_NOSIZE; //SWP_NOMOVE;                   //Запретить изменение размеров окна
   return FALSE;
}

//------------------------------------------------------------------------------

#pragma argsused
LRESULT M_Win_OnNotify(HWND hwnd, int idFrom, NMHDR *pnmhdr)
{
// if(pnmhdr->code == NM_RETURN)                             //Нажат Enter
//    return ChangeSelect();                                 //Изменение выбора файлов в дерева
   if(pnmhdr->code == TVN_KEYDOWN)                           //Нажата какая-то клавиша
   {  TV_KEYDOWN *ptvkd = (TV_KEYDOWN *)pnmhdr;
      switch(ptvkd->wVKey)
      {  case VK_INSERT:                                     //Нажата клавиша Insert
                return ChangeSelect_and_Down();              //Изменение выбора файлов в дерева
//       case VK_UP:
//       case VK_DOWN:
//              return ChangeSelect_for_UpDown(ptvkd);       //Изменение выбора файлов в дерева
      }
      return false;
   }

   if(pnmhdr->code == NM_DBLCLK)                             //Двойной click левой кнопкой на строке
   {  int sShift = ((GetKeyState(VK_SHIFT) & 0xFF00) == 0) ? 0 : 1;  //Проверка состояния клавиши Shift
      return ChangeSelect(sShift);                           //Изменение выбора файлов в дерева
   }
/*
   if(pnmhdr->code == NM_CLICK)                              //Одиночный click на строке
     if(numSel > 0)                                          //Число выбранных имен при групповом выборе
     {  TV_ITEM tvi;                                         //если есть групповой выбор, то убираем вбор по одиночносу клику
        memset(&tvi, 0, sizeof(tvi));
        HTREEITEM Selected = TreeView_GetSelection(hwndTree);
        if(Selected == NULL) return false;
        tvi.mask = TVIF_PARAM;
        tvi.hItem = Selected;
        tvi.mask = TVIF_STATE;
        tvi.state = !TVIS_SELECTED;
        tvi.stateMask = TVIS_SELECTED;
        if(TreeView_SetItem(hwndTree, &tvi) == -1)  return false;
        return true;
     }
*/
   return false;
}

//------------------------------------------------------------------------------

#pragma argsused
static void M_Win_Size(HWND hwnd, WPARAM wParam, LPARAM lParam, long fn) //Изменение размеров окна
{
   if(wParam == SIZE_MINIMIZED) prWinMinMax = 1;             //0-окно распахнутое, 1-окно свернутое
   if(wParam == SIZE_RESTORED)
   {  prWinMinMax = 0;                                       //0-окно распахнутое, 1-окно свернутое
      SetWindowText(MainWin, AppName);                       //Восстановили заголовок
   }
}

//------------------------------------------------------------------------------

#pragma argsused
static void M_Win_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT CodeNotify)
{
   switch(id)
   {  case IDC_COPY:     CopyFiles();                        //Выполнение копирования
                         break;
      case IDC_CLEAR:    ClearSelect();                      //Снятие всех выделений
                         break;
      case IDC_CONFIG:   NewConfig();                        //Изменение конфигурации
                         break;
      case IDCANCEL:     SendMessage(hwnd, WM_DESTROY, 0, 0);//Завершение программы и закрытие окон
                         break;
      case IDC_SIZE_HDD: View_Map_HDD();                     //Карта занятости HDD
                         break;
      case IDC_ABOUT:    About();                            //Вывод информации
                         break;
      case IDC_TO_TXT:   OutTxt();                           //Запись дерева в текстовый файл
                         break;
#if defined WRITE_YES                                        //Режим записи разрешен
      case IDC_WRITE_FI: WriteFileToHDD(0);  break;          //Запись файлов на диск
      case IDC_WRITE_FO: WriteFileToHDD(1);  break;          //Запись файлов на диск
      case IDC_NEW_FO:   WriteFileToHDD(2);  break;          //Запись файлов на диск
      case IDC_REN_FI:   WriteFileToHDD(3);  break;          //Переименование
      case IDC_DEL_FI:   WriteFileToHDD(4);  break;          //Удаление
      case IDC_CORR67:   Correct67Sec();     break;          //Коррекция содержимого 67 сектора
#endif
   }
}

//------------------------------------------------------------------------------

#pragma argsused
static LRESULT CALLBACK WndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   switch(Msg)
   {  case WM_CTLCOLORSTATIC:
        if(HWND(lParam) == hNumSel || HWND(lParam) == hSizeSel)
        {  SetTextColor((HDC)wParam, RGB(0, 0, 200));
           SetBkColor((HDC)wParam, FonLTGRAY);
           return (BOOL)FonBrush;
        }
        if(HWND(lParam) == hPrShift)
        {  SetTextColor((HDC)wParam, RGB(255, 255, 255));
  //       SetBkColor((HDC)wParam, FonLTGRAY);
           SetBkColor((HDC)wParam, RGB(180,0,0));
           return (BOOL)FonBrush;
        }
        if(HWND(lParam) == hSizeHDD)
        {  SetTextColor((HDC)wParam, RGB(0, 80, 0));
           SetBkColor((HDC)wParam, FonLTGRAY);
           return (BOOL)FonBrush;
        }
        return TRUE;
     HANDLE_MSG(hwnd, WM_CREATE,  M_Win_OnCreate);
     HANDLE_MSG(hwnd, WM_CLOSE,   M_Win_OnClose);
     HANDLE_MSG(hwnd, WM_DESTROY, M_Win_OnDestroy);
//   HANDLE_MSG(hwnd, WM_PAINT,   Decod_OnPaint);
     HANDLE_MSG(hwnd, WM_COMMAND, M_Win_OnCommand);
     HANDLE_MSG(hwnd, WM_NOTIFY,  M_Win_OnNotify);
     HANDLE_MSG(hwnd, WM_SIZE,    M_Win_Size);               //Изменение размеров окна
     HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING, NoPosChanged);   //Изменение положения окна
     default: return DefWindowProc(hwnd, Msg, wParam, lParam);
  }
}

//------------------------------------------------------------------------------

static BOOL Register(HINSTANCE hInst)                        //Регистрация окна
{
   WNDCLASS WndClass;

   WndClass.style         = CS_HREDRAW | CS_VREDRAW;         //Внешний вид окна
   WndClass.lpfnWndProc   = WndProc;                         //Имя оконной процедуры
   WndClass.cbClsExtra    = 0;
   WndClass.cbWndExtra    = 0;
   WndClass.hInstance     = hInst;
   WndClass.hIcon         = MyIcon;
// WndClass.hIcon         = LoadIcon(hInst, NULL);
   WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);     //Форма курсора
   WndClass.hbrBackground = FonBrush;//GetStockBrush(LTGRAY_BRUSH);   //Цвет окна
// WndClass.lpszMenuName  = "Menu1";                         //Меню окна
   WndClass.lpszMenuName  = NULL;                            //Меню окна
   WndClass.lpszClassName = ClassName;                       //Имя класа окна
   return RegisterClass(&WndClass);
}

//------------------------------------------------------------------------------

static HWND CreateNewWin(HINSTANCE hInst, int nCmdShow)      //Создание окна
{
   int WinScrS_x = 982;
   int WinScrS_y = 680;                                      //Размер экрана
   int ScrSize_x = GetSystemMetrics(SM_CXFULLSCREEN);
   int ScrSize_y = GetSystemMetrics(SM_CYFULLSCREEN) + GetSystemMetrics(SM_CYCAPTION);
   if(WinScrS_x > ScrSize_x) WinScrS_x = ScrSize_x;
   if(WinScrS_y > ScrSize_y) WinScrS_y = ScrSize_y;
   int Win_ScrSt_x = (ScrSize_x - WinScrS_x) / 2;            //Позиция начала окна
   int Win_ScrSt_y = (ScrSize_y - WinScrS_y) / 2;            //Позиция начала окна

   MainWin = CreateWindow(ClassName, AppName, // WS_OVERLAPPEDWINDOW,
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, // | WS_MAXIMIZEBOX
                          Win_ScrSt_x, Win_ScrSt_y, WinScrS_x, WinScrS_y,
                          NULL, NULL, hInst, NULL);
   if(MainWin == NULL)  return MainWin;
   ShowWindow(MainWin, nCmdShow);                            //Показать только что созданное окно
   UpdateWindow(MainWin);                                    //Послать только что споявившемуся окну сообщение WM_PAINT
   return MainWin;
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL CALLBACK DlgPust(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch(Message)
   {   case WM_INITDIALOG:
           SetClassLongPtr(hDlg, GCLP_HICON, (LONG_PTR)MyIcon);
           EndDialog(hDlg, FALSE);
           break;
      default: return FALSE;
   }
   return TRUE;
}

//------------------------------------------------------------------------------

#pragma argsused
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
   MSG Msg;

   MainWin = NULL;
   LoadConfig();                                             //Загрузка конфигурации
   hMutex = CreateMutex(NULL, TRUE, "From_HDD_LG_to_PC");
   if(GetLastError() == ERROR_ALREADY_EXISTS)
      if(MsgYesNo3(AppName, (Lan+0)->msg, (Lan+1)->msg) == 'N')//if(MsgYesNo3(AppName, "Программа уже запущена.", "Вы настаивайте на запуске еще одной копии ?") == 'N') return FALSE;
      {  CloseHandle(hMutex);  return FALSE; }
   prProcCopy = TRUE;                                        //Признак процесса копирования
   MainInst = hInst;
   MyIcon = LoadIcon(MainInst, MAKEINTRESOURCE(IDI_ICON1));
                                          //Окна еще нет
   CreateDialog(MainInst, MAKEINTRESOURCE(IDD_DLG_PUST), MainWin, (DLGPROC)DlgPust); //Для привязки моей иконки ко всем стандартным диалогам
// if(FindWindow(ClassName, AppName) != 0)
//    if(MsgYesNo3(AppName, (Lan+0)->msg, (Lan+1)->msg) == 'N') return FALSE; //if(MsgYesNo3(AppName, "Программа уже запущена.", "Вы настаивайте на запуске еще одной копии ?") == 'N') return FALSE;
// LoadConfig();                                             //Загрузка конфигурации
   InitCommonControls();                                     //Подключение библиотеки
   FonLTGRAY = GetSysColor(COLOR_BTNFACE);                   //Фоновый цвет - цвет кнопок(серый)
   FonBrush = CreateSolidBrush(FonLTGRAY);                   //Фоновая кисть
   if(FonBrush == NULL)
   {  Error1((Lan+2)->msg);  return FALSE;  }                //"Ошибка при создании фоновой кисти."
   if(!hPrevInstance)
     if(!Register(hInst))  return FALSE;
   if(CreateNewWin(hInst, nCmdShow) == NULL) return FALSE;
   OSVERSIONINFO VersionInformation;
   VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&VersionInformation);
   if(!(VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT && //Win NT ночиная с 4 версии
        VersionInformation.dwMajorVersion >= 4))
   {  Error1((Lan+3)->msg); return FALSE; }                  //"Данная программа не работает в операционных системах Windows 95/98/ME."
   if(Ctrl_Adm() < 0) return FALSE;                          //Проверка наличия прав администратора
   if(!Register_MapHDDWin()) return FALSE;                   //Регистрация окна
#if defined OUT_TEST
   if(!Register_SpecWin()) return FALSE;
   if(Create_SpecWin() == NULL) return FALSE;
   Add_SpecSpis(AppName);
   AddToSpecSpis_VerWin();                                   //Запись в дамп версии Windows
#endif
#if defined TEST_WIN
   if(!Register_TestWin()) return FALSE;
   if(Create_TestWin() == NULL) return FALSE;
#endif
   prHDD_LG_Yes = 0;                                         //Признак, присутствия HDD: 0-есть, 1-не найден
#if defined TEST_IDX
   prHDD_LG_Yes = 1;                                         //Признак, присутствия HDD: 0-есть, 1-не найден
   EnableWindow(hwndTree, FALSE);                            //Если диск не найден, то остаемся в программе
   EnableWindow(hCopy, FALSE);
   EnableWindow(hToTxt, FALSE);
   EnableWindow(hWriteFi, false);
   EnableWindow(hWriteFo, false);
   EnableWindow(hNew_Fo, false);
   EnableWindow(hRenFi, false);
   EnableWindow(hDelFi, false);
   EnableWindow(hSizeHDD, false);
   EnableWindow(hCorr67, false);
   Read_File_idx();
#else
   if(Read_HDD() < 0)                                        //Поиск HDD из рекордера LG и загрузка служебной информации
   {  prHDD_LG_Yes = 1;                                      //Признак, присутствия HDD: 0-есть, 1-не найден
      EnableWindow(hwndTree, FALSE);                         //Если диск не найден, то остаемся в программе но все гасим
      EnableWindow(hCopy, FALSE);
      EnableWindow(hToTxt, FALSE);
      EnableWindow(hWriteFi, false);
      EnableWindow(hWriteFo, false);
      EnableWindow(hNew_Fo, false);
      EnableWindow(hRenFi, false);
      EnableWindow(hDelFi, false);
      EnableWindow(hSizeHDD, false);
      EnableWindow(hCorr67, false);
   }
#endif
   while(GetMessage(&Msg, NULL, 0, 0))
   {  TranslateMessage(&Msg);
      DispatchMessage(&Msg);
   }
   CloseHandle(hMutex);
   return Msg.wParam;
}

#define WinMain

