#include <pspsdk.h>
#include <pspkernel.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <pspreg.h>
#include <psprtc.h>

#include "main.h"

int GetRegistryValue(const char *dir, const char *name, void *buf, int bufsize, int inttype) {
    int ret = 0;
    struct RegParam reg;
    REGHANDLE h;
    
    memset(&reg, 0, sizeof(reg));
    reg.regtype = 1;
    reg.namelen = strlen("/system");
    reg.unk2 = 1;
    reg.unk3 = 1;
    strncpy(reg.name, "/system", 8);
    
    if (sceRegOpenRegistry(&reg, 2, &h) == 0) {
        REGHANDLE hd;
        if (!sceRegOpenCategory(h, dir, 2, &hd)) {
            REGHANDLE hk;
            unsigned int type, size;
            
            if (!sceRegGetKeyInfo(hd, name, &hk, &type, &size)) {
                if (!sceRegGetKeyValue(hd, hk, buf, bufsize)) {
                    ret = inttype ? 1 : (int)buf;
                    sceRegFlushCategory(hd);
                }
            }
            
            sceRegCloseCategory(hd);
        }
        
        sceRegFlushRegistry(h);
        sceRegCloseRegistry(h);
    }
    
    return ret;
}

int Random(int min, int max) {
    u64 tick;
    SceKernelUtilsMt19937Context ctx;
    sceRtcGetCurrentTick(&tick);
    sceKernelUtilsMt19937Init(&ctx, (u32)tick);
    return min + (sceKernelUtilsMt19937UInt(&ctx) % max);
}

int utf82unicode(wchar_t *dest, char *src) {
    int i, x;
    unsigned char *usrc = (unsigned char *)src;
    
    for(i = 0, x = 0; usrc[i];) {
        wchar_t ch;
        
        if ((usrc[i] & 0xE0) == 0xE0) {
            ch = ((usrc[i] & 0x0F) << 12) | ((usrc[i + 1] & 0x3F) << 6) | (usrc[i + 2] & 0x3F);
            i += 3;
        }
        else if ((usrc[i] & 0xC0) == 0xC0) {
            ch = ((usrc[i] & 0x1F) << 6) | (usrc[i+1] & 0x3F);
            i += 2;
        }
        else {
            ch = usrc[i];
            i += 1;
        }
        
        dest[x++] = ch;
    }
    
    dest[x++] = '\0';
    return x;
}

void ascii2unicode(char *unicode, const char *ascii) {
    while(*ascii != '\0') {
        if ((unsigned char)*ascii >= 0xC0) {
            *unicode++ = (unsigned char)*ascii - 0xB0;
            *unicode++ = 0x04;
        }
        else if ((unsigned char)*ascii == 0x99) {
            *unicode++ = 0x22;
            *unicode++ = 0x21;
        }
        else if ((unsigned char)*ascii == 0xB8) {
            *unicode++ = 0x51;
            *unicode++ = 0x04;
        }
        else if ((unsigned char)*ascii == 0xA8) {
            *unicode++ = 0x01;
            *unicode++ = 0x04;
        }
        else {
            *unicode++ = *ascii;
            *unicode++ = '\0';
        }
        
        ascii++;
    }
    
    *unicode++ = '\0';
    *unicode++ = '\0';
}

VlfText pspEverestPrintf(int x, int y, const char *text, ...) {
    char ascii[256], unicode[256];
    va_list list;
    va_start(list, text);
    vsprintf(ascii, text, list);
    va_end(list);
    ascii2unicode(unicode, ascii);
    return vlfGuiAddTextW(x, y, (u16 *)unicode);
}
