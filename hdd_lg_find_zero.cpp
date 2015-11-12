
//                               03.11.2009
#ifndef __HDD_LG_FIND_ZERO__
#define __HDD_LG_FIND_ZERO__

#include "from_hdd_lg_to_pc.h"

#if defined FIND_NO_ZERO_SEC

//============================ hdd_lg_find_zero ================================
int  FindNoZeroSec(void);                                    //����� �� ������� �������� � �������� ������

DWORD static NUM_S = 1024; //4096;                                   //����� �������� � ����� ������ ������
DWORD static SIZE_B = 512 * NUM_S;                           //����� ���� � ����� ������ ������ (������ ������)
DWORD static AllNumSec = 156301488;                          //����� ����� �������� �� WinHEX

//------------------------------------------------------------------------------

static int Find_(BYTE *buff)
{
   DWORD nb;
   char Ss[260];
   char *aSs = Ss;
   int indSs = 0;
   int numC = AllNumSec / NUM_S;
   InitProgressBar(numC, "����� ��������� ��������");
   int NumSec = 0;                                           //���������� ����� �������������� �������
   int oldSec = -2;                                          //���������� ����� ����������� �������
   int pr = 0;
   for(int i=0; i<numC; i++)
   {  if(ReadFile(hDrive, buff, SIZE_B, &nb, NULL) == FALSE || nb != SIZE_B)
      {  ErrorSys1("������ ��� ������ ������ ��������.");
         CloseFile(&hDrive);                                 //������� ������� ����
         return -1;
      }
      for(int s=0; s<NUM_S; s++, NumSec++)                   //�� ����� �������� � ������
      {   for(int b=s*512; b<(s+1)*512; b++)                 //�� ������ ����������� � ����������� �������
            if(*(buff + b) != 0x00)
            {  if(NumSec-1 != oldSec)
               {  if(pr > 0)
                  {  if(pr == 1)
                       aSs += sprintf(aSs, " %d", oldSec);
                     else
                       aSs += sprintf(aSs, " <> %d", oldSec);
                     indSs++;
                     if(indSs > 10)
                     {  Add_SpecSpis(Ss);
                        aSs = Ss;
                        indSs = 0;
                     }
                  }
                  aSs += sprintf(aSs, " %d", NumSec);
                  indSs++;
                  if(indSs > 10)
                  {  Add_SpecSpis(Ss);
                     aSs = Ss;
                     indSs = 0;
                  }
                  pr = 0;
               }
               else
                 pr++;
               oldSec = NumSec;
               break;                                        //������ ������ �� �������
            }
      }
      if(ProgressBar(sSecB) < 0) return -1;
   }
   Add_SpecSpis(Ss);
   Add_SpecSpis("����� ������ ��������� ��������.");
   return 0;
}

//------------------------------------------------------------------------------

int FindNoZeroSec(void)                                      //����� �� ������� �������� � �������� ������
{
   char Ss[260];
   Add_SpecSpis("����� �� ������� ��������.");
   sprintf(Ss, "����� �������� �� �����: %d", AllNumSec);
   Add_SpecSpis(Ss);
   BYTE *buff = (BYTE *)MyAllocMem(SIZE_B);                  //������ ��� ����� ������
   if(buff == NULL)  return -1;
   if(SetInFilePointer(0) < 0) return -1;                    //��������� ������� ��������� � �����
   InitProgressBar = InitProgressBar1;
   ProgressBar = ProgressBar1;
   Close_ProgressBar = Close_ProgressBar1;
   int ret = Find_(buff);
   Close_ProgressBar();
   MyFreeMem(&(void*)buff);
   return ret;
}

#endif
#endif

