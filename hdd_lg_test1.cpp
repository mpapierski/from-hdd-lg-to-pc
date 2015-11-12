
//                               10.07.2007
#ifndef __HDD_LG_TEST1__
#define __HDD_LG_TEST1__

#include "from_hdd_lg_to_pc.h"

#if defined TEST_LG1

//============================ hdd_lg_test1 ====================================
int  Read_Test1(void);                                       //Тестовое чтение и вывод

static DWORD numClPointMEDIA;                                //Номер кластера с папкой '.' в папке MEDIA
static int Read_One_Dir_T(DWORD nClast, int pr);             //Чтение и визуализация одного каталога

//------------------------------------------------------------------------------

static int Ctrl_FAT_For_File_T(DWORD clSt, LONGLONG SizeF)   //Проверка соответствия длины файла и FAT
{
   DWORD nCl = clSt;                                         //Текущий номер кластера равен первому кластеру файла
   DWORD wSizeB = sCl_B;                                     //Число записываемых байт равно числу байт в кластере
   LONGLONG SizeToEnd = SizeF;                               //Число байт которые осталось в цепочке
   DWORD numCl = DWORD((SizeF + sCl_B - 1) / sCl_B);         //Число кластеров необходимое для размещения файла данного размера
   for(DWORD i=0; i<numCl; i++)                              //По числу кластеров которые нужны для данного файла
   {  if(*(FAT1 + nCl) == 0)
      {  Add_Spis(".....Обнаружено несоответствие значения FAT и ссылки на кластер файла."); break; }
      if(nCl == 0x0FFFFFFF)
      {  Add_Spis(".....Неожиданно найден признак конца цепочки FAT."); break; }
      if(wSizeB > SizeToEnd) wSizeB = DWORD(SizeToEnd);      //Размер остатка записи меньше размера кластера
      SizeToEnd -= wSizeB;                                   //Число байт которые осталось записать
      nCl = *(FAT1 + nCl);                                    //Номер следующего кластера
      if(nCl > maxZapFAT1 && nCl != 0x0FFFFFFF)
      {  Add_Spis(".....Номер кластера в цепочке FAT превышает допустимое значение."); break; }
      if(SizeToEnd == 0 && nCl != 0x0FFFFFFF)
     {  Add_Spis(".....Нужное число кластеров пройдено, а признак конца цепочки FAT не найден."); break; }
   }
   return 0;
}

//------------------------------------------------------------------------------

static void View_Str_Dir_Hex(int n, One_Str_Cat *Kat)        //Вывод строки каталога в шеснадцатиричном виде
{
   char *Adr, Ss[300];

   sprintf(Ss, "....Строка каталога %d..............................................", n+1);
   Add_Spis(Ss);
   BYTE *bb = (BYTE*)(Kat + n);
   for(int i=0; i<128; i+=32)                                //Вывод шеснадцатиричного дампа одной строки каталога
   {   Adr = Ss;
       for(int j=i; j<i+32; j++)
       {   Adr += sprintf(Adr, " %02X", bb[j]);
           if((j+1) % 8 == 0)
             Adr += sprintf(Adr, "   ");
       }
       *(Adr + 1) = 0;
       Add_Spis(Ss);                                         //Добавление строки в список
   }
}

//------------------------------------------------------------------------------

static const DWORD Point1[] = { 0x00002E00, 0x20002E00 };    //Код точки (у старейшая серия вместо нуля - пробелы)

static int View_Dir_T(BYTE *buff, int pr)                    //Тестовый Вывод содержимого каталога
{
   char *Adr, Ss[300];

   One_Str_Cat *Kat = (One_Str_Cat *)buff;
   for(int n=0; n<4*sClSec; n++)                             //В каждом секторе 4 записи
   {  if((Kat + n)->pf.type == 0) return 1;                  //Конец каталога
//    if((Kat + n)->Name[0] == 0 && (Kat + n)->Name[1] == 0) return 1; //Конец каталога

//      View_Str_Dir_Hex(n, Kat);                            //Вывод строки каталога в шеснадцатиричном виде

      DWORD NamePoint = *((DWORD*)&(Kat + n)->Name);
      int prPoint1 = (NamePoint == *(Point1 + pr_tRec)) ? 1 : 0;//Это одна точка т.е. кластер текущиго каталога
      int prPoint2 = (NamePoint == 0x2E002E00) ? 1 : 0;      //Это две точки т.е. кластер предыдущего каталога
      WORD NameDel = *((WORD*)&(Kat + n)->Name);
//    if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0) continue; //Это удаленное имя (первая буква имени а и номер кластера начала ноль)
      if((Kat + n)->pf.type != 32 && (Kat + n)->pf.type != 48)
         return Error1("Неизвестный тип записи в каталоге");
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
//    if(strcmp(ext, "idx") == 0) continue;

      FillMemory(Ss, sizeof(Ss), '_');
      if((Kat + n)->pf.type == 32)                           //Это имя файла
      {  if(*ext != 0) sprintf(Ss+2*pr, "%03d. %s.%s", n+1, nam, ext);
         else strcpy(Ss+2*pr, nam);
      }
      if((Kat + n)->pf.type == 48)                           //Это имя папки
      {  if(*ext != 0) sprintf(Ss+2*pr, "%03d.  [%s.%s]", n+1, nam, ext);
         else sprintf(Ss+2*pr, "%03d. [%s]", n+1, nam);
         if((Kat + n)->pf.SizeF != 0)
            return Error1("Строка каталога опознанная как папка имеет размер в байтах.");
      }
      int l = lstrlen(Ss);
      for(int i=l; i<50; i++) Ss[i] = ' ';
      Adr = Ss + 50;
      if((Kat + n)->pf.type == 32)
           Adr += sprintf(Adr, "%18.0lf", double((Kat + n)->pf.SizeF));
      else Adr += sprintf(Adr, "                  ");
      Adr += sprintf(Adr, "%10d  ", (Kat + n)->pf.ClSt);
      Adr += sprintf(Adr, "%02d-%02d-%04d  %02d:%02d:%02d  ", (Kat + n)->pf.Day, (Kat + n)->pf.Mon, (Kat + n)->pf.Year,
                                                     (Kat + n)->pf.Hour, (Kat + n)->pf.Min, (Kat + n)->pf.Sec);
//    BYTE *b = (BYTE*)&(Kat + n)->pf.LL1;
//    Adr += sprintf(Adr, "| %02X%02X%02X%02X %02X%02X%02X%02X ||", b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
      Adr += sprintf(Adr, "%16.0lf |", double((Kat + n)->pf.lL1));
      if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0)       //Это удаленное имя (первая буква имени а и номер кластера начала ноль)
         Adr += sprintf(Adr, "  УДАЛЕНО");
      Add_Spis(Ss);                                          //Добавление строки в список
      if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0) continue; //Это удаленное имя (первая буква имени а и номер кластера начала ноль)
      if((Kat + n)->pf.type == 48)                           //Это имя папки
      {  if(pr == 0 && prPoint1 == 1) numClPointMEDIA = (Kat + n)->pf.ClSt; //Номер кластера с папкой '.' в папке MEDIA
         if(pr == 0) continue;
         if(prPoint1 == 1) continue;                         //Кататог '.'  меня не интересует
         if(prPoint2 == 1) continue;                         //Кататог '..'  меня не интересует
         if(Read_One_Dir_T((Kat + n)->pf.ClSt, pr+1) < 0) return -1;//Чтение и визуализация одного каталога
      }
      if((Kat + n)->pf.type == 32)                           //Это имя файла
        Ctrl_FAT_For_File_T((Kat + n)->pf.ClSt, (Kat + n)->pf.SizeF); //Проверка соответствия длины файла и FAT
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir_T_(BYTE *buff, DWORD nClast, int pr) //Тестовое Чтение и визуализация одного каталога
{
   char Ss[300];
   DWORD nSector = Start_SecDir1 + (nClast - 1) * sClSec;    //Позиция первого главного каталога MEDIA
   sprintf(Ss, " ********** Кластер %d.  Начальный сектор %d  (0x%08X) **********", nClast, nSector, nSector);
   Add_Spis(Ss);                                             //Добавление строки в список
   if(ReadClast1_P(nSector, buff) < 0) return -1;            //Чтение кластера
// if(View_Dir_T(buff, pr) < 0) return -1;                   //Вывод содержимого каталога
   View_Dir_T(buff, pr);                                     //Вывод содержимого каталога
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir_T(DWORD nClast, int pr)              //Чтение и визуализация одного каталога
{
   if(nClast == 0 || nClast >= maxZapFAT1)
   {  Add_Spis((Lan+100)->msg);                              //Добавление строки в список
      return Error1((Lan+100)->msg);                         //return Error1("Недопустимый номер кластера каталога.");
   }
   if(nClast != 1 && *(FAT1 + nClast) == 0)                   //
      return Error1((Lan+18)->msg);                          //return Error1("Обнаружено несоответствие значения FAT и ссылки на кластер каталога.");
   if(nClast != 1 && *(FAT1 + nClast) != 0x0FFFFFFF)
      return Error1((Lan+17)->msg);                          //return Error1("Размер одного каталога превышает размер кластера.");
   BYTE *buff = (BYTE *)MyAllocMem(sCl_B);                   //Память под один кластер
   if(buff == NULL)  return -1;
   int ret = Read_One_Dir_T_(buff, nClast, pr);              //Чтение и визуализация одного каталога
   MyFreeMem(&(void*)buff);
   return ret;
}

//------------------------------------------------------------------------------

static int Read_Dir_T(void)                                  //Тестовое xтение каталога
{
   Read_One_Dir_T(1, 0);                                     //Чтение и визуализация каталога MEDIA
   if(stCl_Media2 != 0)                                      //Кластер начала второй части папки MEDIA
   { Add_Spis("Второй кластер каталога папки MEDIA");
     Read_One_Dir_T(stCl_Media2, 0);                         //Чтение и визуализация второй части папки MEDIA
   }
   Read_One_Dir_T(2, 1);                                     //Чтение и визуализация остальных каталогов
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir_T_Sec_(BYTE *buff, DWORD nSec, int pr) //Тестовое Чтение и визуализация одного каталога
{
   char Ss[300];
   double nClast = (double(nSec) - Start_SecDir1) / sClSec + 1;
   sprintf(Ss, " ********** Кластер %.2lf.  Начальный сектор %d  (0x%08X) **********", nClast, nSec, nSec);
   Add_Spis(Ss);                                             //Добавление строки в список
   if(ReadClast1_P(nSec, buff) < 0) return -1;
   if(View_Dir_T(buff, pr) < 0) return -1;                   //Вывод содержимого каталога
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir_T_Sec(DWORD nSec, int pr)            //Чтение и визуализация одного каталога
{
   BYTE *buff = (BYTE *)MyAllocMem(sCl_B);                   //Память под один кластер
   if(buff == NULL)  return -1;
   int ret = Read_One_Dir_T_Sec_(buff, nSec, pr);            //Чтение и визуализация одного каталога
   MyFreeMem(&(void*)buff);
   return ret;
}

//------------------------------------------------------------------------------

#if defined TEST_LG1_AND_TS                                  //Первый раздел в служебном виде и буфер TimeShift
#define NDW_S 16                                             //Число значение в строке
static int View_Folder_Point0(BYTE *buff)                    //Вывод содержимого каталога [.] из папки MEDIA
{
   char Ss[1024];

   Add_Spis("Cодержимого каталога [.] из папки MEDIA");
   int n = sCl_B / sizeof(DWORD);                            //Число DWORD в кластера
   DWORD *DW = (DWORD*)buff;                                 //Кластер как значения DWORD
   InitProgressBar1(n/NDW_S, "Формирование массива");
   for(int i=0; i<n; i+=NDW_S)                               //По числу строк с NDW_S значениями
   {  char *Adr = Ss;
      for(int j=0; j<NDW_S; j++)
      { //SWAP32(DW+i+j);
        Adr += sprintf(Adr, "%12d", *(DW+i+j));
//      if(j != 2) continue;
        WORD *W = (WORD*)(DW+i+j);
        Adr += sprintf(Adr, "(%6d %6d) ", *W, *(W+1));
        Adr += sprintf(Adr, "[%8X] ", *(DW+i+j));
      }
      *Adr = 0;
      Add_Spis(Ss);                                          //Добавление строки в список
      if(ProgressBar1(sSecB) < 0)                            //Оператор нажал кнопку Прервать
      {  Close_ProgressBar1();  return -1;   }
   }
   Close_ProgressBar1();
   return 0;
}
#endif

//------------------------------------------------------------------------------

#if defined TEST_LG1_AND_TS                                  //Первый раздел в служебном виде и буфер TimeShift
#define NB_S 48                                              //Число значение в строке
static int View_Folder_Point1(BYTE *buff)                    //Вывод содержимого каталога [.] из папки MEDIA в десятичном виде
{
   char Ss[1024];

   Add_Spis("Cодержимого каталога [.] из папки MEDIA");
   int n = sCl_B;
   BYTE *B = (BYTE*)buff;                                    //Кластер как значения DWORD
   InitProgressBar1(n/NB_S, "Формирование массива");
   for(int i=0; i<n; i+=NB_S)                                //По числу строк с NB_S значениями
   {  char *Adr = Ss;
      for(int j=0; j<NB_S; j++)
        Adr += sprintf(Adr, "%3d ", *(B+i+j));
      *Adr = 0;
      Add_Spis(Ss);                                          //Добавление строки в список
      if(ProgressBar1(sSecB) < 0)                            //Оператор нажал кнопку Прервать
      {  Close_ProgressBar1();  return -1;   }
   }
   Close_ProgressBar1();
   return 0;
}
#endif

//-------------------------------------------------------------------------------

#if defined TEST_LG1_AND_TS                                  //Первый раздел в служебном виде и буфер TimeShift
static void View_Cl_Hex(BYTE *buff, int pr)                  //Вывод содержимого кластера в шеснадцатиричном виде
{
   BYTE *t_buff = buff;
   InitProgressBar1(sClSec, "Формирование дампа");
   for(int i=0; i<sClSec; i++)                               //По числу секторов в кластере
   {  View_Sec_Hex(t_buff, pr);                              //Вывод содержимого сектора
      t_buff += sSecB;                                       //Увеличили адрес вывода на размер сектора
      Add_Spis("----------------------------------------------------------------------------");
      if(ProgressBar1(sSecB) < 0)                            //Оператор нажал кнопку Прервать
      {  Close_ProgressBar1();  return;   }
   }
   Close_ProgressBar1();
}
#endif

//------------------------------------------------------------------------------

#if defined TEST_LG1_AND_TS                                  //Первый раздел в служебном виде и буфер TimeShift
static int ViewSpecCl(DWORD nCl, int pr)                     //Визуализация кластера
{
   BYTE buff[sCl_B];
   char Ss[300];

   DWORD nSector = (nCl - 1) * sClSec + Start_SecDir1;
   sprintf(Ss, " ********** Кластер %d.  Начальный сектор %d  (0x%08X) **********", nCl, nSector, nSector);
   Add_Spis(Ss);                                             //Добавление строки в список
   if(ReadClast1_P(nSector, buff) < 0) return -1;            //Чтение кластера
   if(pr == 1)
     View_Cl_Hex(buff, pr);                                  //Вывод содержимого кластера  в шеснадцатиричном виде
   if(pr == 2)
     View_Folder_Point0(buff);                               //Вывод содержимого каталога [.] из папки MEDIA
   if(pr == 3)
     View_Folder_Point1(buff);                               //Вывод содержимого каталога [.] из папки MEDIA в десятичном виде
   return 0;
}
#endif

//------------------------------------------------------------------------------

int  Read_Test1(void)                                        //Тестовое чтение и вывод
{
   if(Read_Dir_T() < 0) return -1;                           //Тестовое чтение каталога
#if defined TEST_LG1_AND_TS                                  //Первый раздел в служебном виде и буфер TimeShift
// ViewSpecCl(numClPointMEDIA, 1);                           //Вывод содержимого каталога [.] из папки MEDIA в шеснадцатиричном виде
   ViewSpecCl(numClPointMEDIA, 2);                           //Вывод содержимого каталога [.] из папки MEDIA
// ViewSpecCl(numClPointMEDIA, 3);                           //Вывод содержимого каталога [.] из папки MEDIA в десятичном виде
#endif
   return 0;
}

#endif

#endif

