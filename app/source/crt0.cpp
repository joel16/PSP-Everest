
#include <pspkernel.h>
#include <pspsdk.h>
#include <psputility_sysparam.h>
#include <string.h>
#include <stdio.h>
#include <vlf.h>



#include "main.h"
#include "kernel.h"

extern unsigned char kernel_prx_start[], intraFont_prx_start[], kumdman_prx_start[], vlf_prx_start[];
extern unsigned int kernel_prx_size, intraFont_prx_size, kumdman_prx_size, vlf_prx_size;

namespace Callbacks {
    int Exit(int arg1, int arg2, void *common) {
        sceKernelExitGame();
        return 0;
    }
    
    int Thread(SceSize args, void *argp) {
        int cbid = sceKernelCreateCallback("Exit Callback", Callbacks::Exit, nullptr);
        sceKernelRegisterExitCallback(cbid);
        sceKernelSleepThreadCB();
        return 0;
    }
    
    int SetupCallbacks(void) {
        int thid = sceKernelCreateThread("PSP_EVEREST_UPDATE_THREAD", Callbacks::Thread, 0x11, 0xFA0, 0, 0);
        if (thid >= 0) {
            sceKernelStartThread(thid, 0, 0);
        }
        
        return thid;
    }
}

void LoadStartModuleBuffer(const char *path, const void *buf, int size, SceSize args, void *argp) {
    SceUID modId = 0, fd = 0;
    
    sceIoRemove(path);
    fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT, 0777);
    sceIoWrite(fd, buf, size);
    sceIoClose(fd);
    
    modId = sceKernelLoadModule(path, 0, nullptr);
    modId = sceKernelStartModule(modId, args, argp, nullptr, nullptr);
    sceIoRemove(path);
}

void LoadStartModule(const char *path) {
    SceUID modId = sceKernelLoadModule(path, 0, nullptr);
    modId = sceKernelStartModule(modId, 0, nullptr, nullptr, nullptr);
}

#if defined (__cplusplus)
extern "C" {
#endif

extern int app_main(int argc, char *argv[]);

int start_thread(SceSize args, void *argp) {
    char *path = static_cast<char *>(argp);
    int last_trail = -1;
    
    for(int i = 0; path[i]; i++) {
        if (path[i] == '/') {
            last_trail = i;
        }
    }
    
    if (last_trail >= 0)
        path[last_trail] = 0;
        
    sceIoChdir(path);
    path[last_trail] = '/';

    strcpy(reinterpret_cast<char *>(vlf_prx_start) + 0x6D678, "flash0:/font/ltn0.pgf"); // WARNING: Path for font not be more 23 characters!
    
    LoadStartModuleBuffer("kernel.prx", kernel_prx_start, kernel_prx_size, args, argp);
    LoadStartModuleBuffer("intraFont.prx", intraFont_prx_start, intraFont_prx_size, args, argp);
    LoadStartModuleBuffer("vlf.prx", vlf_prx_start, vlf_prx_size, args, argp);

    psp_model = pspGetModel();

    if (psp_model != 4) {
        LoadStartModuleBuffer("kumdman.prx", kumdman_prx_start, kumdman_prx_size, args, argp);
    }

    LoadStartModule("flash0:/kd/chkreg.prx");

    vlfGuiInit(-1, app_main);
    return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, void *argp) {
    Callbacks::SetupCallbacks();
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &language);
    
    SceUID thid = sceKernelCreateThread("PSP_EVEREST_START_THREAD", start_thread, 0x10, 0x4000, 0, nullptr);
    if (thid < 0) {
        return thid;
    }

    sceKernelStartThread(thid, args, argp);
    return 0;
}

#if defined (__cplusplus)
}
#endif
