
//                                    24.01.2009
#ifndef __HDD_LG_MAKE_TREE__
#define __HDD_LG_MAKE_TREE__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_make_tree ================================
int  AddItemToTree_forWrite(char *Name, char *Ext, PAR_FILE *pf);  //���������� ����� ������ � ������ ��� ������
int  AddItemToTree(char *Name, char *Ext, PAR_FILE *pf, int Level);//���������� ����� ������ � ������
void Expand_Tree(int hitem);                                 //������������ ������
int  ClearSelect(void);                                      //������ ���� ���������
int  RemoveSelectionFromCopy(int n);                         //������ ��������� ����� � ������ ��� �����������
bool ChangeSelect(int sShift);                               //��������� ������ ������ � ������
bool ChangeSelect_and_Down(void);                            //��������� ������ ������ � ������
void ViewSize(void);                                         //����� ��������� ������������
     int numEl_Tree;                                         //����� ��������� � ������ ����
     OneStrTree *aTree;                                      //���������� �� ������
     HTREEITEM hPrev[MAX_U];                                 //������ ������� ����������� ������

static int pr_tviSt;                                         //������� ���������� ������
static HTREEITEM item_Sel[2];                                //������ � ������ ������ ���������� ������
static HTREEITEM item_Par1;                                  //������������ ����� ��� ������� ������

//------------------------------------------------------------------------------
#if defined WRITE_YES                                        //����� ������ ��������
int AddItemToTree_forWrite(char *Name, char *Ext, PAR_FILE *pf)//���������� ����� ������ � ������ ��� ������
{
   TV_INSERTSTRUCT tvins;
   char oneStr[256];
   int Level = (aTree + item_Add.lParam)->Level + 1;
   if(Level >= MAX_U)
      return Error1((Lan+30)->msg);                          //return Error2("������� ����������� ����� � ������", "��������� ����������� ���������.");
   if(MakeOneStrForTree(oneStr, Name, Ext, pf) == 1) return 0;//�������� ������ �������� ��� ��������� � ������
   int ind = (pf->type == 48) ? 0 : 1;                       //��� ��� ����� (������ � �������� 0), ��� ���������� �� ��������� ������ 1
   if(pf->type < 48 && Ext != NULL && *Ext != 0)             //�� ����� � ���� ����������
   {  ind = 1;                                               //������ ������
      if(lstrcmpi(Ext, "jpg") == 0) ind = 2;
      if(lstrcmpi(Ext, "mp3") == 0) ind = 6;
      if(lstrcmpi(Ext, "avi") == 0) ind = 4;
      if(lstrcmpi(Ext, "vro") == 0) ind = 5;
      if(lstrcmpi(Ext, "str") == 0) ind = 5;
      if(ind == 5)
      {  if(pf->type == 33) ind = 7;                         //������ ���� 1
         if(pf->type == 34) ind = 8;                         //������ ���� 2
      }
      if(lstrcmpi(Ext, "divx") == 0)ind = 13;
      if(lstrcmpi(Ext, "mpg") == 0) ind = 14;
   }
   lstrcpy((aTree + numEl_Tree)->NameF, Name);
   (aTree + numEl_Tree)->pf = *pf;
   (aTree + numEl_Tree)->prSel = 0;                          //������� ������ ������� �����(0-�� ������)
   (aTree + numEl_Tree)->Level = WORD(Level);                //������� �����������
// (aTree + numEl_Tree)->indTabMME = WORD(indTabMME);        //������ � ������� MME.DB
   (aTree + numEl_Tree)->indTabMME = WORD(-1);               //������ � ������� MME.DB
   tvins.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   tvins.item.pszText = oneStr;
   tvins.item.lParam = numEl_Tree;                           //������ �������� � ������ ����
   tvins.item.iImage = ind;
   tvins.item.iSelectedImage = ind;                          //������ ������
   if(Conf.PrSort == 0)                                      //0-���������� �� �����, 1-��� ����������
      tvins.hInsertAfter = TVI_SORT;
   else
      tvins.hInsertAfter = TVI_LAST;
   tvins.hParent = item_Add.hItem;
   HTREEITEM hItemN = TreeView_InsertItem(hwndTree, &tvins);
   if(++numEl_Tree > MAX_NAME)
      return Error1((Lan+31)->msg);                          //return Error2("��������� ����� ���� ����� � ������ ��������� ����������� ���������.");
// TreeView_Expand(hwndTree, item_Add.hItem, TVE_EXPAND);
   TreeView_EnsureVisible(hwndTree, hItemN);
   if(pf->type == 48)                                        //��� ����� � ������ ��� ������� ������ ��� ������ ���� � ���
   {  item_Add.mask = TVIF_TEXT | TVIF_PARAM;// | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
      item_Add.hItem = hItemN;
      if(TreeView_GetItem(hwndTree, &item_Add) == FALSE)     //����� ������� ������� ������
         return Error1((Lan+33)->msg);                       //"������ ��� ������� ���������� �� �������� ������."
   }
   return 0;
}
#endif
//------------------------------------------------------------------------------

int AddItemToTree(char *Name, char *Ext, PAR_FILE *pf, int Level)//���������� ����� ������ � ������
{
   TV_INSERTSTRUCT tvins;
   char oneStr[256];

   if(Level >= MAX_U)
      return Error1((Lan+30)->msg);                          //return Error2("������� ����������� ����� � ������", "��������� ����������� ���������.");
   if(MakeOneStrForTree(oneStr, Name, Ext, pf) == 1) return 0;//�������� ������ �������� ��� ��������� � ������
   int ind = (pf->type == 48) ? 0 : 1;                       //��� ��� ����� (������ � �������� 0), ��� ���������� �� ��������� ������ 1
   if(pf->type < 48 && Ext != NULL && *Ext != 0)             //�� ����� � ���� ����������
   {  ind = 1;                                               //������ ������
      if(lstrcmpi(Ext, "jpg") == 0) ind = 2;
      if(lstrcmpi(Ext, "mp3") == 0) ind = 6;
      if(lstrcmpi(Ext, "avi") == 0) ind = 4;
      if(lstrcmpi(Ext, "vro") == 0) ind = 5;
      if(lstrcmpi(Ext, "str") == 0) ind = 5;
      if(ind == 5)
      {  if(pf->type == 33) ind = 7;                         //������ ���� 1
         if(pf->type == 34) ind = 8;                         //������ ���� 2
      }
      if(lstrcmpi(Ext, "divx") == 0)ind = 13;
      if(lstrcmpi(Ext, "mpg") == 0) ind = 14;
   }
   lstrcpy((aTree + numEl_Tree)->NameF, Name);
   (aTree + numEl_Tree)->pf = *pf;
   (aTree + numEl_Tree)->prSel = 0;                          //������� ������ ������� �����(0-�� ������)
   (aTree + numEl_Tree)->Level = WORD(Level);                //������� �����������
   (aTree + numEl_Tree)->indTabMME = WORD(indTabMME);        //������ � ������� MME.DB
   tvins.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   tvins.item.pszText = oneStr;
   tvins.item.lParam = numEl_Tree;                           //������ �������� � ������ ����
   tvins.item.iImage = ind;
   tvins.item.iSelectedImage = ind;                          //������ ������
   if(Conf.PrSort == 0)                                      //0-���������� �� �����, 1-��� ����������
      tvins.hInsertAfter = TVI_SORT;
   else
      tvins.hInsertAfter = TVI_LAST;
   if(Level == 0) tvins.hParent = NULL;
   else tvins.hParent = hPrev[Level-1];
   hPrev[Level] = TreeView_InsertItem(hwndTree, &tvins);
   if(++numEl_Tree > MAX_NAME)
      return Error1((Lan+31)->msg);                          //return Error2("��������� ����� ���� ����� � ������ ��������� ����������� ���������.");
   return 0;
}

//------------------------------------------------------------------------------

void Expand_Tree(int hitem)                                  //Tree-View Controls
{
   TreeView_Expand(hwndTree, hPrev[hitem], TVE_EXPAND);
}

//------------------------------------------------------------------------------

static void ViewNumSel(void)                                 //����� ����� ��������� ������
{
   char Ss1[64], Ss2[64], *as;
   if(num_Sel > 0)
   {  wsprintf(Ss1, "%s %d", (Lan+121)->msg, num_Sel);
      sprintf(Ss2, "%10.2lf", double(size_Sel)/1024.0/1024.0);
      as = Char_Dig_p_n(Ss2, 11, 2);                         //�������������� ����������� ����� � ��������
      sprintf(Ss2, "( %s Mb )",  as);
   }
   else
     *Ss1 = *Ss2 = 0;
   SetWindowText(hNumSel, Ss1);
   SetWindowText(hSizeSel, Ss2);
}

//------------------------------------------------------------------------------

static void StateButtonClear(void)                           //��������� ��������� ������ "��������"
{
   bool State = (num_Sel == 0) ? false : true;
   EnableWindow(hClear, State);
}

//------------------------------------------------------------------------------

static int RemoveAllSelection(HTREEITEM hitem)               //����� ��� ��������� ����� � ������
{
   char nName[300];
   HTREEITEM ind = TreeView_GetChild(hwndTree, hitem);       //����� �� ���������� ������� ������
   if(ind == NULL) return 0;                                 //��� ��������� ������ ����� ������ �� ������
   for(;;)                                                   //�������� ����� ������ �� ������� �����
   {  TV_ITEM item;
      item.mask = TVIF_PARAM | TVIF_TEXT;
      item.pszText = nName;
      item.cchTextMax = sizeof(nName);
      item.hItem = ind;
      if(TreeView_GetItem(hwndTree, &item) == false)
         return Error1((Lan+33)->msg);                       //"������ ��� ������� ���������� �� �������� ������."
      if((aTree + item.lParam)->pf.type == 48)               //��������� ��� ��� ��������� �����
      {  if(RemoveAllSelection(ind)  < 0) return -1;  }      //����� ��������� ����� � ������
      else                                                   //��������� ��� ��� ����
      {  if((aTree + item.lParam)->prSel == 1)               //����� ���������� �������
         {  item.mask = TVIF_STATE | TVIF_TEXT;
            item.state = 0;
            item.stateMask = TVIS_BOLD;
            *nName = ' ';
            if(TreeView_SetItem(hwndTree, &item) == -1)
              Error1((Lan+151)->msg);
            (aTree + item.lParam)->prSel = 0;
            if((aTree + item.lParam)->pf.type != 47)         //��� ����� ����������������� Title
            {  num_Sel--;                                    //����� ���������� ������
               size_Sel -= (aTree + item.lParam)->pf.SizeF;
            }
         }
      }
      if((aTree + item.lParam)->pf.type == 47)               //��������� ��� ��� ����� �������������� Title
      {  if(RemoveAllSelection(ind)  < 0) return -1;  }      //����� ��������� ����� � ������
      ind = TreeView_GetNextSibling(hwndTree, ind);          //��������� ������ �� ��� �� ������ ������
      if(ind == NULL) return 0;                              //������ ������� ������� ������ ���
   }
}

//------------------------------------------------------------------------------

int ClearSelect(void)                                        //������ ���� ���������
{
   HTREEITEM hitem = TreeView_GetRoot(hwndTree);
   if(hitem == NULL) return -1;
   int ret = RemoveAllSelection(hitem);                      //����� ��� ��������� ����� � ������
   if(ret < 0) return -1;
   ViewNumSel();                                             //����� ����� ���������
   StateButtonClear();                                       //��������� ��������� ������ "��������"
   return ret;
}

//------------------------------------------------------------------------------

static int Remove_Selection(HTREEITEM hitem, int n)          //����� ��������� ����� � ������
{
   char nName[300];
   HTREEITEM ind = TreeView_GetChild(hwndTree, hitem);       //����� �� ���������� ������� ������
   if(ind == NULL) return 0;                                 //��� ��������� ������ ����� ������ �� ������
   for(;;)                                                   //�������� ����� ������ �� ������� �����
   {  TV_ITEM item;
      item.mask = TVIF_PARAM | TVIF_TEXT;
      item.pszText = nName;
      item.cchTextMax = sizeof(nName);
      item.hItem = ind;
      if(TreeView_GetItem(hwndTree, &item) == false)
         return Error1((Lan+33)->msg);                       //"������ ��� ������� ���������� �� �������� ������."
      if(item.lParam == n &&
         (aTree + item.lParam)->pf.type == 47)               //��������� ��� ��� ��������� �����
      {  if((aTree + item.lParam)->prSel != 2)
            return Error1((Lan+81)->msg);                    //81, "������������ ������ ������ �� �������."
//??     TreeView_EnsureVisible(hwndTree, ind);              //�������� ������� ������ ����� ��� ��� ����� ���������
         item.mask = TVIF_STATE | TVIF_TEXT;
         item.state = 0;
         item.stateMask = TVIS_BOLD;
         *nName = ' ';
         if(TreeView_SetItem(hwndTree, &item) == -1)
            Error1((Lan+151)->msg);
         (aTree + n)->prSel = 0;                             //�������, ���� ����������
         return 1;
      }

      if((aTree + item.lParam)->pf.type == 48 ||             //��������� ��� ��� ��������� �����
         (aTree + item.lParam)->pf.type == 47)               //��������� ��� ��� ��������� �����
      {  int ret = Remove_Selection(ind, n);                 //����� ��������� ����� � ������
         if(ret  < 0) return -1;
         if(ret == 1) return 1;                              //����� ������ ���� � ����� ���������
      }
      else                                                   //��������� ��� ��� ����
      {  if(n == item.lParam)                                //����� ������� ��� �������������� �����
         {  if((aTree + item.lParam)->prSel != 2)
               return Error1((Lan+81)->msg);                 //81, "������������ ������ ������ �� �������."
//??        TreeView_EnsureVisible(hwndTree, ind);           //�������� ������� ������ ����� ��� ��� ����� ���������
            item.mask = TVIF_STATE | TVIF_TEXT;
            item.state = 0;
            item.stateMask = TVIS_BOLD;
            *nName = ' ';
            if(TreeView_SetItem(hwndTree, &item) == -1)
              Error1((Lan+151)->msg);
            (aTree + n)->prSel = 0;                          //�������, ���� ����������
            num_Sel--;                                       //����� ��������� ������
            size_Sel -= (aTree + n)->pf.SizeF;
            return 1;
         }
      }
      ind = TreeView_GetNextSibling(hwndTree, ind);          //��������� ������ �� ��� �� ������ ������
      if(ind == NULL) break;                                 //������ ������� ������� ������ ���
   }
   return 0;                                                 //������ �� ����� � �� �����
}

//------------------------------------------------------------------------------

int RemoveSelectionFromCopy(int n)                           //������ ��������� ����� � ������ ��� �����������
{
   (aTree + n)->prSel = 2;                                   //�������, ���� ����������
   HTREEITEM hitem = TreeView_GetRoot(hwndTree);
   if(hitem == NULL) return -1;
   int ret = Remove_Selection(hitem, n);                     //����� ��������� ����� � ������
   if(ret < 0) return -1;
   if(ret == 0) return -1;
   ViewNumSel();                                             //����� ����� ���������
   return 0;
}

//------------------------------------------------------------------------------

static int ChangeSelectFolder(HTREEITEM h_Item, DWORD indT, WORD prSel)  //��������� ������ ������
{
   TV_ITEM tvi;
   char nName[300];

   HTREEITEM hitem = TreeView_GetChild(hwndTree, h_Item);    //����� �� ���������� ������� ������
   if(hitem == NULL) return Error1((Lan+33)->msg);           //return Error1("������ ��� ������� ���������� �� �������� ������.");
   for(;;)
   {  tvi.mask = TVIF_PARAM | TVIF_TEXT;
      tvi.pszText = nName;
      tvi.cchTextMax = sizeof(nName);
      tvi.hItem = hitem;
      if(TreeView_GetItem(hwndTree, &tvi) == FALSE)
        return Error1((Lan+33)->msg);                        //return Error1("������ ��� ������� ���������� �� �������� ������.");
      tvi.mask = TVIF_STATE | TVIF_TEXT;
      if((aTree + tvi.lParam)->prSel != prSel)               //����� �� ���������
      {  if((aTree + tvi.lParam)->prSel == 0)                //������ ���� �� �������
         {  tvi.state = TVIS_BOLD;// | TVIS_DROPHILITED;
            (aTree + tvi.lParam)->prSel = 1;                 //������ ���� �������
            *nName = '+';
            num_Sel++;                                       //����� ��������� ���� ��� ��������� ������
            size_Sel += (aTree + tvi.lParam)->pf.SizeF;
            (aTree + indT)->numCopy++;                       //������� ������������� ������
         }
         else
         {  tvi.state = 0;
            (aTree + tvi.lParam)->prSel = 0;                 //������ ���� �� �������
            *nName = ' ';
            num_Sel--;                                       //����� ��������� ���� ��� ��������� ������
            size_Sel -= (aTree + tvi.lParam)->pf.SizeF;
            (aTree + indT)->numCopy--;                       //������� ������������� ������
         }
         tvi.stateMask = TVIS_BOLD | TVIS_SELECTED;// | TVIS_DROPHILITED;
         if(TreeView_SetItem(hwndTree, &tvi) == -1)
           return  Error1((Lan+151)->msg);
      }
      hitem = TreeView_GetNextSibling(hwndTree, hitem);      //��������� ������ �� ��� �� ������ ������
      if(hitem == NULL) break;                               //������ ������� ������� ������ ���
   }
   return 0;
}

//------------------------------------------------------------------------------

static bool ChangeOneSelect(TV_ITEM *tvi, char *nName)       //��������� ������ ������ �����
{
   tvi->mask = TVIF_STATE | TVIF_TEXT;
   if((aTree + tvi->lParam)->prSel == 0)                     //������ ���� �� �������
   {  tvi->state = TVIS_BOLD;// | TVIS_DROPHILITED;
      (aTree + tvi->lParam)->prSel = 1;                      //������ ���� �������
      *nName = '+';
      if((aTree + tvi->lParam)->pf.type != 47)               //��� ����� ����������������� Title
      {  num_Sel++;                                          //����� ��������� ���� ��� ��������� ������
         size_Sel += (aTree + tvi->lParam)->pf.SizeF;
      }
   }
   else
   {  tvi->state = 0;
      (aTree + tvi->lParam)->prSel = 0;                      //������ ���� �� �������
      *nName = ' ';
      if((aTree + tvi->lParam)->pf.type != 47)               //��� ����� ����������������� Title
      {  num_Sel--;                                          //����� ��������� ���� ��� ��������� ������
         size_Sel -= (aTree + tvi->lParam)->pf.SizeF;
      }
   }
   tvi->stateMask = TVIS_BOLD | TVIS_SELECTED;// | TVIS_DROPHILITED;
   if(TreeView_SetItem(hwndTree, tvi) == -1)
   {  Error1((Lan+151)->msg);
      return false;
   }
   if((aTree + tvi->lParam)->pf.type == 47)                  //��� ����� ����������������� Title
      ChangeSelectFolder(tvi->hItem, tvi->lParam, (aTree + tvi->lParam)->prSel);//��������� ������ ������
   ViewNumSel();                                             //����� ����� ���������
   StateButtonClear();                                       //��������� ��������� ������ "��������"
   return true;
}

//------------------------------------------------------------------------------

static int Mark_Selected(void)                               //��������� ���������� ������ ������
{
   HTREEITEM ind;
   char nName[300];
   TV_ITEM tvi;
   ZeroMemory(&tvi, sizeof(tvi));
   ind = TreeView_GetChild(hwndTree, item_Par1);             //������ �������� ������� ������������ �����
   if(ind == NULL) return Error1((Lan+33)->msg);             //return Error1("������ ��� ������� ���������� �� �������� ������.");
   for(int i=0; i<5000; i++)                                 //����� ������ ������� �� ����
   {  if(ind == item_Sel[0]) break;
      if(ind == item_Sel[1])
      {  item_Sel[1] = item_Sel[0];
         break;
      }
      ind = TreeView_GetNextSibling(hwndTree, ind);          //��������� ������ �� ��� �� ������ ������
      if(ind == NULL) return Error1((Lan+33)->msg);          //return Error1("������ ��� ������� ���������� �� �������� ������.");
   }
   for(;;)                                                   //�������� ����� ������ �� ������� �����
   {  ind = TreeView_GetNextSibling(hwndTree, ind);          //��������� ������ �� ��� �� ������ ������
      if(ind == NULL) return Error1((Lan+33)->msg);          //return Error1("������ ��� ������� ���������� �� �������� ������.");
      if(ind == item_Sel[1]) break;                          //������� ������ �������
      tvi.mask = TVIF_PARAM | TVIF_TEXT;
      tvi.hItem = ind;
      tvi.pszText = nName;
      tvi.cchTextMax = sizeof(nName);
      if(TreeView_GetItem(hwndTree, &tvi) == false)
        return Error1((Lan+33)->msg);                        //return Error1("������ ��� ������� ���������� �� �������� ������.");
      if(ChangeOneSelect(&tvi, nName) == false) return -1;   //��������� ������ ������ �����
   }
   return 0;
}

//------------------------------------------------------------------------------

static void ViewShift(void)
{
   if(pr_tviSt != 0)                                         //E��� ������ �����
   {  char Ss[300];
      sprintf(Ss, "  %s  ", (Lan+148)->msg);
      SetWindowText(hPrShift, Ss);
   }
   else
      SetWindowText(hPrShift, "");
}

//------------------------------------------------------------------------------

static bool Select_Group(void)                               //��������� ��������� ���� ������ ������ ����� ��������
{
   Mark_Selected();
   pr_tviSt = 0;                                             //������� ���������� ������
   item_Sel[0] = item_Sel[1] = item_Par1 = NULL;
   ViewShift();
   return true;
}

//------------------------------------------------------------------------------

bool ChangeSelect(int sShift)                                //��������� ������ ������ � ������
{
   char nName[300];
   TV_ITEM tvi;
// ZeroMemory(&tvi, sizeof(tvi));
   HTREEITEM Selected = TreeView_GetSelection(hwndTree);
   if(Selected == NULL) return false;
   tvi.mask = TVIF_PARAM | TVIF_TEXT;
   tvi.hItem = Selected;
   tvi.pszText = nName;
   tvi.cchTextMax = sizeof(nName);
   if(TreeView_GetItem(hwndTree, &tvi) == false) return false;
   if((aTree + tvi.lParam)->pf.type == 48) return false;     //��� ����� ��������� ���
   if(sShift != 0 && pr_tviSt != 0)                          //������� Shift ������ � ���� ������ �����
   {  HTREEITEM item_Par2 = TreeView_GetParent(hwndTree, Selected);  //��������� ������������ �����
      if(item_Par1 != item_Par2)                             //��������� ������������ �����. ��� ������ ���� �����������
      {  Error2((Lan+149)->msg, (Lan+150)->msg);
         return true;
      }
      if(item_Sel[0] == Selected)                            //������ ������ ���� �������
      {  Error2((Lan+149)->msg, (Lan+153)->msg);
         return true;
      }
   }
   if(ChangeOneSelect(&tvi, nName) == false) return false;   //��������� ������ ������ �����
   if(sShift == 0)                                           //������� Shift �� ������
   {//if(item_Sel[0] != NULL)
    //  TreeView_SetInsertMark(hwndTree, item_Sel[0], false);//����� ������� ������
      pr_tviSt = 0;                                          //������� ���������� ������
      item_Sel[0] = item_Sel[1] = item_Par1 = NULL;
      ViewShift();
      return true;
   }
   if(pr_tviSt == 0)                                         //��� �� ���������� ������ ���������������� ������
   {  pr_tviSt = 1;                                          //������� ���������� ������
      item_Sel[0] = Selected;
      item_Par1 = TreeView_GetParent(hwndTree, Selected);
//    TreeView_SetInsertMark(hwndTree, item_Sel[0], true);   //����� ������� ������
      ViewShift();
      return true;
   }
   item_Sel[1] = Selected;
// TreeView_SetInsertMark(hwndTree, item_Sel[0], false);     //����� ������� ������
   return Select_Group();                                    //��������� ��������� ���� ������ ������ ����� ��������
}

//------------------------------------------------------------------------------

bool ChangeSelect_and_Down(void)                             //��������� ������ ������ � ������
{
   if(ChangeSelect(0) == false) return false;                //��������� ������ ������ � ������
   HTREEITEM Selected = TreeView_GetSelection(hwndTree);     //������� �����
   if(Selected == NULL) return false;
   HTREEITEM newSelected = TreeView_GetNextSibling(hwndTree, Selected); //��������� ������ � ������
   if(newSelected == NULL) return false;                     //������ ��� ����� �� ������ ������
   return TreeView_SelectItem(hwndTree, newSelected);        //�������� ��������� �����
}

//------------------------------------------------------------------------------

void ViewSize(void)                                          //������ ��������� ������������
{
   char Ss[300];
   if(maxZapFAT1 == 0xFFFFFFFF) return;                     //���� �� ������, ������� ����������
   double maxSize = double(maxZapFAT1 - 1) /1024.0/1024.0/1024.0 * sCl_B;
   double usedSize = double(writeCl) /1024.0/1024.0/1024.0 * sCl_B;
   double freeSize = double(maxZapFAT1 - 1 - writeCl) /1024.0/1024.0/1024.0 * sCl_B;
   sprintf(Ss, "%s:%7.2lf Gb     %s:%7.2lf Gb (%.1lf%c)     %s:%7.2lf Gb (%.1lf%c)",
                (Lan+124)->msg, maxSize,
                (Lan+125)->msg, usedSize, usedSize/maxSize*100.0, '%',
                (Lan+126)->msg, freeSize, freeSize/maxSize*100.0, '%');
   SetWindowText(hSizeHDD, Ss);
}

#endif


