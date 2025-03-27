    .set noreorder

#include "pspimport.s"

    IMPORT_START "SystemCtrlForKernel",0x00090011
    IMPORT_FUNC  "SystemCtrlForKernel",0x159AF5CC,sctrlHENFindFunction

    IMPORT_START "sceChkreg_driver",0x00090011
    IMPORT_FUNC  "sceChkreg_driver",0x59F8491D,sceChkregGetPsCode
    IMPORT_FUNC  "sceChkreg_driver",0x6894A027,sceChkregGetPsFlags

    IMPORT_START "sceSyscon_driver",0x00090011
    IMPORT_FUNC  "sceSyscon_driver",0x2D6F2728,sceSysconGetHPConnect
