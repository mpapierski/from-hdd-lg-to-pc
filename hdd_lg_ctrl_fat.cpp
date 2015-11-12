
//                              17.02.2008
#ifndef __HDD_LG_CTRL_FAT__
#define __HDD_LG_CTRL_FAT__

#include "from_hdd_lg_to_pc.h"

#if defined TEST_FAT1                                        //�������� FAT1

//============================ hdd_lg_ctrl_fat =================================
int  Ctrl_FAT1(void);                                        //�������� ��������� ��������� �������
int  Vost_FAT1(void);                                        //������ FAT1 �� ���������� ���������
     DWORD *cTestFAT1;                                       //����� ������� FAT ������� �������
     DWORD sumClTestFAT1;                                    //��������� ���������� ������� ��������� ��� ������������ FAT1

static int cRead_Media_Dir(DWORD nCl);                       //������ � �������� ������ ��������

//------------------------------------------------------------------------------

static int cCtrl_FAT_For_File(DWORD clSt, LONGLONG SizeF)    //�������� ������������ ����� ����� � FAT
{
   DWORD nCl = clSt;                                         //������� ����� �������� ����� ������� �������� �����
   DWORD wSizeB = sCl_B;                                     //����� ������������ ���� ����� ����� ���� � ��������
   LONGLONG SizeToEnd = SizeF;                               //����� ���� ������� �������� � �������
   DWORD numCl = DWORD((SizeF + sCl_B - 1) / sCl_B);         //����� ��������� ����������� ��� ���������� ����� ������� �������
   for(DWORD i=0; i<numCl; i++)                              //�� ����� ��������� ������� ����� ��� ������� �����
   {  if(nCl == 0x0FFFFFFF)
        return Error1((Lan+12)->msg);                        //return Error1("���������� ������ ������� ����� ������� FAT.");
      if(*(FAT1 + nCl) == 0)
        return Error1((Lan+11)->msg);                        //return Error1("���������� �������������� �������� FAT � ������ �� ������� �����.");
      if(*(cTestFAT1 + nCl) == 0)
         return Error1("������ �� ��������� ������� FAT1.");
//      if(*(FAT + nCl) != 0x0FFFFFFF && nCl + 1 != *(FAT + nCl))
//         Error1("���� ������������");
      *(cTestFAT1 + nCl) = 0;                                //�������� ������������� ������� �������� FAT
      sumClTestFAT1++;                                       //��������� ���������� ������� ���������
      if(wSizeB > SizeToEnd) wSizeB = DWORD(SizeToEnd);      //������ ������� ������ ������ ������� ��������
      SizeToEnd -= wSizeB;                                   //����� ���� ������� �������� ��������
      nCl = *(FAT1 + nCl);                                   //����� ���������� ��������
      if(nCl > maxZapFAT1 && nCl != 0x0FFFFFFF)
        return Error1((Lan+13)->msg);                        //return Error2("����� �������� � ������� FAT", "��������� ���������� ��������.");
      if(SizeToEnd == 0 && nCl != 0x0FFFFFFF)
        return Error1((Lan+14)->msg);                        //return Error2("������ ����� ��������� ��������,", "� ������� ����� ������� FAT �� ������.");
   }
   return 0;
}

//------------------------------------------------------------------------------

static const DWORD Point1[] = { 0x00002E00, 0x20002E00 };

static int cCtrl_Dir(BYTE *buf)                              //�������� ����������� ��������
{
   One_Str_Cat *Kat = (One_Str_Cat *)buf;
   int prPoint = 0;                                          //������� ������� ��������� � ������ ��� ������� ������������ ���������
   for(int n=0; n<4*sClSec; n++)                             //� ������ ������� 4 ������
   {  if((Kat + n)->pf.type == 0) return 1;                  //����� ��������
      DWORD NamePoint = *((DWORD*)&(Kat + n)->Name);
      if(NamePoint == *(Point1 + pr_tRec))                   //��� ���� ����� �.�. ������� �������� ��������
      { if(n == 0) prPoint++; continue; }
      if(NamePoint == 0x2E002E00)                            //��� ��� ����� �.�. ������� ����������� ��������
      { if(n == 1) prPoint++;
        continue;
      }
      WORD NameDel = *((WORD*)&(Kat + n)->Name);
      if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0) continue;      //��� ��������� ���
      if((Kat + n)->pf.type == 32)                           //���� ��� ����, �� ��������� �� FAT
         cCtrl_FAT_For_File((Kat + n)->pf.ClSt, (Kat + n)->pf.SizeF); //�������� ������������ ����� ����� � FAT
      if((Kat + n)->pf.type == 48)                           //���� ��� �����, �� ��������� �����
         cRead_Media_Dir((Kat + n)->pf.ClSt);                //������ � �������� ������ ��������
   }
   return 0;
}

//------------------------------------------------------------------------------

static int cRead_Dir_Media(BYTE *buf, DWORD nCl)             //������ � �������� ������ ��������
{
   DWORD nSector = Start_SecDir1 + (nCl - 1) * sClSec;       //����� ������� �� ������ ��������
   if(ReadClast1_P(nSector, buf) < 0) return -1;             //������ ��������
   if(cCtrl_Dir(buf) < 0)                                    //�������� ����������� ��������
      Error2((Lan+97)->msg, (Lan+98)->msg);                  //Error2("������� ������ � ��������� ������� �� ����� LG.". "������ ����� � ��������� �������� �������� ����������.");
   if(nCl != 1 && *(FAT1 + nCl) != 0x0FFFFFFF)
   {  Error1((Lan+17)->msg);                                 //Error1("������ ������ �������� ��������� ������ ��������.");
      Error2((Lan+97)->msg, (Lan+98)->msg);                  //Error2("������� ������ � ��������� ������� �� ����� LG.". "������ ����� � ��������� �������� �������� ����������.");
   }
   return 0;
}

//------------------------------------------------------------------------------

static int cRead_Media_Dir(DWORD nCl)                        //������ � �������� ������ ��������
{
   BYTE *buf = (BYTE *)MyAllocMem(sCl_B);                    //������ ��� ����� ��������� � ��������
   if(buf == NULL)  return -1;
   int ret = cRead_Dir_Media(buf, nCl);                      //������ � �������� ������ ��������
   *(cTestFAT1 + nCl) = 0;                                   //�������� ������������� ������� �������� FAT
   sumClTestFAT1++;                                          //��������� ���������� ������� ���������
   MyFreeMem(&(void*)buf);
   return ret;
}

//------------------------------------------------------------------------------

int  Ctrl_FAT1(void)                                         //�������� ��������� ��������� �������
{
   cTestFAT1 = (DWORD *)MyAllocMem(Size_FAT1);               //������ ��� FAT
   if(cTestFAT1 == NULL)  return -1;
   CopyMemory(cTestFAT1, FAT1, Size_FAT1);                   //����������� ���������� FAT1
   sumClTestFAT1 = 0;                                        //��������� ���������� ������� ���������
   int ret = cRead_Media_Dir(1);                             //������ � �������� ������ ��������
   if(ret < 0) return ret;
   if(stCl_Media2 != 0)                                      //������� ������ ������ ����� ����� MEDIA
     ret = cRead_Media_Dir(stCl_Media2);                     //������ � ������������ ������ ����� ����� MEDIA
   return ret;
}

//------------------------------------------------------------------------------
#if defined TEST_FAT1_VOST                                   //�������� � �������������� FAT1 �� ���������� ��������
int Vost_FAT1(void)                                          //������ FAT1 �� ���������� ���������
{
   for(DWORD i=2; i<maxZapFAT1; i++)                         //�� ���� ������� FAT
   {  if(*(cTestFAT1 + i) == 0) continue;                    //���������� ������ �������� � ������� �� ��������
//    if(*(FAT_ + i) == 1) continue;                         //�������, ��� ������ ������� ������������
      *(FAT1 + i) = 0;                                       //�������� ������� FAT1 ��� ���������
   }
#if !defined EMULATOR_HDD                                    //����� ���������
   DWORD nb;
   LONGLONG Poz = LONGLONG(sSecB) * Start_SecFAT1;
   if(SetInFilePointer(Poz) < 0) return -1;                  //��������� ������� ��������� � �����
   if(WriteFile(hDrive, FAT1, Size_FAT1, &nb, NULL) == FALSE || nb != Size_FAT1)
      return ErrorSys1((Lan+176)->msg);                      //"������ ��� ������ FAT."
#endif
   return 0;
}
#endif

#endif

#endif


