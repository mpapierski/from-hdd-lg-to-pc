
//                                    24.01.2009
#ifndef __HDD_LG_COPY__
#define __HDD_LG_COPY__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_copy =====================================
int  CopyFiles(void);                                        //Выполнение копирования
     DWORD num_Sel;                                          //Число выбранных имен при групповом выборе
     LONGLONG size_Sel;                                      //Суммарный размер выбранных файлов
     DWORD indF;                                             //Индекс копируемого файла

static HANDLE outFile;
static DWORD numAllInCl;                                     //Суммарное число кластеров во всех копируемых файлах
static DWORD numAllFi;                                       //Число копируемых файлов
static DWORD num_T_Fi;                                       //Номер текущего копируемого файла
static DWORD BytesPerClaster;                                //Число байт в кластере
static DWORDLONG numAllOutCl;                                //Суммарный число выходных кластеров

//------------------------------------------------------------------------------

static int Ctrl_OutName(char *outNameF)                      //Проверка, что длина имени менее предельной
{
// char Driv[MAXDRIVE+3], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT+3];
// fnmerge(outNameF, Driv, Dir, Name, Ext);
   int l = lstrlen(outNameF);
   if(l > 255)  //if(l > 259)                                //Боремся с предельно длинными именвми
     return Error3(outNameF, "", (Lan+95)->msg);             //return Error3(outNameF, "", "Число символов в полном имени файла превышает предел для  Windows.");
   return 0;
}

//------------------------------------------------------------------------------

//11static int Copy_One_File(PAR_FILE *pf, char *msg, char *fName) //Копирование одного файла
static int Copy_One_File(PAR_FILE *pf, char *msg)            //Копирование одного файла
{
   BYTE buff[sCl_B];                                         //Память под один кластер
   DWORD nb;
#if !defined EMULATOR_HDD_AND_COPY                           //НЕТ Режима эмулятора с эмуляцией копирования
   ZeroMemory(buff, sCl_B);                                  //Очистка буфера чтения
#endif
   DWORD wSizeB = sCl_B;                                     //Число записываемых байт равно числу байт в кластере
   DWORD nCl = pf->ClSt;                                     //Текущий номер кластера равен первому кластеру файла
   LONGLONG SizeToEnd = pf->SizeF;                           //Число байт которые осталось записать
   DWORD numCl = DWORD((pf->SizeF + sCl_B - 1) / sCl_B);     //Число кластеров необходимое для размещения файла данного размера
   if(numCl > 0)
      InitProgressBar(numCl, msg);
   for(DWORD i=0; i<numCl; i++)                              //По числу кластеров
   {  if(nCl == 0x0FFFFFFF)                                  //Признак конца цепочки
        return Error1((Lan+12)->msg);                        //return Error1("Неожиданно найден признак конца цепочки FAT.");
      if(*(FAT1 + nCl) == 0)                                 //Ненормальная ситуация
        return Error1((Lan+11)->msg);                        //return Error1("Обнаружено несоответствие значения FAT и ссылки на кластер файла.");
      if(wSizeB > SizeToEnd)
//11     wSizeB = DWORD(((SizeToEnd + 511) / 512) * 512);    //Размер остатка записи меньше размера кластера  ?? Почему сектор
         wSizeB = DWORD(SizeToEnd);                          //Размер остатка записи меньше размера кластера
#if !defined EMULATOR_HDD_AND_COPY                           //НЕТ Режима эмулятора с эмуляцией копирования
      DWORD nSector = Start_SecDir1 + (nCl - 1) * sClSec;    //Сектор начала текущего кластера
      if(ReadClast1_P(nSector, buff) < 0) return -1;         //Чтение очeредного кластера
#endif
      if(WriteFile(outFile, buff, wSizeB, &nb, NULL) == false || nb != wSizeB)
        return ErrorSys1((Lan+83)->msg);                     //return ErrorSys1("Ошибка при записи выходного файла.");
      SizeToEnd -= wSizeB;                                   //Число байт которые осталось записать
      nCl = *(FAT1 + nCl);                                   //Номер следующего кластера
      if(nCl > maxZapFAT1 && nCl != 0x0FFFFFFF)
        return Error1((Lan+13)->msg);                        //"Номер кластера превышает допустимое значение."
      if(SizeToEnd == 0 && nCl != 0x0FFFFFFF)
        Error1((Lan+29)->msg);                               //return Error2("Выходной файл заданного размера записан,", "а признак конца цепочки FAT не найден.");
      if(ProgressBar(wSizeB) < 0) return -1;                 //Оператор нажал кнопку Прервать
   }
   CloseFile(&outFile);                                      //Закрыли выходной файл
/*11
   if((pf->SizeF % sCl_B) != 0)                              //Размер файла не является кратным размеру кластера
 {
   int n = 0;                                                //Счетчик для ожидания освобождения файла
   if(Open_FileR(fName, &outFile, &n) < 0)                   //Открытие выходного файла
      return -1;
   LARGE_INTEGER TPoz;
   TPoz.QuadPart = pf->SizeF;
   TPoz.u.LowPart = SetFilePointer(outFile, TPoz.u.LowPart, &TPoz.u.HighPart, FILE_BEGIN);
   if(TPoz.u.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
      return ErrorSys1((Lan+83)->msg);                       //"Ошибка при позиционировании по диску."
    if(SetEndOfFile(outFile) == false)
      return ErrorSys1((Lan+83)->msg);                     //return ErrorSys1("Ошибка при записи выходного файла.");
 }
*/
   FILETIME fTime, flTime;
   SYSTEMTIME sysTime;
   sysTime.wYear = pf->Year;
   sysTime.wMonth = pf->Mon;
// sysTime.wDayOfWeek = 0;
   sysTime.wDay = pf->Day;
   sysTime.wHour = pf->Hour;
   sysTime.wMinute = pf->Min;
   sysTime.wSecond = pf->Sec;
   sysTime.wMilliseconds = 0;
   SystemTimeToFileTime(&sysTime, &flTime);                  //Преобразовали время
   LocalFileTimeToFileTime(&flTime, &fTime);
   SetFileTime(outFile, &fTime, &fTime, &fTime);             //Дата файла
   return 0;
}

//------------------------------------------------------------------------------

static int Copy_File1(DWORD ind)                             //Копирование выбранного файла на диск ПК
{
   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT];
   char outNameF1[260], NameFF[260];

   fnsplit(outNameF1, Driv, Dir, Name, Ext);
   lstrcpy(NameFF, (aTree+ind)->NameF);
   Ctrl_Name_Ch(NameFF);                                     //Проверка имени на недопустимые символы
   fnmerge(outNameF1, Driv, Dir, NameFF, "");
   indF = ind;                                               //Индекс копируемого файла
   if(OpenOutFileName_New(outNameF1, (aTree+ind)->pf.SizeF, &outFile) < 0) return -1; //Открытие выходного файла
   UpdateWindow(MainWin);                                    //Восстановили окно после диалога выбора файла
   InitProgressBar = InitProgressBar1;
   ProgressBar = ProgressBar1;
   Close_ProgressBar = Close_ProgressBar1;
//11   int ret = Copy_One_File(&((aTree+ind)->pf), (Lan+24)->msg, outNameF1);//int ret = Copy_One_File((Tree+ind)->pf.SizeF, (Tree+ind)->pf.ClSt,  "Копирование файла");
   int ret = Copy_One_File(&((aTree+ind)->pf), (Lan+24)->msg);               //int ret = Copy_One_File((Tree+ind)->pf.SizeF, (Tree+ind)->pf.ClSt,  "Копирование файла");
   Close_ProgressBar();
   CloseFile(&outFile);                                      //Закрыли входной файл
   if(ret < 0) DeleteFile(outNameF1);
   return ret;
}

//------------------------------------------------------------------------------

static int Copy_Folder_(HTREEITEM hitem, char *Name_Dir, char *NameFo)//Копирование выбранной папки на диск ПК
{
   HTREEITEM ind = TreeView_GetChild(hwndTree, hitem);       //Вошли во внутренний уровень дерева
   if(ind == NULL) return 0;
   char nName_Dir[512];                                      //Суммарный путь
   lstrcpy(nName_Dir, Name_Dir);
   lstrcat(nName_Dir, "\\");
   Ctrl_Name_Ch(NameFo);                                     //Проверка имени на недопустимые символы и замена недопустимых символов
   lstrcat(nName_Dir, NameFo);
// int l = lstrlen(nName_Dir);
// if((l+45) >= 256)                                         //Длина имени файла до 45 знаков
//   if(MsgYesNo4(nName_Dir, "Длина пути к указанной папке близка к предельной.",
//                           "Возможны ошибки.", "Продолжать ?") == 'N') return -1;
   if(CtrlFileYesNo(nName_Dir) == 0)                         //Каталога первого копируемого уровня нет
      if(CreateDirectory(nName_Dir, NULL) == FALSE)
        return Error2(nName_Dir, (Lan+93)->msg);             //return Error2(nName_Dir, "Системная ошибка при создании папки.");
   for(;;)                                                   //Просмотр всего дерева от текущей папки
   {  TV_ITEM item;
      char Msg[100];
      item.mask = TVIF_TEXT | TVIF_PARAM;
      item.hItem = ind;
      item.pszText = Msg;
      item.cchTextMax = 100;
      if(TreeView_GetItem(hwndTree, &item) == FALSE)
        return Error1((Lan+33)->msg);                        //return Error1("Ошибка при запросе информации об элементе дерева.");
      if((aTree + item.lParam)->pf.type == 48 ||             //Очередное имя это папка
         (aTree + item.lParam)->pf.type == 47)               //Данное имя это специальная папка
      {   if(Copy_Folder_(ind, nName_Dir, (aTree + item.lParam)->NameF) < 0) return -1; }//Копирование файлов одной папки
      else                                                   //Очередное имя это файл
      {  char outNameF[260], Ss[300], NameFF[260];
         lstrcpy(outNameF, nName_Dir);
         lstrcat(outNameF, "\\");
         lstrcpy(NameFF, (aTree + item.lParam)->NameF);
         Ctrl_Name_Ch(NameFF);                               //Проверка имени на недопустимые символы и замена недопустимых символов
         lstrcat(outNameF, NameFF);
         if(Ctrl_OutName(outNameF) < 0) return  -1;          //Проверка, что длина имени менее предельной
         indF = item.lParam;                                 //Индекс копируемого файла
         int ret = OpenOutFileName_Yes(outNameF, (aTree + item.lParam)->pf.SizeF, &outFile);//Открытие выходного файла
         if(ret == -1) return -1;
         num_T_Fi++;
         sprintf(Ss, "%s:   %d  ( %d )", (Lan+24)->msg, num_T_Fi, numAllFi);
         if(ret == -2)                                       //Нажата кнопка "Пропустить"
         {  ScipProgressBar2((aTree+item.lParam)->pf.SizeF); //Отработка прогресса для пропускаемых файлов
            ret = 0;
         }
         else
//11       ret = Copy_One_File(&((aTree+item.lParam)->pf), Ss, outNameF);//ret = Copy_One_File(&((Tree+item.lParam)->pf), "Копирование файла");
           ret = Copy_One_File(&((aTree+item.lParam)->pf), Ss);          //ret = Copy_One_File(&((Tree+item.lParam)->pf), "Копирование файла");
         Close_ProgressBar();
         CloseFile(&outFile);                                //Закрыли входной файл
         if(ret < 0)
         {  DeleteFile(outNameF);  return -1;  }
      }
      ind = TreeView_GetNextSibling(hwndTree, ind);          //Следующая запись на том же уровне дерева
      if(ind == NULL) break;                                 //Больше записей данного уровня нет
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Calc_Folder_(HTREEITEM hitem, char *Name_Dir, char *NameFo, int pr)//Вычисления для копирование выбранной папки на диск ПК
{
   HTREEITEM ind = TreeView_GetChild(hwndTree, hitem);       //Вошли во внутренний уровень дерева
   if(ind == NULL)
     if(pr == 0) return Error1((Lan+92)->msg);               //if(pr == 0) return Error1("В указанной папке нет файлов для копирования.");
     else return 0;                                          //Для вложенных пустых папок ничего не делаем
   char nName_Dir[512];                                      //Суммарный путь
   lstrcpy(nName_Dir, Name_Dir);
   lstrcat(nName_Dir, "\\");
   lstrcat(nName_Dir, NameFo);
   for(;;)                                                   //Просмотр всего дерева от текущей папки
   {  TV_ITEM item;
      char Msg[100];
      item.mask = TVIF_TEXT | TVIF_PARAM;
      item.hItem = ind;
      item.pszText = Msg;
      item.cchTextMax = 100;
      if(TreeView_GetItem(hwndTree, &item) == FALSE)
         return Error1((Lan+33)->msg);                       //return Error1("Ошибка при запросе информации об элементе дерева.");
      if((aTree + item.lParam)->pf.type == 48 ||             //Очередное имя это папка
         (aTree + item.lParam)->pf.type == 47)               //Данное имя это специальная папка
      {  if(Calc_Folder_(ind, nName_Dir, (aTree + item.lParam)->NameF, ++pr) < 0) return -1; }//Вычисления для копирование выбранной папки на диск ПК
      else                                                   //Очередное имя это файл
      {  numAllInCl += DWORD(((aTree + item.lParam)->pf.SizeF + sCl_B - 1) / sCl_B);  //Суммарное число кластеров во всех копируемых файлах
         numAllOutCl += ((aTree + item.lParam)->pf.SizeF + BytesPerClaster - 1) / BytesPerClaster; //Суммарный число выходных кластеров
         numAllFi++;                                         //Число копируемых файлах
      }
      ind = TreeView_GetNextSibling(hwndTree, ind);          //Следующая запись на том же уровне дерева
      if(ind == NULL) break;                                 //Больше записей данного уровня нет
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Copy_Folder(HTREEITEM hitem, char *NameFo)        //Копирование выбранной папки на диск ПК
{
   char Name_Dir[260];
   if(Get_Name_Dir((Lan+94)->msg, Name_Dir, 0) < 0) return -1;//Запрос имени папки //if(Get_Name_Dir("Укажите диск или папку для записи", Name_Dir) < 0) return -1; //Запрос имени папки
   UpdateWindow(MainWin);                                    //Восстановили окно после диалога выбора файла
   InitProgressBar = InitProgressBar1_2;                     //Функция прогресса для одного файла из группы
   ProgressBar = ProgressBar2;
   Close_ProgressBar = Close_ProgressBar1_2;

   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT], Path[MAXPATH], Ss[512];
   fnsplit(Name_Dir, Driv, Dir, Name, Ext);
   lstrcpy(Path, Driv);
   lstrcat(Path, "\\");
   DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
   if(GetDiskFreeSpace(Path, &SectorsPerCluster, &BytesPerSector,
                    &NumberOfFreeClusters, &TotalNumberOfClusters) == 0)
      return ErrorSys1((Lan+46)->msg);                       //return ErrorSys1("Системная ошибка при запросе емкости диска.");
   BytesPerClaster = SectorsPerCluster * BytesPerSector;     //Число байт в кластере
   numAllInCl = 0;                                           //Суммарное число кластеров во всех копируемых файлах
   numAllFi = 0;                                             //Число копируемых файлов
   num_T_Fi = 0;                                             //Номер текущего копируемого файла
   numAllOutCl = 0;                                          //Суммарный число выходных кластеров
   int l = lstrlen(Name_Dir) - 1;
   if(*(Name_Dir + l) == '\\') *(Name_Dir + l) = 0;          //Для имени диска косая всегда есть
   if(Calc_Folder_(hitem, Name_Dir, NameFo, 0) < 0) return -1;//Вычисления для копирование выбранной папки на диск ПК
   DWORDLONG allSize = numAllOutCl * BytesPerClaster;
   int ret = DiskFreeSpaceEx_F(Name_Dir, allSize);           //Проверка свободного места
   if(ret < 0) return -1;
   if(ret == 1) return Copy_Folder(hitem, NameFo);           //Копирование выбранной папки на диск ПК (повторный запуск для запроса нового пути)
   sprintf(Ss, "%s:  %s", (Lan+91)->msg, NameFo);
   InitProgressBar2_2(numAllInCl, Ss);                       //Функция прогресса для нескольких файлов
   ret = Copy_Folder_(hitem, Name_Dir, NameFo);              //Копирование выбранной папки на диск ПК
   Close_ProgressBar2_2();
   return ret;
}

//------------------------------------------------------------------------------

static int Copy_AllFiles(char *Name_Dir)                     //Копирование выбранных файлов на диск ПК
{
   char outNameF[512], Ss[300], NameFF[260];
   for(int i=0; i<numEl_Tree; i++)                           //По числу элементров в дереве имен
   {  if((aTree + i)->prSel == 0) continue;                  //Данный файл не выбран
      if((aTree + i)->pf.type == 47) continue;               //Данное имя это специальная папка
      lstrcpy(outNameF, Name_Dir);
      lstrcat(outNameF, "\\");
      if((aTree + i)->indFolder != 0)                        //Ссылка на имя папки
      {  lstrcpy(NameFF, (aTree + (aTree + i)->indFolder)->NameF);
         Ctrl_Name_Ch(NameFF);                               //Проверка имени на недопустимые символы и замена недопустимых символов
         lstrcat(outNameF, NameFF);
         if(CtrlFileYesNo(outNameF) == 0)                    //Каталога первого копируемого уровня нет
           if(CreateDirectory(outNameF, NULL) == FALSE)
              return Error2(outNameF, (Lan+93)->msg);        //return Error2(nName_Dir, "Системная ошибка при создании папки.");
         lstrcat(outNameF, "\\");
      }
      lstrcpy(NameFF, (aTree + i)->NameF);
      Ctrl_Name_Ch(NameFF);                                  //Проверка имени на недопустимые символы и замена недопустимых символов
      lstrcat(outNameF, NameFF);
      if(Ctrl_OutName(outNameF) < 0) return -1;              //Проверка, что длина имени менее предельной
      indF = i;                                              //Индекс копируемого файла
      int ret = OpenOutFileName_Yes(outNameF, (aTree + i)->pf.SizeF, &outFile);//Открытие выходного файла
      if(ret == -1) return -1;
      num_T_Fi++;
      sprintf(Ss, "%s:   %d  ( %d )", (Lan+24)->msg, num_T_Fi, numAllFi);//"Копирование файла"
      if(ret == -2)                                          //Нажата кнопка "Пропустить"
      {  ScipProgressBar2((aTree+i)->pf.SizeF);              //Отработка прогресса для пропускаемых файлов
         ret = 0;
      }
      else
//11    ret = Copy_One_File(&((aTree+i)->pf), Ss, outNameF);
        ret = Copy_One_File(&((aTree+i)->pf), Ss);
      Close_ProgressBar();
      CloseFile(&outFile);                                   //Закрыли входной файл
      if(ret < 0)
      {  DeleteFile(outNameF);  return -1;  }
      if((aTree + i)->indFolder != 0)                        //Ссылка на имя папки
         if(--((aTree + (aTree + i)->indFolder)->numCopy) == 0)   //Уменьшили счетчик частей
           RemoveSelectionFromCopy((aTree + i)->indFolder);
      RemoveSelectionFromCopy(i);                            //Снятие выделение файла в дереве при копировании
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Calc_SizeAllFile(void)                            //Вычисления для копирование выбранных файлов на диск ПК
{
   for(int i=0; i<numEl_Tree; i++)                           //По числу элементров в дереве имен
   {  if((aTree + i)->prSel == 0) continue;                  //Данный файл не выбран
      if((aTree + i)->pf.type == 47) continue;               //Данное имя это специальная папка
      numAllInCl += DWORD(((aTree + i)->pf.SizeF + sCl_B - 1) / sCl_B);  //Суммарное число кластеров во всех копируемых файлах
      numAllOutCl += ((aTree + i)->pf.SizeF + BytesPerClaster - 1) / BytesPerClaster; //Суммарный число выходных кластеров
      numAllFi++;                                            //Число копируемых файлах
   }
   if(numAllFi == num_Sel) return 0;                         //Число копируемых файлах
   return Error1((Lan+81)->msg);                             //81, "Неопознанная ошибка работы со списком");
}

//------------------------------------------------------------------------------

static int CopySelect(void)                                  //Копирование выбранных файлов
{
   char Name_Dir[260];
   if(Get_Name_Dir((Lan+94)->msg, Name_Dir, 0) < 0) return -1;//Запрос имени папки // if(Get_Name_Dir("Укажите диск или папку для записи", Name_Dir) < 0) return -1;  //Запрос имени папки
   UpdateWindow(MainWin);                                    //Восстановили окно после диалога выбора файла
   InitProgressBar = InitProgressBar1_2;                     //Функция прогресса для одного файла из группы
   ProgressBar = ProgressBar2;
   Close_ProgressBar = Close_ProgressBar1_2;

   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT], Path[MAXPATH];
   fnsplit(Name_Dir, Driv, Dir, Name, Ext);
   lstrcpy(Path, Driv);
   lstrcat(Path, "\\");
   DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
   if(GetDiskFreeSpace(Path, &SectorsPerCluster, &BytesPerSector,
                       &NumberOfFreeClusters, &TotalNumberOfClusters) == 0)
      return ErrorSys1((Lan+46)->msg);                       //return ErrorSys1("Системная ошибка при запросе емкости диска.");
   BytesPerClaster = SectorsPerCluster * BytesPerSector;     //Число байт в кластере
   numAllInCl = 0;                                           //Суммарное число кластеров во всех копируемых файлах
   numAllFi = 0;                                             //Число копируемых файлов
   num_T_Fi = 0;                                             //Номер текущего копируемого файла
   numAllOutCl = 0;                                          //Суммарный число выходных кластеров
   int l = lstrlen(Name_Dir) - 1;
   if(*(Name_Dir + l) == '\\') *(Name_Dir + l) = 0;          //Для имени диска косая всегда есть
   if(Calc_SizeAllFile() < 0) return -1;                     //Вычисления для копирование выбранных файлов на диск ПК
   DWORDLONG allSize = numAllOutCl * BytesPerClaster;
   int ret = DiskFreeSpaceEx_F(Name_Dir, allSize);           //Проверка свободного места
   if(ret < 0) return -1;
   if(ret == 1) return CopySelect();                         //Копирование выбранной файлов на диск ПК (повторный запуск для запроса нового пути)
   InitProgressBar2_2(numAllInCl, (Lan+122)->msg);           //Функция прогресса для нескольких файлов // if(Get_Name_Dir("Укажите диск или папку для записи", Name_Dir) < 0) return -1;  //Запрос имени папки
   ret = Copy_AllFiles(Name_Dir);                            //Копирование выбранных файлов на диск ПК
   Close_ProgressBar2_2();
   return ret;
}

//------------------------------------------------------------------------------

int  CopyFiles(void)                                         //Выполнение копирования
{
   TV_ITEM item;
   char Msg[256];
   prCopy = 0;                                               //Признак действия при групповом копировании
   if(num_Sel > 0) return CopySelect();                      //Число выбранных имен при групповом выборе
   HTREEITEM ind = TreeView_GetSelection(hwndTree);
   if(ind == NULL)
     return Error1((Lan+32)->msg);                           //return Error1("Нет имени выбранного для копирования.");
   item.mask = TVIF_TEXT | TVIF_PARAM;
   item.hItem = ind;
   item.pszText = Msg;
   item.cchTextMax = 256;
   if(TreeView_GetItem(hwndTree, &item) == FALSE)            //Взяли текущий элемент дерева
      return Error1((Lan+33)->msg);                          //return Error1("Ошибка при запросе информации об элементе дерева.");
   if((aTree + item.lParam)->pf.type == 48 ||                //Текущий выбор это папка
      (aTree + item.lParam)->pf.type == 47)                  //Текущий выбор это папка многофайлового Title
      return Copy_Folder(ind, (aTree + item.lParam)->NameF); //Копирование выбранной папки на диск ПК
   return Copy_File1(item.lParam);                           //Копирование выбранного файла на диск ПК
}

#endif

