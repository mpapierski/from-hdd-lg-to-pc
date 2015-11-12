
//                                    12.02.2009
#ifndef __HDD_LG_READ_IO__
#define __HDD_LG_READ_IO__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_read_io ==================================
int  ReadSec(BYTE *buff);                                    //Чтение сектора
int  ReadClast1(BYTE *buff);                                 //Чтение кластера первого раздела начиная с текущего сектора
int  ReadClast1_P(DWORD nSector, BYTE *buff);                //Чтение кластера первого раздела начиная с указанного сектора
int  ReadClast2(BYTE *buff);                                 //Чтение кластера второго раздела начиная с текущего сектора
int  ReadClast2_P(DWORD nSector, BYTE *buff);                //Чтение кластера второго раздела начиная с указанного сектора
int  Load_FAT1(void);                                         //Загрузка FAT первого раздела
int  Load_FAT2(void);                                        //Загрузка FAT второго раздела
int  FindHDD_LG(void);                                       //Поиск диска из рекордера LG
     DWORD stCl_Media2;                                      //Кластер начала второй части папки MEDIA

//------------------------------------------------------------------------------

int ReadSec(BYTE *buff)                                      //Чтение сектора
{
   DWORD nb;
   if(ReadFile(hDrive, buff, sSecB, &nb, NULL) == FALSE || nb != sSecB)
   {  ErrorSys1((Lan+9)->msg);                               //Ошибка при чтении сектора.
      CloseFile(&hDrive);                                    //Закрыли входной файл
      return -1;
   }
   return 0;
}

//------------------------------------------------------------------------------

int ReadClast1(BYTE *buff)                                   //Чтение кластера
{
   DWORD nb;
   if(ReadFile(hDrive, buff, sCl_B, &nb, NULL) == FALSE || nb != sCl_B)
   {  ErrorSys1((Lan+10)->msg);                              //ErrorSys1("Ошибка при чтении кластера.");
      CloseFile(&hDrive);                                    //Закрыли входной файл
      return -1;
   }
   return 0;
}

//------------------------------------------------------------------------------

int  ReadClast1_P(DWORD nSector, BYTE *buff)                 //Чтение кластера
{
   LONGLONG Poz = LONGLONG(sSecB) * nSector;
#if defined EMULATOR_HDD
   if(ReadClastEmul(Poz, buff) < 0) return -1;               //Чтение кластера эмулятором
#else
   if(SetInFilePointer(Poz) < 0) return -1;                  //Изменение позиции указателя в файле
   if(ReadClast1(buff) < 0) return -1;                       //Чтение кластера
#endif
   return 0;
}

//------------------------------------------------------------------------------

int ReadClast2(BYTE *buff)                                   //Чтение кластера второго раздела
{
   DWORD nb;
   if(ReadFile(hDrive, buff, sCl2_B, &nb, NULL) == FALSE || nb != sCl2_B)
   {  ErrorSys1((Lan+10)->msg);                              //ErrorSys1("Ошибка при чтении кластера.");
      CloseFile(&hDrive);                                    //Закрыли входной файл
      return -1;
   }
   return 0;
}

//------------------------------------------------------------------------------

int  ReadClast2_P(DWORD nSector, BYTE *buff)                 //Чтение кластера
{
   LONGLONG Poz = LONGLONG(sSecB) * nSector;
#if defined EMULATOR_HDD
   if(ReadClast2Emul(Poz, buff) < 0) return -1;              //Чтение кластера эмулятором
#else
   if(SetInFilePointer(Poz) < 0) return -1;                  //Изменение позиции указателя в файле
   if(ReadClast2(buff) < 0) return -1;                       //Чтение кластера
#endif
   return 0;
}

//------------------------------------------------------------------------------
/*
static int Ctrl_FAT_For_File(DWORD clSt, LONGLONG SizeF)     //Проверка соответствия длины файла и FAT
{
   DWORD nCl = clSt;                                         //Текущий номер кластера равен первому кластеру файла
   DWORD wSizeB = sCl_B;                                     //Число записываемых байт равно числу байт в кластере
   LONGLONG SizeToEnd = SizeF;                               //Число байт которые осталось в цепочке
   DWORD numCl = DWORD((SizeF + sCl_B - 1) / sCl_B);         //Число кластеров необходимое для размещения файла данного размера
   for(DWORD i=0; i<numCl; i++)                              //По числу кластеров которые нужны для данного файла
   {  if(nCl == 0x0FFFFFFF)
        return Error1((Lan+12)->msg);                        //return Error1("Неожиданно найден признак конца цепочки FAT.");
      if(*(FAT + nCl) == 0)
        return Error1((Lan+11)->msg);                        //return Error1("Обнаружено несоответствие значения FAT и ссылки на кластер файла.");
      if(wSizeB > SizeToEnd) wSizeB = DWORD(SizeToEnd);      //Размер остатка записи меньше размера кластера
      SizeToEnd -= wSizeB;                                   //Число байт которые осталось записать
      nCl = *(FAT + nCl);                                    //Номер следующего кластера
      if(nCl > num_ZapFAT && nCl != 0x0FFFFFFF)
        return Error1((Lan+13)->msg);                        //return Error2("Номер кластера в цепочке FAT", "превышает допустимое значение.");
      if(SizeToEnd == 0 && nCl != 0x0FFFFFFF)
        return Error1((Lan+14)->msg);                        //return Error2("Нужное число кластеров пройдено,", "а признак конца цепочки FAT не найден.");
   }
   return 0;
}
*/

//------------------------------------------------------------------------------

static int ReadFAT1(void)                                    //Чтение FAT первого раздела
{
   DWORD nb;
   if(ReadFile(hDrive, FAT1, Size_FAT1, &nb, NULL) == FALSE || nb != Size_FAT1)
   {   ErrorSys1((Lan+22)->msg);                             //ErrorSys1("Ошибка при чтении FAT.");
       CloseFile(&hDrive);                                   //Закрыли входной файл
       return -1;
   }
   return 0;
}

//------------------------------------------------------------------------------

static int ReadFAT2(void)                                    //Чтение FAT второго раздела
{
   DWORD nb;
   if(ReadFile(hDrive, FAT2, Size_FAT2, &nb, NULL) == FALSE || nb != Size_FAT2)
   {   ErrorSys1((Lan+22)->msg);                             //ErrorSys1("Ошибка при чтении FAT.");
       CloseFile(&hDrive);                                   //Закрыли входной файл
       return -1;
   }
   return 0;
}

//-------------------------------------------------------------------------------

int Load_FAT1(void)                                          //Загрузка FAT первого раздела
{
#if defined OUT_TEST
   char Ss[300];
   wsprintf(Ss, "Load FAT 1 (sector %d, size (sectors) %d)", Start_SecFAT1, num_SecFAT1);
   Add_SpecSpis(Ss);
#endif
   FAT1 = (DWORD *)MyAllocMem(Size_FAT1);                    //Память под FAT
   if(FAT1 == NULL)  return -1;
   LONGLONG Poz = LONGLONG(sSecB) * Start_SecFAT1;
#if defined EMULATOR_HDD
   if(ReadFATEmul(Poz) < 0) return -1;                       //Чтение FAT эмулятором
#else
   if(SetInFilePointer(Poz) < 0) return -1;                  //Изменение позиции указателя в файле
   if(ReadFAT1() < 0) return -1;                             //Чтение FAT
#endif
#if defined OUT_TEST
   View_HEX_Any((BYTE*)FAT1, Size_FAT1);
#endif
   if(*(FAT1 + 0) != 0x00FFFFF8)
      return Error1((Lan+23)->msg);                          //return Error1("Неожиданное начало FAT.");
   stCl_Media2 = *(FAT1 + 1);                                //Кластер начала второй части папки MEDIA
   //Продолжений может быть до 5, т.е. максимум 6 кластеров
   //Нужен цикл по обработке еще возможных четырех кластеров
   if(stCl_Media2 == 0x0FFFFFFF) stCl_Media2 = 0;            //Вторая часть была, но потом стала не нужной
   if(stCl_Media2 == 0xFFFFFFFF) stCl_Media2 = 0;            //Обработка странного дампа
   return 0;
}

//------------------------------------------------------------------------------

#if defined OUT_TEST
static int Find_FAT2(void)                                   //Поиск и Чтение FAT второго раздела
{
   char Ss[300];
   sprintf(Ss, "Not found FAT2 in sector %d", Start_SecFAT2);
   Add_SpecSpis(Ss);

   DWORD nb;
   InitProgressBar = InitProgressBar1;
   ProgressBar = ProgressBar1;
   Close_ProgressBar = Close_ProgressBar1;
   int numTestSec = Start_SecDir1 - Start_SecFAT1 - num_SecFAT1 - num_SecFAT2; //Число секторов которое надо проанализировать
   DWORD Sec[128];                                           //Размер одного сектора
   int pr = -1;
   LONGLONG Poz = LONGLONG(sSecB) * (Start_SecFAT1 + num_SecFAT1); //Сразу за концом FAT первого раздела
   if(SetInFilePointer(Poz) < 0) return -1;                  //Изменение позиции указателя в файле
   InitProgressBar(numTestSec, (Lan+21)->msg);               //InitProgressBar(nCycl, "Чтение служебной информации");
   for(int i=0; i<numTestSec; i++)
   {  if(ReadFile(hDrive, &Sec, sSecB, &nb, NULL) == FALSE || nb != sSecB)
      {  ErrorSys1((Lan+22)->msg);                           //ErrorSys1("Ошибка при чтении FAT.");
         CloseFile(&hDrive);                                 //Закрыли входной файл
         return -1;
      }
      if(ProgressBar(sSecB) < 0)                             //Оператор нажал кнопку Прервать
      {  Close_ProgressBar();  return -1;   }
      if(*(Sec + 0) != 0x00FFFFF8 || *(Sec + 1) != 0) continue; //Этот сектор не является началом FAT2
      pr = i;
      break;
   }
   Close_ProgressBar();
   if(pr == -1) return -1;
   DWORD stSec_FAT2 = pr + Start_SecFAT1 + num_SecFAT1;
   LONGLONG Poz_St = LONGLONG(sSecB) * stSec_FAT2;
   if(SetInFilePointer(Poz_St) < 0) return -1;               //Изменение позиции указателя в файле
   if(ReadFAT2() < 0) return -1;                             //Чтение FAT второго раздела
   wsprintf(Ss, "Load FAT 2 (sector %d, size (sectors) %d)", stSec_FAT2, num_SecFAT2);
   Add_SpecSpis(Ss);
   return 0;
}
#endif

//-------------------------------------------------------------------------------

int Load_FAT2(void)                                          //Загрузка FAT второго раздела
{
#if defined OUT_TEST
   char Ss[300];
   wsprintf(Ss, "Load FAT 2 (sector %d, size (sectors) %d)", Start_SecFAT2, num_SecFAT2);
   Add_SpecSpis(Ss);
#endif
   FAT2 = (DWORD *)MyAllocMem(Size_FAT2);                     //Память под FAT
   if(FAT2 == NULL)  return -1;
   LONGLONG Poz = LONGLONG(sSecB) * Start_SecFAT2;
#if defined EMULATOR_HDD
   if(ReadFAT2Emul(Poz) < 0) return -1;                      //Чтение FAT эмулятором
#else
   if(Start_SecFAT2 < Start_SecFAT1 + num_SecFAT1 ||
      Start_SecFAT2 >  Start_SecDir1)
  #if defined OUT_TEST
      if(Find_FAT2() < 0) return -1;                         //Поиск и Чтение FAT второго раздела
  #else
      return Error1((Lan+135)->msg);                         //return Error1("Недопустимый номер сектора начала FAT2.");
  #endif
   if(SetInFilePointer(Poz) < 0) return -1;                  //Изменение позиции указателя в файле
   if(ReadFAT2() < 0) return -1;                             //Чтение FAT второго раздела
#endif
#if defined OUT_TEST
   if(*(FAT2 + 0) != 0x00FFFFF8 || *(FAT2 + 1) != 0)         //Если FAT второго раздела не найден
      if(Find_FAT2() < 0) return -1;                         //Поиск и Чтение FAT второго раздела
   View_HEX_Any((BYTE*)FAT2, Size_FAT2);
   if(*(FAT2 + 0) != 0x00FFFFF8 || *(FAT2 + 1) != 0)
      Error1((Lan+23)->msg);                                 //return Error1("Неожиданное начало FAT.");
#else
   if(*(FAT2 + 0) != 0x00FFFFF8 || *(FAT2 + 1) != 0)
      return Error1((Lan+23)->msg);                          //return Error1("Неожиданное начало FAT.");
#endif
   return 0;
}

//------------------------------------------------------------------------------

#define MAX_CHAR 40                                          //Примерная длина строки

static int ErrorSys1_New(char *Msg, DWORD Code)
{
   char Ss[2048], MsgErr[512];

   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, Code,
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                 MsgErr, 512, NULL);
   int n = lstrlen(MsgErr);                                  //Число символов в системном сообщении
   int m = MAX_CHAR;                                         //Длина строки
   if(n > m)
   {  for(;;)
      { int i;
        for(i=m; i<n; i++)                                   //Делим фразу на несколько строк
          if(*(MsgErr+i) == ' ')                             //Делим строку по пробелам
          {  *(MsgErr+i) = '\n';
             m = i + MAX_CHAR + 1;
             break;
          }
        if(n - m < MAX_CHAR || i >= n) break;
      }
   }
   wsprintf(Ss, "%s\n\n%s:\n%s", Msg, (Lan+37)->msg, MsgErr);               //wsprintf(Ss, "%s\n\nСистемное сообщение:\n%s", Msg, MsgErr);
   MessageBox(MainWin, Ss, (Lan+35)->msg, MB_TOPMOST | MB_OK | MB_ICONSTOP);//MessageBox(MainWin, Ss, "Ошибка", MB_TOPMOST | MB_OK | MB_ICONSTOP);
#if defined OUT_TEST
   Add_SpecSpis(Ss);
#endif
   return -1;
}

//------------------------------------------------------------------------------

static int ReadSec0(BYTE *buff)                              //Чтение сектора
{
   DWORD nb;
   if(ReadFile(hDrive, buff, sSecB, &nb, NULL) == FALSE || nb != sSecB)
   {  DWORD Code = GetLastError();
      if(Code != ERROR_NOT_READY)
         ErrorSys1_New((Lan+9)->msg, Code);                  //Ошибка при чтении сектора
#if defined OUT_TEST
      else                                                   //Устройство не откликнулось
         Add_SpecSpis("The device is not ready. ");
#endif
      CloseFile(&hDrive);                                    //Закрыли входной файл
      return -1;
   }
   return 0;
}

//------------------------------------------------------------------------------

#if defined OUT_TEST
static void ReadAnySec(void)
{
   SEC_0 anySec;
   LONGLONG Poz = LONGLONG(sSecB) * 67;
   SetInFilePointer(Poz);
   ReadSec((BYTE *)&anySec);                                 //Чтение сектора
   Add_SpecSpis("Sector 67");
   View_HEX_Any((BYTE *)&anySec, 512);
   ReadSec((BYTE *)&anySec);                                 //Чтение сектора
   Add_SpecSpis("Sector 68");
   View_HEX_Any((BYTE *)&anySec, 512);
   Poz = LONGLONG(sSecB) * 70;
   SetInFilePointer(Poz);
   ReadSec((BYTE *)&anySec);                                 //Чтение сектора
   Add_SpecSpis("Sector 70");
   View_HEX_Any((BYTE *)&anySec, 512);
   ReadSec((BYTE *)&anySec);                                 //Чтение сектора
   Add_SpecSpis("Sector 71");
   View_HEX_Any((BYTE *)&anySec, 512);
}
#endif

//-------------------------------------------------------------------------------

int FindHDD_LG(void)                                         //Поиск диска из рекордера LG
{
   hDrive = NULL;
#if defined OUT_TEST
   Add_SpecSpis("Find HDD from LG");
#endif
#if defined HDD_LG
   for(int drive=HDD_LG; drive<32; drive++)                  //По всем возможным физическим устройствам начиная с указанного
#else
   for(int drive=0; drive<32; drive++)                       //По всем возможным физическим устройствам
#endif
   {  char driveName[256];                                   //Имя физического устройства
      sprintf(driveName, "\\\\.\\PhysicalDrive%d", drive);
#if defined OUT_TEST
      Add_SpecSpis(driveName);
#endif
      hDrive = CreateFile(driveName, GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
//                        FILE_FLAG_NO_BUFFERING |
                          0, NULL);
#if defined OUT_TEST
      if(hDrive == INVALID_HANDLE_VALUE)                     //Устройство не откликнулось
         Add_SpecSpis("Error Drive");
#endif
      if(hDrive == INVALID_HANDLE_VALUE) continue;           //Устройство не откликнулось
      SEC_0 Sec0;
      if(ReadSec0((BYTE*)&Sec0) < 0) continue;               //Чтение сектора, если сектор не читается, то на продолжение поиска
      SEC_63 Sec63, Sec64;
      LONGLONG Poz = LONGLONG(sSecB) * 63;
      if(SetInFilePointer(Poz) < 0)  //return -1;            //Изменение позиции указателя в файле
      {  CloseFile(&hDrive);  continue;  }                   //Почему-то не сработало
      if(ReadSec((BYTE *)&Sec63) < 0) continue;              //Чтение сектора
      if(strncmp(Sec63.Met, "LGEINC  ", 8) != 0 ||
         strncmp(Sec63.Name, "VOLUMELABE FAT32   ", 19) != 0)
#if defined OUT_TEST
      {  Add_SpecSpis("No HDD from LG");
#else
      {
#endif
         CloseFile(&hDrive);  continue;  }
#if defined OUT_TEST
      Add_SpecSpis("HDD from LG found");
      Add_SpecSpis("Sector 63");
      View_HEX_Any((BYTE *)&Sec63, 512);
#endif
      if(Sec63.nf != 1 || Sec63.F8 != 0xF8 || Sec63.SecPerCl != sClSec)
#if defined OUT_TEST
        Error1((Lan+19)->msg);                               //Error1("Найден жесткий диск только частично похожий на то, что он из рекордера LG.");
#else
        return Error1((Lan+19)->msg);                        //return Error1("Найден жесткий диск только частично похожий на то, что он из рекордера LG.");
#endif
      Start_SecFAT1 = 63 + Sec63.numRezSec;                  //Позиция начала таблицы FAT первого раздела
      Start_SecDir1 = Start_SecFAT1 + Sec63.nSecDir;         //Позиция начала первого каталога
      num_SecFAT1 = Sec63.numSecFAT;                         //Число секторов в FAT
      Size_FAT1 = num_SecFAT1 * sSecB;                       //Число байт в FAT
//??  maxZapFAT1 = sSecB / 4 * num_SecFAT1;                  //Максимальное число записей в FAT (по 4 байта)
      maxZapFAT1 = Sec63.numSecPart / sClSec;                //Максимальное число записей в FAT (по 4 байта)
      if(ReadSec((BYTE *)&Sec64) < 0) return -1;             //Чтение сектора
#if defined OUT_TEST
      Add_SpecSpis("Sector 64");
      View_HEX_Any((BYTE *)&Sec64, 512);
#endif
      if(strncmp(Sec64.Met, "LGEINC  ", 8) != 0 ||
         strncmp(Sec64.Name, "VOLUMELABE FAT32   ", 19) != 0)
      {  CloseFile(&hDrive);  continue;  }
      if(Sec64.nf != 1 || Sec64.F8 != 0xF8 || Sec64.SecPerCl != sClSec2)
        Error1((Lan+19)->msg);                               //return Error1("Найден жесткий диск только частично похожий на то, что он из рекордера LG.");
      Start_SecDir2 = Sec63.numSecPart + 63 + Sec64.numRezSec + Sec64.nSecDir; //Номер сектора начала каталога во втором разделе
      num_SecFAT2 = Sec64.numSecFAT;                         //Число секторов в FAT
      Size_FAT2 = num_SecFAT2 * sSecB;                       //Число байт в FAT
      maxZapFAT2 = sSecB / 4 * num_SecFAT2;                  //Максимальное число записей в FAT2 (по 4 байта)
      Start_SecFAT2 = Start_SecFAT1 + Sec0.numClP1;          //Позиция начала таблицы FAT2
//Для проверки наличия BIG LBA здесь можно прочитать сектор с номером 268435456+64 (0x10000000+64)
//и сравнить прочитанное по содержимому с сектором 64
#if defined OUT_TEST
      Add_SpecSpis("Sector 0");
      View_HEX_Any((BYTE *)&Sec0, 512);
      ReadAnySec();
#endif
      break;
   }
   if(hDrive == NULL || hDrive == INVALID_HANDLE_VALUE)
      return Error1((Lan+20)->msg);                          //return Error1("Не найден жесткий диск из рекордера LG.");
   return 0;
}

#endif

