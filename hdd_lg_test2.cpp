
//                               07.08.2008
#ifndef __HDD_LG_TEST2__
#define __HDD_LG_TEST2__

#include "from_hdd_lg_to_pc.h"

#if defined TEST_LG2_MME

//============================ hdd_lg_test2 ====================================
int  Read_Test2(void);                                       //Тестовое чтение и вывод

static char Ss[10 * 4096], *as;
static BYTE *at_MME;                                         //Адрес начала текущей записи
static BYTE *endMME;                                         //Адрес конца записей в массиве MME
static int numF;                                             //Порядковый номер файла (пары)
static DWORD LstNN;                                          //Позиция начала для NN
static int numT;                                             //Число глав

//------------------------------------------------------------------------------

inline void OutDW(DWORD *DW, DWORD N)
{
   for(DWORD j=0; j<N/sizeof(DWORD); j++)
   {  SWAP32(DW+j);
      as += sprintf(as, " %08X", *(DW+j));
   }
}

//------------------------------------------------------------------------------

inline void Out_B(BYTE *B, DWORD N)
{
   as += sprintf(as, " ");
   for(DWORD j=0; j<N; j++)
      as += sprintf(as, "%02X", *(B+j));
   as += sprintf(as, " ");
}

//------------------------------------------------------------------------------

static int CtrlSizeMME_(DWORD Delta)                         //Контроль смещения в массиве MME
{
   at_MME += Delta;                                          //Адрес начала следеющего элемента записи
   if(at_MME > endMME)                                       //Вышли за пределы массива MME
     return Error1("Hex. Структура файла MME.DB неизвестна автору программы.");
   return 0;
}

//-------------------------------------------------------------------------------

static void View_C2l_Hex(BYTE *buff, int pr)                 //Вывод содержимого кластера
{
   BYTE *t_buff = buff;
   for(int i=0; i<sClSec2; i++)                              //По числу секторов в кластере
   {  View_Sec_Hex(t_buff, pr);                              //Вывод содержимого сектора
      t_buff += sSecB;                                       //Увеличили адрес вывода на размер сектора
      Add_Spis("----------------------------------------------------------------------------");
   }
}

//------------------------------------------------------------------------------

static int WorkHead(int ind)                                 //Обработка заголовка
{
   numF = 0;                                                 //Порядковый номер файла (пары)
#if defined EMULATOR_HDD
   wsprintf(Ss, "Тип рекордера - %d, %s", ind, NameFDump);
#else
   wsprintf(Ss, "Тип рекордера - %d", ind);
#endif
   Add_Spis(Ss);                                             //Добавление строки в список
   as = Ss;
   at_MME = MMe;                                             //Адрес начала текущей записи
   PSP_MME *pspMME = (PSP_MME *)MMe;
   if(CtrlSizeMME_(sizeof(PSP_MME)) < 0) return -1;          //Контроль смещения в массиве MME
   as += sprintf(as, "Заголовок:");
   OutDW(pspMME->Dw1, sizeof(pspMME->Dw1));
   SWAP32(&pspMME->NumT);
   as += sprintf(as, " ЧислоTitle=%4d[%08X]", pspMME->NumT, pspMME->NumT);
   OutDW(&pspMME->Dw2, sizeof(pspMME->Dw2));
   SWAP64(&DWORDLONG(pspMME->size1));
   as += sprintf(as, " ВсегоКластеров(S70)=%9.2lf[%08X %08X]", double(pspMME->size1)/1024/512, *((DWORD*)&pspMME->size1 + 1), *(DWORD*)&pspMME->size1);
   SWAP64(&DWORDLONG(pspMME->size2));
   as += sprintf(as, " ПервыйнеЗанятый(S67)=%9.2lf[%08X %08X]", double(pspMME->size2)/1024/512, *((DWORD*)&pspMME->size2 + 1), *(DWORD*)&pspMME->size2);
   SWAP64(&DWORDLONG(pspMME->size3));
   as += sprintf(as, " ??3=%9.2lf[%08X %08X]", double(pspMME->size3)/1024/512, *((DWORD*)&pspMME->size3 + 1), *(DWORD*)&pspMME->size3);
   SWAP64(&DWORDLONG(pspMME->size4));
   as += sprintf(as, " ??4=%9.2lf[%08X %08X]", double(pspMME->size4)/1024/512, *((DWORD*)&pspMME->size4 + 1), *(DWORD*)&pspMME->size4);
   OutDW(pspMME->Dw3, sizeof(pspMME->Dw3));
   *as = 0;
   Add_Spis(Ss);                                             //Добавление строки в список
   Add_Spis("---------------------------------------------");//Добавление строки в список
   numT = pspMME->NumT;
   return 0;
}

//------------------------------------------------------------------------------

static void Out_Dat1(DATE_T1 *DaT1)
{
   SWAP16(&DaT1->year);
   SWAP16(&DaT1->mon);
   SWAP16(&DaT1->day);
   SWAP16(&DaT1->hour);
   as += sprintf(as, " Дата=%04d.%02d.%02d[%04X%02X%02X] Время=%02d:%02d:%02d[%02X%02X%02X]",
                          DaT1->year, DaT1->mon, DaT1->day, DaT1->year, DaT1->mon, DaT1->day,
                          DaT1->hour, DaT1->min, DaT1->sec, DaT1->hour, DaT1->min, DaT1->sec);
}

//------------------------------------------------------------------------------

static void Out_Dat2(DATE_T2 *DaT2)
{
   SWAP16(&DaT2->Year);
   as += sprintf(as, " Дата=%04d.%02d.%02d[%04X%02X%02X] Время=%02d:%02d:%02d[%02X%02X%02X] ",
                         DaT2->Year, DaT2->Mon, DaT2->Day, DaT2->Year, DaT2->Mon, DaT2->Day,
                         DaT2->Hour, DaT2->Min, DaT2->Sec, DaT2->Hour, DaT2->Min, DaT2->Sec);
}

//------------------------------------------------------------------------------

static void Out_Can(BYTE *prCan)
{
   as += sprintf(as, " КаналТВ_№=");
   if(prCan[1] == 0)
   {  if(prCan[0] > 99)
        as += sprintf(as, "?%03d[%02X%02X]", prCan[0], prCan[0], prCan[1]);
      else
        as += sprintf(as, "TV%02d[%02X%02X]", prCan[0], prCan[0], prCan[1]);
   }
   else as += sprintf(as, "????[%02X%02X]", prCan[0], prCan[1]);
}

//------------------------------------------------------------------------------

static void Out_nAV(BYTE nAV)
{
   as += sprintf(as, " Источник=");
   if(nAV == 119)     as += sprintf(as, "DVD[%02X]", nAV);
   else  if(nAV == 0) as += sprintf(as, "TV_[%02X]", nAV);
         else         as += sprintf(as, "AV%d[%02X]", nAV, nAV);
}

//------------------------------------------------------------------------------

static void Out_timeLong(DWORD *timeLong)
{
   SWAP32(timeLong);
   int H = *timeLong / 3600;
   int M1 = *timeLong - H * 3600;
   int M = M1 / 60;
   int S = M1 - M * 60;
   as += sprintf(as, " Длительность=%02d:%02d:%02d(%5dс)[%04X]", H, M, S, *timeLong, *timeLong);
}

//------------------------------------------------------------------------------

static void Out_Q(BYTE Q)
{
   as += sprintf(as, " Качество=");
   switch(Q)
   {  case 3: as += sprintf(as, "HP[%02X]", Q); break;
      case 2: as += sprintf(as, "SP[%02X]", Q); break;
      case 1: as += sprintf(as, "LP[%02X]", Q); break;
      case 0: as += sprintf(as, "EP[%02X]", Q); break;
      default: as += sprintf(as, "??[%02X]", Q);
   }
}

//------------------------------------------------------------------------------

static void Out_Janr(BYTE Janr)
{
   as += sprintf(as, " Жанр=");
   switch(Janr)
   {  case 0xA: as += sprintf(as, "DOCU_[%02X]", Janr); break;
      case 0x9: as += sprintf(as, "EQU._[%02X]", Janr); break;
      case 0x8: as += sprintf(as, "NEWS_[%02X]", Janr); break;
      case 0x7: as += sprintf(as, "DRAMA[%02X]", Janr); break;
      case 0x6: as += sprintf(as, "ANI._[%02X]", Janr); break;
      case 0x5: as += sprintf(as, "SPORT[%02X]", Janr); break;
      case 0x4: as += sprintf(as, "MOVIE[%02X]", Janr); break;
      case 0x3: as += sprintf(as, "SHOW_[%02X]", Janr); break;
      case 0x2: as += sprintf(as, "AV_IN[%02X]", Janr); break;
      case 0x1: as += sprintf(as, "Any__[%02X]", Janr); break;
      case 0x0: as += sprintf(as, "-----[%02X]", Janr); break;
      default:  as += sprintf(as, "?????[%02X]", Janr);
   }
}

//------------------------------------------------------------------------------

static void Out_nPart(WORD *nPart)
{
   SWAP16(nPart);
   as += sprintf(as, " ЧислоЧастей=%3d[%04X]", *nPart, *nPart);
}

//------------------------------------------------------------------------------

static void Out_nameTV(char *nameTV)
{
   as += sprintf(as, " ИмяКанала=%6s[%02X%02X%02X%02X%02X%02X]",
                 nameTV, BYTE(nameTV[0]), BYTE(nameTV[1]), BYTE(nameTV[2]),
                         BYTE(nameTV[3]), BYTE(nameTV[4]), BYTE(nameTV[5]));
}

//------------------------------------------------------------------------------

static void Out_Title(WCHAR *wTitle)
{
   char Title[128];
   UnicodeToAnsi(wTitle, Title, 32);                         //Преобразовали Название
   as += sprintf(as, " Название=%s", Title);
   int l = lstrlen(Title);
   for(int j=l; j<33; j++, (as)++)  *as = '_';
}

//------------------------------------------------------------------------------

static void Out_Number(DWORD Number)
{
   SWAP32(&Number);
   as += sprintf(as, " ВнутреннийНомер=%3d[%08X]", Number, Number);
}

//------------------------------------------------------------------------------

static void Out_NN(WORD *NN)
{
   DWORD lstNN = LstNN - (DWORD(as) - DWORD(Ss));            //Позиция начала для NN
   if(lstNN > 0)
     for(DWORD i=0; i<lstNN; i++, (as)++) *as = '_';
   SWAP16(NN);
   as += sprintf(as, " || NN=%2d[%04X] ", *NN, *NN);
}

//------------------------------------------------------------------------------

static void Out_sizeF(LONGLONG sizeF)
{
   SWAP64(&DWORDLONG(sizeF));
// as += sprintf(as, " РазмерФайла=%11.0lf[%08X %08X]", double(sizeF), *((DWORD*)&sizeF + 1), *(DWORD*)&sizeF);
   char dd[48], *as1;
   sprintf(dd, "%16.0f", double(sizeF));
   as1 = Char_Dig_p(dd, 14);                                  //Преобразование символьного числа в разрядку
   as += sprintf(as, " РазмерФайла=%s[%08X %08X]", as1, *((DWORD*)&sizeF + 1), *(DWORD*)&sizeF);
}

//------------------------------------------------------------------------------

static int WorkName(int nPart)                               //Обработка записей имен
{
   char Name1[128], Name2[128];
   DWORD L = DWORD(as) - DWORD(Ss);                          //Размер потоянной части
   for(int n=0; n<nPart; n++)                                //По числу частей в одном Title
   {  ONE_NAME *aNam = (ONE_NAME *)at_MME;
      if(CtrlSizeMME_(sizeof(ONE_NAME)) < 0) return -1;      //Контроль смещения в массиве MME
      as += sprintf(as, " ||");
      UnicodeToAnsi(aNam->Name1, Name1, 30);                 //Преобразовали расширение
      as += sprintf(as, " %s", Name1);
      if(lstrlen(Name1) < 29) as += sprintf(as, " х");
      Out_B(aNam->b1, sizeof(aNam->b1));
      UnicodeToAnsi(aNam->Name2, Name2, 30);                 //Преобразовали расширение
      as += sprintf(as, " %s", Name2);
      if(lstrlen(Name2) < 29) as += sprintf(as, " х");
      OutDW(aNam->Dw1, sizeof(aNam->Dw1));
      Out_sizeF(aNam->sizeF);
      OutDW(aNam->Dw2, sizeof(aNam->Dw2));
      Out_timeLong(&aNam->timeLong);
      SWAP32(&(aNam->tLong2));
      as += sprintf(as, " tLong2=%6d[%08X]", aNam->tLong2, aNam->tLong2);
      Out_B(aNam->b2, sizeof(aNam->b2));
      Out_Can(aNam->prCan0);
      as += sprintf(as, " %02X", aNam->bb);
      Out_nAV(aNam->nAV);
      Out_B(aNam->b3, sizeof(aNam->b3));
      OutDW(aNam->Dw3, sizeof(aNam->Dw3));
//-----
      SWAP32(&aNam->AAA0);
      as += sprintf(as, " AAA0=%10u[%08X]", aNam->AAA0, aNam->AAA0);
      OutDW(&aNam->Dw4, sizeof(aNam->Dw4));
      SWAP32(&aNam->AAA1);
      as += sprintf(as, " AAA1=%10u[%08X]", aNam->AAA1, aNam->AAA1);
      as += sprintf(as, " {(AAA1-AAA0)/tLong2=%7.1lf}", double(aNam->AAA1-aNam->AAA0)/aNam->tLong2);
      OutDW(aNam->Dw5, sizeof(aNam->Dw5));
//-----
      if(nPart - n == 1) continue;                           //Для последней части не завершаем строку
      *as = 0;
      Add_Spis(Ss);                                          //Добавление строки в список
      if(nPart > 1)                                          //Более одного фрагмента
      {  numF += 2;                                          //Порядковый номер файла (пары)
         sprintf(Ss, "        №файла=%4d", numF);            //Порядковый номер файла
         for(DWORD j=19; j<L; j++)  *(Ss+j) = '.';           //Очистка места постоянной части
      }
      as = Ss + L;                                           //Сместились на размер постоянной части
   }
   return 0;
}

//------------------------------------------------------------------------------

static int WorkMet(void)                                     //Обработка записей меток
{
   END_REC *eRec = (END_REC *)at_MME;
   if(CtrlSizeMME_(sizeof(END_REC)) < 0) return -1;          //Контроль смещения в массиве MME
   SWAP16(&eRec->numMet);
   SWAP16(&eRec->A);
   SWAP32(&eRec->B);
   as += sprintf(as, " || ЧислоМеток=%2d[%04X] %04X %08X", eRec->numMet, eRec->numMet, eRec->A, eRec->B);
   LstNN = DWORD(as) - DWORD(Ss);                            //Позиция начала для NN
   if(eRec->numMet == 0) return 0;                           //Нет меток
   *as = 0;                                                  //Завершили строку листинга
   Add_Spis(Ss);                                             //Добавление строки в список
   for(int i=0; i<eRec->numMet; i++)                         //По числу меток
   {  char Name[128];
      as = Ss;
      as += sprintf(as, "==================================================");
      ONE_MET *oM = (ONE_MET *)at_MME;
      if(CtrlSizeMME_(sizeof(ONE_MET)) < 0) return -1;       //Контроль смещения в массиве MME
      OutDW(oM->Dw1, sizeof(oM->Dw1));
      UnicodeToAnsi(oM->Name, Name, 30);                     //Преобразовали имя файла
      lstrcpy(as, Name);
      as += lstrlen(Name);
      OutDW(oM->Dw2, sizeof(oM->Dw2));
      if(eRec->numMet - i == 1) continue;                    //Для последней части не завершаем строку
      Add_Spis(Ss);
   }
   return 0;
}

//------------------------------------------------------------------------------

static int WorkEndZap(void)                                  //Обработка хвостовых записей
{
   END0_ZAP_MME *aEnd0 = (END0_ZAP_MME *)at_MME;             //Адрес начала заголовка очередной записи
   if(CtrlSizeMME_(sizeof(END0_ZAP_MME)) < 0) return -1;     //Контроль смещения в массиве MME
   Out_NN(&aEnd0->NN);
   Out_B(aEnd0->b1, sizeof(aEnd0->b1));
   DWORD L = DWORD(as) - DWORD(Ss);                          //Размер потоянной части
   for(int i=0; i<aEnd0->NN; i++)
   {  as += sprintf(as, " ||");
      END1_ZAP_MME *aEnd = (END1_ZAP_MME *)at_MME;           //Адрес начала заголовка очередной записи
      if(CtrlSizeMME_(sizeof(END1_ZAP_MME)) < 0) return -1;  //Контроль смещения в массиве MME
      OutDW(aEnd->Dw1, sizeof(aEnd->Dw1));
      char Name[128];
      UnicodeToAnsi(aEnd->Name3, Name, 30);                  //Преобразовали расширение
      as += sprintf(as, " %s", Name);
      OutDW(aEnd->Dw2, sizeof(aEnd->Dw2));
      if(aEnd0->NN - i == 1) continue;                       //Для последней части не завершаем строку
      *as = 0;
      Add_Spis(Ss);                                          //Добавление строки в список
      if(aEnd0->NN > 1)                                      //Более одного фрагмента
          for(DWORD j=0; j<L; j++)  *(Ss+j) = '_';           //Очистка места постоянной части
      as = Ss + L;                                           //Сместились на размер постоянной части
   }
   return 0;
}

//------------------------------------------------------------------------------

static int ViewMME0_Hex(void)                                //Вывод содержимого файла MME
{
   if(WorkHead(0) < 0) return -1;                            //Обработка заголовка
   for(int t=0; t<numT; t++)                                 //По всему файлу MME
   {  as = Ss;
      numF += 2;                                             //Порядковый номер файла (пары)
      as += sprintf(as, "№пп=%3d №файла=%4d", t+1, numF);    //Порядковый номер записи и файла
      PSP_TIT0 *aTit = (PSP_TIT0 *)at_MME;                   //Адрес начала заголовка очередной записи
      if(CtrlSizeMME_(sizeof(PSP_TIT0)) < 0) return -1;      //Контроль смещения в массиве MME
      OutDW(aTit->Dw1, sizeof(aTit->Dw1));
//    Out_Number(aTit->Number);
      as += sprintf(as, " хххххххх хххххххх ххххххххххххххххххххххх");
      Out_Dat1(&aTit->DaT1);
      Out_Can(aTit->prCan1);
//    SWAP32(&aTit->B);
//    as += sprintf(as, " %08X", aTit->B);
      as += sprintf(as, " хххххххх");
      Out_nAV(aTit->nAV);
      Out_Can(aTit->prCan2);
      Out_nameTV(aTit->nameTV);
      Out_B(aTit->b1, sizeof(aTit->b1));
      Out_Q(aTit->Q);
      Out_Dat2(&aTit->DaT2);
      as += sprintf(as, " %02X", aTit->C2);
      Out_timeLong(&aTit->timeLong);
      Out_sizeF(aTit->sizeF);
      Out_Janr(aTit->Janr);
      as += sprintf(as, " %02X", aTit->b22);
      Out_Title(aTit->Title);
      OutDW(aTit->Dw2, sizeof(aTit->Dw2));
      Out_nPart(&aTit->nPart);
      SWAP16(&(aTit->E));
      as += sprintf(as, " %04X", aTit->E);
      OutDW(aTit->Dw3, sizeof(aTit->Dw3));
      if(WorkName(aTit->nPart)< 0) return -1;                //Обработка записей имен
//    Work_Met(&as);                                         //Обработка записей меток
      *as = 0;                                               //Завершили строку листинга
      Add_Spis(Ss);                                          //Добавление строки в список
   }
   return 0;
}

//------------------------------------------------------------------------------

static int ViewMME1_Hex(void)                                //Вывод содержимого файла MME
{
   if(WorkHead(1) < 0) return -1;                            //Обработка заголовка
   for(int t=0; t<numT; t++)                                 //По всему файлу MME
   {  as = Ss;
      numF += 2;                                             //Порядковый номер файла (пары)
      as += sprintf(as, "№пп=%3d №файла=%4d", t+1, numF);    //Порядковый номер записи и файла
      PSP_TIT1 *aTit = (PSP_TIT1 *)at_MME;                   //Адрес начала заголовка очередной записи
      if(CtrlSizeMME_(sizeof(PSP_TIT1)) < 0) return -1;      //Контроль смещения в массиве MME
      OutDW(aTit->Dw1, sizeof(aTit->Dw1));
      as += sprintf(as, " хххххххх хххххххх");
      Out_Number(aTit->Number);
      Out_Dat1(&aTit->DaT1);
      Out_Can(aTit->prCan1);
      SWAP32(&aTit->B);
      as += sprintf(as, " %08X", aTit->B);
      Out_nAV(aTit->nAV);
      Out_Can(aTit->prCan2);
      Out_nameTV(aTit->nameTV);
      Out_B(aTit->b1, sizeof(aTit->b1));
      Out_Q(aTit->Q);
      Out_Dat2(&aTit->DaT2);
      as += sprintf(as, " %02X", aTit->C2);
      Out_timeLong(&aTit->timeLong);
      Out_sizeF(aTit->sizeF);
      Out_Janr(aTit->Janr);
      as += sprintf(as, " %02X", aTit->b22);
      Out_Title(aTit->Title);
      OutDW(aTit->Dw2, sizeof(aTit->Dw2));
      Out_nPart(&aTit->nPart);
      SWAP16(&(aTit->E));
      as += sprintf(as, " %04X", aTit->E);
      OutDW(aTit->Dw3, sizeof(aTit->Dw3));
      if(WorkName(aTit->nPart)< 0) return -1;                //Обработка записей имен
      if(WorkMet() < 0) return -1;                           //Обработка записей меток
      *as = 0;                                               //Завершили строку листинга
      Add_Spis(Ss);                                          //Добавление строки в список
   }
   return 0;
}

//-------------------------------------------------------------------------------

static int ViewMME2_Hex(void)                                //Вывод содержимого файла MME
{
   if(WorkHead(2) < 0) return -1;                            //Обработка заголовка
   for(int t=0; t<numT; t++)                                 //По всему файлу MME
   {  as = Ss;
      numF += 2;                                             //Порядковый номер файла (пары)
      as += sprintf(as, "№пп=%3d №файла=%4d", t+1, numF);    //Порядковый номер записи и файла
      PSP_TIT2 *aTit = (PSP_TIT2 *)at_MME;                   //Адрес начала заголовка очередной записи
      if(CtrlSizeMME_(sizeof(PSP_TIT2)) < 0) return -1;      //Контроль смещения в массиве MME
      OutDW(aTit->Dw1, sizeof(aTit->Dw1));
      Out_Number(aTit->Number);
      Out_Dat1(&aTit->DaT1);
      Out_Can(aTit->prCan1);
      SWAP32(&aTit->B);
      as += sprintf(as, " %08X", aTit->B);
      Out_nAV(aTit->nAV);
      Out_Can(aTit->prCan2);
      Out_nameTV(aTit->nameTV);
      Out_B(aTit->b1, sizeof(aTit->b1));
      Out_Q(aTit->Q);
      Out_Dat2(&aTit->DaT2);
      as += sprintf(as, " %02X", aTit->C2);
      Out_timeLong(&aTit->timeLong);
      Out_sizeF(aTit->sizeF);
      Out_Janr(aTit->Janr);
      as += sprintf(as, " %02X", aTit->b22);
      Out_Title(aTit->Title);
      OutDW(aTit->Dw2, sizeof(aTit->Dw2));
      Out_nPart(&aTit->nPart);
      SWAP16(&(aTit->E));
      as += sprintf(as, " %04X", aTit->E);
      OutDW(aTit->Dw3, sizeof(aTit->Dw3));
      if(WorkName(aTit->nPart)< 0) return -1;                //Обработка записей имен
      if(WorkMet() < 0) return -1;                           //Обработка записей меток
      if(WorkEndZap() < 0) return -1;                        //Обработка хвостовых записей
      *as = 0;                                               //Завершили строку листинга
      Add_Spis(Ss);                                          //Добавление строки в список
   }
   return 0;
}

//-------------------------------------------------------------------------------

static int ViewMME4_Hex(void)                                //Вывод содержимого файла MME
{
   if(WorkHead(4) < 0) return -1;                            //Обработка заголовка
   for(int t=0; t<numT; t++)                                 //По всему файлу MME
   {  as = Ss;
      numF += 2;                                             //Порядковый номер файла (пары)
      as += sprintf(as, "№пп=%3d №файла=%4d", t+1, numF);    //Порядковый номер записи и файла
      PSP_TIT4 *aTit = (PSP_TIT4 *)at_MME;                   //Адрес начала заголовка очередной записи
      if(CtrlSizeMME_(sizeof(PSP_TIT4)) < 0) return -1;      //Контроль смещения в массиве MME
      OutDW(aTit->Dw1, sizeof(aTit->Dw1));
      Out_Number(aTit->Number);
      Out_Dat1(&aTit->DaT1);
      Out_Can(aTit->prCan1);
      SWAP32(&aTit->B);
      as += sprintf(as, " %08X", aTit->B);
      Out_nAV(aTit->nAV);
      Out_Can(aTit->prCan2);
      Out_nameTV(aTit->nameTV);
      Out_B(aTit->b1, sizeof(aTit->b1));
      Out_Q(aTit->Q);
      Out_Dat2(&aTit->DaT2);
      as += sprintf(as, " %02X", aTit->C2);
      Out_timeLong(&aTit->timeLong);
      Out_sizeF(aTit->sizeF);
      Out_Janr(aTit->Janr);
      as += sprintf(as, " %02X", aTit->b22);
      Out_Title(aTit->Title);
      OutDW(aTit->Dw2, sizeof(aTit->Dw2));
      Out_nPart(&aTit->nPart);
      SWAP16(&(aTit->E));
      as += sprintf(as, " %04X", aTit->E);
      OutDW(aTit->Dw3, sizeof(aTit->Dw3));
      if(WorkName(aTit->nPart)< 0) return -1;                //Обработка записей имен
      if(WorkMet() < 0) return -1;                           //Обработка записей меток
      if(WorkEndZap() < 0) return -1;                        //Обработка хвостовых записей
      *as = 0;                                               //Завершили строку листинга
      Add_Spis(Ss);                                          //Добавление строки в список
   }
   return 0;
}

//-------------------------------------------------------------------------------

static int ViewMME5_Hex(void)                                //Вывод содержимого файла MME
{
   if(WorkHead(5) < 0) return -1;                            //Обработка заголовка
   for(int t=0; t<numT; t++)                                 //По всему файлу MME
   {  as = Ss;
      numF += 2;                                             //Порядковый номер файла (пары)
      as += sprintf(as, "№пп=%3d №файла=%4d", t+1, numF);    //Порядковый номер записи и файла
      PSP_TIT5 *aTit = (PSP_TIT5 *)at_MME;                   //Адрес начала заголовка очередной записи
      if(CtrlSizeMME_(sizeof(PSP_TIT5)) < 0) return -1;      //Контроль смещения в массиве MME
      OutDW(aTit->Dw1, sizeof(aTit->Dw1));
      Out_Number(aTit->Number);
      Out_Dat1(&aTit->DaT1);
      Out_Can(aTit->prCan1);
      SWAP32(&aTit->B);
      as += sprintf(as, " %08X", aTit->B);
      Out_nAV(aTit->nAV);
      Out_Can(aTit->prCan2);
      Out_nameTV(aTit->nameTV);
      Out_B(aTit->b1, sizeof(aTit->b1));
      Out_Q(aTit->Q);
      Out_Dat2(&aTit->DaT2);
      as += sprintf(as, " %02X", aTit->C2);
      Out_timeLong(&aTit->timeLong);
      Out_sizeF(aTit->sizeF);
      Out_Janr(aTit->Janr);
      as += sprintf(as, " %02X", aTit->b22);
      Out_Title(aTit->Title);
      OutDW(aTit->Dw2, sizeof(aTit->Dw2));
      Out_nPart(&aTit->nPart);
      SWAP16(&(aTit->E));
      as += sprintf(as, " %04X", aTit->E);
      OutDW(aTit->Dw3, sizeof(aTit->Dw3));
      if(WorkName(aTit->nPart)< 0) return -1;                //Обработка записей имен
      if(WorkMet() < 0) return -1;                           //Обработка записей меток
      if(WorkEndZap() < 0) return -1;                        //Обработка хвостовых записей
      *as = 0;                                               //Завершили строку листинга
      Add_Spis(Ss);                                          //Добавление строки в список
   }
   return 0;
}

//------------------------------------------------------------------------------

#if defined TEST_LG2_ALL
static int View_Dir_Part2(void)                              //Визуализация каталога второго раздела
{
   BYTE buff[sCl2_B];

   sprintf(Ss, " ********** Начальный сектор корневой папки второго раздела - %d **********", Start_SecDir2);
   Add_Spis(Ss);                                             //Добавление строки в список
   if(ReadClast2_P(Start_SecDir2, buff) < 0) return -1;      //Чтение кластера
   One_Str_Cat *Kat = (One_Str_Cat *)buff;
   for(int n=0; n<4*sClSec2; n++)                            //В каждом секторе 4 записи
   {  if((Kat + n)->pf.type == 0) break;                     //Конец каталога
      sprintf(Ss, "....Строка каталога %d..............................................", n+1);
      Add_Spis(Ss);
/*
      BYTE *bb = (BYTE*)(Kat + n);
      for(int i=0; i<128; i+=32)                             //Вывод шеснадцатиричного дампа одной строки каталога
      {   &as = Ss;
          for(int j=i; j<i+32; j++)
          {   &as += sprintf(" %02X", bb[j]);
              if((j+1) % 8 == 0)
                 as += sprintf(as, "   ");
          }
          *(&as + 1) = 0;
          Add_Spis(Ss);                                      //Добавление строки в список
      }
*/
      WORD NameDel = *((WORD*)&(Kat + n)->Name);
      char nam[128], ext[128];
      if(pr_tRec == 0)                                       //Признак рекордера 0 или 1 (старейшая серия)
      {  UnicodeToAnsi((Kat + n)->Name, nam, 41);            //Преобразовали имя из UNICODE
         UnicodeToAnsi((Kat + n)->Ext, ext, 4);              //Преобразовали расширение из UNICODE
      }
      if(pr_tRec == 1)                                       //Признак рекордера 0 или 1 (старейшая серия)
      {  UnicodeToAnsi((Kat + n)->Name, nam, 45);            //Преобразовали имя и расширение из UNICODE
         lstrcpy(ext, nam + 42);                             //Выбрали расширение
         if(*ext == ' ') *ext = 0;                           //Если расширения нет, то очистили
         for(int i=41; i>0; i--)                             //Очистили хвост имени от пробелов остави только символы
         {  if(*(nam + i) != ' ') break;                     //Поставили конец имени после последнего символа
            *(nam + i) = 0;
         }
      }
      if(*ext != 0) sprintf(Ss, "%03d. %s.%s", n+1, nam, ext);
      else strcpy(Ss, nam);
      int l = lstrlen(Ss);
      for(int i=l; i<50; i++) Ss[i] = ' ';
      as = Ss + 50;
      as += sprintf(as, "%18.0lf", double((Kat + n)->pf.SizeF));
      as += sprintf(as, "%10d  ", (Kat + n)->pf.ClSt);
      as += sprintf(as, "%02d-%02d-%04d  %02d:%02d:%02d  ", (Kat + n)->pf.Day, (Kat + n)->pf.Mon, (Kat + n)->pf.Year,
                                                     (Kat + n)->pf.Hour, (Kat + n)->pf.Min, (Kat + n)->pf.Sec);
      BYTE *b = (BYTE*)&(Kat + n)->pf.lL1;
      as += sprintf(as, "| %02X%02X%02X%02X %02X%02X%02X%02X |", b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
      as += sprintf(as, "%16.0lf |", double((Kat + n)->pf.lL2));
      if(NameDel == 0xE500)                                  //Это удаленное имя (первая буква имени а)
         as += sprintf(as, "  УДАЛЕНО");
      Add_Spis(Ss);                                          //Добавление строки в список
      if((Kat + n)->pf.type != 32) // && (Kat + n)->pf.type != 48)
         Add_Spis("Неизвестный тип записи в каталоге.");
   }
   Add_Spis("");                                             //Добавление строки в список
   Add_Spis("");                                             //Добавление строки в список
   return 0;                                                 //Файл не найден
}
#endif

//------------------------------------------------------------------------------

int  Read_Test2(void)                                        //Тестовое чтение и вывод
{
#if defined TEST_LG2_ALL
   if(View_Dir_Part2() < 0) return -1;                       //Визуализация каталога второго раздела
#endif
   if(Read_Dir_Part2_MME() < 0) return -1;                   //Чтение каталога второго раздела и файла MME
   endMME = MMe + SizeMME;                                   //Адрес конца записей в массиве MME
/*
   BYTE *t_buff = MMe;
   int N = (SizeMME + sSecB - 1) / sSecB;
   for(int i=0; i<=N; i++)                                   //По числу секторов в кластере
   {  View_Sec_Hex(t_buff, 0);                               //Вывод содержимого сектора
      t_buff += sSecB;                                       //Увеличили адрес вывода на размер сектора
      Add_Spis("----------------------------------------------------------------------------");
   }
*/
   int ident = CtrlRecoder();                                //Идентификация рекордера
   if(ident < 0)
   {  Error1("Рекордер неопознан.");
      BYTE *t_buff = MMe;
      int N = (SizeMME + sSecB - 1) / sSecB;
      for(int i=0; i<N; i++)                                 //По числу секторов в кластере
      {  View_Sec_Hex(t_buff, 0);                            //Вывод содержимого сектора
         t_buff += sSecB;                                    //Увеличили адрес вывода на размер сектора
         Add_Spis("----------------------------------------------------------------------------");
      }
      return 0;
   }
   switch(ident)
   {  case 0: if(ViewMME0_Hex() < 0) return -1; break;
      case 1: if(ViewMME1_Hex() < 0) return -1; break;
      case 2:
      case 3: if(ViewMME2_Hex() < 0) return -1; break;
      case 4: if(ViewMME4_Hex() < 0) return -1; break;
      case 5: if(ViewMME5_Hex() < 0) return -1; break;
   }
   if(at_MME == endMME)
   {  Add_Spis("Файл MME обработан успешно.");  return 0;  }
   int nOst = DWORD(endMME) - DWORD(at_MME);                 //Число байт оставшееся до конца файла
   char *as = Ss;
   as += sprintf(as, "Конец файла MME: ");                   //Порядковый номер записи
   for(int i=0; i<nOst; i++)
   {  as += sprintf(as, " %02X", *at_MME);
      at_MME++;
   }
   *as = 0;
   Add_Spis(Ss);
//Память освобождается при выходе их программы
   return 0;
}

#endif

#endif

