#include "kernel.h"

namespace MiscInfo {
    const char *GetHeadphoneStatus(void) {
        return pspGetHPConnect()? "connected" : "disconnected";
    }

    const char *GetWlanSwitchState(void) {
        return pspGetWlanSwitch()? "on" : "off";
    }

    const char *GetHoldSwitchState(void) {
        return pspGetHoldSwitch()? "on" : "off";
    }
}
