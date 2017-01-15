
//                                 07.08.2008
#ifndef __MME_DB_HEAD_H__
#define __MME_DB_HEAD_H__

#pragma pack(push,1)

typedef struct { DWORD Dw1[2], NumT, Dw2; LONGLONG size1, size2, size3, size4; DWORD Dw3[6]; } PSP_MME;
typedef struct { WORD year, mon, day, hour; BYTE min, sec; } DATE_T1;
typedef struct { WORD Year; BYTE Mon, Day, Hour, Min, Sec; } DATE_T2;

typedef struct { DWORD Dw1[22]; /*Number;*/ DATE_T1 DaT1; BYTE prCan1[2]; /*DWORD B;*/
                 BYTE nAV; BYTE prCan2[2]; char nameTV[6]; BYTE b1[6], Q;
                 DATE_T2 DaT2; BYTE C2; DWORD timeLong; LONGLONG sizeF; BYTE Janr, b22;
                 WCHAR Title[33]; DWORD Dw2[3]; WORD nPart, E; DWORD Dw3[2];  } PSP_TIT0;
typedef struct { DWORD Dw1[22], Number; DATE_T1 DaT1; BYTE prCan1[2]; DWORD B;
                 BYTE nAV; BYTE prCan2[2]; char nameTV[6]; BYTE b1[6], Q;
                 DATE_T2 DaT2; BYTE C2; DWORD timeLong; LONGLONG sizeF; BYTE Janr, b22;
                 WCHAR Title[33]; DWORD Dw2[3]; WORD nPart, E; DWORD Dw3[2];  } PSP_TIT1;
typedef struct { DWORD Dw1[24], Number; DATE_T1 DaT1; BYTE prCan1[2]; DWORD B;
                 BYTE nAV; BYTE prCan2[2]; char nameTV[6]; BYTE b1[6], Q;
                 DATE_T2 DaT2; BYTE C2; DWORD timeLong; LONGLONG sizeF; BYTE Janr, b22;
                 WCHAR Title[33]; DWORD Dw2[3]; WORD nPart, E; DWORD Dw3[2];  } PSP_TIT2;
typedef struct { DWORD Dw1[24], Number; DATE_T1 DaT1; BYTE prCan1[2]; DWORD B;
                 BYTE nAV; BYTE prCan2[2]; char nameTV[6]; BYTE b1[14], Q;
                 DATE_T2 DaT2; BYTE C2; DWORD timeLong; LONGLONG sizeF; BYTE Janr, b22;
                 WCHAR Title[33]; DWORD Dw2[3]; WORD nPart, E; DWORD Dw3[2];  } PSP_TIT4;
typedef struct { DWORD Dw1[24], Number; DATE_T1 DaT1; BYTE prCan1[2]; DWORD B;
                 BYTE nAV; BYTE prCan2[2]; char nameTV[6]; BYTE b1[14], Q;
                 DATE_T2 DaT2; BYTE C2; DWORD timeLong; LONGLONG sizeF; BYTE Janr, b22;
                 WCHAR Title[33]; DWORD Dw2[3+2]; WORD nPart, E; DWORD Dw3[2];} PSP_TIT5;

typedef struct { WCHAR Name1[30]; BYTE b1[4]; WCHAR Name2[30]; DWORD Dw1[9];
                 LONGLONG sizeF; DWORD Dw2[3], timeLong, tLong2;
                 BYTE b2[5]; BYTE prCan0[2]; BYTE bb;
                 BYTE nAV, b3[3]; DWORD Dw3[3], AAA0, Dw4, AAA1, Dw5[2];}  ONE_NAME;

typedef struct { DWORD Dw1[6]; WCHAR Name[30]; DWORD Dw2[3];  } ONE_MET;
typedef struct { WORD numMet, A; DWORD B; } END_REC;
typedef struct { WORD NN; BYTE b1[6]; } END0_ZAP_MME;
typedef struct { DWORD Dw1[8]; WCHAR Name3[30]; DWORD Dw2[3]; } END1_ZAP_MME;

#pragma pack(pop)

static_assert(sizeof(PSP_TIT0) == 228, "Error sizeof(PSP_TIT0)");
static_assert(sizeof(PSP_TIT1) == 236, "Error sizeof(PSP_TIT1)");
static_assert(sizeof(PSP_TIT2) == 244, "Error sizeof(PSP_TIT2)");
static_assert(sizeof(PSP_TIT4) == 252, "Error sizeof(PSP_TIT4)");
static_assert(sizeof(PSP_TIT5) == 260, "Error sizeof(PSP_TIT5)");
static_assert(sizeof(END1_ZAP_MME) == (112 - 8), "Error sizeof(END1_ZAP_MME)");
static_assert(sizeof(PSP_MME) == 72, "Error sizeof(PSP_MME)");
static_assert(sizeof(ONE_NAME) == 232, "Error sizeof(ONE_NAME)");
static_assert(sizeof(ONE_MET) == 96, "Error sizeof(ONE_MET)");

#endif

