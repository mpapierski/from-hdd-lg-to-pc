
//                                04.11.2009
#ifndef __HDD_LG_WRITE_2__
#define __HDD_LG_WRITE_2__

#include "from_hdd_lg_to_pc.h"

#if defined WRITE_YES                                        //����� ������ ��������

//============================ hdd_lg_write_2 ==================================
BOOL CALLBACK Dlg_NewName(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int  RenameFiFo(HTREEITEM hitem, OneStrTree *aTree);         //��������������
int  DeleteFiFo(HTREEITEM hitem, OneStrTree *aTree);         //��������
int  Change_Sec67(int num_Cl);                               //�������� ��������� � ���������� ������� 67
int  Correct67Sec(void);                                     //��������� ����������� 67 �������
     char NameFoFi[256];                                     //��� ����������� �����
     DWORD MaxClast;                                         //����� ������ �������� �������� ��������������� ��� ������

static int num_del_Cl;                                       //����� ��������� ���������

//------------------------------------------------------------------------------

#pragma argsused
BOOL CALLBACK Dlg_NewName(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)
   {  case WM_INITDIALOG:
             SetWindowText(hDlg, (Lan+183)->msg);            //"������� ���"
             SetDlgItemText(hDlg, IDC_STATICTEXT1, (Lan+185)->msg);  //"�� ����� 38 ��������"
             SetDlgItemText(hDlg, IDOK, (Lan+74)->msg);      //"���������"
             SetDlgItemText(hDlg, IDCANCEL, (Lan+75)->msg);  //"�������"
             if(lParam != 0)
             {  SetDlgItemText(hDlg, IDC_EDIT1, (char*)lParam);
                int l = lstrlen((char*)lParam);
                SendDlgItemMessage(hDlg, IDC_EDIT1, EM_SETSEL, 0, l);   //������� ����� ���������� � ��������� ������ � ����� ������
             }
             SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
             return false;                                   //��� ��������� ������ �����
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
                             {  Error1((Lan+184)->msg);      //"������������ ���"
                                SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
                                return true;
                             }
                             for(int i=0; *(NameFoFi+i)!=0; i++)
                               for(int j=0; j<sizeof(ndS); j++)
                                 if(*(NameFoFi+i) == *(ndS+j))
                                 {  Error1((Lan+184)->msg);  //"������������ ���"
                                    SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
                                    return true;
                                 }
                             *(NameFoFi+38) = 0;             //���� ��� �������, �� ��� ���������
              case IDCANCEL:
                             EndDialog(hDlg, LOWORD(wParam));//������� ���� �� �����
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
             SetWindowText(hDlg, (Lan+36)->msg);             //"��������"
             SetDlgItemText(hDlg, IDC_STATICTEXT1, (Lan+191)->msg);
             SetDlgItemText(hDlg, IDC_STATICTEXT2, (char*)lParam);
             SetDlgItemText(hDlg, IDOK, (Lan+187)->msg);     //"�������"
             SetDlgItemText(hDlg, IDCANCEL, (Lan+75)->msg);  //"�������"
             return true;                                    //��� ��������� ������ �����
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
              case IDCANCEL: EndDialog(hDlg, LOWORD(wParam));//������� ���� �� �����
                             return true;
           }
   }
   return false;
}

//------------------------------------------------------------------------------

static int GetName_FiFo(OneStrTree *aTree, char *oldName, char *oldExt)//��������� ������ �������� � ������ ������
{
   One_Str_Cat *Kat = (One_Str_Cat *)bufDir;
   //������ �������� �������� (�������� ����� � �� ������������ �, � ��� ����� �������� ������ �������� ������)
   //���������� �������� (���������� ������ ������ �����) � � ��� ����� �������� ������ ��������
   DWORD nSector = Start_SecDir1 + (ClStDir - 1) * sClSec;   //����� ������� �� ������ ��������
   if(ReadClast1_P(nSector, bufDir) < 0) return -1;          //������ ��������
   if(*((DWORD*)&(Kat + 0)->Name) != 0x00002E00)             //��� ���� ����� �.�. ������� �������� ��������
      return Error1((Lan+96)->msg);                          //"�������� ��������� ��������."
   if(*((DWORD*)&(Kat + 1)->Name) != 0x2E002E00)             //��� ��� ����� �.�. ������� ����������� ��������
      return Error1((Lan+96)->msg);                          //"�������� ��������� ��������."
   WCHAR NameU[64], ExtU[8];                                 //����� � Unicode
   lstrcpy(oldName, aTree->NameF);
   *oldExt = 0;
   if(aTree->pf.type != 48)                                  //��� ����
   {  char *adr = strrchr(oldName, '.');
      if(adr != NULL)
      {  *adr = 0;                                           //������� ���
         lstrcpy(oldExt, adr+1);                             //�������� ����������
      }
   }
   Ansi_To_Unicode(oldName, NameU, 40);                      //������������� ��� � UNICODE
   Ansi_To_Unicode(oldExt, ExtU, 4);                         //������������� ���������� �� UNICODE
   WORD *nameI = (WORD*)NameU;
   WORD *extI = (WORD*)ExtU;
   int nsDir = 2;                                            //����� ������ �������� � ������ ������
   int t1 = 0;
   for(; nsDir<4*sClSec; nsDir++)                            //�� ���� ��������� ������� � ����� �������� �������� (� ������ ������� 4 ������)
   {  if((Kat + nsDir)->pf.type == 0) break;                 //����� ������� ��������
      WORD *nameK = (WORD*)(Kat + nsDir)->Name;
      int t;
      for(t=0; t<40; t++)                                    //�� ����� �����
      { if(*(nameI + t) == 0) break;                         //������� ��� �����������
        if(*(nameK + t) == 0) break;                         //��� � �������� �����������
        if(*(nameI + t) != *(nameK + t)) break;              //������������ ��������
      }
      if(*(nameI + t) != 0 || *(nameK + t) != 0) continue;   //�� ����� �� ����� ����� ����
      if(!(t >= 40 || *(nameI + t) == 0)) continue;          //���� ������ ��� ��������� �� ����� ������� ���� �� ����� �� ����� �����
      if(aTree->pf.type != 48)                               //��� ����
      {  WORD *extK = (WORD*)(Kat + nsDir)->Ext;
         int t;
         for(t=0; t<4; t++)                                  //�� ����� �����
           if(*(extI+t) == *(extK + t)) continue;
         if(!(t >= 4 || *(extI+t) == 0)) continue;           //���� ������ ��� ��������� ������� ���� �����  �� ����� �����
      }
      t1 = 1;                                                //������� ��� � ��������
      break;                                                 //�������� ����������
   }
   if(t1 == 0)
      return Error1((Lan+188)->msg);                         //"� �������� �� ������� ��������� ���."
   return nsDir;
}

//------------------------------------------------------------------------------

int  RenameFiFo(HTREEITEM hitem, OneStrTree *aTree)          //��������������
{
   char oldName[80], oldExt[8];
   One_Str_Cat *Kat = (One_Str_Cat *)bufDir;
   int nsDir = GetName_FiFo(aTree, oldName, oldExt);         //��������� ������ �������� � ������ ������
   if(nsDir < 0) return -1;
// lstrcat(oldName, ".");
// lstrcat(oldName, oldExt);
   if(DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_NAME),
                MainWin, Dlg_NewName, LPARAM(oldName)) == IDCANCEL) return -1;
   Ansi_To_Unicode(NameFoFi, (Kat + nsDir)->Name, 40);       //������������� ��� �� UNICODE � ������ � �������
#if !defined EMULATOR_HDD                                    //����� ���������
   if(Save_Dir() < 0) return -1;                             //���������� �������� ��������
#endif
   char NewStr[256];
   if(MakeOneStrForTree(NewStr, NameFoFi, oldExt, &((Kat + nsDir)->pf)) == 1) return 0;//�������� ������ �������� ��� ��������� � ������
   TV_ITEM item;                                             //������ ������
   item.mask = TVIF_TEXT;                                    //������ ������ �����
   item.hItem = hitem;
   item.pszText = NewStr;
   item.cchTextMax = 256;
   if(TreeView_SetItem(hwndTree, &item) == FALSE)            //����� ������� ������� ������
      return Error1((Lan+33)->msg);                          //"������ ��� ������� ���������� �� �������� ������."
   TreeView_EnsureVisible(hwndTree, hitem);
   lstrcpy(aTree->NameF, NameFoFi);
//   lstrcat(aTree->NameF, ".");
//   lstrcat(aTree->NameF, oldExt);
   return 0;
}

//------------------------------------------------------------------------------

static int Del_OneFileFAT(OneStrTree *aTree)                 //�������� ������ ����� �� FAT (������� ����� ��� ����������)
{
   DWORD nCl = aTree->pf.ClSt;                               //������� ����� �������� ����� ������� �������� �����
   DWORD numCl = DWORD((aTree->pf.SizeF + sCl_B - 1) / sCl_B);//����� ��������� ����������� ��� ���������� ����� ������� �������
   for(DWORD i=0; i<numCl; i++)                              //�� ����� ���������
   {  if(nCl == 0x0FFFFFFF)                                  //������� ����� �������
        return Error1((Lan+12)->msg);                        //return Error1("���������� ������ ������� ����� ������� FAT.");
      if(*(FAT1 + nCl) == 0)                                 //������������ ��������
        return Error1((Lan+11)->msg);                        //return Error1("���������� �������������� �������� FAT � ������ �� ������� �����.");
      *(c_FAT1 + nCl) = 0;                                   //���������� �������
      nCl = *(FAT1 + nCl);                                   //����� ���������� ��������
      if(nCl > maxZapFAT1 && nCl != 0x0FFFFFFF)
        return Error1((Lan+13)->msg);                        //"����� �������� ��������� ���������� ��������."
   }
   num_del_Cl += numCl;                                      //����� ��������� ���������
   return 0;
}

//------------------------------------------------------------------------------

static int Del_DirFAT(OneStrTree *aTree)                     //�������� �������� �� FAT (������� ����� ��� ����������)
{
   DWORD nCl = aTree->pf.ClSt;                               //������� ����� �������� ����� ������� �������� �����
   if(*(FAT1 + nCl) != 0x0FFFFFFF)                           //������� ����� �������
      return Error1((Lan+18)->msg);                          //���������� �������������� FAT � ��������� �� ������� ��������.
   *(c_FAT1 + nCl) = 0;                                      //���������� �������
   num_del_Cl++;                                             //����� ��������� ���������
   return 0;
}

//------------------------------------------------------------------------------

static int Del_Folder(HTREEITEM hitem, OneStrTree *aTreeDir) //�������� �����
{
   if(Del_DirFAT(aTreeDir) < 0) return -1;                   //�������� �������� �� FAT (������� ����� ��� ����������)

   HTREEITEM ind = TreeView_GetChild(hwndTree, hitem);       //����� �� ���������� ������� ������
   if(ind == NULL) return 0;                                 //��� ����������� ������, �.�. ����� ������
   for(;;)                                                   //�������� ����� ������ �� ������� �����
   {  TV_ITEM item;
      item.mask = TVIF_PARAM;
      item.hItem = ind;
      if(TreeView_GetItem(hwndTree, &item) == FALSE)
        return Error1((Lan+33)->msg);                        //return Error1("������ ��� ������� ���������� �� �������� ������.");
      if((aTree + item.lParam)->pf.type == 48)               //��������� ��� ��� �����
      {   if(Del_Folder(ind, aTree + item.lParam) < 0) return -1; } //�������� �����
      else                                                   //��������� ��� ��� ����
          Del_OneFileFAT(aTree + item.lParam);               //�������� ������ ����� �� FAT (������� ����� ��� ����������)
      ind = TreeView_GetNextSibling(hwndTree, ind);          //��������� ������ �� ��� �� ������ ������
      if(ind == NULL) break;                                 //������ ������� ������� ������ ���
   }
   return 0;
}

//------------------------------------------------------------------------------

int  DeleteFiFo(HTREEITEM hitem, OneStrTree *aTree)          //�������� ����� ��� ��������
{
   char oldName[80], oldExt[8];

   One_Str_Cat *Kat = (One_Str_Cat *)bufDir;
   int nsDir = GetName_FiFo(aTree, oldName, oldExt);         //��������� ������ �������� � ������ ������
   if(nsDir < 0) return -1;
   if(*oldExt != 0)
   {  lstrcat(oldName, ".");
      lstrcat(oldName, oldExt);
   }
   if(DialogBoxParam(MainInst, MAKEINTRESOURCE(IDD_DLG_DEL_NAME),
                MainWin, Dlg_DelName, LPARAM(oldName)) == IDCANCEL) return -1;
   c_FAT1 = (DWORD *)MyAllocMem(Size_FAT1);                  //������ ��� FAT
   if(c_FAT1 == NULL)  return -1;
   CopyMemory(c_FAT1, FAT1, Size_FAT1);                      //����������� ���������� FAT1

   *((WORD*)&(Kat + nsDir)->Name) = 0xE500;                  //������� ������ � ������ � ��������
   (Kat + nsDir)->pf.ClSt = 0;                               //������ ������� ��������� ������
#if !defined EMULATOR_HDD                                    //����� ���������
   if(Save_Dir() < 0) return -1;                             //���������� �������� ��������
#endif
   num_del_Cl = 0;                                           //����� ��������� ���������
   int ret = (aTree->pf.type == 48) ?                        //������� ����� ��� �����
       Del_Folder(hitem, aTree) :                            //�������� �����
       Del_OneFileFAT(aTree);                                //�������� ������ ����� �� FAT
   if(ret < 0) return -1;
   TreeView_DeleteItem(hwndTree, hitem);                     //�������� ����� � ������ ����� �������� ����� ����������
// ���� �� ��� ������� ��� � ���� ������� ������ aTree, ���� �������� ��� �� ����� ��������
#if !defined EMULATOR_HDD                                    //����� ���������
   if(Save_FAT1() < 0) return -1;                            //���������� ����������� FAT ������� �������
   if(Change_Sec67(num_del_Cl) < 0) return -1;               //�������� ��������� � ���������� ������� 67
#endif
   writeCl -= num_del_Cl;                                    //����� ���������� ���������
   ViewSize();                                               //������ ��������� ������������
   return 0;
}

//------------------------------------------------------------------------------

#if !defined EMULATOR_HDD                                    //��� ������ ���������
int WriteSec(BYTE *buff)                                     //������ �������
{
   DWORD nb;
   if(WriteFile(hDrive, buff, sSecB, &nb, NULL) == FALSE || nb != sSecB)
      return ErrorSys1((Lan+175)->msg);                      //������ ��� ������ ��������. (���� ���������: ������ ��� ������ �������.)
   return 0;
}
#endif

//------------------------------------------------------------------------------

#if !defined EMULATOR_HDD                                    //��� ������ ���������
int Change_Sec67(int num_Cl)                                 //�������� ��������� � ���������� ������� 67
{
   SEC_67 Sec67;
   LONGLONG Poz = LONGLONG(sSecB) * 67;
   SetInFilePointer(Poz);                                    //������ � ������ 67 �������
   ReadSec((BYTE *)&Sec67);                                  //������ 67 �������
   Sec67.FreeCl += num_Cl;                                   //�������� ����� ��������� ���������
   if(MaxClast+1 > Sec67.LastCl) Sec67.LastCl = MaxClast+1;  //����� ������ �������� ���������� ��������
   SetInFilePointer(Poz);                                    //������ � ������ 67 �������
   return WriteSec((BYTE *)&Sec67);                          //������ �������
}
#endif

//------------------------------------------------------------------------------

int  Correct67Sec(void)                                      //��������� ����������� 67 �������
{
   int n;
   for(n=maxZapFAT1-1; n>1; n--)
     if(*(FAT1+n) != 0) break;
   int FreeFat = maxZapFAT1 - 2;                             //����� ��������� ���������
   for(int i=2; i<n; i++)                                    //n+1 - ������ ������� �������������� ������� FAT
     if(*(FAT1+i) != 0)  FreeFat--;
   SEC_67 Sec67;
   LONGLONG Poz = LONGLONG(sSecB) * 67;
   SetInFilePointer(Poz);                                    //������ � ������ 67 �������
   ReadSec((BYTE *)&Sec67);                                  //������ 67 �������
// char ss1[300], ss2[300];
// sprintf(ss1, "���������: �������� - %8d,  ������ �������������� - %8d", FreeFat, n+1);
// sprintf(ss2, "��������:  �������� - %8d,  ������ �������������� - %8d", Sec67.FreeCl, Sec67.LastCl);
// Message2(ss1, ss2);
   if(DWORD(FreeFat) == Sec67.FreeCl && DWORD(n+1) >= Sec67.LastCl)
   {   Message1((Lan+194)->msg);
       return 0;
   }
   Sec67.FreeCl = FreeFat;
   if(DWORD(n+1) > Sec67.LastCl)
      Sec67.LastCl = n + 1;
   SetInFilePointer(Poz);                                    //������ � ������ 67 �������
   if(WriteSec((BYTE *)&Sec67) != -1)                        //������ �������
      Message1((Lan+195)->msg);
   return 0;
}

#endif
#endif

