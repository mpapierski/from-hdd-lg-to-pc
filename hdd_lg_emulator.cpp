
//                                    11.08.2008
#ifndef __HDD_LG_EMULATOR__
#define __HDD_LG_EMULATOR__

#include "from_hdd_lg_to_pc.h"

#if defined EMULATOR_HDD

//============================ hdd_lg_emulator =================================
int ReadClastEmul(DWORDLONG Poz, BYTE *buff);                //Чтение кластера эмулятором
int ReadClast2Emul(DWORDLONG Poz, BYTE *buff);               //Чтение кластера эмулятором
int ReadFATEmul(DWORDLONG Poz);                              //Чтение FAT эмулятором
int ReadFAT2Emul(DWORDLONG Poz);                             //Чтение FAT эмулятором
int Read_Sec_Emul(int Num, BYTE* Sec);                       //Чтение сектора по номеру эмулятором
int FindHDD_LG_Emul(void);                                   //Поиск диска из рекордера LG эмулятором
    char *inBuf;
    char NameFDump[260];                                     //Имя файла с дампом

static DWORD inSize;
static char *Adr, *AdrEnd;

//------------------------------------------------------------------------------

static int OpenInFile(char *NameF, HANDLE *inFile)                           //Открытие файла
{
   *inFile = CreateFile(NameF,                                //Адрес строки с именем файла
                       GENERIC_READ,                         //Способ доступа к файлу
                       FILE_SHARE_READ,                      //Посторонний процес может только читать файл
                       NULL,                                 //Адрес структуры с параметрами защиты ядра
                       OPEN_EXISTING,                        //Тип открытия файла
                       FILE_ATTRIBUTE_NORMAL,                //Атрибуты файла
                       NULL);                                //Используется для временных файлов
   if(*inFile == INVALID_HANDLE_VALUE)
     return ErrorSys2("Ошибка при открытии файла", NameF);
   inSize = GetFileSize(*inFile, NULL);
   return 0;
}

//------------------------------------------------------------------------------

static int GetName_InFile(char *NameF)                       //Ввод имени обрабатываемого файла
{
   OPENFILENAME ofn;
   char DirName[260];                                        //Начальный путь для поиска файлов
   char FileTitle[256];                                      //Заголовок окна
   char Filter[256];                                         //Фильтры поиска
   char NameFile[260];

   strcpy(NameFile, NameF);                                  //Начальный путь для поиска файлов
   *DirName = 0;                                             //На случай если имени небыло
   if(*NameFile != 0)                                        //Имя уже было
   {  char *A;
      lstrcpy(DirName, NameFile);                            //Начальный путь для поиска файлов
      //Наверно логично создать полное имя
      A = strrchr(DirName, '\\');                            //Нашли последний разделитель
      if(A != NULL) *A = 0;
      else *DirName = 0;                                     //Нет имени файла
   }
   *NameFile = 0;
   lstrcpy(Filter, "Файлы дампа|*.txt|Все файлы (*.*)|*.*|");
   for(int i=0; Filter[i]!=0; i++)                           //Заменили разделитель 0
     if(Filter[i] == '|') Filter[i] = 0;
   ZeroMemory(&ofn, sizeof(OPENFILENAME));                   //Заполнили нулями
   ofn.lStructSize = sizeof(OPENFILENAME);                   //Длина структуры
   ofn.hwndOwner = MainWin;                                  //Владелец диалога
   ofn.hInstance = MainInst;                                 //Идентификатор программы владеющая диалогом
   ofn.lpstrFilter = Filter;                                 //Типы просматриваемых файлов
// ofn.lpstrCustomFilter                                     //Специальные фильтры
// ofn.nMaxCustFilter                                        //Длина специального фильтра
   ofn.nFilterIndex = 1;                                     //Индекс для работы с фильтрами
   ofn.lpstrFile = NameFile;                                 //Имя файла в случае успеха
   ofn.n_MAX_FNAME = sizeof(NameFile);                          //Длина поля имени файла
   ofn.lpstrFileTitle = FileTitle;                           //Маршрут и имя файла в случае успеха
   ofn.n_MAX_FNAMETitle = sizeof(FileTitle);                    //Длина поля
   ofn.lpstrInitialDir = DirName;                            //Начальный каталог файлов
   ofn.lpstrTitle = "Укажите имя файла дампа";
   ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
// ofn.Flags |= OFN_ENABLESIZING;                            //Можно менять размер окна диалога
// ofn.nFileOffset;                                          //Смещение в внутри lpstrFile
// ofn.nFileExtension;                                       //Смещение в внутри lpstrFile
   ofn.lpstrDefExt = "txt";                                  //Расширение по умолчанию
// ofn.lCustData;                                            //Для функции обработчиков
//   ofn.lpfnHook = ModDlg.fnHook;                           //Функция обработки дополнительного диалога
//   ofn.lpTemplateName = ModDlg.DopDlgName;                 //Дополнительный диалог который суммируется с основным
//   if(NumWinNT != 0)                                       //0-Система Win95/98 или номер NT
//     ofn.FlagsEx = OFN_EX_NOPLACESBAR;                     //Убирает иконку слева
   if(GetOpenFileName(&ofn) == FALSE) return -1;
   if(*NameFile == 0) return -1;
   strcpy(NameF, NameFile);                                  //Переслали имя файла
   return 0;
}

//------------------------------------------------------------------------------

static int FindStr(char *msg)                                //Поиск строки начинающейся со строк msg
{
   int n = lstrlen(msg);
   Adr = inBuf;
   for(;;)                                                   //До конца файла
   {  char *NewAdr = strchr(Adr, '\0');
      if(NewAdr > AdrEnd || NewAdr == NULL) return -1;       //Данные кончились и Больше нет ни одной полной строки
      if(strncmp(Adr, msg, n) == 0) return 0;
      Adr = NewAdr + 1;
   }
}

//------------------------------------------------------------------------------

static int Read_Dump(BYTE *buf, DWORD size)
{
   int num, ahex;
   ZeroMemory(buf, size);                                    //Обнулили буфер на случай группового нуля
   char *NewAdr = strchr(Adr, '\0');
   if(NewAdr > AdrEnd || NewAdr == NULL) return -1;          //Данные кончились и Больше нет ни одной полной строки
   Adr = NewAdr + 1;
   DWORD nn = 0;
   for(int n=1, hex=0; ; n++,hex+=128)                       //До конца файла
   {  NewAdr = strchr(Adr, '\0');
      if(NewAdr > AdrEnd || NewAdr == NULL) return -1;       //Данные кончились и Больше нет ни одной полной строки
      if(lstrcmp(Adr, "All bytes == 0\r") == 0) return 0;    //Абсолютно везде нули
      sscanf(Adr, "%d: %x:", &num, &ahex);
      if(n != num && ahex != hex)
        return Error2("Нарушение порядка следования строк.", "Эмулятор.");
      Adr += 14;
      for(int j=1; j<=128; j++)
      {  if(strncmp(Adr, "(0)", 3) == 0) return 0;           //Далее все нули
         DWORD OneB;
         sscanf(Adr, "%x", &OneB);
         *(buf + nn) = BYTE(OneB);
         nn++;
         if(nn > size)
           return Error2("Переполнение буфера.", "Эмулятор.");
         Adr += 3;
         if((j % 8) == 0) Adr++;
      }
      Adr = NewAdr + 1;
      if(nn == size) return 0;
   }
}

//------------------------------------------------------------------------------

static int Load_Dump(void)                                   //Загрузка дампа
{
   HANDLE inFile;
   DWORD nb;
   if(GetName_InFile(NameFDump) < 0) return -1;              //Ввод имени обрабатываемого файла
   if(OpenInFile(NameFDump, &inFile) < 0) return -1;         //Открытие входного файла
   inBuf = (char *)MyAllocMem(inSize);
   if(inBuf == NULL) return -1;
   if(ReadFile(inFile, inBuf, inSize, &nb, NULL) == FALSE || nb != inSize)
      return ErrorSys2("Ошибка при чтении исходного файла", NameFDump);
   CloseFile(&inFile);                                       //Закрыли входной файл
   AdrEnd = inBuf + inSize;
   Adr = inBuf;
   for(;;)                                                   //До конца файла
   {  char *NewAdr = strchr(Adr, '\n');
      if(NewAdr > AdrEnd || NewAdr == NULL) break;           //Данные кончились и Больше нет ни одной полной строки
      *NewAdr = 0;                                           //Ограничили строку
      Adr = NewAdr + 1;
   }
   return 0;
}

//------------------------------------------------------------------------------

int ReadClastEmul(DWORDLONG Poz, BYTE *buff)                 //Чтение кластера эмулятором
{
   DWORD nSector = DWORD(Poz / sSecB);
   DWORD nClast = (nSector - Start_SecDir1) / sClSec + 1;
   char Ss[300];
   wsprintf(Ss, "Load Dir (claster %d, sector %d)", nClast, nSector);
   if(FindStr(Ss) < 0)
//     return Error2("Не найдена строка", Ss);
   {  Error3("Не найдена строка", Ss, "Эмулятор.");  return 0;  }
   if(Read_Dump(buff, sCl_B) < 0) return -1;
   return 0;
}

//------------------------------------------------------------------------------

int ReadClast2Emul(DWORDLONG Poz, BYTE *buff)                //Чтение кластера эмулятором
{
   DWORD nSector = DWORD(Poz / sSecB);
   DWORD nClast = (nSector - Start_SecDir2) / sClSec2 + 1;
   char Ss[300];
   wsprintf(Ss, "Load Dir (claster2 %d, sector %d)", nClast, nSector);
   if(FindStr(Ss) < 0)
//     return Error2("Не найдена строка", Ss);
   { Error3("Не найдена строка", Ss, "Эмулятор.");  return 0; }
   if(Read_Dump(buff, sCl2_B) < 0) return -1;
   return 0;
}

//------------------------------------------------------------------------------

#pragma argsused
int ReadFATEmul(DWORDLONG Poz)                               //Чтение FAT эмулятором
{
   if(FindStr("Load FAT 1") < 0) return -1;
   if(Read_Dump((BYTE*)FAT1, Size_FAT1) < 0) return -1;
   return 0;
}

//------------------------------------------------------------------------------

#pragma argsused
int ReadFAT2Emul(DWORDLONG Poz)                               //Чтение FAT эмулятором
{
   if(FindStr("Load FAT 2") < 0) return -1;
   if(Read_Dump((BYTE*)FAT2, Size_FAT2) < 0) return -1;
   return 0;
}

//-------------------------------------------------------------------------------

int Read_Sec_Emul(int Num, BYTE* Sec)                        //Чтение сектора по номеру эмулятором
{
   char Ss[100];
   sprintf (Ss, "Sector %d", Num);
   if(FindStr(Ss) < 0) return -1;
   if(Read_Dump(Sec, 512) < 0) return -1;
   return 0;
}

//-------------------------------------------------------------------------------

int FindHDD_LG_Emul(void)                                    //Поиск диска из рекордера LG эмулятором
{
#if !defined EMULATOR_HDD_AND_COPY                           //Режим эмулятора с эмуляцией копирования
   EnableWindow(hCopy, FALSE);
#endif
   if(Load_Dump() < 0) return -1;                            //Загрузка дампа

   SEC_0 Sec0;
// if(FindStr("Sector 0") < 0) return -1;
// if(Read_Dump((BYTE*)&Sec0, 512) < 0) return -1;
   if(Read_Sec_Emul(0, (BYTE*)&Sec0) < 0) return -1;         //Чтение сектора по номеру эмулятором
#if defined OUT_TEST
   Add_SpecSpis("Find HDD from LG emulator");
#endif
   SEC_63 Sec63;
   if(FindStr("Sector 63") < 0) return -1;
   if(Read_Dump((BYTE*)&Sec63, 512) < 0) return -1;
   if(strncmp(Sec63.Met, "LGEINC  ", 8) != 0 ||
      strncmp(Sec63.Name, "VOLUMELABE FAT32   ", 19) != 0)
     return Error2("Содержимое сектора 63 не соответствует требованиям.", "Эмулятор.");
#if defined OUT_TEST
   Add_SpecSpis("HDD from LG found");
   Add_SpecSpis("Sector 63");
   View_HEX_Any((BYTE *)&Sec63, 512);
#endif
   if(Sec63.nf != 1 || Sec63.F8 != 0xF8 || Sec63.SecPerCl != sClSec)
     return Error1((Lan+19)->msg);                           //return Error1("Найден жесткий диск только частично похожий на то, что он из рекордера LG.");

   Start_SecFAT1 = 63 + Sec63.numRezSec;                     //Позиция начала таблицы FAT
   Start_SecDir1 = Start_SecFAT1 + Sec63.nSecDir;            //Позиция начала первого каталога
   num_SecFAT1 = Sec63.numSecFAT;                            //Число секторов в FAT
   Size_FAT1 = num_SecFAT1 * sSecB;                          //Число байт в FAT
// maxZapFAT1 = sSecB / 4 * num_SecFAT1;                     //Число записей в FAT (по 4 байта)
   maxZapFAT1 = Sec63.numSecPart / sClSec;                   //Максимальное число записей в FAT (по 4 байта)
   SEC_63 Sec64;
   if(FindStr("Sector 64") < 0) return -1;
   if(Read_Dump((BYTE*)&Sec64, 512) < 0) return -1;
#if defined OUT_TEST
   Add_SpecSpis("Sector 64");
   View_HEX_Any((BYTE *)&Sec64, 512);
#endif
   if(strncmp(Sec64.Met, "LGEINC  ", 8) != 0 ||
      strncmp(Sec64.Name, "VOLUMELABE FAT32   ", 19) != 0)
     return Error2("Содержимое сектора 64 не соответствует требованиям.", "Эмулятор.");
   Start_SecDir2 = Sec63.numSecPart + 63 + Sec64.numRezSec + Sec64.nSecDir; //Номер сектора начала каталога во втором разделе
   num_SecFAT2 = Sec64.numSecFAT;                            //Число секторов в FAT
   Size_FAT2 = num_SecFAT2 * sSecB;                          //Число байт в FAT
   maxZapFAT2 = sSecB / 4 * num_SecFAT2;                     //Число записей в FAT (по 4 байта)
   Start_SecFAT2 = Start_SecFAT1 + Sec0.numClP1;             //Позиция начала таблицы FAT2
   return 0;
}

#endif

#endif

