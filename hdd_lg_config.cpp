
//                             12.02.2009
#ifndef __Config_IO__
#define __Config_IO__

#include "from_hdd_lg_to_pc.h"

//============================ config_io =======================================
int  WriteConfig(void);                                      //������ ����� ������������
int  LoadConfig(void);                                       //�������� ������������
void NewConfig(void);                                        //��������� ������������
     LANG *Lan;
     CONF Conf;                                              //���������
     char *msgLan;                                           //��� �������� ��� ������� �����

static char NameF_Lan[260];                                  //��� ����� ����� ���� ��� ������
static char *AdrEnd;                                         //����� ����� ����� ����� � ������
static int prLanDef;                                         //������� ����� �� ��������� 0-�������,1-������

//------------------------------------------------------------------------------

static int Write_Config(HANDLE *hFile)                       //������ ����� ������������
{
   DWORD nb;

   char NameF[260];
   char Driv[MAXDRIVE], Dir[MAXDIR], Name[MAXFILE], Ext[MAXEXT];
   if(GetModuleFileName(NULL, NameF, sizeof(NameF)) == 0) return -1;
   fnsplit(NameF, Driv, Dir, Name, Ext);                     //��������� ��� �����
   fnmerge(NameF, Driv, Dir, Name, ".conf");                 //�������� ��� �����
   *hFile = CreateFile(NameF, GENERIC_WRITE, 0, NULL,
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if(*hFile == INVALID_HANDLE_VALUE)
     return ErrorSys2(NameF, (Lan+82)->msg);                 //return ErrorSys2(NameF, "������ ��� �������� ����� ��� ������.");
   if(WriteFile(*hFile, &Conf, sizeof(CONF), &nb, NULL) == FALSE || nb != sizeof(CONF))
     return ErrorSys2(NameF, (Lan+83)->msg);                 //return ErrorSys2(NameF, "������ ��� ������ �����.");
   if(Conf.nLanguage != 2) return 0;                         //0-�������, 1-����������, 2-������� ����
   DWORD Size = lstrlen(NameF_Lan) + 1;                      //������ ����� �����
   if(WriteFile(*hFile, &NameF_Lan, Size, &nb, NULL) == FALSE || nb != Size)
     return ErrorSys2(NameF, (Lan+83)->msg);                 //return ErrorSys2(NameF, "������ ��� ������ �����.");
   return 0;
}

//------------------------------------------------------------------------------

int WriteConfig(void)                                        //������ ����� ������������
{
   HANDLE hFile;
   int ret = Write_Config(&hFile);                           //������ ����� ������������
   CloseFile(&hFile);
   return ret;
}

//------------------------------------------------------------------------------

static int SaveConfig(HWND hDlg)                             //���������� ����� ������������
{
   lstrcpy(Conf.IdentStr, IDENT_STR);                        //������������� �����
   for(DWORD i=0; i<sizeof(Conf.rez); i++) *(Conf.rez + i) = 0;
   int IndV = SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_GETCURSEL, 0, 0L); //������ � ���� ������
   if(IndV != LB_ERR)                                        //������ �� �������
   {  if(IndV >= 2)
      {  SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_GETTEXT, IndV, (LPARAM)NameF_Lan);  //����� ����� ����� �� ������
         Conf.nLanguage = 2;                                 //0-�������, 1-����������, 2-������� ����
      }
      else
      {  Conf.nLanguage = BYTE(IndV);                        //0-�������, 1-����������, 2-������� ����
         *NameF_Lan = 0;                                     //��� ����� ����� ���� ��� ������
      }
   }
   return WriteConfig();                                     //������ ����� ������������
}

//------------------------------------------------------------------------------

static void Lan_Default(void)                                //��������� ����� �� ���������
{
   MyFreeMem(&(void*)msgLan);
   if(prLanDef == 0)                                         //������� ����� �� ��������� 0-�������,1-������
   {  Lan = Lan_RU;                                          //��������� ������� ��������
      Conf.nLanguage = 0;                                    //0-�������, 1-����������, 2-������� ����
   }
   else
   {  Lan = Lan_EN;                                          //��������� ���������� ��������
      Conf.nLanguage = 1;                                    //0-�������, 1-����������, 2-������� ����
   }
   *NameF_Lan = 0;                                           //��� ����� ����� ���� ��� ������
}

//------------------------------------------------------------------------------

static void ConfigDefault(void)                              //��������� ������������ �� ���������
{
   ZeroMemory(&Conf, sizeof(Conf));
   Lan_Default();                                            //��������� ����� �� ���������
}

//------------------------------------------------------------------------------

static int ReadConfig(char *NameF, HANDLE *hFile)            //�������� ������������
{
   DWORD nb;

   *hFile = CreateFile(NameF, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(*hFile == INVALID_HANDLE_VALUE)
      return ErrorSys2(NameF, (Lan+84)->msg);                //return ErrorSys2(NameF, "������ ��� �������� ����� ��� ������.");
   DWORD FSize = GetFileSize(*hFile, NULL);
   if(FSize == 0xFFFFFFFF)
      return ErrorSys2(NameF, (Lan+85)->msg);                //return ErrorSys2(NameF, "������ ��� ������� ������� �����.");
   if(FSize < sizeof(CONF))
      return Error2(NameF, (Lan+86)->msg);                   //return Error2(NameF, "����������� ����� ������ �����.");
   if(ReadFile(*hFile, &Conf, sizeof(CONF), &nb, NULL) == FALSE || nb != sizeof(CONF))
      return ErrorSys2(NameF, (Lan+87)->msg);                //return ErrorSys2(NameF, "������ ��� ������ �����.");
   if(lstrcmp(Conf.IdentStr, IDENT_STR) != 0)                //������������� �����
   {  ConfigDefault();                                       //��������� ������������ �� ���������
      return Error2(NameF, (Lan+88)->msg);                   //return Error2(NameF, "�������� ������������� �����.");
   }
   *NameF_Lan = 0;
   if(Conf.nLanguage != 2)  return 0;                        //0-�������, 1-����������, 2-������� ����
   DWORD Size = FSize - sizeof(CONF);
   if(ReadFile(*hFile, &NameF_Lan, Size, &nb, NULL) == FALSE || nb != Size)
      return ErrorSys2(NameF, (Lan+87)->msg);                //return ErrorSys2(NameF, "������ ��� ������ �����.");
   return 0;
}

//------------------------------------------------------------------------------

static int ReadLanFile(char *NameF, HANDLE *hFile)           //������ ����� �����
{
   DWORD nb;

   *hFile = CreateFile(NameF, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(*hFile == INVALID_HANDLE_VALUE)
      return ErrorSys2(NameF, (Lan+84)->msg);                //return ErrorSys2(NameF, "������ ��� �������� ����� ��� ������.");
   DWORD FSize = GetFileSize(*hFile, NULL);
   if(FSize == 0xFFFFFFFF)
      return ErrorSys2(NameF, (Lan+85)->msg);                //return ErrorSys2(NameF, "������ ��� ������� ������� �����.");
   if(FSize < 120)
      return Error2(NameF, (Lan+86)->msg);                   //return Error2(NameF, "����������� ����� ������ �����.");
   MyFreeMem(&(void*)msgLan);
   msgLan = (char *)MyAllocMem(FSize+1);                     //������ ��� ���� �������
   if(msgLan == NULL)  return -1;
   if(ReadFile(*hFile, msgLan, FSize, &nb, NULL) == FALSE || nb != FSize)
      return ErrorSys2(NameF, (Lan+87)->msg);                //return ErrorSys2(NameF, "������ ��� ������ �����.");
   AdrEnd = msgLan + FSize;                                  //����� ����� ����� ����� � ������
   *AdrEnd = '\n';                                           //�� ������ ������, ����� ������
   AdrEnd++;
   return 0;
}

//------------------------------------------------------------------------------

static int SetNewLan(void)                                   //��������� ������ �����
{
   char *Adr;                                                //������� ����� ������ ������
   char *NewAdr = msgLan - 1;                                //����� ����� ������ (����� ������ ����� - 1 ��� ������ ������� ��������� �����)
   LANG *LanD = (prLanDef == 0) ? Lan_RU : Lan_EN;           //������� ����� �� ��������� 0-�������,1-������
   for(int i=0; i<numStrLan; i++)                            //����������� ��� ��������� � ������ ������
     *(Lan_Any + i) = *(LanD + i);
   for(;;)                                                   //�������� ��� ��������� �� ��������� �� ��������� �����
   {  Adr = NewAdr + 1;
      NewAdr = strchr(Adr, '\n');                            //����� ����� ������
      if(NewAdr > AdrEnd) break;                             //������ ���������
      if(NewAdr == NULL)  break;                             //������ ��� �� ����� ������ ������
      *NewAdr = 0;                                           //���������� ������
      if(*Adr == 0) continue;                                //������ ������
      if(*Adr == '/' && *(Adr+1) == '/') continue;           //������ �����������
      int ind_m = int(atof(Adr));                            //����� ������ �����
      if(ind_m < 0 || ind_m > numStrLan)                     //����� ������ ��� ����������� ���������
      {   Error3(NameF_Lan, Adr, (Lan+89)->msg);             //Error3(NameF_Lan, Adr, "������������ ������ ������.");
          continue;
      }
      int ik1 = -1, ik2 = -1;                                //������� ������ � ������ �������
      for(int i=0; i<NewAdr-Adr; i++)                        //�� ���� ������ ���� ���� �������
        if(*(Adr+i) == '"')
          if(ik1 == -1) ik1 = i;                             //����� ������ �������
          else ik2 = i;                                      //����� ������ �������
      if(ik1 == -1 || ik2 == -1)
      {   Error3(NameF_Lan, Adr, (Lan+90)->msg);             //Error3(NameF_Lan, Adr, "�� ������ ����� ������������ ���������.");
          continue;
      }
      (Lan_Any + ind_m)->msg = Adr + ik1 + 1;                //������ ������ ������ ������ ����� ������ �������
      *(Adr + ik2) = 0;                                      //�������� ������ ������� �� ������� ����� ������
   }
   Lan = Lan_Any;
   return 0;
}

//------------------------------------------------------------------------------

static int LoadLanFile(void)                                 //�������� ����� �����
{
   HANDLE hFile;
   char NameF[260];
   char Driv[MAXDRIVE], Dir[MAXDIR], Name[MAXFILE], Ext[MAXEXT];
   if(GetModuleFileName(NULL, NameF, sizeof(NameF)) == 0) return -1;
   fnsplit(NameF, Driv, Dir, Name, Ext);                     //��������� ��� �����
   fnmerge(NameF, Driv, Dir, NameF_Lan, "");                 //�������� ��� �����
   int ret = ReadLanFile(NameF, &hFile);                     //������ ����� �����
   CloseFile(&hFile);
   if(ret < 0) return -1;
   ret = SetNewLan();                                        //��������� ������ �����
   return ret;
}

//------------------------------------------------------------------------------

static int SetNewLanguage(void)                              //�������� � ��������� ����� �����
{
   if(Conf.nLanguage == 0)                                   //0-�������, 1-����������, 2-������� ����
   {  Lan = Lan_RU;  return 0;  }                            //��������� ������� ��������
   if(Conf.nLanguage == 1)                                   //0-�������, 1-����������, 2-������� ����
   {  Lan = Lan_EN;  return 0;  }                            //��������� ���������� ��������
   if(LoadLanFile() == 0) return 0;                          //�������� ����� �����
   Lan_Default();                                            //��������� ����� �� ���������
   return 0;
}

//------------------------------------------------------------------------------

int LoadConfig(void)                                        //�������� ������������
{
   char NameF[260];
   char Driv[MAXDRIVE], Dir[MAXDIR], Name[MAXFILE], Ext[MAXEXT];
   if(GetModuleFileName(NULL, NameF, sizeof(NameF)) == 0) return -1;
   fnsplit(NameF, Driv, Dir, Name, Ext);                     //��������� ��� �����
   fnmerge(NameF, Driv, Dir, Name, ".conf");                 //�������� ��� �����

   char LanDef[5];
   GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILANGUAGE, LanDef, 5);
   if(lstrcmp(LanDef, "0419") == 0)
   {  prLanDef = 0; Lan = Lan_RU; }                          //������� ����� �� ��������� 0-�������,1-������
   else
   {  prLanDef = 1; Lan = Lan_EN; }                          //������� ����� �� ��������� 0-�������,1-������
   msgLan = NULL;
   if(CtrlFileYesNo(NameF) == 0)                             //�������� ������� ����� (0-����� ���)
   {  ConfigDefault();                                       //��������� ������������ �� ���������
      return 0;
   }
   HANDLE hFile;
   int ret = ReadConfig(NameF, &hFile);                      //�������� ������������
   CloseFile(&hFile);
   if(ret == 0) return SetNewLanguage();                     //�������� � ��������� ����� �����
   ConfigDefault();                                          //��������� ������������ �� ���������
   return 0;
}

//------------------------------------------------------------------------------

static void Init_Dlg_Name(HWND hDlg)                         //������������ � ����������� �� �����
{
   SetWindowText(hDlg, (Lan+5)->msg);                        //������ ����� ���������
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
#if defined WRITE_YES                                        //����� ������ ��������
   SetDlgItemText(hDlg, IDC_GROUPBOX10, (Lan+165)->msg);
   SetDlgItemText(hDlg, IDC_CHECKBOX8, (Lan+166)->msg);
   if(pr_tRec == 1)                                          //������� ��������� 0 ��� 1 (��������� �����)
   {  ShowWindow(GetDlgItem(hDlg, IDC_GROUPBOX10), SW_HIDE);
      ShowWindow(GetDlgItem(hDlg, IDC_CHECKBOX8), SW_HIDE);
      Conf.WriteYes = 0;                                     //��� ������ ����� ������� ������
   }
#else
   ShowWindow(GetDlgItem(hDlg, IDC_GROUPBOX10), SW_HIDE);
   ShowWindow(GetDlgItem(hDlg, IDC_CHECKBOX8), SW_HIDE);
#endif
}

//------------------------------------------------------------------------------

static void Init_Dlg_ParForConfig(HWND hDlg)                 //��������� � ����������� �� ������������
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

static void Init_Dlg_Par(HWND hDlg)                          //��������� � ����������� �� ������������
{
   int State;
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON1+Conf.PrSort, BM_SETCHECK, 1, 0L);
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON3+Conf.altName, BM_SETCHECK, 1, 0L);
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON5+Conf.typeRec, BM_SETCHECK, 1, 0L);
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON7+Conf.typeSize, BM_SETCHECK, 1, 0L);
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON11+Conf.typeTime, BM_SETCHECK, 1, 0L);//0-���������� ����� ������ ������, 1-����� �������� �����
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON13+Conf.poz_Ql, BM_SETCHECK, 1, 0L);  //�������� ������: 0-� �������, 1-� �����, 2-�� ����������
   SendDlgItemMessage(hDlg, IDC_RADIOBUTTON16+Conf.poz_In, BM_SETCHECK, 1, 0L);  //�������� ������: 0-� �������, 1-� �����, 2-�� ����������

   State = (Conf.ViewVRO == 0) ? BST_CHECKED : BST_UNCHECKED;      //0-��������� ���������� *.vro, 1-�� ����������
   SendDlgItemMessage(hDlg, IDC_CHECKBOX6, BM_SETCHECK, State, 0L);
   State = (Conf.ViewPart == 0) ? BST_CHECKED : BST_UNCHECKED;      //0-��������� ���������� *.vro, 1-�� ����������
   SendDlgItemMessage(hDlg, IDC_CHECKBOX7, BM_SETCHECK, State, 0L);
   State = (Conf.ViewIDX == 0) ? BST_CHECKED : BST_UNCHECKED;      //0-�� ��������� ����� *.idx, 1-����������
   SendDlgItemMessage(hDlg, IDC_CHECKBOX1, BM_SETCHECK, State, 0L);
   State = (Conf.Ren_STR == 0) ? BST_CHECKED : BST_UNCHECKED;      //0-��������������� *.str � *.vro
   SendDlgItemMessage(hDlg, IDC_CHECKBOX2, BM_SETCHECK, State, 0L);
   State = (Conf.SwapNPart == 0) ? BST_CHECKED : BST_UNCHECKED;    //0-������������ ����� ����� � ����� �����
   SendDlgItemMessage(hDlg, IDC_CHECKBOX3, BM_SETCHECK, State, 0L);
   State = (Conf.ChangeNul == 0) ? BST_CHECKED : BST_UNCHECKED;    //0-�������� ������ '0' �� '_'
   SendDlgItemMessage(hDlg, IDC_CHECKBOX4, BM_SETCHECK, State, 0L);
   State = (Conf.NoViewOnePart == 0) ? BST_CHECKED : BST_UNCHECKED;//0-�� ��������������� ��� ����� �����
   SendDlgItemMessage(hDlg, IDC_CHECKBOX5, BM_SETCHECK, State, 0L);
   State = (Conf.WriteYes == 1) ? BST_CHECKED : BST_UNCHECKED;     //1-������� ������ ��������
   SendDlgItemMessage(hDlg, IDC_CHECKBOX8, BM_SETCHECK, State, 0L);
   Init_Dlg_ParForConfig(hDlg);                                    //��������� � ����������� �� ������������
}

//------------------------------------------------------------------------------

static void Create_SpisLangF(HWND hDlg)                      //�������� ������ �������� ������
{
   HANDLE File;
   WIN32_FIND_DATA Data;
   char NameF[260];
   char Driv[MAXDRIVE], Dir[MAXDIR], Name[MAXFILE], Ext[MAXEXT];
   if(GetModuleFileName(NULL, NameF, sizeof(NameF)) == 0) return;
   fnsplit(NameF, Driv, Dir, Name, Ext);                     //��������� ��� �����
   fnmerge(NameF, Driv, Dir, "*", ".lng");                   //�������� ��� �����
   if((File = FindFirstFile(NameF, &Data)) == INVALID_HANDLE_VALUE)  return; //���� �� ������
   for(;;)                                                   //������ �������� ������
   {  SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_ADDSTRING, 0, (LPARAM)Data.cFileName);
      if(FindNextFile(File, &Data) == 0)
          if(GetLastError() == ERROR_NO_MORE_FILES) break;   //������ ��� ������ � ����������� *.lng
   }
   FindClose(File);
}

//------------------------------------------------------------------------------

static void Init_SpisLang(HWND hDlg)                         //�������� ������ ������
{
   SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_ADDSTRING, 0, (LPARAM)"������� (Russian)");
   SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_ADDSTRING, 0, (LPARAM)"English");
   Create_SpisLangF(hDlg);                                   //�������� ������ �������� ������
   if(Conf.nLanguage != 2)                                   //0-�������, 1-����������, 2-������� ����
     SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_SETCURSEL, Conf.nLanguage, 0L);
   else
   {  int IndV = SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_FINDSTRING, -1, (LPARAM)NameF_Lan);
      if(IndV == LB_ERR)                                     //������ �� �������
         Error2((Lan+81)->msg, "(code -44)");                //Error2("������������ ������ ������ �� �������", "(code -44)");
      SendDlgItemMessage(hDlg, IDC_LISTBOX1, LB_SETCURSEL, IndV, 0L);    //������ �� ����� � ������
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
             Init_Dlg_Name(hDlg);                            //������������ � ����������� �� �����
             Init_Dlg_Par(hDlg);                             //��������� � ����������� �� ������������
             Init_SpisLang(hDlg);                            //�������� ������ ������
             return TRUE;
      case WM_COMMAND:
           switch(LOWORD(wParam))
           {   case IDOK:     if(SaveConfig(hDlg) < 0) break;//���������� ����� ������������
                              SetNewLanguage();              //�������� � ��������� ����� �����
                              EndDialog(hDlg, IDOK);
                              return TRUE;
               case IDCANCEL: EndDialog(hDlg, IDCANCEL);
                              return TRUE;
               case IDC_RADIOBUTTON1: Conf.PrSort = 0; break;//0-���������� �� �����, 1-��� ���������
               case IDC_RADIOBUTTON2: Conf.PrSort = 1; break;//0-���������� �� �����, 1-��� ���������
               case IDC_RADIOBUTTON3: Conf.altName = 0;
                                   Init_Dlg_ParForConfig(hDlg); //��������� � ����������� �� ������������
                                   break;
               case IDC_RADIOBUTTON4: Conf.altName = 1;
                                   Init_Dlg_ParForConfig(hDlg); //��������� � ����������� �� ������������
                                   break;
               case IDC_RADIOBUTTON5:  Conf.typeRec  = 0; break;//0-������ �����, 1-����� ����� RH2000
               case IDC_RADIOBUTTON6:  Conf.typeRec  = 1; break;//0-������ �����, 1-����� ����� RH2000
               case IDC_RADIOBUTTON7:  Conf.typeSize = 0; break;//0-�����, 1-������, 2-������, 3-������
               case IDC_RADIOBUTTON8:  Conf.typeSize = 1; break;//0-�����, 1-������, 2-������, 3-������
               case IDC_RADIOBUTTON9:  Conf.typeSize = 2; break;//0-�����, 1-������, 2-������, 3-������
               case IDC_RADIOBUTTON10: Conf.typeSize = 3; break;//0-�����, 1-������, 2-������, 3-������
               case IDC_RADIOBUTTON11: Conf.typeTime = 0; break;//0-���������� ����� ������ ������, 1-����� �������� �����
               case IDC_RADIOBUTTON12: Conf.typeTime = 1; break;//0-���������� ����� ������ ������, 1-����� �������� �����
               case IDC_RADIOBUTTON13: Conf.poz_Ql = 0;   break;//�������� ������: 0-� �������, 1-� �����, 2-�� ����������
               case IDC_RADIOBUTTON14: Conf.poz_Ql = 1;   break;//�������� ������: 0-� �������, 1-� �����, 2-�� ����������
               case IDC_RADIOBUTTON15: Conf.poz_Ql = 2;   break;//�������� ������: 0-� �������, 1-� �����, 2-�� ����������
               case IDC_RADIOBUTTON16: Conf.poz_In = 0;   break;//�������� ������: 0-� �������, 1-� �����, 2-�� ����������
               case IDC_RADIOBUTTON17: Conf.poz_In = 1;   break;//�������� ������: 0-� �������, 1-� �����, 2-�� ����������
               case IDC_RADIOBUTTON18: Conf.poz_In = 2;   break;//�������� ������: 0-� �������, 1-� �����, 2-�� ����������
               case IDC_CHECKBOX1: Conf.ViewIDX       = (Conf.ViewIDX == 0)       ? BYTE(1) : BYTE(0);//0-�� ��������� ����� *.idx, 1-����������
                                   break;
               case IDC_CHECKBOX2: Conf.Ren_STR       = (Conf.Ren_STR == 0)       ? BYTE(1) : BYTE(0);//0-��������������� *.str � *.vro
                                   break;
               case IDC_CHECKBOX3: Conf.SwapNPart     = (Conf.SwapNPart == 0)     ? BYTE(1) : BYTE(0);//0-������������ ����� ����� � ����� �����
                                   Init_Dlg_ParForConfig(hDlg);                                       //��������� � ����������� �� ������������
                                   break;
               case IDC_CHECKBOX4: Conf.ChangeNul     = (Conf.ChangeNul == 0)     ? BYTE(1) : BYTE(0);//0-�������� ������ '0' �� '_'
                                   break;
               case IDC_CHECKBOX5: Conf.NoViewOnePart = (Conf.NoViewOnePart == 0) ? BYTE(1) : BYTE(0);//0-�� ��������������� ��� ����� �����
                                   break;
               case IDC_CHECKBOX6: Conf.ViewVRO  = (Conf.ViewVRO == 0)  ? BYTE(1) : BYTE(0);//0-���������� ���������� *.vro, 1- �� ����������
                                   break;
               case IDC_CHECKBOX7: Conf.ViewPart  = (Conf.ViewPart == 0)  ? BYTE(1) : BYTE(0);//0-���������� Title � ���� �����
                                   break;
               case IDC_CHECKBOX8: if(Conf.WriteYes == 0)
                                      if(MsgYesNo5((Lan+167)->msg, (Lan+168)->msg, (Lan+169)->msg,
                                                   (Lan+170)->msg, (Lan+171)->msg) == 'N')
                                   {   SendDlgItemMessage(hDlg, IDC_CHECKBOX8, BM_SETCHECK, BST_UNCHECKED, 0L);
                                       break;
                                   }
                                   Conf.WriteYes  = (Conf.WriteYes == 0)  ? BYTE(1) : BYTE(0);//0-���������� Title � ���� �����
                                   break;
           }
           break;
   }
   return FALSE;
}

//------------------------------------------------------------------------------

void NewConfig(void)                                         //��������� ������������
{
   CONF oldConf = Conf;                                      //��������� ���������� ���������
   char oldNameF_Lan[260];                                   //��� ����� ����� ���� ��� ������
   lstrcpy(oldNameF_Lan, NameF_Lan);                         //��������� ���������� ���������
   if(DialogBox(MainInst, MAKEINTRESOURCE(IDD_DLG_CONFIG), MainWin, Dlg_NewConf) == IDCANCEL)
   {  Conf = oldConf;                                        //������������ ���������� ���������
      lstrcpy(NameF_Lan, oldNameF_Lan);                      //������������ ���������� ���������
      return;
   }
#if defined WRITE_YES                                        //����� ������ ��������
   if(Conf.WriteYes != oldConf.WriteYes)
      Message1((Lan+172)->msg);                              //"��������� � �������� ������ ������� � ���� ����� ����������� ���������."
#endif
   if(Conf.nLanguage != oldConf.nLanguage ||                 //0-�������, 1-����������, 2-������� ����
      (Conf.nLanguage == 2 && lstrcmp(NameF_Lan, oldNameF_Lan) != 0))
   {  SetWindowText(hCopy,  (Lan+4)->msg);                   //����� ������� �� ������
      SetWindowText(hConf,  (Lan+5)->msg);                   //����� ������� �� ������
      SetWindowText(hClose, (Lan+6)->msg);                   //����� ������� �� ������
      SetWindowText(hClear, (Lan+123)->msg);                 //����� ������� �� ������
      SetWindowText(hToTxt, (Lan+146)->msg);                 //����� ������� �� ������
#if defined WRITE_YES                                        //����� ������ ��������
      SetWindowText(hWriteFi, (Lan+177)->msg);               //����� ������� �� ������
      SetWindowText(hWriteFo, (Lan+178)->msg);               //����� ������� �� ������
      SetWindowText(hNew_Fo,  (Lan+179)->msg);               //����� ������� �� ������
      SetWindowText(hRenFi,   (Lan+163)->msg);               //"�������������"
      SetWindowText(hDelFi,   (Lan+187)->msg);               //"�������"
#endif
      ViewSize();                                            //������ ��������� ������������
   }
   if(prHDD_LG_Yes != 0) return;                             //�������, ����������� HDD: 0-����, 1-�� ������
   if(Conf.typeRec != oldConf.typeRec ||                     //0-������ �����, 1-����� ����� RH2000
      Conf.altName != oldConf.altName)                       //0-������������� ���, 1-�������� ���
      Update_MME_DB();                                       //������������ ������� ������������� ����
   if(Conf.PrSort !=        oldConf.PrSort    ||             //0-���������� �� �����, 1-��� ���������
      Conf.ViewIDX !=       oldConf.ViewIDX   ||             //0-�� ��������� ����� *.idx, 1-����������
      Conf.Ren_STR !=       oldConf.Ren_STR   ||             //0-��������������� *.str � *.vro
      Conf.SwapNPart !=     oldConf.SwapNPart ||             //0-������������ ����� ����� � ����� �����
      Conf.ChangeNul !=     oldConf.ChangeNul ||             //0-�������� ������ '0' �� '_'
      Conf.NoViewOnePart != oldConf.NoViewOnePart ||         //0-�� ��������������� ��� ����� �����
      Conf.altName != oldConf.altName ||                     //0-������������� ���, 1-�������� ���
      Conf.typeRec != oldConf.typeRec ||                     //0-������ �����, 1-����� ����� RH2000
      Conf.typeSize != oldConf.typeSize ||                   //0-�����, 1-������, 2-������, 3-������
      Conf.ViewVRO != oldConf.ViewVRO ||                     //0-��������� ���������� *.vro, 1-�� ����������
      Conf.ViewPart != oldConf.ViewPart ||                   //0-��������� ������ ����������������� Title
      Conf.typeTime != oldConf.typeTime ||                   //0-���������� ����� ������ ������, 1-����� �������� �����
      Conf.poz_Ql != oldConf.poz_Ql ||                       //�������� ������: 0-� �������, 1-� �����, 2-�� ����������
      Conf.poz_In != oldConf.poz_In)                         //�������� ������: 0-� �������, 1-� �����, 2-�� ����������
      Update_Tree();                                         //������������ ������ ������ � ����� ��� ��������� ��������
}

#endif


