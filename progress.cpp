
//                             11.08.2008
#ifndef __ProgressBar__
#define __ProgressBar__

#include "from_hdd_lg_to_pc.h"

//============================ ProgressBar =====================================
void EnableW(bool T_F);                                      //Изменение активности кнопок
void InitProgressBar1(DWORD Size, char *Msg);
int  ProgressBar1(DWORD Size);
void Close_ProgressBar1(void);
void InitProgressBar1_2(DWORD Size, char *Msg);              //Функция прогресса для одного файла
void InitProgressBar2_2(DWORD Size, char *Msg);              //Функция прогресса для нескольких файлов
int  ProgressBar2(DWORD Size);
void ScipProgressBar2(DWORDLONG SizeF);                      //Отработка прогресса для пропускаемых файлов
void Close_ProgressBar1_2(void);
void Close_ProgressBar2_2(void);
     void (*InitProgressBar)(DWORD Size, char *Msg);
     int  (*ProgressBar)(DWORD Size);
     void (*Close_ProgressBar)(void);
     bool prProcCopy;                                        //Признак процесса копирования (чтобы блокировать закрытие окна крестиком)
     int  prWinMinMax;                                       //0-окно распахнутое, 1-окно свернутое

static int    prDlg;                                         //Признак диалога 0-один прогресс, 1-два прогресса
static int    PrCansel;                                      //Признак отмены режима
static HWND   hwndDlg;
static HWND   PrBar;
static RECT   rect_pr;                                       //Координаты прогресса во второй секции статус-бара
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
static RECT   rect_pr2;                                      //Координаты прогресса во второй секции статус-бара
static HDC    DC_PR2;
static HPEN   Pen2, OldPen2;
static HBRUSH FBrush2, OldBrush2;
static double Delta2, Poz2;
static long   NewPoz2;
static long   MakeC2, NumC2;
static DWORD  TEnd2, tStart2;
static double OneProc2, proc2;
static long   newproc2;
static DWORDLONG SumSizeA, SumSize1;                         //Суммарный размер скопированных данных

//------------------------------------------------------------------------------

void EnableW(bool T_F)                                       //Изменение активности кнопок
{
   prProcCopy = T_F;                                         //Признак процесса копирования
   EnableWindow(hwndTree, T_F);                              //Окно дерева
   EnableWindow(hCopy, T_F);                                 //Окна кнопок
   EnableWindow(hConf, T_F);                                 //Окна кнопок
   EnableWindow(hClose, T_F);                                //Окна кнопок
   EnableWindow(hClear, T_F);                                //Окна кнопок
   EnableWindow(hSizeHDD, T_F);
   EnableWindow(hToTxt, T_F);
   EnableWindow(hAbout, T_F);                                //Окна кнопок
   if(num_Sel == 0) EnableWindow(hClear, FALSE);
#if defined WRITE_YES                                        //Режим записи разрешен
   if(Conf.WriteYes == 1)
   {  EnableWindow(hWriteFi, T_F);                           //Окна кнопок
      EnableWindow(hWriteFo, T_F);                           //Окна кнопок
      EnableWindow(hNew_Fo, T_F);                            //Окна кнопок
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
             PrCansel = 0;                                   //Признак отмены режима
             CenterDlg(hDlg, +40);                           //Центрирование окна диалога в главном окне
             SetDlgItemText(hDlg, IDCANCEL, (Lan+62)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT6, (Lan+63)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT7, (Lan+64)->msg);
             if(prDlg == 1)                                  //Признак диалога 0-один прогресс, 1-два прогресса
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
                     if(PrCansel == 1) return TRUE;          //Боремся с несколькими нажатиями
                     if(MsgYesNo1((Lan+8)->msg)== 'Y') PrCansel = 1;//Признак отмены режима //if(MsgYesNo1("Вы уверены, что хотите прервать процесс ?")== 'Y') PrCansel = 1;//Признак отмены режима
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
   EnableW(FALSE);                                           //Изменение активности кнопок
   prDlg = 0;                                                //Признак диалога 0-один прогресс, 1-два прогресса
   hwndDlg = CreateDialog(MainInst, MAKEINTRESOURCE(IDD_DLG_PROGR_BAR), MainWin, DlgProgressBar);
   SetDlgItemText(hwndDlg, IDC_STATICTEXT1, Msg);
   PrBar = GetDlgItem(hwndDlg, IDC_PROGRESS);
   DC_PR = GetDC(PrBar);
   GetClientRect(PrBar, &rect_pr);                           //Получили размеры рабочей области окна

   Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 128));
   OldPen = SelectPen(DC_PR, Pen);
   FBrush = CreateSolidBrush(RGB(0, 0, 128));
   OldBrush = SelectBrush(DC_PR, FBrush);

   NewPoz = rect_pr.left;
   Poz = NewPoz;
   Delta = double(rect_pr.right - rect_pr.left + 1) / Size;
   OneProc = 1000.0 / (rect_pr.right - rect_pr.left + 1);
   newproc = 0;
   NumC = Size;                                              //Число циклов
   MakeC = 0;                                                //Число выполненных циклов
   TEnd = tStart = tSt1 = GetTickCount();                    //Текущее время от запуска Windows
   SumSizeA = SumSize1 = 0;                                  //Суммарный размер скопированных данных
}

//------------------------------------------------------------------------------

static void OutTime(double Sec, int CTRL)                    //Вывод времени
{
   char  Ss[80];

   DWORD S = DWORD(Sec);
   DWORD M = S / 60;                                         //Число минут до конца процесса
   S -= M * 60;                                              //Число секунд до конца процесса
   DWORD H = M / 60;                                         //Число часов до конца процесса
   M -= H * 60;                                              //Число минут до конца процесса
   wsprintf(Ss, "%02ld:%02ld:%02ld", H, M, S);
   SetDlgItemText(hwndDlg, CTRL, Ss);
}

//------------------------------------------------------------------------------

int ProgressBar1(DWORD Size)
{
   MSG  msg;

   if(hwndDlg == NULL) return 0;                             //Диалог уже закрыт
   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           //Проверка наличия сообщения
   {  TranslateMessage(&msg);
      DispatchMessage(&msg);                                 //Нужно для перерисовки окна
   }
// while(PeekMessage(&msg, hwndDlg, 0, 0, PM_REMOVE))        //Проверка наличия сообщения
// {  TranslateMessage(&msg);
//    DispatchMessage(&msg);                                 //Нужно для анализа нажатия кнопки
// }
   if(PrCansel == 1)                                         //Признак отмены режима
   {  Close_ProgressBar();                                   //Закрали диалог
      return -1;                                             //Признак отмены режима
   }
   MakeC++;                                                  //Число выполненных циклов
   SumSizeA += Size;                                         //Суммарный размер скопированных данных
   SumSize1 += Size;                                         //Суммарный размер скопированных данных
   DWORD tEnd = GetTickCount();                              //Текущее время от запуска Windows
   if(tEnd - TEnd > 1000)                                    //Прошло более 0.1 секунды
   {  TEnd = tEnd;                                           //Текущее время становится последним
      double Razn = (tEnd - tStart) / 1000.0;                //Число секунд от начала процесса
      OutTime(Razn, IDC_STATICTEXT5);                        //Вывод времени от начала процесса
      double S = Razn * (NumC - MakeC) / MakeC;              //В секундах до окончания процесса
      OutTime(S, IDC_STATICTEXT3);                           //Вывод времени до окончания процесса
      double SpeedA = SumSizeA / Razn / 1024.0 / 1024.0;     //Скорость копирования в Mбайтах / с
      if(tEnd - tSt1 != 0)
      {  double Speed1 = SumSize1 / ((tEnd - tSt1) / 1000.0) / 1024.0 / 1024.0; //Мгновенная Скорость копирования в Mбайтах / с
         char Ss[200];
         sprintf(Ss, "%.1lf Mb/s (%.1lf Mb/s)", Speed1, SpeedA);
         SetDlgItemText(hwndDlg, IDC_STATICTEXT2, Ss);
      }
      tSt1 = GetTickCount();                                 //Текущее время от запуска Windows
      SumSize1 = 0;                                          //Суммарный размер скопированных данных
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
      if(prWinMinMax == 1)                                   //0-окно распахнутое, 1-окно свернутое
      {  sprintf(s1, "%s\% From_HDD_LG_to_PC", Ss);
         SetWindowText(MainWin, s1);                         //Процент выполнения в качестве имени окна
      }
   }
   return 0;
}

//------------------------------------------------------------------------------

void Close_ProgressBar1(void)
{
   EnableW(TRUE);                                            //Изменение активности кнопок
   if(hwndDlg == NULL) return;                               //Диалог уже закрыт
   SetDlgItemText(hwndDlg, IDC_STATICTEXT4, "100.0");

   SelectObject(DC_PR, OldBrush);                            //Восстановили старую кисть
   DeleteObject(FBrush);                                     //Удалили новую кисть
   SelectPen(DC_PR, OldPen);                                 //Восстановили старое перо
   DeleteObject(Pen);                                        //Удалили новое перо
// Pen1 = CreatePen(PS_SOLID, 1, FonLTGRAY);
// OldPen1 = SelectPen(DC_PR1, Pen1);
// OldBrush1 = SelectBrush(DC_PR1, FonBrush);
// Rectangle(DC_PR1, rect_pr1.left, rect_pr1.top, rect_pr1.right+2, rect_pr1.bottom);
// SelectObject(DC_PR1, OldBrush1);                          //Восстановили старую кисть
// SelectPen(DC_PR1, OldPen1);                               //Восстановили старое перо
// DeleteObject(Pen1);                                       //Удалили новое перо
   ReleaseDC(PrBar, DC_PR);                                  //Освободили контекст устройства
   EndDialog(hwndDlg, IDC_OK);
   hwndDlg = NULL;
   if(prWinMinMax == 1)                                      //0-окно распахнутое, 1-окно свернутое
   {  SetWindowText(MainWin, "100\% From_HDD_LG_to_PC");
    //SetWindowText(MainWin, AppName);                       //Восстановили исходное имя окна
    //ShowWindow(MainWin, SW_RESTORE);                       //Распахнули окно
   }
}

//------------------------------------------------------------------------------

void InitProgressBar1_2(DWORD Size, char *Msg)               //Функция прогресса для одного файла
{
   if(hwndDlg == NULL) return;                               //Инициализации диалога не было
   SetDlgItemText(hwndDlg, IDC_STATICTEXT1, Msg);
   PrBar = GetDlgItem(hwndDlg, IDC_PROGRESS1);
   DC_PR = GetDC(PrBar);
   GetClientRect(PrBar, &rect_pr);                           //Получили размеры рабочей области окна

   Pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
   OldPen = SelectPen(DC_PR, Pen);
   FBrush = CreateSolidBrush(RGB(255, 255, 255));
   OldBrush = SelectBrush(DC_PR, FBrush);
   Rectangle(DC_PR, rect_pr.left, rect_pr.top, rect_pr.right, rect_pr.bottom);
   SelectObject(DC_PR, OldBrush);                            //Восстановили старую кисть
   DeleteObject(FBrush);                                     //Удалили новую кисть
   SelectPen(DC_PR, OldPen);                                 //Восстановили старое перо
   DeleteObject(Pen);                                        //Удалили новое перо

   Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 128));
   OldPen = SelectPen(DC_PR, Pen);
   FBrush = CreateSolidBrush(RGB(0, 0, 128));
   OldBrush = SelectBrush(DC_PR, FBrush);

   NewPoz = rect_pr.left;
   Poz = NewPoz;
   Delta = double(rect_pr.right - rect_pr.left + 1) / Size;
   OneProc = 1000.0 / (rect_pr.right - rect_pr.left + 1);
   newproc = 0;
   NumC = Size;                                              //Число циклов
   MakeC = 0;                                                //Число выполненных циклов
   SumSize1 = 0;                                             //Суммарный размер скопированных данных
   TEnd = tStart = tSt1 = GetTickCount();                    //Текущее время от запуска Windows
}

//------------------------------------------------------------------------------

void InitProgressBar2_2(DWORD Size, char *Msg)               //Функция прогресса для нескольких файлов
{
   EnableW(FALSE);                                           //Изменение активности кнопок
   prDlg = 1;                                                //Признак диалога 0-один прогресс, 1-два прогресса
   hwndDlg = CreateDialog(MainInst, MAKEINTRESOURCE(IDD_DLG_PROGR_BAR2), MainWin, DlgProgressBar);
   SetDlgItemText(hwndDlg, IDC_STATICTEXT11, Msg);
   PrBar2 = GetDlgItem(hwndDlg, IDC_PROGRESS2);

   DC_PR2 = GetDC(PrBar2);
   GetClientRect(PrBar2, &rect_pr2);                         //Получили размеры рабочей области окна
/*
   Pen2 = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
   OldPen2 = SelectPen(DC_PR2, Pen2);
   FBrush2 = CreateSolidBrush(RGB(255, 255, 255));
   OldBrush2 = SelectBrush(DC_PR2, FBrush2);
   Rectangle(DC_PR2, rect_pr2.left, rect_pr2.top, rect_pr2.right, rect_pr2.bottom);
   SelectObject(DC_PR2, OldBrush2);                          //Восстановили старую кисть
   DeleteObject(FBrush2);                                    //Удалили новую кисть
   SelectPen(DC_PR2, OldPen2);                               //Восстановили старое перо
   DeleteObject(Pen2);                                       //Удалили новое перо
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
   NumC2 = Size;                                             //Число циклов
   MakeC2 = 0;                                               //Число выполненных циклов
   TEnd2 = tStart2 = GetTickCount();                         //Текущее время от запуска Windows
   SumSizeA = SumSize1 = 0;                                  //Суммарный размер скопированных данных
}

//------------------------------------------------------------------------------

int  ProgressBar2(DWORD Size)
{
   MSG  msg;

   if(hwndDlg == NULL) return 0;                             //Диалог уже закрыт
   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           //Проверка наличия сообщения
   {  TranslateMessage(&msg);
      DispatchMessage(&msg);                                 //Нужно для перерисовки окна
   }
// while(PeekMessage(&msg, hwndDlg, 0, 0, PM_REMOVE))        //Проверка наличия сообщения
// {  TranslateMessage(&msg);
//    DispatchMessage(&msg);                                 //Нужно для анализа нажатия кнопки
// }
   if(PrCansel == 1)                                         //Признак отмены режима
   {  Close_ProgressBar();                                   //Закрали диалог
      return -1;                                             //Признак отмены режима
   }

   MakeC++;                                                  //Число выполненных циклов
   DWORD tEnd = GetTickCount();                              //Текущее время от запуска Windows
   if(tEnd - TEnd > 1000)                                    //Прошло более 0.1 секунды
   {  TEnd = tEnd;                                           //Текущее время становится последним
      double Razn = (tEnd - tStart) / 1000.0;                //Число секунд от начала процесса
      OutTime(Razn, IDC_STATICTEXT5);                        //Вывод времени от начала процесса
      double S = Razn * (NumC - MakeC) / MakeC;              //В секундах до окончания процесса
      OutTime(S, IDC_STATICTEXT3);                           //Вывод времени до окончания процесса
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

   if(hwndDlg == NULL) return 0;                             //Диалог уже закрыт
   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           //Проверка наличия сообщения
   {  TranslateMessage(&msg);
      DispatchMessage(&msg);                                 //Нужно для анализа нажатия кнопки
   }
// while(PeekMessage(&msg, hwndDlg, 0, 0, PM_REMOVE))        //Проверка наличия сообщения
// {  TranslateMessage(&msg);
//    DispatchMessage(&msg);                                 //Нужно для анализа нажатия кнопки
// }
   if(PrCansel == 1)                                         //Признак отмены режима
   {  Close_ProgressBar();                                   //Закрали диалог
      return -1;                                             //Признак отмены режима
   }

   MakeC2++;                                                 //Число выполненных циклов
// DWORD tEnd = GetTickCount();                              //Текущее время от запуска Windows
   SumSizeA += Size;                                         //Суммарный размер скопированных данных
   SumSize1 += Size;                                         //Суммарный размер скопированных данных
   if(tEnd - TEnd2 > 1000)                                   //Прошло более 0.1 секунды
   {  TEnd2 = tEnd;                                          //Текущее время становится последним
      double Razn = (tEnd - tStart2) / 1000.0;               //Число секунд от начала процесса
      OutTime(Razn, IDC_STATICTEXT15);                       //Вывод времени от начала процесса
      double S = Razn * (NumC2 - MakeC2) / MakeC2;           //В секундах до окончания процесса
      OutTime(S, IDC_STATICTEXT13);                          //Вывод времени до окончания процесса
      double SpeedA = SumSizeA / Razn / 1024.0 / 1024.0;     //Скорость копирования в Мбайтах / с
      if(tEnd - tSt1 != 0)
      {  double Speed1 = SumSize1/ ((tEnd - tSt1) / 1000.0) / 1024.0 / 1024.0; //Мгновенная Скорость копирования в Mбайтах / с
         char Ss[200];
         sprintf(Ss, "%.1lf Mb/s (%.1lf Mb/s)", Speed1, SpeedA);
         SetDlgItemText(hwndDlg, IDC_STATICTEXT2, Ss);
      }
      tSt1 = GetTickCount();                                 //Текущее время от запуска Windows
      SumSize1 = 0;                                          //Суммарный размер скопированных данных
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
      if(prWinMinMax == 1)                                   //0-окно распахнутое, 1-окно свернутое
      {  sprintf(s1, "%s\% From_HDD_LG_to_PC", Ss);
         SetWindowText(MainWin, s1);                         //Процент выполнения в качестве имени окна
      }
   }
   return 0;
}

//------------------------------------------------------------------------------

void  ScipProgressBar2(DWORDLONG SizeF)
{
   DWORD numC = DWORD((SizeF + sCl_B - 1) / sCl_B);
   MakeC2 += numC;                                           //Число выполненных циклов
//   SumSizeA += SizeF;                                        //Суммарный размер скопированных данных

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
      if(prWinMinMax == 1)                                    //0-окно распахнутое, 1-окно свернутое
      {  sprintf(s1, "%s\% From_HDD_LG_to_PC", Ss);
         SetWindowText(MainWin, s1);                          //Процент выполнения в качестве имени окна
      }
   }
}

//------------------------------------------------------------------------------

void Close_ProgressBar1_2(void)
{
   if(hwndDlg == NULL) return;                               //Диалог уже закрыт
   SetDlgItemText(hwndDlg, IDC_STATICTEXT4, "100.0");

   SelectObject(DC_PR, OldBrush);                            //Восстановили старую кисть
   DeleteObject(FBrush);                                     //Удалили новую кисть
   SelectPen(DC_PR, OldPen);                                 //Восстановили старое перо
   DeleteObject(Pen);                                        //Удалили новое перо
   ReleaseDC(PrBar, DC_PR);                                  //Освободили контекст устройства
}

//------------------------------------------------------------------------------

void Close_ProgressBar2_2(void)
{
   EnableW(TRUE);                                            //Изменение активности кнопок
   if(hwndDlg == NULL) return;                               //Диалог уже закрыт
   SetDlgItemText(hwndDlg, IDC_STATICTEXT14, "100.0");

   SelectObject(DC_PR2, OldBrush2);                          //Восстановили старую кисть
   DeleteObject(FBrush2);                                    //Удалили новую кисть
   SelectPen(DC_PR2, OldPen2);                               //Восстановили старое перо
   DeleteObject(Pen2);                                       //Удалили новое перо
   ReleaseDC(PrBar2, DC_PR2);                                //Освободили контекст устройства
   EndDialog(hwndDlg, IDC_OK);
   hwndDlg = NULL;
   if(prWinMinMax == 1)                                      //0-окно распахнутое, 1-окно свернутое
   {  SetWindowText(MainWin, "100\% From_HDD_LG_to_PC");
    //SetWindowText(MainWin, AppName);                       //Восстановили исходное имя окна
    //ShowWindow(MainWin, SW_RESTORE);                       //Распахнули окно
   }
}

#endif

