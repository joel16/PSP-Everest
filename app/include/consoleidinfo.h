#pragma once

#include <pspopenpsid.h>

namespace ConsoleIdInfo {
    const char *GetProductCodeInfo(u16 productCode);
    const char *GetProductSubCodeInfo(u16 productSubCode);
    const char *GetFactoryCodeInfo(u16 factoryCode);
    const char *GetPSID(PspOpenPSID *openpsid);
}
