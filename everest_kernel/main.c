#include <pspsdk.h>
#include <pspkernel.h>
#include <string.h>
#include <stdio.h>
#include <pspidstorage.h>
#include <pspsysmem_kernel.h>
#include <pspnand_driver.h>
#include <pspwlan.h>
#include <pspsysmem.h>

#include "systemctrl.h"
#include "everest_kernel.h"

PSP_MODULE_INFO("EVEREST_KERNEL", 0x1006, 7, 4);
PSP_MAIN_THREAD_ATTR(0);

#define MAKE_CALL(f) (0x0C000000 | (((u32)(f) >> 2) & 0x03ffffff))

/**
 * This structure represents a unique per-console identifier. It contains console specific information and can be used,
 * for example, for DRM purposes and simple PSP hardware model checks.
 *
 * @remark On the PSP, Sony uses the term "PSID" (not to mixup with the term "OpenPSID" which represents a different set of
 * unique identifier bits). On later consoles, like the PS Vita and PS4, Sony uses the term "ConsoleId" for this set of
 * identifier bits. To be consistent within the PS family, we are going with the term "ConsoleId" here, even though APIs like
 * sceOpenPSIDGetPSID() (which returns the ConsoleId) will remain as originally named by Sony.
 */
typedef struct {
    /* Unknown. On retail set to 0. */
    u16 unk0; // 0
    /* Company code. Set to 1. */
    u16 companyCode; // 2
    /* Product code. */
    u16 productCode; // 4
    /* Product sub code. */
    u16 productSubCode; // 6
    /* Upper two bit of PsFlags. */
    u8 psFlagsMajor : 2; // 8
    /* Factory code. */
    u8 factoryCode : 6; // 8
    u8 uniqueIdMajor : 2; // 9
    /* Lower six bit of the PsFlags. Contain the QA flag, if set. */
    u8 psFlagsMinor : 6; // 9
    u8 uniqueIdMinor[6]; // 10
} SceConsoleId; // size = 16

/*
 * This structure contains the ConsoleId (termed "PSID" on the PSP) and an ECDSA signature used to verify the correctness of the 
 * ConsoleId.
 * The ConsoleId is used, for example, in PSN DRM, DNAS and system configuration (with its derived PSCode).
 */
typedef struct {
    /* Unique per-console identifier. */
    SceConsoleId consoleId; // 0
    /* Contains the public key of the certificate. No padding. */
    u8 plantextPublicKey[0x28]; // 16
    /* The 'r' part of the ECDSA signature pair (r, s). */
    u8 r[0x14]; // 56
    /* The 's' part of the ECDSA signature pair (r, s). */
    u8 s[0x14]; // 76
    /* The ECDSA public key (can be used to verify ECDSA signature rs). */
    u8 publicKey[0x28]; // 96
    /* Contains the encrypted private key of the certificate (with padding). */
    u8 encPrivateKey[0x20]; // 136
    /* Hash of previous data. */
    u8 hash[0x10]; // 168
} SceIdStorageConsoleIdCertificate; // size = 184

s32 sceSysconGetBaryonVersion(s32 *baryon);
s32 sceSysconGetPommelVersion(s32 *pommel);
s32 sceSyscon_driver_FB148FB6(s32 *polestar); // sceSysconGetPolestarVersion
u64 sceSysreg_driver_4F46EEDE(void);          // sceSysregGetFuseId
u32 sceSysreg_driver_8F4F4E96(void);          // sceSysregGetFuseConfig
int sceSysregKirkBusClockEnable(void);
int sceSysregAtaBusClockEnable(void);
u32 sceSysconCmdExec(void *param, int unk);
int sceSysconBatteryGetElec(int *elec);
int sceSyscon_driver_4C539345(int *elec);     // sceSysconBatteryGetTotalElec
static int (*sceUtilsBufferCopyWithRange)(u8 *outbuff, int outsize, u8 *inbuff, int insize, int cmd);

static SceIdStorageConsoleIdCertificate g_ConsoleIdCertificate;

static int _sceUtilsBufferCopyWithRange(u8 *outbuff, int outsize, u8 *inbuff, int insize, int cmd) {
    return (*sceUtilsBufferCopyWithRange)(outbuff, outsize, inbuff, insize, cmd);
}

static void pspPatchMemlmd(SceModule *mod) {
    u32 text_addr = mod->text_addr;
    sceUtilsBufferCopyWithRange = (void *)(sctrlHENFindFunction("sceMemlmd", "semaphore", 0x4C537C72));
    _sw(MAKE_CALL(_sceUtilsBufferCopyWithRange), text_addr+0x000009FC);
}

static void pspSyncCache(void) {
    sceKernelIcacheInvalidateAll();
    sceKernelDcacheWritebackInvalidateAll();
}

u32 pspGetBaryonVersion(s32 *baryon) {
    int k1 = pspSdkSetK1(0);
    u32 err = sceSysconGetBaryonVersion(baryon);
    pspSdkSetK1(k1);
    return err;
}

u32 pspGetPommelVersion(s32 *pommel) {
    int k1 = pspSdkSetK1(0);
    u32 err = sceSysconGetPommelVersion(pommel);
    pspSdkSetK1(k1);
    return err;
}

u32 pspGetTachyonVersion(void) {
    int k1 = pspSdkSetK1(0);
    u32 err = sceSysregGetTachyonVersion();
    pspSdkSetK1(k1);
    return err;
}

u32 pspGetPolestarVersion(s32 *polestar) {
    int k1 = pspSdkSetK1(0);
    u32 err = sceSyscon_driver_FB148FB6(polestar);
    pspSdkSetK1(k1);
    return err;
}

u64 pspGetFuseId(void) {
    int k1 = pspSdkSetK1(0);
    u64 err = sceSysreg_driver_4F46EEDE();
    pspSdkSetK1(k1);
    return err;
}

u32 pspGetFuseConfig(void) {
    int k1 = pspSdkSetK1(0);
    u32 err = sceSysreg_driver_8F4F4E96();
    pspSdkSetK1(k1);
    return err;
}

u32 pspGetKirkVersion(void) {
    int k1 = pspSdkSetK1(0);
    sceSysregKirkBusClockEnable();
    sceKernelDelayThread(1000);
    u32 err = *(u32 *)0xBDE00004;
    pspSdkSetK1(k1);
    return err;
}

u32 pspGetSpockVersion(void) {
    int k1 = pspSdkSetK1(0);
    sceSysregAtaBusClockEnable();
    sceKernelDelayThread(1000);
    u32 err = *(u32 *)0xBDF00004;
    pspSdkSetK1(k1);
    return err;
}

u32 pspNandGetScramble(void) {
    int k1 = pspSdkSetK1(0);
    u32 magic = 0;
    u32 buf[4];
    u32 sha[5];
    
    buf[0] = *(vu32 *)(0xBC100090);
    buf[1] = *(vu32 *)(0xBC100094);
    buf[2] = *(vu32 *)(0xBC100090) << 1;
    buf[3] = 0xD41D8CD9;
    
    sceKernelUtilsSha1Digest((u8 *)buf, sizeof(buf), (u8 *)sha);
    magic = (sha[0] ^ sha[3]) + sha[2];
    pspSdkSetK1(k1);
    return magic;
}

int pspIdStorageLookup(u16 key, u32 offset, void *buf, u32 len) {
    int k1 = pspSdkSetK1(0);
    
    memset(buf, 0, len);
    int ret = sceIdStorageLookup(key, offset, buf, len);
    
    pspSdkSetK1(k1);
    return ret;
}

/*
0x03 - Japan
0x04 - America
0x05 - Europe
0x06 - Korea
0x07 - United Kingdom
0x08 - Mexico
0x09 - Australia
0x0A - Hong-Kong
0x0B - Taiwan
0x0C - Russia
0x0D - China
*/
int pspGetRegion(void) {
    u8 region[1];
    pspIdStorageLookup(0x100, 0x3D, region, 1);
    
    if (region[0] == 0x03)      // Japan
        return 0;
    else if (region[0] == 0x04) // America
        return 1;
    else if (region[0] == 0x09) // Australia
        return 2;
    else if (region[0] == 0x07) // United Kingdom
        return 3;
    else if (region[0] == 0x05) // Europe
        return 4;
    else if (region[0] == 0x06) // Korea
        return 5;
    else if (region[0] == 0x0A) // Hong-Kong
        return 6;
    else if (region[0] == 0x0B) // Taiwan
        return 7;
    else if (region[0] == 0x0C) // Russia
        return 8;
    else if (region[0] == 0x0D) // China
        return 9;
    else if (region[0] == 0x08) // Mexico
        return 10;
    else
        return -1;
}

char *pspGetInitialFW(char *buf) {
    pspIdStorageLookup(0x51, 0, buf, 5);
    
    if (buf[0] == 0)
        snprintf(buf, 2, "-");
    
    return buf;
}

int pspNandGetPageSize(void) {
    int k1 = pspSdkSetK1(0);
    int ret = sceNandGetPageSize();
    pspSdkSetK1(k1);
    return ret;
}

int pspNandGetPagesPerBlock(void) {
    int k1 = pspSdkSetK1(0);
    int ret = sceNandGetPagesPerBlock();
    pspSdkSetK1(k1);
    return ret;
}

int pspNandGetTotalBlocks(void) {
    int k1 = pspSdkSetK1(0);
    int ret = sceNandGetTotalBlocks();
    pspSdkSetK1(k1);
    return ret;
}

u8 *pspGetMACAddress(u8 *buf) {
    sceWlanGetEtherAddr(buf);
    return buf;
}

static u32 pspWriteBat(u8 addr, u16 data) {
    int k1 = pspSdkSetK1(0);
    
    int res = 0;
    u8 param[0x60];
    
    if (addr > 0x7F)
        return(0x80000102);
        
    param[0x0C] = 0x73;
    param[0x0D] = 5;
    param[0x0E] = addr;
    param[0x0F] = data;
    param[0x10] = data >> 8;
    
    res = sceSysconCmdExec(param, 0);
    if (res < 0)
        return res;
        
    pspSdkSetK1(k1);
    return 0;
}

int pspWriteSerial(u16 *serial) {
    int err = 0;
    
    err = pspWriteBat(0x07, serial[0]);
    if (!err)
        err = pspWriteBat(0x09, serial[1]);
        
    return err;
}

static u32 pspReadEEPROM(u8 addr) {
    u8 param[0x60];
    
    if (addr > 0x7F)
        return 0x80000102;
        
    param[0x0C] = 0x74;
    param[0x0D] = 3;
    param[0x0E] = addr;
    
    int res = sceSysconCmdExec(param, 0);
    if (res < 0)
        return res;
        
    return (param[0x21] << 8) | param[0x20];
}

int pspReadSerial(u16 *pdata) {
    int err = 0;
    u32 data;
    
    u32 k1 = pspSdkSetK1(0);
    
    int pspErrCheck(u32 chdata) {
        if ((chdata & 0x80250000) == 0x80250000)
            return -1;
        else if (chdata & 0xFFFF0000)
            return(chdata & 0xFFFF0000) >> 16;
            
        return 0;
    }
    
    data = pspReadEEPROM(0x07);	
    err = pspErrCheck(data);
    
    if (err >= 0) {
        pdata[0] = (data & 0xFFFF);
        data = pspReadEEPROM(0x09);
        err = pspErrCheck(data);
        if (err >= 0)
            pdata[1] = (data & 0xFFFF);
        else
            err = data;
    }
    else
        err = data;
        
    pspSdkSetK1(k1);
    return err;
}

// Re-implementation of Subroutine sub_000001C4 - Address 0x000001C4 (openpsid.prx)
static int sceOpenPSIDLookupAndVerifyConsoleIdCertificate(void) {
    int ret = 0;
    const int KIRK_CERT_LEN = 0xB8;
    
    /* Obtain a ConsoleId certificate. TODO: Use include/idstorage.h for these values once chkreg gets merged */
    ret = pspIdStorageLookup(0x100, 0x38, &g_ConsoleIdCertificate, KIRK_CERT_LEN);
    if (ret < 0) {
        ret = pspIdStorageLookup(0x120, 0x38, &g_ConsoleIdCertificate, KIRK_CERT_LEN);
        if (ret < 0)
            return 0xC0520002;
    }
    
    int k1 = pspSdkSetK1(0);
    ret = _sceUtilsBufferCopyWithRange(NULL, 0, (u8 *)&g_ConsoleIdCertificate, KIRK_CERT_LEN, 0x12);
    pspSdkSetK1(k1);
    
    if (ret != 0)
        return 0xC0520001;
    
    return 0;
}

// Reimplementation of Subroutine sceChkreg_driver_59F8491D (without sema) - Address 0x00000438
int pspChkregGetPsCode(ScePsCode *pPsCode) {
    int ret = 0;
    
    if (((ret = sceOpenPSIDLookupAndVerifyConsoleIdCertificate()) == 0)) {
        pPsCode->companyCode = g_ConsoleIdCertificate.consoleId.companyCode >> 0x8;
        pPsCode->productCode = g_ConsoleIdCertificate.consoleId.productCode >> 0x8;
        pPsCode->productSubCode = g_ConsoleIdCertificate.consoleId.productSubCode >> 0x8;
        pPsCode->factoryCode = g_ConsoleIdCertificate.consoleId.factoryCode;
    }
    
    return ret;
}

int pspSysconBatteryGetElec(int *elec) {
    int k1 = pspSdkSetK1(0);
    int ret = sceSysconBatteryGetElec(elec);
    pspSdkSetK1(k1);
    return ret;
}

int pspSysconBatteryGetTotalElec(int *elec) {
    int k1 = pspSdkSetK1(0);
    int ret = sceSyscon_driver_4C539345(elec);
    pspSdkSetK1(k1);
    return ret;
}

int module_start(SceSize args __attribute__((unused)), void *argp __attribute__((unused))) {
    pspPatchMemlmd(sceKernelFindModuleByName("sceMesgLed"));
    pspSyncCache();
    return 0;
}

int module_stop(void) {
    return 0;
}
