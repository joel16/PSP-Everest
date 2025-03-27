#include "kernel.h"

namespace MiscInfo {
    const char *GetHeadphoneStatus(void) {
        return pspGetHPConnect()? "connected" : "disconnected";
    }
}