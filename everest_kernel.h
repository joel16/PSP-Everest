#ifndef ___KPSPIDENT_H___
#define ___KPSPIDENT_H___

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

u32 pspGetBaryonVersion(u32 *baryon); // 0x7EC5A957
u32 pspGetPommelVersion(u32 *pommel); // 0xE7E87741
u32 pspGetTachyonVersion(void);       // 0xE2A5D1EE
u64 pspGetFuseId(void);               // 0x4F46EEDE
u32 pspGetFuseConfig(void);           // 0x8F4F4E96
u32 pspGetKirkVersion(void);
u32 pspGetSpockVersion(void);
u32 pspNandGetScramble(void);
int pspGetRegion(void);
char *pspGetInitialFW(void);
int pspNandGetPageSize(void);
int pspNandGetTotalBlocks(void);
int pspNandGetPagesPerBlock(void);
u8 *pspGetMACAddress(u8 *buf);
int pspReadSerial(u16 *pdata);
int pspWriteSerial(u16* serial);
int pspChkregGetPsCode(ScePsCode *pPsCode);

#endif
