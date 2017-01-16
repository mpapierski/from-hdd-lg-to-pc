
//                               11.08.2008
#ifndef __HDD_LG_MME_DB__
#define __HDD_LG_MME_DB__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_mme_db ===================================
int  CtrlSize_MME(DWORD Delta);                              //Контроль смещения в массиве MME
int  CtrlRecoder(void);                                      //Идентификация рекордера
int  Read_Dir_Part2_MME(void);                               //Чтение каталога второго раздела и файла MME
int  ReadMME_DB(void);                                       //Чтение файла базы записей
int  Update_MME_DB(void);                                    //Пересоздание массива информативных имен
     BYTE *MMe;                                              //Содержимое файла базы
     DWORD SizeMME;                                          //Число байт в файле MME
     TABL_MME *tabMME;                                       //Таблица соответствия имен
     DWORD numNam;                                           //Число имен в таблице MME (больше числа Title из-за наличия частей)
     TABL_MME *tabMME_Part;                                  //Таблица имен отредактированных Title
     DWORD numNam_Part;                                      //Число имен в таблице имен отредактированных Title

static BYTE *endMME;                                         //Адрес конца записей в массиве MME
static BYTE *at_MME;                                         //Адрес начала текущего элемента записи в массиве MME
static int idRec;                                            //Идентификатор рекордера
static char Title[128];                                      //Имя файла записанного в папке MEDIA
static char nameTV[8];

//------------------------------------------------------------------------------

int CtrlSize_MME(DWORD Delta)                                //Контроль смещения в массиве MME
{
   at_MME += Delta;                                          //Адрес начала следеющего элемента записи
   if(at_MME > endMME)                                       //Вышли за пределы массива MME
     return Error4((Lan+107)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg);//return Error1("Структура файла MME.DB неизвестна автору программы.");
   return 0;
}

//------------------------------------------------------------------------------

static int Ctrl_Recoder(int d, int e)                        //Идентификация рекордера
{
   if(*(MMe + d+0) != 0 || *(MMe + d+1) != 0x41 || *(MMe + d+2) != 0 || *(MMe + d+3) != 0x3A ||
      *(MMe + d+4) != 0 || *(MMe + d+5) != 0x5C || *(MMe + e+0) != 0 || *(MMe + e+1) != 0x2E ||
      *(MMe + e+2) != 0 || *(MMe + e+3) != 0x73 || *(MMe + e+4) != 0 || *(MMe + e+5) != 0x74 ||
      *(MMe + e+6) != 0 || *(MMe + e+7) != 0x72 ||
      *(MMe + d+64) != 0 || *(MMe + d+65) != 0x41 || *(MMe + d+66) != 0 || *(MMe + d+67) != 0x3A ||
      *(MMe + d+68) != 0 || *(MMe + d+69) != 0x5C || *(MMe + e+64) != 0 || *(MMe + e+65) != 0x2E ||
      *(MMe + e+66) != 0 || *(MMe + e+67) != 0x69 || *(MMe + e+68) != 0 || *(MMe + e+69) != 0x64 ||
      *(MMe + e+70) != 0 || *(MMe + e+71) != 0x78 ) return 0;
   return 1;
}

//------------------------------------------------------------------------------

int CtrlRecoder(void)                                        //Идентификация рекордера
{
   if(MMe == NULL) return -1;
   if(Ctrl_Recoder(300, 346) == 1) return 0;                 //Идентификация рекордера
   if(Ctrl_Recoder(308, 358) == 1) return 1;                 //Идентификация рекордера
   if(Ctrl_Recoder(316, 366) == 1) return (2 + Conf.typeRec);//Идентификация рекордера
   if(Ctrl_Recoder(324, 374) == 1) return 4;                 //Идентификация рекордера
   if(Ctrl_Recoder(332, 382) == 1) return 5;                 //Идентификация рекордера
   return -1;
}

//-------------------------------------------------------------------------------

static void Recalc_Par(WCHAR *wTitle, char *name_TV, DATE_T2 *DaT2,
                       WORD *nPart, DATE_T1 *DaT1)           //Преобразование данных
{
   char Title0[128];                                         //Имя файла записанного в папке MEDIA
   UnicodeToAnsi(wTitle, Title0, 33);                        //Преобразовали имя Title
   for(int j=0; j<6; j++) *(nameTV + j) = *(name_TV + j);
   for(int j=4; j>0; j--)                                    //Удаление всех хвостовых пробелов
   {  if(*(nameTV + j) != ' ') break;                        //Первый не пробел
      *(nameTV + j) = 0;                                     //Убрали пробел
   }
   SWAP16(nPart);                                            //Число частей в данном Title
   SWAP16(&DaT2->Year);
   DaT2->Year = WORD(DaT2->Year - (DaT2->Year / 100) * 100); //Оставили два знака
   SWAP16(&DaT1->year);
   SWAP16(&DaT1->mon);
   SWAP16(&DaT1->day);
   SWAP16(&DaT1->hour);
   wsprintf(Title, "%02d%02d%02d%02d%02d %s", DaT2->Year, DaT2->Mon, DaT2->Day, DaT2->Hour, DaT2->Min, Title0);
// wsprintf(cData, "%02d%02d%02d%02d%02d%02d %s", DaT2->Year, DaT2->Mon, DaT2->Day, DaT2->Hour, DaT2->Min, DaT2->Sec, Title0);
}

//-------------------------------------------------------------------------------

char *Qu[6][5] = { { "HQ", "SQ", "LQ", "--", "--" },         //рекордер 0
                   { "EQ", "LQ", "SQ", "HQ", "--" },         //рекордер 1
                   { "EP", "LP", "SP", "XP", "--" },         //рекордер 2
                   { "MLP", "EP", "LP", "SP", "XP" },        //рекордер 3
                   { "MLP", "EP", "LP", "SP", "XP" },        //рекордер 4
                   { "MLP", "EP", "LP", "SP", "XP" },        //рекордер 5
                 };
#pragma argsused
static int MakeNewName(DWORD t, int n, WORD nPart, BYTE Q, BYTE nAV, DATE_T1 *DaT1)//Создание имени и занесение его в таблицу имен по файлу MME
{
   char nameIn[8];
   ONE_NAME *aNam = (ONE_NAME *)at_MME;                      //Адрес начала записей по одной части данного Title
   if(CtrlSize_MME(sizeof(ONE_NAME)) < 0) return -1;         //Контроль смещения в массиве MME
//??SWAP64(&DWORDLONG(aNam->sizeF));
   SWAP64((DWORDLONG*)&aNam->sizeF);
   (tabMME+numNam)->SizeF = aNam->sizeF;                     //Занесли Размер файла
   SWAP32(&aNam->timeLong);
   (tabMME+numNam)->timeLong = aNam->timeLong;               //Занесли Длительность записей в файле
   UnicodeToAnsi(aNam->Name1+3, (tabMME+numNam)->NameF, 27); //Занесли Преобразовали имя файла в папке MEDIA (убрали символы A:\)
   *strchr((tabMME+numNam)->NameF, '.') = 0;                 //Обрубили расширение
   if(Q > 4) Q = 4;
   if(nAV == 119) sprintf(nameIn, "DVD");                    //Обозначение источника сигнала
   else  if(nAV == 0) sprintf(nameIn, "%s", nameTV);
         else sprintf(nameIn, "AV%1d", nAV);
   lstrcpy((tabMME+numNam)->aName, Title);                   //Альтернативное имя файла
   lstrcpy((tabMME+numNam)->Qual, Qu[idRec][Q]);
   lstrcpy((tabMME+numNam)->nameIn, nameIn);
   (tabMME+numNam)->numPart = 0;                             //Номер части
   (tabMME+numNam)->dt = *DaT1;                              //Дата и время начала записи файла
   (tabMME+numNam)->prYes = 0;                               //Признак, что имя парное (1- есть и в mme.db и в каталоге)
   (tabMME+numNam)->indFolder = 0xFFFF;
   (tabMME+numNam)->nPart = nPart;
   (tabMME+numNam)->indTitle = WORD(t+1);                    //Порядковый номер Title
   if(nPart > 1)                                             //Если больше одной части
   { (tabMME+numNam)->numPart = WORD(n + 1);                 //Номер части
     (tabMME+numNam)->indFolder = WORD(numNam_Part);         //Индекс строки в таблице имен папок многофайловых Title
     if(n == 0)                                              //Первая часть многофайлового Title
     { *(tabMME_Part+numNam_Part) = *(tabMME+numNam);        //Таблица имен отредактированных Title
       numNam_Part++;                                        //Число имен в таблице имен отредактированных Title
     }
     else
     { (tabMME_Part+numNam_Part-1)->SizeF += aNam->sizeF;    //Суммарный размер
       (tabMME_Part+numNam_Part-1)->timeLong += aNam->timeLong;//Суммарная длительность записей в файле
     }
   }
   if(++numNam >= MAX_NAME_MME)
      return Error1((Lan+106)->msg);                         //return Error1("Число записей в файле MME превысило возможности программы.");
   return 0;
}

//-------------------------------------------------------------------------------

static int Make_Tab_Name_MME0(DWORD NumT)                    //Создание таблицы имен по файлу MME
{
   for(DWORD t=0; t<NumT; t++)                               //По всему файлу MME
   {  PSP_TIT0 *aTit = (PSP_TIT0 *)at_MME;                   //Адрес начала заголовка очередной записи
      if(CtrlSize_MME(sizeof(PSP_TIT0)) < 0) return -1;      //Контроль смещения в массиве MME
      Recalc_Par(aTit->Title, aTit->nameTV, &aTit->DaT2, &aTit->nPart, &aTit->DaT1);
      for(int n=0; n<aTit->nPart; n++)                       //По числу частей в одном Title
         if(MakeNewName(t, n, aTit->nPart, aTit->Q, aTit->nAV, &aTit->DaT1) < 0) return -1; //Создание имени и занесение его в таблицу имен по файлу MME
//??  END_REC *eRec = (END_REC *)atMME;
//??  atMME += sizeof(END_REC);
//??  if(eRec->nM == 0)  continue;
//??  SWAP16(&eRec->nM);
//??  atMME += eRec->nM * sizeof(ONE_MET);                   //Пропустили все записи о метках
   }
   return 0;
}

//-------------------------------------------------------------------------------

static int Make_Tab_Name_MME1(DWORD NumT)                    //Создание таблицы имен по файлу MME
{
   for(DWORD t=0; t<NumT; t++)                               //По всему файлу MME
   {  PSP_TIT1 *aTit = (PSP_TIT1 *)at_MME;                   //Адрес начала заголовка очередной записи
      if(CtrlSize_MME(sizeof(PSP_TIT1)) < 0) return -1;      //Контроль смещения в массиве MME
      Recalc_Par(aTit->Title, aTit->nameTV, &aTit->DaT2, &aTit->nPart, &aTit->DaT1);
      for(int n=0; n<aTit->nPart; n++)                       //По числу частей
         if(MakeNewName(t, n, aTit->nPart, aTit->Q, aTit->nAV, &aTit->DaT1) < 0) return -1; //Создание имени и занесение его в таблицу имен по файлу MME
      END_REC *eRec = (END_REC *)at_MME;
      if(CtrlSize_MME(sizeof(END_REC)) < 0) return -1;       //Контроль смещения в массиве MME
      if(eRec->numMet == 0)  continue;                       //Меток нет
      SWAP16(&eRec->numMet);
      if(CtrlSize_MME(eRec->numMet * sizeof(ONE_MET)) < 0) return -1;//Пропустили все записи о метках
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Make_Tab_Name_MME2(DWORD NumT)                    //Создание таблицы имен по файлу MME
{
   for(DWORD t=0; t<NumT; t++)                               //По всему файлу MME
   {  PSP_TIT2 *aTit = (PSP_TIT2 *)at_MME;                   //Адрес начала заголовка очередной записи
      if(CtrlSize_MME(sizeof(PSP_TIT2)) < 0) return -1;      //Контроль смещения в массиве MME
      Recalc_Par(aTit->Title, aTit->nameTV, &aTit->DaT2, &aTit->nPart, &aTit->DaT1);
      for(int n=0; n<aTit->nPart; n++)                       //По числу частей в одном Title
         if(MakeNewName(t, n, aTit->nPart, aTit->Q, aTit->nAV, &aTit->DaT1) < 0) return -1; //Создание имени и занесение его в таблицу имен по файлу MME
      END_REC *eRec = (END_REC *)at_MME;
      if(CtrlSize_MME(sizeof(END_REC)) < 0) return -1;       //Контроль смещения в массиве MME
      if(eRec->numMet != 0)
      {  SWAP16(&eRec->numMet);
         if(CtrlSize_MME(eRec->numMet * sizeof(ONE_MET)) < 0) return -1;//Пропустили все записи о метках
      }
      END0_ZAP_MME *aEnd0 = (END0_ZAP_MME *)at_MME;          //Адрес начала заголовка очередной записи
      if(CtrlSize_MME(sizeof(END0_ZAP_MME)) < 0) return -1;  //Контроль смещения в массиве MME
      SWAP16(&aEnd0->NN);
      if(CtrlSize_MME(aEnd0->NN * sizeof(END1_ZAP_MME)) < 0) return -1;//Пропустили все записи не знаю о чем
   }
   return 0;
}

//-------------------------------------------------------------------------------

static int  Make_Tab_Name_MME4(DWORD NumT)                   //Создание таблицы имен по файлу MME
{
   for(DWORD t=0; t<NumT; t++)                               //По всему файлу MME
   {  PSP_TIT4 *aTit = (PSP_TIT4 *)at_MME;                   //Адрес начала заголовка очередной записи
      if(CtrlSize_MME(sizeof(PSP_TIT4)) < 0) return -1;      //Контроль смещения в массиве MME
      Recalc_Par(aTit->Title, aTit->nameTV, &aTit->DaT2, &aTit->nPart, &aTit->DaT1);
      for(int n=0; n<aTit->nPart; n++)                       //По числу частей в одном Title
         if(MakeNewName(t, n, aTit->nPart, aTit->Q, aTit->nAV, &aTit->DaT1) < 0) return -1; //Создание имени и занесение его в таблицу имен по файлу MME
      END_REC *eRec = (END_REC *)at_MME;
      if(CtrlSize_MME(sizeof(END_REC)) < 0) return -1;       //Контроль смещения в массиве MME
      if(eRec->numMet != 0)
      {  SWAP16(&eRec->numMet);
         if(CtrlSize_MME(eRec->numMet * sizeof(ONE_MET)) < 0) return -1;//Пропустили все записи о метках
      }
      END0_ZAP_MME *aEnd0 = (END0_ZAP_MME *)at_MME;          //Адрес начала заголовка очередной записи
      if(CtrlSize_MME(sizeof(END0_ZAP_MME)) < 0) return -1;  //Контроль смещения в массиве MME
      SWAP16(&aEnd0->NN);
      if(CtrlSize_MME(aEnd0->NN * sizeof(END1_ZAP_MME)) < 0) return -1;//Пропустили все записи не знаю о чем
   }
   return 0;
}

//-------------------------------------------------------------------------------

static int Make_Tab_Name_MME5(DWORD NumT)                    //Создание таблицы имен по файлу MME
{
   for(DWORD t=0; t<NumT; t++)                               //По всему файлу MME
   {  PSP_TIT5 *aTit = (PSP_TIT5 *)at_MME;                   //Адрес начала заголовка очередной записи
      if(CtrlSize_MME(sizeof(PSP_TIT5)) < 0) return -1;      //Контроль смещения в массиве MME
      Recalc_Par(aTit->Title, aTit->nameTV, &aTit->DaT2, &aTit->nPart, &aTit->DaT1);
      for(int n=0; n<aTit->nPart; n++)                       //По числу частей в одном Title
         if(MakeNewName(t, n, aTit->nPart, aTit->Q, aTit->nAV, &aTit->DaT1) < 0) return -1; //Создание имени и занесение его в таблицу имен по файлу MME
      END_REC *eRec = (END_REC *)at_MME;
      if(CtrlSize_MME(sizeof(END_REC)) < 0) return -1;       //Контроль смещения в массиве MME
      if(eRec->numMet != 0)
      {  SWAP16(&eRec->numMet);
         if(CtrlSize_MME(eRec->numMet * sizeof(ONE_MET)) < 0) return -1;//Пропустили все записи о метках
      }
      END0_ZAP_MME *aEnd0 = (END0_ZAP_MME *)at_MME;          //Адрес начала заголовка очередной записи
      if(CtrlSize_MME(sizeof(END0_ZAP_MME)) < 0) return -1;  //Контроль смещения в массиве MME
      SWAP16(&aEnd0->NN);
      if(CtrlSize_MME(aEnd0->NN * sizeof(END1_ZAP_MME)) < 0) return -1;//Пропустили все записи не знаю о чем
   }
   return 0;
}

//-------------------------------------------------------------------------------

static int Make_Tab_Name_MME(void)                           //Создание таблицы имен по файлу MME
{
   tabMME = (TABL_MME *)MyAllocMem((MAX_NAME_MME+256) * sizeof(TABL_MME));//Таблица соответствия имен
   if(tabMME == NULL)  return -1;
   tabMME_Part = tabMME+MAX_NAME_MME;                        //Таблица имен отредактированных Title
   numNam_Part = 0;                                          //Число имен в таблице имен отредактированных Title
   at_MME = MMe;
   PSP_MME *pspMME = (PSP_MME *)at_MME;                      //Адрес начала паспорта файла MME
   if(CtrlSize_MME(sizeof(PSP_MME)) < 0) return -1;          //Контроль смещения в массиве MME
   SWAP32(&pspMME->NumT);                                    //Число Title в файле MME
   if(pspMME->NumT == 0) return 0;                           //Нет ни одной записи в базе
   idRec = CtrlRecoder();                                    //Идентификация рекордера
   if(idRec < 0)
      return Error4((Lan+107)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg); //return Error1("Тип рекордера неопознан.");
   switch(idRec)
   { case 0: if(Make_Tab_Name_MME0(pspMME->NumT) < 0) return -1; break;
     case 1: if(Make_Tab_Name_MME1(pspMME->NumT) < 0) return -1; break;
     case 2:
     case 3: if(Make_Tab_Name_MME2(pspMME->NumT) < 0) return -1; break;
     case 4: if(Make_Tab_Name_MME4(pspMME->NumT) < 0) return -1; break;
     case 5: if(Make_Tab_Name_MME5(pspMME->NumT) < 0) return -1; break;
   }
   if(at_MME != endMME)
      return Error4((Lan+107)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg);//return Error1("Структура файла MME.DB неизвестна автору программы.");
   return 0;
}

//------------------------------------------------------------------------------

int Read_File_MME(DWORD ClSt)                                //Чтение файла базы
{
   DWORD numCl = (SizeMME + sCl2_B - 1) / sCl2_B;            //Число кластеров необходимое для размещения файла данного размера
   DWORD sizeF = numCl * sCl2_B;                             //Запас под полный кластер
   MMe = (BYTE *)MyAllocMem(sizeF);                          //Память файл базы
   if(MMe == NULL)  return -1;
   endMME = MMe + SizeMME;                                   //Адрес конца записей в массиве MME
   BYTE *aMME = MMe;
   DWORD wSizeB = sCl2_B;                                    //Число записываемых байт равно числу байт в кластере
   DWORD nCl = ClSt;                                         //Текущий номер кластера равен первому кластеру файла
   DWORD SizeToEnd = SizeMME;                                //Число байт которые осталось записать
   for(DWORD i=0; i<numCl; i++)                              //По числу кластеров
   {  if(nCl == 0x0FFFFFFF)
        return Error1((Lan+12)->msg);                        //return Error1("Неожиданно найден признак конца цепочки FAT.");
      if(*(FAT2 + nCl) == 0)
        return Error1((Lan+11)->msg);                        //return Error1("Обнаружено несоответствие значения FAT и ссылки на кластер файла.");
      if(wSizeB > SizeToEnd) wSizeB = DWORD(SizeToEnd);      //Размер остатка записи меньше размера кластера
      DWORD nSector = Start_SecDir2 + (nCl - 1) * sClSec2;   //Сектор начала текущего кластера
#if defined OUT_TEST
      char Ss[300];
      wsprintf(Ss, "Load Dir (claster2 %d, sector %d)", nCl, nSector);
      Add_SpecSpis(Ss);
#endif
      if(ReadClast2_P(nSector, aMME) < 0) return -1;         //Чтение кластера
#if defined OUT_TEST
      View_HEX_Any(aMME, sCl2_B);
#endif
      aMME += sCl2_B;
      SizeToEnd -= wSizeB;                                   //Число байт которые осталось записать
      nCl = *(FAT2 + nCl);                                   //Номер следующего кластера
      if(nCl > maxZapFAT2 && nCl != 0x0FFFFFFF)
        return Error1((Lan+13)->msg);                        //"Номер кластера превышает допустимое значение."
      if(SizeToEnd <= 0 && nCl != 0x0FFFFFFF)
        return Error1((Lan+29)->msg);                        //return Error2("Выходной файл заданного размера записан,", "а признак конца цепочки FAT не найден.");
   }
   return 0;
}

//------------------------------------------------------------------------------

int Read_Dir_Part2_MME(void)                                 //Чтение каталога второго раздела и файла MME
{
   BYTE buff[sCl2_B];

   SizeMME = 0;                                              //Нулевая длина файла (как признак отсутствия)
   tabMME = NULL;                                            //Таблица соответствия имен
   numNam = 0;                                               //Число имен в таблице MME
#if defined OUT_TEST
   DWORD nClast = 1;
   DWORD nSector = Start_SecDir2 + (nClast-1) * sClSec;      //Номер сектора по номеру кластера
   char Ss[300];
   wsprintf(Ss, "Load Dir (claster2 %d, sector %d)", nClast, nSector);
   Add_SpecSpis(Ss);
#endif
   if(ReadClast2_P(Start_SecDir2, buff) < 0) return -1;      //Чтение кластера
#if defined OUT_TEST
   View_HEX_Any(buff, sCl2_B);
#endif
   One_Str_Cat *Kat = (One_Str_Cat *)buff;
   for(DWORD n=0; n<4 * sClSec2; n++)                          //В каждом секторе 4 записи
   {  if((Kat + n)->pf.type == 0) return 0;                  //Конец каталога и файл не найден
//      if((Kat + n)->pf.type == 0xFFFF) continue;             //Конец каталога и файл не найден
//    if(n == 0) return 0;                                   //Нет записей в папке MEDIA
//    else return Error4((Lan+15)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg); //"Неизвестный тип записи в строке каталога."
      WORD NameDel = *((WORD*)&(Kat + n)->Name);
      if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0) continue; //Это удаленное имя
      if((Kat + n)->pf.type != 32 && (Kat + n)->pf.type != 48)
        return Error4((Lan+15)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg); //"Неизвестный тип записи в строке каталога."
      int prRec = 0;
      if(*(Kat->Name+14) == 0x2000) prRec = 1;               //Старейшая серия (у нее пробелы в имени)
      char nam[128], ext[128];
      if(prRec == 0)                                         //Признак рекордера 0 или 1
      {  UnicodeToAnsi(Kat->Name, nam, 41);                  //Преобразовали имя
         UnicodeToAnsi(Kat->Ext, ext, 4);                    //Преобразовали расширение
      }
      if(prRec == 1)                                         //Признак рекордера 0 или 1 (старейшая серия)
      {  UnicodeToAnsi(Kat->Name, nam, 45);                  //Преобразовали имя
         lstrcpy(ext, nam + 42);
         if(*(ext+2) == ' ') *(ext+2) = 0;                   //В данном каталоге может быть только расширение DB (2 символа)
         for(int i=41; i>0; i--)
         {  if(*(nam + i) != ' ') break;
            *(nam + i) = 0;
         }
      }
      if(lstrcmp(nam, "MME") != 0) continue;                 //Это не база
//В дампе файла "BACKUP" нет
//    if(lstrcmp(nam, "BACKUP") != 0) continue;              //Это не база
      if(lstrcmp(ext, "DB") != 0) continue;                  //Это не база
      SizeMME = DWORD((Kat + n)->pf.SizeF);                  //Число байт в файле MME
      if(SizeMME == 0) return 0;                             //Нулевая длина файла
      return Read_File_MME((Kat + n)->pf.ClSt);              //Чтение файла базы
   }
   return 0;                                                 //Файл не найден
}

//------------------------------------------------------------------------------

#if defined VIEW_TAB_MME
static void View_Tab_MME_DB(void)                            //Визуализация таблицы
{
   char Ss[1024];
   for(DWORD i=0; i<numNam; i++)
   {  int tlHour = BYTE((tabMME + i)->timeLong / 3600);
      int M1 = (tabMME + i)->timeLong - tlHour * 3600;
      int tlMin = BYTE(M1 / 60);
      int tlSec = BYTE(M1 - tlMin * 60);
      sprintf(Ss, "%4d. %-024s (n=%5d) %-040s %-08s %-08s %12.0lf [%02d:%02d:%02d] R %02d-%02d-%04d %02d:%02d:%02d",
                  i+1, (tabMME + i)->NameF, (tabMME + i)->numPart, (tabMME + i)->aName,
                  (tabMME + i)->Qual, (tabMME + i)->nameIn, double((tabMME + i)->SizeF),
                  tlHour, tlMin, tlSec,
                  (tabMME + i)->dt.day, (tabMME + i)->dt.mon, (tabMME + i)->dt.year,
                  (tabMME + i)->dt.hour, (tabMME + i)->dt.min, (tabMME + i)->dt.sec);
      Add_Spis(Ss);                                          //Добавление строки в список
   }
}
#endif

//------------------------------------------------------------------------------

int ReadMME_DB(void)                                         //Чтение файла базы записей
{
   if(Conf.altName == 1) return 0;                           //Не показывать информативные имена
   if(Read_Dir_Part2_MME() < 0) return -1;                   //Чтение каталога второго раздела и файла MME
   if(SizeMME == 0) return 0;                                //Нулевая длина файла
#if !defined OUT_TEST1 && !defined TEST_LG2_MME
   int ret = Make_Tab_Name_MME();                            //Создание таблицы имен по файлу MME
   MyFreeMem(reinterpret_cast<void**>(&MMe));
   if(ret < 0)
   {  MyFreeMem(reinterpret_cast<void**>(&tabMME));
      return -1;
   }
  #if defined VIEW_TAB_MME
   View_Tab_MME_DB();                                        //Визуализация таблицы
  #endif
#endif
   return 0;
}

//------------------------------------------------------------------------------

int Update_MME_DB(void)                                      //Пересоздание массива информативных имен
{
   MyFreeMem(reinterpret_cast<void**>(&MMe));
   MyFreeMem(reinterpret_cast<void**>(&tabMME));                                //Таблица соответствия имен
   return ReadMME_DB();                                      //Чтение файла базы записей
}

#endif

