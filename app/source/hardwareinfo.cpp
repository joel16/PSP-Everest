#include <cstring>
#include <cstdio>
#include <pspkernel.h>

#include "kernel.h"
#include "kumdman.h"
#include "main.h"
#include "translate.h"
#include "utils.h"

#define UNKNOWN 0x00000000

ATAPI_INQURIY ai;
u8 buf[0x38];
u8 param[4] = { 0, 0, 0x38, 0 };
char outtxt[0x12];

namespace HardwareInfo {
    typedef struct {
        s32 tachyon;
        s32 baryon;
        s32 pommel;
        const char *mobo_name;
    } Motherboard;
    
    Motherboard detmobo[] = {
        /* PSP-100x */
        { 0x00140000, 0x00030600, 0x00000103, "TA-079v1" },
        { 0x00200000, 0x00030600, 0x00000103, "TA-079v2" },
        { 0x00200000, 0x00040600, 0x00000103, "TA-079v3" },
        { 0x00300000, 0x00040600, 0x00000103, "TA-081v1" },
        { 0x00300000, 0x00040600, 0x00000104, "TA-081v2" },
        { 0x00400000, 0x00114000, 0x00000112, "TA-082" },
        { 0x00400000, 0x00121000, 0x00000112, "TA-086" },
        
        /* PSP-200x */
        { 0x00500000, 0x0022B200, 0x00000123, "TA-085v1" },
        { 0x00500000, 0x00234000, 0x00000123, "TA-085v2" },
        { 0x00500000, 0x00243000, 0x00000123, "TA-088v1" },// >----| If initial FW: 4.01, skipped TA-088v1...
        { 0x00500000, 0x00243000, 0x00000123, "TA-088v2" },// <----| ...and detected TA-088v2.
        { 0x00600000, 0x00243000, 0x00000123, "TA-088v3" },
        { 0x00500000, 0x00243000, 0x00000132, "TA-090v1" },
        
        /* PSP-300x */
        { 0x00600000, 0x00263100, 0x00000132, "TA-090v2" },
        { 0x00600000, 0x00263100, 0x00000133, "TA-090v3" },
        { 0x00600000, 0x00285000, 0x00000133, "TA-092" },	
        { 0x00810000, 0x002C4000, 0x00000141, "TA-093v1" },
        { 0x00810000, 0x002C4000, 0x00000143, "TA-093v2" },
        { 0x00810000, 0x002E4000, 0x00000154, "TA-095v1" },
        { 0x00820000, 0x002E4000, 0x00000154, "TA-095v2" },
        
        /* PSP-N100x (PSPgo) */
        { 0x00720000, 0x00304000, 0x00000133, "TA-091" },
        { 0x00800000, 0x002A0000, UNKNOWN, "TA-094" },
        
        /* PSP-E100x (PSP Essentials aka PSP Street) */
        { 0x00900000, 0x00403000, 0x00000154, "TA-096" },
        
        /* DTP-T1000A */
        { 0x00100000, UNKNOWN, UNKNOWN, "Devkit" },
    };

    int GetModelSymbol(void) {
        switch(psp_model + 1) {
            case 1:
            case 5:
            case 11:
                return 1;
            
            case 2:
                return 2;
                
            case 3:
            case 4:
            case 7:
            case 9:
                return 3;
                
            default:
                break;
        }
        
        return -1;
    }
    
    const char *GetRegion(void) {
        int region = pspGetRegion();
        
        if (region >= 0 && region < 11) {
            return trans->hardware.regname[region];
        }
        
        return "-";
    }
    
    char *GetMotherboard(s32 *tachyon, s32 *baryon, s32 *pommel) {
        char initial_fw[8];
        char *ret_mobo = const_cast<char *>("-");
        
        for(unsigned int i = 0; i < sizeof(detmobo) / sizeof(Motherboard); i++) {
            if (detmobo[i].tachyon == *tachyon && (detmobo[i].baryon == *baryon || detmobo[i].baryon == UNKNOWN) && (detmobo[i].pommel == *pommel || detmobo[i].pommel == UNKNOWN)) {
                /* TA-088v1(3.95) / TA-088v2 (4.01) */
                if (i == 9 /* TA-088v1 */ && !strncmp(pspGetInitialFW(initial_fw), "4.01", 4)) {
                    continue;
                }
                else if (i == 10 /* TA-088v2 */ && strncmp(pspGetInitialFW(initial_fw), "3.95", 4)) {
                    ret_mobo = const_cast<char *>("TA-088v1/v2");
                }
                
                ret_mobo = const_cast<char *>(detmobo[i].mobo_name);
            }
        }
        
        return ret_mobo;
    }
    
    char *GetUMDFirmware(void) {
        pspUmdExecInquiryCmd(pspUmdManGetUmdDrive(0), param, buf);
        memset(outtxt, 0, sizeof(outtxt));
        memcpy(&ai, buf, sizeof(ATAPI_INQURIY));
        snprintf(outtxt, 5, ai.sony_spec);
        return outtxt;
    }
    
    char *GetMacAddress(void) {
        u8 macaddr[18];
        pspGetMACAddress(macaddr);
        
        static char macbuf[18];
        snprintf(macbuf, 18, "%02X:%02X:%02X:%02X:%02X:%02X", macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
        
        return macbuf;
    }
    
    const char *GetModel(void) {
        const char *models[] = { "PSP Fat", "PSP Slim", "PSP Brite", "PSP Brite", "PSPgo", "-", "PSP Brite", "-", "PSP Brite", "-", "PSP Street" };
        return models[psp_model];
    }

    const char *GetQAFlag(void) {
        u8 ps_flags = 0;
        int ret = pspChkregGetPsFlags(&ps_flags, 0);

        if (ret < 0) {
            return "-";
        }

        return ps_flags == 0x00000001? "0x00000001" : "0x00000002";
    }
}
