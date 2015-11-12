
//                                 16.11.2008
#ifndef __HDD_LG_TO_PC_DEF_H__
#define __HDD_LG_TO_PC_DEF_H__

#define WRITE_YES                                            //Программа с режимом записи

//#define VIEW_DEL                                           //Показ удаленных файлов и папок

//#define OUT_TEST                                           //Версия для создания дампа
//#define FIND_NO_ZERO_SEC                                   //Поиск не нулевых секторов

//#define EMULATOR_HDD_AND_COPY                              //Режим эмулятора с эмуляцией копирования
//#define EMULATOR_HDD                                       //Режим эмулятора

//#define TEST_FAT1                                          //Проверка FAT1 на потерянные кластеры
//#define TEST_FAT1_VOST                                     //Проверка и восстановление FAT1 на потерянные кластеры
//#define TEST_LG1                                           //Первый раздел в служебном виде
//#define TEST_LG1_AND_TS                                    //Первый раздел в служебном виде и папка точка (буфер TimeShift ?)
//#define TEST_LG2_MME                                       //Второй раздел в служебном виде (только файл MME.DB)
//#define TEST_LG2_ALL                                       //Второй раздел в служебном виде (все)
//#define HDD_LG  4                                          //Поиск выключен, фиксированный номер канала на котором HDD
//#define VIEW_TAB_MME                                       //Вывод полного содержимого масива mme.db (в рабочем режиме)
//#define TEST_67_68_70_71                                   //Вывод информации из секторов
//#define TEST_IDX                                           //Изучение файла IDX

#if defined OUT_TEST                                         //Версия для создания дампа
  #undef TEST_67_68_70_71                                    //Сектора
#endif

#if defined TEST_FAT1_VOST                                 //Проверка и восстановление FAT1 на потерянные кластеры
   #define TEST_FAT1                                         //Проверка FAT1 на потерянные кластеры
#endif

#if defined EMULATOR_HDD_AND_COPY                            //Режим эмулятора с эмуляцией копирования
  #define EMULATOR_HDD                                       //Режим эмулятора
#endif
#if defined TEST_LG1_AND_TS                                  //Первый раздел в служебном виде и буфер TimeShift
  #define TEST_LG1                                           //Первый раздел в служебном виде
  #if defined EMULATOR_HDD
    #error TEST_LG1_AND_TS and EMULATOR_HDD
  #endif
  #if defined OUT_TEST
    #error TEST_LG1_AND_TS and OUT_TEST
  #endif
#endif
#if defined TEST_LG2_ALL
  #define TEST_LG2_MME                                       //Второй раздел в служебном виде (только файл MME.DB)
#endif

#if defined EMULATOR_HDD && defined WRITE_YES                //Режим эмулятора не совмстим с запись
    #error WRITE_YES and EMULATOR_HDD
#endif

#if defined TEST_LG1 || defined TEST_LG2_MME || defined VIEW_TAB_MME || defined TEST_67_68_70_71 || defined TEST_IDX
  #define TEST_WIN
#endif

#if defined WRITE_YES                                        //Режим записи разрешен
   #define N_VER_TXT "2.06 beta"                             //Номер версии программы
#else
   #define N_VER_TXT "1.15"                                  //Номер версии программы
#endif

#define TEST_N_VER_TXT "10"                                  //Номер версии дампа

#define MAX_NAME 16000 //2000+2000+2000+2*4096+запас         //Максимальное число имен в дереве
#define MAX_U 64                                             //Максимальная вложенность папок
#define MAX_NAME_MME 4096                                    //Максимальное число имен в файле MME
#define MAX_NAME_S 4096                                      //Максимальное число имен в списке
#define MAX_SP (256*MAX_NAME_S)                              //Размер памяти под список файлов

typedef struct { WORD type; DWORD ClSt; BYTE Sec, Min, Hour, r, Day, Mon; WORD Year;
                 LONGLONG SizeF, lL1, lL2; } PAR_FILE;       //Часть строки каталога
typedef struct { char NameF[128]; PAR_FILE pf; WORD prSel, Level;
                 WORD numPart, indTabMME, indTitle, indFolder, numCopy; } OneStrTree;
typedef struct { LONGLONG SizeF; DWORD timeLong; char NameF[48], aName[72],
                 Qual[8], nameIn[8]; DATE_T1 dt;
                 WORD prYes, numPart, nPart, indFolder, indTitle; } TABL_MME;
typedef struct { WCHAR Name[41]; WCHAR Ext[4]; PAR_FILE pf; } One_Str_Cat;
typedef struct { BYTE j[3];                                  //E9 00 00 - A jump instruction to skip the record and reach the code to execute.
                 char Met[8];                                //"LGEINC  " - идентификационная строка
                 WORD BytesPerSector;                        //00 02 = 512 - The number of bytes in each sector
                 BYTE r1;                                    //00
                 WORD numRezSec;                             //20 00 = 32 - Число зарезервированных секторов в начале раздела
                 BYTE nf;                                    //01 - The number of copies of the FAT
                 BYTE r2[4];                                 //00 00 00 00
                 BYTE F8;                                    //F8 - Media descriptor byte
                 BYTE r3[10];                                //Нули
                 DWORD numSecPart;                           //Общее число секторов в разделе (совпадает со значением в Таблице разделов - 0 сектор)
                 DWORD numSecFAT;                            //Число секторов под FAT
                 BYTE r4[12];                                //Нули
                 DWORD nSecDir;                              //Смещение к первому каталогу
                 BYTE r5[15];                                //Нули
                 char Name[19];                              //"VOLUMELABE FAT32   "
                 BYTE r6[6];                                 //Нули
                 WORD SecPerCl;                              //Число секторов в одном кластере
                 BYTE b[414]; } SEC_63;                      //Нули
typedef struct { DWORD Ident1;
                 DWORD A[120];
                 DWORD Ident2;
                 DWORD FreeCl;                               //Число свободных кластеров
                 DWORD LastCl;                               //Последний занятый кластер
                 DWORD B[3];
                 DWORD Ident3; } SEC_67;
typedef struct { BYTE a[440];                                //Нули
                 DWORD Ident;                                //Неизвестный идентификатор HDD
                 BYTE c[2];                                  //Нули
                 DWORD A1;                                   //(01)активный раздел, (00)головка, (80=128)сектор, (0B=11)цилиндр
                 DWORD numClP1;                              //Число кластеров в первом разделе
                 DWORD B1;                                   //3F 00 00 00 == 63 - Число секторов предшествующих разделу
                 DWORD numSecP1;                             //Общее число секторов в разделе
                 DWORD A2;                                   //(01)активный раздел, (00)головка, (00)сектор, (0А=15)цилиндр
                 DWORD numClP2;                              //Число кластеров в первом разделе
                 DWORD B2;                                   //3F 00 00 00 == 63 - Число секторов предшествующих разделу
                 DWORD numSecP2;                             //Общее число секторов в разделе
                 DWORD C[8];                                 //Две пустых строки - нули
                 WORD  IdEnd; } SEC_0;                       //Ending Signature - 55 AA
#if sizeof(One_Str_Cat) != 128
   #error Error sizeof(One_Str_Cat)
#endif
#if sizeof(SEC_63) != 512
   #error Error sizeof(SEC_63)
#endif
#if sizeof(SEC_0) != 512
   #error Error sizeof(SEC_0)
#endif
typedef struct { long Ind; char *msg; } LANG;                //Структура языкового массива
#define IDENT_STR "from_hdd_lg_to_pc.conf"
typedef struct                                               //64 байта
   {  char IdentStr[sizeof(IDENT_STR)];
      BYTE PrSort;                                           //0-сортировка по имени, 1-без сотировки
      BYTE ViewIDX;                                          //0-не показыват файлы *.idx, 1-показывать
      BYTE Ren_STR;                                          //0-переименовывоть *.str в *.vro
      BYTE SwapNPart;                                        //0-переставлять номер части в конец имени
      BYTE ChangeNul;                                        //0-заменять символ '0' на '_'
      BYTE NoViewOnePart;                                    //0-не показыватьномер для одной части
      BYTE nLanguage;                                        //0-русский, 1-английский, 2-внешний файл
      BYTE altName;                                          //0-использовать аьтернативное имя в папке MEDIA, 1-не использовать альтернативное имя
      BYTE typeRec;                                          //0-старые серии, 1-новая серия RH2000
      BYTE typeSize;                                         //0-байты, 1-Кбайты, 2-Мбайты, 3-ГБайты
      BYTE ViewVRO;                                          //0-показывать расширение *.vro, 1-не показывать
      BYTE typeTime;                                         //0-показывать время начала записи, 1-время создания файла
      BYTE poz_Ql;                                           //Качество записи: 0-в колонке, 1-в имени, 2-не показывать
      BYTE poz_In;                                           //Источник записи: 0-в колонке, 1-в имени, 2-не показывать
      BYTE OutJ_BOX;
      BYTE OutMEDIA;
      BYTE OutPHOTO;
      BYTE OutVIDEO;
      BYTE ViewPart;                                         //Способ показа частей: 0-папкой, 1-отдельно
      BYTE WriteYes;                                         //Разрешение на запись: 0-нет, 1-есть
      BYTE rez[21];
   } CONF;

     const DWORD sSecB = 512;                                //Размер сектора в байтах
     const DWORD sClSec = 1024;                              //Число секторов в кластере
     const DWORD sCl_B = sSecB * sClSec;                     //Размер кластера в байтах
     const DWORD sClSec2 = 8;                                //Число секторов в кластере во втором разделе
     const DWORD sCl2_B = sSecB * sClSec2;                   //Размер кластера в байтах во втором разделе
typedef struct                                               //Описание элементов модификации диалога
  { DWORD         Flags;                                     //Дополнительные флаги
    LPOFNHOOKPROC fnHook;                                    //Функция обработки дополнительного диалога
    LPCTSTR       DopDlgName;                                //Дополнительный диалог который суммируется с основным
  } ModifDialog;

//------------------------------------------------------------------------------
inline void SWAP16(WORD *W)                                  //Перестановка двух байт
{
  *W = WORD(((*W & 0x00ffU) << 8) | ((*W & 0xff00U) >> 8));
}
//------------------------------------------------------------------------------
inline void SWAP32(DWORD *DW)                                //Перестановка четырех юайт
{
  *DW = DWORD( ((*DW & 0x000000ffU) << 24) | ((*DW & 0x0000ff00U) <<  8) |
               ((*DW & 0x00ff0000U) >>  8) | ((*DW & 0xff000000U) >> 24));
}
//------------------------------------------------------------------------------
inline void SWAP64(DWORDLONG *DWL)                           //Перестановка восьми байт
{
/*
  *DWL = DWORDLONG( ((*DWL & 0x00000000000000ff) << 56) | ((*DWL & 0x000000000000ff00) << 48) |
                    ((*DWL & 0x0000000000ff0000) << 40) | ((*DWL & 0x00000000ff000000) << 32) |
                    ((*DWL & 0x000000ff00000000) >> 32) | ((*DWL & 0x0000ff0000000000) >> 40) |
                    ((*DWL & 0x00ff000000000000) >> 48) | ((*DWL & 0xff00000000000000) >> 56));
*/
   DWORD D = *(DWORD*)DWL;
   *(DWORD*)DWL = *((DWORD*)DWL + 1);
   *((DWORD*)DWL + 1) = D;
   SWAP32((DWORD*)DWL);
   SWAP32((DWORD*)DWL + 1);
}

#endif

