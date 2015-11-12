
//                                14.03.2009
#ifndef __HDD_LG_WRITE_1__
#define __HDD_LG_WRITE_1__

#include "from_hdd_lg_to_pc.h"

#if defined WRITE_YES                                        //Режим записи разрешен

//============================ hdd_lg_write_1 ==================================
int  File_ToHDD(void);                                       //Запись файлов на диск LG
int  New_Folder_ToHDD(void);                                 //Cоздание папок на диске LG
int  Folder_ToHDD(void);                                     //Запись папок на диск LG
     char *spisF;                                            //Память списка файлов

static BYTE bufIO[sCl_B];                                    //Память под один кластер
static int numF_Sp;                                          //Число имен в списке
static DWORD all_nCl;                                        //Общее число кластеров в записи
static int writeNewCl;                                       //Число записанных кластеров
static DWORD *pozNameF;                                      //Таблица смещений к именам файла в списке

//------------------------------------------------------------------------------

static int MaloMesta(DWORD nCl)
{
   char ss1[200], ss2[200];
   wsprintf(ss1, "     %s   %s %s.", (Lan+48)->msg, FileSize_Txt(DWORDLONG(maxZapFAT1 - 1 - writeCl) * sCl_B), (Lan+47)->msg);
   wsprintf(ss2, "     %s %s %s.", (Lan+49)->msg, FileSize_Txt(DWORDLONG(nCl) * sCl_B), (Lan+47)->msg);
   return Error3((Lan+190)->msg, ss1, ss2);                  //"Запись невозможна, на HDD LG нет свободного места."},
}

//------------------------------------------------------------------------------

static int WriteFile_ToHDD(char *NameF, char *msg)           //Запись файла на HDD рекордера
{
   DWORD nb;

   if(OpenInFilePC(NameF) < 0) return -1;                    //Открыли входной файл для чтения с компьютера
   DWORD wSizeB = sCl_B;                                     //Число байт чтения равно числу байт в кластере
   LONGLONG SizeToEnd = Size_inF.QuadPart;                   //Число байт которые осталось записать
   DWORD numCl = DWORD((Size_inF.QuadPart + sCl_B - 1) / sCl_B);//Число кластеров необходимое для размещения файла данного размера
   if(numCl > maxZapFAT1 - 1 - writeCl)
      return MaloMesta(numCl);                               //"Запись невозможна, на HDD LG нет свободного места."},
   if(numCl > 0)
      InitProgressBar(numCl, msg);
   DWORD n, nCl = 1;                                         //Номер последнего записанного кластера
   for(DWORD i=0; i<numCl; i++)                              //По числу кластеров
   {  for(n=nCl+1; n<maxZapFAT1; n++)                        //Просмотр FAT для поиска первого свободного кластера
        if(*(c_FAT1 + n) == 0) break;
      if(n >= maxZapFAT1)
         return Error1((Lan+190)->msg);                      //"Запись невозможна, на HDD LG нет свободного места."},
      if(wSizeB > SizeToEnd)
      {  wSizeB = DWORD(SizeToEnd);                          //Размер остатка чтения меньше размера кластера
         ZeroMemory(bufIO, sCl_B);
      }
      if(ReadFile(inFile, bufIO, wSizeB, &nb, NULL) == false || nb != wSizeB)
         return ErrorSys1((Lan+87)->msg);                    //"Ошибка при чтении файла."
#if !defined EMULATOR_HDD                                    //НЕТ Режима эмулятора
      if(WriteClast1P(n, bufIO) < 0) return -1;              //Запись кластера
#endif
      if(nCl > 1)
        *(c_FAT1 + nCl) = n;                                 //Ссылка на следубщий кластер в цепочке кластеров
      else nCl_1 = n;                                        //Номер кластера начала файла
      nCl = n;                                               //Последний записанный кластер
      SizeToEnd -= wSizeB;                                   //Число байт которые осталось прочитать
      if(ProgressBar(wSizeB) < 0) return -1;                 //Оператор нажал кнопку Прервать
   }
   *(c_FAT1 + nCl) = 0x0FFFFFFF;                             //Признак конца цепочки
   CloseFile(&inFile);
   writeNewCl += numCl;                                      //Число записанных кластеров
   return Change_Dir_For_File(NameF);                        //Изменение каталога (запись нового файла)
}

//------------------------------------------------------------------------------

static int CopyOne_FileToHDD(char *NameF)                    //Запись единственного файла на HDD рекордера
{
   //Надо проверить, что такого имени в папке нет
   c_FAT1 = (DWORD *)MyAllocMem(Size_FAT1);                  //Память под FAT
   if(c_FAT1 == NULL)  return -1;
   CopyMemory(c_FAT1, FAT1, Size_FAT1);                      //Скопировали содержимое FAT1
   if(Load_Dir() < 0) return -1;                             //Загрузка кластера каталога
   if(WriteFile_ToHDD(NameF, (Lan+24)->msg) < 0) return -1;  //Запись файла на HDD рекордера
#if !defined EMULATOR_HDD                                    //Режим эмулятора
   if(Save_FAT1() < 0) return -1;                            //Сохранение обновленной FAT первого раздела
   if(Save_Dir() < 0) return -1;                             //Сохранение кластера каталога
   if(Change_Sec67(-writeNewCl) < 0) return -1;              //Внесение изменений в содержимое сектора 67
#endif
   writeCl += writeNewCl;                                    //Число записанных кластеров
   ViewSize();                                               //Показа дискового пространства
   return 0;
}

//------------------------------------------------------------------------------

static int CopyOneFileToHDD(char *NameF)                     //Запись единственного файла на HDD рекордера
{
   writeNewCl = 0;                                           //Число записанных кластеров
   InitProgressBar = InitProgressBar1;
   ProgressBar = ProgressBar1;
   Close_ProgressBar = Close_ProgressBar1;
   int ret = CopyOne_FileToHDD(NameF);                       //Запись единственного файла на HDD рекордера
   Close_ProgressBar();
   MyFreeMem(&(void*)c_FAT1);
   MyFreeMem(&(void*)spisF);
   CloseFile(&inFile);
   return ret;
}

//------------------------------------------------------------------------------

static int Work_Spis(char *SpisF, int sm)                      //Разборка списка файлов
{
   numF_Sp = 0;                                              //Число имен в списке
   pozNameF[numF_Sp] = sm;                                   //Таблица смещений к именам файлов (смещение к имени первого файла)
   for(;;)                                                   //По списку файлов
   {  int L = lstrlen(SpisF + pozNameF[numF_Sp]);            //Число символов в имени файла
      if(L == 0) break;                                      //Пустое имя это конец списка
      numF_Sp++;
      if(numF_Sp >= MAX_NAME_S)                              //Максимальное число файлов
        return Error1((Lan+182)->msg);                       //"Число файлов для копирования превысило возможности программы.");
      pozNameF[numF_Sp] = pozNameF[numF_Sp-1] + L + 1;       //Таблица смещений к именам файлов (смешение к имени очередного файла)
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Mk_AllSize(char *SpisF)                           //Вычисление общего число кластеров записи
{
   char NameF[260];
   all_nCl = 0;
   for(int i=0; i<numF_Sp; i++)                              //По числу имен в списке
   {  lstrcpy(NameF, SpisF);                                 //Переслали имя каталога
      lstrcat(NameF, "\\");                                  //Добавили разделитель
      lstrcat(NameF, SpisF + pozNameF[i]);                   //Добавили имя файла
      int ret = OpenInFilePC(NameF);                         //Открыли входной файл для чтения с компьютера
      CloseFile(&inFile);
      if(ret < 0) return -1;
      all_nCl += DWORD((Size_inF.QuadPart + sCl_B - 1) / sCl_B);//Число кластеров необходимое для размещения всех файлов
   }
   if(all_nCl > maxZapFAT1 - 1 - writeCl)
      return MaloMesta(all_nCl);                             //"Запись невозможна, на HDD LG нет свободного места."},
   return 0;
}

//------------------------------------------------------------------------------

static int CopyAll_FileToHDD(char *SpisF)                    //Запись списка файла на HDD рекордера
{
   char NameF[260], Ss[100];
   //Надо проверить, что такого имени в папке нет
   InitProgressBar2_2(all_nCl, "");                          //Функция прогресса для нескольких файлов
   c_FAT1 = (DWORD *)MyAllocMem(Size_FAT1);                  //Память под FAT
   if(c_FAT1 == NULL)  return -1;
   CopyMemory(c_FAT1, FAT1, Size_FAT1);                      //Скопировали содержимое FAT1
   if(Load_Dir() < 0) return -1;                             //Загрузка кластера каталога
   for(int i=0; i<numF_Sp; i++)                              //По числу имен в списке
   {  lstrcpy(NameF, SpisF);                                 //Переслали имя каталога
      lstrcat(NameF, "\\");                                  //Добавили разделитель
      lstrcat(NameF, SpisF + pozNameF[i]);                   //Добавили имя файла
      sprintf(Ss, "%s:   %d  ( %d )", (Lan+24)->msg, i+1, numF_Sp);
      int ret = WriteFile_ToHDD(NameF, Ss);                  //Запись файла на HDD рекордера
      CloseFile(&inFile);
      if(ret < 0) return -1;
      Close_ProgressBar();
   }
#if !defined EMULATOR_HDD                                    //Режим эмулятора
   if(Save_FAT1() < 0) return -1;                            //Сохранение обновленной FAT первого раздела
   if(Save_Dir() < 0) return -1;                             //Сохранение кластера каталога
   if(Change_Sec67(-writeNewCl) < 0) return -1;              //Внесение изменений в содержимое сектора 67
#endif
   writeCl += writeNewCl;                                    //Число записанных кластеров
   ViewSize();                                               //Показа дискового пространства
   return 0;
}

//------------------------------------------------------------------------------

static int Copy_File_ToHDD(int sm)                           //Запись нескольких файлов на HDD рекордера
{
   writeNewCl = 0;                                           //Число записанных кластеров
   if(Work_Spis(spisF, sm) < 0) return -1;                   //Разборка списка нескольких файлов на HDD рекордера
   if(Mk_AllSize(spisF) < 0) return -1;                      //Вычисление общего число кластеров записи
   InitProgressBar = InitProgressBar1_2;                     //Функция прогресса для одного файла из группы
   ProgressBar = ProgressBar2;
   Close_ProgressBar = Close_ProgressBar1_2;
   int ret = CopyAll_FileToHDD(spisF);                       //Запись списка файла на HDD рекордера
   Close_ProgressBar2_2();
   MyFreeMem(&(void*)c_FAT1);
   MyFreeMem(&(void*)spisF);
   return ret;
}

//------------------------------------------------------------------------------

int File_ToHDD(void)                                         //Запись файлов на диск LG
{
   int sm;
   spisF = (char *)MyAllocMem(MAX_SP + MAX_NAME_S*sizeof(DWORD));
   if(spisF == NULL) return -1;                              //Нет памяти
   pozNameF = (DWORD*)(spisF + MAX_SP);                      //Таблица смещений к именам файла в списке
   *spisF = 0;
   int ret = GetSpNameInFile(spisF, MAX_SP, &sm);            //Ввод списка имен для записи
   if(ret != 0)  return ret;
   if(*(spisF+sm-1) != 0)                                    //Проверка это единственное имя или список
      return CopyOneFileToHDD(spisF);                        //Запись единственного файла на HDD рекордера
   return Copy_File_ToHDD(sm);                               //Запись нескольких файлов на HDD рекордера
}

//------------------------------------------------------------------------------

static int CreateNewFolder(void)                             //Создание новой папки на HDD рекордера
{
   DWORD nCl = 2;
   for(; nCl<maxZapFAT1; nCl++)                              //Просмотр FAT для поиска первого свободного кластера
      if(*(c_FAT1 + nCl) == 0) break;
   if(nCl >= maxZapFAT1)
      return Error1((Lan+190)->msg);                         //"Запись невозможна, на HDD LG нет свободного места."},
   ZeroMemory(bufIO, sCl_B);                                 //Все очистили
   SYSTEMTIME sysTime;
   GetLocalTime(&sysTime);
   One_Str_Cat *Kat = (One_Str_Cat *)bufIO;                  //Каталог
   *(DWORD *)(Kat+0)->Name = 0x00002E00;                     //Это одна точка т.е. кластер текущего каталога
   *(DWORD *)(Kat+1)->Name = 0x2E002E00;                     //Это две точки т.е. кластер предыдущего каталога
   (Kat+0)->pf.type = (Kat+1)->pf.type = 48;                 //Тип папка
   (Kat+0)->pf.ClSt = nCl_1 = nCl;                           //Номер кластера текущего каталога
   (Kat+1)->pf.ClSt = ClStDir;                               //Номер кластера родительского каталога
   (Kat+0)->pf.Sec  = (Kat+1)->pf.Sec  = sysTime.wSecond;
   (Kat+0)->pf.Min  = (Kat+1)->pf.Min  = sysTime.wMinute;
   (Kat+0)->pf.Hour = (Kat+1)->pf.Hour = sysTime.wHour;
   (Kat+0)->pf.Day  = (Kat+1)->pf.Day  = sysTime.wDay;
   (Kat+0)->pf.Mon  = (Kat+1)->pf.Mon  = sysTime.wMonth;
   (Kat+0)->pf.Year = (Kat+1)->pf.Year = sysTime.wYear;
#if !defined EMULATOR_HDD                                    //Режим эмулятора
   if(WriteClast1P(nCl, bufIO) < 0) return -1;               //Запись кластера
#endif
   *(c_FAT1 + nCl) = 0x0FFFFFFF;                             //Признак конца цепочки
   return Change_Dir_For_Folder(NameFoFi);                   //Изменение каталога (запись новой папки)
}

//------------------------------------------------------------------------------

static int Create_New_Folder_ToHDD(void)                     //Cоздание папки на диске LG
{
   if(*NameFoFi == 0)
     return Error1((Lan+184)->msg);                          //"Недопустимое имя"
   c_FAT1 = (DWORD *)MyAllocMem(Size_FAT1);                  //Память под FAT
   if(c_FAT1 == NULL)  return -1;
   CopyMemory(c_FAT1, FAT1, Size_FAT1);                      //Скопировали содержимое FAT1
   if(Load_Dir() < 0) return -1;                             //Загрузка кластера каталога
   if(CreateNewFolder() < 0) return -1;                      //Создание новой папки на HDD рекордера
#if !defined EMULATOR_HDD                                    //Режим эмулятора
   if(Save_FAT1() < 0) return -1;                            //Сохранение обновленной FAT первого раздела
   if(Save_Dir() < 0) return -1;                             //Сохранение кластера каталога
   if(Change_Sec67(-1) < 0) return -1;                       //Внесение изменений в содержимое сектора 67
#endif
   writeCl++;                                                //Число записанных кластеров
   ViewSize();                                               //Показа дискового пространства
   MyFreeMem(&(void*)c_FAT1);
   MyFreeMem(&(void*)spisF);
   return 0;
}

//------------------------------------------------------------------------------

int New_Folder_ToHDD(void)                                   //Cоздание папок на диске LG
{
   if(DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_NAME),
                MainWin, Dlg_NewName, LPARAM(0)) == IDCANCEL) return -1;
   return Create_New_Folder_ToHDD();                         //Cоздание папки на диске LG
}

//------------------------------------------------------------------------------

typedef char OneExt[5];
static OneExt ExtEn[] =  { ".mp3", ".wma",
                           ".jpg",
                           ".avi", ".srt", ".smi", ".sub", ".txt", ".mpg", ".divx" };
static int nExt[3] = {2, 1, 7};                              //Число расширений в списке
static int pExt[3] = {0, 2, 3};                              //Позиция первого расширений в списке

static int Make_Spis(int *n)                                 //Создание списка файлов
{
   char NameF[256];
   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT];

   WIN32_FIND_DATA Data;
   HANDLE File;

   int l = lstrlen(spisF) + 1;
   lstrcpy(NameF, spisF);
   lstrcat(NameF, "\\*.*");
   if((File = FindFirstFile(NameF, &Data)) == INVALID_HANDLE_VALUE)  return -1; //Файл не найден (папка пустая)
   for(;;)
   {
      fnsplit(Data.cFileName, Driv, Dir, Name, Ext);
      if(!(lstrcmp(Data.cFileName, ".") == 0 ||              //Имя файла  "." нас не интересует
           lstrcmp(Data.cFileName, "..") == 0))              //Имя файла  ".." нас не интересует
      {  //lstrcpy(NameF+l, Data.cFileName);                 //Сделали полное имя с путем
         if((Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) //Текущее имя это вложенная папка
         {
   //Если внутри папки есть другие папки, то повторить все для внутренний папок, т.е. писать все дерево
         }
         else
         {  OneExt *ExtE = ExtEn + pExt[prFolder];
            for(int i=0; i<nExt[prFolder]; i++)
              if(lstrcmpi(ExtE[i], Ext) == 0)
              {  lstrcpy(spisF+l, Data.cFileName);
                 l += lstrlen(spisF+l) + 1;
                 (*n)++;
                 break;
              }
         }
      }
      if(FindNextFile(File, &Data) == 0)
        if(GetLastError() == ERROR_NO_MORE_FILES) break;
   }
   FindClose(File);
   *(spisF+l) = 0;
   return 0;
}

//------------------------------------------------------------------------------

int Folder_ToHDD(void)                                       //Запись папок на диск LG
{
   char NameDir[256], Driv[MAXDRIVE], Dir[MAXPATH], Ext[MAXEXT];

   if(Get_Name_Dir((Lan+186)->msg, NameDir, 1) < 0) return -1;//Запрос имени папки
   fnsplit(NameDir, Driv, Dir, NameFoFi, Ext);
   lstrcat(NameFoFi, Ext);                                   //У папки расширения нет как такового
   if(Create_New_Folder_ToHDD() < 0) return -1;              //Cоздание папки на диске LG
   ClStDir = nCl_1;                                          //Номер кластера каталога в который выполняется запись, т.е. вновь созданный каталог
   spisF = (char *)MyAllocMem(MAX_SP + MAX_NAME_S * sizeof(DWORD));
   if(spisF == NULL) return -1;                              //Нет памяти
   pozNameF = (DWORD*)(spisF + MAX_SP);                      //Таблица смещений к именам файла в списке
   lstrcpy(spisF, NameDir);
   int sm = lstrlen(spisF) + 1;
   int n = 0;
   if(Make_Spis(&n) < 0) return -1;                          //Создание списка файлов
   if(n == 0) return 0;                                      //Нет ни одного имени в списке
   return Copy_File_ToHDD(sm);                               //Запись нескольких файлов на HDD рекордера
}

#endif
#endif

