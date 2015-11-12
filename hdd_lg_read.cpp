
//                              03.11.2009
#ifndef __HDD_LG_READ__
#define __HDD_LG_READ__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_read =====================================
int  Update_Tree(void);                                      //Пересоздание дерева файлов и папок при изменении настроек
int  Read_HDD(void);                                         //Поиск HDD из рекордера LG и загрузка служебной информации
     HANDLE hDrive;                                          //Указатель на диск
     DWORD *FAT1;                                            //Таблица FAT первого раздела
     DWORD Start_SecDir1;                                    //Позиция начала первого каталога первого раздела
     DWORD Start_SecFAT1;                                    //Позиция начала таблицы FAT первого раздела
     DWORD num_SecFAT1;                                      //Число секторов в FAT
     DWORD Size_FAT1;                                        //Число байт в FAT
     DWORD maxZapFAT1 = 0xFFFFFFFF;                          //Максимальное число записей в FAT
     DWORD *FAT2;                                            //Таблица FAT второго раздела
     DWORD Start_SecDir2;                                    //Номер сектора начала каталога во втором разделе
     DWORD Start_SecFAT2;                                    //Позиция начала таблицы FAT первого раздела
     DWORD num_SecFAT2;                                      //Число секторов в FAT
     DWORD Size_FAT2;                                        //Число байт в FAT второго раздела
     DWORD maxZapFAT2;                                       //Максимальное число записей в FAT второго раздела
     int   indTabMME;                                        //Индекс в таблице MME.DB для текущего файла в папке MEDIA
     DWORD writeCl;                                          //Число занятых кластеров
     int prMEDIA;                                            //Признак обрабатываемой папка
     int pr_tRec;                                            //Признак рекордера 0 или 1 (старейшая серия)

static int Read_One_Dir(DWORD nClast, int pr);               //Чтение и визуализация одного каталога
typedef struct { DWORD nCl; char  nam[48]; } FOLDER_INFO;
#define MAX_FOLDER 4                                         //Максимальное число вложенных папок для рекордера типа 1
static FOLDER_INFO F_Inf[MAX_FOLDER];                        //Информаци ядля вложенных папок для рекордера типа 1
static int numFolder;                                        //Число вложенных папок для рекордера типа 1
static int nCl_3Cat;                                         //Номер кластера с тремя главными каталогами

//------------------------------------------------------------------------------

static int CreateName(char *nam, char *ext, One_Str_Cat *Kat)//Формирование имени файла
{
   if(pr_tRec == 0)                                          //Признак рекордера 0 или 1 (старейшая серия)
   {  UnicodeToAnsi(Kat->Name, nam, 41);                     //Преобразовали имя из UNICODE
      UnicodeToAnsi(Kat->Ext, ext, 4);                       //Преобразовали расширение из UNICODE
   }
   if(pr_tRec == 1)                                          //Признак рекордера 0 или 1 (старейшая серия)
   {  UnicodeToAnsi(Kat->Name, nam, 45);                     //Преобразовали имя и расширение из UNICODE
      lstrcpy(ext, nam + 42);                                //Выбрали расширение
      if(*ext == ' ') *ext = 0;                              //Если расширения нет, то очистили
      for(int i=41; i>0; i--)                                //Очистили хвост имени от пробелов остави только символы
      {  if(*(nam + i) != ' ') break;                        //Поставили конец имени после последнего символа
         *(nam + i) = 0;
      }
   }
   if(nCl_3Cat == -1 &&  prMEDIA == 1 && Kat->pf.type == 48 &&
      lstrcmp(nam, "MEDIA") == 0 && *ext == 0)
   {  nCl_3Cat = Kat->pf.ClSt;                               //Номер кластера с тремя главными каталогами
      return 1;                                              //Пропускаем папку с названием MEDIA
   }
/*
   if(prM == 2 && n == 2 && lstrcmp(nam, "JUKEBOX") != 0)
     return Error2((Lan+96)->msg, (Lan+100)->msg);           //return Error2("Нарушена структура каталога.", "Не найдена папка MEDIA.");
   if(prM == 2 && n == 3 && lstrcmp(nam, "VIDEO") != 0)
     return Error2((Lan+96)->msg, (Lan+101)->msg);           //return Error2("Нарушена структура каталога.", "Не найдена папка MEDIA.");
   if(prM == 2 && n == 4 && lstrcmp(nam, "PHOTO") != 0)
       return Error2((Lan+96)->msg, (Lan+102)->msg);         //return Error2("Нарушена структура каталога.", "Не найдена папка MEDIA.");
*/
   if(Kat->pf.type == 48)                                    //Это имя папки
      if(Kat->pf.SizeF != 0)
         return Error1((Lan+16)->msg);                       //return Error1("Строка каталога опознанная как папка имеет размерность в байтах.");
   return 0;
}

//------------------------------------------------------------------------------

static const DWORD Point1[] = { 0x00002E00, 0x20002E00 };    //Код точки (у старейшая серия вместо нуля - пробелы)

static int View_Dir(BYTE *buff, int prM)                     //Вывод содержимого каталога
{
   One_Str_Cat *Kat = (One_Str_Cat *)buff;
   int prPoint = 0;                                          //Признак наличия каталогов с точкой как признак правильности структуры
   prMEDIA = prM;                                            //Признак обрабатываемой папка
   if(prM == 0) prMEDIA = 1;                                 //Коррекция для второго кластера папки MEDIA
   for(DWORD n=0; n<4*sClSec; n++)                             //В каждом секторе 4 записи
   {  if((Kat + n)->pf.type == 0) return 1;                  //Конец каталога
      if((Kat + n)->pf.type != 32 && (Kat + n)->pf.type != 48)
        return Error1((Lan+15)->msg);                        //return Error1("Неизвестный тип записи в строке каталога.");
      DWORD NamePoint = *((DWORD*)&(Kat + n)->Name);
      if(NamePoint == *(Point1 + pr_tRec))                   //Это одна точка т.е. кластер текущего каталога
      { if(n == 0) prPoint++; continue; }
      if(NamePoint == 0x2E002E00)                            //Это две точки т.е. кластер предыдущего каталога
      { if(n == 1) prPoint++; continue; }
      if(prM > 1 && prPoint != 2)                            //Все каталоги кроме MEDIA должны начинаться с точки и двух точек
        return Error1((Lan+96)->msg);                        //return Error1("Нарушена структура каталога.");
      WORD NameDel = *((WORD*)&(Kat + n)->Name);
#if !defined VIEW_DEL                                        //Показ удаленных файлов и папок
      if(NameDel == 0xE500 && (Kat + n)->pf.ClSt == 0) continue; //Это удаленное имя
#else
//      *((WORD*)&(Kat + n)->Name) = 0xE600;
#endif
      writeCl += DWORD(((Kat + n)->pf.type == 48) ? 1 : ((Kat + n)->pf.SizeF + sCl_B - 1) / sCl_B);
      char nam[128], ext[128];
      int ret = CreateName(nam, ext, Kat + n);               //Формирование имени файла
      if(ret == 1) continue;                                 //Этото имя нас не интересует  //Пропускаем папку с названием MEDIA
      if(ret < 0) return -1;
      int lPr;                                               //Признак уровня дерева
      if(prM <= 1)                                           //Каталог самого первого кластера (MEDIA или JUKEBOX)
      {  if(pr_tRec == 0)                                    //Признак рекордера 0 или 1 (старейшая серия)
         {  //??if(n == 0) lPr = 1;                              //Каталог первого уровня MEDIA (имя MEDIA записано в первой строке)
            //??else       lPr = 2;                              //Вложенные файлы в папке MEDIA
            //??if(prM == 0) lPr = 2;                            //Для второго кластера папки MEDIA всегда второй уровень
            lPr = 2;                                         //Вложенные файлы в папке MEDIA
         }
         if(pr_tRec == 1)                                    //Признак рекордера 0 или 1 (старейшая серия)
         {  if((Kat + n)->pf.type == 48)                     //Каталог первого уровня в самом первом кластере
            {  (F_Inf + numFolder)->nCl = (Kat + n)->pf.ClSt;//Сохранили информацию по вложенной папке
               lstrcpy((F_Inf + numFolder)->nam, nam);
               if(numFolder+1 >= MAX_FOLDER)                 //Номер вложенной папки
               {  Error1((Lan+99)->msg);                     //Error1("Недопустимое число папок верхнего уровня.");
                  continue;
               }
               numFolder++;                                  //Номер вложенной папки
               continue;
            }
            else  lPr = 2;                                   //Вложенные файлы в папке MEDIA
         }
      }
      else                                                   //Второй и последующий кластер с каталогами
        if(prM == 2) lPr = 1;                                //Остальные три главных каталога
        else lPr = prM - 1;                                  //Признак уровня вложенности папок и файлов
      if(AddItemToTree(nam, ext, &((Kat + n)->pf), lPr) < 0) return -1; //Добавили строку в дерево
      if(prM > 1)                                            //Это не Каталог MEDIA и значит могут быть вложенные папка
        if((Kat + n)->pf.type == 48)                         //Это имя вложенной папки
           if(Read_One_Dir((Kat + n)->pf.ClSt, prM+1) < 0) return -1;  //Чтение и визуализация одной вложенной папки
//??  if((Kat + n)->pf.Type == 32)                           //Это имя файла
//??      Ctrl_FAT_For_File((Kat + n)->pf.ClSt, (Kat + n)->pf.SizeF); //Проверка соответствия длины файла и FAT
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir_(BYTE *buff, DWORD nClast, int pr)   //Чтение и визуализация одного каталога
{
   DWORD nSector = Start_SecDir1 + (nClast - 1) * sClSec;    //Номер сектора по номеру кластера
   if(ReadClast1_P(nSector, buff) < 0) return -1;            //Чтение кластера
#if defined OUT_TEST
   View_HEX_Any(buff, sCl_B);
#endif
   if(pr == 1)                                               //Это самай первый каталог MEDIA
   {  if(*(DWORDLONG*)buff      == 0 && *(DWORDLONG*)(buff+ 8) == 0 &&//Проверка наличия имени
         *(DWORDLONG*)(buff+16) == 0 && *(DWORDLONG*)(buff+24) == 0 &&//и если имени нет, то ошибка
         *(DWORDLONG*)(buff+32) == 0 && *(DWORDLONG*)(buff+40) == 0)
        return Error1((Lan+101)->msg);                       //return Error1("На жестком диске не обнаружено ни одной записи.");
      pr_tRec = 0;                                           //Признак рекордера 0 или 1 (старейшая серия)
      DWORDLONG *dwBuf = (DWORDLONG*)buff;
      for(DWORD n=0; n<4*sClSec; n++)                          //В каждом секторе 4 записи (просмотр всего кластера)
	  {  if(*dwBuf == 0x4900440045004D00i64) break;             //Признак рекордера 0 или 1      //MEDIA
		 if(*dwBuf == 0x45004B0055004A00i64) { pr_tRec = 1; break; }//Признак рекордера 0 или 1 //JKEBOX
         if(*dwBuf == 0x0000000000000000i64) break;             //Признак конца записей
         dwBuf += 128 / sizeof(DWORDLONG);
      }
      if(pr_tRec == 1)
      {  if(Conf.WriteYes == 1)                              //Для старой серии никакой записи
         {  EnableWindow(hWriteFi, false);
            EnableWindow(hWriteFo, false);
            EnableWindow(hNew_Fo, false);
            EnableWindow(hRenFi, false);
            EnableWindow(hDelFi, false);
            EnableWindow(hCorr67, false);
         }
         Conf.WriteYes = 0;                                  //Для старой серии никакой записи
      }
      PAR_FILE pf;
      ZeroMemory(&pf, sizeof(PAR_FILE));
      pf.type = 48;                                          //Признак, что это папка
      AddItemToTree("MEDIA", NULL, &pf, 1);                  //Добавили строку в дерево (папка самого верхнего уровня)
   }
   if(View_Dir(buff, pr) < 0)                                //Вывод содержимого каталога
      Error2((Lan+97)->msg, (Lan+98)->msg);                  //Error2("Найдены ошибки в структуре записей на диске LG.". "Дерево папок и каталогов содержит неполную информацию.");
   if(nClast != 1 && *(FAT1 + nClast) != 0x0FFFFFFF)
   {  Error1((Lan+17)->msg);                                 //Error1("Размер одного каталога превышает размер кластера.");
      Error2((Lan+97)->msg, (Lan+98)->msg);                  //Error2("Найдены ошибки в структуре записей на диске LG.". "Дерево папок и каталогов содержит неполную информацию.");
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Read_One_Dir(DWORD nClast, int prM)               //Чтение и визуализация одного каталога
{
#if defined OUT_TEST
   DWORD nSector = Start_SecDir1 + (nClast-1) * sClSec;      //Номер сектора по номеру кластера
   char Ss[300];
   wsprintf(Ss, "Load Dir (claster %d, sector %d)", nClast, nSector);
   Add_SpecSpis(Ss);
#endif
   if(nClast == 0 || nClast >= maxZapFAT1)
      return Error1((Lan+100)->msg);                         //return Error1("Недопустимый номер кластера каталога.");
   if(nClast != 1 && *(FAT1 + nClast) == 0)
      return Error1((Lan+18)->msg);                          //return Error1("Обнаружено несоответствие значения FAT и ссылки на кластер каталога.");
   BYTE *buff = (BYTE *)MyAllocMem(sCl_B);                   //Память под число кластеров в каталоге
   if(buff == NULL)  return -1;
   int ret = Read_One_Dir_(buff, nClast, prM);               //Чтение и визуализация одного каталога
   MyFreeMem(&(void*)buff);
   return ret;
}

//------------------------------------------------------------------------------

static int Read_AllDir(void)                                 //Чтение всех каталогов
{
   nCl_3Cat = -1;                                            //Номер кластера с тремя главными каталогами
   numEl_Tree = 0;                                           //Число элементров в дереве имен
   num_Sel = 0;                                              //Число выбранных имен при групповом выборе
   size_Sel = 0;                                             //Суммарный размер выбранных файлов
   for(int i=0; i<MAX_U; i++)
     hPrev[i] = NULL;                                        //Массив уровней вложенности дерева
   PAR_FILE pf;
   ZeroMemory(&pf, sizeof(PAR_FILE));
   pf.type = 48;                                             //Признак, что это папка
   AddItemToTree("HDD_LG", NULL, &pf, 0);                    //Добавили строку в дерево (папка самого верхнего уровня)
   numFolder = 0;                                            //Номер вложенной папки
   if(Read_One_Dir(1, 1) < 0) return -1;                     //Чтение и визуализация одного папки MEDIA
   if(stCl_Media2 != 0)                                      //Кластер начала второй части папки MEDIA
     if(Read_One_Dir(stCl_Media2, 0) < 0) return -1;         //Чтение и визуализация второй части папки MEDIA
#if !defined OUT_TEST
   ViewAbsendName();                                         //Показ имен присутствующих в файле MME.DB, но отсутствующих в каталоге
#endif
   if(pr_tRec == 0 && nCl_3Cat != -1)                        //Была папка MEDIA
     if(Read_One_Dir(nCl_3Cat, 2) < 0) return -1;            //Чтение и визуализация одного каталога (JUKEBOX, VIDEO и FOTO)
   if(pr_tRec == 1 && numFolder > 0)                         //Cтарейшая серия рекордеров
     for(int i=0; i<numFolder; i++)
     {  AddItemToTree((F_Inf+i)->nam, NULL, &pf, 1);         //Добавили строку в дерево (папка самого верхнего уровня)
        if(Read_One_Dir((F_Inf+i)->nCl, 2) < 0) return -1;   //Чтение и визуализация одного каталога (JUKEBOX, VIDEO и FOTO)
     }
   Expand_Tree(0);                                           //Распахнули дерево на первом (нулевом) уровне
   return 0;
}

//-------------------------------------------------------------------------------

static int My_Read_HDD(void)                                 //Поиск HDD из рекордера LG и загрузка служебной информации
{
#if defined EMULATOR_HDD
   if(FindHDD_LG_Emul() < 0) return -1;                      //Поиск диска из рекордера LG эмулятором
#else
   if(FindHDD_LG() < 0) return -1;                           //Поиск диска из рекордера LG (реального)
#endif
   if(Load_FAT1() < 0) return -1;                            //Загрузка FAT первого раздела
   if(Load_FAT2() >= 0)                                      //Загрузка FAT второго раздела
      ReadMME_DB();                                          //Чтение файла базы записей (только 3,0если найден FAT32)
   if(Read_AllDir() < 0) return -1;                          //Чтение всех каталогов
   if(Conf.altName == 0 && Conf.ViewPart == 0 && tabMME != NULL)//Вывод информативного имени, показ отредактированных глав как папок и есть таблица имен из MME.DB
     RemakeTree();                                           //Переформирование дерева с учетом многофайловых Titles
#if defined TEST_67_68_70_71                                 //Сектора
   if(Read_Sec_67_68_70_71() < 0) return -1;                 //Чтение специальных секторов
#endif
#if defined TEST_LG1
   if(Read_Test1() < 0) return -1;                           //Тестовое чтение и вывод
#endif
#if defined TEST_LG2_MME
   if(Read_Test2() < 0) return -1;                           //Тестовое чтение и вывод
#endif
   return 0;
}

//------------------------------------------------------------------------------

static int Update_Tree_(void)                                //Пересоздание дерева файлов и папок при изменении настроек
{
   TreeView_DeleteAllItems(hwndTree);                        //Очистка дерева для создания нового
   if(Read_AllDir() < 0) return -1;                          //Чтение всех каталогов
   if(Conf.altName == 0 && Conf.ViewPart == 0 && tabMME != NULL)//Вывод информативного имени, показ отредактированных глав как папок и есть таблица имен из MME.DB
     RemakeTree();                                           //Переформирование дерева с учетом многофайловых Titles
   return 0;
}

//------------------------------------------------------------------------------

int Update_Tree(void)                                        //Пересоздание дерева файлов и папок при изменении настроек
{
   writeCl = 0;                                              //Число занятых кластеров
   SetCursor(LoadCursor(NULL, IDC_WAIT));                    //Форма курсора (часы)
   int ret = Update_Tree_();                                 //Пересоздание дерева файлов и папок при изменении настроек
   SetWindowText(hNumSel, "");
   SetWindowText(hSizeSel, "");
   EnableWindow(hClear, false);
   SetCursor(LoadCursor(NULL, IDC_ARROW));                   //Форма курсора (стрелка)
   return ret;
}

//------------------------------------------------------------------------------

int Read_HDD(void)                                           //Поиск HDD из рекордера LG и загрузка служебной информации
{
   writeCl = 0;                                              //Число занятых кластеров
   aTree = (OneStrTree *)MyAllocMem(MAX_NAME * sizeof(OneStrTree));//Память под дерево
   if(aTree == NULL)  return -1;
#if !defined TEST_LG1 && !defined TEST_LG2_MME
   SetCursor(LoadCursor(NULL, IDC_WAIT));                    //Форма курсора (часы)
   Open_Wait_Found_HDD();                                    //Окошко со словами подождите
#endif
   int ret = My_Read_HDD();
#if !defined TEST_LG1 && !defined TEST_LG2_MME
   Close_Wait_Found_HDD();                                   //Окошко со словами подождите
   SetCursor(LoadCursor(NULL, IDC_ARROW));                   //Форма курсора (стрелка)
#endif
#if defined OUT_TEST
   Add_SpecSpis("End of dump");
#endif
#if defined FIND_NO_ZERO_SEC                                 //Поиск не нулевых секторов
   FindNoZeroSec();                                          //Поиск не нулевых секторов и создание списка
#endif
   ViewSize();                                               //Показ дискового пространства
   return ret;
}

#endif

