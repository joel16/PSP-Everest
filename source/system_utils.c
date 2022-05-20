#include <pspsdk.h>
#include <pspkernel.h>
#include <systemctrl.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "utils.h"
#include "translate.h"

int sctrlHENGetMinorVersion();
static char get_firmware_buf[256], version_txt_buf[256];

char *pspGetFirmwareName(void) {
    char *cfwname = "";
    int henid = sctrlHENGetVersion();
    
    if (devkit == 0x05000010)
        cfwname = "m33";
    else if (devkit == 0x05000210)
        cfwname = "GEN";
    else if (devkit == 0x05000310) {
        if (henid != 0x8002013A)
            cfwname = "GEN/MHU";
        else
            cfwname = "GEN/MHU";
    }
    else if (devkit == 0x05050010)
        cfwname = "GEN";
    else if (devkit == 0x06020010) {
        if (sctrlHENGetMinorVersion() != 0x8002013A) {
            if (henid == 0x00001001)
                cfwname = "PRO";
            else if (henid == 0x00001002)
                cfwname = "PRO-B";
            else if (henid == 0x00001003)
                cfwname = "PRO-C";
        }
        else if (henid == 0x00001000)
            cfwname = "TN-A";
        else if (henid == 0x00001001)
            cfwname = "TN-B";
        else if (henid == 0x00001002)
            cfwname = "TN-C";
        else if (henid == 0x00001003)
            cfwname = "TN-D";
        else if (henid == 0x00001004)
            cfwname = "TN-E";
    }
    else if (devkit == 0x06030110)
        cfwname = "PRO HEN";
    else if (devkit == 0x06030510) {
        if (sctrlHENGetMinorVersion() != 0x8002013A) {
            if (henid == 0x00001001)
                cfwname = "PRO";
            else if (henid == 0x00001002)
                cfwname = "PRO-B";
            else if (henid == 0x00001003)
                cfwname = "PRO-C";
        }
        else if (henid == 0x00001000)
            cfwname = "Custom";
    }
    else if (devkit == 0x06030610)
        cfwname = "PRO HEN";
    else if (devkit == 0x06030710 && henid == 0x00001000)
        cfwname = "ME";
    else if (devkit == 0x06030810 && henid == 0x00001000)
        cfwname = "ME";
    else if (devkit == 0x06030910) {
        if (sctrlHENGetMinorVersion() != 0x8002013A) {
            if (henid == 0x00001001)
                cfwname = "PRO";
            else if (henid == 0x00001002)
                cfwname = "PRO-B";
            else if (henid == 0x00001003)
                cfwname = "PRO-C";
        }
        else if (henid == 0x00001000)
            cfwname = "ME";
        else if (henid == 0x00002000)
            cfwname = "TN-A";
    }
    else if (devkit == 0x06060010) {
        if (sctrlHENGetMinorVersion() != 0x8002013A) {
            if (henid == 0x00001001)
                cfwname = "PRO";
            else if (henid == 0x00001002)
                cfwname = "PRO-B";
            else if (henid == 0x00001003)
                cfwname = "PRO-C";
        }
        else if (henid == 0x00001000)
            cfwname = "ME";
    }
    
    char *devkit_chr = (char *)&devkit;
    sprintf(get_firmware_buf, "%i.%i%i %s", devkit_chr[3], devkit_chr[2], devkit_chr[1], cfwname);
    return get_firmware_buf;
}

char *pspGetVersionTxt(void) {
    memset(version_txt_buf, 0, sizeof(version_txt_buf));
    SceUID fd = sceIoOpen("flash0:/vsh/etc/version.txt", PSP_O_RDONLY, 777);
    
    if (fd >= 0)
        sceIoRead(fd, version_txt_buf, 255);
    else {
        sceIoClose(fd);
        return NULL;
    }
    
    sceIoClose(fd);
    return version_txt_buf;
}
