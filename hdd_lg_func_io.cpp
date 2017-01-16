
//                                    07.12.2008
#ifndef __HDD_LG_FUNC_IO__
#define __HDD_LG_FUNC_IO__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_func_io ==================================
void ClearDopDlg(void);                                      //Очистка параметров дополнительного диалога
int  SetInFilePointer(LONGLONG Poz);                         //Изменение позиции указателя в файле (на физическом диске)
void CloseFile(HANDLE *File);                                //Закрыли файл (физический диск)
int  GetName_OutFile(char *NameF);                           //Ввод имени для записи файла
int  CtrlFileYesNo(char *NameF);                             //Проверка наличия файла
int  DiskFreeSpaceEx_F(char *Name_Dir, DWORDLONG Size);      //Проверка свободного места
int  Open_OutFileAlw(char *Name, HANDLE *File);              //Открытие выходного файла
//int  Open_FileR(char *Name, HANDLE *File, int *nn);          //Открытие существующего файла
//11int  Open_OutFile512(char *Name, HANDLE *File);          //Открытие выходного файла
int  Open_OutFile(char *Name, HANDLE *File);                 //Открытие выходного файла
int  DiskFreeSpaceEx(char *NameF, DWORDLONG Size);           //Проверка свободного места
     ModifDialog ModDlg;                                     //Для модификации диалога
     WIN32_FIND_DATA DataF;                                  //Параметры файла если он есть

//------------------------------------------------------------------------------

void ClearDopDlg(void)                                       //Очистка параметров дополнительного диалога
{
   ModDlg.Flags = 0;                                         //Дополнительные флаги
   ModDlg.fnHook = 0;                                        //Функция обработки дополнительного диалога
   ModDlg.DopDlgName = 0;                                    //Дополнительный диалог который суммируется с основным
}

//------------------------------------------------------------------------------

int SetInFilePointer(LONGLONG Poz)                           //Изменение позиции указателя в файле (на физическом диске)
{
   LARGE_INTEGER TPoz;

   TPoz.QuadPart = Poz;
   TPoz.u.LowPart = SetFilePointer(hDrive, TPoz.u.LowPart, &TPoz.u.HighPart, FILE_BEGIN);
   if(TPoz.u.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
      return ErrorSys1((Lan+41)->msg);                       //"Ошибка при позиционировании по диску."
   return 0;
}

//------------------------------------------------------------------------------

void CloseFile(HANDLE *File)                                 //Закрыли файл (физический диск)
{
   if(*File == 0) return;
   CloseHandle(*File);
   *File = 0;
}

//------------------------------------------------------------------------------

int GetName_OutFile(char *NameF)                             //Ввод имени для записи файла
{
   OPENFILENAME ofn;
   char DirName[260];                                        //Начальный путь для поиска файлов
   char FileTitle[260];                                      //Заголовок окна
   char Filter[260];                                         //Фильтры поиска
   char *Title = (Lan+42)->msg;                              //"Укажите имя записываемого файла"
   char NameFile[260];
   char Driv[_MAX_DRIVE], Dir[_MAX_PATH], Name[_MAX_FNAME], Ext[_MAX_EXT];

   strcpy(NameFile, NameF);                                  //Начальный путь для поиска файлов
   *DirName = 0;                                             //На случай если имени небыло
   if(*NameFile != 0)                                        //Имя уже было
   {  _splitpath(NameFile, Driv, Dir, Name, Ext);
      _makepath(DirName, Driv, Dir, NULL, NULL);
      _makepath(NameFile, NULL, NULL, Name, Ext);
      if(*Name == '*') *Name = 0;                            //Если нет конкретного имени
   }
   sprintf(Filter, "%s *%s|*%s|%s (*.*)|*.*", (Lan+43)->msg, Ext, Ext, (Lan+44)->msg); //Фильтры поиска //sprintf(Filter, "Файлы *%s|*%s|Все файлы (*.*)|*.*", Ext, Ext);
   for(int i=0; Filter[i]!='\0'; i++)                        //Заменили разделитель 0
     if(Filter[i] == '|') Filter[i] = '\0';
   ZeroMemory(&ofn, sizeof(OPENFILENAME));                   //Заполнили нулями
   ofn.lStructSize = sizeof(OPENFILENAME);                   //Длина структуры
   ofn.hwndOwner = MainWin;                                  //Владелец диалога
   ofn.hInstance = MainInst;                                 //Идентификатор программы владеющая диалогом
   ofn.lpstrFilter = Filter;                                 //Типы просматриваемых файлов
//     lpstrCustomFilter                                     //Специальные фильтры
//     nMaxCustFilter                                        //Длина специального фильтра
   ofn.nFilterIndex = 1;                                     //Индекс для работы с фильтрами
   ofn.lpstrFile = NameFile;                                 //Имя файла в случае успеха
   ofn.nMaxFile = sizeof(NameFile);                          //Длина поля имени файла
   ofn.lpstrFileTitle = FileTitle;                           //Текст заголовка
   ofn.nMaxFileTitle = sizeof(FileTitle);                    //
   ofn.lpstrInitialDir = DirName;                            //Начальный каталог файлов
   ofn.lpstrTitle = Title;
   ofn.Flags = ModDlg.Flags | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
// ofn.nFileOffset;                                          //Смещение в внутри lpstrFile
// ofn.nFileExtension;                                       //Смещение в внутри lpstrFile
// ofn.lpstrDefExt = ".*";
// ofn.lCustData;                                            //Для функции обработчиков
   ofn.lpfnHook = ModDlg.fnHook;                             //Функция обработки дополнительного диалога
   ofn.lpTemplateName = ModDlg.DopDlgName;                   //Дополнительный диалог который суммируется с основным
   if(GetSaveFileName(&ofn) == FALSE || *NameFile == 0)      //Функция диалога
   {   *NameF = 0;    return -1;  }
   strcpy(NameF, NameFile);                                  //Переслали имя файла
   return 0;
}

//------------------------------------------------------------------------------

int CtrlFileYesNo(char *NameF)                               //Проверка наличия файла
{
// WIN32_FIND_DATA DataF;
   HANDLE  File;

   if((File = FindFirstFile(NameF, &DataF)) == INVALID_HANDLE_VALUE) return 0; //Файла нет                                              //Файл не найден
   FindClose(File);
   return 1;                                                 //Файл есть
}

//------------------------------------------------------------------------------

int Open_OutFileAlw(char *Name, HANDLE *File)                //Открытие выходного файла
{
  *File = CreateFile(Name,                                   //Адрес строки с именем файла
                     GENERIC_WRITE,                          //Способ доступа к файлу
                     0,                                      //Посторонний процес не получит доступа
                     NULL,                                   //Адрес структуры с параметрами защиты ядра
                     CREATE_ALWAYS,                          //Тип открытия файла
                     FILE_ATTRIBUTE_NORMAL,                  //Атрибуты файла
                     NULL);                                  //Используется для временных файлов
   if(*File == INVALID_HANDLE_VALUE)
   {  *File = NULL;
      return ErrorSys2((Lan+82)->msg, Name);                 //"Ошибка при открытии выходного файла"
   }
   return 0;
}

//------------------------------------------------------------------------------
/*
int Open_FileR(char *Name, HANDLE *File, int *nn)            //Открытие существующего файла
{
  *File = CreateFile(Name,                                   //Адрес строки с именем файла
                     GENERIC_READ | GENERIC_WRITE,           //Способ доступа к файлу
                     0,                                      //Посторонний процес не получит доступа
                     NULL,                                   //Адрес структуры с параметрами защиты ядра
                     OPEN_EXISTING,                          //Тип открытия файла
                     FILE_ATTRIBUTE_NORMAL,                  //Атрибуты файла
                     NULL);                                  //Используется для временных файлов
   if(*File == INVALID_HANDLE_VALUE)
   {  *File = NULL;
      if(GetLastError() == 32)                               //Файл еще занят
      { (*nn)++;
        Sleep(250);                                          //Задержка на 250 мс
        if(*nn < 100)                                        //Надеюсь, что за 25 секунд файл освободится
           return Open_FileR(Name, File, nn);                //Еще раз Открытие существующего файла
      }
      return ErrorSys2((Lan+82)->msg, Name);                 //"Ошибка при открытии выходного файла"
   }
   return 0;
}
*/
//------------------------------------------------------------------------------

//11int Open_OutFile512(char *Name, HANDLE *File)                //Открытие выходного файла
int Open_OutFile(char *Name, HANDLE *File)                //Открытие выходного файла
{
  *File = CreateFile(Name,                                   //Адрес строки с именем файла
                     GENERIC_WRITE,                          //Способ доступа к файлу
                     0,                                      //Посторонний процес не получит доступа
                     NULL,                                   //Адрес структуры с параметрами защиты ядра
                     CREATE_NEW,                             //Тип открытия файла (не должно быть)
//11                 FILE_FLAG_NO_BUFFERING |
           //        FILE_FLAG_SEQUENTIAL_SCAN |
                     FILE_ATTRIBUTE_NORMAL,                  //Атрибуты файла
                     NULL);                                  //Используется для временных файлов
   if(*File == INVALID_HANDLE_VALUE)
   {  *File = NULL;
      return ErrorSys2((Lan+82)->msg, Name);                 //"Ошибка при открытии выходного файла"
   }
   return 0;
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL CALLBACK Dlg_FileSize(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch(Message)
   {
      case WM_INITDIALOG:
             CenterDlg(hDlg, 0);                             //Центрирование окна диалога в главном окне
             SetWindowText(hDlg, (Lan+36)->msg);             //Вывели новый заголовок  ВНИМАНИЕ!
             SetDlgItemText(hDlg, IDC_STATICTEXT1, (Lan+50)->msg); //На диске указанном для записи недостаточно свободного места.
             SetDlgItemText(hDlg, IDC_STATICTEXT3, (Lan+48)->msg); //Доступно
             SetDlgItemText(hDlg, IDC_STATICTEXT4, FileSize_Txt(*(DWORDLONG*)(lParam)));
             SetDlgItemText(hDlg, IDC_STATICTEXT5, (Lan+47)->msg); //байт
             SetDlgItemText(hDlg, IDC_STATICTEXT6, (Lan+49)->msg); //Требуется
             SetDlgItemText(hDlg, IDC_STATICTEXT7, FileSize_Txt(*((DWORDLONG*)(lParam) + 1)));
             SetDlgItemText(hDlg, IDC_STATICTEXT8, (Lan+47)->msg); //байт
             SetDlgItemText(hDlg, IDC_STATICTEXT9, (Lan+52)->msg); //Хотите указать другой путь ?
             SetDlgItemText(hDlg, IDOK, (Lan+45)->msg);            //Да
             SetDlgItemText(hDlg, IDCANCEL, (Lan+51)->msg);        //Нет
             return TRUE;
      case WM_CTLCOLORSTATIC:
             if(GetDlgCtrlID(HWND(lParam)) == IDC_STATICTEXT1)
             {  SetTextColor((HDC)wParam, RGB(255, 0, 0));
                SetBkColor((HDC)wParam, FonLTGRAY);
                return (BOOL)FonBrush;
             }
             return TRUE;
      case WM_COMMAND:
             switch(LOWORD(wParam))
             {  case IDOK:
                case IDCANCEL: EndDialog(hDlg, LOWORD(wParam));    //Вернули один из двух кодов
                               return TRUE;
             }
             break;
   }
   return FALSE;
}

//------------------------------------------------------------------------------

int DiskFreeSpaceEx(char *NameF, DWORDLONG Size)             //Проверка свободного места
{
    ULARGE_INTEGER FreeBytes, TotalBytes, TotalFreeBytes;
    char   Driv[_MAX_DRIVE], Dir[_MAX_PATH], Name[_MAX_FNAME], Ext[_MAX_EXT];
    char   Path[_MAX_PATH];

    _splitpath(NameF, Driv, Dir, Name, Ext);
    lstrcpy(Path, Driv);
    lstrcat(Path, "\\");

    if(GetDiskFreeSpaceEx(Path, &FreeBytes,                  //Число свободных доступных байт (максимальный размер файла)
                                &TotalBytes,                 //Общее число байт на диске
                                &TotalFreeBytes) == FALSE)   //Общее число свободных байт на диске (сумма всех свободных мест)
       return ErrorSys1((Lan+46)->msg);                      //"Системная ошибка при запросе емкости диска."
    if(FreeBytes.QuadPart < Size)
    {  DWORDLONG size[2];
       size[0] = FreeBytes.QuadPart;
       size[1] = Size;
       if(DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_FILE_SIZE),
          MainWin, Dlg_FileSize, LPARAM(size)) == IDCANCEL) return -1;
       return 1;
    }
    if(Size <= 0xFFFFFFFF-2) return 0;                       //Предельный размер для FAT32 и наш файл меньше

    char FileSystemName[260];
    GetVolumeInformation(Path, NULL, 0, NULL, 0, NULL,
                         FileSystemName, sizeof(FileSystemName));
    if(lstrcmp(FileSystemName, "FAT32") != 0) return 0;
  //if(MsgYesNo3("Для записи указан диск с файловой системой FAT32",
  //             "максимальный размер файла в которой 4ГБ - 2",
  //             "Хотите указать другой путь ?") == 'N') return -1;
    if(MsgYesNo3((Lan+53)->msg, (Lan+54)->msg, (Lan+55)->msg) == 'N') return -1;
    return 1;                                                //Места нет
}

//------------------------------------------------------------------------------

int DiskFreeSpaceEx_F(char *Name_Dir, DWORDLONG Size)        //Проверка свободного места
{
   ULARGE_INTEGER FreeBytes, TotalBytes, TotalFreeBytes;
   char Driv[_MAX_DRIVE], Dir[_MAX_PATH], Name[_MAX_FNAME], Ext[_MAX_EXT], Path[_MAX_PATH];
   _splitpath(Name_Dir, Driv, Dir, Name, Ext);
   lstrcpy(Path, Driv);
   lstrcat(Path, "\\");
   if(GetDiskFreeSpaceEx(Path, &FreeBytes, &TotalBytes, &TotalFreeBytes) == FALSE)//Общее число свободных байт на диске (сумма всех свободных мест)
       return ErrorSys1((Lan+46)->msg);                      //"Системная ошибка при запросе емкости диска."

   if(TotalFreeBytes.QuadPart >= Size)  return 0;
   DWORDLONG size[2];
   size[0] = TotalFreeBytes.QuadPart;
   size[1] = Size;
   if(DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_FILE_SIZE),
          MainWin, Dlg_FileSize, LPARAM(size)) == IDCANCEL) return -1;
   return 1;
}

#endif

