    .set noreorder

#include "pspstub.s"

    STUB_START "SystemCtrlForUser",0x40090000,0x00020005
    STUB_FUNC  0x1090A2E1,sctrlHENGetVersion
    STUB_FUNC  0x5328B431,sctrlHENGetMinorVersion
