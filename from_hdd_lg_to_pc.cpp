
//                                    04.11.2009
#if defined(__BORLANDC__)
#include <condefs.h>
#endif // defined(__BORLANDC__)

#include "from_hdd_lg_to_pc.h"

//==============================================================================
   HINSTANCE MainInst;                                       //������������� ����������
   HWND  MainWin;                                            //������� ����
   HWND  hwndTree;                                           //���� ������
   DWORD FonLTGRAY;                                          //������� ����
   HBRUSH FonBrush;                                          //������� �����
   HICON MyIcon;                                             //
   HFONT MyFont, MyFont_Cur, MyFontSm;
   HWND hCopy, hConf, hClose, hClear, hSizeHDD;              //���� ������
   HWND hMap, hToTxt, hPrShift;                              //���� ������
   HWND hNumSel, hSizeSel, hAbout;                           //���� ������
   HWND hWriteFi, hWriteFo, hNew_Fo, hRenFi, hDelFi;         //������ ��� ������
   HWND hCorr67;
   int prHDD_LG_Yes;                                         //�������, ����������� HDD: 0-����, 1-�� ������
#if defined EMULATOR_HDD
   const char *NVER_TXT = N_VER_TXT"-emul";                  //����� � "� ���������"
#else
   const char *NVER_TXT = N_VER_TXT;
#endif
#if defined OUT_TEST                                         //��������� ���������
   char *AppName = " From_HDD_LG_to_PC ("N_VER_TXT".dump."TEST_N_VER_TXT") (The special test version with creation of a dump for Val238)";
#else
#if defined TEST_WIN
   char *AppName = " From_HDD_LG_to_PC TEST  TEST  TEST  TEST  TEST  TEST  TEST  TEST  TEST ";
#else
#if defined EMULATOR_HDD
   char *AppName = " From_HDD_LG_to_PC ("N_VER_TXT"-emul)";
#else
   char *AppName = " From_HDD_LG_to_PC ("N_VER_TXT")";
#endif
#endif
#endif
static char *ClassName = "Val238";
#define KK 39
static BYTE MyName[] = { 'V'-KK, 'a'-KK, 'l'-KK, '2'-KK, '3'-KK, '8'-KK };
static HANDLE hMutex;

//------------------------------------------------------------------------------

#pragma argsused
static void M_Win_OnClose(HWND hwnd)                         //���� ���������� ������ �������� ����
{
   if(prProcCopy == FALSE)                                   //������� �������� �����������
   {  Error1((Lan+105)->msg); return;  }                     //Error1("��� ������ �� ��������� �������� ������� �����������.");
   DestroyWindow(hwnd);                                      //��������� ����
}

//------------------------------------------------------------------------------

#pragma argsused
static void M_Win_OnDestroy(HWND hwnd)
{
   if(MyFont != NULL) DeleteObject(MyFont);                  //������ �����
   if(MyFontSm != NULL) DeleteObject(MyFontSm);              //������ �����
   if(MyFont_Cur != NULL) DeleteObject(MyFont_Cur);          //������ �����
   if(FonBrush != NULL) DeleteObject(FonBrush);              //������� �����
   CloseFile(&hDrive);                                       //������� ������� ����
   MyFreeMem(&(void*)FAT1);
   MyFreeMem(&(void*)FAT2);
   MyFreeMem(&(void*)aTree);
   MyFreeMem(&(void*)MMe);
   MyFreeMem(&(void*)tabMME);                                //������� ������������ ����
#if defined EMULATOR_HDD
   MyFreeMem(&(void*)inBuf);
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

#define sw_d    6                                            //���������� �� ������� ���� ���� �� ������� ���� ������
#define sw_hb  24                                            //������ ������
#define sw_lx  100                                           //������ ������
#define sw_lx2 120                                           //������ ������
#define sw_dx  20                                            //���������� ���� ���������
#define sw_xs   6                                            //������� ����� ������� ���� ������
#define sw_ys  24                                            //������� ������� ������� ���� ������ !!! ��� �� ��������� ���� � ������ ������ �����

#pragma argsused
static BOOL M_Win_OnCreate(HWND hwnd, CREATESTRUCT FAR *lpCreateStruct)
{
   RECT WRect;                                               //������� ������� ������� ����
   GetClientRect(hwnd, &WRect);                              //�������� ������� ������� ������� ����
   if(CreateMyFont() < 0) return FALSE;                      //�������� ����������� �����
   int sw_x1 = WRect.right / 2 - sw_lx / 2 - 2*sw_lx - 2*sw_dx;
   int sw_x2 = WRect.right / 2 - sw_lx / 2 - sw_lx - sw_dx;
   int sw_x3 = WRect.right / 2 - sw_lx / 2;
   int sw_x4 = WRect.right / 2 + sw_lx / 2 + sw_dx;
   int sw_x5 = WRect.right / 2 + sw_lx / 2 + sw_lx + 2*sw_dx;
#if defined WRITE_YES                                        //����� ������ ��������
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
   int sw_ls = WRect.right - 2 * sw_xs;                      //������ ���� ������
   int sw_hs = WRect.bottom - sw_ys - sw_hb - 3 * sw_d - ddh2;//������ ���� ������

#if defined WRITE_YES                                        //����� ������ ��������
   int sw_y2 = WRect.bottom - sw_hb - 2*sw_d - ddh2;
#endif
   hSizeHDD = CreateWindowEx(WS_EX_STATICEDGE, "button", "", //������ �������� �����
                    WS_CHILD | WS_VISIBLE,
                    sw_xs, sw_ys-22, 560, 22, hwnd, HMENU(IDC_SIZE_HDD),
                    MainInst, NULL);
   SendMessage(hSizeHDD, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hPrShift = CreateWindow("static", "",                     //������� ��������� �������
//                    WS_BORDER |                              //������ ����� �������� � �������
                    SS_RIGHT | WS_CHILD | WS_VISIBLE,
                    sw_xs+sw_ls-200, sw_ys-18, 200, 18, hwnd, HMENU(IDC_PR_SHIFT),
                    MainInst, NULL);
   SendMessage(hPrShift, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hwndTree = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, "Tree View", //���� ������
                    TVS_HASBUTTONS |
                    TVS_HASLINES |
                    TVS_DISABLEDRAGDROP |
               //   TVS_FULLROWSELECT |
               //   TVS_SINGLEEXPAND |                       //��� ������� CTRL ������� �������� ������
               //   TVS_TRACKSELECT |                        //��� �������� ���� ������� �������������� �������������� �����
                    TVS_LINESATROOT |
               //   TVS_CHECKBOXES |                         //����� ������� � ������ ���������� ChekBox � ������� ����� ������� � ������� �������
                    TVS_SHOWSELALWAYS | WS_VISIBLE | WS_CHILD | WS_BORDER,
                    sw_xs, sw_ys, sw_ls, sw_hs, hwnd, HMENU(IDC_TREEBOX),
                    MainInst, NULL);
   SendMessage(hwndTree, WM_SETFONT, (WPARAM)MyFont_Cur, LPARAM(TRUE));
// TV_SORTCB psort;
// psort.hParent = psort.lpfnCompare = CompareFunc;
// TreeView_SortChildrenCB(hwndTree, &psort, 0);

   hNumSel = CreateWindow("static", "",                      //����� ��������� ������
//                    WS_BORDER |                              //������ ����� �������� � �������
                    WS_CHILD | WS_VISIBLE,
                    sw_xs, sw_y1-4, 160, 18, hwnd, HMENU(IDC_NUM_SEL),
                    MainInst, NULL);
   SendMessage(hNumSel, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   hSizeSel = CreateWindow("static", "",                     //��������� ������ ��������� ������
//                    WS_BORDER |                              //������ ����� �������� � �������
               //   WS_BORDER | SS_CENTER |                  //������ ����� �������� � �������
                    WS_CHILD | WS_VISIBLE,
                    sw_xs, sw_y1+12, 160, 18, hwnd, HMENU(IDC_SIZE_SEL),
                    MainInst, NULL);
   SendMessage(hSizeSel, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hCopy = CreateWindow("button", (Lan+4)->msg,              //"���������� ����",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_x1, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDC_COPY),
                     MainInst, NULL);
   SendMessage(hCopy, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hClear = CreateWindow("button", (Lan+123)->msg,           //"��������",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
                     sw_x2, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDC_CLEAR),
                     MainInst, NULL);
   SendMessage(hClear, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hToTxt = CreateWindow("button", (Lan+146)->msg,           //������ txt
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                     sw_x3, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDC_TO_TXT),
                     MainInst, NULL);
   SendMessage(hToTxt, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hConf = CreateWindow("button", (Lan+5)->msg,              //"���������",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_x4, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDC_CONFIG),
                     MainInst, NULL);
   SendMessage(hConf, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

   hClose = CreateWindow("button", (Lan+6)->msg,             //"�����"
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_x5, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDCANCEL),
                     MainInst, NULL);
   SendMessage(hClose, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

#if defined WRITE_YES                                        //����� ������ ��������
   if(Conf.WriteYes == 1)
   {
      hWriteFi = CreateWindow("button", (Lan+177)->msg,      //"����������� ����� �� HDD LG"
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xx1, sw_y2, sw_lx2, sw_hb, hwnd, HMENU(IDC_WRITE_FI),
                     MainInst, NULL);
      SendMessage(hWriteFi, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

      hWriteFo = CreateWindow("button", (Lan+178)->msg,      //"����������� ����� �� HDD LG"
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xx2, sw_y2, sw_lx2, sw_hb, hwnd, HMENU(IDC_WRITE_FO),
                     MainInst, NULL);
      SendMessage(hWriteFo, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

      hNew_Fo = CreateWindow("button", (Lan+179)->msg,       //"�������� ����� �� HDD LG"
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xx3, sw_y2, sw_lx2, sw_hb, hwnd, HMENU(IDC_NEW_FO),
                     MainInst, NULL);
      SendMessage(hNew_Fo, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

      hRenFi = CreateWindow("button", (Lan+163)->msg,       //"�������������"
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xx4, sw_y2, sw_lx2, sw_hb, hwnd, HMENU(IDC_REN_FI),
                     MainInst, NULL);
      SendMessage(hRenFi, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));

      hDelFi = CreateWindow("button", (Lan+187)->msg,       //"�������"
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
   HIMAGELIST hImageList;                                    //������ ��� ������ ������
   hImageList = ImageList_Create(16, 16, ILC_COLOR16, 15, 15);
   hBitMap = LoadBitmap(MainInst, MAKEINTRESOURCE(IDB_ICON));
   ImageList_Add(hImageList, hBitMap, NULL);                                                                                      // Macro: Attach the image, to the image list
   SendMessage(hwndTree, TVM_SETIMAGELIST, 0, (LPARAM)hImageList);
   return TRUE;
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL NoPosChanged(HWND hwnd, WINDOWPOS *Pos)          //������ ��������� �������� � ��������� ����
{
   Pos->flags |= SWP_NOSIZE; //SWP_NOMOVE;                   //��������� ��������� �������� ����
   return FALSE;
}

//------------------------------------------------------------------------------

#pragma argsused
LRESULT M_Win_OnNotify(HWND hwnd, int idFrom, NMHDR *pnmhdr)
{
// if(pnmhdr->code == NM_RETURN)                             //����� Enter
//    return ChangeSelect();                                 //��������� ������ ������ � ������
   if(pnmhdr->code == TVN_KEYDOWN)                           //������ �����-�� �������
   {  TV_KEYDOWN *ptvkd = (TV_KEYDOWN *)pnmhdr;
      switch(ptvkd->wVKey)
      {  case VK_INSERT:                                     //������ ������� Insert
                return ChangeSelect_and_Down();              //��������� ������ ������ � ������
//       case VK_UP:
//       case VK_DOWN:
//              return ChangeSelect_for_UpDown(ptvkd);       //��������� ������ ������ � ������
      }
      return false;
   }

   if(pnmhdr->code == NM_DBLCLK)                             //������� click ����� ������� �� ������
   {  int sShift = ((GetKeyState(VK_SHIFT) & 0xFF00) == 0) ? 0 : 1;  //�������� ��������� ������� Shift
      return ChangeSelect(sShift);                           //��������� ������ ������ � ������
   }
/*
   if(pnmhdr->code == NM_CLICK)                              //��������� click �� ������
     if(numSel > 0)                                          //����� ��������� ���� ��� ��������� ������
     {  TV_ITEM tvi;                                         //���� ���� ��������� �����, �� ������� ���� �� ���������� �����
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
static void M_Win_Size(HWND hwnd, WPARAM wParam, LPARAM lParam, long fn) //��������� �������� ����
{
   if(wParam == SIZE_MINIMIZED) prWinMinMax = 1;             //0-���� �����������, 1-���� ���������
   if(wParam == SIZE_RESTORED)
   {  prWinMinMax = 0;                                       //0-���� �����������, 1-���� ���������
      SetWindowText(MainWin, AppName);                       //������������ ���������
   }
}

//------------------------------------------------------------------------------

#pragma argsused
static void M_Win_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT CodeNotify)
{
   switch(id)
   {  case IDC_COPY:     CopyFiles();                        //���������� �����������
                         break;
      case IDC_CLEAR:    ClearSelect();                      //������ ���� ���������
                         break;
      case IDC_CONFIG:   NewConfig();                        //��������� ������������
                         break;
      case IDCANCEL:     SendMessage(hwnd, WM_DESTROY, 0, 0);//���������� ��������� � �������� ����
                         break;
      case IDC_SIZE_HDD: View_Map_HDD();                     //����� ��������� HDD
                         break;
      case IDC_ABOUT:    About();                            //����� ����������
                         break;
      case IDC_TO_TXT:   OutTxt();                           //������ ������ � ��������� ����
                         break;
#if defined WRITE_YES                                        //����� ������ ��������
      case IDC_WRITE_FI: WriteFileToHDD(0);  break;          //������ ������ �� ����
      case IDC_WRITE_FO: WriteFileToHDD(1);  break;          //������ ������ �� ����
      case IDC_NEW_FO:   WriteFileToHDD(2);  break;          //������ ������ �� ����
      case IDC_REN_FI:   WriteFileToHDD(3);  break;          //��������������
      case IDC_DEL_FI:   WriteFileToHDD(4);  break;          //��������
      case IDC_CORR67:   Correct67Sec();     break;          //��������� ����������� 67 �������
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
     HANDLE_MSG(hwnd, WM_SIZE,    M_Win_Size);               //��������� �������� ����
     HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING, NoPosChanged);   //��������� ��������� ����
     default: return DefWindowProc(hwnd, Msg, wParam, lParam);
  }
}

//------------------------------------------------------------------------------

static BOOL Register(HINSTANCE hInst)                        //����������� ����
{
   WNDCLASS WndClass;

   WndClass.style         = CS_HREDRAW | CS_VREDRAW;         //������� ��� ����
   WndClass.lpfnWndProc   = WndProc;                         //��� ������� ���������
   WndClass.cbClsExtra    = 0;
   WndClass.cbWndExtra    = 0;
   WndClass.hInstance     = hInst;
   WndClass.hIcon         = MyIcon;
// WndClass.hIcon         = LoadIcon(hInst, NULL);
   WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);     //����� �������
   WndClass.hbrBackground = FonBrush;//GetStockBrush(LTGRAY_BRUSH);   //���� ����
// WndClass.lpszMenuName  = "Menu1";                         //���� ����
   WndClass.lpszMenuName  = NULL;                            //���� ����
   WndClass.lpszClassName = ClassName;                       //��� ����� ����
   return RegisterClass(&WndClass);
}

//------------------------------------------------------------------------------

static HWND CreateNewWin(HINSTANCE hInst, int nCmdShow)      //�������� ����
{
   int WinScrS_x = 982;
   int WinScrS_y = 680;                                      //������ ������
   int ScrSize_x = GetSystemMetrics(SM_CXFULLSCREEN);
   int ScrSize_y = GetSystemMetrics(SM_CYFULLSCREEN) + GetSystemMetrics(SM_CYCAPTION);
   if(WinScrS_x > ScrSize_x) WinScrS_x = ScrSize_x;
   if(WinScrS_y > ScrSize_y) WinScrS_y = ScrSize_y;
   int Win_ScrSt_x = (ScrSize_x - WinScrS_x) / 2;            //������� ������ ����
   int Win_ScrSt_y = (ScrSize_y - WinScrS_y) / 2;            //������� ������ ����

   MainWin = CreateWindow(ClassName, AppName, // WS_OVERLAPPEDWINDOW,
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, // | WS_MAXIMIZEBOX
                          Win_ScrSt_x, Win_ScrSt_y, WinScrS_x, WinScrS_y,
                          NULL, NULL, hInst, NULL);
   if(MainWin == NULL)  return MainWin;
   ShowWindow(MainWin, nCmdShow);                            //�������� ������ ��� ��������� ����
   UpdateWindow(MainWin);                                    //������� ������ ��� ������������� ���� ��������� WM_PAINT
   return MainWin;
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL CALLBACK DlgPust(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch(Message)
   {   case WM_INITDIALOG:
           SetClassLong(hDlg, GCL_HICON, (LONG)MyIcon);
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
   LoadConfig();                                             //�������� ������������
   hMutex = CreateMutex(NULL, TRUE, "From_HDD_LG_to_PC");
   if(GetLastError() == ERROR_ALREADY_EXISTS)
      if(MsgYesNo3(AppName, (Lan+0)->msg, (Lan+1)->msg) == 'N')//if(MsgYesNo3(AppName, "��������� ��� ��������.", "�� ����������� �� ������� ��� ����� ����� ?") == 'N') return FALSE;
      {  CloseHandle(hMutex);  return FALSE; }
   prProcCopy = TRUE;                                        //������� �������� �����������
   MainInst = hInst;
   MyIcon = LoadIcon(MainInst, MAKEINTRESOURCE(IDI_ICON1));
                                          //���� ��� ���
   CreateDialog(MainInst, MAKEINTRESOURCE(IDD_DLG_PUST), MainWin, (DLGPROC)DlgPust); //��� �������� ���� ������ �� ���� ����������� ��������
// if(FindWindow(ClassName, AppName) != 0)
//    if(MsgYesNo3(AppName, (Lan+0)->msg, (Lan+1)->msg) == 'N') return FALSE; //if(MsgYesNo3(AppName, "��������� ��� ��������.", "�� ����������� �� ������� ��� ����� ����� ?") == 'N') return FALSE;
// LoadConfig();                                             //�������� ������������
   InitCommonControls();                                     //����������� ����������
   FonLTGRAY = GetSysColor(COLOR_BTNFACE);                   //������� ���� - ���� ������(�����)
   FonBrush = CreateSolidBrush(FonLTGRAY);                   //������� �����
   if(FonBrush == NULL)
   {  Error1((Lan+2)->msg);  return FALSE;  }                //"������ ��� �������� ������� �����."
   if(!hPrevInstance)
     if(!Register(hInst))  return FALSE;
   if(CreateNewWin(hInst, nCmdShow) == NULL) return FALSE;
   OSVERSIONINFO VersionInformation;
   VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&VersionInformation);
   if(!(VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT && //Win NT ������� � 4 ������
        VersionInformation.dwMajorVersion >= 4))
   {  Error1((Lan+3)->msg); return FALSE; }                  //"������ ��������� �� �������� � ������������ �������� Windows 95/98/ME."
   if(Ctrl_Adm() < 0) return FALSE;                          //�������� ������� ���� ��������������
   if(!Register_MapHDDWin()) return FALSE;                   //����������� ����
#if defined OUT_TEST
   if(!Register_SpecWin()) return FALSE;
   if(Create_SpecWin() == NULL) return FALSE;
   Add_SpecSpis(AppName);
   AddToSpecSpis_VerWin();                                   //������ � ���� ������ Windows
#endif
#if defined TEST_WIN
   if(!Register_TestWin()) return FALSE;
   if(Create_TestWin() == NULL) return FALSE;
#endif
   prHDD_LG_Yes = 0;                                         //�������, ����������� HDD: 0-����, 1-�� ������
#if defined TEST_IDX
   prHDD_LG_Yes = 1;                                         //�������, ����������� HDD: 0-����, 1-�� ������
   EnableWindow(hwndTree, FALSE);                            //���� ���� �� ������, �� �������� � ���������
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
   if(Read_HDD() < 0)                                        //����� HDD �� ��������� LG � �������� ��������� ����������
   {  prHDD_LG_Yes = 1;                                      //�������, ����������� HDD: 0-����, 1-�� ������
      EnableWindow(hwndTree, FALSE);                         //���� ���� �� ������, �� �������� � ��������� �� ��� �����
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

