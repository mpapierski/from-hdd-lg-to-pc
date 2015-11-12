
//                                04.11.2009
#ifndef __HDD_LG_WRITE_2__
#define __HDD_LG_WRITE_2__

#include "from_hdd_lg_to_pc.h"

#if defined WRITE_YES                                        //Режим записи разрешен

//============================ hdd_lg_write_2 ==================================
BOOL CALLBACK Dlg_NewName(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int  RenameFiFo(HTREEITEM hitem, OneStrTree *aTree);         //Переименование
int  DeleteFiFo(HTREEITEM hitem, OneStrTree *aTree);         //Удаление
int  Change_Sec67(int num_Cl);                               //Внесение изменений в содержимое сектора 67
int  Correct67Sec(void);                                     //Коррекция содержимого 67 сектора
     char NameFoFi[256];                                     //Имя создаваемой папки
     DWORD MaxClast;                                         //Номер самого старшего кластера использованного для записи

static int num_del_Cl;                                       //Число удаленных кластеров

//------------------------------------------------------------------------------

#pragma argsused
BOOL CALLBACK Dlg_NewName(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)
   {  case WM_INITDIALOG:
             SetWindowText(hDlg, (Lan+183)->msg);            //"Укажите имя"
             SetDlgItemText(hDlg, IDC_STATICTEXT1, (Lan+185)->msg);  //"Не более 38 символов"
             SetDlgItemText(hDlg, IDOK, (Lan+74)->msg);      //"Сохранить"
             SetDlgItemText(hDlg, IDCANCEL, (Lan+75)->msg);  //"Закрыть"
             if(lParam != 0)
             {  SetDlgItemText(hDlg, IDC_EDIT1, (char*)lParam);
                int l = lstrlen((char*)lParam);
                SendDlgItemMessage(hDlg, IDC_EDIT1, EM_SETSEL, 0, l);   //Сделать текст выделенным и поставить курсор в конец текста
             }
             SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
             return false;                                   //Для установки фокуса ввода
      case WM_CTLCOLORSTATIC:
             if(GetDlgCtrlID(HWND(lParam)) == IDC_STATICTEXT1)
             {  SetTextColor((HDC)wParam, RGB(0, 0, 200));
                SetBkColor((HDC)wParam, FonLTGRAY);
                return (BOOL)FonBrush;
             }
             return true;
      case WM_COMMAND:
           switch(LOWORD(wParam))
           {  case IDOK:     GetDlgItemText(hDlg, IDC_EDIT1, NameFoFi, sizeof(NameFoFi)-1);
                             if(*NameFoFi == 0)
                             {  Error1((Lan+184)->msg);      //"Недопустимое имя"
                                SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
                                return true;
                             }
                             for(int i=0; *(NameFoFi+i)!=0; i++)
                               for(int j=0; j<sizeof(ndS); j++)
                                 if(*(NameFoFi+i) == *(ndS+j))
                                 {  Error1((Lan+184)->msg);  //"Недопустимое имя"
                                    SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
                                    return true;
                                 }
                             *(NameFoFi+38) = 0;             //Если имя большое, то оно отсечется
              case IDCANCEL:
                             EndDialog(hDlg, LOWORD(wParam));//Вернули один из кодов
                             return true;
           }
   }
   return false;
}

//------------------------------------------------------------------------------

#pragma argsused
BOOL CALLBACK Dlg_DelName(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)
   {  case WM_INITDIALOG:
             SetWindowText(hDlg, (Lan+36)->msg);             //"Внимание"
             SetDlgItemText(hDlg, IDC_STATICTEXT1, (Lan+191)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT2, (char*)lParam);
             SetDlgItemText(hDlg, IDOK, (Lan+187)->msg);     //"Удалить"
             SetDlgItemText(hDlg, IDCANCEL, (Lan+75)->msg);  //"Закрыть"
             return true;                                    //Для установки фокуса ввода
      case WM_CTLCOLORSTATIC:
             if(GetDlgCtrlID(HWND(lParam)) == IDC_STATICTEXT1)
             {  SetTextColor((HDC)wParam, RGB(0, 0, 200));
                SetBkColor((HDC)wParam, FonLTGRAY);
                return (BOOL)FonBrush;
             }
             return true;
      case WM_COMMAND:
           switch(LOWORD(wParam))
           {  case IDOK:
              case IDCANCEL: EndDialog(hDlg, LOWORD(wParam));//Вернули один из кодов
                             return true;
           }
   }
   return false;
}

//------------------------------------------------------------------------------

static int GetName_FiFo(OneStrTree *aTree, char *oldName, char *oldExt)//Получение строки каталога с нужным именем
{
   One_Str_Cat *Kat = (One_Str_Cat *)bufDir;
   //Чтение кластера каталога (каталога может и не существовать и, в том числе каталога самого верхнего уровня)
   //Обновление каталога (добавление строки нового файла) и в том числе создание нового каталога
   DWORD nSector = Start_SecDir1 + (ClStDir - 1) * sClSec;   //Номер сектора по номеру кластера
   if(ReadClast1_P(nSector, bufDir) < 0) return -1;          //Чтение кластера
   if(*((DWORD*)&(Kat + 0)->Name) != 0x00002E00)             //Это одна точка т.е. кластер текущего каталога
      return Error1((Lan+96)->msg);                          //"Нарушена структура каталога."
   if(*((DWORD*)&(Kat + 1)->Name) != 0x2E002E00)             //Это две точки т.е. кластер предыдущего каталога
      return Error1((Lan+96)->msg);                          //"Нарушена структура каталога."
   WCHAR NameU[64], ExtU[8];                                 //Именя в Unicode
   lstrcpy(oldName, aTree->NameF);
   *oldExt = 0;
   if(aTree->pf.type != 48)                                  //Это файл
   {  char *adr = strrchr(oldName, '.');
      if(adr != NULL)
      {  *adr = 0;                                           //Отсекли имя
         lstrcpy(oldExt, adr+1);                             //Выделили расширение
      }
   }
   Ansi_To_Unicode(oldName, NameU, 40);                      //Преобразовали имя в UNICODE
   Ansi_To_Unicode(oldExt, ExtU, 4);                         //Преобразовали расширение из UNICODE
   WORD *nameI = (WORD*)NameU;
   WORD *extI = (WORD*)ExtU;
   int nsDir = 2;                                            //Номер строки каталога с нужнвм именем
   int t1 = 0;
   for(; nsDir<4*sClSec; nsDir++)                            //По всем возможным записям в одном кластере каталога (В каждом секторе 4 записи)
   {  if((Kat + nsDir)->pf.type == 0) break;                 //Конец записей каталога
      WORD *nameK = (WORD*)(Kat + nsDir)->Name;
      int t;
      for(t=0; t<40; t++)                                    //По длине имени
      { if(*(nameI + t) == 0) break;                         //Входное имя кончилолось
        if(*(nameK + t) == 0) break;                         //Имя в каталоге кончилолось
        if(*(nameI + t) != *(nameK + t)) break;              //Несовпадение символов
      }
      if(*(nameI + t) != 0 || *(nameK + t) != 0) continue;   //Не дошли до конца обоих имен
      if(!(t >= 40 || *(nameI + t) == 0)) continue;          //Либо прошли все возможные по числу символы либо не дошли до конца имени
      if(aTree->pf.type != 48)                               //Это файл
      {  WORD *extK = (WORD*)(Kat + nsDir)->Ext;
         int t;
         for(t=0; t<4; t++)                                  //По длине имени
           if(*(extI+t) == *(extK + t)) continue;
         if(!(t >= 4 || *(extI+t) == 0)) continue;           //Либо прошли все возсодные символы либо дошли  до конца имени
      }
      t1 = 1;                                                //Найдено имя в каталоге
      break;                                                 //Просмотр прекращаем
   }
   if(t1 == 0)
      return Error1((Lan+188)->msg);                         //"В каталоге не найдено указанное имя."
   return nsDir;
}

//------------------------------------------------------------------------------

int  RenameFiFo(HTREEITEM hitem, OneStrTree *aTree)          //Переименование
{
   char oldName[80], oldExt[8];
   One_Str_Cat *Kat = (One_Str_Cat *)bufDir;
   int nsDir = GetName_FiFo(aTree, oldName, oldExt);         //Получение строки каталога с нужным именем
   if(nsDir < 0) return -1;
// lstrcat(oldName, ".");
// lstrcat(oldName, oldExt);
   if(DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_NAME),
                MainWin, Dlg_NewName, LPARAM(oldName)) == IDCANCEL) return -1;
   Ansi_To_Unicode(NameFoFi, (Kat + nsDir)->Name, 40);       //Преобразовали имя из UNICODE и внесли в каталог
#if !defined EMULATOR_HDD                                    //Режим эмулятора
   if(Save_Dir() < 0) return -1;                             //Сохранение кластера каталога
#endif
   char NewStr[256];
   if(MakeOneStrForTree(NewStr, NameFoFi, oldExt, &((Kat + nsDir)->pf)) == 1) return 0;//Создание строки символов для отражения в дереве
   TV_ITEM item;                                             //Строка дерева
   item.mask = TVIF_TEXT;                                    //Меняем только текст
   item.hItem = hitem;
   item.pszText = NewStr;
   item.cchTextMax = 256;
   if(TreeView_SetItem(hwndTree, &item) == FALSE)            //Взяли текущий элемент дерева
      return Error1((Lan+33)->msg);                          //"Ошибка при запросе информации об элементе дерева."
   TreeView_EnsureVisible(hwndTree, hitem);
   lstrcpy(aTree->NameF, NameFoFi);
//   lstrcat(aTree->NameF, ".");
//   lstrcat(aTree->NameF, oldExt);
   return 0;
}

//------------------------------------------------------------------------------

static int Del_OneFileFAT(OneStrTree *aTree)                 //Удаление одного файла из FAT (отметка места как незанятого)
{
   DWORD nCl = aTree->pf.ClSt;                               //Текущий номер кластера равен первому кластеру файла
   DWORD numCl = DWORD((aTree->pf.SizeF + sCl_B - 1) / sCl_B);//Число кластеров необходимое для размещения файла данного размера
   for(DWORD i=0; i<numCl; i++)                              //По числу кластеров
   {  if(nCl == 0x0FFFFFFF)                                  //Признак конца цепочки
        return Error1((Lan+12)->msg);                        //return Error1("Неожиданно найден признак конца цепочки FAT.");
      if(*(FAT1 + nCl) == 0)                                 //Ненормальная ситуация
        return Error1((Lan+11)->msg);                        //return Error1("Обнаружено несоответствие значения FAT и ссылки на кластер файла.");
      *(c_FAT1 + nCl) = 0;                                   //Освободили кластер
      nCl = *(FAT1 + nCl);                                   //Номер следующего кластера
      if(nCl > maxZapFAT1 && nCl != 0x0FFFFFFF)
        return Error1((Lan+13)->msg);                        //"Номер кластера превышает допустимое значение."
   }
   num_del_Cl += numCl;                                      //Число удаленных кластеров
   return 0;
}

//------------------------------------------------------------------------------

static int Del_DirFAT(OneStrTree *aTree)                     //Удаление каталога из FAT (отметка места как незанятого)
{
   DWORD nCl = aTree->pf.ClSt;                               //Текущий номер кластера равен первому кластеру файла
   if(*(FAT1 + nCl) != 0x0FFFFFFF)                           //Признак конца цепочки
      return Error1((Lan+18)->msg);                          //Обнаружено несоответствие FAT и указателя на кластер каталога.
   *(c_FAT1 + nCl) = 0;                                      //Освободили кластер
   num_del_Cl++;                                             //Число удаленных кластеров
   return 0;
}

//------------------------------------------------------------------------------

static int Del_Folder(HTREEITEM hitem, OneStrTree *aTreeDir) //Удаление папки
{
   if(Del_DirFAT(aTreeDir) < 0) return -1;                   //Удаление каталога из FAT (отметка места как незанятого)

   HTREEITEM ind = TreeView_GetChild(hwndTree, hitem);       //Вошли во внутренний уровень дерева
   if(ind == NULL) return 0;                                 //Нет внутреннего уровня, т.е. папка пустая
   for(;;)                                                   //Просмотр всего дерева от текущей папки
   {  TV_ITEM item;
      item.mask = TVIF_PARAM;
      item.hItem = ind;
      if(TreeView_GetItem(hwndTree, &item) == FALSE)
        return Error1((Lan+33)->msg);                        //return Error1("Ошибка при запросе информации об элементе дерева.");
      if((aTree + item.lParam)->pf.type == 48)               //Очередное имя это папка
      {   if(Del_Folder(ind, aTree + item.lParam) < 0) return -1; } //Удаление папки
      else                                                   //Очередное имя это файл
          Del_OneFileFAT(aTree + item.lParam);               //Удаление одного файла из FAT (отметка места как незанятого)
      ind = TreeView_GetNextSibling(hwndTree, ind);          //Следующая запись на том же уровне дерева
      if(ind == NULL) break;                                 //Больше записей данного уровня нет
   }
   return 0;
}

//------------------------------------------------------------------------------

int  DeleteFiFo(HTREEITEM hitem, OneStrTree *aTree)          //Удаление файла или каталога
{
   char oldName[80], oldExt[8];

   One_Str_Cat *Kat = (One_Str_Cat *)bufDir;
   int nsDir = GetName_FiFo(aTree, oldName, oldExt);         //Получение строки каталога с нужным именем
   if(nsDir < 0) return -1;
   if(*oldExt != 0)
   {  lstrcat(oldName, ".");
      lstrcat(oldName, oldExt);
   }
   if(DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_DEL_NAME),
                MainWin, Dlg_DelName, LPARAM(oldName)) == IDCANCEL) return -1;
   c_FAT1 = (DWORD *)MyAllocMem(Size_FAT1);                  //Память под FAT
   if(c_FAT1 == NULL)  return -1;
   CopyMemory(c_FAT1, FAT1, Size_FAT1);                      //Скопировали содержимое FAT1

   *((WORD*)&(Kat + nsDir)->Name) = 0xE500;                  //Затерли строку с именем в каталоге
   (Kat + nsDir)->pf.ClSt = 0;                               //Второй признак удаленной записи
#if !defined EMULATOR_HDD                                    //Режим эмулятора
   if(Save_Dir() < 0) return -1;                             //Сохранение кластера каталога
#endif
   num_del_Cl = 0;                                           //Число удаленных кластеров
   int ret = (aTree->pf.type == 48) ?                        //Текущий выбор это папка
       Del_Folder(hitem, aTree) :                            //Удаление папки
       Del_OneFileFAT(aTree);                                //Удаление одного файла из FAT
   if(ret < 0) return -1;
   TreeView_DeleteItem(hwndTree, hitem);                     //Удаление имени в дереве после удаления всего вложенного
// Надо бы еще удалить имя в моем массиве дерева aTree, хотя возможно это не имеет значения
#if !defined EMULATOR_HDD                                    //Режим эмулятора
   if(Save_FAT1() < 0) return -1;                            //Сохранение обновленной FAT первого раздела
   if(Change_Sec67(num_del_Cl) < 0) return -1;               //Внесение изменений в содержимое сектора 67
#endif
   writeCl -= num_del_Cl;                                    //Число записанных кластеров
   ViewSize();                                               //Показа дискового пространства
   return 0;
}

//------------------------------------------------------------------------------

#if !defined EMULATOR_HDD                                    //НЕТ Режима эмулятора
int WriteSec(BYTE *buff)                                     //Запись сектора
{
   DWORD nb;
   if(WriteFile(hDrive, buff, sSecB, &nb, NULL) == FALSE || nb != sSecB)
      return ErrorSys1((Lan+175)->msg);                      //Ошибка при записи кластера. (хотя правильно: Ошибка при записи сектора.)
   return 0;
}
#endif

//------------------------------------------------------------------------------

#if !defined EMULATOR_HDD                                    //НЕТ Режима эмулятора
int Change_Sec67(int num_Cl)                                 //Внесение изменений в содержимое сектора 67
{
   SEC_67 Sec67;
   LONGLONG Poz = LONGLONG(sSecB) * 67;
   SetInFilePointer(Poz);                                    //Встали в начало 67 сектора
   ReadSec((BYTE *)&Sec67);                                  //Чтение 67 сектора
   Sec67.FreeCl += num_Cl;                                   //Изменили число свободных кластеров
   if(MaxClast+1 > Sec67.LastCl) Sec67.LastCl = MaxClast+1;  //Номер самого старшего свободного кластера
   SetInFilePointer(Poz);                                    //Встали в начало 67 сектора
   return WriteSec((BYTE *)&Sec67);                          //Запись сектора
}
#endif

//------------------------------------------------------------------------------

int  Correct67Sec(void)                                      //Коррекция содержимого 67 сектора
{
   int n;
   for(n=maxZapFAT1-1; n>1; n--)
     if(*(FAT1+n) != 0) break;
   int FreeFat = maxZapFAT1 - 2;                             //Число свободных кластеров
   for(int i=2; i<n; i++)                                    //n+1 - Первый кластер неиспользуемой области FAT
     if(*(FAT1+i) != 0)  FreeFat--;
   SEC_67 Sec67;
   LONGLONG Poz = LONGLONG(sSecB) * 67;
   SetInFilePointer(Poz);                                    //Встали в начало 67 сектора
   ReadSec((BYTE *)&Sec67);                                  //Чтение 67 сектора
// char ss1[300], ss2[300];
// sprintf(ss1, "Вычислено: Свободно - %8d,  Первый неиспользуемый - %8d", FreeFat, n+1);
// sprintf(ss2, "Записано:  Свободно - %8d,  Первый неиспользуемый - %8d", Sec67.FreeCl, Sec67.LastCl);
// Message2(ss1, ss2);
   if(DWORD(FreeFat) == Sec67.FreeCl && DWORD(n+1) >= Sec67.LastCl)
   {   Message1((Lan+194)->msg);
       return 0;
   }
   Sec67.FreeCl = FreeFat;
   if(DWORD(n+1) > Sec67.LastCl)
      Sec67.LastCl = n + 1;
   SetInFilePointer(Poz);                                    //Встали в начало 67 сектора
   if(WriteSec((BYTE *)&Sec67) != -1)                        //Запись сектора
      Message1((Lan+195)->msg);
   return 0;
}

#endif
#endif

