
//                                    11.08.2008
#ifndef __HDD_LG_REMAKE_TREE__
#define __HDD_LG_REMAKE_TREE__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_remake_tree ==============================
int  RemakeTree(void);                                       //Переформирование дерева с учетом многофайловых Titles

static char *strSize[] = { "", "Kb", "Mb", "Gb" };
static HTREEITEM hFOLDER;                                    //Вновь созданная папка
static WORD indTitle;                                        //Индекс Title

//------------------------------------------------------------------------------

static int MakeNameFolder(char *oneStr, DWORD ind)           //Создание имени специальной папки
{
   char size[32], *as;

   int m = (tabMME + (aTree + ind)->indTabMME)->indFolder;
   int l = wsprintf(oneStr, "  %s", (tabMME_Part + m)->aName);
   if(Conf.poz_In == 1)                                      //Источник записи: 0-в колонке, 1-в имени, 2-не показывать
     l += wsprintf(oneStr + l, ".%s", (tabMME_Part + m)->nameIn);
   if(Conf.poz_Ql == 1)                                      //Качество записи: 0-в колонке, 1-в имени, 2-не показывать
     l += wsprintf(oneStr + l, ".%s", (tabMME_Part + m)->Qual);
   l += wsprintf(oneStr + l, " [%d]", (tabMME_Part + m)->nPart);//Добавили число частей
   for(int i=l; i<256; i++)                                  //Заполнили хвост строки пробелами
       *(oneStr + i) = ' ';
   switch(Conf.typeSize)                                     //0-байты, 1-Кбайты, 2-Мбайты, 3-ГБайты
   {  case 0: sprintf(size, "%12.0lf", double((tabMME_Part + m)->SizeF));
              as = Char_Dig_p(size, 15);  break;             //Преобразование символьного числа в разрядку
      case 1: sprintf(size, "%11.1lf", double((tabMME_Part + m)->SizeF)/1024.0);
              as = Char_Dig_p_n(size, 12, 1);  break;        //Преобразование символьного числа в разрядку
      case 2: sprintf(size, "%9.2lf", double((tabMME_Part + m)->SizeF)/1024.0/1024.0);
              as = Char_Dig_p_n(size, 9, 2);  break;         //Преобразование символьного числа в разрядку
      case 3: sprintf(size, "%7.3lf", double((tabMME_Part + m)->SizeF)/1024.0/1024.0/1024.0);
              as = Char_Dig_p_n(size, 6, 3);  break;         //Преобразование символьного числа в разрядку
   }
   char sDate[64], sTime[64], Pr = 'R';
   SYSTEMTIME sysTime;
   sysTime.wYear = ((tabMME_Part + m)->dt.year == 0) ? WORD(1900) : (tabMME_Part + m)->dt.year;
   sysTime.wMonth = ((tabMME_Part + m)->dt.mon == 0) ? WORD(1) : (tabMME_Part + m)->dt.mon;
   sysTime.wDay = ((tabMME_Part + m)->dt.day == 0) ? WORD(1) : (tabMME_Part + m)->dt.day;
   sysTime.wHour = (tabMME_Part + m)->dt.hour;
   sysTime.wMinute = (tabMME_Part + m)->dt.min;
   sysTime.wSecond = (tabMME_Part + m)->dt.sec;
   sysTime.wDayOfWeek = 1;
   sysTime.wMilliseconds = 0;

   GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, sDate, 64);
   GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, &sysTime, NULL, sTime, 64);
   sysTime.wHour = WORD((tabMME_Part + m)->timeLong / 3600);
   int M1 = (tabMME_Part + m)->timeLong - sysTime.wHour * 3600;
   sysTime.wMinute = WORD(M1 / 60);
   sysTime.wSecond = WORD(M1 - sysTime.wMinute * 60);
   sysTime.wMilliseconds = 0;
   char sLTime[64];
   GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, &sysTime, NULL, sLTime, 64);
   if(Conf.ViewVRO == 1)                                     //0-показыват расширение *.vro, 1-не показывать
      for(int i=l-4; i<l; i++)                               //Стерли расширение .vro и точку
          *(oneStr + i) = ' ';
   int n = (Conf.poz_Ql == 0) ? 59 : 63;
   if(Conf.poz_In == 0)                                      //Источник записи: 0-в колонке, 1-в имени, 2-не показывать
      sprintf(oneStr + n,  "%s        ", (tabMME_Part + m)->nameIn);
   if(Conf.poz_Ql == 0)                                      //Качество записи: 0-в колонке, 1-в имени, 2-не показывать
      sprintf(oneStr + 66, "%s        ", (tabMME_Part + m)->Qual);
   sprintf(oneStr+70, "%s %s  [%8s]  %c %10s  %8s", as, strSize[Conf.typeSize], sLTime, Pr, sDate, sTime);
   return 0;
}

//------------------------------------------------------------------------------

static int Add_Folder_in_Tree(HTREEITEM hitem, DWORD ind)
{
   TV_INSERTSTRUCT tvins;
   char oneStr[256];

   indTitle = (aTree + ind)->indTitle;                       //Индекс Title
   MakeNameFolder(oneStr, ind);                              //Создание имени специальной папки
   ZeroMemory((void*)(&(aTree + numEl_Tree)->pf), sizeof(PAR_FILE));
   int m = (tabMME + (aTree + ind)->indTabMME)->indFolder;
   wsprintf((aTree + numEl_Tree)->NameF, "%s [%d]", (tabMME_Part + m)->aName, (tabMME_Part + m)->nPart);
   (aTree + numEl_Tree)->pf.type = 47;                       //Данное имя это специальная папка
   (aTree + numEl_Tree)->prSel = 0;                          //Признак выбора данного файла(0-не выбран)
   (aTree + numEl_Tree)->Level = (aTree + ind)->Level;       //Уровень вложенности
   (aTree + numEl_Tree)->indFolder = 0;                      //Ссылка на имя папки
   tvins.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   tvins.item.pszText = oneStr;
   tvins.item.lParam = numEl_Tree;                           //Индекс элемента в дереве имен
   tvins.item.iImage = tvins.item.iSelectedImage = 12;       //Индекс иконки
   tvins.hInsertAfter = TVI_SORT;
// tvins.hInsertAfter = TVI_LAST;
   tvins.hParent = hitem;
   hFOLDER = TreeView_InsertItem(hwndTree, &tvins);
   if(++numEl_Tree > MAX_NAME)
      return Error1((Lan+31)->msg);                          //return Error2("Суммарное число имен папок и файлов превышает возможности программы.");
   return 0;
}

//------------------------------------------------------------------------------

static int Change_Tree(HTREEITEM ind, HTREEITEM hitem, TV_ITEM *item)
{
   TV_INSERTSTRUCT tvins;
   if((aTree + item->lParam)->numPart == 0) return 0;        //Однофайловый Title
   if((aTree + item->lParam)->numPart == 1 && indTitle == 0xFFFF) //Для первого добавляем папку
      if(Add_Folder_in_Tree(hitem, item->lParam) < 0) return-1;
   if(indTitle != (aTree + item->lParam)->indTitle) return 0;//Индексы Title не совпали
   (aTree + item->lParam)->Level++;                          //Уровень вложенности
   (aTree + item->lParam)->indFolder = WORD(numEl_Tree-1);   //Ссылка на имя папки
   tvins.item = *item;                                       //Индекс иконки
   tvins.hInsertAfter = TVI_SORT;
// tvins.hInsertAfter = TVI_LAST;
   tvins.hParent = hFOLDER;
   TreeView_InsertItem(hwndTree, &tvins);                    //Перенесли имя во внутрь папки
   TreeView_DeleteItem(hwndTree, ind);                       //Удалили имя вне папки
   return 0;
}

//------------------------------------------------------------------------------

static int Remake_TreeMEDIA(HTREEITEM hitemMEDIA)            //Переформирование дерева для папки MEDIA
{
   TV_ITEM item;
   HTREEITEM ind, ind_next;
   char Msg[256];
   item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   item.pszText = Msg;
   item.cchTextMax = 256;
   indTitle = 0xFFFF;                                        //Индекс Title
   for(;;)                                                   //Для каждой первой части просматриваем все дерево
   {  ind = TreeView_GetChild(hwndTree, hitemMEDIA);         //Вошли во внутренний уровень дерева для MEDIA
      if(ind == NULL) return 0;
      for(;;)                                                //Просмотр всего дерева от текущей папки
      {  ind_next = TreeView_GetNextSibling(hwndTree, ind);  //Следующая запись на том же уровне дерева
         item.hItem = ind;
         if(TreeView_GetItem(hwndTree, &item) == FALSE)
           return Error1((Lan+33)->msg);                     //return Error1("Ошибка при запросе информации об элементе дерева.");
         if(Change_Tree(ind, hitemMEDIA, &item) < 0) return -1;
         if(ind_next == NULL) break;                         //Больше записей данного уровня нет
         ind = ind_next;
      }
      if(indTitle == 0xFFFF) return 0;                       //Прошли всю папку и ничего не нашли
      indTitle = 0xFFFF;                                     //Индекс Title
   }
}

//------------------------------------------------------------------------------

int  RemakeTree(void)                                        //Переформирование дерева с учетом многофайловых Titles
{
   TV_ITEM item;
   char Msg[256];

   item.mask = TVIF_TEXT;
   item.pszText = Msg;
   item.cchTextMax = 256;
   HTREEITEM hitem = TreeView_GetRoot(hwndTree);             //Начали поиск с самого верха
   hitem = TreeView_GetChild(hwndTree, hitem);               //Вошли во внутренний уровень дерева
   if(hitem == NULL) return 0;                               //Ничего нет
   for(int i=0; i<4; i++)                                    //Прошли по четырем возможным папкам
   {  item.hItem = hitem;
      if(TreeView_GetItem(hwndTree, &item) == FALSE)
        return Error1((Lan+33)->msg);                        //return Error1("Ошибка при запросе информации об элементе дерева.");
      if(lstrcmp(Msg, "MEDIA") == 0)                         //1- папка MEDIA
        return Remake_TreeMEDIA(hitem);                      //Переформирование дерева для папки MEDIA
      hitem = TreeView_GetNextSibling(hwndTree, hitem);      //Следующая запись на том же уровне дерева
      if(hitem == NULL) break;                               //Больше записей данного уровня нет
   }
   return 0;                                                 //Папки MEDIA нет
}

#endif

