#pragma once

namespace HardwareInfo {
    int GetModelSymbol(void);
    const char *GetRegion(void);
    char *GetMotherboard(s32 *tachyon, s32 *baryon, s32 *pommel);
    char *GetUMDFirmware(void);
    char *GetMacAddress(void);
    const char *GetModel(void);
    const char *GetQAFlag(void);
}
