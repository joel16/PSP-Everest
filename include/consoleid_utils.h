#pragma once

#include <pspopenpsid.h>

const char *pspGetProductCodeInfo(u16 productCode);
const char *pspGetProductSubCodeInfo(u16 productSubCode);
const char *pspGetFactoryCodeInfo(u16 factoryCode);
const char *pspGetPSID(PspOpenPSID *openpsid);
