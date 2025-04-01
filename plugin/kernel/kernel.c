#include <string.h>
#include <stdio.h>
#include <pspidstorage.h>
#include <pspkernel.h>
#include <pspnand_driver.h>
#include <pspsyscon.h>
#include <pspsysmem_kernel.h>
#include <pspwlan.h>

#include "systemctrl.h"
#include "kernel.h"

PSP_MODULE_INFO("kernel", 0x1006, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define MAKE_CALL(f) (0x0C000000 | (((u32)(f) >> 2) & 0x03ffffff))

u64 sceSysreg_driver_4F46EEDE(void);          // sceSysregGetFuseId
u32 sceSysreg_driver_8F4F4E96(void);          // sceSysregGetFuseConfig
int sceSysregKirkBusClockEnable(void);
int sceSysregAtaBusClockEnable(void);
u32 sceSysconCmdExec(void *param, int unk);
int sceSysconBatteryGetElec(int *elec);
int sceSyscon_driver_4C539345(int *elec);     // sceSysconBatteryGetTotalElec
static int (*sceUtilsBufferCopyWithRange)(u8 *outbuff, int outsize, u8 *inbuff, int insize, int cmd);
s32 sceChkregGetPsCode(ScePsCode *pPsCode);
s32 sceChkregGetPsFlags(u8 *pPsFlags, s32 index);
s8 sceSysconGetWlanSwitch(void);
s8 sceSysconGetBtSwitch(void);
s8 sceSysconGetHoldSwitch(void);

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

int pspGetBaryonVersion(int *baryon) {
    int k1 = pspSdkSetK1(0);
    int ret = sceSysconGetBaryonVersion(baryon);
    pspSdkSetK1(k1);
    return ret;
}

int pspGetPommelVersion(int *pommel) {
    int k1 = pspSdkSetK1(0);
    int ret = sceSysconGetPommelVersion(pommel);
    pspSdkSetK1(k1);
    return ret;
}

u32 pspGetTachyonVersion(void) {
    int k1 = pspSdkSetK1(0);
    u32 ret = sceSysregGetTachyonVersion();
    pspSdkSetK1(k1);
    return ret;
}

int pspGetPolestarVersion(int *polestar) {
    int k1 = pspSdkSetK1(0);
    int ret = sceSysconGetPolestarVersion(polestar);
    pspSdkSetK1(k1);
    return ret;
}

u64 pspGetFuseId(void) {
    int k1 = pspSdkSetK1(0);
    u64 ret = sceSysreg_driver_4F46EEDE();
    pspSdkSetK1(k1);
    return ret;
}

u32 pspGetFuseConfig(void) {
    int k1 = pspSdkSetK1(0);
    u32 ret = sceSysreg_driver_8F4F4E96();
    pspSdkSetK1(k1);
    return ret;
}

u32 pspGetKirkVersion(void) {
    int k1 = pspSdkSetK1(0);
    sceSysregKirkBusClockEnable();
    sceKernelDelayThread(1000);
    u32 ret = *(u32 *)0xBDE00004;
    pspSdkSetK1(k1);
    return ret;
}

u32 pspGetSpockVersion(void) {
    int k1 = pspSdkSetK1(0);
    sceSysregAtaBusClockEnable();
    sceKernelDelayThread(1000);
    u32 ret = *(u32 *)0xBDF00004;
    pspSdkSetK1(k1);
    return ret;
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

int pspGetRegion(void) {
    u8 region;
    pspIdStorageLookup(0x100, 0x3D, &region, 1);

    switch (region) {
        case 0x03: 
            return 0;  // Japan
        
        case 0x04: 
            return 1;  // America
        
        case 0x09: 
            return 2;  // Australia
        
        case 0x07: 
            return 3;  // United Kingdom
        
        case 0x05: 
            return 4;  // Europe
        
        case 0x06: 
            return 5;  // Korea
        
        case 0x0A: 
            return 6;  // Hong-Kong
        
        case 0x0B: 
            return 7;  // Taiwan
        
        case 0x0C: 
            return 8;  // Russia
        
        case 0x0D: 
            return 9;  // China
        
        case 0x08: 
            return 10; // Mexico
        
        default:   
            return -1; // Unknown region
    }
}

char *pspGetInitialFW(char *buf) {
    pspIdStorageLookup(0x51, 0, buf, 5);
    
    if (buf[0] == 0) {
        snprintf(buf, 2, "-");
    }
    
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
    
    if (addr > 0x7F) {
        return(0x80000102);
    }
        
    param[0x0C] = 0x73;
    param[0x0D] = 5;
    param[0x0E] = addr;
    param[0x0F] = data;
    param[0x10] = data >> 8;
    
    res = sceSysconCmdExec(param, 0);
    if (res < 0) {
        return res;
    }
        
    pspSdkSetK1(k1);
    return 0;
}

int pspWriteSerial(u16 *serial) {
    int ret = 0;
    
    ret = pspWriteBat(0x07, serial[0]);
    if (!ret) {
        ret = pspWriteBat(0x09, serial[1]);
    }
        
    return ret;
}

static u32 pspReadEEPROM(u8 addr) {
    u8 param[0x60];
    
    if (addr > 0x7F) {
        return 0x80000102;
    }
        
    param[0x0C] = 0x74;
    param[0x0D] = 3;
    param[0x0E] = addr;
    
    int res = sceSysconCmdExec(param, 0);
    if (res < 0) {
        return res;
    }
        
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
        if (err >= 0) {
            pdata[1] = (data & 0xFFFF);
        }
        else {
            err = data;
        }
    }
    else {
        err = data;
    }
        
    pspSdkSetK1(k1);
    return err;
}

int pspChkregGetPsCode(ScePsCode *psCode) {
    int k1 = pspSdkSetK1(0);
    int ret = sceChkregGetPsCode(psCode);
    pspSdkSetK1(k1);
    return ret;
}

int pspChkregGetPsFlags(u8 *psFlags, s32 index) {
    int k1 = pspSdkSetK1(0);
    int ret = sceChkregGetPsFlags(psFlags, index);
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

s8 pspGetHPConnect(void) {
    int k1 = pspSdkSetK1(0);
    s8 ret = sceSysconGetHPConnect();
    pspSdkSetK1(k1);
    return ret;
}

s8 pspGetWlanSwitch(void) {
    int k1 = pspSdkSetK1(0);
    s8 ret = sceSysconGetWlanSwitch();
    pspSdkSetK1(k1);
    return ret;
}

s8 pspGetHoldSwitch(void) {
    int k1 = pspSdkSetK1(0);
    s8 ret = sceSysconGetHoldSwitch();
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
