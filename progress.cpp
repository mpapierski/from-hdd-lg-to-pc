
//                             11.08.2008
#ifndef __ProgressBar__
#define __ProgressBar__

#include "from_hdd_lg_to_pc.h"

//============================ ProgressBar =====================================
void EnableW(bool T_F);                                      //��������� ���������� ������
void InitProgressBar1(DWORD Size, char *Msg);
int  ProgressBar1(DWORD Size);
void Close_ProgressBar1(void);
void InitProgressBar1_2(DWORD Size, char *Msg);              //������� ��������� ��� ������ �����
void InitProgressBar2_2(DWORD Size, char *Msg);              //������� ��������� ��� ���������� ������
int  ProgressBar2(DWORD Size);
void ScipProgressBar2(DWORDLONG SizeF);                      //��������� ��������� ��� ������������ ������
void Close_ProgressBar1_2(void);
void Close_ProgressBar2_2(void);
     void (*InitProgressBar)(DWORD Size, char *Msg);
     int  (*ProgressBar)(DWORD Size);
     void (*Close_ProgressBar)(void);
     bool prProcCopy;                                        //������� �������� ����������� (����� ����������� �������� ���� ���������)
     int  prWinMinMax;                                       //0-���� �����������, 1-���� ���������

static int    prDlg;                                         //������� ������� 0-���� ��������, 1-��� ���������
static int    PrCansel;                                      //������� ������ ������
static HWND   hwndDlg;
static HWND   PrBar;
static RECT   rect_pr;                                       //���������� ��������� �� ������ ������ ������-����
static HDC    DC_PR;
static HPEN   Pen, OldPen;
static HBRUSH FBrush, OldBrush;
static double Delta, Poz;
static long   NewPoz;
static long   MakeC, NumC;
static DWORD  TEnd, tStart, tSt1;
static double OneProc, proc;
static long   newproc;
static HWND   PrBar2;
static RECT   rect_pr2;                                      //���������� ��������� �� ������ ������ ������-����
static HDC    DC_PR2;
static HPEN   Pen2, OldPen2;
static HBRUSH FBrush2, OldBrush2;
static double Delta2, Poz2;
static long   NewPoz2;
static long   MakeC2, NumC2;
static DWORD  TEnd2, tStart2;
static double OneProc2, proc2;
static long   newproc2;
static DWORDLONG SumSizeA, SumSize1;                         //��������� ������ ������������� ������

//------------------------------------------------------------------------------

void EnableW(bool T_F)                                       //��������� ���������� ������
{
   prProcCopy = T_F;                                         //������� �������� �����������
   EnableWindow(hwndTree, T_F);                              //���� ������
   EnableWindow(hCopy, T_F);                                 //���� ������
   EnableWindow(hConf, T_F);                                 //���� ������
   EnableWindow(hClose, T_F);                                //���� ������
   EnableWindow(hClear, T_F);                                //���� ������
   EnableWindow(hSizeHDD, T_F);
   EnableWindow(hToTxt, T_F);
   EnableWindow(hAbout, T_F);                                //���� ������
   if(num_Sel == 0) EnableWindow(hClear, FALSE);
#if defined WRITE_YES                                        //����� ������ ��������
   if(Conf.WriteYes == 1)
   {  EnableWindow(hWriteFi, T_F);                           //���� ������
      EnableWindow(hWriteFo, T_F);                           //���� ������
      EnableWindow(hNew_Fo, T_F);                            //���� ������
      EnableWindow(hRenFi, T_F);
      EnableWindow(hDelFi, T_F);
      EnableWindow(hCorr67, T_F);
   }
#endif

}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL CALLBACK DlgProgressBar(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch(Message)
   {  case WM_INITDIALOG:
             PrCansel = 0;                                   //������� ������ ������
             CenterDlg(hDlg, +40);                           //������������� ���� ������� � ������� ����
             SetDlgItemText(hDlg, IDCANCEL, (Lan+62)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT6, (Lan+63)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT7, (Lan+64)->msg);
             if(prDlg == 1)                                  //������� ������� 0-���� ��������, 1-��� ���������
             {  SetDlgItemText(hDlg, IDC_STATICTEXT16, (Lan+63)->msg);
                SetDlgItemText(hDlg, IDC_STATICTEXT17, (Lan+64)->msg);
             }
             return TRUE;
      case WM_CTLCOLORSTATIC:
           { int ID = GetDlgCtrlID(HWND(lParam));
             if(ID == IDC_PROGRESS || ID == IDC_PROGRESS1 || ID == IDC_PROGRESS2)
                return (BOOL)GetStockBrush(WHITE_BRUSH);
           } return TRUE;
      case WM_COMMAND:
           switch(LOWORD(wParam))
           {  case IDOK:
              case IDCANCEL:
                     if(PrCansel == 1) return TRUE;          //������� � ����������� ���������
                     if(MsgYesNo1((Lan+8)->msg)== 'Y') PrCansel = 1;//������� ������ ������ //if(MsgYesNo1("�� �������, ��� ������ �������� ������� ?")== 'Y') PrCansel = 1;//������� ������ ������
                     return TRUE;
              default: return FALSE;
           }

      default: break;
   }
   return FALSE;
}

//------------------------------------------------------------------------------

void InitProgressBar1(DWORD Size, char *Msg)
{
   EnableW(FALSE);                                           //��������� ���������� ������
   prDlg = 0;                                                //������� ������� 0-���� ��������, 1-��� ���������
   hwndDlg = CreateDialog(MainInst, MAKEINTRESOURCE(IDD_DLG_PROGR_BAR), MainWin, DlgProgressBar);
   SetDlgItemText(hwndDlg, IDC_STATICTEXT1, Msg);
   PrBar = GetDlgItem(hwndDlg, IDC_PROGRESS);
   DC_PR = GetDC(PrBar);
   GetClientRect(PrBar, &rect_pr);                           //�������� ������� ������� ������� ����

   Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 128));
   OldPen = SelectPen(DC_PR, Pen);
   FBrush = CreateSolidBrush(RGB(0, 0, 128));
   OldBrush = SelectBrush(DC_PR, FBrush);

   NewPoz = rect_pr.left;
   Poz = NewPoz;
   Delta = double(rect_pr.right - rect_pr.left + 1) / Size;
   OneProc = 1000.0 / (rect_pr.right - rect_pr.left + 1);
   newproc = 0;
   NumC = Size;                                              //����� ������
   MakeC = 0;                                                //����� ����������� ������
   TEnd = tStart = tSt1 = GetTickCount();                    //������� ����� �� ������� Windows
   SumSizeA = SumSize1 = 0;                                  //��������� ������ ������������� ������
}

//------------------------------------------------------------------------------

static void OutTime(double Sec, int CTRL)                    //����� �������
{
   char  Ss[80];

   DWORD S = DWORD(Sec);
   DWORD M = S / 60;                                         //����� ����� �� ����� ��������
   S -= M * 60;                                              //����� ������ �� ����� ��������
   DWORD H = M / 60;                                         //����� ����� �� ����� ��������
   M -= H * 60;                                              //����� ����� �� ����� ��������
   wsprintf(Ss, "%02ld:%02ld:%02ld", H, M, S);
   SetDlgItemText(hwndDlg, CTRL, Ss);
}

//------------------------------------------------------------------------------

int ProgressBar1(DWORD Size)
{
   MSG  msg;

   if(hwndDlg == NULL) return 0;                             //������ ��� ������
   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           //�������� ������� ���������
   {  TranslateMessage(&msg);
      DispatchMessage(&msg);                                 //����� ��� ����������� ����
   }
// while(PeekMessage(&msg, hwndDlg, 0, 0, PM_REMOVE))        //�������� ������� ���������
// {  TranslateMessage(&msg);
//    DispatchMessage(&msg);                                 //����� ��� ������� ������� ������
// }
   if(PrCansel == 1)                                         //������� ������ ������
   {  Close_ProgressBar();                                   //������� ������
      return -1;                                             //������� ������ ������
   }
   MakeC++;                                                  //����� ����������� ������
   SumSizeA += Size;                                         //��������� ������ ������������� ������
   SumSize1 += Size;                                         //��������� ������ ������������� ������
   DWORD tEnd = GetTickCount();                              //������� ����� �� ������� Windows
   if(tEnd - TEnd > 1000)                                    //������ ����� 0.1 �������
   {  TEnd = tEnd;                                           //������� ����� ���������� ���������
      double Razn = (tEnd - tStart) / 1000.0;                //����� ������ �� ������ ��������
      OutTime(Razn, IDC_STATICTEXT5);                        //����� ������� �� ������ ��������
      double S = Razn * (NumC - MakeC) / MakeC;              //� �������� �� ��������� ��������
      OutTime(S, IDC_STATICTEXT3);                           //����� ������� �� ��������� ��������
      double SpeedA = SumSizeA / Razn / 1024.0 / 1024.0;     //�������� ����������� � M������ / �
      if(tEnd - tSt1 != 0)
      {  double Speed1 = SumSize1 / ((tEnd - tSt1) / 1000.0) / 1024.0 / 1024.0; //���������� �������� ����������� � M������ / �
         char Ss[200];
         sprintf(Ss, "%.1lf Mb/s (%.1lf Mb/s)", Speed1, SpeedA);
         SetDlgItemText(hwndDlg, IDC_STATICTEXT2, Ss);
      }
      tSt1 = GetTickCount();                                 //������� ����� �� ������� Windows
      SumSize1 = 0;                                          //��������� ������ ������������� ������
   }

   Poz += Delta;
   if(Poz - NewPoz > 1)
   {  NewPoz = int(Poz);
      Rectangle(DC_PR, rect_pr.left, rect_pr.top, NewPoz, rect_pr.bottom);
   }
   proc = Poz * OneProc;
   if(proc - newproc > 1)
   {  char Ss[100], s1[200];
      newproc = int(proc);
      sprintf(Ss, "%.1lf", 0.1 * newproc);
      SetDlgItemText(hwndDlg, IDC_STATICTEXT4, Ss);
      if(prWinMinMax == 1)                                   //0-���� �����������, 1-���� ���������
      {  sprintf(s1, "%s\% From_HDD_LG_to_PC", Ss);
         SetWindowText(MainWin, s1);                         //������� ���������� � �������� ����� ����
      }
   }
   return 0;
}

//------------------------------------------------------------------------------

void Close_ProgressBar1(void)
{
   EnableW(TRUE);                                            //��������� ���������� ������
   if(hwndDlg == NULL) return;                               //������ ��� ������
   SetDlgItemText(hwndDlg, IDC_STATICTEXT4, "100.0");

   SelectObject(DC_PR, OldBrush);                            //������������ ������ �����
   DeleteObject(FBrush);                                     //������� ����� �����
   SelectPen(DC_PR, OldPen);                                 //������������ ������ ����
   DeleteObject(Pen);                                        //������� ����� ����
// Pen1 = CreatePen(PS_SOLID, 1, FonLTGRAY);
// OldPen1 = SelectPen(DC_PR1, Pen1);
// OldBrush1 = SelectBrush(DC_PR1, FonBrush);
// Rectangle(DC_PR1, rect_pr1.left, rect_pr1.top, rect_pr1.right+2, rect_pr1.bottom);
// SelectObject(DC_PR1, OldBrush1);                          //������������ ������ �����
// SelectPen(DC_PR1, OldPen1);                               //������������ ������ ����
// DeleteObject(Pen1);                                       //������� ����� ����
   ReleaseDC(PrBar, DC_PR);                                  //���������� �������� ����������
   EndDialog(hwndDlg, IDC_OK);
   hwndDlg = NULL;
   if(prWinMinMax == 1)                                      //0-���� �����������, 1-���� ���������
   {  SetWindowText(MainWin, "100\% From_HDD_LG_to_PC");
    //SetWindowText(MainWin, AppName);                       //������������ �������� ��� ����
    //ShowWindow(MainWin, SW_RESTORE);                       //���������� ����
   }
}

//------------------------------------------------------------------------------

void InitProgressBar1_2(DWORD Size, char *Msg)               //������� ��������� ��� ������ �����
{
   if(hwndDlg == NULL) return;                               //������������� ������� �� ����
   SetDlgItemText(hwndDlg, IDC_STATICTEXT1, Msg);
   PrBar = GetDlgItem(hwndDlg, IDC_PROGRESS1);
   DC_PR = GetDC(PrBar);
   GetClientRect(PrBar, &rect_pr);                           //�������� ������� ������� ������� ����

   Pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
   OldPen = SelectPen(DC_PR, Pen);
   FBrush = CreateSolidBrush(RGB(255, 255, 255));
   OldBrush = SelectBrush(DC_PR, FBrush);
   Rectangle(DC_PR, rect_pr.left, rect_pr.top, rect_pr.right, rect_pr.bottom);
   SelectObject(DC_PR, OldBrush);                            //������������ ������ �����
   DeleteObject(FBrush);                                     //������� ����� �����
   SelectPen(DC_PR, OldPen);                                 //������������ ������ ����
   DeleteObject(Pen);                                        //������� ����� ����

   Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 128));
   OldPen = SelectPen(DC_PR, Pen);
   FBrush = CreateSolidBrush(RGB(0, 0, 128));
   OldBrush = SelectBrush(DC_PR, FBrush);

   NewPoz = rect_pr.left;
   Poz = NewPoz;
   Delta = double(rect_pr.right - rect_pr.left + 1) / Size;
   OneProc = 1000.0 / (rect_pr.right - rect_pr.left + 1);
   newproc = 0;
   NumC = Size;                                              //����� ������
   MakeC = 0;                                                //����� ����������� ������
   SumSize1 = 0;                                             //��������� ������ ������������� ������
   TEnd = tStart = tSt1 = GetTickCount();                    //������� ����� �� ������� Windows
}

//------------------------------------------------------------------------------

void InitProgressBar2_2(DWORD Size, char *Msg)               //������� ��������� ��� ���������� ������
{
   EnableW(FALSE);                                           //��������� ���������� ������
   prDlg = 1;                                                //������� ������� 0-���� ��������, 1-��� ���������
   hwndDlg = CreateDialog(MainInst, MAKEINTRESOURCE(IDD_DLG_PROGR_BAR2), MainWin, DlgProgressBar);
   SetDlgItemText(hwndDlg, IDC_STATICTEXT11, Msg);
   PrBar2 = GetDlgItem(hwndDlg, IDC_PROGRESS2);

   DC_PR2 = GetDC(PrBar2);
   GetClientRect(PrBar2, &rect_pr2);                         //�������� ������� ������� ������� ����
/*
   Pen2 = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
   OldPen2 = SelectPen(DC_PR2, Pen2);
   FBrush2 = CreateSolidBrush(RGB(255, 255, 255));
   OldBrush2 = SelectBrush(DC_PR2, FBrush2);
   Rectangle(DC_PR2, rect_pr2.left, rect_pr2.top, rect_pr2.right, rect_pr2.bottom);
   SelectObject(DC_PR2, OldBrush2);                          //������������ ������ �����
   DeleteObject(FBrush2);                                    //������� ����� �����
   SelectPen(DC_PR2, OldPen2);                               //������������ ������ ����
   DeleteObject(Pen2);                                       //������� ����� ����
*/
   Pen2 = CreatePen(PS_SOLID, 1, RGB(0, 0, 128));
   OldPen2 = SelectPen(DC_PR2, Pen2);
   FBrush2 = CreateSolidBrush(RGB(0, 0, 128));
   OldBrush2 = SelectBrush(DC_PR2, FBrush2);

   NewPoz2 = rect_pr2.left;
   Poz2 = NewPoz2;
   Delta2 = double(rect_pr2.right - rect_pr2.left + 1) / Size;
   OneProc2 = 1000.0 / (rect_pr2.right - rect_pr2.left + 1);
   newproc2 = 0;
   NumC2 = Size;                                             //����� ������
   MakeC2 = 0;                                               //����� ����������� ������
   TEnd2 = tStart2 = GetTickCount();                         //������� ����� �� ������� Windows
   SumSizeA = SumSize1 = 0;                                  //��������� ������ ������������� ������
}

//------------------------------------------------------------------------------

int  ProgressBar2(DWORD Size)
{
   MSG  msg;

   if(hwndDlg == NULL) return 0;                             //������ ��� ������
   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           //�������� ������� ���������
   {  TranslateMessage(&msg);
      DispatchMessage(&msg);                                 //����� ��� ����������� ����
   }
// while(PeekMessage(&msg, hwndDlg, 0, 0, PM_REMOVE))        //�������� ������� ���������
// {  TranslateMessage(&msg);
//    DispatchMessage(&msg);                                 //����� ��� ������� ������� ������
// }
   if(PrCansel == 1)                                         //������� ������ ������
   {  Close_ProgressBar();                                   //������� ������
      return -1;                                             //������� ������ ������
   }

   MakeC++;                                                  //����� ����������� ������
   DWORD tEnd = GetTickCount();                              //������� ����� �� ������� Windows
   if(tEnd - TEnd > 1000)                                    //������ ����� 0.1 �������
   {  TEnd = tEnd;                                           //������� ����� ���������� ���������
      double Razn = (tEnd - tStart) / 1000.0;                //����� ������ �� ������ ��������
      OutTime(Razn, IDC_STATICTEXT5);                        //����� ������� �� ������ ��������
      double S = Razn * (NumC - MakeC) / MakeC;              //� �������� �� ��������� ��������
      OutTime(S, IDC_STATICTEXT3);                           //����� ������� �� ��������� ��������
   }

   Poz += Delta;
   if(Poz - NewPoz > 1)
   {  NewPoz = int(Poz);
      if(NewPoz > rect_pr.right) NewPoz = rect_pr.right;
      Rectangle(DC_PR, rect_pr.left, rect_pr.top, NewPoz, rect_pr.bottom);
   }
   proc = Poz * OneProc;
   if(proc - newproc > 1)
   {  char Ss[100];
      newproc = int(proc);
      sprintf(Ss, "%.1lf", 0.1* newproc);
      SetDlgItemText(hwndDlg, IDC_STATICTEXT4, Ss);
   }

   if(hwndDlg == NULL) return 0;                             //������ ��� ������
   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           //�������� ������� ���������
   {  TranslateMessage(&msg);
      DispatchMessage(&msg);                                 //����� ��� ������� ������� ������
   }
// while(PeekMessage(&msg, hwndDlg, 0, 0, PM_REMOVE))        //�������� ������� ���������
// {  TranslateMessage(&msg);
//    DispatchMessage(&msg);                                 //����� ��� ������� ������� ������
// }
   if(PrCansel == 1)                                         //������� ������ ������
   {  Close_ProgressBar();                                   //������� ������
      return -1;                                             //������� ������ ������
   }

   MakeC2++;                                                 //����� ����������� ������
// DWORD tEnd = GetTickCount();                              //������� ����� �� ������� Windows
   SumSizeA += Size;                                         //��������� ������ ������������� ������
   SumSize1 += Size;                                         //��������� ������ ������������� ������
   if(tEnd - TEnd2 > 1000)                                   //������ ����� 0.1 �������
   {  TEnd2 = tEnd;                                          //������� ����� ���������� ���������
      double Razn = (tEnd - tStart2) / 1000.0;               //����� ������ �� ������ ��������
      OutTime(Razn, IDC_STATICTEXT15);                       //����� ������� �� ������ ��������
      double S = Razn * (NumC2 - MakeC2) / MakeC2;           //� �������� �� ��������� ��������
      OutTime(S, IDC_STATICTEXT13);                          //����� ������� �� ��������� ��������
      double SpeedA = SumSizeA / Razn / 1024.0 / 1024.0;     //�������� ����������� � ������� / �
      if(tEnd - tSt1 != 0)
      {  double Speed1 = SumSize1/ ((tEnd - tSt1) / 1000.0) / 1024.0 / 1024.0; //���������� �������� ����������� � M������ / �
         char Ss[200];
         sprintf(Ss, "%.1lf Mb/s (%.1lf Mb/s)", Speed1, SpeedA);
         SetDlgItemText(hwndDlg, IDC_STATICTEXT2, Ss);
      }
      tSt1 = GetTickCount();                                 //������� ����� �� ������� Windows
      SumSize1 = 0;                                          //��������� ������ ������������� ������
   }

   Poz2 += Delta2;
   if(Poz2 - NewPoz2 > 1)
   {  NewPoz2 = int(Poz2);
      Rectangle(DC_PR2, rect_pr2.left, rect_pr2.top, NewPoz2, rect_pr2.bottom);
   }
   proc2 = Poz2 * OneProc2;
   if(proc2 - newproc2 > 1)
   {  char Ss[100], s1[200];
      newproc2 = int(proc2);
      sprintf(Ss, "%.1lf", 0.1* newproc2);
      SetDlgItemText(hwndDlg, IDC_STATICTEXT14, Ss);
      if(prWinMinMax == 1)                                   //0-���� �����������, 1-���� ���������
      {  sprintf(s1, "%s\% From_HDD_LG_to_PC", Ss);
         SetWindowText(MainWin, s1);                         //������� ���������� � �������� ����� ����
      }
   }
   return 0;
}

//------------------------------------------------------------------------------

void  ScipProgressBar2(DWORDLONG SizeF)
{
   DWORD numC = DWORD((SizeF + sCl_B - 1) / sCl_B);
   MakeC2 += numC;                                           //����� ����������� ������
//   SumSizeA += SizeF;                                        //��������� ������ ������������� ������

   Poz2 += Delta2 * numC;
   if(Poz2 - NewPoz2 > 1)
   {  NewPoz2 = int(Poz2);
      Rectangle(DC_PR2, rect_pr2.left, rect_pr2.top, NewPoz2, rect_pr2.bottom);
   }
   proc2 = Poz2 * OneProc2;
   if(proc2 - newproc2 > 1)
   {  char Ss[100], s1[200];
      newproc2 = int(proc2);
      sprintf(Ss, "%.1lf", 0.1* newproc2);
      SetDlgItemText(hwndDlg, IDC_STATICTEXT14, Ss);
      if(prWinMinMax == 1)                                    //0-���� �����������, 1-���� ���������
      {  sprintf(s1, "%s\% From_HDD_LG_to_PC", Ss);
         SetWindowText(MainWin, s1);                          //������� ���������� � �������� ����� ����
      }
   }
}

//------------------------------------------------------------------------------

void Close_ProgressBar1_2(void)
{
   if(hwndDlg == NULL) return;                               //������ ��� ������
   SetDlgItemText(hwndDlg, IDC_STATICTEXT4, "100.0");

   SelectObject(DC_PR, OldBrush);                            //������������ ������ �����
   DeleteObject(FBrush);                                     //������� ����� �����
   SelectPen(DC_PR, OldPen);                                 //������������ ������ ����
   DeleteObject(Pen);                                        //������� ����� ����
   ReleaseDC(PrBar, DC_PR);                                  //���������� �������� ����������
}

//------------------------------------------------------------------------------

void Close_ProgressBar2_2(void)
{
   EnableW(TRUE);                                            //��������� ���������� ������
   if(hwndDlg == NULL) return;                               //������ ��� ������
   SetDlgItemText(hwndDlg, IDC_STATICTEXT14, "100.0");

   SelectObject(DC_PR2, OldBrush2);                          //������������ ������ �����
   DeleteObject(FBrush2);                                    //������� ����� �����
   SelectPen(DC_PR2, OldPen2);                               //������������ ������ ����
   DeleteObject(Pen2);                                       //������� ����� ����
   ReleaseDC(PrBar2, DC_PR2);                                //���������� �������� ����������
   EndDialog(hwndDlg, IDC_OK);
   hwndDlg = NULL;
   if(prWinMinMax == 1)                                      //0-���� �����������, 1-���� ���������
   {  SetWindowText(MainWin, "100\% From_HDD_LG_to_PC");
    //SetWindowText(MainWin, AppName);                       //������������ �������� ��� ����
    //ShowWindow(MainWin, SW_RESTORE);                       //���������� ����
   }
}

#endif

