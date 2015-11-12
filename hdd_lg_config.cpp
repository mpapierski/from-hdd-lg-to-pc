
//                             12.02.2009
#ifndef __Config_IO__
#define __Config_IO__

#include "from_hdd_lg_to_pc.h"

//============================ config_io =======================================
int  WriteConfig(void);                                      //Запись файла конфигурации
int  LoadConfig(void);                                       //Загрузка конфигурации
void NewConfig(void);                                        //Изменение конфигурации
     LANG *Lan;
     CONF Conf;                                              //Параметры
     char *msgLan;                                           //Все собщения для другого языка

static char NameF_Lan[260];                                  //Имя файла языка если оно задано
static char *AdrEnd;                                         //Адрес конца файла языка в памяти
static int prLanDef;                                         //Признак языка по умолчанию 0-русский,1-другой

//------------------------------------------------------------------------------

static int Write_Config(HANDLE *hFile)                       //Запись файла конфигурации
{
   DWORD nb;

   char NameF[260];
   char Driv[MAXDRIVE], Dir[MAXDIR], Name[MAXFILE], Ext[MAXEXT];
   if(GetModuleFileName(NULL, NameF, sizeof(NameF)) == 0) return -1;
   fnsplit(NameF, Driv, Dir, Name, Ext);                     //Разложили имя файла
   fnmerge(NameF, Driv, Dir, Name, ".conf");                 //Получили имя файла
   *hFile = CreateFile(NameF, GENERIC_WRITE, 0, NULL,
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if(*hFile == INVALID_HANDLE_VALUE)
     return ErrorSys2(NameF, (Lan+82)->msg);                 //return ErrorSys2(NameF, "Ошибка при открытии файла для записи.");
   if(WriteFile(*hFile, &Conf, sizeof(CONF), &nb, NULL) == FALSE || nb != sizeof(CONF))
     return ErrorSys2(NameF, (Lan+83)->msg);                 //return ErrorSys2(NameF, "Ошибка при записи файла.");
   if(Conf.nLanguage != 2) return 0;                         //0-русский, 1-английский, 2-внешний файл
   DWORD Size = lstrlen(NameF_Lan) + 1;                      //Размер имени файла
   if(WriteFile(*hFile, &NameF_Lan, Size, &nb, NULL) == FALSE || nb != Size)
     return ErrorSys2(NameF, (Lan+83)->msg);                 //return ErrorSys2(NameF, "Ошибка при записи файла.");
   return 0;
}

//------------------------------------------------------------------------------

int WriteConfig(void)                                        //Запись файла конфигурации
{
   HANDLE hFile;
   int ret = Write_Config(&hFile);                           //Запись файла конфигурации
   CloseFile(&hFile);
   return ret;
}

//------------------------------------------------------------------------------

static int SaveConfig(HWND hDlg)                             //Сохранение файла конфигурации
{
   lstrcpy(Conf.IdentStr, IDENT_STR);                        //Идентификатор файла
   for(DWORD i=0; i<sizeof(Conf.rez); i++) *(Conf.rez + i) = 0;
   int IndV = SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_GETCURSEL, 0, 0L); //Индекс в окне списка
   if(IndV != LB_ERR)                                        //Ничего не выбрано
   {  if(IndV >= 2)
      {  SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_GETTEXT, IndV, (LPARAM)NameF_Lan);  //Взяли текст имени из списка
         Conf.nLanguage = 2;                                 //0-русский, 1-английский, 2-внешний файл
      }
      else
      {  Conf.nLanguage = BYTE(IndV);                        //0-русский, 1-английский, 2-внешний файл
         *NameF_Lan = 0;                                     //Имя файла языка если оно задано
      }
   }
   return WriteConfig();                                     //Запись файла конфигурации
}

//------------------------------------------------------------------------------

static void Lan_Default(void)                                //Установка языка по умолчанию
{
   MyFreeMem(&(void*)msgLan);
   if(prLanDef == 0)                                         //Признак языка по умолчанию 0-русский,1-другой
   {  Lan = Lan_RU;                                          //Загрузили русскую страницу
      Conf.nLanguage = 0;                                    //0-русский, 1-английский, 2-внешний файл
   }
   else
   {  Lan = Lan_EN;                                          //Загрузили английскую страницу
      Conf.nLanguage = 1;                                    //0-русский, 1-английский, 2-внешний файл
   }
   *NameF_Lan = 0;                                           //Имя файла языка если оно задано
}

//------------------------------------------------------------------------------

static void ConfigDefault(void)                              //Установка конфигурации по умолчанию
{
   ZeroMemory(&Conf, sizeof(Conf));
   Lan_Default();                                            //Установка языка по умолчанию
}

//------------------------------------------------------------------------------

static int ReadConfig(char *NameF, HANDLE *hFile)            //Загрузка конфигурации
{
   DWORD nb;

   *hFile = CreateFile(NameF, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(*hFile == INVALID_HANDLE_VALUE)
      return ErrorSys2(NameF, (Lan+84)->msg);                //return ErrorSys2(NameF, "Ошибка при открытии файла для чтения.");
   DWORD FSize = GetFileSize(*hFile, NULL);
   if(FSize == 0xFFFFFFFF)
      return ErrorSys2(NameF, (Lan+85)->msg);                //return ErrorSys2(NameF, "Ошибка при запросе размера файла.");
   if(FSize < sizeof(CONF))
      return Error2(NameF, (Lan+86)->msg);                   //return Error2(NameF, "Недопустимо малый размер файла.");
   if(ReadFile(*hFile, &Conf, sizeof(CONF), &nb, NULL) == FALSE || nb != sizeof(CONF))
      return ErrorSys2(NameF, (Lan+87)->msg);                //return ErrorSys2(NameF, "Ошибка при чтении файла.");
   if(lstrcmp(Conf.IdentStr, IDENT_STR) != 0)                //Идентификатор файла
   {  ConfigDefault();                                       //Установка конфигурации по умолчанию
      return Error2(NameF, (Lan+88)->msg);                   //return Error2(NameF, "Неверный идентификатор файла.");
   }
   *NameF_Lan = 0;
   if(Conf.nLanguage != 2)  return 0;                        //0-русский, 1-английский, 2-внешний файл
   DWORD Size = FSize - sizeof(CONF);
   if(ReadFile(*hFile, &NameF_Lan, Size, &nb, NULL) == FALSE || nb != Size)
      return ErrorSys2(NameF, (Lan+87)->msg);                //return ErrorSys2(NameF, "Ошибка при чтении файла.");
   return 0;
}

//------------------------------------------------------------------------------

static int ReadLanFile(char *NameF, HANDLE *hFile)           //Чтение файла языка
{
   DWORD nb;

   *hFile = CreateFile(NameF, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(*hFile == INVALID_HANDLE_VALUE)
      return ErrorSys2(NameF, (Lan+84)->msg);                //return ErrorSys2(NameF, "Ошибка при открытии файла для чтения.");
   DWORD FSize = GetFileSize(*hFile, NULL);
   if(FSize == 0xFFFFFFFF)
      return ErrorSys2(NameF, (Lan+85)->msg);                //return ErrorSys2(NameF, "Ошибка при запросе размера файла.");
   if(FSize < 120)
      return Error2(NameF, (Lan+86)->msg);                   //return Error2(NameF, "Недопустимо малый размер файла.");
   MyFreeMem(&(void*)msgLan);
   msgLan = (char *)MyAllocMem(FSize+1);                     //Память под один кластер
   if(msgLan == NULL)  return -1;
   if(ReadFile(*hFile, msgLan, FSize, &nb, NULL) == FALSE || nb != FSize)
      return ErrorSys2(NameF, (Lan+87)->msg);                //return ErrorSys2(NameF, "Ошибка при чтении файла.");
   AdrEnd = msgLan + FSize;                                  //Адрес конца файла языка в памяти
   *AdrEnd = '\n';                                           //На всякий случай, вдруг забыли
   AdrEnd++;
   return 0;
}

//------------------------------------------------------------------------------

static int SetNewLan(void)                                   //Установка нового языка
{
   char *Adr;                                                //Текущий адрес начала строки
   char *NewAdr = msgLan - 1;                                //Адрес конца строки (равен началу файла - 1 для работы первого оператора цикла)
   LANG *LanD = (prLanDef == 0) ? Lan_RU : Lan_EN;           //Признак языка по умолчанию 0-русский,1-другой
   for(int i=0; i<numStrLan; i++)                            //Скопировали все сообщения в полном объеме
     *(Lan_Any + i) = *(LanD + i);
   for(;;)                                                   //Заменяем мои сообщения на сообщения из языкового файла
   {  Adr = NewAdr + 1;
      NewAdr = strchr(Adr, '\n');                            //Нашли конец строки
      if(NewAdr > AdrEnd) break;                             //Данные кончились
      if(NewAdr == NULL)  break;                             //Больше нет ни одной полной строки
      *NewAdr = 0;                                           //Ограничили строку
      if(*Adr == 0) continue;                                //Пустая строка
      if(*Adr == '/' && *(Adr+1) == '/') continue;           //Строка комментария
      int ind_m = int(atof(Adr));                            //Номер строки языка
      if(ind_m < 0 || ind_m > numStrLan)                     //Номер строки вне допустимого диапазона
      {   Error3(NameF_Lan, Adr, (Lan+89)->msg);             //Error3(NameF_Lan, Adr, "Недопустимый индекс строки.");
          continue;
      }
      int ik1 = -1, ik2 = -1;                                //Индексы первой и второй кавычек
      for(int i=0; i<NewAdr-Adr; i++)                        //По всей строке ищем пару кавычек
        if(*(Adr+i) == '"')
          if(ik1 == -1) ik1 = i;                             //Нашли первую кавычку
          else ik2 = i;                                      //Нашли вторую кавычку
      if(ik1 == -1 || ik2 == -1)
      {   Error3(NameF_Lan, Adr, (Lan+90)->msg);             //Error3(NameF_Lan, Adr, "Не найден текст ограниченный кавычками.");
          continue;
      }
      (Lan_Any + ind_m)->msg = Adr + ik1 + 1;                //Начало текста первый символ после первой кавычки
      *(Adr + ik2) = 0;                                      //Заменили вторую кавычку на признак конца строки
   }
   Lan = Lan_Any;
   return 0;
}

//------------------------------------------------------------------------------

static int LoadLanFile(void)                                 //Загрузка файла языка
{
   HANDLE hFile;
   char NameF[260];
   char Driv[MAXDRIVE], Dir[MAXDIR], Name[MAXFILE], Ext[MAXEXT];
   if(GetModuleFileName(NULL, NameF, sizeof(NameF)) == 0) return -1;
   fnsplit(NameF, Driv, Dir, Name, Ext);                     //Разложили имя файла
   fnmerge(NameF, Driv, Dir, NameF_Lan, "");                 //Получили имя файла
   int ret = ReadLanFile(NameF, &hFile);                     //Чтение файла языка
   CloseFile(&hFile);
   if(ret < 0) return -1;
   ret = SetNewLan();                                        //Установка нового языка
   return ret;
}

//------------------------------------------------------------------------------

static int SetNewLanguage(void)                              //Загрузка и установка файла языка
{
   if(Conf.nLanguage == 0)                                   //0-русский, 1-английский, 2-внешний файл
   {  Lan = Lan_RU;  return 0;  }                            //Загрузили русскую страницу
   if(Conf.nLanguage == 1)                                   //0-русский, 1-английский, 2-внешний файл
   {  Lan = Lan_EN;  return 0;  }                            //Загрузили английскую страницу
   if(LoadLanFile() == 0) return 0;                          //Загрузка файла языка
   Lan_Default();                                            //Установка языка по умолчанию
   return 0;
}

//------------------------------------------------------------------------------

int LoadConfig(void)                                        //Загрузка конфигурации
{
   char NameF[260];
   char Driv[MAXDRIVE], Dir[MAXDIR], Name[MAXFILE], Ext[MAXEXT];
   if(GetModuleFileName(NULL, NameF, sizeof(NameF)) == 0) return -1;
   fnsplit(NameF, Driv, Dir, Name, Ext);                     //Разложили имя файла
   fnmerge(NameF, Driv, Dir, Name, ".conf");                 //Получили имя файла

   char LanDef[5];
   GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILANGUAGE, LanDef, 5);
   if(lstrcmp(LanDef, "0419") == 0)
   {  prLanDef = 0; Lan = Lan_RU; }                          //Признак языка по умолчанию 0-русский,1-другой
   else
   {  prLanDef = 1; Lan = Lan_EN; }                          //Признак языка по умолчанию 0-русский,1-другой
   msgLan = NULL;
   if(CtrlFileYesNo(NameF) == 0)                             //Проверка наличия файла (0-файла нет)
   {  ConfigDefault();                                       //Установка конфигурации по умолчанию
      return 0;
   }
   HANDLE hFile;
   int ret = ReadConfig(NameF, &hFile);                      //Загрузка конфигурации
   CloseFile(&hFile);
   if(ret == 0) return SetNewLanguage();                     //Загрузка и установка файла языка
   ConfigDefault();                                          //Установка конфигурации по умолчанию
   return 0;
}

//------------------------------------------------------------------------------

static void Init_Dlg_Name(HWND hDlg)                         //Наименования в зависимости от языка
{
   SetWindowText(hDlg, (Lan+5)->msg);                        //Вывели новый заголовок
   SetDlgItemText(hDlg, IDC_GROUPBOX2, (Lan+69)->msg);
   SetDlgItemText(hDlg, IDC_CHECKBOX1, (Lan+70)->msg);
   SetDlgItemText(hDlg, IDC_CHECKBOX2, (Lan+71)->msg);
   SetDlgItemText(hDlg, IDC_CHECKBOX3, (Lan+72)->msg);
   SetDlgItemText(hDlg, IDC_CHECKBOX4, (Lan+76)->msg);
   SetDlgItemText(hDlg, IDC_CHECKBOX5, (Lan+77)->msg);
   SetDlgItemText(hDlg, IDC_GROUPBOX1, (Lan+73)->msg);
   SetDlgItemText(hDlg, IDOK, (Lan+74)->msg);
   SetDlgItemText(hDlg, IDCANCEL, (Lan+75)->msg);
   SetDlgItemText(hDlg, IDC_GROUPBOX3, (Lan+78)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON1, (Lan+79)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON2, (Lan+80)->msg);
   SetDlgItemText(hDlg, IDC_GROUPBOX4, (Lan+109)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON3, (Lan+110)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON4, (Lan+111)->msg);
   SetDlgItemText(hDlg, IDC_GROUPBOX5, (Lan+112)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON6, (Lan+113)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON5, (Lan+114)->msg);
   SetDlgItemText(hDlg, IDC_GROUPBOX6, (Lan+115)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON7, (Lan+116)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON8, (Lan+117)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON9, (Lan+118)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON10, (Lan+119)->msg);
   SetDlgItemText(hDlg, IDC_CHECKBOX6, (Lan+120)->msg);
   SetDlgItemText(hDlg, IDC_CHECKBOX7, (Lan+154)->msg);
   SetDlgItemText(hDlg, IDC_GROUPBOX7, (Lan+127)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON11, (Lan+128)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON12, (Lan+129)->msg);
   SetDlgItemText(hDlg, IDC_GROUPBOX8, (Lan+130)->msg);
   SetDlgItemText(hDlg, IDC_GROUPBOX9, (Lan+131)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON13, (Lan+132)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON14, (Lan+133)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON15, (Lan+134)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON16, (Lan+132)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON17, (Lan+133)->msg);
   SetDlgItemText(hDlg, IDC_RADIOBUTTON18, (Lan+134)->msg);
#if defined WRITE_YES                                        //Режим записи разрешен
   SetDlgItemText(hDlg, IDC_GROUPBOX10, (Lan+165)->msg);
   SetDlgItemText(hDlg, IDC_CHECKBOX8, (Lan+166)->msg);
   if(pr_tRec == 1)                                          //Признак рекордера 0 или 1 (старейшая серия)
   {  ShowWindow(GetDlgItem(hDlg, IDC_GROUPBOX10), SW_HIDE);
      ShowWindow(GetDlgItem(hDlg, IDC_CHECKBOX8), SW_HIDE);
      Conf.WriteYes = 0;                                     //Для старой серии никакой записи
   }
#else
   ShowWindow(GetDlgItem(hDlg, IDC_GROUPBOX10), SW_HIDE);
   ShowWindow(GetDlgItem(hDlg, IDC_CHECKBOX8), SW_HIDE);
#endif
}

//------------------------------------------------------------------------------

static void Init_Dlg_ParForConfig(HWND hDlg)                 //Параметры в зависимости от конфигурации
{
   if(Conf.altName == 0)
   {  EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX1), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX2), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX3), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX4), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX5), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX6), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX7), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_GROUPBOX7), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_GROUPBOX8), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_GROUPBOX9), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON11), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON12), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON13), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON14), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON15), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON16), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON17), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON18), TRUE);
      return;
   }
   EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX1), TRUE);
   EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX2), TRUE);
   EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX3), TRUE);
   int State1 = (Conf.SwapNPart == 0) ? TRUE : FALSE;
   EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX4), State1);
   EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX5), State1);
   EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX6), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_CHECKBOX7), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_GROUPBOX7), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_GROUPBOX8), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_GROUPBOX9), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON11), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON12), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON13), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON14), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON15), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON16), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON17), FALSE);
   EnableWindow(GetDlgItem(hDlg, IDC_RADIOBUTTON18), FALSE);
}

//------------------------------------------------------------------------------

static void Init_Dlg_Par(HWND hDlg)                          //Параметры в зависимости от конфигурации
{
   int State;
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON1+Conf.PrSort, BM_SETCHECK, 1, 0L);
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON3+Conf.altName, BM_SETCHECK, 1, 0L);
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON5+Conf.typeRec, BM_SETCHECK, 1, 0L);
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON7+Conf.typeSize, BM_SETCHECK, 1, 0L);
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON11+Conf.typeTime, BM_SETCHECK, 1, 0L);//0-показывать время начала записи, 1-время создания файла
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON13+Conf.poz_Ql, BM_SETCHECK, 1, 0L);  //Качество записи: 0-в колонке, 1-в имени, 2-не показывать
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON16+Conf.poz_In, BM_SETCHECK, 1, 0L);  //Источник записи: 0-в колонке, 1-в имени, 2-не показывать

   State = (Conf.ViewVRO == 0) ? BST_CHECKED : BST_UNCHECKED;      //0-показыват расширение *.vro, 1-не показывать
   SendDlgItemMessage(hDlg, IDC_CHECKBOX6, BM_SETCHECK, State, 0L);
   State = (Conf.ViewPart == 0) ? BST_CHECKED : BST_UNCHECKED;      //0-показыват расширение *.vro, 1-не показывать
   SendDlgItemMessage(hDlg, IDC_CHECKBOX7, BM_SETCHECK, State, 0L);
   State = (Conf.ViewIDX == 0) ? BST_CHECKED : BST_UNCHECKED;      //0-не показыват файлы *.idx, 1-показывать
   SendDlgItemMessage(hDlg, IDC_CHECKBOX1, BM_SETCHECK, State, 0L);
   State = (Conf.Ren_STR == 0) ? BST_CHECKED : BST_UNCHECKED;      //0-переименовывоть *.str в *.vro
   SendDlgItemMessage(hDlg, IDC_CHECKBOX2, BM_SETCHECK, State, 0L);
   State = (Conf.SwapNPart == 0) ? BST_CHECKED : BST_UNCHECKED;    //0-переставлять номер части в конец имени
   SendDlgItemMessage(hDlg, IDC_CHECKBOX3, BM_SETCHECK, State, 0L);
   State = (Conf.ChangeNul == 0) ? BST_CHECKED : BST_UNCHECKED;    //0-заменять символ '0' на '_'
   SendDlgItemMessage(hDlg, IDC_CHECKBOX4, BM_SETCHECK, State, 0L);
   State = (Conf.NoViewOnePart == 0) ? BST_CHECKED : BST_UNCHECKED;//0-не показыватьномер для одной части
   SendDlgItemMessage(hDlg, IDC_CHECKBOX5, BM_SETCHECK, State, 0L);
   State = (Conf.WriteYes == 1) ? BST_CHECKED : BST_UNCHECKED;     //1-функции записи включены
   SendDlgItemMessage(hDlg, IDC_CHECKBOX8, BM_SETCHECK, State, 0L);
   Init_Dlg_ParForConfig(hDlg);                                    //Параметры в зависимости от конфигурации
}

//------------------------------------------------------------------------------

static void Create_SpisLangF(HWND hDlg)                      //Создание списка языковых файлов
{
   HANDLE File;
   WIN32_FIND_DATA Data;
   char NameF[260];
   char Driv[MAXDRIVE], Dir[MAXDIR], Name[MAXFILE], Ext[MAXEXT];
   if(GetModuleFileName(NULL, NameF, sizeof(NameF)) == 0) return;
   fnsplit(NameF, Driv, Dir, Name, Ext);                     //Разложили имя файла
   fnmerge(NameF, Driv, Dir, "*", ".lng");                   //Получили имя файла
   if((File = FindFirstFile(NameF, &Data)) == INVALID_HANDLE_VALUE)  return; //Файл не найден
   for(;;)                                                   //Список языковых файлов
   {  SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_ADDSTRING, 0, (LPARAM)Data.cFileName);
      if(FindNextFile(File, &Data) == 0)
          if(GetLastError() == ERROR_NO_MORE_FILES) break;   //Больше нет файлов с расширением *.lng
   }
   FindClose(File);
}

//------------------------------------------------------------------------------

static void Init_SpisLang(HWND hDlg)                         //Создание списка языков
{
   SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_ADDSTRING, 0, (LPARAM)"Русский (Russian)");
   SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_ADDSTRING, 0, (LPARAM)"English");
   Create_SpisLangF(hDlg);                                   //Создание списка языковых файлов
   if(Conf.nLanguage != 2)                                   //0-русский, 1-английский, 2-внешний файл
     SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_SETCURSEL, Conf.nLanguage, 0L);
   else
   {  int IndV = SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_FINDSTRING, -1, (LPARAM)NameF_Lan);
      if(IndV == LB_ERR)                                     //Ничего не выбрано
         Error2((Lan+81)->msg, "(code -44)");                //Error2("Неопознанная ошибка работы со списком", "(code -44)");
      SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_SETCURSEL, IndV, 0L);    //Курсор на номер в списке
   }
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL CALLBACK Dlg_NewConf(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch(Message)
   {
      case WM_INITDIALOG:
             CenterDlg(hDlg, 0);
             Init_Dlg_Name(hDlg);                            //Наименования в зависимости от языка
             Init_Dlg_Par(hDlg);                             //Параметры в зависимости от конфигурации
             Init_SpisLang(hDlg);                            //Создание списка языков
             return TRUE;
      case WM_COMMAND:
           switch(LOWORD(wParam))
           {   case IDOK:     if(SaveConfig(hDlg) < 0) break;//Сохранение файла конфигурации
                              SetNewLanguage();              //Загрузка и установка файла языка
                              EndDialog(hDlg, IDOK);
                              return TRUE;
               case IDCANCEL: EndDialog(hDlg, IDCANCEL);
                              return TRUE;
               case IDC_RADIOBUTTON1: Conf.PrSort = 0; break;//0-сортировка по имени, 1-без сотировки
               case IDC_RADIOBUTTON2: Conf.PrSort = 1; break;//0-сортировка по имени, 1-без сотировки
               case IDC_RADIOBUTTON3: Conf.altName = 0;
                                   Init_Dlg_ParForConfig(hDlg); //Параметры в зависимости от конфигурации
                                   break;
               case IDC_RADIOBUTTON4: Conf.altName = 1;
                                   Init_Dlg_ParForConfig(hDlg); //Параметры в зависимости от конфигурации
                                   break;
               case IDC_RADIOBUTTON5:  Conf.typeRec  = 0; break;//0-старые серии, 1-новая серия RH2000
               case IDC_RADIOBUTTON6:  Conf.typeRec  = 1; break;//0-старые серии, 1-новая серия RH2000
               case IDC_RADIOBUTTON7:  Conf.typeSize = 0; break;//0-байты, 1-Кбайты, 2-Мбайты, 3-ГБайты
               case IDC_RADIOBUTTON8:  Conf.typeSize = 1; break;//0-байты, 1-Кбайты, 2-Мбайты, 3-ГБайты
               case IDC_RADIOBUTTON9:  Conf.typeSize = 2; break;//0-байты, 1-Кбайты, 2-Мбайты, 3-ГБайты
               case IDC_RADIOBUTTON10: Conf.typeSize = 3; break;//0-байты, 1-Кбайты, 2-Мбайты, 3-ГБайты
               case IDC_RADIOBUTTON11: Conf.typeTime = 0; break;//0-показывать время начала записи, 1-время создания файла
               case IDC_RADIOBUTTON12: Conf.typeTime = 1; break;//0-показывать время начала записи, 1-время создания файла
               case IDC_RADIOBUTTON13: Conf.poz_Ql = 0;   break;//Качество записи: 0-в колонке, 1-в имени, 2-не показывать
               case IDC_RADIOBUTTON14: Conf.poz_Ql = 1;   break;//Качество записи: 0-в колонке, 1-в имени, 2-не показывать
               case IDC_RADIOBUTTON15: Conf.poz_Ql = 2;   break;//Качество записи: 0-в колонке, 1-в имени, 2-не показывать
               case IDC_RADIOBUTTON16: Conf.poz_In = 0;   break;//Источник записи: 0-в колонке, 1-в имени, 2-не показывать
               case IDC_RADIOBUTTON17: Conf.poz_In = 1;   break;//Источник записи: 0-в колонке, 1-в имени, 2-не показывать
               case IDC_RADIOBUTTON18: Conf.poz_In = 2;   break;//Источник записи: 0-в колонке, 1-в имени, 2-не показывать
               case IDC_CHECKBOX1: Conf.ViewIDX       = (Conf.ViewIDX == 0)       ? BYTE(1) : BYTE(0);//0-не показыват файлы *.idx, 1-показывать
                                   break;
               case IDC_CHECKBOX2: Conf.Ren_STR       = (Conf.Ren_STR == 0)       ? BYTE(1) : BYTE(0);//0-переименовывоть *.str в *.vro
                                   break;
               case IDC_CHECKBOX3: Conf.SwapNPart     = (Conf.SwapNPart == 0)     ? BYTE(1) : BYTE(0);//0-переставлять номер части в конец имени
                                   Init_Dlg_ParForConfig(hDlg);                                       //Параметры в зависимости от конфигурации
                                   break;
               case IDC_CHECKBOX4: Conf.ChangeNul     = (Conf.ChangeNul == 0)     ? BYTE(1) : BYTE(0);//0-заменять символ '0' на '_'
                                   break;
               case IDC_CHECKBOX5: Conf.NoViewOnePart = (Conf.NoViewOnePart == 0) ? BYTE(1) : BYTE(0);//0-не показыватьномер для одной части
                                   break;
               case IDC_CHECKBOX6: Conf.ViewVRO  = (Conf.ViewVRO == 0)  ? BYTE(1) : BYTE(0);//0-показывать расширение *.vro, 1- не показывать
                                   break;
               case IDC_CHECKBOX7: Conf.ViewPart  = (Conf.ViewPart == 0)  ? BYTE(1) : BYTE(0);//0-показывать Title в виде папки
                                   break;
               case IDC_CHECKBOX8: if(Conf.WriteYes == 0)
                                      if(MsgYesNo5((Lan+167)->msg, (Lan+168)->msg, (Lan+169)->msg,
                                                   (Lan+170)->msg, (Lan+171)->msg) == 'N')
                                   {   SendDlgItemMessage(hDlg, IDC_CHECKBOX8, BM_SETCHECK, BST_UNCHECKED, 0L);
                                       break;
                                   }
                                   Conf.WriteYes  = (Conf.WriteYes == 0)  ? BYTE(1) : BYTE(0);//0-показывать Title в виде папки
                                   break;
           }
           break;
   }
   return FALSE;
}

//------------------------------------------------------------------------------

void NewConfig(void)                                         //Изменение конфигурации
{
   CONF oldConf = Conf;                                      //Сохранили предыдущее состояние
   char oldNameF_Lan[260];                                   //Имя файла языка если оно задано
   lstrcpy(oldNameF_Lan, NameF_Lan);                         //Сохранили предыдущее состояние
   if(DialogBox(MainInst, MAKEINTRESOURCE(IDD_DLG_CONFIG), MainWin, Dlg_NewConf) == IDCANCEL)
   {  Conf = oldConf;                                        //Восстановили предыдущее состояние
      lstrcpy(NameF_Lan, oldNameF_Lan);                      //Восстановили предыдущее состояние
      return;
   }
#if defined WRITE_YES                                        //Режим записи разрешен
   if(Conf.WriteYes != oldConf.WriteYes)
      Message1((Lan+172)->msg);                              //"Изменения с функцией записи вступят в силу после перезапуска программы."
#endif
   if(Conf.nLanguage != oldConf.nLanguage ||                 //0-русский, 1-английский, 2-внешний файл
      (Conf.nLanguage == 2 && lstrcmp(NameF_Lan, oldNameF_Lan) != 0))
   {  SetWindowText(hCopy,  (Lan+4)->msg);                   //Новая надпись на кнопке
      SetWindowText(hConf,  (Lan+5)->msg);                   //Новая надпись на кнопке
      SetWindowText(hClose, (Lan+6)->msg);                   //Новая надпись на кнопке
      SetWindowText(hClear, (Lan+123)->msg);                 //Новая надпись на кнопке
      SetWindowText(hToTxt, (Lan+146)->msg);                 //Новая надпись на кнопке
#if defined WRITE_YES                                        //Режим записи разрешен
      SetWindowText(hWriteFi, (Lan+177)->msg);               //Новая надпись на кнопке
      SetWindowText(hWriteFo, (Lan+178)->msg);               //Новая надпись на кнопке
      SetWindowText(hNew_Fo,  (Lan+179)->msg);               //Новая надпись на кнопке
      SetWindowText(hRenFi,   (Lan+163)->msg);               //"Переименовать"
      SetWindowText(hDelFi,   (Lan+187)->msg);               //"Удалить"
#endif
      ViewSize();                                            //Показа дискового пространства
   }
   if(prHDD_LG_Yes != 0) return;                             //Признак, присутствия HDD: 0-есть, 1-не найден
   if(Conf.typeRec != oldConf.typeRec ||                     //0-старые серии, 1-новая серия RH2000
      Conf.altName != oldConf.altName)                       //0-информативное имя, 1-истинное имя
      Update_MME_DB();                                       //Пересоздание массива информативных имен
   if(Conf.PrSort !=        oldConf.PrSort    ||             //0-сортировка по имени, 1-без сотировки
      Conf.ViewIDX !=       oldConf.ViewIDX   ||             //0-не показыват файлы *.idx, 1-показывать
      Conf.Ren_STR !=       oldConf.Ren_STR   ||             //0-переименовывоть *.str в *.vro
      Conf.SwapNPart !=     oldConf.SwapNPart ||             //0-переставлять номер части в конец имени
      Conf.ChangeNul !=     oldConf.ChangeNul ||             //0-заменять символ '0' на '_'
      Conf.NoViewOnePart != oldConf.NoViewOnePart ||         //0-не показыватьномер для одной части
      Conf.altName != oldConf.altName ||                     //0-информативное имя, 1-истинное имя
      Conf.typeRec != oldConf.typeRec ||                     //0-старые серии, 1-новая серия RH2000
      Conf.typeSize != oldConf.typeSize ||                   //0-байты, 1-Кбайты, 2-Мбайты, 3-ГБайты
      Conf.ViewVRO != oldConf.ViewVRO ||                     //0-показыват расширение *.vro, 1-не показывать
      Conf.ViewPart != oldConf.ViewPart ||                   //0-показыват папкой отредактированные Title
      Conf.typeTime != oldConf.typeTime ||                   //0-показывать время начала записи, 1-время создания файла
      Conf.poz_Ql != oldConf.poz_Ql ||                       //Источник записи: 0-в колонке, 1-в имени, 2-не показывать
      Conf.poz_In != oldConf.poz_In)                         //Источник записи: 0-в колонке, 1-в имени, 2-не показывать
      Update_Tree();                                         //Пересоздание дерева файлов и папок при изменении настроек
}

#endif


