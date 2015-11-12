
//                                    24.01.2009
#ifndef __HDD_LG_COPY__
#define __HDD_LG_COPY__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_copy =====================================
int  CopyFiles(void);                                        //���������� �����������
     DWORD num_Sel;                                          //����� ��������� ���� ��� ��������� ������
     LONGLONG size_Sel;                                      //��������� ������ ��������� ������
     DWORD indF;                                             //������ ����������� �����

static HANDLE outFile;
static DWORD numAllInCl;                                     //��������� ����� ��������� �� ���� ���������� ������
static DWORD numAllFi;                                       //����� ���������� ������
static DWORD num_T_Fi;                                       //����� �������� ����������� �����
static DWORD BytesPerClaster;                                //����� ���� � ��������
static DWORDLONG numAllOutCl;                                //��������� ����� �������� ���������

//------------------------------------------------------------------------------

static int Ctrl_OutName(char *outNameF)                      //��������, ��� ����� ����� ����� ����������
{
// char Driv[MAXDRIVE+3], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT+3];
// fnmerge(outNameF, Driv, Dir, Name, Ext);
   int l = lstrlen(outNameF);
   if(l > 255)  //if(l > 259)                                //������� � ��������� �������� �������
     return Error3(outNameF, "", (Lan+95)->msg);             //return Error3(outNameF, "", "����� �������� � ������ ����� ����� ��������� ������ ���  Windows.");
   return 0;
}

//------------------------------------------------------------------------------

//11static int Copy_One_File(PAR_FILE *pf, char *msg, char *fName) //����������� ������ �����
static int Copy_One_File(PAR_FILE *pf, char *msg)            //����������� ������ �����
{
   BYTE buff[sCl_B];                                         //������ ��� ���� �������
   DWORD nb;
#if !defined EMULATOR_HDD_AND_COPY                           //��� ������ ��������� � ��������� �����������
   ZeroMemory(buff, sCl_B);                                  //������� ������ ������
#endif
   DWORD wSizeB = sCl_B;                                     //����� ������������ ���� ����� ����� ���� � ��������
   DWORD nCl = pf->ClSt;                                     //������� ����� �������� ����� ������� �������� �����
   LONGLONG SizeToEnd = pf->SizeF;                           //����� ���� ������� �������� ��������
   DWORD numCl = DWORD((pf->SizeF + sCl_B - 1) / sCl_B);     //����� ��������� ����������� ��� ���������� ����� ������� �������
   if(numCl > 0)
      InitProgressBar(numCl, msg);
   for(DWORD i=0; i<numCl; i++)                              //�� ����� ���������
   {  if(nCl == 0x0FFFFFFF)                                  //������� ����� �������
        return Error1((Lan+12)->msg);                        //return Error1("���������� ������ ������� ����� ������� FAT.");
      if(*(FAT1 + nCl) == 0)                                 //������������ ��������
        return Error1((Lan+11)->msg);                        //return Error1("���������� �������������� �������� FAT � ������ �� ������� �����.");
      if(wSizeB > SizeToEnd)
//11     wSizeB = DWORD(((SizeToEnd + 511) / 512) * 512);    //������ ������� ������ ������ ������� ��������  ?? ������ ������
         wSizeB = DWORD(SizeToEnd);                          //������ ������� ������ ������ ������� ��������
#if !defined EMULATOR_HDD_AND_COPY                           //��� ������ ��������� � ��������� �����������
      DWORD nSector = Start_SecDir1 + (nCl - 1) * sClSec;    //������ ������ �������� ��������
      if(ReadClast1_P(nSector, buff) < 0) return -1;         //������ ��e������� ��������
#endif
      if(WriteFile(outFile, buff, wSizeB, &nb, NULL) == false || nb != wSizeB)
        return ErrorSys1((Lan+83)->msg);                     //return ErrorSys1("������ ��� ������ ��������� �����.");
      SizeToEnd -= wSizeB;                                   //����� ���� ������� �������� ��������
      nCl = *(FAT1 + nCl);                                   //����� ���������� ��������
      if(nCl > maxZapFAT1 && nCl != 0x0FFFFFFF)
        return Error1((Lan+13)->msg);                        //"����� �������� ��������� ���������� ��������."
      if(SizeToEnd == 0 && nCl != 0x0FFFFFFF)
        Error1((Lan+29)->msg);                               //return Error2("�������� ���� ��������� ������� �������,", "� ������� ����� ������� FAT �� ������.");
      if(ProgressBar(wSizeB) < 0) return -1;                 //�������� ����� ������ ��������
   }
   CloseFile(&outFile);                                      //������� �������� ����
/*11
   if((pf->SizeF % sCl_B) != 0)                              //������ ����� �� �������� ������� ������� ��������
 {
   int n = 0;                                                //������� ��� �������� ������������ �����
   if(Open_FileR(fName, &outFile, &n) < 0)                   //�������� ��������� �����
      return -1;
   LARGE_INTEGER TPoz;
   TPoz.QuadPart = pf->SizeF;
   TPoz.u.LowPart = SetFilePointer(outFile, TPoz.u.LowPart, &TPoz.u.HighPart, FILE_BEGIN);
   if(TPoz.u.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
      return ErrorSys1((Lan+83)->msg);                       //"������ ��� ���������������� �� �����."
    if(SetEndOfFile(outFile) == false)
      return ErrorSys1((Lan+83)->msg);                     //return ErrorSys1("������ ��� ������ ��������� �����.");
 }
*/
   FILETIME fTime, flTime;
   SYSTEMTIME sysTime;
   sysTime.wYear = pf->Year;
   sysTime.wMonth = pf->Mon;
// sysTime.wDayOfWeek = 0;
   sysTime.wDay = pf->Day;
   sysTime.wHour = pf->Hour;
   sysTime.wMinute = pf->Min;
   sysTime.wSecond = pf->Sec;
   sysTime.wMilliseconds = 0;
   SystemTimeToFileTime(&sysTime, &flTime);                  //������������� �����
   LocalFileTimeToFileTime(&flTime, &fTime);
   SetFileTime(outFile, &fTime, &fTime, &fTime);             //���� �����
   return 0;
}

//------------------------------------------------------------------------------

static int Copy_File1(DWORD ind)                             //����������� ���������� ����� �� ���� ��
{
   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT];
   char outNameF1[260], NameFF[260];

   fnsplit(outNameF1, Driv, Dir, Name, Ext);
   lstrcpy(NameFF, (aTree+ind)->NameF);
   Ctrl_Name_Ch(NameFF);                                     //�������� ����� �� ������������ �������
   fnmerge(outNameF1, Driv, Dir, NameFF, "");
   indF = ind;                                               //������ ����������� �����
   if(OpenOutFileName_New(outNameF1, (aTree+ind)->pf.SizeF, &outFile) < 0) return -1; //�������� ��������� �����
   UpdateWindow(MainWin);                                    //������������ ���� ����� ������� ������ �����
   InitProgressBar = InitProgressBar1;
   ProgressBar = ProgressBar1;
   Close_ProgressBar = Close_ProgressBar1;
//11   int ret = Copy_One_File(&((aTree+ind)->pf), (Lan+24)->msg, outNameF1);//int ret = Copy_One_File((Tree+ind)->pf.SizeF, (Tree+ind)->pf.ClSt,  "����������� �����");
   int ret = Copy_One_File(&((aTree+ind)->pf), (Lan+24)->msg);               //int ret = Copy_One_File((Tree+ind)->pf.SizeF, (Tree+ind)->pf.ClSt,  "����������� �����");
   Close_ProgressBar();
   CloseFile(&outFile);                                      //������� ������� ����
   if(ret < 0) DeleteFile(outNameF1);
   return ret;
}

//------------------------------------------------------------------------------

static int Copy_Folder_(HTREEITEM hitem, char *Name_Dir, char *NameFo)//����������� ��������� ����� �� ���� ��
{
   HTREEITEM ind = TreeView_GetChild(hwndTree, hitem);       //����� �� ���������� ������� ������
   if(ind == NULL) return 0;
   char nName_Dir[512];                                      //��������� ����
   lstrcpy(nName_Dir, Name_Dir);
   lstrcat(nName_Dir, "\\");
   Ctrl_Name_Ch(NameFo);                                     //�������� ����� �� ������������ ������� � ������ ������������ ��������
   lstrcat(nName_Dir, NameFo);
// int l = lstrlen(nName_Dir);
// if((l+45) >= 256)                                         //����� ����� ����� �� 45 ������
//   if(MsgYesNo4(nName_Dir, "����� ���� � ��������� ����� ������ � ����������.",
//                           "�������� ������.", "���������� ?") == 'N') return -1;
   if(CtrlFileYesNo(nName_Dir) == 0)                         //�������� ������� ����������� ������ ���
      if(CreateDirectory(nName_Dir, NULL) == FALSE)
        return Error2(nName_Dir, (Lan+93)->msg);             //return Error2(nName_Dir, "��������� ������ ��� �������� �����.");
   for(;;)                                                   //�������� ����� ������ �� ������� �����
   {  TV_ITEM item;
      char Msg[100];
      item.mask = TVIF_TEXT | TVIF_PARAM;
      item.hItem = ind;
      item.pszText = Msg;
      item.cchTextMax = 100;
      if(TreeView_GetItem(hwndTree, &item) == FALSE)
        return Error1((Lan+33)->msg);                        //return Error1("������ ��� ������� ���������� �� �������� ������.");
      if((aTree + item.lParam)->pf.type == 48 ||             //��������� ��� ��� �����
         (aTree + item.lParam)->pf.type == 47)               //������ ��� ��� ����������� �����
      {   if(Copy_Folder_(ind, nName_Dir, (aTree + item.lParam)->NameF) < 0) return -1; }//����������� ������ ����� �����
      else                                                   //��������� ��� ��� ����
      {  char outNameF[260], Ss[300], NameFF[260];
         lstrcpy(outNameF, nName_Dir);
         lstrcat(outNameF, "\\");
         lstrcpy(NameFF, (aTree + item.lParam)->NameF);
         Ctrl_Name_Ch(NameFF);                               //�������� ����� �� ������������ ������� � ������ ������������ ��������
         lstrcat(outNameF, NameFF);
         if(Ctrl_OutName(outNameF) < 0) return  -1;          //��������, ��� ����� ����� ����� ����������
         indF = item.lParam;                                 //������ ����������� �����
         int ret = OpenOutFileName_Yes(outNameF, (aTree + item.lParam)->pf.SizeF, &outFile);//�������� ��������� �����
         if(ret == -1) return -1;
         num_T_Fi++;
         sprintf(Ss, "%s:   %d  ( %d )", (Lan+24)->msg, num_T_Fi, numAllFi);
         if(ret == -2)                                       //������ ������ "����������"
         {  ScipProgressBar2((aTree+item.lParam)->pf.SizeF); //��������� ��������� ��� ������������ ������
            ret = 0;
         }
         else
//11       ret = Copy_One_File(&((aTree+item.lParam)->pf), Ss, outNameF);//ret = Copy_One_File(&((Tree+item.lParam)->pf), "����������� �����");
           ret = Copy_One_File(&((aTree+item.lParam)->pf), Ss);          //ret = Copy_One_File(&((Tree+item.lParam)->pf), "����������� �����");
         Close_ProgressBar();
         CloseFile(&outFile);                                //������� ������� ����
         if(ret < 0)
         {  DeleteFile(outNameF);  return -1;  }
      }
      ind = TreeView_GetNextSibling(hwndTree, ind);          //��������� ������ �� ��� �� ������ ������
      if(ind == NULL) break;                                 //������ ������� ������� ������ ���
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Calc_Folder_(HTREEITEM hitem, char *Name_Dir, char *NameFo, int pr)//���������� ��� ����������� ��������� ����� �� ���� ��
{
   HTREEITEM ind = TreeView_GetChild(hwndTree, hitem);       //����� �� ���������� ������� ������
   if(ind == NULL)
     if(pr == 0) return Error1((Lan+92)->msg);               //if(pr == 0) return Error1("� ��������� ����� ��� ������ ��� �����������.");
     else return 0;                                          //��� ��������� ������ ����� ������ �� ������
   char nName_Dir[512];                                      //��������� ����
   lstrcpy(nName_Dir, Name_Dir);
   lstrcat(nName_Dir, "\\");
   lstrcat(nName_Dir, NameFo);
   for(;;)                                                   //�������� ����� ������ �� ������� �����
   {  TV_ITEM item;
      char Msg[100];
      item.mask = TVIF_TEXT | TVIF_PARAM;
      item.hItem = ind;
      item.pszText = Msg;
      item.cchTextMax = 100;
      if(TreeView_GetItem(hwndTree, &item) == FALSE)
         return Error1((Lan+33)->msg);                       //return Error1("������ ��� ������� ���������� �� �������� ������.");
      if((aTree + item.lParam)->pf.type == 48 ||             //��������� ��� ��� �����
         (aTree + item.lParam)->pf.type == 47)               //������ ��� ��� ����������� �����
      {  if(Calc_Folder_(ind, nName_Dir, (aTree + item.lParam)->NameF, ++pr) < 0) return -1; }//���������� ��� ����������� ��������� ����� �� ���� ��
      else                                                   //��������� ��� ��� ����
      {  numAllInCl += DWORD(((aTree + item.lParam)->pf.SizeF + sCl_B - 1) / sCl_B);  //��������� ����� ��������� �� ���� ���������� ������
         numAllOutCl += ((aTree + item.lParam)->pf.SizeF + BytesPerClaster - 1) / BytesPerClaster; //��������� ����� �������� ���������
         numAllFi++;                                         //����� ���������� ������
      }
      ind = TreeView_GetNextSibling(hwndTree, ind);          //��������� ������ �� ��� �� ������ ������
      if(ind == NULL) break;                                 //������ ������� ������� ������ ���
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Copy_Folder(HTREEITEM hitem, char *NameFo)        //����������� ��������� ����� �� ���� ��
{
   char Name_Dir[260];
   if(Get_Name_Dir((Lan+94)->msg, Name_Dir, 0) < 0) return -1;//������ ����� ����� //if(Get_Name_Dir("������� ���� ��� ����� ��� ������", Name_Dir) < 0) return -1; //������ ����� �����
   UpdateWindow(MainWin);                                    //������������ ���� ����� ������� ������ �����
   InitProgressBar = InitProgressBar1_2;                     //������� ��������� ��� ������ ����� �� ������
   ProgressBar = ProgressBar2;
   Close_ProgressBar = Close_ProgressBar1_2;

   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT], Path[MAXPATH], Ss[512];
   fnsplit(Name_Dir, Driv, Dir, Name, Ext);
   lstrcpy(Path, Driv);
   lstrcat(Path, "\\");
   DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
   if(GetDiskFreeSpace(Path, &SectorsPerCluster, &BytesPerSector,
                    &NumberOfFreeClusters, &TotalNumberOfClusters) == 0)
      return ErrorSys1((Lan+46)->msg);                       //return ErrorSys1("��������� ������ ��� ������� ������� �����.");
   BytesPerClaster = SectorsPerCluster * BytesPerSector;     //����� ���� � ��������
   numAllInCl = 0;                                           //��������� ����� ��������� �� ���� ���������� ������
   numAllFi = 0;                                             //����� ���������� ������
   num_T_Fi = 0;                                             //����� �������� ����������� �����
   numAllOutCl = 0;                                          //��������� ����� �������� ���������
   int l = lstrlen(Name_Dir) - 1;
   if(*(Name_Dir + l) == '\\') *(Name_Dir + l) = 0;          //��� ����� ����� ����� ������ ����
   if(Calc_Folder_(hitem, Name_Dir, NameFo, 0) < 0) return -1;//���������� ��� ����������� ��������� ����� �� ���� ��
   DWORDLONG allSize = numAllOutCl * BytesPerClaster;
   int ret = DiskFreeSpaceEx_F(Name_Dir, allSize);           //�������� ���������� �����
   if(ret < 0) return -1;
   if(ret == 1) return Copy_Folder(hitem, NameFo);           //����������� ��������� ����� �� ���� �� (��������� ������ ��� ������� ������ ����)
   sprintf(Ss, "%s:  %s", (Lan+91)->msg, NameFo);
   InitProgressBar2_2(numAllInCl, Ss);                       //������� ��������� ��� ���������� ������
   ret = Copy_Folder_(hitem, Name_Dir, NameFo);              //����������� ��������� ����� �� ���� ��
   Close_ProgressBar2_2();
   return ret;
}

//------------------------------------------------------------------------------

static int Copy_AllFiles(char *Name_Dir)                     //����������� ��������� ������ �� ���� ��
{
   char outNameF[512], Ss[300], NameFF[260];
   for(int i=0; i<numEl_Tree; i++)                           //�� ����� ���������� � ������ ����
   {  if((aTree + i)->prSel == 0) continue;                  //������ ���� �� ������
      if((aTree + i)->pf.type == 47) continue;               //������ ��� ��� ����������� �����
      lstrcpy(outNameF, Name_Dir);
      lstrcat(outNameF, "\\");
      if((aTree + i)->indFolder != 0)                        //������ �� ��� �����
      {  lstrcpy(NameFF, (aTree + (aTree + i)->indFolder)->NameF);
         Ctrl_Name_Ch(NameFF);                               //�������� ����� �� ������������ ������� � ������ ������������ ��������
         lstrcat(outNameF, NameFF);
         if(CtrlFileYesNo(outNameF) == 0)                    //�������� ������� ����������� ������ ���
           if(CreateDirectory(outNameF, NULL) == FALSE)
              return Error2(outNameF, (Lan+93)->msg);        //return Error2(nName_Dir, "��������� ������ ��� �������� �����.");
         lstrcat(outNameF, "\\");
      }
      lstrcpy(NameFF, (aTree + i)->NameF);
      Ctrl_Name_Ch(NameFF);                                  //�������� ����� �� ������������ ������� � ������ ������������ ��������
      lstrcat(outNameF, NameFF);
      if(Ctrl_OutName(outNameF) < 0) return -1;              //��������, ��� ����� ����� ����� ����������
      indF = i;                                              //������ ����������� �����
      int ret = OpenOutFileName_Yes(outNameF, (aTree + i)->pf.SizeF, &outFile);//�������� ��������� �����
      if(ret == -1) return -1;
      num_T_Fi++;
      sprintf(Ss, "%s:   %d  ( %d )", (Lan+24)->msg, num_T_Fi, numAllFi);//"����������� �����"
      if(ret == -2)                                          //������ ������ "����������"
      {  ScipProgressBar2((aTree+i)->pf.SizeF);              //��������� ��������� ��� ������������ ������
         ret = 0;
      }
      else
//11    ret = Copy_One_File(&((aTree+i)->pf), Ss, outNameF);
        ret = Copy_One_File(&((aTree+i)->pf), Ss);
      Close_ProgressBar();
      CloseFile(&outFile);                                   //������� ������� ����
      if(ret < 0)
      {  DeleteFile(outNameF);  return -1;  }
      if((aTree + i)->indFolder != 0)                        //������ �� ��� �����
         if(--((aTree + (aTree + i)->indFolder)->numCopy) == 0)   //��������� ������� ������
           RemoveSelectionFromCopy((aTree + i)->indFolder);
      RemoveSelectionFromCopy(i);                            //������ ��������� ����� � ������ ��� �����������
   }
   return 0;
}

//------------------------------------------------------------------------------

static int Calc_SizeAllFile(void)                            //���������� ��� ����������� ��������� ������ �� ���� ��
{
   for(int i=0; i<numEl_Tree; i++)                           //�� ����� ���������� � ������ ����
   {  if((aTree + i)->prSel == 0) continue;                  //������ ���� �� ������
      if((aTree + i)->pf.type == 47) continue;               //������ ��� ��� ����������� �����
      numAllInCl += DWORD(((aTree + i)->pf.SizeF + sCl_B - 1) / sCl_B);  //��������� ����� ��������� �� ���� ���������� ������
      numAllOutCl += ((aTree + i)->pf.SizeF + BytesPerClaster - 1) / BytesPerClaster; //��������� ����� �������� ���������
      numAllFi++;                                            //����� ���������� ������
   }
   if(numAllFi == num_Sel) return 0;                         //����� ���������� ������
   return Error1((Lan+81)->msg);                             //81, "������������ ������ ������ �� �������");
}

//------------------------------------------------------------------------------

static int CopySelect(void)                                  //����������� ��������� ������
{
   char Name_Dir[260];
   if(Get_Name_Dir((Lan+94)->msg, Name_Dir, 0) < 0) return -1;//������ ����� ����� // if(Get_Name_Dir("������� ���� ��� ����� ��� ������", Name_Dir) < 0) return -1;  //������ ����� �����
   UpdateWindow(MainWin);                                    //������������ ���� ����� ������� ������ �����
   InitProgressBar = InitProgressBar1_2;                     //������� ��������� ��� ������ ����� �� ������
   ProgressBar = ProgressBar2;
   Close_ProgressBar = Close_ProgressBar1_2;

   char Driv[MAXDRIVE], Dir[MAXPATH], Name[MAXFILE], Ext[MAXEXT], Path[MAXPATH];
   fnsplit(Name_Dir, Driv, Dir, Name, Ext);
   lstrcpy(Path, Driv);
   lstrcat(Path, "\\");
   DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
   if(GetDiskFreeSpace(Path, &SectorsPerCluster, &BytesPerSector,
                       &NumberOfFreeClusters, &TotalNumberOfClusters) == 0)
      return ErrorSys1((Lan+46)->msg);                       //return ErrorSys1("��������� ������ ��� ������� ������� �����.");
   BytesPerClaster = SectorsPerCluster * BytesPerSector;     //����� ���� � ��������
   numAllInCl = 0;                                           //��������� ����� ��������� �� ���� ���������� ������
   numAllFi = 0;                                             //����� ���������� ������
   num_T_Fi = 0;                                             //����� �������� ����������� �����
   numAllOutCl = 0;                                          //��������� ����� �������� ���������
   int l = lstrlen(Name_Dir) - 1;
   if(*(Name_Dir + l) == '\\') *(Name_Dir + l) = 0;          //��� ����� ����� ����� ������ ����
   if(Calc_SizeAllFile() < 0) return -1;                     //���������� ��� ����������� ��������� ������ �� ���� ��
   DWORDLONG allSize = numAllOutCl * BytesPerClaster;
   int ret = DiskFreeSpaceEx_F(Name_Dir, allSize);           //�������� ���������� �����
   if(ret < 0) return -1;
   if(ret == 1) return CopySelect();                         //����������� ��������� ������ �� ���� �� (��������� ������ ��� ������� ������ ����)
   InitProgressBar2_2(numAllInCl, (Lan+122)->msg);           //������� ��������� ��� ���������� ������ // if(Get_Name_Dir("������� ���� ��� ����� ��� ������", Name_Dir) < 0) return -1;  //������ ����� �����
   ret = Copy_AllFiles(Name_Dir);                            //����������� ��������� ������ �� ���� ��
   Close_ProgressBar2_2();
   return ret;
}

//------------------------------------------------------------------------------

int  CopyFiles(void)                                         //���������� �����������
{
   TV_ITEM item;
   char Msg[256];
   prCopy = 0;                                               //������� �������� ��� ��������� �����������
   if(num_Sel > 0) return CopySelect();                      //����� ��������� ���� ��� ��������� ������
   HTREEITEM ind = TreeView_GetSelection(hwndTree);
   if(ind == NULL)
     return Error1((Lan+32)->msg);                           //return Error1("��� ����� ���������� ��� �����������.");
   item.mask = TVIF_TEXT | TVIF_PARAM;
   item.hItem = ind;
   item.pszText = Msg;
   item.cchTextMax = 256;
   if(TreeView_GetItem(hwndTree, &item) == FALSE)            //����� ������� ������� ������
      return Error1((Lan+33)->msg);                          //return Error1("������ ��� ������� ���������� �� �������� ������.");
   if((aTree + item.lParam)->pf.type == 48 ||                //������� ����� ��� �����
      (aTree + item.lParam)->pf.type == 47)                  //������� ����� ��� ����� �������������� Title
      return Copy_Folder(ind, (aTree + item.lParam)->NameF); //����������� ��������� ����� �� ���� ��
   return Copy_File1(item.lParam);                           //����������� ���������� ����� �� ���� ��
}

#endif

