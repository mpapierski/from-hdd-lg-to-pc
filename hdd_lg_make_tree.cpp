
//                                    24.01.2009
#ifndef __HDD_LG_MAKE_TREE__
#define __HDD_LG_MAKE_TREE__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_make_tree ================================
int  AddItemToTree_forWrite(char *Name, char *Ext, PAR_FILE *pf);  //Добавление новой строки в дерево при записи
int  AddItemToTree(char *Name, char *Ext, PAR_FILE *pf, int Level);//Добавление новой строки в дерево
void Expand_Tree(int hitem);                                 //Распахивание дерева
int  ClearSelect(void);                                      //Снятие всех выделений
int  RemoveSelectionFromCopy(int n);                         //Снятие выделение файла в дереве при копировании
bool ChangeSelect(int sShift);                               //Изменение выбора файлов в дерева
bool ChangeSelect_and_Down(void);                            //Изменение выбора файлов в дерева
void ViewSize(void);                                         //Показ дискового пространства
     int numEl_Tree;                                         //Число элементов в дереве имен
     OneStrTree *aTree;                                      //Информация по дереву
     HTREEITEM hPrev[MAX_U];                                 //Массив уровней вложенности дерева

static int pr_tviSt;                                         //Признак группового выбора
static HTREEITEM item_Sel[2];                                //Первая и вторая строки группового выбора
static HTREEITEM item_Par1;                                  //Родительская папка для первого выбора

//------------------------------------------------------------------------------
#if defined WRITE_YES                                        //Режим записи разрешен
int AddItemToTree_forWrite(char *Name, char *Ext, PAR_FILE *pf)//Добавление новой строки в дерево при записи
{
   TV_INSERTSTRUCT tvins;
   char oneStr[256];
   int Level = (aTree + item_Add.lParam)->Level + 1;
   if(Level >= MAX_U)
      return Error1((Lan+30)->msg);                          //return Error2("Уровень вложенности папок и файлов", "превышает возможности программы.");
   if(MakeOneStrForTree(oneStr, Name, Ext, pf) == 1) return 0;//Создание строки символов для отражения в дереве
   int ind = (pf->type == 48) ? 0 : 1;                       //Это имя папки (иконка с индексом 0), для остального по умолчанию иконка 1
   if(pf->type < 48 && Ext != NULL && *Ext != 0)             //Не папка и есть расширение
   {  ind = 1;                                               //Индекс иконки
      if(lstrcmpi(Ext, "jpg") == 0) ind = 2;
      if(lstrcmpi(Ext, "mp3") == 0) ind = 6;
      if(lstrcmpi(Ext, "avi") == 0) ind = 4;
      if(lstrcmpi(Ext, "vro") == 0) ind = 5;
      if(lstrcmpi(Ext, "str") == 0) ind = 5;
      if(ind == 5)
      {  if(pf->type == 33) ind = 7;                         //Ошибка типа 1
         if(pf->type == 34) ind = 8;                         //Ошибка типа 2
      }
      if(lstrcmpi(Ext, "divx") == 0)ind = 13;
      if(lstrcmpi(Ext, "mpg") == 0) ind = 14;
   }
   lstrcpy((aTree + numEl_Tree)->NameF, Name);
   (aTree + numEl_Tree)->pf = *pf;
   (aTree + numEl_Tree)->prSel = 0;                          //Признак выбора данного файла(0-не выбран)
   (aTree + numEl_Tree)->Level = WORD(Level);                //Уровень вложенности
// (aTree + numEl_Tree)->indTabMME = WORD(indTabMME);        //Индекс в таблице MME.DB
   (aTree + numEl_Tree)->indTabMME = WORD(-1);               //Индекс в таблице MME.DB
   tvins.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   tvins.item.pszText = oneStr;
   tvins.item.lParam = numEl_Tree;                           //Индекс элемента в дереве имен
   tvins.item.iImage = ind;
   tvins.item.iSelectedImage = ind;                          //Индекс иконки
   if(Conf.PrSort == 0)                                      //0-сортировка по имени, 1-без сортировки
      tvins.hInsertAfter = TVI_SORT;
   else
      tvins.hInsertAfter = TVI_LAST;
   tvins.hParent = item_Add.hItem;
   HTREEITEM hItemN = TreeView_InsertItem(hwndTree, &tvins);
   if(++numEl_Tree > MAX_NAME)
      return Error1((Lan+31)->msg);                          //return Error2("Суммарное число имен папок и файлов превышает возможности программы.");
// TreeView_Expand(hwndTree, item_Add.hItem, TVE_EXPAND);
   TreeView_EnsureVisible(hwndTree, hItemN);
   if(pf->type == 48)                                        //Это папка и значит при наличии файлов они должны быть в ней
   {  item_Add.mask = TVIF_TEXT | TVIF_PARAM;// | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
      item_Add.hItem = hItemN;
      if(TreeView_GetItem(hwndTree, &item_Add) == FALSE)     //Взяли текущий элемент дерева
         return Error1((Lan+33)->msg);                       //"Ошибка при запросе информации об элементе дерева."
   }
   return 0;
}
#endif
//------------------------------------------------------------------------------

int AddItemToTree(char *Name, char *Ext, PAR_FILE *pf, int Level)//Добавление новой строки в дерево
{
   TV_INSERTSTRUCT tvins;
   char oneStr[256];

   if(Level >= MAX_U)
      return Error1((Lan+30)->msg);                          //return Error2("Уровень вложенности папок и файлов", "превышает возможности программы.");
   if(MakeOneStrForTree(oneStr, Name, Ext, pf) == 1) return 0;//Создание строки символов для отражения в дереве
   int ind = (pf->type == 48) ? 0 : 1;                       //Это имя папки (иконка с индексом 0), для остального по умолчанию иконка 1
   if(pf->type < 48 && Ext != NULL && *Ext != 0)             //Не папка и есть расширение
   {  ind = 1;                                               //Индекс иконки
      if(lstrcmpi(Ext, "jpg") == 0) ind = 2;
      if(lstrcmpi(Ext, "mp3") == 0) ind = 6;
      if(lstrcmpi(Ext, "avi") == 0) ind = 4;
      if(lstrcmpi(Ext, "vro") == 0) ind = 5;
      if(lstrcmpi(Ext, "str") == 0) ind = 5;
      if(ind == 5)
      {  if(pf->type == 33) ind = 7;                         //Ошибка типа 1
         if(pf->type == 34) ind = 8;                         //Ошибка типа 2
      }
      if(lstrcmpi(Ext, "divx") == 0)ind = 13;
      if(lstrcmpi(Ext, "mpg") == 0) ind = 14;
   }
   lstrcpy((aTree + numEl_Tree)->NameF, Name);
   (aTree + numEl_Tree)->pf = *pf;
   (aTree + numEl_Tree)->prSel = 0;                          //Признак выбора данного файла(0-не выбран)
   (aTree + numEl_Tree)->Level = WORD(Level);                //Уровень вложенности
   (aTree + numEl_Tree)->indTabMME = WORD(indTabMME);        //Индекс в таблице MME.DB
   tvins.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   tvins.item.pszText = oneStr;
   tvins.item.lParam = numEl_Tree;                           //Индекс элемента в дереве имен
   tvins.item.iImage = ind;
   tvins.item.iSelectedImage = ind;                          //Индекс иконки
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

void Expand_Tree(int hitem)                                  //Tree-View Controls
{
   TreeView_Expand(hwndTree, hPrev[hitem], TVE_EXPAND);
}

//------------------------------------------------------------------------------

static void ViewNumSel(void)                                 //Вывод числа выбранных файлов
{
   char Ss1[64], Ss2[64], *as;
   if(num_Sel > 0)
   {  wsprintf(Ss1, "%s %d", (Lan+121)->msg, num_Sel);
      sprintf(Ss2, "%10.2lf", double(size_Sel)/1024.0/1024.0);
      as = Char_Dig_p_n(Ss2, 11, 2);                         //Преобразование символьного числа в разрядку
      sprintf(Ss2, "( %s Mb )",  as);
   }
   else
     *Ss1 = *Ss2 = 0;
   SetWindowText(hNumSel, Ss1);
   SetWindowText(hSizeSel, Ss2);
}

//------------------------------------------------------------------------------

static void StateButtonClear(void)                           //Изменение состояния кнопки "Очистить"
{
   bool State = (num_Sel == 0) ? false : true;
   EnableWindow(hClear, State);
}

//------------------------------------------------------------------------------

static int RemoveAllSelection(HTREEITEM hitem)               //Сняли все выделения файла в дереве
{
   char nName[300];
   HTREEITEM ind = TreeView_GetChild(hwndTree, hitem);       //Вошли во внутренний уровень дерева
   if(ind == NULL) return 0;                                 //Для вложенных пустых папок ничего не делаем
   for(;;)                                                   //Просмотр всего дерева от текущей папки
   {  TV_ITEM item;
      item.mask = TVIF_PARAM | TVIF_TEXT;
      item.pszText = nName;
      item.cchTextMax = sizeof(nName);
      item.hItem = ind;
      if(TreeView_GetItem(hwndTree, &item) == false)
         return Error1((Lan+33)->msg);                       //"Ошибка при запросе информации об элементе дерева."
      if((aTree + item.lParam)->pf.type == 48)               //Очередное имя это вложенная папка
      {  if(RemoveAllSelection(ind)  < 0) return -1;  }      //Сняли выделение файла в дереве
      else                                                   //Очередное имя это файл
      {  if((aTree + item.lParam)->prSel == 1)               //Нашли выделенный элемент
         {  item.mask = TVIF_STATE | TVIF_TEXT;
            item.state = 0;
            item.stateMask = TVIS_BOLD;
            *nName = ' ';
            if(TreeView_SetItem(hwndTree, &item) == -1)
              Error1((Lan+151)->msg);
            (aTree + item.lParam)->prSel = 0;
            if((aTree + item.lParam)->pf.type != 47)         //Это папка отредактированных Title
            {  num_Sel--;                                    //Число выделенных файлов
               size_Sel -= (aTree + item.lParam)->pf.SizeF;
            }
         }
      }
      if((aTree + item.lParam)->pf.type == 47)               //Очередное имя это папка многофайлового Title
      {  if(RemoveAllSelection(ind)  < 0) return -1;  }      //Сняли выделение файла в дереве
      ind = TreeView_GetNextSibling(hwndTree, ind);          //Следующая запись на том же уровне дерева
      if(ind == NULL) return 0;                              //Больше записей данного уровня нет
   }
}

//------------------------------------------------------------------------------

int ClearSelect(void)                                        //Снятие всех выделений
{
   HTREEITEM hitem = TreeView_GetRoot(hwndTree);
   if(hitem == NULL) return -1;
   int ret = RemoveAllSelection(hitem);                      //Сняли все выделение файла в дереве
   if(ret < 0) return -1;
   ViewNumSel();                                             //Вывод числа выбранных
   StateButtonClear();                                       //Изменение состояния кнопки "Очистить"
   return ret;
}

//------------------------------------------------------------------------------

static int Remove_Selection(HTREEITEM hitem, int n)          //Сняли выделение файла в дереве
{
   char nName[300];
   HTREEITEM ind = TreeView_GetChild(hwndTree, hitem);       //Вошли во внутренний уровень дерева
   if(ind == NULL) return 0;                                 //Для вложенных пустых папок ничего не делаем
   for(;;)                                                   //Просмотр всего дерева от текущей папки
   {  TV_ITEM item;
      item.mask = TVIF_PARAM | TVIF_TEXT;
      item.pszText = nName;
      item.cchTextMax = sizeof(nName);
      item.hItem = ind;
      if(TreeView_GetItem(hwndTree, &item) == false)
         return Error1((Lan+33)->msg);                       //"Ошибка при запросе информации об элементе дерева."
      if(item.lParam == n &&
         (aTree + item.lParam)->pf.type == 47)               //Очередное имя это вложенная папка
      {  if((aTree + item.lParam)->prSel != 2)
            return Error1((Lan+81)->msg);                    //81, "Неопознанная ошибка работы со списком."
//??     TreeView_EnsureVisible(hwndTree, ind);              //Показали элемент дерева перед тем как снять выделение
         item.mask = TVIF_STATE | TVIF_TEXT;
         item.state = 0;
         item.stateMask = TVIS_BOLD;
         *nName = ' ';
         if(TreeView_SetItem(hwndTree, &item) == -1)
            Error1((Lan+151)->msg);
         (aTree + n)->prSel = 0;                             //Признак, файл скопирован
         return 1;
      }

      if((aTree + item.lParam)->pf.type == 48 ||             //Очередное имя это вложенная папка
         (aTree + item.lParam)->pf.type == 47)               //Очередное имя это вложенная папка
      {  int ret = Remove_Selection(ind, n);                 //Сняли выделение файла в дереве
         if(ret  < 0) return -1;
         if(ret == 1) return 1;                              //Нашли нужный файл и сняли выделение
      }
      else                                                   //Очередное имя это файл
      {  if(n == item.lParam)                                //Нашли элемент для скопированного файла
         {  if((aTree + item.lParam)->prSel != 2)
               return Error1((Lan+81)->msg);                 //81, "Неопознанная ошибка работы со списком."
//??        TreeView_EnsureVisible(hwndTree, ind);           //Показали элемент дерева перед тем как снять выделение
            item.mask = TVIF_STATE | TVIF_TEXT;
            item.state = 0;
            item.stateMask = TVIS_BOLD;
            *nName = ' ';
            if(TreeView_SetItem(hwndTree, &item) == -1)
              Error1((Lan+151)->msg);
            (aTree + n)->prSel = 0;                          //Признак, файл скопирован
            num_Sel--;                                       //Число выбранных файлов
            size_Sel -= (aTree + n)->pf.SizeF;
            return 1;
         }
      }
      ind = TreeView_GetNextSibling(hwndTree, ind);          //Следующая запись на том же уровне дерева
      if(ind == NULL) break;                                 //Больше записей данного уровня нет
   }
   return 0;                                                 //Прошли до конца и не нашли
}

//------------------------------------------------------------------------------

int RemoveSelectionFromCopy(int n)                           //Снятие выделение файла в дереве при копировании
{
   (aTree + n)->prSel = 2;                                   //Признак, файл скопирован
   HTREEITEM hitem = TreeView_GetRoot(hwndTree);
   if(hitem == NULL) return -1;
   int ret = Remove_Selection(hitem, n);                     //Сняли выделение файла в дереве
   if(ret < 0) return -1;
   if(ret == 0) return -1;
   ViewNumSel();                                             //Вывод числа выбранных
   return 0;
}

//------------------------------------------------------------------------------

static int ChangeSelectFolder(HTREEITEM h_Item, DWORD indT, WORD prSel)  //Изменение выбора группы
{
   TV_ITEM tvi;
   char nName[300];

   HTREEITEM hitem = TreeView_GetChild(hwndTree, h_Item);    //Вошли во внутренний уровень дерева
   if(hitem == NULL) return Error1((Lan+33)->msg);           //return Error1("Ошибка при запросе информации об элементе дерева.");
   for(;;)
   {  tvi.mask = TVIF_PARAM | TVIF_TEXT;
      tvi.pszText = nName;
      tvi.cchTextMax = sizeof(nName);
      tvi.hItem = hitem;
      if(TreeView_GetItem(hwndTree, &tvi) == FALSE)
        return Error1((Lan+33)->msg);                        //return Error1("Ошибка при запросе информации об элементе дерева.");
      tvi.mask = TVIF_STATE | TVIF_TEXT;
      if((aTree + tvi.lParam)->prSel != prSel)               //Выбор не совпадает
      {  if((aTree + tvi.lParam)->prSel == 0)                //Данный файл не выделен
         {  tvi.state = TVIS_BOLD;// | TVIS_DROPHILITED;
            (aTree + tvi.lParam)->prSel = 1;                 //Данный файл выделен
            *nName = '+';
            num_Sel++;                                       //Число выбранных имен при групповом выборе
            size_Sel += (aTree + tvi.lParam)->pf.SizeF;
            (aTree + indT)->numCopy++;                       //Счетчик скопированных частей
         }
         else
         {  tvi.state = 0;
            (aTree + tvi.lParam)->prSel = 0;                 //Данный файл не выделен
            *nName = ' ';
            num_Sel--;                                       //Число выбранных имен при групповом выборе
            size_Sel -= (aTree + tvi.lParam)->pf.SizeF;
            (aTree + indT)->numCopy--;                       //Счетчик скопированных частей
         }
         tvi.stateMask = TVIS_BOLD | TVIS_SELECTED;// | TVIS_DROPHILITED;
         if(TreeView_SetItem(hwndTree, &tvi) == -1)
           return  Error1((Lan+151)->msg);
      }
      hitem = TreeView_GetNextSibling(hwndTree, hitem);      //Следующая запись на том же уровне дерева
      if(hitem == NULL) break;                               //Больше записей данного уровня нет
   }
   return 0;
}

//------------------------------------------------------------------------------

static bool ChangeOneSelect(TV_ITEM *tvi, char *nName)       //Изменение выбора одного файла
{
   tvi->mask = TVIF_STATE | TVIF_TEXT;
   if((aTree + tvi->lParam)->prSel == 0)                     //Данный файл не выделен
   {  tvi->state = TVIS_BOLD;// | TVIS_DROPHILITED;
      (aTree + tvi->lParam)->prSel = 1;                      //Данный файл выделен
      *nName = '+';
      if((aTree + tvi->lParam)->pf.type != 47)               //Это папка отредактированных Title
      {  num_Sel++;                                          //Число выбранных имен при групповом выборе
         size_Sel += (aTree + tvi->lParam)->pf.SizeF;
      }
   }
   else
   {  tvi->state = 0;
      (aTree + tvi->lParam)->prSel = 0;                      //Данный файл не выделен
      *nName = ' ';
      if((aTree + tvi->lParam)->pf.type != 47)               //Это папка отредактированных Title
      {  num_Sel--;                                          //Число выбранных имен при групповом выборе
         size_Sel -= (aTree + tvi->lParam)->pf.SizeF;
      }
   }
   tvi->stateMask = TVIS_BOLD | TVIS_SELECTED;// | TVIS_DROPHILITED;
   if(TreeView_SetItem(hwndTree, tvi) == -1)
   {  Error1((Lan+151)->msg);
      return false;
   }
   if((aTree + tvi->lParam)->pf.type == 47)                  //Это папка отредактированных Title
      ChangeSelectFolder(tvi->hItem, tvi->lParam, (aTree + tvi->lParam)->prSel);//Изменение выбора группы
   ViewNumSel();                                             //Вывод числа выбранных
   StateButtonClear();                                       //Изменение состояния кнопки "Очистить"
   return true;
}

//------------------------------------------------------------------------------

static int Mark_Selected(void)                               //Изменение маркировки группы файлов
{
   HTREEITEM ind;
   char nName[300];
   TV_ITEM tvi;
   ZeroMemory(&tvi, sizeof(tvi));
   ind = TreeView_GetChild(hwndTree, item_Par1);             //Первый дочерний элемент родительской папки
   if(ind == NULL) return Error1((Lan+33)->msg);             //return Error1("Ошибка при запросе информации об элементе дерева.");
   for(int i=0; i<5000; i++)                                 //Поиск первой границы из двух
   {  if(ind == item_Sel[0]) break;
      if(ind == item_Sel[1])
      {  item_Sel[1] = item_Sel[0];
         break;
      }
      ind = TreeView_GetNextSibling(hwndTree, ind);          //Следующая запись на том же уровне дерева
      if(ind == NULL) return Error1((Lan+33)->msg);          //return Error1("Ошибка при запросе информации об элементе дерева.");
   }
   for(;;)                                                   //Просмотр всего дерева от текущей папки
   {  ind = TreeView_GetNextSibling(hwndTree, ind);          //Следующая запись на том же уровне дерева
      if(ind == NULL) return Error1((Lan+33)->msg);          //return Error1("Ошибка при запросе информации об элементе дерева.");
      if(ind == item_Sel[1]) break;                          //Найдена нижняя граница
      tvi.mask = TVIF_PARAM | TVIF_TEXT;
      tvi.hItem = ind;
      tvi.pszText = nName;
      tvi.cchTextMax = sizeof(nName);
      if(TreeView_GetItem(hwndTree, &tvi) == false)
        return Error1((Lan+33)->msg);                        //return Error1("Ошибка при запросе информации об элементе дерева.");
      if(ChangeOneSelect(&tvi, nName) == false) return -1;   //Изменение выбора одного файла
   }
   return 0;
}

//------------------------------------------------------------------------------

static void ViewShift(void)
{
   if(pr_tviSt != 0)                                         //Eсть первый выбор
   {  char Ss[300];
      sprintf(Ss, "  %s  ", (Lan+148)->msg);
      SetWindowText(hPrShift, Ss);
   }
   else
      SetWindowText(hPrShift, "");
}

//------------------------------------------------------------------------------

static bool Select_Group(void)                               //Изменение состояния всей группы файлов между строками
{
   Mark_Selected();
   pr_tviSt = 0;                                             //Признак группового выбора
   item_Sel[0] = item_Sel[1] = item_Par1 = NULL;
   ViewShift();
   return true;
}

//------------------------------------------------------------------------------

bool ChangeSelect(int sShift)                                //Изменение выбора файлов в дерева
{
   char nName[300];
   TV_ITEM tvi;
// ZeroMemory(&tvi, sizeof(tvi));
   HTREEITEM Selected = TreeView_GetSelection(hwndTree);
   if(Selected == NULL) return false;
   tvi.mask = TVIF_PARAM | TVIF_TEXT;
   tvi.hItem = Selected;
   tvi.pszText = nName;
   tvi.cchTextMax = sizeof(nName);
   if(TreeView_GetItem(hwndTree, &tvi) == false) return false;
   if((aTree + tvi.lParam)->pf.type == 48) return false;     //Для папки выделения нет
   if(sShift != 0 && pr_tviSt != 0)                          //Клавиша Shift нажата и есть первый выбор
   {  HTREEITEM item_Par2 = TreeView_GetParent(hwndTree, Selected);  //Получение родительской папки
      if(item_Par1 != item_Par2)                             //Сравнение родительских папок. Они должны быть одинаковыми
      {  Error2((Lan+149)->msg, (Lan+150)->msg);
         return true;
      }
      if(item_Sel[0] == Selected)                            //Строки должны быть разными
      {  Error2((Lan+149)->msg, (Lan+153)->msg);
         return true;
      }
   }
   if(ChangeOneSelect(&tvi, nName) == false) return false;   //Изменение выбора одного файла
   if(sShift == 0)                                           //Клавиша Shift не нажата
   {//if(item_Sel[0] != NULL)
    //  TreeView_SetInsertMark(hwndTree, item_Sel[0], false);//Марка первого выбора
      pr_tviSt = 0;                                          //Признак группового выбора
      item_Sel[0] = item_Sel[1] = item_Par1 = NULL;
      ViewShift();
      return true;
   }
   if(pr_tviSt == 0)                                         //Еще не отмечалась первая строкагруппового выбора
   {  pr_tviSt = 1;                                          //Признак группового выбора
      item_Sel[0] = Selected;
      item_Par1 = TreeView_GetParent(hwndTree, Selected);
//    TreeView_SetInsertMark(hwndTree, item_Sel[0], true);   //Марка первого выбора
      ViewShift();
      return true;
   }
   item_Sel[1] = Selected;
// TreeView_SetInsertMark(hwndTree, item_Sel[0], false);     //Марка первого выбора
   return Select_Group();                                    //Изменение состояния всей группы файлов между строками
}

//------------------------------------------------------------------------------

bool ChangeSelect_and_Down(void)                             //Изменение выбора файлов в дерева
{
   if(ChangeSelect(0) == false) return false;                //Изменение выбора файлов в дерева
   HTREEITEM Selected = TreeView_GetSelection(hwndTree);     //Текущий выбор
   if(Selected == NULL) return false;
   HTREEITEM newSelected = TreeView_GetNextSibling(hwndTree, Selected); //Следующая строка в дереве
   if(newSelected == NULL) return false;                     //Больше нет строк на данном уровне
   return TreeView_SelectItem(hwndTree, newSelected);        //Отметили следующую стоку
}

//------------------------------------------------------------------------------

void ViewSize(void)                                          //Показа дискового пространства
{
   char Ss[300];
   if(maxZapFAT1 == 0xFFFFFFFF) return;                     //Диск не найден, размеры неизвестны
   double maxSize = double(maxZapFAT1 - 1) /1024.0/1024.0/1024.0 * sCl_B;
   double usedSize = double(writeCl) /1024.0/1024.0/1024.0 * sCl_B;
   double freeSize = double(maxZapFAT1 - 1 - writeCl) /1024.0/1024.0/1024.0 * sCl_B;
   sprintf(Ss, "%s:%7.2lf Gb     %s:%7.2lf Gb (%.1lf%c)     %s:%7.2lf Gb (%.1lf%c)",
                (Lan+124)->msg, maxSize,
                (Lan+125)->msg, usedSize, usedSize/maxSize*100.0, '%',
                (Lan+126)->msg, freeSize, freeSize/maxSize*100.0, '%');
   SetWindowText(hSizeHDD, Ss);
}

#endif


