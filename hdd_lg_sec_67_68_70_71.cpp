
//                               24.02.2008
#ifndef __HDD_LG_TEST_67_68_70_71__
#define __HDD_LG_TEST_67_68_70_71__

#include "from_hdd_lg_to_pc.h"

#if defined TEST_67_68_70_71                                 //Сектора

//============================ hdd_lg_sect_67_68_70_71 =========================
int  Read_Sec_67_68_70_71(void);                             //Чтение специальных секторов

//------------------------------------------------------------------------------

static int View_Sec_0(SEC_0 *Sec0)                                  //Вывод содержимого сектора
{
   char Ss[300];
   Add_Spis("Сектор 0");
   sprintf(Ss, "Число кластеров в разделе 1      - %10d (%08X)", Sec0->numClP1, Sec0->numClP1);
   Add_Spis(Ss);
   sprintf(Ss, "Общее число секторов в разделе 1 - %10d (%08X) В кластерах (1024 сектора): %.2f", Sec0->numSecP1, Sec0->numSecP1, Sec0->numSecP1 / 1024.0);
   Add_Spis(Ss);
   sprintf(Ss, "Число кластеров в разделе 2      - %10d (%08X)", Sec0->numClP2, Sec0->numClP2);
   Add_Spis(Ss);
   sprintf(Ss, "Общее число секторов в разделе 2 - %10d (%08X) В кластерах (8 секторов): %.2f", Sec0->numSecP2, Sec0->numSecP2, Sec0->numSecP2 / 8.0);
   Add_Spis(Ss);
   sprintf(Ss, "Общее число секторов в разделе 2 - %10d (%08X) В кластерах (1024 сектора): %.2f", Sec0->numSecP2, Sec0->numSecP2, Sec0->numSecP2 / 1024.0);
   Add_Spis(Ss);
// View_Sec_Hex((BYTE *)Sec, 1);                         //Вывод содержимого сектора
   return 0;
}

//------------------------------------------------------------------------------

int View_Sec_67_68(char *msg, SEC_67 *Sec)                      //Вывод содержимого сектора
{
   char Ss[300];
   if(Sec->Ident1 != 0x41615252 || Sec->Ident2 != 0x61417272 || Sec->Ident3 != 0xAA550000)
     return Error1("Нарушение служебной информации сектора.");
   for(int i=0; i<120; i++)
     if(Sec->A[i] != 0)
       return Error1("Нарушение служебной информации сектора.");
   for(int i=1; i<3; i++)
     if(Sec->B[i] != 0)
       return Error1("Нарушение служебной информации сектора.");
   sprintf(Ss, "*** %s:  Свободно кластеров - %8d,  Номер кластера начала неиспользованной области - %8d", msg, Sec->FreeCl, Sec->LastCl);
   Add_Spis(Ss);
// View_Sec_Hex((BYTE *)Sec, 1);                         //Вывод содержимого сектора
   return 0;
}

//------------------------------------------------------------------------------

int View_Sec_70_71(char *msg, SEC_67 *Sec)                   //Вывод содержимого сектора
{
   char Ss[300];
   if(Sec->Ident1 != 0x41615252 || Sec->Ident2 != 0x61417272 || Sec->Ident3 != 0xAA550000)
     return Error1("Нарушение служебной информации сектора.");
   for(int i=0; i<120; i++)
     if(Sec->A[i] != 0)
       return Error1("Нарушение служебной информации сектора.");
   for(int i=0; i<3; i++)
     if(Sec->B[i] != 0)
       return Error1("Нарушение служебной информации сектора.");
   sprintf(Ss, "*** %s:  Всего кластеров -    %8d,  Первый занятый -    %8d", msg, Sec->FreeCl, Sec->LastCl);
   Add_Spis(Ss);
// View_Sec_Hex((BYTE *)Sec, 1);                             //Вывод содержимого сектора
   return 0;
}

//------------------------------------------------------------------------------

int View_Sec_63(char *msg, SEC_63 *Sec63)                    //Вывод содержимого сектора
{
   char Ss[300];
   if(strncmp(Sec63->Met, "LGEINC  ", 8) != 0 ||
      strncmp(Sec63->Name, "VOLUMELABE FAT32   ", 19) != 0 ||
      Sec63->nf != 1 || Sec63->F8 != 0xF8)
     return Error1("Нарушение служебной информации сектора.");
   Add_Spis(msg);
   sprintf(Ss, "   Общее число секторов в разделе:      %11d", Sec63->numSecPart);
   Add_Spis(Ss);
   sprintf(Ss, "   Число секторов под FAT:              %11d", Sec63->numSecFAT);
   Add_Spis(Ss);
   sprintf(Ss, "   Смещение к первому каталогу (от FAT):%11d", Sec63->nSecDir);
   Add_Spis(Ss);
   sprintf(Ss, "   Число секторов в одном кластере:     %11d", Sec63->SecPerCl);
   Add_Spis(Ss);

   DWORD Start_SecFAT = 63 + Sec63->numRezSec;               //Позиция начала таблицы FAT раздела
   DWORD Start_SecDir = Start_SecFAT + Sec63->nSecDir;       //Позиция начала первого каталога
   DWORD max_ZapFAT = sSecB / 4 * Sec63->numSecFAT;          //Максимальное число записей в FAT (по 4 байта)
//??   DWORD max_ZapFAT1 = sSecB / 4 * (Sec63->numSecFAT-400);   //Максимальное число записей в FAT (по 4 байта)
   sprintf(Ss, "      Позиция начала таблицы FAT раздела (sec):      %8d", Start_SecFAT);
   Add_Spis(Ss);
   sprintf(Ss, "      Позиция начала первого каталога (sec):         %8d", Start_SecDir);
   Add_Spis(Ss);
   sprintf(Ss, "      Максимальное число записей в FAT (по 4 байта): %8d", max_ZapFAT);
   Add_Spis(Ss);
// View_Sec_Hex((BYTE *)Sec, 1);                             //Вывод содержимого сектора
   return 0;
}

//------------------------------------------------------------------------------

static int  ViewParFAT1(void)                                //Информация
{
   char Ss[300];
   int n;
   Add_Spis("Мои вычисления по FAT1");
   for(n=maxZapFAT1-1; n>1; n--)
     if(*(FAT1+n) != 0) break;
   int FreeFat = maxZapFAT1-1;
   int ZanFat = 2;
   for(int i=2; i<n; i++)
     if(*(FAT1+i) != 0)
     {  FreeFat--; ZanFat++; }
   sprintf(Ss, "     Всего - %8d, Занято - %8d, Свободно - %8d, (всего сумма - %8d), Первый неиспользуемый - %8d",
           maxZapFAT1, ZanFat, FreeFat, ZanFat+FreeFat, n+1);
   Add_Spis(Ss);
   return 0;
}

//------------------------------------------------------------------------------

static int  ViewParFAT2(void)                                //Информация
{
   char Ss[300];
   int n;
   Add_Spis("Мои вычисления по FAT2");
   for(n=maxZapFAT2-1; n>1; n--)
     if(*(FAT2+n) != 0) break;
   int FreeFat = maxZapFAT2-1;
   int ZanFat = 2;
   for(int i=2; i<n; i++)
     if(*(FAT2+i) != 0)
     {  FreeFat--; ZanFat++; }
   sprintf(Ss, "     Всего - %8d, Занято - %8d, Свободно - %8d, (всего сумма - %8d), Первый неиспользуемый - %8d",
           maxZapFAT2, ZanFat, FreeFat, ZanFat+FreeFat, n+1);
   Add_Spis(Ss);
   return 0;
}

//------------------------------------------------------------------------------

int Read_Sec_67_68_70_71(void)                               //Чтение специальных секторов
{
   SEC_67 anySec;
   SEC_0 Sec0;
   if(Read_Sec_Emul(0, (BYTE*)&Sec0) < 0) return -1;         //Чтение сектора по номеру эмулятором
   View_Sec_0(&Sec0);                                        //Вывод содержимого сектора

   if(Read_Sec_Emul(63, (BYTE*)&anySec) < 0) return -1;      //Чтение сектора по номеру эмулятором
   View_Sec_63("Сектор 63", (SEC_63*)&anySec);               //Вывод содержимого сектора
   if(Read_Sec_Emul(67, (BYTE*)&anySec) < 0) return -1;      //Чтение сектора по номеру эмулятором
   View_Sec_67_68("Сектор 67", &anySec);                     //Вывод содержимого сектора
   if(Read_Sec_Emul(70, (BYTE*)&anySec) < 0) return -1;      //Чтение сектора по номеру эмулятором
   View_Sec_70_71("Сектор 70", &anySec);                     //Вывод содержимого сектора

   if(Read_Sec_Emul(64, (BYTE*)&anySec) < 0) return -1;      //Чтение сектора по номеру эмулятором
   View_Sec_63("Сектор 64", (SEC_63*)&anySec);               //Вывод содержимого сектора
   if(Read_Sec_Emul(68, (BYTE*)&anySec) < 0) return -1;      //Чтение сектора по номеру эмулятором
   View_Sec_67_68("Сектор 68", &anySec);                     //Вывод содержимого сектора
   if(Read_Sec_Emul(71, (BYTE*)&anySec) < 0) return -1;      //Чтение сектора по номеру эмулятором
   View_Sec_70_71("Сектор 71", &anySec);                     //Вывод содержимого сектора

   ViewParFAT1();                                            //Информация
   ViewParFAT2();                                            //Информация
   return 0;
}

#endif

#endif

