
//                                    18.08.2008
#ifndef __HDD_LG_TO_TXT__
#define __HDD_LG_TO_TXT__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_to_txt =====================================
int OutTxt(void);                                            //������ ������ � ��������� ����

static HANDLE txtFile;
static int prOut;                                            //������� ������

//------------------------------------------------------------------------------

static int Out_Str(int pr, DWORD Level, char *NameF)         //����� ���� � ��������� ����
{
   DWORD nb, Nb, n;
   char Ss[300], *Adr;

   if(prOut == 0) return 0;                                  //������� ������
   if(Level == 0) return 0;                                  //��� ������ � ��������
   Nb = 2 * (Level - 1);
   for(DWORD i=0; i<Nb; i++) *(Ss+i) = ' ';
   for(n=0; n<5; n++) if(*(NameF+n) != ' ') break;           //���������� ������� � ������ ������� �����
   Adr = Ss + Nb;
   if(pr == 0)                                               //�����
        Nb += sprintf(Adr, "[%s]\r\n", NameF+n);
   else Nb += sprintf(Adr, "%s\r\n", NameF+n);

   if(WriteFile(txtFile, Ss, Nb, &nb, NULL) == FALSE || nb != Nb)
      return ErrorSys1((Lan+83)->msg);                       //return ErrorSys1("������ ��� ������ ��������� �����.");
   return 0;
}

//------------------------------------------------------------------------------

static int Out_Folder_Txt(HTREEITEM hitem, DWORD Level, char *NameF)//����� ���� � ��������� ����
{
   if(Level == 0) prOut = 0;
   if(Level == 1)
   { prOut = 0;
     if(Conf.OutJ_BOX == 0 && lstrcmp(NameF, "JUKEBOX") == 0) prOut = 1;
     if(Conf.OutMEDIA == 0 && lstrcmp(NameF, "MEDIA") == 0) prOut = 1;
     if(Conf.OutPHOTO == 0 && lstrcmp(NameF, "PHOTO") == 0) prOut = 1;
     if(Conf.OutVIDEO == 0 && lstrcmp(NameF, "VIDEO") == 0) prOut = 1;
   }
   if(Out_Str(0, Level, NameF) < 0) return -1;               //����� - ����� ���� � ��������� ����
   HTREEITEM ind = TreeView_GetChild(hwndTree, hitem);       //����� �� ���������� ������� ������
   if(ind == NULL) return 0;
   for(;;)                                                   //�������� ����� ������ �� ������� �����
   {  TV_ITEM item;
      char Msg[256];
      item.mask = TVIF_TEXT | TVIF_PARAM;
      item.hItem = ind;
      item.pszText = Msg;
      item.cchTextMax = 256;
      if(TreeView_GetItem(hwndTree, &item) == FALSE)
        return Error1((Lan+33)->msg);                        //return Error1("������ ��� ������� ���������� �� �������� ������.");
      if((aTree + item.lParam)->pf.type == 48)               //��������� ��� ��� �����
      {  if(Out_Folder_Txt(ind, (aTree + item.lParam)->Level, (aTree + item.lParam)->NameF) < 0) return -1; }//����������� ������ ����� �����
//    else
//    if((aTree + item.lParam)->pf.type == 47)
//    {  if(Out_Folder_Txt(ind, (aTree + item.lParam)->Level, Msg) < 0) return -1; }//����������� ������ ����� �����
      else                                                   //��������� ��� ��� ����
//    {  if(Out_Str(1, (aTree + item.lParam)->Level, (aTree + item.lParam)->NameF) < 0) return -1; }                        //����� ���� � ��������� ����
      {  if(Out_Str(1, (aTree + item.lParam)->Level, Msg) < 0) return -1; } //����� ���� � ��������� ����
      ind = TreeView_GetNextSibling(hwndTree, ind);          //��������� ������ �� ��� �� ������ ������
      if(ind == NULL) break;                                 //������ ������� ������� ������ ���
   }
   return 0;
}

//------------------------------------------------------------------------------

static int MakeFileTxt(void)                                 //������ ������ � ��������� ����
{
   TV_ITEM item;
   char Msg[256];
   HTREEITEM ind = TreeView_GetRoot(hwndTree);               //������ ����� � ������ �����
   item.mask = TVIF_TEXT | TVIF_PARAM;
   item.hItem = ind;
   item.pszText = Msg;
   item.cchTextMax = 256;
   if(TreeView_GetItem(hwndTree, &item) == FALSE)
      return Error1((Lan+33)->msg);                          //return Error1("������ ��� ������� ���������� �� �������� ������.");
   return Out_Folder_Txt(ind, (aTree + item.lParam)->Level, (aTree + item.lParam)->NameF); //����� ���� � ��������� ����
}

//------------------------------------------------------------------------------

static void SetCHECKBOX(HWND hDlg, int IDC_, BYTE Par)
{
   WPARAM State = (Par == 0) ? BST_CHECKED : BST_UNCHECKED;      //0-��������� ���������� *.vro, 1-�� ����������
   SendDlgItemMessage(hDlg, IDC_, BM_SETCHECK, State, 0L);
}

//------------------------------------------------------------------------------

static void ModPar(BYTE *Par)                                //��������� ��������� ������
{
   *Par = (*Par == 0) ? BYTE(1) : BYTE(0);
   WriteConfig();                                            //������ ����� ������������
}

//------------------------------------------------------------------------------

#pragma argsused
static BOOL CALLBACK AddDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)
   {  case WM_INITDIALOG:
             SetDlgItemText(hDlg, IDC_GROUPBOX1, (Lan+147)->msg);
             SetCHECKBOX(hDlg, IDC_CHECKBOX1, Conf.OutJ_BOX);
             SetCHECKBOX(hDlg, IDC_CHECKBOX2, Conf.OutMEDIA);
             SetCHECKBOX(hDlg, IDC_CHECKBOX3, Conf.OutPHOTO);
             SetCHECKBOX(hDlg, IDC_CHECKBOX4, Conf.OutVIDEO);
           break;
      case WM_COMMAND:
           switch(LOWORD(wParam))
           {  case IDC_CHECKBOX1: ModPar(&Conf.OutJ_BOX); return TRUE;
              case IDC_CHECKBOX2: ModPar(&Conf.OutMEDIA); return TRUE;
              case IDC_CHECKBOX3: ModPar(&Conf.OutPHOTO); return TRUE;
              case IDC_CHECKBOX4: ModPar(&Conf.OutVIDEO); return TRUE;
           }
           return TRUE;
      default:
           return FALSE;
   }
   return TRUE;
}

//------------------------------------------------------------------------------

int OutTxt(void)                                             //������ ������ � ��������� ����
{
   char NameF[256];
   SYSTEMTIME SysT;                                          //������� �����
   GetLocalTime(&SysT);                                      //������� �����
   wsprintf(NameF, "hdd_lg.%04d_%02d_%02d.txt", SysT.wYear, SysT.wMonth, SysT.wDay);
   prOut = 0;                                                //������� ������
   ModDlg.fnHook = (LPOFNHOOKPROC)AddDlgProc;
   ModDlg.DopDlgName = MAKEINTRESOURCE(IDD_DLG_FOLDER);
   ModDlg.Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;
   if(GetName_OutFile(NameF) < 0) return -1;                 //���� ����� ��� ������ �����
   int ret = Open_OutFileAlw(NameF, &txtFile);               //�������� ��������� �����
   ClearDopDlg();                                            //������� ���������� ��������������� �������
   if(ret < 0) return -1;
   ret = MakeFileTxt();                                      //������ ������ � ��������� ����
   CloseFile(&txtFile);                                      //������� ������� ����
   Message2((Lan+155)->msg, NameF);
   return ret;
}

#endif

