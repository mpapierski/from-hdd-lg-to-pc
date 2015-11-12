
//                                04.11.2009
#ifndef __HDD_LG_WRITE__
#define __HDD_LG_WRITE__

#include "from_hdd_lg_to_pc.h"

#if defined WRITE_YES                                        //����� ������ ��������

//============================ hdd_lg_write ====================================
int  WriteFileToHDD(int pr);                                 //������ ������ �� ����
     int prFolder;                                           //������� ���������� ����� (0-mp3, 1-jpg, 2-avi)
     DWORD ClStDir;                                          //����� �������� �������� � ������� ����������� ������
     TV_ITEM item_Add;                                       //����� � ������ ������ ���� ����� ����������
     int NumMsg38;                                           //����� ��������� � ���������� ����� �����

//------------------------------------------------------------------------------

int  WriteFileToHDD(int pr)                                  //������ ������ �� ���� LG
{
   char Msg0[256], Msg1[256];
   NumMsg38 = 0;                                             //����� ��������� � ���������� ����� �����
   if(num_Sel > 0)                                           //����� ��������� ���� ��� ��������� ������
     return Error1((Lan+173)->msg);                          //"������ ���� ������� ���� ����� ���������� ��� �����������."
   HTREEITEM indSel = TreeView_GetSelection(hwndTree);
   if(indSel == NULL)
     return Error1((Lan+173)->msg);                          //"������ ���� ������� ���� ����� ���������� ��� �����������."
   TV_ITEM itemSel;                                          //����� � ������ ������
   itemSel.mask = TVIF_TEXT | TVIF_PARAM;
   itemSel.hItem = indSel;
   itemSel.pszText = Msg0;
   itemSel.cchTextMax = 256;
   if(TreeView_GetItem(hwndTree, &itemSel) == FALSE)         //����� ������� ������� ������
      return Error1((Lan+33)->msg);                          //"������ ��� ������� ���������� �� �������� ������."
   if(pr < 3 && (aTree + itemSel.lParam)->pf.type != 48)     //��� ������ ���� ������� ������� ����� �.�. ������
      return Error1((Lan+173)->msg);                         //"������ ���� ������� ���� ����� ���������� ��� �����������."
   if(pr > 2 && (aTree + itemSel.lParam)->Level <= 1)        //������ ��������������� � ������� ����������� �����
      return Error1((Lan+189)->msg);                         //"������ �������� ����������� �����."
   TV_ITEM item1 = itemSel;
   if((aTree + itemSel.lParam)->Level > 1)
   {  HTREEITEM ind1 = indSel;
      for(;;)
      {  ind1 = TreeView_GetParent(hwndTree, ind1);          //��������� ������������ �����
         item1.mask = TVIF_TEXT | TVIF_PARAM;
         item1.hItem = ind1;
         item1.pszText = Msg1;
         item1.cchTextMax = 256;
         if(TreeView_GetItem(hwndTree, &item1) == FALSE)     //����� ������� ������� ������
            return Error1((Lan+33)->msg);                    //"������ ��� ������� ���������� �� �������� ������."
         if((aTree + item1.lParam)->Level > 1) continue;
         break;
      }
   }
   prFolder = -1;                                            //������� ���������� ����� (0-mp3, 1-jpg, 2-avi)
   if(lstrcmp(item1.pszText, "JUKEBOX") == 0) prFolder = 0;  //������� ���������� ����� (0-mp3, 1-jpg, 2-avi)
   if(lstrcmp(item1.pszText, "PHOTO") == 0)   prFolder = 1;  //������� ���������� ����� (0-mp3, 1-jpg, 2-avi)
   if(lstrcmp(item1.pszText, "VIDEO") == 0)   prFolder = 2;  //������� ���������� ����� (0-mp3, 1-jpg, 2-avi)
   if(prFolder < 0)
     return Error1((Lan+180)->msg);                          //"��� ����������� ����� �������� ������ �����: JUKEBOX, PHOTO, VIDEO ��� ���������� ��� ���"
   ClStDir = (aTree + itemSel.lParam)->pf.ClSt;              //����� �������� �������� � ������� ����������� ������
   if(pr > 2)                                                //��� �������� � �������������� ����� ������� � ������� ��������� ���
   {  TV_ITEM item = itemSel;
      HTREEITEM ind1 = TreeView_GetParent(hwndTree, itemSel.hItem);//��������� ������������ �����
      item.mask = TVIF_TEXT | TVIF_PARAM;
      item.hItem = ind1;
      item.pszText = Msg1;
      item.cchTextMax = 256;
      if(TreeView_GetItem(hwndTree, &item) == FALSE)         //����� ������� ������� ������
          return Error1((Lan+33)->msg);                      //"������ ��� ������� ���������� �� �������� ������."
      ClStDir = (aTree + item.lParam)->pf.ClSt;              //����� �������� �������� � ������� ����������� ������
   }
   EnableW(false);                                           //��������� ���������� ������
   int ret = 0;
   item_Add = itemSel;
   MaxClast = 0;                                             //����� ������ �������� �������� ��������������� ��� ������
   switch(pr)
   {  case 0: ret = File_ToHDD();   break;                   //������ ������ �� ���� LG
      case 1: ret = Folder_ToHDD(); break;                   //������ ����� �� ���� LG
      case 2: ret = New_Folder_ToHDD(); break;               //�������� ����� �� ����� LG
      case 3: ret = RenameFiFo(indSel, aTree + itemSel.lParam); break; //��������������
      case 4: ret = DeleteFiFo(indSel, aTree + itemSel.lParam); break; //��������
   }
   MyFreeMem(&(void*)c_FAT1);
   MyFreeMem(&(void*)spisF);
   if(ret < 0)                                               //���� ���� ������, �� ������ �����������
   {  Update_Tree();                                         //������������ ������ ������ � ����� ��� ��������� ��������
      ViewSize();                                            //������ ��������� ������������
   }
   else
     if(NumMsg38 != 0)                                       //����� ��������� � ���������� ����� �����
      Message1((Lan+192)->msg);                              //"��������� � �������� ������ ������� � ���� ����� ����������� ���������."
//�������� � ���, ��� ����� ������������ ������ ind0 - ��������������
// TreeView_SelectDropTarget(hwndTree, ind0);
// TreeView_Expand(hwndTree, ind0, TVE_EXPAND);
   EnableW(true);                                            //��������� ���������� ������
   return ret;
}

#endif
#endif
