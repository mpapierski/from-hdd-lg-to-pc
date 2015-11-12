
//                                04.11.2009
#ifndef __HDD_LG_WRITE__
#define __HDD_LG_WRITE__

#include "from_hdd_lg_to_pc.h"

#if defined WRITE_YES                                        //Режим записи разрешен

//============================ hdd_lg_write ====================================
int  WriteFileToHDD(int pr);                                 //Запись файлов на диск
     int prFolder;                                           //Вариант расширения файла (0-mp3, 1-jpg, 2-avi)
     DWORD ClStDir;                                          //Номер кластера каталога в который выполняется запись
     TV_ITEM item_Add;                                       //Выбор в строке дерева куда будет добавление
     int NumMsg38;                                           //Число сообщений о превышении длины имени

//------------------------------------------------------------------------------

int  WriteFileToHDD(int pr)                                  //Запись файлов на диск LG
{
   char Msg0[256], Msg1[256];
   NumMsg38 = 0;                                             //Число сообщений о превышении длины имени
   if(num_Sel > 0)                                           //Число выбранных имен при групповом выборе
     return Error1((Lan+173)->msg);                          //"Должна быть выбрана одна папка назначения для копирования."
   HTREEITEM indSel = TreeView_GetSelection(hwndTree);
   if(indSel == NULL)
     return Error1((Lan+173)->msg);                          //"Должна быть выбрана одна папка назначения для копирования."
   TV_ITEM itemSel;                                          //Выбор в строке дерева
   itemSel.mask = TVIF_TEXT | TVIF_PARAM;
   itemSel.hItem = indSel;
   itemSel.pszText = Msg0;
   itemSel.cchTextMax = 256;
   if(TreeView_GetItem(hwndTree, &itemSel) == FALSE)         //Взяли текущий элемент дерева
      return Error1((Lan+33)->msg);                          //"Ошибка при запросе информации об элементе дерева."
   if(pr < 3 && (aTree + itemSel.lParam)->pf.type != 48)     //Для первых трех функций Текущий выбор д.б. папкой
      return Error1((Lan+173)->msg);                         //"Должна быть выбрана одна папка назначения для копирования."
   if(pr > 2 && (aTree + itemSel.lParam)->Level <= 1)        //Нельзя переименовывоть и удалять стандартные папки
      return Error1((Lan+189)->msg);                         //"Нельзя изменять стандартные папки."
   TV_ITEM item1 = itemSel;
   if((aTree + itemSel.lParam)->Level > 1)
   {  HTREEITEM ind1 = indSel;
      for(;;)
      {  ind1 = TreeView_GetParent(hwndTree, ind1);          //Получение родительской папки
         item1.mask = TVIF_TEXT | TVIF_PARAM;
         item1.hItem = ind1;
         item1.pszText = Msg1;
         item1.cchTextMax = 256;
         if(TreeView_GetItem(hwndTree, &item1) == FALSE)     //Взяли текущий элемент дерева
            return Error1((Lan+33)->msg);                    //"Ошибка при запросе информации об элементе дерева."
         if((aTree + item1.lParam)->Level > 1) continue;
         break;
      }
   }
   prFolder = -1;                                            //Вариант расширения файла (0-mp3, 1-jpg, 2-avi)
   if(lstrcmp(item1.pszText, "JUKEBOX") == 0) prFolder = 0;  //Вариант расширения файла (0-mp3, 1-jpg, 2-avi)
   if(lstrcmp(item1.pszText, "PHOTO") == 0)   prFolder = 1;  //Вариант расширения файла (0-mp3, 1-jpg, 2-avi)
   if(lstrcmp(item1.pszText, "VIDEO") == 0)   prFolder = 2;  //Вариант расширения файла (0-mp3, 1-jpg, 2-avi)
   if(prFolder < 0)
     return Error1((Lan+180)->msg);                          //"Для копирования можно выбирать только папки: JUKEBOX, PHOTO, VIDEO или внутренние для них"
   ClStDir = (aTree + itemSel.lParam)->pf.ClSt;              //Номер кластера каталога в который выполняется запись
   if(pr > 2)                                                //Для удаление и переименования нужен каталог в котором прописано имя
   {  TV_ITEM item = itemSel;
      HTREEITEM ind1 = TreeView_GetParent(hwndTree, itemSel.hItem);//Получение родительской папки
      item.mask = TVIF_TEXT | TVIF_PARAM;
      item.hItem = ind1;
      item.pszText = Msg1;
      item.cchTextMax = 256;
      if(TreeView_GetItem(hwndTree, &item) == FALSE)         //Взяли текущий элемент дерева
          return Error1((Lan+33)->msg);                      //"Ошибка при запросе информации об элементе дерева."
      ClStDir = (aTree + item.lParam)->pf.ClSt;              //Номер кластера каталога в который выполняется запись
   }
   EnableW(false);                                           //Изменение активности кнопок
   int ret = 0;
   item_Add = itemSel;
   MaxClast = 0;                                             //Номер самого старшего кластера использованного для записи
   switch(pr)
   {  case 0: ret = File_ToHDD();   break;                   //Запись файлов на диск LG
      case 1: ret = Folder_ToHDD(); break;                   //Запись папок на диск LG
      case 2: ret = New_Folder_ToHDD(); break;               //Создание папок на диске LG
      case 3: ret = RenameFiFo(indSel, aTree + itemSel.lParam); break; //Переименование
      case 4: ret = DeleteFiFo(indSel, aTree + itemSel.lParam); break; //Удаление
   }
   MyFreeMem(&(void*)c_FAT1);
   MyFreeMem(&(void*)spisF);
   if(ret < 0)                                               //Если была ошибка, то дерево пересоздаем
   {  Update_Tree();                                         //Пересоздание дерева файлов и папок при изменении настроек
      ViewSize();                                            //Показа дискового пространства
   }
   else
     if(NumMsg38 != 0)                                       //Число сообщений о превышении длины имени
      Message1((Lan+192)->msg);                              //"Изменения с функцией записи вступят в силу после перезапуска программы."
//Проблема в том, что после пересоздания дерева ind0 - недействителен
// TreeView_SelectDropTarget(hwndTree, ind0);
// TreeView_Expand(hwndTree, ind0, TVE_EXPAND);
   EnableW(true);                                            //Изменение активности кнопок
   return ret;
}

#endif
#endif
