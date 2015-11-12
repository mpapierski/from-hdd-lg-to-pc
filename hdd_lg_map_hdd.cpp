
//                             24.03.2009
#ifndef __HDD_LG_MAP_HDD__
#define __HDD_LG_MAP_HDD__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_map_hdd ==================================
int  View_Map_HDD(void);                                     //����� ��������� HDD
BOOL Register_MapHDDWin(void);                               //����������� ����

static HWND MapHDD_WinM, MapHDD_WinM1;
static char *MapWM_Name = "MapM_HDD";
static char *MapW1_Name = "Map1_HDD";
#define xM 4                                                 //������ ����������
#define yM 4                                                 //������ ����������
static int nG;                                               //����� ��������������� �� �����������
static int NumCl_Bl;                                         //����� ��������� � ����� ����������
static int numBl;                                            //������� ��������� �����������
static int numErr;                                           //����� ������
static short *Map;                                           //������ ��� ����� ������
static HWND hLegend1, hLegend2, hLegend3, hLegend4, hLegend5;
static COLORREF ColFu = RGB(0, 0, 180);                      //���� ��������� �������� �����
static COLORREF Col_P = RGB(0, 255, 255);                    //���� �������� ��������/����������� �����
static COLORREF ColFr = RGB(0, 180, 0);                      //���� ��������� ���������� �����
static COLORREF ColEr = RGB(255, 0, 0);                      //���� ����� � �������
static DWORD numCl_Err;                                      //����� ������� ��������� �� ���������� � ���������
#define IDC_VOST 3000

//------------------------------------------------------------------------------

static int Init_Map(void)
{
   RECT WRect;                                               //������� ������� ������� ����
   GetClientRect(MapHDD_WinM1, &WRect);                      //�������� ������� ������� ������� ����

   int nZ_FAT = maxZapFAT1 - 2;                              //����� �������
   nG = WRect.right / (xM + 1);                              //����� ��������������� �� �����������
   int nV = WRect.bottom / (yM + 1);                         //����� ��������������� �� ���������
   NumCl_Bl = (nZ_FAT + nV * nG - 1)/ (nV * nG);             //����� ��������� � ����� ����������
   numBl = nZ_FAT / NumCl_Bl;                                //������� ��������� �����������
   Map = (short *)MyAllocMem(numBl * sizeof(short));         //������ ��� ����� ������
   if(Map == NULL)  return -1;

   numErr = 0;                                               //����� ������
#if defined TEST_FAT1                                        //�������� FAT1
   DWORD *FAT_ = cTestFAT1 + 2;
#else
   DWORD *FAT_ = FAT1 + 2;
#endif
   numCl_Err = 0;                                            //����� ������� ��������� �� ���������� � ���������
   for(int i=0; i<nZ_FAT; i++)                               //�� ���� ������� FAT
   {  if(*(FAT_ + i) == 0) continue;                         //���������� ������ �������� � ������� �� ��������
//    if(*(FAT_ + i) == 1) continue;                         //�������, ��� ������ ������� ������������
      numCl_Err++;                                           //����� ������� ��������� ��� "�������"
      int n = i / NumCl_Bl;                                  //������ ����� � ����� HDD
      if(*(FAT_ + i) > maxZapFAT1 && *(FAT_ + i) != 0x0FFFFFFF) //��������� ��������
      { numErr++; *(Map + n) = -1; continue; }               //������� ������
      if(*(Map + n) != -1)                                   //������ � ������ ����� ����� ������������
        (*(Map + n))++;                                      //����������� �������
   }
   return 0;
}

//------------------------------------------------------------------------------

static void Create_MapHDD(HDC DC)                            //����� ��������� HDD
{
   if(Map == NULL) return;                                   //��� ����� HDD
   int y = 1;
   int x = 1;
   for(int i=0,j=1; i<numBl; i++,j++)
   {  COLORREF Col = Col_P;                                  //���� �������� ��������/����������� �����
      if(*(Map + i) == NumCl_Bl) Col = ColFu;                //���� ��������� �������� �����
      if(*(Map + i) == 0)        Col = ColFr;                //���� ��������� ���������� �����
      if(*(Map + i) == -1)       Col = ColEr;                //���� ����� � �������
#if defined TEST_FAT1                                        //�������� FAT1
      if(*(Map + i) != 0)        Col = ColEr;                //�� ������� ���� ��� ������
#endif
      HPEN Pen = CreatePen(PS_SOLID, 1, Col);
      HPEN OldPen = SelectPen(DC, Pen);
      HBRUSH FBrush = CreateSolidBrush(Col);
      HBRUSH OldBrush = SelectBrush(DC, FBrush);
      Rectangle(DC, x, y, x+xM, y+yM);
      SelectObject(DC, OldBrush);                            //������������ ������ �����
      DeleteObject(FBrush);                                  //������� ����� �����
      SelectPen(DC, OldPen);                                 //������������ ������ ����
      DeleteObject(Pen);                                     //������� ����� ����
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
   PAINTSTRUCT PaintStruct;                                  //������� ��������� � ��������
   HDC PaintDC = BeginPaint(hwnd, &PaintStruct);
   Create_MapHDD(PaintDC);                                   //����� ��������� HDD
   EndPaint(hwnd, &PaintStruct);
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL MapHDDWinM_OnCreate(HWND hwnd, CREATESTRUCT FAR *lpCreateStruct)
{
   RECT WRect;                                               //������� ������� ������� ����
   GetClientRect(hwnd, &WRect);                              //�������� ������� ������� ������� ����
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
#if defined TEST_FAT1_VOST                                 //�������� � �������������� FAT1 �� ���������� ��������
   HWND hVost = CreateWindow("button", "����.", //(Lan+6)->msg,
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_xs+sw_ls-80-50, sw_y, 40, 24, hwnd, HMENU(IDC_VOST),
                     MainInst, NULL);
   SendMessage(hVost, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
#endif
   if(Init_Map() < 0) return FALSE;
   char s1[200], s2[200], s3[200], s4[200], s5[200];
   SIZE SizeTxt1, SizeTxt2, SizeTxt3, SizeTxt4, SizeTxt5;
   wsprintf(s1, "%s: %3d", (Lan+136)->msg, NumCl_Bl);        //����� ��������� � ����� �����
   wsprintf(s2, "   %s   ", (Lan+125)->msg);                 //������
   wsprintf(s3, "   %s / %s   ", (Lan+125)->msg, (Lan+126)->msg); //������/��������
   wsprintf(s4, "   %s   ", (Lan+126)->msg);                 //��������
   wsprintf(s5, "   %s = %d  ", (Lan+137)->msg, numErr);     //������ FAT
#if defined TEST_FAT1                                        //�������� FAT1
   char s11[200], s12[200], s13[200];
   SIZE SizeTxt11, SizeTxt12, SizeTxt13;
   wsprintf(s11, "   %s   ", "��� ������");                  //��� ������
   wsprintf(s12, "   %s   ", "���������� ��������");         //���������� ��������
   double maxSize = double(maxZapFAT1 - 1) /1024.0/1024.0/1024.0 * sCl_B;
//   double usedSize = double(writeCl) /1024.0/1024.0/1024.0 * sCl_B;
//   double freeSize = double(max_ZapFAT - 1 - writeCl) /1024.0/1024.0/1024.0 * sCl_B;

//   int lostCl = writeCl+1 - sumClTestFAT1;                   //���� �������� �������� � ���������� ����� � FAT
//   double lostSize = double(lostCl) /1024.0/1024.0/1024.0 * sCl_B;
//   sprintf(s13, "   %s: %d (%.2lf Gb) (%.1lf\%)", "�������� ���������", lostCl, lostSize, lostSize/maxSize*100.0);

   double lostSize = double(numCl_Err) /1024.0/1024.0/1024.0 * sCl_B;
   sprintf(s13, "   %s: %d (%.2lf Gb) (%.1lf\%)", "�������� ���������", numCl_Err, lostSize, lostSize/maxSize*100.0);
#endif
   HDC DC = GetDC(hClose);
   GetTextExtentPoint32(DC, s1, lstrlen(s1), &SizeTxt1);     //����� ��������� � ����� �����
   GetTextExtentPoint32(DC, s2, lstrlen(s2), &SizeTxt2);     //������
   GetTextExtentPoint32(DC, s3, lstrlen(s3), &SizeTxt3);     //������/��������
   GetTextExtentPoint32(DC, s4, lstrlen(s4), &SizeTxt4);     //��������
   GetTextExtentPoint32(DC, s5, lstrlen(s5), &SizeTxt5);     //������ FAT
#if defined TEST_FAT1                                        //�������� FAT1
   GetTextExtentPoint32(DC, s11, lstrlen(s11), &SizeTxt11);  //����� ��������� � ����� �����
   GetTextExtentPoint32(DC, s12, lstrlen(s12), &SizeTxt12);  //������
   GetTextExtentPoint32(DC, s13, lstrlen(s13), &SizeTxt13);  //������/��������
#endif
   ReleaseDC(hClose, DC);
   hLegend1 = CreateWindow("static", s1,
         //          WS_BORDER |                             //������ ����� �������� � �������
                     WS_CHILD | WS_VISIBLE,
                     sw_xs, sw_y+4, SizeTxt1.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend1, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
#if defined TEST_FAT1                                        //�������� FAT1
   hLegend4 = CreateWindow("static", s11,
              //     WS_BORDER |                             //������ ����� �������� � �������
                     SS_CENTER | WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx, sw_y+4, SizeTxt11.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend4, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   hLegend5 = CreateWindow("static", s12,
              //     WS_BORDER |                             //������ ����� �������� � �������
                     SS_CENTER | WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx+SizeTxt11.cx, sw_y+4, SizeTxt12.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend5, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   HWND hLostCl = CreateWindow("static", s13,
            //       WS_BORDER |                             //������ ����� �������� � �������
                     WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx+SizeTxt11.cx+SizeTxt12.cx, sw_y+4, SizeTxt13.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLostCl, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
#else
   hLegend2 = CreateWindow("static", s2,
          //         WS_BORDER |                             //������ ����� �������� � �������
                     SS_CENTER | WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx, sw_y+4, SizeTxt2.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend2, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   hLegend3 = CreateWindow("static", s3,
          //         WS_BORDER |                             //������ ����� �������� � �������
                     SS_CENTER | WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx+SizeTxt2.cx, sw_y+4, SizeTxt3.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend3, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   hLegend4 = CreateWindow("static", s4,
          //         WS_BORDER |                             //������ ����� �������� � �������
                     SS_CENTER | WS_CHILD | WS_VISIBLE,
                     sw_xs+SizeTxt1.cx+SizeTxt2.cx+SizeTxt3.cx, sw_y+4, SizeTxt4.cx, 18, hwnd, HMENU(-1),
                     MainInst, NULL);
   SendMessage(hLegend4, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   if(numErr == 0) return TRUE;
   hLegend5 = CreateWindow("static", s5,
          //         WS_BORDER |                             //������ ����� �������� � �������
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
#if defined TEST_FAT1_VOST                                 //�������� � �������������� FAT1 �� ���������� ��������
      case IDC_VOST: Vost_FAT1();                          //������ FAT1 �� ���������� ���������
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
#if defined TEST_FAT1                                        //�������� FAT1
   MyFreeMem(&(void*)cTestFAT1);
#endif
   MyFreeMem(&(void*)Map);                                   //������ ��� ����� HDD
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
           SetBkColor((HDC)wParam, ColFu);                   //���� ��������� �������� �����
           return (BOOL)FonBrush;
        }
        if(HWND(lParam) == hLegend3)
        {  //SetTextColor((HDC)wParam, Col4);
           SetBkColor((HDC)wParam, Col_P);                   //���� �������� ��������/����������� �����
           return (BOOL)FonBrush;
        }
        if(HWND(lParam) == hLegend4)
        {  //SetTextColor((HDC)wParam, Col4);
           SetBkColor((HDC)wParam, ColFr);                   //���� ��������� ���������� �����
           return (BOOL)FonBrush;
        }
        if(HWND(lParam) == hLegend5)
        {  SetTextColor((HDC)wParam, RGB(255, 255, 255));
           SetBkColor((HDC)wParam, ColEr);                   //���� ����� � �������
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

#define sw_ys  24                                            //������� ������� ������� ���� ������ !!! ��� �� ��������� ���� � ������ ������ �����

HWND Create_MapHDDWin(void)                                  //�������� ����
{
   RECT WRect;                                               //������� ������� ������� ����
   GetWindowRect(MainWin, &WRect);                           //�������� ������� ������� ������� ����
   int STit  = GetSystemMetrics(SM_CYCAPTION);               //������ ���������
   int SizeBordX = GetSystemMetrics(SM_CXFIXEDFRAME);        //������ �������
   int SizeBordY = GetSystemMetrics(SM_CYFIXEDFRAME);        //������ �������
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

int  View_Map_HDD(void)                                      //����� ��������� HDD
{
   if(maxZapFAT1 == 0xFFFFFFFF) return 0;                    //���� �� ������, ������� ����������
   if(MapHDD_WinM != NULL) return 0;                         //���� ��� �� ������
#if defined TEST_FAT1                                        //�������� FAT1
   if(Ctrl_FAT1() < 0) return -1;                            //�������� ��������� ��������� �������
   int ret = 0;
   if(Create_MapHDDWin() == NULL) ret = -1;                  //�������� ����
   return ret;
#else
   if(Create_MapHDDWin() == NULL) return -1;                 //�������� ����
   return 0;
#endif
}

//------------------------------------------------------------------------------

BOOL Register_MapHDDWin(void)                                //����������� ����
{
   WNDCLASS WndClass;

   WndClass.style         = CS_HREDRAW | CS_VREDRAW;         //������� ��� ����
   WndClass.lpfnWndProc   = WndProc_MapHDD_WM;               //��� ������� ���������
   WndClass.cbClsExtra    = 0;
   WndClass.cbWndExtra    = 0;
   WndClass.hInstance     = MainInst;
   WndClass.hIcon         = NULL;
   WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);     //����� �������
   WndClass.hbrBackground = FonBrush;                        //���� ����
   WndClass.lpszMenuName  = NULL;                            //���� ����
   WndClass.lpszClassName = MapWM_Name;                      //��� ����� ����
   if(!RegisterClass(&WndClass)) return FALSE;

   WndClass.style         = CS_HREDRAW | CS_VREDRAW;         //������� ��� ����
   WndClass.lpfnWndProc   = WndProc_MapHDD_W1;               //��� ������� ���������
   WndClass.cbClsExtra    = 0;
   WndClass.cbWndExtra    = 0;
   WndClass.hInstance     = MainInst;
   WndClass.hIcon         = NULL;
   WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);     //����� �������
//   WndClass.hbrBackground = GetStockBrush(WHITE_BRUSH);     //���� ����
   WndClass.hbrBackground = FonBrush;//GetStockBrush(LTGRAY_BRUSH);   //���� ����
   WndClass.lpszMenuName  = NULL;                            //���� ����
   WndClass.lpszClassName = MapW1_Name;                      //��� ����� ����
   return RegisterClass(&WndClass);
}

#endif



