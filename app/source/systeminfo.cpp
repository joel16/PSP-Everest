#include <cstdio>
#include <cstring>
#include <pspkernel.h>
#include <systemctrl.h>

int sctrlHENGetMinorVersion();

namespace SystemInfo {
    static char get_firmware_buf[22], version_txt_buf[256];

    const char *GetCFWName(s32 *devkit) {
        u32 hen_version = sctrlHENGetVersion();
        
        if (*devkit == 0x05000010) {
            return "m33";
        }
        else if (*devkit == 0x05000210) {
            return "GEN";
        }
        else if (*devkit == 0x05000310) {
            if (hen_version != 0x8002013A) {
                return "GEN/MHU";
            }
            else {
                return "GEN/MHU";
            }
        }
        else if (*devkit == 0x05050010) {
            return "GEN";
        }
        else if (*devkit == 0x06020010) {
            if (static_cast<u32>(sctrlHENGetMinorVersion()) != 0x8002013A) {
                if (hen_version == 0x00001001) {
                    return "PRO";
                }
                else if (hen_version == 0x00001002) {
                    return "PRO-B";
                }
                else if (hen_version == 0x00001003) {
                    return "PRO-C";
                }
            }
            else if (hen_version == 0x00001000) {
                return "TN-A";
            }
            else if (hen_version == 0x00001001) {
                return "TN-B";
            }
            else if (hen_version == 0x00001002) {
                return "TN-C";
            }
            else if (hen_version == 0x00001003) {
                return "TN-D";
            }
            else if (hen_version == 0x00001004) {
                return "TN-E";
            }
        }
        else if (*devkit == 0x06030110) {
            return "PRO HEN";
        }
        else if (*devkit == 0x06030510) {
            if (static_cast<u32>(sctrlHENGetMinorVersion()) != 0x8002013A) {
                if (hen_version == 0x00001001) {
                    return "PRO";
                }
                else if (hen_version == 0x00001002) {
                    return "PRO-B";
                }
                else if (hen_version == 0x00001003) {
                    return "PRO-C";
                }
            }
            else if (hen_version == 0x00001000) {
                return "Custom";
            }
        }
        else if (*devkit == 0x06030610) {
            return "PRO HEN";
        }
        else if (*devkit == 0x06030710 && hen_version == 0x00001000) {
            return "ME";
        }
        else if (*devkit == 0x06030810 && hen_version == 0x00001000) {
            return "ME";
        }
        else if (*devkit == 0x06030910) {
            if (static_cast<u32>(sctrlHENGetMinorVersion()) != 0x8002013A) {
                if (hen_version == 0x00001001) {
                    return "PRO";
                }
                else if (hen_version == 0x00001002) {
                    return "PRO-B";
                }
                else if (hen_version == 0x00001003) {
                    return "PRO-C";
                }
            }
            else if (hen_version == 0x00001000) {
                return "ME";
            }
            else if (hen_version == 0x00002000) {
                return "TN-A";
            }
        }
        else if (*devkit == 0x06060010) {
            if (static_cast<u32>(sctrlHENGetMinorVersion()) != 0x8002013A) {
                if (hen_version == 0x00001001) {
                    return "PRO";
                }
                else if (hen_version == 0x00001002) {
                    return "PRO-B";
                }
                else if (hen_version == 0x00001003) {
                    return "PRO-C";
                }
            }
            else if (hen_version == 0x00001000) {
                return "ME";
            }
        }
        else if (*devkit == 0x06060110) {
            if (static_cast<u32>(sctrlHENGetMinorVersion()) != 0x8002013A) {
                if (hen_version == 0x00001001) {
                    return "PRO";
                }
                else if (hen_version == 0x00001002) {
                    return "PRO-B";
                }
                else if (hen_version == 0x00001003) {
                    return "PRO-C";
                }
            }
            else if (hen_version == 0x00001000) {
                return "ME";
            }
        }
        
        return "";
    }
    
    char *GetFirmware(s32 *devkit) {
        char *devkit_chr = (char *)&(*devkit);
        snprintf(get_firmware_buf, 22, "%i.%i%i %s", devkit_chr[3], devkit_chr[2], devkit_chr[1], SystemInfo::GetCFWName(devkit));
        return get_firmware_buf;
    }
    
    char *GetVersionTxt(void) {
        memset(version_txt_buf, 0, sizeof(version_txt_buf));
        SceUID fd = sceIoOpen("flash0:/vsh/etc/version.txt", PSP_O_RDONLY, 777);
        
        if (fd >= 0) {
            sceIoRead(fd, version_txt_buf, 255);
            sceIoClose(fd);
            return version_txt_buf;
        }
        
        return nullptr;
    }
}
