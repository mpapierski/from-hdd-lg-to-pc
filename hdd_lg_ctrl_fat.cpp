
//                              17.02.2008
#ifndef __HDD_LG_CTRL_FAT__
#define __HDD_LG_CTRL_FAT__

#include "from_hdd_lg_to_pc.h"

#if defined TEST_FAT1                                        //Проверка FAT1

//============================ hdd_lg_ctrl_fat =================================
int  Ctrl_FAT1(void);                                        //Проверка структуры служебных записей
int  Vost_FAT1(void);                                        //Чистка FAT1 от потерянных кластеров
     DWORD *cTestFAT1;                                       //Копия таблицы FAT первого раздела
     DWORD sumClTestFAT1;                                    //Суммарное количество занятых кластеров при тестировании FAT1

static int cRead_Media_Dir(DWORD nCl);                       //Чтение и контроль одного каталога

//------------------------------------------------------------------------------

static int cCtrl_FAT_For_File(DWORD clSt, LONGLONG SizeF)    //Проверка соответствия длины файла и FAT
{
   DWORD nCl = clSt;                                         //Текущий номер кластера равен первому кластеру файла
   DWORD wSizeB = sCl_B;                                     //Число записываемых байт равно числу байт в кластере
   LONGLONG SizeToEnd = SizeF;                               //Число байт которые осталось в цепочке
   DWORD numCl = DWORD((SizeF + sCl_B - 1) / sCl_B);         //Число кластеров необходимое для размещения файла данного размера
   for(DWORD i=0; i<numCl; i++)                              //По числу кластеров которые нужны для данного файла
   {  if(nCl == 0x0FFFFFFF)
        return Error1((Lan+12)->msg);                        //return Error1("Неожиданно найден признак конца цепочки FAT.");
      if(*(FAT1 + nCl) == 0)
        return Error1((Lan+11)->msg);                        //return Error1("Обнаружено несоответствие значения FAT и ссылки на кластер файла.");
      if(*(cTestFAT1 + nCl) == 0)
         return Error1("Ссылка на свободный элемент FAT1.");
//      if(*(FAT + nCl) != 0x0FFFFFFF && nCl + 1 != *(FAT + nCl))
//         Error1("Есть фрагментация");
      *(cTestFAT1 + nCl) = 0;                                //Отметили использование данного элемента FAT
      sumClTestFAT1++;                                       //Суммарное количество занятых кластеров
      if(wSizeB > SizeToEnd) wSizeB = DWORD(SizeToEnd);      //Размер остатка записи меньше размера кластера
      SizeToEnd -= wSizeB;                                   //Число байт которые осталось записать
      nCl = *(FAT1 + nCl);                                   //Номер следующего кластера
      if(nCl > maxZapFAT1 && nCl != 0x0FFFFFFF)
        return Error1((Lan+13)->msg);                        //return Error2("Номер кластера в цепочке FAT", "превышает допустимое значение.");
      if(SizeToEnd == 0 && nCl != 0x0FFFFFFF)
        return Error1((Lan+14)->msg);                        //return Error2("Нужное число кластеров пройдено,", "а признак конца цепочки FAT не найден.");
   }
   return 0;
}

//------------------------------------------------------------------------------

static const DWORD Point1[] = { 0x00002E00, 0x20002E00 };

static int cCtrl_Dir(BYTE *buf)                              //Контроль содержимого каталога
{
   One_Str_Cat *Kat = (One_Str_Cat *)buf;
   int prPoint = 0;                                          //Признак наличия каталогов с точкой как признак правильности структуры
   for(int n=0; n<4*sClSec; n++)                             //В каждом секторе 4 записи
   {  if((Kat + n)->pf.type == 0) return 1;                  //Конец каталога
      DWORD NamePoint = *((DWORD*)&(Kat + n)->Name);
      if(NamePoint == *(Point1 + pr_tRec))                   //Это одна точка т.е. кластер текущиго каталога
      { if(n == 0) prPoint++; continue; }
      if(NamePoint == 0x2E002E00)                            //Это две точки т.е. кластер предыдущего каталога
      { if(n == 1) prPoint++;
        continue;
      }
      WORD NameDel = *((WORD*)&(Kat + n)->Name);
      if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0) continue;      //Это удаленное имя
      if((Kat + n)->pf.type == 32)                           //Если это файл, то проверяем по FAT
         cCtrl_FAT_For_File((Kat + n)->pf.ClSt, (Kat + n)->pf.SizeF); //Проверка соответствия длины файла и FAT
      if((Kat + n)->pf.type == 48)                           //Если это папка, то проверяем папку
         cRead_Media_Dir((Kat + n)->pf.ClSt);                //Чтение и контроль одного каталога
   }
   return 0;
}

//------------------------------------------------------------------------------

static int cRead_Dir_Media(BYTE *buf, DWORD nCl)             //Чтение и контроль одного каталога
{
   DWORD nSector = Start_SecDir1 + (nCl - 1) * sClSec;       //Номер сектора по номеру кластера
   if(ReadClast1_P(nSector, buf) < 0) return -1;             //Чтение кластера
   if(cCtrl_Dir(buf) < 0)                                    //Контроль содержимого каталога
      Error2((Lan+97)->msg, (Lan+98)->msg);                  //Error2("Найдены ошибки в структуре записей на диске LG.". "Дерево папок и каталогов содержит неполную информацию.");
   if(nCl != 1 && *(FAT1 + nCl) != 0x0FFFFFFF)
   {  Error1((Lan+17)->msg);                                 //Error1("Размер одного каталога превышает размер кластера.");
      Error2((Lan+97)->msg, (Lan+98)->msg);                  //Error2("Найдены ошибки в структуре записей на диске LG.". "Дерево папок и каталогов содержит неполную информацию.");
   }
   return 0;
}

//------------------------------------------------------------------------------

static int cRead_Media_Dir(DWORD nCl)                        //Чтение и контроль одного каталога
{
   BYTE *buf = (BYTE *)MyAllocMem(sCl_B);                    //Память под число кластеров в каталоге
   if(buf == NULL)  return -1;
   int ret = cRead_Dir_Media(buf, nCl);                      //Чтение и контроль одного каталога
   *(cTestFAT1 + nCl) = 0;                                   //Отметили использование данного элемента FAT
   sumClTestFAT1++;                                          //Суммарное количество занятых кластеров
   MyFreeMem(&(void*)buf);
   return ret;
}

//------------------------------------------------------------------------------

int  Ctrl_FAT1(void)                                         //Проверка структуры служебных записей
{
   cTestFAT1 = (DWORD *)MyAllocMem(Size_FAT1);               //Память под FAT
   if(cTestFAT1 == NULL)  return -1;
   CopyMemory(cTestFAT1, FAT1, Size_FAT1);                   //Скопировали содержимое FAT1
   sumClTestFAT1 = 0;                                        //Суммарное количество занятых кластеров
   int ret = cRead_Media_Dir(1);                             //Чтение и контроль одного каталога
   if(ret < 0) return ret;
   if(stCl_Media2 != 0)                                      //Кластер начала второй части папки MEDIA
     ret = cRead_Media_Dir(stCl_Media2);                     //Чтение и визуализация второй части папки MEDIA
   return ret;
}

//------------------------------------------------------------------------------
#if defined TEST_FAT1_VOST                                   //Проверка и восстановление FAT1 на потерянные кластеры
int Vost_FAT1(void)                                          //Чистка FAT1 от потерянных кластеров
{
   for(DWORD i=2; i<maxZapFAT1; i++)                         //По всем записям FAT
   {  if(*(cTestFAT1 + i) == 0) continue;                    //Изначально Пустое значение и счетчик не меняется
//    if(*(FAT_ + i) == 1) continue;                         //Признак, что данный элемент используется
      *(FAT1 + i) = 0;                                       //Пометили элемент FAT1 как свободный
   }
#if !defined EMULATOR_HDD                                    //Режим эмулятора
   DWORD nb;
   LONGLONG Poz = LONGLONG(sSecB) * Start_SecFAT1;
   if(SetInFilePointer(Poz) < 0) return -1;                  //Изменение позиции указателя в файле
   if(WriteFile(hDrive, FAT1, Size_FAT1, &nb, NULL) == FALSE || nb != Size_FAT1)
      return ErrorSys1((Lan+176)->msg);                      //"Ошибка при записи FAT."
#endif
   return 0;
}
#endif

#endif

#endif


