#include <cstdio>
#include <pspkernel.h>

#include "consoleidinfo.h"

// Info from https://github.com/CelesteBlue-dev/PS-ConsoleId-wiki/blob/master/PS-ConsoleId-wiki.txt
namespace ConsoleIdInfo {
    const char *GetProductCodeInfo(u16 productCode) {
        const char *product_codes[] = {
            "Test - Prototype / Test Unit",
            "Tool - DevKit / Development Tool",
            "DEX - TestKit / Testing Kit",
            "CEX - Japan",
            "CEX - North America",
            "CEX - Europe/East/Africa",
            "CEX - Korea",
            "CEX - Great Britain/United Kingdom",
            "CEX - Mexico/Latin America",
            "CEX - Australia/New Zeland",
            "CEX - Hong Kong/Singapore",
            "CEX - Taiwan",
            "CEX - Russia",
            "CEX - China",
            "AVTOOL - AV Testing Tool",
        };
        
        if ((productCode >= 0x00) && (productCode <= 0x0E)) {
            return product_codes[productCode];
        }
        
        return "-";
    }
    
    const char *GetProductSubCodeInfo(u16 productSubCode) {
        const char *product_sub_codes[] = {
            "-",
            "TA-079 / TA-081, TMU-002",
            "TA-082 / TA-086",
            "TA-085 / TA-088",
            "TA-090 / TA-092",
            "TA-091",
            "TA-093",
            "TA-094",
            "TA-095",
            "TA-096 / TA-097",
        };
        
        if ((productSubCode >= 0x01) && (productSubCode <= 0x09)) {
            return product_sub_codes[productSubCode];
        }
        
        return "-";
    }
    
    const char *GetFactoryCodeInfo(u16 factoryCode) {
        const char *factory_codes[] {
            "Invalid: PSP Kicho & Dencho Program",
            "-",
            "China manufacture 1 (01g)",
            "China manufacture 2 (02g, 03g, 04g, 05g, 07g, 09g)",
            "China manufacture 3 (04g and 11g)"
        };

        if (factoryCode < 5) {
            return factory_codes[factoryCode];
        }
        else {
            if (factoryCode == 35) {
                return "Japan Diagnosis Center 1 (Diag/QA PSP)";
            }
            else if (factoryCode == 62) {
                return "Servicing Center 2 (refurbished 01g)";
            }
        }
        
        return "-";
    }
    
    const char *GetPSID(PspOpenPSID *openpsid) {
        static char PSID[33];
        
        for (int i = 0; i < 16; i++) {
            snprintf(&PSID[2 * i], 33, "%02X", openpsid->data[i]);
        }
        
        return PSID;
    }
}
