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
#include "kernel.h"

PSP_MODULE_INFO("kernel", 0x1006, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define MAKE_CALL(f) (0x0C000000 | (((u32)(f) >> 2) & 0x03ffffff))

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
s32 sceChkreg_driver_59F8491D(ScePsCode *pPsCode);

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

static int pspErrCheck(u32 chdata) {
    if ((chdata & 0x80250000) == 0x80250000) {
        return -1;
    }
    else if (chdata & 0xFFFF0000) {
        return(chdata & 0xFFFF0000) >> 16;
    }

    return 0;
}

int pspReadSerial(u16 *pdata) {
    int err = 0;
    u32 data;
    
    u32 k1 = pspSdkSetK1(0);
    
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

int pspChkregGetPsCode(ScePsCode *pPsCode) {
    int k1 = pspSdkSetK1(0);
    int ret = sceChkreg_driver_59F8491D(pPsCode);
    pspSdkSetK1(k1);
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

int pspGetModel(void) {
    int k1 = pspSdkSetK1(0);
    int ret = sceKernelGetModel();
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
