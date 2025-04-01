//>>> PSP_EVEREST 2
//Copyright(C) 2023, frostegater, Joel16

#include <cstdio>
#include <cstring>
#include <pspctrl.h>
#include <pspidstorage.h>
#include <pspkernel.h>
#include <psppower.h>
#include <psputility_sysparam.h>

#include "gui.h"
#include "info.h"
#include "kernel.h"
#include "main.h"
#include "screenshot.h"
#include "translate.h"
#include "utils.h"
#include "vlf.h"

PSP_MODULE_INFO("PSP EVEREST 2 Rev 10", PSP_MODULE_USER, 2, 10);
PSP_MAIN_THREAD_ATTR(0);

typedef struct {
    char initialFw[8];
    char kirk[4];
    char spock[4];
    s32 tachyon;
    int baryon;
    int pommel;
    int polestar;
    s32 devkit;
    u32 fusecfg;
    u32 scramble;
    u64 fuseid;
    PspOpenPSID psid;
    ScePsCode pscode;
    char *versionTxt;
} GuiData;

static GuiData guiData = { 0 };
int model = 0, language = 0;

namespace Menus {
    static constexpr u8 NUM_ITEMS_MAIN = 6;
    static constexpr u8 NUM_ITEMS_HARDWARE = 20;
    static constexpr u8 NUM_ITEMS_BATTERY = 14;
    static constexpr u8 NUM_ITEMS_SYSTEM = 7;
    static constexpr u8 NUM_ITEMS_CONSOLEID = 8;
    static constexpr u8 NUM_ITEMS_MISC = 4;

    static u8 menu = 0;
    static bool clear = false;
    static u16 bSerialData[2], serialData[2];
    static VlfText hardwareText[NUM_ITEMS_HARDWARE], batteryText[NUM_ITEMS_BATTERY],
        systemText[NUM_ITEMS_SYSTEM], consoleIdText[NUM_ITEMS_CONSOLEID], miscText[NUM_ITEMS_MISC];
        
    enum PageState {
        HARDWARE_INFO_PAGE,
        BATTERY_INFO_PAGE,
        SOFTWARE_INFO_PAGE,
        CONSOLE_ID_INFO_PAGE,
        MISC_INFO_PAGE,
        EXIT
    };

    void MainMenu(int select);

    int SubMenuHandler(int enter, bool clear, int max_items, VlfText *items, int selection) {
        if (!enter) {
            if (!clear) {
                for(int i = 0; i < max_items; i++) {
                    vlfGuiRemoveText(items[i]);
                }
            }
            
            if (!button_assign) {
                vlfGuiBottomDialog(-1, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, nullptr);
            }
            
            vlfGuiCancelBottomDialog();
            Menus::MainMenu(selection);
        }

        return VLF_EV_RET_NOTHING;
    }

    int HardwareInfoHandler(int enter) {
        return Menus::SubMenuHandler(enter, false, NUM_ITEMS_HARDWARE, hardwareText, HARDWARE_INFO_PAGE);
    }
    
    int BatteryInfoHandler(int enter) {
        clear = true;
        return Menus::SubMenuHandler(enter, clear, 0, nullptr, BATTERY_INFO_PAGE);
    }
    
    int SystemInfoHandler(int enter) {
        if (pic_button_assign != nullptr) {
            vlfGuiRemovePicture(pic_button_assign);
            pic_button_assign = nullptr;
        }

        return Menus::SubMenuHandler(enter, false, NUM_ITEMS_SYSTEM, systemText, SOFTWARE_INFO_PAGE);
    }
    
    int ConsoleIdInfoHandler(int enter) {
        return Menus::SubMenuHandler(enter, false, NUM_ITEMS_CONSOLEID, consoleIdText, CONSOLE_ID_INFO_PAGE);
    }

    int MiscInfoHandler(int enter) {
        clear = true;
        return Menus::SubMenuHandler(enter, clear, 0, nullptr, MISC_INFO_PAGE);
    }
    
    void HardwareInfo(void) {
        GUI::SetTitle(trans->hardware_title);
        
        hardwareText[0] = GUI::Printf(10, 40, "Tachyon: 0x%08X", guiData.tachyon);
        hardwareText[1] = GUI::Printf(10, 60, "Baryon: 0x%08X (%d.%d)", guiData.baryon, PSP_SYSCON_BARYON_GET_VERSION_MAJOR(guiData.baryon), PSP_SYSCON_BARYON_GET_VERSION_MINOR(guiData.baryon));
        hardwareText[2] = GUI::Printf(10, 80, "Pommel: 0x%08X", guiData.pommel);
        hardwareText[3] = GUI::Printf(10, 100, "Polestar: 0x%08X", guiData.polestar);
        hardwareText[4] = GUI::Printf(10, 120, "FuseID: 0x%llX", guiData.fuseid);
        hardwareText[5] = GUI::Printf(10, 140, "FuseCFG: 0x%08X", guiData.fusecfg);
        hardwareText[6] = GUI::Printf(10, 160, "IDScramble: 0x%08X", guiData.scramble);
        hardwareText[7] = GUI::Printf(10, 180, "Kirk: %c%c%c%c", guiData.kirk[3], guiData.kirk[2], guiData.kirk[1], guiData.kirk[0]);
        hardwareText[8] = GUI::Printf(10, 200, model == 4 ? "Spock: -" : "Spock: %c%c%c%c", guiData.spock[3], guiData.spock[2], guiData.spock[1], guiData.spock[0]);
        hardwareText[9] = GUI::Printf(10, 220, HardwareInfo::GetModelSymbol() != -1 ? trans->hardware.model : trans->hardware.no_model, model == 4 ? "N" : model == 10 ? "E" : "", HardwareInfo::GetModelSymbol(), pspGetRegion() < 10 ? "0" : "", pspGetRegion(), HardwareInfo::GetModel());
        
        hardwareText[10] = GUI::Printf(250, 40, trans->hardware.mobo, HardwareInfo::GetMotherboard(&guiData.tachyon, &guiData.baryon, &guiData.pommel));
        hardwareText[11] = GUI::Printf(250, 60, trans->hardware.region, HardwareInfo::GetRegion());
        hardwareText[12] = GUI::Printf(250, 80, trans->hardware.gen, model < 10 ? "0" : "", model + 1);
        hardwareText[13] = GUI::Printf(250, 100, trans->hardware.eeprom, guiData.tachyon <= 0x00500000 && guiData.tachyon != 0x00100000 && guiData.baryon <= 0x0022B200 ? trans->yes : trans->no);
        hardwareText[14] = GUI::Printf(250, 120, trans->hardware.pandora, guiData.tachyon <= 0x00500000 ? trans->yes : trans->no);
        hardwareText[15] = GUI::Printf(250, 140, "MAC: %s", HardwareInfo::GetMacAddress());
        hardwareText[16] = GUI::Printf(250, 160, trans->hardware.initialfw, guiData.initialFw);
        hardwareText[17] = GUI::Printf(250, 180, trans->hardware.umdfw, model == 4 ? "-" : HardwareInfo::GetUMDFirmware());
        hardwareText[18] = GUI::Printf(250, 200, trans->hardware.nandsize, (pspNandGetPageSize() * pspNandGetPagesPerBlock() * pspNandGetTotalBlocks()) / 1024 / 1024);
        hardwareText[19] = GUI::Printf(250, 220, "QA Flag: %s", HardwareInfo::GetQAFlag());
        
        GUI::SetBottomDialog(false, true, Menus::HardwareInfoHandler, true);
        GUI::SetFade();
    }
    
    void BatteryInfo(void) {
        bool swbt = true, checkbt = false;
        int elec = 0, total_elec = 0;
        
        if (button_assign) {
            vlfGuiBottomDialog(VLF_DI_BACK, -1, 1, 0, 0, nullptr);
        }
        
        vlfGuiCancelBottomDialog();
        GUI::SetTitle(trans->battery_title);
        
        fadeCtrl = false;
        bool firstCycle = true;
        
        for(int update = 0;; update++) {
            if (update == 25 || fadeCtrl) {
                update = 1;
                firstCycle = fadeCtrl;
                fadeCtrl = false;
            }
            
            vlfGuiDrawFrame();
            
            if (!firstCycle || update == 0) {
                for(int i = 0; i < NUM_ITEMS_BATTERY; i++) {
                    if (batteryText[i] != nullptr) {
                        vlfGuiRemoveText(batteryText[i]);
                        batteryText[i] = nullptr;
                    }
                }
                
                int battery_percent = scePowerGetBatteryLifePercent();
                int battery_life_time = scePowerGetBatteryLifeTime();
                
                if (update != 0 && scePowerIsBatteryExist() && swbt && (model == 0 || (guiData.tachyon <= 0x00500000 && guiData.baryon == 0x0022B200))) {
                    pspReadSerial(bSerialData);
                    
                    u16 wrbuffer[0x80];
                    wrbuffer[0] = 0x5053;
                    
                    if (bSerialData[0] != 0x5058) {
                        wrbuffer[1] = 0x5058;
                    }
                    else {
                        wrbuffer[1] = 0x4456;
                    }
                    
                    pspWriteSerial(wrbuffer);
                    
                    checkbt = false;
                    pspReadSerial(serialData);
                    
                    if (serialData[0] == wrbuffer[0] && serialData[1] == wrbuffer[1]) {
                        checkbt = true;
                        pspWriteSerial(bSerialData);
                    }
                    
                    swbt = false;
                }
                
                if (!swbt && !scePowerIsBatteryExist()) {
                    swbt = true;
                }
                
                batteryText[0] = GUI::Printf(15, 70, trans->battery.ex_power, model == 4 ? "-" : scePowerIsPowerOnline() ? trans->yes : trans->no);
                batteryText[1] = GUI::Printf(15, 90, trans->battery.batt_stat,  scePowerIsBatteryExist() ? trans->battery.batt_stat_present : trans->battery.batt_stat_absent);
                batteryText[2] = GUI::Printf(15, 110, trans->battery.charging, scePowerIsBatteryCharging() ? scePowerIsBatteryExist() ? battery_percent == 100 ? trans->battery.charging_cpl : trans->yes : trans->no : trans->no);
                batteryText[3] = GUI::Printf(15, 130, battery_percent > 100 || battery_percent < 0 ? trans->battery.no_charge_lvl : trans->battery.charge_lvl, battery_percent);
                batteryText[4] = GUI::Printf(15, 150, trans->battery.charge_stat, battery_percent >= 0 && battery_percent <= 20 ? trans->battery.charge_stat_low : battery_percent > 20 && battery_percent < 70 ? trans->battery.charge_stat_normal : battery_percent >= 70 && battery_percent <= 100 ? trans->battery.charge_stat_strong : "-");
                batteryText[5] = GUI::Printf(15, 170, battery_life_time < 0 || battery_life_time / 60 > 100 || (battery_life_time / 60 == 0 && battery_life_time - (battery_life_time / 60 * 60) == 0) ? trans->battery.no_left_time : trans->battery.left_time, battery_life_time / 60, battery_life_time - (battery_life_time / 60 * 60));
                batteryText[6] = GUI::Printf(15, 190, pspSysconBatteryGetElec(&elec) < 0? "Elec Charge: -" : "Elec Charge: %d mA", elec);
                
                batteryText[7] = GUI::Printf(240, 70, scePowerGetBatteryVolt() <= 0 ? trans->battery.no_voltage : trans->battery.voltage, (float)scePowerGetBatteryVolt() / 1000.0);
                batteryText[8] = GUI::Printf(240, 90, scePowerGetBatteryTemp() <= 0 ? trans->battery.no_temperature : trans->battery.temperature, scePowerGetBatteryTemp());
                batteryText[9] = GUI::Printf(240, 110, scePowerGetBatteryRemainCapacity() <= 0 ? trans->battery.no_remain_capacity : trans->battery.remain_capacity, scePowerGetBatteryRemainCapacity());
                batteryText[10] = GUI::Printf(240, 130, scePowerGetBatteryFullCapacity() <= 0 ? trans->battery.no_total_capacity : trans->battery.total_capacity, scePowerGetBatteryFullCapacity());
                batteryText[11] = GUI::Printf(240, 150, scePowerIsBatteryExist() && (model == 0 || (guiData.tachyon == 0x00500000 && guiData.baryon == 0x0022B200)) && checkbt ? trans->battery.serial : trans->battery.no_serial, bSerialData[0], bSerialData[1]);	
                batteryText[12] = GUI::Printf(240, 170, trans->battery.mode, 
                    checkbt && scePowerIsBatteryExist() && bSerialData[0] == 0xFFFF && bSerialData[1] == 0xFFFF && (model == 0 || (guiData.tachyon <= 0x00500000 && guiData.baryon == 0x0022B200)) ? trans->battery.mode_service :
                    checkbt && scePowerIsBatteryExist() && bSerialData[0] == 0x0000 && bSerialData[1] == 0x0000 && (model == 0 || (guiData.tachyon <= 0x00500000 && guiData.baryon == 0x0022B200)) ? trans->battery.mode_autoboot :
                    checkbt && scePowerIsBatteryExist() && (model == 0 || (guiData.tachyon <= 0x00500000 && guiData.baryon == 0x0022B200)) ? trans->battery.mode_default : "-");
                batteryText[13] = GUI::Printf(240, 190, pspSysconBatteryGetTotalElec(&total_elec) < 0? "Total Elec Charge: -" : "Total Elec Charge: %d mA", total_elec);
            }
            
            if (!update) {
                GUI::SetBottomDialog(false, true, Menus::BatteryInfoHandler, false);
                GUI::SetFade();
            }
            
            vlfGuiDrawFrame();
            
            if (clear) {
                for(int i = 0; i < NUM_ITEMS_BATTERY; i++) {
                    if (batteryText[i] != nullptr) {
                        vlfGuiRemoveText(batteryText[i]);
                        batteryText[i] = nullptr;
                    }
                }
                
                break;
            }
        }
    }
    
    void SystemInfo(void) {
        char username[32], password[5];;
        memset(username, 0, sizeof(username));
        memset(password, 0, sizeof(password));
        
        GUI::SetTitle(trans->system_title);
        
        systemText[0] = GUI::Printf(10, 45, trans->system.fw, SystemInfo::GetFirmware(&guiData.devkit));
        systemText[1] = GUI::Printf(10, 65, trans->system.button_assign);
        
        if (button_assign) {
            pic_button_assign = vlfGuiAddPictureResource("system_plugin_fg.rco", "tex_cross", 4, -2);
        }
        else {
            pic_button_assign = vlfGuiAddPictureResource("system_plugin_fg.rco", "tex_circle", 4, -2);
        }
        
        vlfGuiSetPictureXY(pic_button_assign, 131, 68);
        
        char unicode_username[26];
        Utils::UTF8ToUnicode(reinterpret_cast<wchar_t *>(unicode_username), reinterpret_cast<char *>(Utils::GetRegistryValue("/CONFIG/SYSTEM", "owner_name", &username, sizeof(username), 0)));
        
        systemText[2] = GUI::Printf(237, 45, trans->system.username);
        systemText[3] = vlfGuiAddTextW(language == PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN ? 337 : 327, 45, reinterpret_cast<u16 *>(unicode_username));
        systemText[4] = GUI::Printf(237, 65, trans->system.password, Utils::GetRegistryValue("/CONFIG/SYSTEM/LOCK", "password", &password, sizeof(password), 0));
        systemText[5] = GUI::Printf(10, 120, "version.txt:");
        
        if (guiData.versionTxt != nullptr) {
            systemText[6] = vlfGuiAddTextF(10, 143, guiData.versionTxt);
        }
        else {
            systemText[6] = GUI::Printf(10, 143, trans->system.vertxterr);
        }
        
        vlfGuiSetTextFontSize(systemText[6], 0.75f);
        GUI::SetBottomDialog(false, true, Menus::SystemInfoHandler, true);
        GUI::SetFade();
    }
    
    void ConsoleIdInfo(void) {
        GUI::SetTitle("Console ID Information");

        consoleIdText[0] = GUI::Printf(10, 40, "PSID: %s", ConsoleIdInfo::GetPSID(&guiData.psid));
        
        consoleIdText[1] = GUI::Printf(10, 60, "Company Code: %d", guiData.pscode.companyCode);
        
        consoleIdText[2] = GUI::Printf(10, 90, "Factory Code: %d", guiData.pscode.factoryCode);
        consoleIdText[3] = GUI::Printf(10, 110, ConsoleIdInfo::GetFactoryCode(guiData.pscode.factoryCode));
        
        consoleIdText[4] = GUI::Printf(10, 140, "Product Code: 0x%04X", guiData.pscode.productCode);
        consoleIdText[5] = GUI::Printf(10, 160, ConsoleIdInfo::GetProductCode(guiData.pscode.productCode));
        
        consoleIdText[6] = GUI::Printf(10, 190, "Product Sub Code: 0x%04X", guiData.pscode.productSubCode);
        consoleIdText[7] = GUI::Printf(10, 210, ConsoleIdInfo::GetProductSubCode(guiData.pscode.productSubCode));
        
        GUI::SetBottomDialog(false, true, Menus::ConsoleIdInfoHandler, true);
        GUI::SetFade();
    }

    void MiscInfo(void) {
        GUI::SetTitle("Miscellaneous Information");

        fadeCtrl = false;
        bool firstCycle = true;
        
        for(int update = 0;; update++) {
            if (update == 25 || fadeCtrl) {
                update = 1;
                firstCycle = fadeCtrl;
                fadeCtrl = false;
            }
            
            vlfGuiDrawFrame();
            
            if (!firstCycle || update == 0) {
                for(int i = 0; i < NUM_ITEMS_MISC; i++) {
                    if (miscText[i] != nullptr) {
                        vlfGuiRemoveText(miscText[i]);
                        miscText[i] = nullptr;
                    }
                }
                
                miscText[0] = GUI::Printf(10, 40, "Headphone status: %s", MiscInfo::GetHeadphoneStatus());
                miscText[1] = GUI::Printf(10, 60, "Hold switch: %s", MiscInfo::GetHoldSwitchState());

                if (model != 10) {
                    miscText[2] = GUI::Printf(10, 80, "Wlan switch: %s", MiscInfo::GetWlanSwitchState());
                }
            }

            if (!update) {
                GUI::SetBottomDialog(false, true, Menus::MiscInfoHandler, false);
                GUI::SetFade();
            }
            
            vlfGuiDrawFrame();
            
            if (clear) {
                for(int i = 0; i < NUM_ITEMS_MISC; i++) {
                    if (miscText[i] != nullptr) {
                        vlfGuiRemoveText(miscText[i]);
                        miscText[i] = nullptr;
                    }
                }
                
                break;
            }
        }
    }

    int Capture(void *param) {
        Screenshot::Capture(menu);
        return VLF_EV_RET_NOTHING;
    }

    void HandleScreenshot(int select) {
        menu = select;
        vlfGuiRemoveEventHandler(Menus::Capture);
        vlfGuiAddEventHandler(PSP_CTRL_SQUARE, -1, Menus::Capture, nullptr);
    }
    
    int MainMenuHandler(int select) {
        switch(select) {
            case HARDWARE_INFO_PAGE:
                vlfGuiCancelCentralMenu();
                Menus::HardwareInfo();
                Menus::HandleScreenshot(select);
                break;
                
            case BATTERY_INFO_PAGE:
                vlfGuiCancelCentralMenu();
                clear = false;
                Menus::HandleScreenshot(select);
                Menus::BatteryInfo();
                break;
                
            case SOFTWARE_INFO_PAGE:
                vlfGuiCancelCentralMenu();
                Menus::SystemInfo();
                Menus::HandleScreenshot(select);
                break;
                
            case CONSOLE_ID_INFO_PAGE:
                vlfGuiCancelCentralMenu();
                Menus::ConsoleIdInfo();
                Menus::HandleScreenshot(select);
                break;
                
            case MISC_INFO_PAGE:
                vlfGuiCancelCentralMenu();
                clear = false;
                Menus::HandleScreenshot(select);
                Menus::MiscInfo();
                break;
                
            case EXIT:
                sceKernelExitGame();
                break;
        }
        
        return VLF_EV_RET_NOTHING;
    }
    
    void MainMenu(int select) {
        GUI::SetTitle("PSP EVEREST 2 Rev 10");
        
        const char *main_guiDatas[] = {
            "Hardware Information",
            "Battery Information",
            "Software Information",
            "Console ID Information",
            "Miscellaneous Information",
            "Exit"
        };
        
        vlfGuiCentralMenu(NUM_ITEMS_MAIN, main_guiDatas, select, Menus::MainMenuHandler, 0, 0);
        GUI::SetBottomDialog(true, false, Menus::MainMenuHandler, false);
    }
}

extern "C" int app_main(int argc, char *argv[]) {
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &language);
    SetupTranslate();
    
    if (language == PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN) {
        vlfGuiSetLanguage(PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN);
    }
    else {
        vlfGuiSetLanguage(PSP_SYSTEMPARAM_LANGUAGE_ENGLISH);
    }
        
    *(u32 *)guiData.kirk = pspGetKirkVersion();
    *(u32 *)guiData.spock = pspGetSpockVersion();
    guiData.tachyon = pspGetTachyonVersion();
    guiData.fuseid = pspGetFuseId();
    guiData.fusecfg = pspGetFuseConfig();
    guiData.scramble = pspNandGetScramble();
    pspGetBaryonVersion(&guiData.baryon);
    pspGetPommelVersion(&guiData.pommel);
    pspGetPolestarVersion(&guiData.polestar);
    guiData.devkit = sceKernelDevkitVersion();
    pspGetInitialFW(guiData.initialFw);
    pspChkregGetPsCode(&guiData.pscode);
    sceOpenPSIDGetOpenPSID(&guiData.psid);
    Utils::GetRegistryValue("/CONFIG/SYSTEM/XMB", "button_assign", &button_assign, 4, 1);
    guiData.versionTxt = SystemInfo::GetVersionTxt();
    
    vlfGuiSystemSetup(1, 1, 1);
    vlfGuiAddEventHandler(PSP_CTRL_RTRIGGER, -1, GUI::OnBackgroundPlus, nullptr);
    vlfGuiAddEventHandler(PSP_CTRL_LTRIGGER, -1, GUI::OnBackgroundMinus, nullptr);

    GUI::GenRandomBackground();
    GUI::SetBackground();
    Menus::MainMenu(0);
    
    while(1) {
        vlfGuiDrawFrame();
    }
        
    return 0;
}
