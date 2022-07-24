#pragma once

/**
 * This structure contains console specific information. It is a subset of the ::SceConsoleId.
 * Check <openpsid_kernel.h> for possible member values.
 */
typedef struct {
    /* Company code. Set to 1. */
    u16 companyCode; // 0
    /* Product code. */
    u16 productCode; // 2
    /* Product sub code. */
    u16 productSubCode; // 4
    /* Factory code. */
    u16 factoryCode; // 6
} ScePsCode; // size = 8

u32 pspGetBaryonVersion(s32 *baryon);
u32 pspGetPommelVersion(s32 *pommel);
u32 pspGetPolestarVersion(s32 *polestar);
u32 pspGetTachyonVersion(void);
u64 pspGetFuseId(void);
u32 pspGetFuseConfig(void);
u32 pspGetKirkVersion(void);
u32 pspGetSpockVersion(void);
u32 pspNandGetScramble(void);
int pspGetRegion(void);
char *pspGetInitialFW(char *buf);
int pspNandGetPageSize(void);
int pspNandGetTotalBlocks(void);
int pspNandGetPagesPerBlock(void);
u8 *pspGetMACAddress(u8 *buf);
int pspReadSerial(u16 *pdata);
int pspWriteSerial(u16* serial);
int pspChkregGetPsCode(ScePsCode *pPsCode);
int pspSysconBatteryGetElec(int *elec);
int pspSysconBatteryGetTotalElec(int *elec);
