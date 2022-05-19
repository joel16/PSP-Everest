#ifndef ___KPSPIDENT_H___
#define ___KPSPIDENT_H___

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

#endif
