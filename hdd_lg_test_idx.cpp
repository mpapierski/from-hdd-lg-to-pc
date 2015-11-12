
//                                    11.08.2008
#ifndef __HDD_LG_TEST_IDX__
#define __HDD_LG_TEST_IDX__

#include "from_hdd_lg_to_pc.h"

#if defined TEST_IDX

//============================ hdd_lg_test_idx =================================
int Read_File_idx(void);

typedef struct { DWORD A[12];} FILE_IDX;
static FILE_IDX *inBufIdx;
static DWORD inSizeIdx;

//------------------------------------------------------------------------------

static int OpenInFile(char *NameF, HANDLE *inFile)                           //Открытие файла
{
   *inFile = CreateFile(NameF,                                //Адрес строки с именем файла
                       GENERIC_READ,                         //Способ доступа к файлу
                       FILE_SHARE_READ,                      //Посторонний процес может только читать файл
                       NULL,                                 //Адрес структуры с параметрами защиты ядра
                       OPEN_EXISTING,                        //Тип открытия файла
                       FILE_ATTRIBUTE_NORMAL,                //Атрибуты файла
                       NULL);                                //Используется для временных файлов
   if(*inFile == INVALID_HANDLE_VALUE)
     return ErrorSys2("Ошибка при открытии файла", NameF);
   inSizeIdx = GetFileSize(*inFile, NULL);
   return 0;
}

//------------------------------------------------------------------------------

static int GetName_InFile(char *NameF)                       //Ввод имени обрабатываемого файла
{
   OPENFILENAME ofn;
   char DirName[260];                                        //Начальный путь для поиска файлов
   char FileTitle[256];                                      //Заголовок окна
   char Filter[256];                                         //Фильтры поиска
   char NameFile[260];

   strcpy(NameFile, NameF);                                  //Начальный путь для поиска файлов
   *DirName = 0;                                             //На случай если имени небыло
   if(*NameFile != 0)                                        //Имя уже было
   {  char *A;
      lstrcpy(DirName, NameFile);                            //Начальный путь для поиска файлов
      //Наверно логично создать полное имя
      A = strrchr(DirName, '\\');                            //Нашли последний разделитель
      if(A != NULL) *A = 0;
      else *DirName = 0;                                     //Нет имени файла
   }
   *NameFile = 0;
   lstrcpy(Filter, "Файлы idx|*.idx|Все файлы (*.*)|*.*|");
   for(int i=0; Filter[i]!=0; i++)                           //Заменили разделитель 0
     if(Filter[i] == '|') Filter[i] = 0;
   ZeroMemory(&ofn, sizeof(OPENFILENAME));                   //Заполнили нулями
   ofn.lStructSize = sizeof(OPENFILENAME);                   //Длина структуры
   ofn.hwndOwner = MainWin;                                  //Владелец диалога
   ofn.hInstance = MainInst;                                 //Идентификатор программы владеющая диалогом
   ofn.lpstrFilter = Filter;                                 //Типы просматриваемых файлов
// ofn.lpstrCustomFilter                                     //Специальные фильтры
// ofn.nMaxCustFilter                                        //Длина специального фильтра
   ofn.nFilterIndex = 1;                                     //Индекс для работы с фильтрами
   ofn.lpstrFile = NameFile;                                 //Имя файла в случае успеха
   ofn.nMaxFile = sizeof(NameFile);                          //Длина поля имени файла
   ofn.lpstrFileTitle = FileTitle;                           //Маршрут и имя файла в случае успеха
   ofn.nMaxFileTitle = sizeof(FileTitle);                    //Длина поля
   ofn.lpstrInitialDir = DirName;                            //Начальный каталог файлов
   ofn.lpstrTitle = "Укажите имя файла дампа";
   ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
// ofn.Flags |= OFN_ENABLESIZING;                            //Можно менять размер окна диалога
// ofn.nFileOffset;                                          //Смещение в внутри lpstrFile
// ofn.nFileExtension;                                       //Смещение в внутри lpstrFile
   ofn.lpstrDefExt = "idx";                                  //Расширение по умолчанию
// ofn.lCustData;                                            //Для функции обработчиков
//   ofn.lpfnHook = ModDlg.fnHook;                           //Функция обработки дополнительного диалога
//   ofn.lpTemplateName = ModDlg.DopDlgName;                 //Дополнительный диалог который суммируется с основным
//   if(NumWinNT != 0)                                       //0-Система Win95/98 или номер NT
//     ofn.FlagsEx = OFN_EX_NOPLACESBAR;                     //Убирает иконку слева
   if(GetOpenFileName(&ofn) == FALSE) return -1;
   if(*NameFile == 0) return -1;
   strcpy(NameF, NameFile);                                  //Переслали имя файла
   return 0;
}

//-------------------------------------------------------------------------------

int Read_File_idx(void)
{
   HANDLE inFile;
   char Name_F_IDX[260];
   DWORD nb;
   if(GetName_InFile(Name_F_IDX) < 0) return -1;              //Ввод имени обрабатываемого файла
   if(OpenInFile(Name_F_IDX, &inFile) < 0) return -1;         //Открытие входного файла
   inBufIdx = (FILE_IDX *)MyAllocMem(inSizeIdx);
   if(inBufIdx == NULL) return -1;
   if(ReadFile(inFile, inBufIdx, inSizeIdx, &nb, NULL) == FALSE || nb != inSizeIdx)
      return ErrorSys2("Ошибка при чтении исходного файла", Name_F_IDX);
   CloseFile(&inFile);                                       //Закрыли входной файл

   char Ss[1024], *as;
   FILE_IDX *aIn = inBufIdx;
   for(DWORD i=0; i<inSizeIdx / sizeof(FILE_IDX); i++, aIn++)
   {   as = Ss;
       for(DWORD j=0; j<12; j++)
       {  SWAP32(&(aIn->A[j]));
          as += sprintf(as, " %08X", aIn->A[j]);
       }
       *as = 0;
       Add_Spis(Ss);                                             //Добавление строки в список
   }
   return 0;
}

#endif

#endif

