
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
   extern HINSTANCE MainInst;                                //������������� ����������
   extern HWND  MainWin;                                     //������� ����
   extern HWND  hwndTree;                                    //���� ������
   extern DWORD FonLTGRAY;                                   //������� ����
   extern HBRUSH FonBrush;                                   //������� �����
   extern HFONT MyFont, MyFont_Cur, MyFontSm;
   extern HWND hCopy, hConf, hClose, hClear, hSizeHDD;       //���� ������
   extern HWND hMap, hToTxt, hPrShift;                       //���� ������
   extern HWND hNumSel, hSizeSel, hAbout;                    //���� ������
   extern HWND hWriteFi, hWriteFo, hNew_Fo, hRenFi, hDelFi;  //������ ��� ������
   extern HWND hCorr67;
   extern int prHDD_LG_Yes;                                  //�������, ����������� HDD: 0-����, 1-�� ������
   extern const char *NVER_TXT;
   extern char *AppName;

//============================ config_io =======================================
int  WriteConfig(void);                                      //������ ����� ������������
int  LoadConfig(void);                                       //�������� ������������
void NewConfig(void);                                        //��������� ������������
     extern LANG *Lan;
     extern CONF Conf;                                       //���������

//============================ hdd_lg_copy =====================================
int  CopyFiles(void);                                        //���������� �����������
     extern DWORD num_Sel;                                   //����� ��������� ���� ��� ��������� ������
     extern LONGLONG size_Sel;                               //��������� ������ ��������� ������
     extern DWORD indF;                                      //������ ����������� �����

#if defined TEST_FAT1                                        //�������� FAT1
//============================ hdd_lg_ctrl_fat =====================================
int  Ctrl_FAT1(void);                                        //�������� ��������� ��������� �������
int  Vost_FAT1(void);                                        //������ FAT1 �� ���������� ���������
     extern DWORD *cTestFAT1;                                //����� ������� FAT ������� �������
     extern DWORD sumClTestFAT1;                             //��������� ���������� ������� ��������� ��� ������������ FAT1
#endif

#if defined EMULATOR_HDD
//============================ hdd_lg_emulator =================================
int ReadClastEmul(DWORDLONG Poz, BYTE *buff);                //������ �������� ����������
int ReadClast2Emul(DWORDLONG Poz, BYTE *buff);               //������ �������� ����������
int ReadFATEmul(DWORDLONG Poz);                              //������ FAT ����������
int ReadFAT2Emul(DWORDLONG Poz);                             //������ FAT ����������
int Read_Sec_Emul(int Num, BYTE* Sec);                       //������ ������� �� ������ ����������
int FindHDD_LG_Emul(void);                                   //����� ����� �� ��������� LG ����������
    extern char *inBuf;
    extern char NameFDump[];                                 //��� ����� � ������
#endif

#if defined FIND_NO_ZERO_SEC
//============================ hdd_lg_find_zero ================================
int FindNoZeroSec(void);                                     //����� �� ������� �������� � �������� ������
#endif

//============================ hdd_lg_func =====================================
int  CreateMyFont(void);                                     //�������� �����
char *Char_Dig_p(char *CharDig, int N);                      //�������������� ����������� ����� � ��������
char *Char_Dig_p_n(char *CharDig, int N, int n);             //�������������� ����������� ����� � �������� � "n" ������� ����� �������
char *FileSize_Txt(DWORDLONG Size);                          //������ ����� � �����
void CenterDlg(HWND hDlg, int yPoz);                         //������������� ���� ������� � ������� ����
void OutNameDlg(HWND hDlg, int ID_T, char *inText);          //�������� �����
void UnicodeToAnsi(WCHAR *inUNI, char *outANSI, int maxN);   //�������������� Unicode � �������
void Open_Wait_Found_HDD(void);                              //������ �� ������� ���������
void Close_Wait_Found_HDD(void);                             //������ �� ������� ���������
int  Ctrl_Adm(void);                                         //�������� ������� ���� ��������������
void About(void);
void AddToSpecSpis_VerWin(void);                            //������ � ���� ������ Windows

//============================ hdd_lg_func_1 ===================================
void *MyAllocMem(DWORD Size);                                //������ ������������ ������
void MyFreeMem(void **Adr);                                  //������������ ����� ������
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
int  Get_Name_Dir(char *msg, char *Name_Dir, int pr);        //������ ����� �����

//============================ hdd_lg_func_io =====================================
void ClearDopDlg(void);                                      //������� ���������� ��������������� �������
int  SetInFilePointer(LONGLONG Poz);                         //��������� ������� ��������� � ����� (�� ���������� �����)
void CloseFile(HANDLE *File);                                //������� ���� (���������� ����)
int  GetName_OutFile(char *NameF);                           //���� ����� ��� ������ �����
int  CtrlFileYesNo(char *NameF);                             //�������� ������� �����
int  DiskFreeSpaceEx_F(char *Name_Dir, DWORDLONG Size);      //�������� ���������� �����
int  Open_OutFileAlw(char *Name, HANDLE *File);              //�������� ��������� �����
//int  Open_FileR(char *Name, HANDLE *File, int *nn);        //�������� ������������� �����
//11int  Open_OutFile512(char *Name, HANDLE *File);          //�������� ��������� �����
int  Open_OutFile(char *Name, HANDLE *File);                 //�������� ��������� �����
int  DiskFreeSpaceEx(char *NameF, DWORDLONG Size);           //�������� ���������� �����
     extern ModifDialog ModDlg;                              //��� ����������� �������
     extern WIN32_FIND_DATA DataF;                           //��������� ����� ���� �� ����

//============================ hdd_lg_func_io_1 ================================
//11int  OpenOutFile(char *Name, LONGLONG Size, HANDLE *File);    //�������� ��������� �����
void Ctrl_Name_Ch(char *Name);                               //�������� ����� �� ������������ ������� � ������ ������������ ��������
int  OpenOutFileName_New(char *Name, LONGLONG Size, HANDLE *File);//�������� ��������� �����
int  OpenOutFileName_Yes(char *Name, LONGLONG Size, HANDLE *File);//�������� ��������� �����
     extern int prCopy;                                      //������� �������� ��� ��������� �����������
     extern char ndS[9];                                     //�������� ������������ ��������
   //extern int  pr_ndS;                                     //������� ����������� ������ ������������� �������. 1-���� ������

//============================ hdd_lg_language =================================
  extern LANG Lan_EN[];
  extern LANG Lan_RU[];
  extern const int numStrLan;                                //����� �������� �����
  extern LANG Lan_Any[];                                     //������������ ����

//============================ hdd_lg_make_tree ================================
#if defined WRITE_YES                                        //����� ������ ��������
int AddItemToTree_forWrite(char *Name, char *Ext, PAR_FILE *pf);   //���������� ����� ������ � ������ ��� ������
#endif
int  AddItemToTree(char *Name, char *Ext, PAR_FILE *pf, int Level);//���������� ����� ������ � ������
void Expand_Tree(int hitem);                                 //������������ ������
int  ClearSelect(void);                                      //������ ���� ���������
int  RemoveSelectionFromCopy(int n);                         //������ ��������� ����� � ������ ��� �����������
bool ChangeSelect(int sShift);                               //��������� ������ ������ � ������
bool ChangeSelect_and_Down(void);                            //��������� ������ ������ � ������
void ViewSize(void);                                         //����� ��������� ������������
     extern int numEl_Tree;                                  //����� ���������� � ������ ����
     extern OneStrTree *aTree;                               //���������� �� ������
     extern HTREEITEM hPrev[MAX_U];                          //������ ������� ����������� ������

//============================ hdd_lg_map_hdd ==================================
int  View_Map_HDD(void);                                     //����� ��������� HDD
BOOL Register_MapHDDWin(void);                               //����������� ����

//============================ hdd_lg_mk_name ==================================
int  MakeOneStrForTree(char *oneStr, char *Name, char *Ext, PAR_FILE *pf);//�������� ������ �������� ��� ��������� � ������
int  ViewAbsendName(void);                                   //����� ���� �������������� � ����� MME.DB, �� ������������� � ��������

//============================ hdd_lg_mme_db ===================================
int  CtrlSize_MME(DWORD Delta);                              //�������� �������� � ������� MME
int  CtrlRecoder(void);                                      //������������� ���������
int  Read_Dir_Part2_MME(void);                               //������ �������� ������� ������� � ����� MME
int  ReadMME_DB(void);                                       //������ ����� ���� �������
int  Update_MME_DB(void);                                    //������������ ������� ������������� ����
     extern BYTE *MMe;                                       //���������� ����� ����
     extern DWORD SizeMME;                                   //����� ���� � ����� MME
     extern TABL_MME *tabMME;                                //������� ������������ ����
     extern DWORD numNam;                                    //����� ���� � ������� MME
     extern TABL_MME *tabMME_Part;                           //������� ���� ����������������� Title
     extern DWORD numNam_Part;                               //����� ���� � ������� ���� ����������������� Title

#if defined OUT_TEST
//============================ hdd_lg_out_test =====================================
void Add_SpecSpis(char *Str);                                //���������� ������ � ������
void View_HEX_Any(BYTE *buff, DWORD Size);                   //����� ���������������� �����
HWND Create_SpecWin(void);                                   //�������� ����
BOOL Register_SpecWin(void);                                 //����������� ����
#endif

//============================ hdd_lg_read =====================================
int  Update_Tree(void);                                      //������������ ������ ������ � ����� ��� ��������� ��������
int  Read_HDD(void);                                         //����� HDD �� ��������� LG � �������� ��������� ����������
     extern HANDLE hDrive;                                   //��������� �� ����
     extern DWORD *FAT1;                                     //������� FAT ������� �������
     extern DWORD Start_SecDir1;                             //������� ������ ������� �������� ������� �������
     extern DWORD Start_SecFAT1;                             //������� ������ ������� FAT ������� �������
     extern DWORD num_SecFAT1;                               //����� �������� � FAT
     extern DWORD Size_FAT1;                                 //����� ���� � FAT
     extern DWORD maxZapFAT1;                                //������������ ����� ������� � FAT
     extern DWORD *FAT2;                                     //������� FAT ������� �������
     extern DWORD Start_SecDir2;                             //������� ������ ������� �������� ������� �������
     extern DWORD Start_SecFAT2;                             //������� ������ ������� FAT ������� �������
     extern DWORD num_SecFAT2;                               //����� �������� � FAT ������� �������
     extern DWORD Size_FAT2;                                 //����� ���� � FAT ������� �������
     extern DWORD maxZapFAT2;                                //������������ ����� ������� � FAT ������� �������
     extern int indTabMME;                                   //������ � ������� MME.DB ��� �������� ����� � ����� MEDIA
     extern DWORD writeCl;                                   //����� ������� ���������
     extern int prMEDIA;                                     //������� �������������� �����
     extern int pr_tRec;                                     //������� ��������� 0 ��� 1 (��������� �����)

//============================ hdd_lg_read_io ==================================
int  ReadSec(BYTE *buff);                                    //������ �������
int  ReadClast1(BYTE *buff);                                 //������ �������� ������� ������� ������� � �������� �������
int  ReadClast1_P(DWORD nSector, BYTE *buff);                //������ �������� ������� ������� ������� � ���������� �������
int  ReadClast2(BYTE *buff);                                 //������ �������� ������� ������� ������� � �������� �������
int  ReadClast2_P(DWORD nSector, BYTE *buff);                //������ �������� ������� ������� ������� � ���������� �������
int  Load_FAT1(void);                                        //�������� FAT ������� �������
int  Load_FAT2(void);                                        //�������� FAT ������� �������
int  FindHDD_LG(void);                                       //����� ����� �� ��������� LG
     extern DWORD stCl_Media2;                               //������� ������ ������ ����� ����� MEDIA

//============================ hdd_lg_remake_tree ==============================
int  RemakeTree(void);                                       //���������������� ������ � ������ ������������� Titles

#if defined TEST_67_68_70_71                                 //�������
//============================ hdd_lg_sect_68_68_70_71 =========================
int  Read_Sec_67_68_70_71(void);                             //������ ����������� ��������
#endif

#if defined TEST_IDX
//============================ hdd_lg_test_idx =================================
int Read_File_idx(void);
#endif

#if defined TEST_WIN
//============================ hdd_lg_test0 ====================================
void View_Sec_Hex(BYTE *buff, int pr);                       //����� ����������� �������
void Add_Spis(char *Str);                                    //���������� ������ � ������
HWND Create_TestWin(void);                                   //�������� ����
BOOL Register_TestWin(void);                                 //����������� ����
#endif

#if defined TEST_LG1
//============================ hdd_lg_test1 ====================================
int  Read_Test1(void);                                       //�������� ������ � �����
#endif

#if defined TEST_LG2_MME
//============================ hdd_lg_test2 ====================================
int  Read_Test2(void);                                       //�������� ������ � �����
#endif

//============================ hdd_lg_to_txt =====================================
int OutTxt(void);                                            //������ ������ � ��������� ����

#if defined WRITE_YES                                        //����� ������ ��������
//============================ hdd_lg_write ====================================
int  WriteFileToHDD(int pr);                                 //������ ������ �� ����
     extern int prFolder;                                    //������� ���������� ����� (0-mp3, 1-jpg, 2-avi)
     extern DWORD ClStDir;                                   //����� �������� �������� � ������� ����������� ������
     extern TV_ITEM item_Add;                                //����� � ������ ������ ���� ����� ����������
     extern int NumMsg38;                                    //����� ��������� � ���������� ����� �����

//============================ hdd_lg_write_1 ==================================
int  File_ToHDD(void);                                       //������ ������ �� ���� LG
int  Folder_ToHDD(void);                                     //������ ����� �� ���� LG
int  New_Folder_ToHDD(void);                                 //C������� ����� �� ����� LG
     extern char *spisF;                                     //������ ������ ������

//============================ hdd_lg_write_2 ==================================
BOOL CALLBACK Dlg_NewName(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int  RenameFiFo(HTREEITEM hitem, OneStrTree *aTree);         //��������������
int  DeleteFiFo(HTREEITEM hitem, OneStrTree *aTree);         //��������
int  Change_Sec67(int num_Cl);                               //�������� ��������� � ���������� ������� 67
int  Correct67Sec(void);                                     //��������� ����������� 67 �������
     extern char NameFoFi[];                                 //��� ����������� �����
     extern DWORD MaxClast;                                  //����� ������ �������� �������� ��������������� ��� ������

//============================ hdd_lg_write_io =================================
int  GetSpNameInFile(char *Spis, int N, int *sm);            //���� ������ ���� ��� ������
int  OpenInFilePC(char *NameF);                              //������� ������� ���� ��� ������ � ����������
int  WriteClast1P(DWORD nClast, BYTE *buff);                 //������ ��������
int  Save_FAT1(void);                                        //���������� ����������� FAT ������� �������
void Ansi_To_Unicode(char *inANSI, WCHAR *outUNI, int maxN); //�������������� ������ �������� � Unicode
int  Load_Dir(void);                                         //�������� �������� ��������
int  Change_Dir_For_File(char *NameF);                       //��������� �������� (�������� ������ �����)
int  Change_Dir_For_Folder(char *NameF);                     //��������� �������� (�������� ����� �����)
int  Save_Dir(void);                                         //���������� �������� ��������
    extern DWORD *c_FAT1;                                    //����� ������� FAT ������� �������
    extern HANDLE inFile;
    extern ULARGE_INTEGER Size_inF;                          //������ ����� �.�. > 4 ��
    extern int nCl_1;                                        //����� �������� ������ �����
    extern BYTE bufDir[];                                    //������ ��� ���� �������
#endif

//============================ ProgressBar =====================================
void EnableW(bool T_F);                                      //��������� ���������� ������
void InitProgressBar1(DWORD Size, char *Msg);
int  ProgressBar1(DWORD Size);
void Close_ProgressBar1(void);
void InitProgressBar1_2(DWORD Size, char *Msg);              //������� ��������� ��� ������ �����
void InitProgressBar2_2(DWORD Size, char *Msg);              //������� ��������� ��� ���������� ������
int  ProgressBar2(DWORD Size);
void ScipProgressBar2(DWORDLONG SizeF);                      //��������� ��������� ��� ������������ ������
void Close_ProgressBar1_2(void);
void Close_ProgressBar2_2(void);
     extern void (*InitProgressBar)(DWORD Size, char *Msg);
     extern int  (*ProgressBar)(DWORD Size);
     extern void (*Close_ProgressBar)(void);
     extern bool prProcCopy;                                 //������� �������� �����������
     extern int  prWinMinMax;                                //0-���� �����������, 1-���� ���������
     extern DWORD tStart2;

#endif

