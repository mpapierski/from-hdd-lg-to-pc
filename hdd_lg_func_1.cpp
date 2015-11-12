
//                                11.08.2008
#ifndef __HDD_LG_FUNC_1__
#define __HDD_LG_FUNC_1__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_func_1 ===================================
void *MyAllocMem(DWORD Size);                                //������ ������������ ������
void MyFreeMem(void **Adr);                                  //������������ ����� ������
int  Error1(char *Msg);
int  Error2(char *Msg1, char *Msg2);
int  Error3(char *Msg1, char *Msg2, char *Msg3);
int  Error4(char *Msg1, char *Msg2, char *Msg3, char *Msg4);
int  ErrorSys1(char *Msg);
int  ErrorSys2(char *Msg1, char *Msg2);
char MsgYesNo1(char *Msg1);
char MsgYesNo2(char *Msg1, char *Msg2);
char MsgYesNo3(char *Msg1, char *Msg2, char *Msg3);
char MsgYesNo4(char *Msg1, char *Msg2, char *Msg3, char *Msg4);
char MsgYesNo5(char *Msg1, char *Msg2, char *Msg3, char *Msg4, char *Msg5);
void Message1(char *Msg1);
void Message2(char *Msg1, char *Msg2);

//------------------------------------------------------------------------------

void *MyAllocMem(DWORD Size)                                 //������ ������������ ������
{
   void *Adr;
   if(Size <= 0)
   {  Error1((Lan+38)->msg);                                 //Error1("������� �������� ������ ������� �����.");
      return NULL;
   }
   if((Adr = VirtualAlloc(NULL, Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE) )== NULL)
      ErrorSys1((Lan+39)->msg);                              //ErrorSys1("��� ��������� ������ ��� ������.");
   return Adr;
}

//------------------------------------------------------------------------------

void MyFreeMem(void **Adr)                                   //������������ ����� ������
{
   if(*Adr == NULL)  return;                                 //������ �� ������
   if(VirtualFree(*Adr, NULL, MEM_RELEASE) == FALSE)
      ErrorSys1((Lan+40)->msg);                              //ErrorSys1("������ ������������ ������.");
   *Adr = NULL;
}

//------------------------------------------------------------------------------

int Error1(char *Msg)
{
#if defined OUT_TEST
   Add_SpecSpis(Msg);                         //���������� ������ � ������
#endif
   MessageBox(MainWin, Msg, (Lan+35)->msg, MB_TOPMOST | MB_OK | MB_ICONSTOP | MB_APPLMODAL);// MessageBox(MainWin, Msg, "������", MB_TOPMOST | MB_OK | MB_ICONSTOP);
   return -1;
}

//------------------------------------------------------------------------------

int Error2(char *Msg1, char *Msg2)
{
   char Ss[4096];
   wsprintf(Ss, "%s\n%s", Msg1, Msg2);
   return Error1(Ss);
}

//------------------------------------------------------------------------------

int Error3(char *Msg1, char *Msg2, char *Msg3)
{
   char Ss[4096];
   wsprintf(Ss, "%s\n%s\n%s", Msg1, Msg2, Msg3);
   return Error1(Ss);
}

//------------------------------------------------------------------------------

int Error4(char *Msg1, char *Msg2, char *Msg3, char *Msg4)
{
   char Ss[4096];
   wsprintf(Ss, "%s\n%s\n%s\n%s", Msg1, Msg2, Msg3, Msg4);
   return Error1(Ss);
}

//------------------------------------------------------------------------------

#define MAX_CHAR 40                                          //��������� ����� ������

int ErrorSys1(char *Msg)
{
   char Ss[2048], MsgErr[512];
   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  MsgErr, 512, NULL);
   int n = lstrlen(MsgErr);                                  //����� �������� � ��������� ���������
   int m = MAX_CHAR;                                         //����� ������
   if(n > m)
   {  for(;;)
      { int i;
        for(i=m; i<n; i++)                                   //����� ����� �� ��������� �����
          if(*(MsgErr+i) == ' ')                             //����� ������ �� ��������
          {  *(MsgErr+i) = '\n';
             m = i + MAX_CHAR + 1;
             break;
          }
        if(n - m < MAX_CHAR || i >= n) break;
      }
   }
#if defined OUT_TEST
   Add_SpecSpis(Msg);                                        //���������� ������ � ������
   Add_SpecSpis(MsgErr);                                     //���������� ������ � ������
#endif
   wsprintf(Ss, "%s\n\n%s:\n%s", Msg, (Lan+37)->msg, MsgErr);// wsprintf(Ss, "%s\n\n��������� ���������:\n%s", Msg, MsgErr);
   MessageBox(MainWin, Ss, (Lan+35)->msg, MB_TOPMOST | MB_OK | MB_ICONSTOP);// MessageBox(MainWin, Ss, "������", MB_TOPMOST | MB_OK | MB_ICONSTOP);
   return -1;
}

//------------------------------------------------------------------------------

int ErrorSys2(char *Msg1, char *Msg2)
{
   char Ss[1024];
   wsprintf(Ss, "%s\n%s", Msg1, Msg2);
   return ErrorSys1(Ss);
}

//------------------------------------------------------------------------------

char MsgYesNo1(char *Msg1)
{
//   if(MessageBox(MainWin, Msg1, (Lan+36)->msg, MB_TOPMOST | MB_YESNO | MB_ICONQUESTION) == IDYES) return 'Y';// if(MessageBox(MainWin, Msg1, "�������� !!!", MB_TOPMOST | MB_YESNO | MB_ICONQUESTION) == IDYES) return 'Y';
   if(MessageBox(NULL, Msg1, (Lan+36)->msg, MB_TOPMOST | MB_YESNO | MB_ICONQUESTION) == IDYES) return 'Y';// if(MessageBox(MainWin, Msg1, "�������� !!!", MB_TOPMOST | MB_YESNO | MB_ICONQUESTION) == IDYES) return 'Y';
   return 'N';
}

//------------------------------------------------------------------------------

char MsgYesNo2(char *Msg1, char *Msg2)
{
   char Ss[4096];
   wsprintf(Ss, "%s\n\n%s", Msg1, Msg2);
   return MsgYesNo1(Ss);
}

//------------------------------------------------------------------------------

char MsgYesNo3(char *Msg1, char *Msg2, char *Msg3)
{
   char Ss[4096];
   wsprintf(Ss, "%s\n%s\n\n%s", Msg1, Msg2, Msg3);
   return MsgYesNo1(Ss);
}

//------------------------------------------------------------------------------

char MsgYesNo4(char *Msg1, char *Msg2, char *Msg3, char *Msg4)
{
   char Ss[4096];
   wsprintf(Ss, "%s\n%s\n%s\n\n%s", Msg1, Msg2, Msg3, Msg4);
   return MsgYesNo1(Ss);
}

//------------------------------------------------------------------------------

char MsgYesNo5(char *Msg1, char *Msg2, char *Msg3, char *Msg4, char *Msg5)
{
   char Ss[4096];
   wsprintf(Ss, "%s\n%s\n%s\n%s\n\n%s", Msg1, Msg2, Msg3, Msg4, Msg5);
   return MsgYesNo1(Ss);
}

//------------------------------------------------------------------------------

void Message1(char *Msg1)
{
   MessageBox(MainWin, Msg1, (Lan+36)->msg, MB_TOPMOST | MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
}

//------------------------------------------------------------------------------

void Message2(char *Msg1, char *Msg2)
{
   char Ss[4096];
   wsprintf(Ss, "%s\n%s", Msg1, Msg2);
   Message1(Ss);
}

#endif

