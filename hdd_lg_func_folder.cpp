
//                                       15.03.2009
#ifndef __HDD_LG_FUNC_IO_FOLDER__
#define __HDD_LG_FUNC_IO_FOLDER__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_func_io_folder ===========================
int  Get_Name_Dir(char *msg, char *Name_Dir, int pr);        //������ ����� �����

static char NameRabDir[260];

typedef struct _SHITEMID         // mkid
{   USHORT cb;                   // size of identifier, including cb itself
    BYTE   abID[1];              // variable length item identifier
} SHITEMID, * LPSHITEMID;
typedef const SHITEMID  * LPCSHITEMID;
typedef struct _ITEMIDLIST       // idl
{   SHITEMID mkid;               // list of item identifers
} ITEMIDLIST, * LPITEMIDLIST;
typedef const ITEMIDLIST * LPCITEMIDLIST;
typedef int (CALLBACK* BFFCALLBACK)(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
typedef struct _browseinfo
{   HWND hwndOwner;
    LPCITEMIDLIST pidlRoot;
    LPSTR pszDisplayName;
    LPCSTR lpszTitle;
    UINT ulFlags;
    BFFCALLBACK lpfn;
    LPARAM lParam;
    int iImage;
} BROWSEINFO, *PBROWSEINFO, *LPBROWSEINFO;
#define BIF_RETURNONLYFSDIRS   0x0001      // For finding a folder to start document searching
//#define BIF_DONTGOBELOWDOMAIN  0x0002    // For starting the Find Computer
//#define BIF_NEWDIALOGSTYLE     0x0040    // Use the new dialog layout with the ability to resize Caller needs to call OleInitialize() before using this API
#define BIF_EDITBOX            0x0010      // Add an editbox to the dialog
//#define BIF_VALIDATE           0x0020    // insist on valid result (or CANCEL)
#define BIF_NEWDIALOGSTYLE     0x0040      // Use the new dialog layout with the ability to resize
#define BIF_NONEWFOLDERBUTTON  0x0200      // Do not add the "New Folder" button to the dialog.  Only applicable with BIF_NEWDIALOGSTYLE.
SHSTDAPI_(LPITEMIDLIST) SHBrowseForFolder(LPBROWSEINFO lpbi);
SHSTDAPI_(BOOL) SHGetPathFromIDList(LPCITEMIDLIST pidl, LPSTR pszPath);
SHSTDAPI SHGetMalloc(LPMALLOC * ppMalloc);
#define BFFM_INITIALIZED        1
#define BFFM_SETSELECTION      (WM_USER + 102)

//------------------------------------------------------------------------------

#pragma argsused
static int __stdcall BrowseProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
   switch(uMsg)
   {  case BFFM_INITIALIZED: CenterDlg(hwnd, 0);             //������������� ���� ������� � ������� ����
                             SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
                             break;
   }
   return 0;
}

//------------------------------------------------------------------------------

int Get_Name_Dir(char *msg, char *Name_Dir, int pr)          //������ ����� �����
{
//����� �� ��������� ���������� ��� ���������� - ����������� �� ����������� vcl.h ��� ��� ������ VCL-����������:
//#define NO_WIN32_LEAN_AND_MEAN
//#include <vcl.h>
//#include <shlobj.h>
   BROWSEINFO bi;                                            //��������� ��� ������� ������ �����
   char dir[260];                                            //����� ��� ���� � ����� ����� ������� ������ ���� ��� �����

   LPMALLOC lpMalloc;
   if(SHGetMalloc(&lpMalloc) != NO_ERROR) return -1;         //��������� ������ ������������ ������ �������
   bi.hwndOwner = MainWin;
   bi.pidlRoot = NULL;
   bi.pszDisplayName = dir;
   bi.lpszTitle = msg;
   bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
   if(pr == 1)
     bi.ulFlags |= BIF_NONEWFOLDERBUTTON;
// bi.lpfn = NULL;
   bi.lpfn = BrowseProc;
// bi.lParam = 0;
   bi.lParam = LPARAM(NameRabDir);
   bi.iImage = 0;
   ITEMIDLIST *il = SHBrowseForFolder(&bi);                  //������������ ������� ������ �����
   if(il == NULL) return -1;                                 //���� ������ ������ "������"
   SHGetPathFromIDList(il, Name_Dir);                        //����� �� ������ ����� ����� �� ����������� ������� � ������
   lpMalloc->Free(il);                                       //������������ ���� ������
   lpMalloc->Release();
   lstrcpy(NameRabDir, Name_Dir);
   return 0;
}

#endif

