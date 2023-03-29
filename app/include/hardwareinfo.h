#pragma once

namespace HardwareInfo {
    int GetModelSymbol(void);
    const char *GetRegion(void);
    char *GetMotherboard(void);
    char *GetUMDFirmware(void);
    char *GetMacAddress(void);
    const char *GetModel(void);
    const char *GetQAFlag(void);
}
