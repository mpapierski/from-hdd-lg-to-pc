
//                                 16.11.2008
#ifndef __HDD_LG_TO_PC_DEF_H__
#define __HDD_LG_TO_PC_DEF_H__

#define WRITE_YES                                            //��������� � ������� ������

//#define VIEW_DEL                                           //����� ��������� ������ � �����

//#define OUT_TEST                                           //������ ��� �������� �����
//#define FIND_NO_ZERO_SEC                                   //����� �� ������� ��������

//#define EMULATOR_HDD_AND_COPY                              //����� ��������� � ��������� �����������
//#define EMULATOR_HDD                                       //����� ���������

//#define TEST_FAT1                                          //�������� FAT1 �� ���������� ��������
//#define TEST_FAT1_VOST                                     //�������� � �������������� FAT1 �� ���������� ��������
//#define TEST_LG1                                           //������ ������ � ��������� ����
//#define TEST_LG1_AND_TS                                    //������ ������ � ��������� ���� � ����� ����� (����� TimeShift ?)
//#define TEST_LG2_MME                                       //������ ������ � ��������� ���� (������ ���� MME.DB)
//#define TEST_LG2_ALL                                       //������ ������ � ��������� ���� (���)
//#define HDD_LG  4                                          //����� ��������, ������������� ����� ������ �� ������� HDD
//#define VIEW_TAB_MME                                       //����� ������� ����������� ������ mme.db (� ������� ������)
//#define TEST_67_68_70_71                                   //����� ���������� �� ��������
//#define TEST_IDX                                           //�������� ����� IDX

#if defined OUT_TEST                                         //������ ��� �������� �����
  #undef TEST_67_68_70_71                                    //�������
#endif

#if defined TEST_FAT1_VOST                                 //�������� � �������������� FAT1 �� ���������� ��������
   #define TEST_FAT1                                         //�������� FAT1 �� ���������� ��������
#endif

#if defined EMULATOR_HDD_AND_COPY                            //����� ��������� � ��������� �����������
  #define EMULATOR_HDD                                       //����� ���������
#endif
#if defined TEST_LG1_AND_TS                                  //������ ������ � ��������� ���� � ����� TimeShift
  #define TEST_LG1                                           //������ ������ � ��������� ����
  #if defined EMULATOR_HDD
    #error TEST_LG1_AND_TS and EMULATOR_HDD
  #endif
  #if defined OUT_TEST
    #error TEST_LG1_AND_TS and OUT_TEST
  #endif
#endif
#if defined TEST_LG2_ALL
  #define TEST_LG2_MME                                       //������ ������ � ��������� ���� (������ ���� MME.DB)
#endif

#if defined EMULATOR_HDD && defined WRITE_YES                //����� ��������� �� �������� � ������
    #error WRITE_YES and EMULATOR_HDD
#endif

#if defined TEST_LG1 || defined TEST_LG2_MME || defined VIEW_TAB_MME || defined TEST_67_68_70_71 || defined TEST_IDX
  #define TEST_WIN
#endif

#if defined WRITE_YES                                        //����� ������ ��������
   #define N_VER_TXT "2.06 beta"                             //����� ������ ���������
#else
   #define N_VER_TXT "1.15"                                  //����� ������ ���������
#endif

#define TEST_N_VER_TXT "10"                                  //����� ������ �����

#define MAX_NAME 16000 //2000+2000+2000+2*4096+�����         //������������ ����� ���� � ������
#define MAX_U 64                                             //������������ ����������� �����
#define MAX_NAME_MME 4096                                    //������������ ����� ���� � ����� MME
#define MAX_NAME_S 4096                                      //������������ ����� ���� � ������
#define MAX_SP (256*MAX_NAME_S)                              //������ ������ ��� ������ ������

typedef struct { WORD type; DWORD ClSt; BYTE Sec, Min, Hour, r, Day, Mon; WORD Year;
                 LONGLONG SizeF, lL1, lL2; } PAR_FILE;       //����� ������ ��������
typedef struct { char NameF[128]; PAR_FILE pf; WORD prSel, Level;
                 WORD numPart, indTabMME, indTitle, indFolder, numCopy; } OneStrTree;
typedef struct { LONGLONG SizeF; DWORD timeLong; char NameF[48], aName[72],
                 Qual[8], nameIn[8]; DATE_T1 dt;
                 WORD prYes, numPart, nPart, indFolder, indTitle; } TABL_MME;
typedef struct { WCHAR Name[41]; WCHAR Ext[4]; PAR_FILE pf; } One_Str_Cat;
typedef struct { BYTE j[3];                                  //E9 00 00 - A jump instruction to skip the record and reach the code to execute.
                 char Met[8];                                //"LGEINC  " - ����������������� ������
                 WORD BytesPerSector;                        //00 02 = 512 - The number of bytes in each sector
                 BYTE r1;                                    //00
                 WORD numRezSec;                             //20 00 = 32 - ����� ����������������� �������� � ������ �������
                 BYTE nf;                                    //01 - The number of copies of the FAT
                 BYTE r2[4];                                 //00 00 00 00
                 BYTE F8;                                    //F8 - Media descriptor byte
                 BYTE r3[10];                                //����
                 DWORD numSecPart;                           //����� ����� �������� � ������� (��������� �� ��������� � ������� �������� - 0 ������)
                 DWORD numSecFAT;                            //����� �������� ��� FAT
                 BYTE r4[12];                                //����
                 DWORD nSecDir;                              //�������� � ������� ��������
                 BYTE r5[15];                                //����
                 char Name[19];                              //"VOLUMELABE FAT32   "
                 BYTE r6[6];                                 //����
                 WORD SecPerCl;                              //����� �������� � ����� ��������
                 BYTE b[414]; } SEC_63;                      //����
typedef struct { DWORD Ident1;
                 DWORD A[120];
                 DWORD Ident2;
                 DWORD FreeCl;                               //����� ��������� ���������
                 DWORD LastCl;                               //��������� ������� �������
                 DWORD B[3];
                 DWORD Ident3; } SEC_67;
typedef struct { BYTE a[440];                                //����
                 DWORD Ident;                                //����������� ������������� HDD
                 BYTE c[2];                                  //����
                 DWORD A1;                                   //(01)�������� ������, (00)�������, (80=128)������, (0B=11)�������
                 DWORD numClP1;                              //����� ��������� � ������ �������
                 DWORD B1;                                   //3F 00 00 00 == 63 - ����� �������� �������������� �������
                 DWORD numSecP1;                             //����� ����� �������� � �������
                 DWORD A2;                                   //(01)�������� ������, (00)�������, (00)������, (0�=15)�������
                 DWORD numClP2;                              //����� ��������� � ������ �������
                 DWORD B2;                                   //3F 00 00 00 == 63 - ����� �������� �������������� �������
                 DWORD numSecP2;                             //����� ����� �������� � �������
                 DWORD C[8];                                 //��� ������ ������ - ����
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
typedef struct { long Ind; char *msg; } LANG;                //��������� ��������� �������
#define IDENT_STR "from_hdd_lg_to_pc.conf"
typedef struct                                               //64 �����
   {  char IdentStr[sizeof(IDENT_STR)];
      BYTE PrSort;                                           //0-���������� �� �����, 1-��� ���������
      BYTE ViewIDX;                                          //0-�� ��������� ����� *.idx, 1-����������
      BYTE Ren_STR;                                          //0-��������������� *.str � *.vro
      BYTE SwapNPart;                                        //0-������������ ����� ����� � ����� �����
      BYTE ChangeNul;                                        //0-�������� ������ '0' �� '_'
      BYTE NoViewOnePart;                                    //0-�� ��������������� ��� ����� �����
      BYTE nLanguage;                                        //0-�������, 1-����������, 2-������� ����
      BYTE altName;                                          //0-������������ ������������� ��� � ����� MEDIA, 1-�� ������������ �������������� ���
      BYTE typeRec;                                          //0-������ �����, 1-����� ����� RH2000
      BYTE typeSize;                                         //0-�����, 1-������, 2-������, 3-������
      BYTE ViewVRO;                                          //0-���������� ���������� *.vro, 1-�� ����������
      BYTE typeTime;                                         //0-���������� ����� ������ ������, 1-����� �������� �����
      BYTE poz_Ql;                                           //�������� ������: 0-� �������, 1-� �����, 2-�� ����������
      BYTE poz_In;                                           //�������� ������: 0-� �������, 1-� �����, 2-�� ����������
      BYTE OutJ_BOX;
      BYTE OutMEDIA;
      BYTE OutPHOTO;
      BYTE OutVIDEO;
      BYTE ViewPart;                                         //������ ������ ������: 0-������, 1-��������
      BYTE WriteYes;                                         //���������� �� ������: 0-���, 1-����
      BYTE rez[21];
   } CONF;

     const DWORD sSecB = 512;                                //������ ������� � ������
     const DWORD sClSec = 1024;                              //����� �������� � ��������
     const DWORD sCl_B = sSecB * sClSec;                     //������ �������� � ������
     const DWORD sClSec2 = 8;                                //����� �������� � �������� �� ������ �������
     const DWORD sCl2_B = sSecB * sClSec2;                   //������ �������� � ������ �� ������ �������
typedef struct                                               //�������� ��������� ����������� �������
  { DWORD         Flags;                                     //�������������� �����
    LPOFNHOOKPROC fnHook;                                    //������� ��������� ��������������� �������
    LPCTSTR       DopDlgName;                                //�������������� ������ ������� ����������� � ��������
  } ModifDialog;

//------------------------------------------------------------------------------
inline void SWAP16(WORD *W)                                  //������������ ���� ����
{
  *W = WORD(((*W & 0x00ffU) << 8) | ((*W & 0xff00U) >> 8));
}
//------------------------------------------------------------------------------
inline void SWAP32(DWORD *DW)                                //������������ ������� ����
{
  *DW = DWORD( ((*DW & 0x000000ffU) << 24) | ((*DW & 0x0000ff00U) <<  8) |
               ((*DW & 0x00ff0000U) >>  8) | ((*DW & 0xff000000U) >> 24));
}
//------------------------------------------------------------------------------
inline void SWAP64(DWORDLONG *DWL)                           //������������ ������ ����
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

