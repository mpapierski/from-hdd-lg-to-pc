
//                                    19.08.2008
#ifndef __HDD_LG_MK_NAME__
#define __HDD_LG_MK_NAME__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_mk_name ==================================
int  MakeOneStrForTree(char *oneStr, char *Name, char *Ext, PAR_FILE *pf);//Создание строки символов для отражения в дереве
int  ViewAbsendName(void);                                   //Показ имен присутствующих в файле MME.DB, но отсутствующих в каталоге

static int nErr1 = 0, nErr2 = 0, nErr3 = 0, nErr4 = 0;       //Для ограничения числа сообщений об ошибке
static char *strSize[] = { "", "Kb", "Mb", "Gb" };

//------------------------------------------------------------------------------

static int Ctrl_Get_Size_From_FAT(DWORD clSt, LONGLONG *SizeFromFAT)//Вычисление максимальной длины файла по FAT
{
   DWORD nCl = clSt;                                         //Текущий номер кластера равен первому кластеру файла
   DWORD numCl = 0;                                          //Число кластеров прописанное в FAT для данного файла
/*
   for(;; numCl++)                                           //Идем по цепочке кластеров
   {  if(nCl == 0x0FFFFFFF) break;                           //Найден признак конца цепочки FAT
      if(*(FAT + nCl) == 0) return -1;                       //Обрыв цепочки кластеров
      nCl = *(FAT + nCl);                                    //Номер следующего кластера
      if(nCl > max_ZapFAT && nCl != 0x0FFFFFFF) return -2;   //Номер кластера в цепочке FAT превышает допустимое значение
   }
   *SizeFromFAT =  LONGLONG(numCl) * sCl_B;                  //Максимально возможный размер файла для данного числа кластеров
   return 0;
*/
   for(;; numCl++)                                           //Идем по цепочке кластеров
   {  if(nCl == 0x0FFFFFFF) break;                           //Найден признак конца цепочки FAT
      if(*(FAT1 + nCl) == 0) return -1;                      //Обрыв цепочки кластеров
      nCl = *(FAT1 + nCl);                                   //Номер следующего кластера
      if(nCl > maxZapFAT1 && nCl != 0x0FFFFFFF) return -2;   //Номер кластера в цепочке FAT превышает допустимое значение
     *SizeFromFAT =  LONGLONG(numCl) * sCl_B;                //Максимально возможный размер файла для данного числа кластеров
   }
   *SizeFromFAT =  LONGLONG(numCl+1) * sCl_B;                //Максимально возможный размер файла для данного числа кластеров
   return 0;
}

//------------------------------------------------------------------------------

static int ChangeNameInMEDIA(char *nam, char *ext, PAR_FILE *pf)//Изменние имени в папке MEDIA
{
   int prF = -1;                                             //Тип файла
   if(lstrcmp(ext, "idx") == 0) prF = 0;
   if(lstrcmp(ext, "str") == 0) prF = 1;                     //
   if((Conf.altName == 0 || Conf.ViewIDX == 0) && prF == 0) return 1; //Не показывать файлы  *.idx
   if((Conf.altName == 0 || Conf.Ren_STR == 0) && prF == 1)  //Менять расширение файлов *.str
       lstrcpy(ext, "vro");
   (aTree + numEl_Tree)->numPart = 0;                        //Номер части многофайлового Title
   (aTree + numEl_Tree)->indTitle = 0xFFFF;
   (aTree + numEl_Tree)->indFolder = 0;                      //Ссылка на имя папки
   (aTree + numEl_Tree)->numCopy = 0;                        //Счетчик скопированных частей
   (aTree + numEl_Tree)->prSel = 0;                          //Признак выбора данного файла(0-не выбран)

   indTabMME = -1;                                           //Индекс в таблице MME.DB для текущего файла в папке MEDIA
   if(Conf.altName == 0 && tabMME != NULL)                   //Вывод информативного имени и есть таблица имен из MME.DB
   {  for(DWORD n=0; n<numNam; n++)                          //По числу имен в таблице по MME.DB
        if(lstrcmp(nam, (tabMME + n)->NameF) == 0)           //Сравнение имен из каталога и из файла базы. Имена совпали
        {  (aTree + numEl_Tree)->numPart = (tabMME + n)->numPart;//Номер части многофайлового Title
           (aTree + numEl_Tree)->indTitle = (tabMME + n)->indTitle;
           if(pf->SizeF == (tabMME + n)->SizeF)              //Сверить длины файлов
           {  indTabMME = n;                                 //Индекс в таблице по MME.DB для текущего файла в папке MEDIA
              (tabMME + n)->prYes = 1;                       //Признак, что имя парное (1 - есть и в MME.DB и в каталоге)
              return 0;                                      //Файл найден
           }
           else                                              //Имя нашли, но длины файлов отличаются
           {  LONGLONG SizeFromFAT;
              if(Ctrl_Get_Size_From_FAT(pf->ClSt, &SizeFromFAT) < 0 ||//Вычисление максимальной длины файла по FAT
                 pf->SizeF > SizeFromFAT)
              {  pf->SizeF = SizeFromFAT;//0;                //У файлов с нарушениями в FAT принимаем длину == 0
                 if(nErr2 == 0)                              //Сообщения об ошибке еще небыло
                   Error3((Lan+138)->msg, (Lan+139)->msg, (Lan+152)->msg);
                 nErr2++;                                    //Чтобы не дублировать сообщения
                 pf->type = 34;                              //Признак для показа специальной иконки E2
              }
              else
              {  if(nErr3 == 0)                              //Сообщения об ошибке еще небыло
                   Error3((Lan+141)->msg, (Lan+142)->msg, (Lan+143)->msg);
                 nErr3++;                                    //Чтобы не дублировать сообщения
                 pf->type = 33;                              //Признак для показа специальной иконки e1
              }
              indTabMME = n;                                 //Индекс в таблице MME.DB для текущего файла в папке MEDIA
              (tabMME + n)->prYes = 1;                       //Признак, что имя парное (1 - есть и в MME.DB и в каталоге)
              return 0;
           }
        }
      //Прошли все и вышли из цикла и этого недолжно быть
      if(nErr1 == 0)                                         //Сообщения об ошибке еще небыло
        Error4((Lan+108)->msg, (Lan+102)->msg, (Lan+103)->msg, (Lan+104)->msg);
      nErr1++;                                               //Чтобы не дублировать сообщения
   }
   //Если нет файла MME.DB или его использование отключено (признак в конфиге)
   if(Conf.SwapNPart == 0)                                   //Включена перестановка номера части в конец имени файла
   {  char nPart[8];
      int iPart;
      MoveMemory(nPart, nam, 4);                             //Переслали символы счетчика
      *(nPart + 4) = 0;
      sscanf(nPart, "%X", &iPart);
      strncpy(nam, nam + 4, 42);                             //Переслали (сдвинули) имя файла без счетчика (предельная длина)
      if(Conf.ChangeNul == 0)// && prF == 1)                 //Включена замена 0 на _ в счетчике частей
      {  for(int i=0; i<4; i++)
         {  if(*(nPart + i) != '0') break;                   //Встретили номер
            if(*(nPart + i) == '0') *(nPart + i) = '_';
         }
         if(pf->lL2 != 0 && *(nPart + 3) == '_') *(nPart + 3) = '0'; //Восстановили 0 для первой части если их много
      }
      if(!(Conf.NoViewOnePart == 0 && pf->lL2 == 0) ||       //0-не показыватьномер для одной части
         prF == 0 ||                                         //Это не файл *.std (*.vro)
         iPart != 0)                                         //Есть номер части
      {  lstrcat(nam, ".");                                  //Добавили точку к имени
         strncat(nam, nPart, 4);                             //Добавили номер части
      }
      LONGLONG SizeFromFAT;
      if(Ctrl_Get_Size_From_FAT(pf->ClSt, &SizeFromFAT) < 0 ||//Вычисление максимальной длины файла по FAT
         pf->SizeF > SizeFromFAT)
      {  pf->SizeF = SizeFromFAT;//0;                        //У файлов с нарушениями в FAT принимаем длину == 0
         if(nErr2 == 0)                                      //Сообщения об ошибке еще небыло
           Error3((Lan+138)->msg, (Lan+139)->msg, (Lan+152)->msg);
         nErr2++;                                            //Чтобы не дублировать сообщения
         pf->type = 34;                                      //Признак для показа специальной иконки E2
      }
   }
   return 0;
}

//------------------------------------------------------------------------------

static void MkInformName(char *Name, char *Ext)
{
   if(indTabMME < 0) return;                                 //Не найдено соответствие имен
   TABL_MME *tMME = tabMME + indTabMME;                      //Строка таблицы относящяеся к данному имени
   int l = wsprintf(Name, "%s", tMME->aName);
   if(Conf.poz_In == 1)                                      //Источник записи: 0-в колонке, 1-в имени, 2-не показывать
     l += wsprintf(Name + l, ".%s", tMME->nameIn);
   if(Conf.poz_Ql == 1)                                      //Качество записи: 0-в колонке, 1-в имени, 2-не показывать
     l += wsprintf(Name + l, ".%s", tMME->Qual);
   if(tMME->numPart > 0)                                     //Если больше одной части
     sprintf(Name + l, ".%03d", tMME->numPart);              //Добавили номер части
   lstrcpy(Ext, "vro");
}

//------------------------------------------------------------------------------

int MakeOneStrForTree(char *oneStr, char *Name, char *Ext, PAR_FILE *pf)//Создание строки символов для отражения в дереве
{
   char size[32], *as;
   if(pf->type == 48)                                        //Это имя папки
   {  sprintf(oneStr, "%s", Name);                           //Для папки ничего не меняем
      return 0;
   }
   if(prMEDIA == 1)                                          //Папка MEDIA
      if(ChangeNameInMEDIA(Name, Ext, pf) == 1) return 1;    //Изменние имени в папке MEDIA
   if(prMEDIA == 1 && Conf.altName == 0 && tabMME != NULL)
      MkInformName(Name, Ext);                               //Формирование информативного имени
   if(*Ext != 0)                                             //Добавили расширение если оно есть
   {  lstrcat(Name, ".");
      lstrcat(Name, Ext);
   }
   int l = sprintf(oneStr, "  %s", Name);
   for(int i=l; i<256; i++)                                  //Заполнили хвост строки пробелами
       *(oneStr + i) = ' ';
   switch(Conf.typeSize)                                     //0-байты, 1-Кбайты, 2-Мбайты, 3-ГБайты
   {  case 0: sprintf(size, "%12.0lf", double(pf->SizeF));
              as = Char_Dig_p(size, 15);  break;             //Преобразование символьного числа в разрядку
      case 1: sprintf(size, "%11.1lf", double(pf->SizeF)/1024.0);
              as = Char_Dig_p_n(size, 12, 1);  break;        //Преобразование символьного числа в разрядку
      case 2: sprintf(size, "%9.2lf", double(pf->SizeF)/1024.0/1024.0);
              as = Char_Dig_p_n(size, 9, 2);  break;         //Преобразование символьного числа в разрядку
      case 3: sprintf(size, "%7.3lf", double(pf->SizeF)/1024.0/1024.0/1024.0);
              as = Char_Dig_p_n(size, 6, 3);  break;         //Преобразование символьного числа в разрядку
   }
/*
   if(prMEDIA == 1 && Conf.altName == 0 && tabMME != NULL)
     if((tabMME + indTabMME)->dt.year !=  pf.Year ||
        (tabMME + indTabMME)->dt.mon  !=  pf.Mon  ||
        (tabMME + indTabMME)->dt.day  !=  pf.Day  ||
        (tabMME + indTabMME)->dt.hour !=  pf.Hour ||
        (tabMME + indTabMME)->dt.min  !=  pf.Min  ||
        (tabMME + indTabMME)->dt.sec  !=  pf.Sec)
     {
        char sf[260], sb[260], ss[260];
        wsprintf(sf, "В файле: %02d.%02d.%04d  %02d:%02d:%02d", pf.Day, pf.Mon, pf.Year, pf.Hour, pf.Min, pf.Sec);
        wsprintf(sb, "В базе:    %02d.%02d.%04d  %02d:%02d:%02d",
                    (tabMME + indTabMME)->dt.day, (tabMME + indTabMME)->dt.mon, (tabMME + indTabMME)->dt.year,
                    (tabMME + indTabMME)->dt.hour, (tabMME + indTabMME)->dt.min, (tabMME + indTabMME)->dt.sec);
        WORD wHour = WORD((tabMME + indTabMME)->timeLong / 3600);
        int M1 = (tabMME + indTabMME)->timeLong - wHour * 3600;
        WORD wMinute = WORD(M1 / 60);
        WORD wSecond = WORD(M1 - wMinute * 60);
        wsprintf(ss, "Длительность: %02d:%02d:%02d", wHour, wMinute, wSecond);
        Error4("Несовпадение дат:", ss, sb, sf);
     }
*/
   char sDate[64], sTime[64], Pr = ' ';
   SYSTEMTIME sysTime;
   if(prMEDIA == 1 && Conf.altName == 0 &&                   //Это папка MEDIA и есть признак информативного имени
      tabMME != NULL && *(DWORD*)Ext == 0x006F7276 &&        //Есть массив MME и файл с расширением *.vro
      Conf.typeTime == 0 &&                                  //0-показывать время начала записи, 1-время создания файла
      indTabMME != -1)                                       //Не найдено соответствие имен
   {  pf->Year = (tabMME + indTabMME)->dt.year;
	  pf->Mon = BYTE((tabMME + indTabMME)->dt.mon);
	  pf->Day = BYTE((tabMME + indTabMME)->dt.day);
	  pf->Hour = BYTE((tabMME + indTabMME)->dt.hour);
	  pf->Min = BYTE((tabMME + indTabMME)->dt.min);
	  pf->Sec = BYTE((tabMME + indTabMME)->dt.sec);
      Pr = 'R';
   }
   sysTime.wYear = (pf->Year == 0) ? WORD(1900) : pf->Year;
   sysTime.wMonth = (pf->Mon == 0) ? WORD(1) : pf->Mon;
   sysTime.wDay = (pf->Day == 0) ? WORD(1) : pf->Day;
   sysTime.wHour = pf->Hour;
   sysTime.wMinute = pf->Min;
   sysTime.wSecond = pf->Sec;
   sysTime.wDayOfWeek = 1;
   sysTime.wMilliseconds = 0;

   GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, sDate, 64);
   GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, &sysTime, NULL, sTime, 64);
/*
   int prNoDT = 0;                                           //Признак отсутствия даты/времени (часы не установлены)
   if((sysTime.wYear == 2004 && sysTime.wMonth == 1 && sysTime.wDay == 1) ||
       sysTime.wYear >= 2055 || sysTime.wYear < 2004) prNoDT = 1;                 //Признак отсутствия даты/времени (часы не установлены)
   if(prNoDT == 1)                                           //Признак отсутствия даты/времени (часы не установлены)
   {  for(int i=0; i<sizeof(sDate); i++)
      {  if(*(sDate+i) == 0)  break;
         if(*(sDate+i) >= '0' && *(sDate+i) <= '9') *(sDate+i) = '-';
      }
      for(int i=0; i<sizeof(sTime); i++)
      {  if(*(sTime+i) == 0)  break;
         if(*(sTime+i) >= '0' && *(sTime+i) <= '9') *(sTime+i) = '-';
      }
   }
*/
   if(prMEDIA == 1 && Conf.altName == 0 &&                   //Это папка MEDIA и есть признак информативного имени
      tabMME != NULL && *(DWORD*)Ext == 0x006F7276 &&        //Есть массив MME и файл с расширением *.vro
      indTabMME != -1)                                       //Не найдено соответствие имен
   {  sysTime.wHour = WORD((tabMME + indTabMME)->timeLong / 3600);
      int M1 = (tabMME + indTabMME)->timeLong - sysTime.wHour * 3600;
      sysTime.wMinute = WORD(M1 / 60);
      sysTime.wSecond = WORD(M1 - sysTime.wMinute * 60);
      sysTime.wMilliseconds = 0;
      char sLTime[64];
      GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, &sysTime, NULL, sLTime, 64);
      if(Conf.ViewVRO == 1)                                  //0-показыват расширение *.vro, 1-не показывать
         for(int i=l-4; i<l; i++)                            //Стерли расширение .vro и точку
             *(oneStr + i) = ' ';
      int n = (Conf.poz_Ql == 0) ? 59 : 63;
      if(Conf.poz_In == 0)                                   //Источник записи: 0-в колонке, 1-в имени, 2-не показывать
         sprintf(oneStr + n,  "%s        ", (tabMME + indTabMME)->nameIn);
      if(Conf.poz_Ql == 0)                                   //Качество записи: 0-в колонке, 1-в имени, 2-не показывать
         sprintf(oneStr + 66, "%s        ", (tabMME + indTabMME)->Qual);
      sprintf(oneStr+70, "%s %s  [%8s]  %c %10s  %8s", as, strSize[Conf.typeSize], sLTime, Pr, sDate, sTime);
   }
   else  sprintf(oneStr + 52, "%s %s  %10s  %8s", as, strSize[Conf.typeSize], sDate, sTime);
   return 0;
}

//------------------------------------------------------------------------------

static int MkAbsendName(char *oneStr, char *Name, PAR_FILE *pf)//Создание строки символов для отражения в дереве для отсуттвующего имени
{
   char size[32], *as;
   if(pf->type == 48)                                        //Это имя папки
   {  sprintf(oneStr, "%s", Name);                           //Для папки ничего не меняем
      return 0;
   }
   char Ext[8];
   MkInformName(Name, Ext);                                  //Формирование информативного имени
   int l = sprintf(oneStr, "  %s", Name);
   for(int i=l; i<256; i++)                                  //Заполнили хвост строки пробелами
       *(oneStr + i) = ' ';
   switch(Conf.typeSize)                                     //0-байты, 1-Кбайты, 2-Мбайты, 3-ГБайты
   {  case 0: sprintf(size, "%12.0lf", double(pf->SizeF));
              as = Char_Dig_p(size, 15);  break;             //Преобразование символьного числа в разрядку
      case 1: sprintf(size, "%11.1lf", double(pf->SizeF)/1024.0);
              as = Char_Dig_p_n(size, 12, 1);  break;        //Преобразование символьного числа в разрядку
      case 2: sprintf(size, "%9.2lf", double(pf->SizeF)/1024.0/1024.0);
              as = Char_Dig_p_n(size, 9, 2);  break;         //Преобразование символьного числа в разрядку
      case 3: sprintf(size, "%7.3lf", double(pf->SizeF)/1024.0/1024.0/1024.0);
              as = Char_Dig_p_n(size, 6, 3);  break;         //Преобразование символьного числа в разрядку
   }
   char sDate[64], sTime[64], Pr = 'R';
   SYSTEMTIME sysTime;
   pf->Year = (tabMME + indTabMME)->dt.year;
   pf->Mon = BYTE((tabMME + indTabMME)->dt.mon);
   pf->Day = BYTE((tabMME + indTabMME)->dt.day);
   pf->Hour = BYTE((tabMME + indTabMME)->dt.hour);
   pf->Min = BYTE((tabMME + indTabMME)->dt.min);
   pf->Sec = BYTE((tabMME + indTabMME)->dt.sec);
   sysTime.wYear = (pf->Year == 0) ? WORD(1900) : pf->Year;
   sysTime.wMonth = (pf->Mon == 0) ? WORD(1) : pf->Mon;
   sysTime.wDay = (pf->Day == 0) ? WORD(1) : pf->Day;
   sysTime.wHour = pf->Hour;
   sysTime.wMinute = pf->Min;
   sysTime.wSecond = pf->Sec;
   sysTime.wDayOfWeek = 1;
   sysTime.wMilliseconds = 0;

   GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, sDate, 64);
   GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, &sysTime, NULL, sTime, 64);
   sysTime.wHour = WORD((tabMME + indTabMME)->timeLong / 3600);
   int M1 = (tabMME + indTabMME)->timeLong - sysTime.wHour * 3600;
   sysTime.wMinute = WORD(M1 / 60);
   sysTime.wSecond = WORD(M1 - sysTime.wMinute * 60);
   sysTime.wMilliseconds = 0;
   char sLTime[64];
   GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, &sysTime, NULL, sLTime, 64);
   int n = (Conf.poz_Ql == 0) ? 59 : 63;
   if(Conf.poz_In == 0)                                   //Источник записи: 0-в колонке, 1-в имени, 2-не показывать
      sprintf(oneStr + n,  "%s        ", (tabMME + indTabMME)->nameIn);
   if(Conf.poz_Ql == 0)                                   //Качество записи: 0-в колонке, 1-в имени, 2-не показывать
      sprintf(oneStr + 66, "%s        ", (tabMME + indTabMME)->Qual);
   sprintf(oneStr+70, "%s %s  [%8s]  %c %10s  %8s", as, strSize[Conf.typeSize], sLTime, Pr, sDate, sTime);
   return 0;
}

//------------------------------------------------------------------------------

static int AddAbsendNameToTree(PAR_FILE *pf, int Level)      //Добавление новой строки в дерево
{
   TV_INSERTSTRUCT tvins;
   char oneStr[256], Name[256];

   MkAbsendName(oneStr, Name, pf);                           //Создание строки символов для отражения в дереве для отсуттвующего имени

   lstrcpy((aTree + numEl_Tree)->NameF, Name);
   (aTree + numEl_Tree)->pf = *pf;
   (aTree + numEl_Tree)->numPart = 0;                        //Номер части многофайлового Title
   (aTree + numEl_Tree)->indTitle = 0xFFFF;
   (aTree + numEl_Tree)->indFolder = 0;                      //Ссылка на имя папки
   (aTree + numEl_Tree)->numCopy = 0;                        //Счетчик скопированных частей
   (aTree + numEl_Tree)->prSel = 0;                          //Признак выбора данного файла(0-не выбран)

   tvins.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   tvins.item.pszText = oneStr;
   tvins.item.lParam = numEl_Tree;
   tvins.item.iImage = tvins.item.iSelectedImage = 9;        //Индекс иконки
   if(Conf.PrSort == 0)                                      //0-сортировка по имени, 1-без сортировки
      tvins.hInsertAfter = TVI_SORT;
   else
      tvins.hInsertAfter = TVI_LAST;
   if(Level == 0) tvins.hParent = NULL;
   else tvins.hParent = hPrev[Level-1];
   hPrev[Level] = TreeView_InsertItem(hwndTree, &tvins);
   if(++numEl_Tree > MAX_NAME)
      return Error1((Lan+31)->msg);                          //return Error2("Суммарное число имен папок и файлов превышает возможности программы.");
   return 0;
}

//------------------------------------------------------------------------------

int  ViewAbsendName(void)                                    //Показ имен присутствующих в файле MME.DB, но отсутствующих в каталоге
{
   PAR_FILE pf;

   if(tabMME == NULL) return 0;                              //Таблица отсутствует
   indTabMME = -1;                                           //Индекс в таблице MME.DB для текущего файла в папке MEDIA
   for(DWORD n=0; n<numNam; n++)                             //По числу имен в таблице MME
   {   if((tabMME + n)->prYes == 1) continue;                //Признак, что имя парное (1 - есть и в mme.db и в каталоге)
       if(nErr4 == 0)                                        //Сообщения об ошибке еще небыло
           Error3((Lan+144)->msg, (Lan+145)->msg, (Lan+140)->msg);
       nErr4++;                                              //Чтобы не дублировать сообщения
       indTabMME = n;
       pf.type = 35;                                         //Признак специальной иконки  (e3)
       pf.SizeF = 0;
       AddAbsendNameToTree(&pf, 2);                          //Добавление новой строки в дерево
   }
   return 0;
}

#endif

