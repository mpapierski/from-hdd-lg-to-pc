
//                               07.08.2008
#ifndef __HDD_LG_TEST0__
#define __HDD_LG_TEST0__

#include "from_hdd_lg_to_pc.h"

#if defined TEST_WIN

//============================ hdd_lg_test0 ====================================
void View_Sec_Hex(BYTE *buff, int pr);                       //����� ����������� �������
void Add_Spis(char *Str);                                    //���������� ������ � ������
HWND Create_TestWin(void);                                   //�������� ����
BOOL Register_TestWin(void);                                 //����������� ����

static HWND TestWin;
static HWND hListBox;
static char *TW_Name = "Test";

//-------------------------------------------------------------------------------

#pragma argsused
void View_Sec_Hex(BYTE *buff, int pr)                        //����� ����������� �������
{
   char Ss[512];
   for(int i=0; i<sSecB; i+=16)                              //�� ����� �����  � 16 ����������
   {  char *Adr = Ss;
      sprintf(Adr, "%04X: ", i);                             //�������� ������� ����� ������ � �������
      Adr += 6;
      for(int j=0; j<16; j++)                                //�� ����� ������ � 16 ����������
      {   sprintf(Adr, " %02X", *(buff+i+j));
          Adr += 3;
          if(j == 7)
          {  *Adr = ' ';   Adr++; }                          //��������� ��������
      }
      *Adr = ' ';   Adr++;                                   //��������� ��������
      *Adr = ' ';   Adr++;                                   //��������� ��������
      *Adr = ' ';   Adr++;                                   //��������� ��������
      *Adr = ' ';   Adr++;                                   //��������� ��������
      for(int j=0; j<16; j++)                                //�� ����� ������ � 16 ����������
      {   *Adr = *(buff+i+j);                                //���������� ��� ������
          if(*Adr < 32) *Adr = '.';                          //��������� ������� ������ �� �����
          Adr++;
          if(j == 7)
          {  *Adr = ' ';   Adr++; }                          //��������� ��������
      }
/*
      if(pr == 1)                                            //����������� ����� ��� �������� [.] � ����� MEDIA
      {  DWORD *DW = (DWORD*)(buff+i);                       //������ ��� 4 ��������
         for(int j=0; j<4; j++) SWAP32(DW+j);
         sprintf(Adr, "%12d%12d%12d%12d", *(DW+0), *(DW+1), *(DW+2), *(DW+3));
         Adr += 48;
      }
*/
      *Adr = 0;
      Add_Spis(Ss);                                          //���������� ������ � ������
   }
}

//------------------------------------------------------------------------------

void Add_Spis(char *Str)                                     //���������� ������ � ������
{
   SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)Str);
   int Poz = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
   SendMessage(hListBox,LB_SETCURSEL, Poz-1, 0);
   UpdateWindow(hListBox);                                    //������������ ���� ����� ������� ������ �����
}

//------------------------------------------------------------------------------

static int SaveFileTxt(void)                                 //���������� �������� � ��������� ����
{
   char Driv[MAXDRIVE], Dir[MAXDIR], Name[MAXFILE], Ext[MAXEXT];
   char NameF[260];

   if(GetModuleFileName(NULL, NameF, sizeof(NameF)) == 0) return -1;
   fnsplit(NameF, Driv, Dir, Name, Ext);                     //��������� ��� �����
   fnmerge(NameF, Driv, Dir, "!mme_db", ".txt");                  //�������� ��� �����
   FILE *PrgF;
   if((PrgF = fopen(NameF, "at")) == NULL)
      return Error2(NameF, "Error open file.");
   char Str[10 * 4096];
   int num = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
   fprintf(PrgF, "==============================================================\n");
   for(int i=0; i<num; i++)
   {  SendMessage(hListBox, LB_GETTEXT, i, (LPARAM)Str);
      fprintf(PrgF, "%s\n", Str);
   }
   fprintf(PrgF, "\n");
   fclose(PrgF);
   return 0;
}

//------------------------------------------------------------------------------

#define sw_d    6                                            //���������� �� ������� ���� ���� �� ������� ���� ������
#define sw_hb  24                                            //������ ������
#define sw_lx 140                                            //������ ������
#define sw_dx  20                                            //���������� ���� ���������
#define sw_xs   6                                            //������� ����� ������� ���� ������
#define sw_ys   6                                            //������� ������� ������� ���� ������ !!! ��� �� ��������� ���� � ������ ������ �����

#pragma argsused
static BOOL TestWin_OnCreate(HWND hwnd, CREATESTRUCT FAR *lpCreateStruct)
{
   RECT WRect;                                              //������� ������� ������� ����
   GetClientRect(hwnd, &WRect);                             //�������� ������� ������� ������� ����
   int sw_x1 = WRect.right / 2 - sw_dx / 2 - sw_lx;
   int sw_x2 = WRect.right / 2 + sw_dx / 2;
   int sw_y1 = WRect.bottom - sw_hb - sw_d;
   int sw_ls = WRect.right - 2 * sw_xs;                      //������ ����
   int sw_hs = WRect.bottom - sw_ys - sw_hb - 2 * sw_d;      //������ ����
   hListBox = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", NULL,
               //   LBS_NOTIFY | LBS_DISABLENOSCROLL | LBS_NOINTEGRALHEIGHT |
                    WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE,
                    sw_xs, sw_ys, sw_ls, sw_hs, hwnd, HMENU(IDC_LISTBOX),
                    MainInst, NULL);
   SendMessage(hListBox, WM_SETFONT, (WPARAM)MyFont_Cur, LPARAM(TRUE));
   SendMessage(hListBox, LB_SETHORIZONTALEXTENT, 10*2048, 0);
   HWND hFile = CreateWindow("button", "�������� � ����",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_x1, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDOK),
                     MainInst, NULL);
   SendMessage(hFile, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   HWND hClose = CreateWindow("button", "�������",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                     sw_x2, sw_y1, sw_lx, sw_hb, hwnd, HMENU(IDCANCEL),
                     MainInst, NULL);
   SendMessage(hClose, WM_SETFONT, (WPARAM)MyFont, LPARAM(TRUE));
   return TRUE;

}

//------------------------------------------------------------------------------

#pragma argsused
static void TestWin_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT CodeNotify)
{
   switch(id)
   {  case IDOK:     SaveFileTxt();             //���������� �������� � ��������� ����
//                   DestroyWindow(hwnd);
                     break;
      case IDCANCEL: DestroyWindow(hwnd);
                     break;
   }
}

//------------------------------------------------------------------------------

#pragma argsused
static LRESULT CALLBACK WndProc_TestW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   switch(Msg)
   {
      HANDLE_MSG(hwnd, WM_CREATE,  TestWin_OnCreate);
//    HANDLE_MSG(hwnd, WM_DESTROY, M_Win_OnDestroy);
//    HANDLE_MSG(hwnd, WM_PAINT,   Decod_OnPaint);
      HANDLE_MSG(hwnd, WM_COMMAND, TestWin_OnCommand);
//    HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING, NoPosChanged); //��������� ��������� ����
      default: return DefWindowProc(hwnd, Msg, wParam, lParam);
  }
}

//------------------------------------------------------------------------------

HWND Create_TestWin(void)                                     //�������� ����
{
   int WinScrS_x = GetSystemMetrics(SM_CXFULLSCREEN) - 20;
   int WinScrS_y = GetSystemMetrics(SM_CYFULLSCREEN) + GetSystemMetrics(SM_CYCAPTION) - 20;                                      //������ ������
   int Win_ScrSt_x = 10;
   int Win_ScrSt_y = 10;
   TestWin = CreateWindow(TW_Name, TW_Name,
                          WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_OVERLAPPED | WS_VISIBLE,
                          Win_ScrSt_x, Win_ScrSt_y, WinScrS_x, WinScrS_y,
                          MainWin, NULL, MainInst, NULL);
   return TestWin;
}

//------------------------------------------------------------------------------

BOOL Register_TestWin(void)                                  //����������� ����
{
   WNDCLASS WndClass;

   WndClass.style         = CS_HREDRAW | CS_VREDRAW;         //������� ��� ����
   WndClass.lpfnWndProc   = WndProc_TestW;                   //��� ������� ���������
   WndClass.cbClsExtra    = 0;
   WndClass.cbWndExtra    = 0;
   WndClass.hInstance     = MainInst;
// WndClass.hIcon         = MyIcon;
   WndClass.hIcon         = LoadIcon(MainInst, NULL);
   WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);     //����� �������
   WndClass.hbrBackground = FonBrush;//GetStockBrush(LTGRAY_BRUSH);   //���� ����
   WndClass.lpszMenuName  = NULL;                            //���� ����
   WndClass.lpszClassName = TW_Name;                         //��� ����� ����
   return RegisterClass(&WndClass);
}

#endif

#endif

