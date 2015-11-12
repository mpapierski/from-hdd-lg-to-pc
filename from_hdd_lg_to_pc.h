
//                                 16.04.2010
#ifndef __HDD_LG_TO_PC_H__
#define __HDD_LG_TO_PC_H__

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#include <dir.h>
#pragma hdrstop
#include "defparam.h"
#include "mme_db_head.h"
#include "from_hdd_lg_to_pc_def.h"

//==============================================================================
   extern HINSTANCE MainInst;                                //Идентификатор приложения
   extern HWND  MainWin;                                     //Главное окно
   extern HWND  hwndTree;                                    //Окно дерева
   extern DWORD FonLTGRAY;                                   //Фоновый цвет
   extern HBRUSH FonBrush;                                   //Фоновая кисть
   extern HFONT MyFont, MyFont_Cur, MyFontSm;
   extern HWND hCopy, hConf, hClose, hClear, hSizeHDD;       //Окна кнопок
   extern HWND hMap, hToTxt, hPrShift;                       //Окна кнопок
   extern HWND hNumSel, hSizeSel, hAbout;                    //Окна кнопок
   extern HWND hWriteFi, hWriteFo, hNew_Fo, hRenFi, hDelFi;  //Кнопки для записи
   extern HWND hCorr67;
   extern int prHDD_LG_Yes;                                  //Признак, присутствия HDD: 0-есть, 1-не найден
   extern const char *NVER_TXT;
   extern char *AppName;

//============================ config_io =======================================
int  WriteConfig(void);                                      //Запись файла конфигурации
int  LoadConfig(void);                                       //Загрузка конфигурации
void NewConfig(void);                                        //Изменение конфигурации
     extern LANG *Lan;
     extern CONF Conf;                                       //Параметры

//============================ hdd_lg_copy =====================================
int  CopyFiles(void);                                        //Выполнение копирования
     extern DWORD num_Sel;                                   //Число выбранных имен при групповом выборе
     extern LONGLONG size_Sel;                               //Суммарный размер выбранных файлов
     extern DWORD indF;                                      //Индекс копируемого файла

#if defined TEST_FAT1                                        //Проверка FAT1
//============================ hdd_lg_ctrl_fat =====================================
int  Ctrl_FAT1(void);                                        //Проверка структуры служебных записей
int  Vost_FAT1(void);                                        //Чистка FAT1 от потерянных кластеров
     extern DWORD *cTestFAT1;                                //Копия таблицы FAT первого раздела
     extern DWORD sumClTestFAT1;                             //Суммарное количество занятых кластеров при тестировании FAT1
#endif

#if defined EMULATOR_HDD
//============================ hdd_lg_emulator =================================
int ReadClastEmul(DWORDLONG Poz, BYTE *buff);                //Чтение кластера эмулятором
int ReadClast2Emul(DWORDLONG Poz, BYTE *buff);               //Чтение кластера эмулятором
int ReadFATEmul(DWORDLONG Poz);                              //Чтение FAT эмулятором
int ReadFAT2Emul(DWORDLONG Poz);                             //Чтение FAT эмулятором
int Read_Sec_Emul(int Num, BYTE* Sec);                       //Чтение сектора по номеру эмулятором
int FindHDD_LG_Emul(void);                                   //Поиск диска из рекордера LG эмулятором
    extern char *inBuf;
    extern char NameFDump[];                                 //Имя файла с дампом
#endif

#if defined FIND_NO_ZERO_SEC
//============================ hdd_lg_find_zero ================================
int FindNoZeroSec(void);                                     //Поиск не нулевых секторов и создание списка
#endif

//============================ hdd_lg_func =====================================
int  CreateMyFont(void);                                     //Создание фонта
char *Char_Dig_p(char *CharDig, int N);                      //Преобразование символьного числа в разрядку
char *Char_Dig_p_n(char *CharDig, int N, int n);             //Преобразование символьного числа в разрядку с "n" знаками после запятой
char *FileSize_Txt(DWORDLONG Size);                          //Размер файла в текст
void CenterDlg(HWND hDlg, int yPoz);                         //Центрирование окна диалога в главном окне
void OutNameDlg(HWND hDlg, int ID_T, char *inText);          //Усечение имени
void UnicodeToAnsi(WCHAR *inUNI, char *outANSI, int maxN);   //Преобразование Unicode в символы
void Open_Wait_Found_HDD(void);                              //Окошко со словами подождите
void Close_Wait_Found_HDD(void);                             //Окошко со словами подождите
int  Ctrl_Adm(void);                                         //Проверка наличия прав администратора
void About(void);
void AddToSpecSpis_VerWin(void);                            //Запись в дамп версии Windows

//============================ hdd_lg_func_1 ===================================
void *MyAllocMem(DWORD Size);                                //Запрос динамической памяти
void MyFreeMem(void **Adr);                                  //Освобождение блока памяти
int  Error1(char *Msg);
int  Error2(char *Msg1, char *Msg2);
int  Error3(char *Msg1, char *Msg2, char *Msg3);
int  Error4(char *Msg1, char *Msg2, char *Msg3, char *Msg4);
int  ErrorSys1(char *Msg);
int  ErrorSys2(char *Msg1, char *Msg2);
char MsgYesNo1(char *Msg1);
char MsgYesNo2(char *Msg1, char *Msg2);
char MsgYesNo3(char *Msg1, char *Msg2, char *Msg3);
char MsgYesNo4(char *Msg1, char *Msg2, char *Msg3, char *Msg4);
char MsgYesNo5(char *Msg1, char *Msg2, char *Msg3, char *Msg4, char *Msg5);
void Message1(char *Msg1);
void Message2(char *Msg1, char *Msg2);

//============================ hdd_lg_func_io_folder ===========================
int  Get_Name_Dir(char *msg, char *Name_Dir, int pr);        //Запрос имени папки

//============================ hdd_lg_func_io =====================================
void ClearDopDlg(void);                                      //Очистка параметров дополнительного диалога
int  SetInFilePointer(LONGLONG Poz);                         //Изменение позиции указателя в файле (на физическом диске)
void CloseFile(HANDLE *File);                                //Закрыли файл (физический диск)
int  GetName_OutFile(char *NameF);                           //Ввод имени для записи файла
int  CtrlFileYesNo(char *NameF);                             //Проверка наличия файла
int  DiskFreeSpaceEx_F(char *Name_Dir, DWORDLONG Size);      //Проверка свободного места
int  Open_OutFileAlw(char *Name, HANDLE *File);              //Открытие выходного файла
//int  Open_FileR(char *Name, HANDLE *File, int *nn);        //Открытие существующего файла
//11int  Open_OutFile512(char *Name, HANDLE *File);          //Открытие выходного файла
int  Open_OutFile(char *Name, HANDLE *File);                 //Открытие выходного файла
int  DiskFreeSpaceEx(char *NameF, DWORDLONG Size);           //Проверка свободного места
     extern ModifDialog ModDlg;                              //Для модификации диалога
     extern WIN32_FIND_DATA DataF;                           //Параметры файла если он есть

//============================ hdd_lg_func_io_1 ================================
//11int  OpenOutFile(char *Name, LONGLONG Size, HANDLE *File);    //Открытие выходного файла
void Ctrl_Name_Ch(char *Name);                               //Проверка имени на недопустимые символы и замена недопустимых символов
int  OpenOutFileName_New(char *Name, LONGLONG Size, HANDLE *File);//Открытие выходного файла
int  OpenOutFileName_Yes(char *Name, LONGLONG Size, HANDLE *File);//Открытие выходного файла
     extern int prCopy;                                      //Признак действия при групповом копировании
     extern char ndS[9];                                     //Перечень недопустимых символов
   //extern int  pr_ndS;                                     //Признак выполненной замены недопустимого символа. 1-была замена

//============================ hdd_lg_language =================================
  extern LANG Lan_EN[];
  extern LANG Lan_RU[];
  extern const int numStrLan;                                //Число языковых строк
  extern LANG Lan_Any[];                                     //Произвольный язык

//============================ hdd_lg_make_tree ================================
#if defined WRITE_YES                                        //Режим записи разрешен
int AddItemToTree_forWrite(char *Name, char *Ext, PAR_FILE *pf);   //Добавление новой строки в дерево при записи
#endif
int  AddItemToTree(char *Name, char *Ext, PAR_FILE *pf, int Level);//Добавление новой строки в дерево
void Expand_Tree(int hitem);                                 //Распахивание дерева
int  ClearSelect(void);                                      //Снятие всех выделений
int  RemoveSelectionFromCopy(int n);                         //Снятие выделение файла в дереве при копировании
bool ChangeSelect(int sShift);                               //Изменение выбора файлов в дерева
bool ChangeSelect_and_Down(void);                            //Изменение выбора файлов в дерева
void ViewSize(void);                                         //Показ дискового пространства
     extern int numEl_Tree;                                  //Число элементров в дереве имен
     extern OneStrTree *aTree;                               //Информация по дереву
     extern HTREEITEM hPrev[MAX_U];                          //Массив уровней вложенности дерева

//============================ hdd_lg_map_hdd ==================================
int  View_Map_HDD(void);                                     //Карта занятости HDD
BOOL Register_MapHDDWin(void);                               //Регистрация окна

//============================ hdd_lg_mk_name ==================================
int  MakeOneStrForTree(char *oneStr, char *Name, char *Ext, PAR_FILE *pf);//Создание строки символов для отражения в дереве
int  ViewAbsendName(void);                                   //Показ имен присутствующих в файле MME.DB, но отсутствующих в каталоге

//============================ hdd_lg_mme_db ===================================
int  CtrlSize_MME(DWORD Delta);                              //Контроль смещения в массиве MME
int  CtrlRecoder(void);                                      //Идентификация рекордера
int  Read_Dir_Part2_MME(void);                               //Чтение каталога второго раздела и файла MME
int  ReadMME_DB(void);                                       //Чтение файла базы записей
int  Update_MME_DB(void);                                    //Пересоздание массива информативных имен
     extern BYTE *MMe;                                       //Содержимое файла базы
     extern DWORD SizeMME;                                   //Число байт в файле MME
     extern TABL_MME *tabMME;                                //Таблица соответствия имен
     extern DWORD numNam;                                    //Число имен в таблице MME
     extern TABL_MME *tabMME_Part;                           //Таблица имен отредактированных Title
     extern DWORD numNam_Part;                               //Число имен в таблице имен отредактированных Title

#if defined OUT_TEST
//============================ hdd_lg_out_test =====================================
void Add_SpecSpis(char *Str);                                //Добавление строки в список
void View_HEX_Any(BYTE *buff, DWORD Size);                   //Вывод шеснадцатиричных кодов
HWND Create_SpecWin(void);                                   //Создание окна
BOOL Register_SpecWin(void);                                 //Регистрация окна
#endif

//============================ hdd_lg_read =====================================
int  Update_Tree(void);                                      //Пересоздание дерева файлов и папок при изменении настроек
int  Read_HDD(void);                                         //Поиск HDD из рекордера LG и загрузка служебной информации
     extern HANDLE hDrive;                                   //Указатель на диск
     extern DWORD *FAT1;                                     //Таблица FAT первого раздела
     extern DWORD Start_SecDir1;                             //Позиция начала первого каталога первого раздела
     extern DWORD Start_SecFAT1;                             //Позиция начала таблицы FAT первого раздела
     extern DWORD num_SecFAT1;                               //Число секторов в FAT
     extern DWORD Size_FAT1;                                 //Число байт в FAT
     extern DWORD maxZapFAT1;                                //Максимальное число записей в FAT
     extern DWORD *FAT2;                                     //Таблица FAT второго раздела
     extern DWORD Start_SecDir2;                             //Позиция начала первого каталога второго раздела
     extern DWORD Start_SecFAT2;                             //Позиция начала таблицы FAT второго раздела
     extern DWORD num_SecFAT2;                               //Число секторов в FAT второго раздела
     extern DWORD Size_FAT2;                                 //Число байт в FAT второго раздела
     extern DWORD maxZapFAT2;                                //Максимальное число записей в FAT второго раздела
     extern int indTabMME;                                   //Индекс в таблице MME.DB для текущего файла в папке MEDIA
     extern DWORD writeCl;                                   //Число занятых кластеров
     extern int prMEDIA;                                     //Признак обрабатываемой папка
     extern int pr_tRec;                                     //Признак рекордера 0 или 1 (старейшая серия)

//============================ hdd_lg_read_io ==================================
int  ReadSec(BYTE *buff);                                    //Чтение сектора
int  ReadClast1(BYTE *buff);                                 //Чтение кластера первого раздела начиная с текущего сектора
int  ReadClast1_P(DWORD nSector, BYTE *buff);                //Чтение кластера первого раздела начиная с указанного сектора
int  ReadClast2(BYTE *buff);                                 //Чтение кластера второго раздела начиная с текущего сектора
int  ReadClast2_P(DWORD nSector, BYTE *buff);                //Чтение кластера второго раздела начиная с указанного сектора
int  Load_FAT1(void);                                        //Загрузка FAT первого раздела
int  Load_FAT2(void);                                        //Загрузка FAT второго раздела
int  FindHDD_LG(void);                                       //Поиск диска из рекордера LG
     extern DWORD stCl_Media2;                               //Кластер начала второй части папки MEDIA

//============================ hdd_lg_remake_tree ==============================
int  RemakeTree(void);                                       //Переформирование дерева с учетом многофайловых Titles

#if defined TEST_67_68_70_71                                 //Сектора
//============================ hdd_lg_sect_68_68_70_71 =========================
int  Read_Sec_67_68_70_71(void);                             //Чтение специальных секторов
#endif

#if defined TEST_IDX
//============================ hdd_lg_test_idx =================================
int Read_File_idx(void);
#endif

#if defined TEST_WIN
//============================ hdd_lg_test0 ====================================
void View_Sec_Hex(BYTE *buff, int pr);                       //Вывод содержимого сектора
void Add_Spis(char *Str);                                    //Добавление строки в список
HWND Create_TestWin(void);                                   //Создание окна
BOOL Register_TestWin(void);                                 //Регистрация окна
#endif

#if defined TEST_LG1
//============================ hdd_lg_test1 ====================================
int  Read_Test1(void);                                       //Тестовое чтение и вывод
#endif

#if defined TEST_LG2_MME
//============================ hdd_lg_test2 ====================================
int  Read_Test2(void);                                       //Тестовое чтение и вывод
#endif

//============================ hdd_lg_to_txt =====================================
int OutTxt(void);                                            //Запись дерева в текстовый файл

#if defined WRITE_YES                                        //Режим записи разрешен
//============================ hdd_lg_write ====================================
int  WriteFileToHDD(int pr);                                 //Запись файлов на диск
     extern int prFolder;                                    //Вариант расширения файла (0-mp3, 1-jpg, 2-avi)
     extern DWORD ClStDir;                                   //Номер кластера каталога в который выполняется запись
     extern TV_ITEM item_Add;                                //Выбор в строке дерева куда будет добавление
     extern int NumMsg38;                                    //Число сообщений о превышении длины имени

//============================ hdd_lg_write_1 ==================================
int  File_ToHDD(void);                                       //Запись файлов на диск LG
int  Folder_ToHDD(void);                                     //Запись папок на диск LG
int  New_Folder_ToHDD(void);                                 //Cоздание папок на диске LG
     extern char *spisF;                                     //Память списка файлов

//============================ hdd_lg_write_2 ==================================
BOOL CALLBACK Dlg_NewName(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int  RenameFiFo(HTREEITEM hitem, OneStrTree *aTree);         //Переименование
int  DeleteFiFo(HTREEITEM hitem, OneStrTree *aTree);         //Удаление
int  Change_Sec67(int num_Cl);                               //Внесение изменений в содержимое сектора 67
int  Correct67Sec(void);                                     //Коррекция содержимого 67 сектора
     extern char NameFoFi[];                                 //Имя создаваемой папки
     extern DWORD MaxClast;                                  //Номер самого старшего кластера использованного для записи

//============================ hdd_lg_write_io =================================
int  GetSpNameInFile(char *Spis, int N, int *sm);            //Ввод списка имен для записи
int  OpenInFilePC(char *NameF);                              //Открыли входной файл для чтения с компьютера
int  WriteClast1P(DWORD nClast, BYTE *buff);                 //Запись кластера
int  Save_FAT1(void);                                        //Сохранение обновленной FAT первого раздела
void Ansi_To_Unicode(char *inANSI, WCHAR *outUNI, int maxN); //Преобразование строки символов в Unicode
int  Load_Dir(void);                                         //Загрузка кластера каталога
int  Change_Dir_For_File(char *NameF);                       //Изменение каталога (внесение нового файла)
int  Change_Dir_For_Folder(char *NameF);                     //Изменение каталога (внесение новой папки)
int  Save_Dir(void);                                         //Сохранение кластера каталога
    extern DWORD *c_FAT1;                                    //Копия таблицы FAT первого раздела
    extern HANDLE inFile;
    extern ULARGE_INTEGER Size_inF;                          //Размер файла м.б. > 4 ГБ
    extern int nCl_1;                                        //Номер кластера начала файла
    extern BYTE bufDir[];                                    //Память под один кластер
#endif

//============================ ProgressBar =====================================
void EnableW(bool T_F);                                      //Изменение активности кнопок
void InitProgressBar1(DWORD Size, char *Msg);
int  ProgressBar1(DWORD Size);
void Close_ProgressBar1(void);
void InitProgressBar1_2(DWORD Size, char *Msg);              //Функция прогресса для одного файла
void InitProgressBar2_2(DWORD Size, char *Msg);              //Функция прогресса для нескольких файлов
int  ProgressBar2(DWORD Size);
void ScipProgressBar2(DWORDLONG SizeF);                      //Отработка прогресса для пропускаемых файлов
void Close_ProgressBar1_2(void);
void Close_ProgressBar2_2(void);
     extern void (*InitProgressBar)(DWORD Size, char *Msg);
     extern int  (*ProgressBar)(DWORD Size);
     extern void (*Close_ProgressBar)(void);
     extern bool prProcCopy;                                 //Признак процесса копирования
     extern int  prWinMinMax;                                //0-окно распахнутое, 1-окно свернутое
     extern DWORD tStart2;

#endif

