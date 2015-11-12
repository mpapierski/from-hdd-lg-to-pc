
//                               24.02.2008
#ifndef __HDD_LG_TEST_67_68_70_71__
#define __HDD_LG_TEST_67_68_70_71__

#include "from_hdd_lg_to_pc.h"

#if defined TEST_67_68_70_71                                 //�������

//============================ hdd_lg_sect_67_68_70_71 =========================
int  Read_Sec_67_68_70_71(void);                             //������ ����������� ��������

//------------------------------------------------------------------------------

static int View_Sec_0(SEC_0 *Sec0)                                  //����� ����������� �������
{
   char Ss[300];
   Add_Spis("������ 0");
   sprintf(Ss, "����� ��������� � ������� 1      - %10d (%08X)", Sec0->numClP1, Sec0->numClP1);
   Add_Spis(Ss);
   sprintf(Ss, "����� ����� �������� � ������� 1 - %10d (%08X) � ��������� (1024 �������): %.2f", Sec0->numSecP1, Sec0->numSecP1, Sec0->numSecP1 / 1024.0);
   Add_Spis(Ss);
   sprintf(Ss, "����� ��������� � ������� 2      - %10d (%08X)", Sec0->numClP2, Sec0->numClP2);
   Add_Spis(Ss);
   sprintf(Ss, "����� ����� �������� � ������� 2 - %10d (%08X) � ��������� (8 ��������): %.2f", Sec0->numSecP2, Sec0->numSecP2, Sec0->numSecP2 / 8.0);
   Add_Spis(Ss);
   sprintf(Ss, "����� ����� �������� � ������� 2 - %10d (%08X) � ��������� (1024 �������): %.2f", Sec0->numSecP2, Sec0->numSecP2, Sec0->numSecP2 / 1024.0);
   Add_Spis(Ss);
// View_Sec_Hex((BYTE *)Sec, 1);                         //����� ����������� �������
   return 0;
}

//------------------------------------------------------------------------------

int View_Sec_67_68(char *msg, SEC_67 *Sec)                      //����� ����������� �������
{
   char Ss[300];
   if(Sec->Ident1 != 0x41615252 || Sec->Ident2 != 0x61417272 || Sec->Ident3 != 0xAA550000)
     return Error1("��������� ��������� ���������� �������.");
   for(int i=0; i<120; i++)
     if(Sec->A[i] != 0)
       return Error1("��������� ��������� ���������� �������.");
   for(int i=1; i<3; i++)
     if(Sec->B[i] != 0)
       return Error1("��������� ��������� ���������� �������.");
   sprintf(Ss, "*** %s:  �������� ��������� - %8d,  ����� �������� ������ ���������������� ������� - %8d", msg, Sec->FreeCl, Sec->LastCl);
   Add_Spis(Ss);
// View_Sec_Hex((BYTE *)Sec, 1);                         //����� ����������� �������
   return 0;
}

//------------------------------------------------------------------------------

int View_Sec_70_71(char *msg, SEC_67 *Sec)                   //����� ����������� �������
{
   char Ss[300];
   if(Sec->Ident1 != 0x41615252 || Sec->Ident2 != 0x61417272 || Sec->Ident3 != 0xAA550000)
     return Error1("��������� ��������� ���������� �������.");
   for(int i=0; i<120; i++)
     if(Sec->A[i] != 0)
       return Error1("��������� ��������� ���������� �������.");
   for(int i=0; i<3; i++)
     if(Sec->B[i] != 0)
       return Error1("��������� ��������� ���������� �������.");
   sprintf(Ss, "*** %s:  ����� ��������� -    %8d,  ������ ������� -    %8d", msg, Sec->FreeCl, Sec->LastCl);
   Add_Spis(Ss);
// View_Sec_Hex((BYTE *)Sec, 1);                             //����� ����������� �������
   return 0;
}

//------------------------------------------------------------------------------

int View_Sec_63(char *msg, SEC_63 *Sec63)                    //����� ����������� �������
{
   char Ss[300];
   if(strncmp(Sec63->Met, "LGEINC  ", 8) != 0 ||
      strncmp(Sec63->Name, "VOLUMELABE FAT32   ", 19) != 0 ||
      Sec63->nf != 1 || Sec63->F8 != 0xF8)
     return Error1("��������� ��������� ���������� �������.");
   Add_Spis(msg);
   sprintf(Ss, "   ����� ����� �������� � �������:      %11d", Sec63->numSecPart);
   Add_Spis(Ss);
   sprintf(Ss, "   ����� �������� ��� FAT:              %11d", Sec63->numSecFAT);
   Add_Spis(Ss);
   sprintf(Ss, "   �������� � ������� �������� (�� FAT):%11d", Sec63->nSecDir);
   Add_Spis(Ss);
   sprintf(Ss, "   ����� �������� � ����� ��������:     %11d", Sec63->SecPerCl);
   Add_Spis(Ss);

   DWORD Start_SecFAT = 63 + Sec63->numRezSec;               //������� ������ ������� FAT �������
   DWORD Start_SecDir = Start_SecFAT + Sec63->nSecDir;       //������� ������ ������� ��������
   DWORD max_ZapFAT = sSecB / 4 * Sec63->numSecFAT;          //������������ ����� ������� � FAT (�� 4 �����)
//??   DWORD max_ZapFAT1 = sSecB / 4 * (Sec63->numSecFAT-400);   //������������ ����� ������� � FAT (�� 4 �����)
   sprintf(Ss, "      ������� ������ ������� FAT ������� (sec):      %8d", Start_SecFAT);
   Add_Spis(Ss);
   sprintf(Ss, "      ������� ������ ������� �������� (sec):         %8d", Start_SecDir);
   Add_Spis(Ss);
   sprintf(Ss, "      ������������ ����� ������� � FAT (�� 4 �����): %8d", max_ZapFAT);
   Add_Spis(Ss);
// View_Sec_Hex((BYTE *)Sec, 1);                             //����� ����������� �������
   return 0;
}

//------------------------------------------------------------------------------

static int  ViewParFAT1(void)                                //����������
{
   char Ss[300];
   int n;
   Add_Spis("��� ���������� �� FAT1");
   for(n=maxZapFAT1-1; n>1; n--)
     if(*(FAT1+n) != 0) break;
   int FreeFat = maxZapFAT1-1;
   int ZanFat = 2;
   for(int i=2; i<n; i++)
     if(*(FAT1+i) != 0)
     {  FreeFat--; ZanFat++; }
   sprintf(Ss, "     ����� - %8d, ������ - %8d, �������� - %8d, (����� ����� - %8d), ������ �������������� - %8d",
           maxZapFAT1, ZanFat, FreeFat, ZanFat+FreeFat, n+1);
   Add_Spis(Ss);
   return 0;
}

//------------------------------------------------------------------------------

static int  ViewParFAT2(void)                                //����������
{
   char Ss[300];
   int n;
   Add_Spis("��� ���������� �� FAT2");
   for(n=maxZapFAT2-1; n>1; n--)
     if(*(FAT2+n) != 0) break;
   int FreeFat = maxZapFAT2-1;
   int ZanFat = 2;
   for(int i=2; i<n; i++)
     if(*(FAT2+i) != 0)
     {  FreeFat--; ZanFat++; }
   sprintf(Ss, "     ����� - %8d, ������ - %8d, �������� - %8d, (����� ����� - %8d), ������ �������������� - %8d",
           maxZapFAT2, ZanFat, FreeFat, ZanFat+FreeFat, n+1);
   Add_Spis(Ss);
   return 0;
}

//------------------------------------------------------------------------------

int Read_Sec_67_68_70_71(void)                               //������ ����������� ��������
{
   SEC_67 anySec;
   SEC_0 Sec0;
   if(Read_Sec_Emul(0, (BYTE*)&Sec0) < 0) return -1;         //������ ������� �� ������ ����������
   View_Sec_0(&Sec0);                                        //����� ����������� �������

   if(Read_Sec_Emul(63, (BYTE*)&anySec) < 0) return -1;      //������ ������� �� ������ ����������
   View_Sec_63("������ 63", (SEC_63*)&anySec);               //����� ����������� �������
   if(Read_Sec_Emul(67, (BYTE*)&anySec) < 0) return -1;      //������ ������� �� ������ ����������
   View_Sec_67_68("������ 67", &anySec);                     //����� ����������� �������
   if(Read_Sec_Emul(70, (BYTE*)&anySec) < 0) return -1;      //������ ������� �� ������ ����������
   View_Sec_70_71("������ 70", &anySec);                     //����� ����������� �������

   if(Read_Sec_Emul(64, (BYTE*)&anySec) < 0) return -1;      //������ ������� �� ������ ����������
   View_Sec_63("������ 64", (SEC_63*)&anySec);               //����� ����������� �������
   if(Read_Sec_Emul(68, (BYTE*)&anySec) < 0) return -1;      //������ ������� �� ������ ����������
   View_Sec_67_68("������ 68", &anySec);                     //����� ����������� �������
   if(Read_Sec_Emul(71, (BYTE*)&anySec) < 0) return -1;      //������ ������� �� ������ ����������
   View_Sec_70_71("������ 71", &anySec);                     //����� ����������� �������

   ViewParFAT1();                                            //����������
   ViewParFAT2();                                            //����������
   return 0;
}

#endif

#endif

