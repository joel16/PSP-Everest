#include <pspsdk.h>
#include <pspkernel.h>
#include <string.h>
#include <stdio.h>
#include <vlf.h>
#include <kubridge.h>
#include <psputility_sysparam.h>

#include "main.h"
#include "everest_kernel_prx.h"
#include "kumdman_prx.h"
#include "intraFont_prx.h"
#include "vlf_prx.h"

extern int app_main(int argc, char *argv[]);

int SetupCallbacks(void) {
	int CallbackThread(SceSize args, void *argp) {
		int exit_callback(int arg1, int arg2, void *common) {
			sceKernelExitGame();
			return 0;
		}

		int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
		sceKernelRegisterExitCallback(cbid);
		sceKernelSleepThreadCB();

		return 0;
	}

	int thid = sceKernelCreateThread("PSP_EVEREST_UPDATE_THREAD", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
		sceKernelStartThread(thid, 0, 0);

	return thid;
}

void LoadStartModuleBuffer(char *path, char *buf, int size, SceSize args, void *argp) {
	SceUID mod, out;

	sceIoRemove(path);
	out = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT, 0777);
	sceIoWrite(out, buf, size);
	sceIoClose(out);

	mod = sceKernelLoadModule(path, 0, NULL);
	mod = sceKernelStartModule(mod, args, argp, NULL, NULL);
	sceIoRemove(path);
}

int start_thread(SceSize args, void *argp) {
	char *path = (char *)argp;
	int last_trail = -1;

	int i;
	for(i = 0; path[i]; i++)
		if(path[i] == '/')
			last_trail = i;

	if(last_trail >= 0)
		path[last_trail] = 0;

	sceIoChdir(path);
	path[last_trail] = '/';

	if(psp_model != 4)
		LoadStartModuleBuffer("kumdman.prx", (void *)kumdman_prx, size_kumdman_prx, args, argp);

	LoadStartModuleBuffer("everest_kernel.prx", (void *)everest_kernel_prx, size_everest_kernel_prx, args, argp);
	LoadStartModuleBuffer("intraFont.prx", (void *)intraFont_prx, size_intraFont_prx, args, argp);
	strcpy((void *)vlf_prx + 0x6D678, "flash0:/font/ltn0.pgf");//WARNING: Path for font not be more 23 characters!
	LoadStartModuleBuffer("vlf.prx", (void *)vlf_prx, size_vlf_prx, args, argp);

	vlfGuiInit(-1, app_main);

	return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, void *argp) {
	SetupCallbacks();
	psp_model = kuKernelGetModel();
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &language);

	SceUID thid = sceKernelCreateThread("PSP_EVEREST_START_THREAD", start_thread, 0x10, 0x4000, 0, NULL);
	if(thid < 0)
		return thid;

	sceKernelStartThread(thid, args, argp);

	return 0;
}
