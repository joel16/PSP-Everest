#pragma once

#include <pspopenpsid.h>

namespace ConsoleIdInfo {
    const char *GetProductCode(u16 productCode);
    const char *GetProductSubCode(u16 productSubCode);
    const char *GetFactoryCode(u16 factoryCode);
    const char *GetPSID(PspOpenPSID *openpsid);
}

namespace HardwareInfo {
    int GetModelSymbol(void);
    const char *GetRegion(void);
    char *GetMotherboard(s32 *tachyon, s32 *baryon, s32 *pommel);
    char *GetUMDFirmware(void);
    char *GetMacAddress(void);
    const char *GetModel(void);
    const char *GetQAFlag(void);
}

namespace MiscInfo {
    const char *GetHeadphoneStatus(void);
}

namespace SystemInfo {
    char *GetFirmware(s32 *devkit);
    char *GetVersionTxt(void);
}
