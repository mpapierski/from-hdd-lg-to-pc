
//                                25.01.2009
#ifndef __HDD_LG_WRITE_IO__
#define __HDD_LG_WRITE_IO__

#include "from_hdd_lg_to_pc.h"

#if defined WRITE_YES                                        //Режим записи разрешен

//============================ hdd_lg_write_io =================================
int  GetSpNameInFile(char *Spis, int N, int *sm);            //Ввод списка имен для записи
int  OpenInFilePC(char *NameF);                              //Открыли входной файл для чтения с компьютера
int  WriteClast1P(DWORD nClast, BYTE *buff);                 //Запись кластера
int  Save_FAT1(void);                                        //Сохранение обновленной FAT первого раздела
void Ansi_To_Unicode(char *inANSI, WCHAR *outUNI, int maxN); //Преобразование строки символов в Unicode
int  Load_Dir(void);                                         //Загрузка кластера каталога
int  Change_Dir_For_File(char *NameF);                       //Изменение каталога (внесение нового файла)
int  Change_Dir_For_Folder(char *NameF);                     //Изменение каталога (внесение новой папки)
int  Save_Dir(void);                                         //Сохранение кластера каталога
     DWORD *c_FAT1;                                          //Копия таблицы FAT первого раздела
     HANDLE inFile;
     ULARGE_INTEGER Size_inF;                                //Размер файла м.б. > 4 ГБ
     int nCl_1;                                              //Номер кластера начала файла или новой папки
     BYTE bufDir[sCl_B];                                     //Память под один кластер

static FILETIME Time_inF;                                    //Дата и время входного файла
static int ns_Dir;                                           //Номер свободной строки каталога
static One_Str_Cat *s_Kat;                                   //Каталог

//------------------------------------------------------------------------------

int GetSpNameInFile(char *Spis, int N, int *sm)              //Ввод списка имен для записи
{
   OPENFILENAME ofn;
   char DirName[260];                                        //Начальный путь для поиска файлов
   char FileTitle[256];                                      //Заголовок окна
   char Filter[256];                                         //Фильтры поиска

   *DirName = 0;                                             //На случай если имени небыло
   lstrcpy(Filter, (Lan+43)->msg);
   switch(prFolder)
   {  case 0: lstrcat(Filter, " mp3, wma|*.mp3;*.wma;|");  break;
      case 1: lstrcat(Filter, " jpg|*.jpg|");  break;
      case 2: //lstrcat(Filter, " avi|*.avi|");  break;
              lstrcat(Filter, " avi, srt, smi, sub, txt, mpg, divx|*.avi;*.srt;*.smi;*.sub;*.txt;*.mpg;*.divx;"); break;
   }
   for(int i=0; Filter[i]!='\0'; i++)                        //Заменили разделитель 0
     if(Filter[i] == '|') Filter[i] = '\0';
   ZeroMemory(&ofn, sizeof(OPENFILENAME));                   //Заполнили нулями
   ofn.lStructSize = sizeof(OPENFILENAME);                   //Длина структуры
   ofn.hwndOwner = MainWin;                                  //Владелец диалога
   ofn.hInstance = MainInst;                                 //Идентификатор программы владеющая диалогом
   ofn.lpstrFilter = Filter;                                 //Типы просматриваемых файлов
// ofn.lpstrCustomFilter                                     //Специальные фильтры
// ofn.nMaxCustFilter                                        //Длина специального фильтра
   ofn.nFilterIndex = 1;//prFolder + 1;                      //Индекс для работы с фильтрами
   ofn.lpstrFile = Spis;                                     //Имя файла в случае успеха
   ofn.nMaxFile = N;                                         //Длина поля имени файла
   ofn.lpstrFileTitle = FileTitle;                           //Маршрут и имя файла в случае успеха
   ofn.nMaxFileTitle = sizeof(FileTitle);                    //Длина поля
   ofn.lpstrInitialDir = DirName;                            //Начальный каталог файлов
   ofn.lpstrTitle = (Lan+174)->msg;
   ofn.Flags = OFN_ALLOWMULTISELECT |
               OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
   if(GetOpenFileName(&ofn) == FALSE) return -1;
   if(*Spis == 0) return -1;
   *sm = ofn.nFileOffset;                                    //Смещение к имени файла
   return 0;
}
//------------------------------------------------------------------------------

int OpenInFilePC(char *NameF)                                  //Открыли входной файл для чтения с компьютера
{
   Size_inF.QuadPart = 0;
   inFile = CreateFile(NameF, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(inFile == INVALID_HANDLE_VALUE)
      return ErrorSys2(NameF, (Lan+84)->msg);                //"Ошибка при открытии файла для чтения.");
   Size_inF.u.LowPart = GetFileSize(inFile, &Size_inF.u.HighPart);
   if(Size_inF.u.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
      return ErrorSys2(NameF, (Lan+85)->msg);                //"Ошибка при запросе размера файла."
   if(Size_inF.QuadPart == 0)
      return Error2(NameF, (Lan+86)->msg);                   //"Недопустимо малый размер файла."
   GetFileTime(inFile, NULL, NULL, &Time_inF);               //Дата последней записи
   return 0;
}

//------------------------------------------------------------------------------

#if !defined EMULATOR_HDD                                    //НЕТ Режима эмулятора
int WriteClast1P(DWORD nClast, BYTE *buff)                   //Запись кластера
{
   DWORD nb;
   if(MaxClast < nClast) MaxClast = nClast;                  //Номер самого старшего кластера использованного для записи
   DWORD nSector = Start_SecDir1 + (nClast - 1) * sClSec;    //Номер сектора по номеру кластера
   LONGLONG Poz = LONGLONG(sSecB) * nSector;
   if(SetInFilePointer(Poz) < 0) return -1;                  //Изменение позиции указателя на диске
   if(WriteFile(hDrive, buff, sCl_B, &nb, NULL) == FALSE || nb != sCl_B)
     return ErrorSys1((Lan+175)->msg);                       //"Ошибка при записи кластера."
   return 0;
}
#endif

//-------------------------------------------------------------------------------

#if !defined EMULATOR_HDD                                    //НЕТ Режима эмулятора
int Save_FAT1(void)                                          //Сохранение обновленной FAT первого раздела
{
   //Возможно лучше менять не всю FAT, а только изменившиеся сектора или один изменившийся кусок FAT (несколько секторов)
   //Контрольное чтение FAT и сравнение с памятью, повторная запись в случае обнаружения несоответсвия
   DWORD nb;
   LONGLONG Poz = LONGLONG(sSecB) * Start_SecFAT1;
   if(SetInFilePointer(Poz) < 0) return -1;                  //Изменение позиции указателя в файле
   if(WriteFile(hDrive, c_FAT1, Size_FAT1, &nb, NULL) == FALSE || nb != Size_FAT1)
      return ErrorSys1((Lan+176)->msg);                      //"Ошибка при записи FAT."
   CopyMemory(FAT1, c_FAT1, Size_FAT1);                      //Скопировали содержимое FAT1
   return 0;
}
#endif

//------------------------------------------------------------------------------

void Ansi_To_Unicode(char *inANSI, WCHAR *outUNI, int maxN)  //Преобразование строки символов в Unicode
{
   int n = lstrlen(inANSI);
   MultiByteToWideChar(CP_ACP, 0, inANSI, n, outUNI, maxN);
   for(int i=0; i<n; i++) SWAP16((WORD *)(outUNI + i));      //В каталоге байты переставлены
   for(int i=n; i<maxN; i++) *(WORD *)(outUNI + i) = 0;      //Обнулили хвост
}

//------------------------------------------------------------------------------

static void NameToKat(char *NameF, char *ExtF, One_Str_Cat *Kat) //Формирование имени файла
{
   int n = lstrlen(NameF);
   if(n > 38)
   {  *(NameF + 38) = 0;                                     //Отсекли имя
       NumMsg38++;
   }
   Ansi_To_Unicode(NameF, Kat->Name, 40);                    //Преобразовали имя из UNICODE
   if(*ExtF == '.')                                          //Есть расширение
      Ansi_To_Unicode(ExtF+1, Kat->Ext, 4);                  //Преобразовали расширение из UNICODE
   else
      for(int i=0; i<4; i++) *(WORD *)(Kat->Ext + i) = 0;    //Обнулили неиспользуемое
}

//------------------------------------------------------------------------------

static int Add_Item_To_Tree(char *NameF)                     //Добавления строки в дерево
{
   char Driv[_MAX_DRIVE], Dir[_MAX_PATH], Name[_MAX_FNAME], Ext[_MAX_EXT];

   _splitpath(NameF, Driv, Dir, Name, Ext);
   if((s_Kat+ns_Dir)->pf.type == 48)                         //Это папка
   {  lstrcat(Name, Ext);                                    //У папки расширения нет как такового
      *Ext = *(Ext+1) = 0;
   }
   int l = lstrlen(Name);
   if(l > 38) *(Name + 38) = 0;
   return AddItemToTree_forWrite(Name, Ext+1, &((s_Kat+ns_Dir)->pf));//Добавление новой строки в дерево
}

//------------------------------------------------------------------------------

int Load_Dir(void)                                           //Загрузка кластера каталога
{
   //Чтение кластера каталога (каталога может и не существовать и, в том числе каталога самого верхнего уровня)
   //Обновление каталога (добавление строки нового файла) и в том числе создание нового каталога
   DWORD nSector = Start_SecDir1 + (ClStDir - 1) * sClSec;   //Номер сектора по номеру кластера
   if(ReadClast1_P(nSector, bufDir) < 0) return -1;          //Чтение кластера
   s_Kat = (One_Str_Cat *)bufDir;
   int prPoint = 0;                                          //Признак наличия каталогов с точкой как признак правильности структуры
   ns_Dir = 0;                                               //Номер свободной строки каталога
   for(; ns_Dir<4*sClSec; ns_Dir++)                          //По всем возможным записям в одном кластере каталога (В каждом секторе 4 записи)
   {  if((s_Kat + ns_Dir)->pf.type == 0) break;              //Конец записей каталога
      DWORD NamePoint = *((DWORD*)&(s_Kat + ns_Dir)->Name);
      if(NamePoint == 0x00002E00)                            //Это одна точка т.е. кластер текущего каталога
      { if(ns_Dir == 0) prPoint++; continue; }
      if(NamePoint == 0x2E002E00)                            //Это две точки т.е. кластер предыдущего каталога
      { if(ns_Dir == 1) prPoint++; continue; }
      WORD NameDel = *((WORD*)&(s_Kat + ns_Dir)->Name);
      if(NameDel == 0xE500 && (s_Kat + ns_Dir)->pf.ClSt == 0) break;//Это удаленное имя т.е. свободная строка
   }
   if(prPoint != 2)
      return Error1((Lan+96)->msg);                          //"Нарушена структура каталога."
   if(ns_Dir < 2 || ns_Dir >= 4*sClSec)
      return Error1((Lan+181)->msg);                         //"В каталоге нет места для новой записи."
   return 0;
}

//------------------------------------------------------------------------------

int Change_Dir_For_File(char *NameF)                         //Изменение каталога (внесение нового файла)
{
   for(; ns_Dir<4*sClSec; ns_Dir++)                          //По всем возможным записям в одном кластере каталога (В каждом секторе 4 записи)
   {  if((s_Kat + ns_Dir)->pf.type == 0) break;              //Конец записей каталога
      WORD NameDel = *((WORD*)&(s_Kat + ns_Dir)->Name);
      if(NameDel == 0xE500 && (s_Kat + ns_Dir)->pf.ClSt == 0) break;//Это удаленное имя т.е. свободная строка
   }
   if(ns_Dir >= 4*sClSec)
      return Error1((Lan+181)->msg);                         //"В каталоге нет места для новой записи."
   ZeroMemory(s_Kat+ns_Dir, sizeof(One_Str_Cat));            //Все очистили
   char Driv[_MAX_DRIVE], Dir[_MAX_PATH], Name[_MAX_FNAME], Ext[_MAX_EXT];

   _splitpath(NameF, Driv, Dir, Name, Ext);
   NameToKat(Name, Ext, s_Kat+ns_Dir);                       //Формирование имени файла в Unicode
   SYSTEMTIME sysTime;
   FILETIME TimeF;                                           //Дата и время входного файла
   FileTimeToLocalFileTime(&Time_inF, &TimeF);
   FileTimeToSystemTime(&TimeF, &sysTime);
   (s_Kat+ns_Dir)->pf.type = 32;                             //Тип файл
   (s_Kat+ns_Dir)->pf.ClSt = nCl_1;                          //Номер кластера начала файла
   (s_Kat+ns_Dir)->pf.Sec = sysTime.wSecond;
   (s_Kat+ns_Dir)->pf.Min = sysTime.wMinute;
   (s_Kat+ns_Dir)->pf.Hour = sysTime.wHour;
   (s_Kat+ns_Dir)->pf.Day = sysTime.wDay;
   (s_Kat+ns_Dir)->pf.Mon = sysTime.wMonth;
   (s_Kat+ns_Dir)->pf.Year = sysTime.wYear;
   (s_Kat+ns_Dir)->pf.SizeF = Size_inF.QuadPart;
   Add_Item_To_Tree(NameF);                                  //Добавления строки в дерево
   return 0;
}

//------------------------------------------------------------------------------

int Change_Dir_For_Folder(char *NameF)                       //Изменение каталога (внесение новой папки)
{
   for(; ns_Dir<4*sClSec; ns_Dir++)                          //По всем возможным записям в одном кластере каталога (В каждом секторе 4 записи)
   {  if((s_Kat + ns_Dir)->pf.type == 0) break;              //Конец записей каталога
      WORD NameDel = *((WORD*)&(s_Kat + ns_Dir)->Name);
      if(NameDel == 0xE500 && (s_Kat + ns_Dir)->pf.ClSt == 0) break;//Это удаленное имя т.е. свободная строка
   }
   if(ns_Dir >= 4*sClSec)
      return Error1((Lan+181)->msg);                         //"В каталоге нет места для новой записи."
   ZeroMemory(s_Kat+ns_Dir, sizeof(One_Str_Cat));            //Все очистили
   NameToKat(NameF, "", s_Kat+ns_Dir);                       //Формирование имени папки в Unicode
   SYSTEMTIME sysTime;
   GetLocalTime(&sysTime);
   (s_Kat+ns_Dir)->pf.type = 48;                             //Тип папка
   (s_Kat+ns_Dir)->pf.ClSt = nCl_1;                          //Номер кластера начала файла
   (s_Kat+ns_Dir)->pf.Sec = sysTime.wSecond;
   (s_Kat+ns_Dir)->pf.Min = sysTime.wMinute;
   (s_Kat+ns_Dir)->pf.Hour = sysTime.wHour;
   (s_Kat+ns_Dir)->pf.Day = sysTime.wDay;
   (s_Kat+ns_Dir)->pf.Mon = sysTime.wMonth;
   (s_Kat+ns_Dir)->pf.Year = sysTime.wYear;
   Add_Item_To_Tree(NameF);                                  //Добавления строки в дерево
   return 0;
}

//------------------------------------------------------------------------------

#if !defined EMULATOR_HDD                                    //НЕТ Режима эмулятора
int Save_Dir(void)                                           //Сохранение кластера каталога
{
   if(WriteClast1P(ClStDir, bufDir) < 0) return -1;          //Запись кластера каталога
   //Контрольное чтение и сравнения с эталоном, повторная запись в случае обнаружения несоответсвия
   return 0;
}

#endif

#endif
#endif

