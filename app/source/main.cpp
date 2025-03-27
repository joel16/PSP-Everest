//>>> PSP_EVEREST 2
//Copyright(C) 2023, frostegater, Joel16

#include <cstdio>
#include <cstring>
#include <pspctrl.h>
#include <pspidstorage.h>
#include <pspkernel.h>
#include <psppower.h>
#include <psputility_sysparam.h>

#include "consoleidinfo.h"
#include "gui.h"
#include "hardwareinfo.h"
#include "kernel.h"
#include "main.h"
#include "miscinfo.h"
#include "screenshot.h"
#include "systeminfo.h"
#include "translate.h"
#include "utils.h"
#include "vlf.h"

PSP_MODULE_INFO("PSP EVEREST 2 Rev 9", PSP_MODULE_USER, 2, 7);
PSP_MAIN_THREAD_ATTR(0);

typedef struct {
    char initial_fw[8];
    char kirk[4];
    char spock[4];
    s32 tachyon;
    s32 baryon;
    s32 pommel;
    s32 polestar;
    s32 devkit;
    u32 fusecfg;
    u32 scramble;
    u64 fuseid;
    PspOpenPSID psid;
    ScePsCode pscode;
    char *vertxt;
} MenuItem;

static MenuItem menu_item = { 0 };
int psp_model = 0, language = 0;

namespace Menus {
    static constexpr u8 NUM_DEL_ITEMS_MAIN = 6;
    static constexpr u8 NUM_DEL_ITEMS_HARDWARE = 20;
    static constexpr u8 NUM_DEL_ITEMS_BATTERY = 14;
    static constexpr u8 NUM_DEL_ITEMS_SYSTEM = 7;
    static constexpr u8 NUM_DEL_ITEMS_CONSOLEID = 8;
    static constexpr u8 NUM_DEL_ITEMS_MISC = 1;

    static u8 menu = 0;
    static bool clear = false;
    static u16 bserialdata[2], serialdata[2];
    static VlfText text_hardware[NUM_DEL_ITEMS_HARDWARE], text_battery[NUM_DEL_ITEMS_BATTERY],
        text_system[NUM_DEL_ITEMS_SYSTEM], text_consoleId[NUM_DEL_ITEMS_CONSOLEID], text_misc[NUM_DEL_ITEMS_MISC];
        
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
        return Menus::SubMenuHandler(enter, false, NUM_DEL_ITEMS_HARDWARE, text_hardware, HARDWARE_INFO_PAGE);
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

        return Menus::SubMenuHandler(enter, false, NUM_DEL_ITEMS_SYSTEM, text_system, SOFTWARE_INFO_PAGE);
    }
    
    int ConsoleIdInfoHandler(int enter) {
        return Menus::SubMenuHandler(enter, false, NUM_DEL_ITEMS_CONSOLEID, text_consoleId, CONSOLE_ID_INFO_PAGE);
    }

    int MiscInfoHandler(int enter) {
        clear = true;
        return Menus::SubMenuHandler(enter, false, NUM_DEL_ITEMS_MISC, text_misc, MISC_INFO_PAGE);
    }
    
    void HardwareInfo(void) {
        GUI::SetTitle(trans->hardware_title);
        
        text_hardware[0] = GUI::Printf(10, 40, "Tachyon: 0x%08X", menu_item.tachyon);
        text_hardware[1] = GUI::Printf(10, 60, "Baryon: 0x%08X", menu_item.baryon);
        text_hardware[2] = GUI::Printf(10, 80, "Pommel: 0x%08X", menu_item.pommel);
        text_hardware[3] = GUI::Printf(10, 100, "Polestar: 0x%08X", menu_item.polestar);
        text_hardware[4] = GUI::Printf(10, 120, "FuseID: 0x%llX", menu_item.fuseid);
        text_hardware[5] = GUI::Printf(10, 140, "FuseCFG: 0x%08X", menu_item.fusecfg);
        text_hardware[6] = GUI::Printf(10, 160, "IDScramble: 0x%08X", menu_item.scramble);
        text_hardware[7] = GUI::Printf(10, 180, "Kirk: %c%c%c%c", menu_item.kirk[3], menu_item.kirk[2], menu_item.kirk[1], menu_item.kirk[0]);
        text_hardware[8] = GUI::Printf(10, 200, psp_model == 4 ? "Spock: -" : "Spock: %c%c%c%c", menu_item.spock[3], menu_item.spock[2], menu_item.spock[1], menu_item.spock[0]);
        text_hardware[9] = GUI::Printf(10, 220, HardwareInfo::GetModelSymbol() != -1 ? trans->hardware.model : trans->hardware.no_model, psp_model == 4 ? "N" : psp_model == 10 ? "E" : "", HardwareInfo::GetModelSymbol(), pspGetRegion() < 10 ? "0" : "", pspGetRegion(), HardwareInfo::GetModel());
        
        text_hardware[10] = GUI::Printf(250, 40, trans->hardware.mobo, HardwareInfo::GetMotherboard(&menu_item.tachyon, &menu_item.baryon, &menu_item.pommel));
        text_hardware[11] = GUI::Printf(250, 60, trans->hardware.region, HardwareInfo::GetRegion());
        text_hardware[12] = GUI::Printf(250, 80, trans->hardware.gen, psp_model < 10 ? "0" : "", psp_model + 1);
        text_hardware[13] = GUI::Printf(250, 100, trans->hardware.eeprom, menu_item.tachyon <= 0x00500000 && menu_item.tachyon != 0x00100000 && menu_item.baryon <= 0x0022B200 ? trans->yes : trans->no);
        text_hardware[14] = GUI::Printf(250, 120, trans->hardware.pandora, menu_item.tachyon <= 0x00500000 ? trans->yes : trans->no);
        text_hardware[15] = GUI::Printf(250, 140, "MAC: %s", HardwareInfo::GetMacAddress());
        text_hardware[16] = GUI::Printf(250, 160, trans->hardware.initialfw, menu_item.initial_fw);
        text_hardware[17] = GUI::Printf(250, 180, trans->hardware.umdfw, psp_model == 4 ? "-" : HardwareInfo::GetUMDFirmware());
        text_hardware[18] = GUI::Printf(250, 200, trans->hardware.nandsize, (pspNandGetPageSize() * pspNandGetPagesPerBlock() * pspNandGetTotalBlocks()) / 1024 / 1024);
        text_hardware[19] = GUI::Printf(250, 220, "QA Flag: %s", HardwareInfo::GetQAFlag());
        
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
        bool first_cycle = true;
        
        for(int update = 0;; update++) {
            if (update == 25 || fadeCtrl) {
                update = 1;
                first_cycle = fadeCtrl;
                fadeCtrl = false;
            }
            
            vlfGuiDrawFrame();
            
            if (!first_cycle || update == 0) {
                for(int i = 0; i < NUM_DEL_ITEMS_BATTERY; i++) {
                    if (text_battery[i] != nullptr) {
                        vlfGuiRemoveText(text_battery[i]);
                        text_battery[i] = nullptr;
                    }
                }
                
                int battery_percent = scePowerGetBatteryLifePercent();
                int battery_life_time = scePowerGetBatteryLifeTime();
                
                if (update != 0 && scePowerIsBatteryExist() && swbt && (psp_model == 0 || (menu_item.tachyon <= 0x00500000 && menu_item.baryon == 0x0022B200))) {
                    pspReadSerial(bserialdata);
                    
                    u16 wrbuffer[0x80];
                    wrbuffer[0] = 0x5053;
                    
                    if (bserialdata[0] != 0x5058) {
                        wrbuffer[1] = 0x5058;
                    }
                    else {
                        wrbuffer[1] = 0x4456;
                    }
                    
                    pspWriteSerial(wrbuffer);
                    
                    checkbt = false;
                    pspReadSerial(serialdata);
                    
                    if (serialdata[0] == wrbuffer[0] && serialdata[1] == wrbuffer[1]) {
                        checkbt = true;
                        pspWriteSerial(bserialdata);
                    }
                    
                    swbt = false;
                }
                
                if (!swbt && !scePowerIsBatteryExist()) {
                    swbt = true;
                }
                
                text_battery[0] = GUI::Printf(15, 70, trans->battery.ex_power, psp_model == 4 ? "-" : scePowerIsPowerOnline() ? trans->yes : trans->no);
                text_battery[1] = GUI::Printf(15, 90, trans->battery.batt_stat,  scePowerIsBatteryExist() ? trans->battery.batt_stat_present : trans->battery.batt_stat_absent);
                text_battery[2] = GUI::Printf(15, 110, trans->battery.charging, scePowerIsBatteryCharging() ? scePowerIsBatteryExist() ? battery_percent == 100 ? trans->battery.charging_cpl : trans->yes : trans->no : trans->no);
                text_battery[3] = GUI::Printf(15, 130, battery_percent > 100 || battery_percent < 0 ? trans->battery.no_charge_lvl : trans->battery.charge_lvl, battery_percent);
                text_battery[4] = GUI::Printf(15, 150, trans->battery.charge_stat, battery_percent >= 0 && battery_percent <= 20 ? trans->battery.charge_stat_low : battery_percent > 20 && battery_percent < 70 ? trans->battery.charge_stat_normal : battery_percent >= 70 && battery_percent <= 100 ? trans->battery.charge_stat_strong : "-");
                text_battery[5] = GUI::Printf(15, 170, battery_life_time < 0 || battery_life_time / 60 > 100 || (battery_life_time / 60 == 0 && battery_life_time - (battery_life_time / 60 * 60) == 0) ? trans->battery.no_left_time : trans->battery.left_time, battery_life_time / 60, battery_life_time - (battery_life_time / 60 * 60));
                text_battery[6] = GUI::Printf(15, 190, pspSysconBatteryGetElec(&elec) < 0? "Elec Charge: -" : "Elec Charge: %d mA", elec);
                
                text_battery[7] = GUI::Printf(240, 70, scePowerGetBatteryVolt() <= 0 ? trans->battery.no_voltage : trans->battery.voltage, (float)scePowerGetBatteryVolt() / 1000.0);
                text_battery[8] = GUI::Printf(240, 90, scePowerGetBatteryTemp() <= 0 ? trans->battery.no_temperature : trans->battery.temperature, scePowerGetBatteryTemp());
                text_battery[9] = GUI::Printf(240, 110, scePowerGetBatteryRemainCapacity() <= 0 ? trans->battery.no_remain_capacity : trans->battery.remain_capacity, scePowerGetBatteryRemainCapacity());
                text_battery[10] = GUI::Printf(240, 130, scePowerGetBatteryFullCapacity() <= 0 ? trans->battery.no_total_capacity : trans->battery.total_capacity, scePowerGetBatteryFullCapacity());
                text_battery[11] = GUI::Printf(240, 150, scePowerIsBatteryExist() && (psp_model == 0 || (menu_item.tachyon == 0x00500000 && menu_item.baryon == 0x0022B200)) && checkbt ? trans->battery.serial : trans->battery.no_serial, bserialdata[0], bserialdata[1]);	
                text_battery[12] = GUI::Printf(240, 170, trans->battery.mode, 
                    checkbt && scePowerIsBatteryExist() && bserialdata[0] == 0xFFFF && bserialdata[1] == 0xFFFF && (psp_model == 0 || (menu_item.tachyon <= 0x00500000 && menu_item.baryon == 0x0022B200)) ? trans->battery.mode_service :
                    checkbt && scePowerIsBatteryExist() && bserialdata[0] == 0x0000 && bserialdata[1] == 0x0000 && (psp_model == 0 || (menu_item.tachyon <= 0x00500000 && menu_item.baryon == 0x0022B200)) ? trans->battery.mode_autoboot :
                    checkbt && scePowerIsBatteryExist() && (psp_model == 0 || (menu_item.tachyon <= 0x00500000 && menu_item.baryon == 0x0022B200)) ? trans->battery.mode_default : "-");
                text_battery[13] = GUI::Printf(240, 190, pspSysconBatteryGetTotalElec(&total_elec) < 0? "Total Elec Charge: -" : "Total Elec Charge: %d mA", total_elec);
            }
            
            if (!update) {
                GUI::SetBottomDialog(false, true, Menus::BatteryInfoHandler, false);
                GUI::SetFade();
            }
            
            vlfGuiDrawFrame();
            
            if (clear) {
                for(int i = 0; i < NUM_DEL_ITEMS_BATTERY; i++) {
                    if (text_battery[i] != nullptr) {
                        vlfGuiRemoveText(text_battery[i]);
                        text_battery[i] = nullptr;
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
        
        text_system[0] = GUI::Printf(10, 45, trans->system.fw, SystemInfo::GetFirmware(&menu_item.devkit));
        text_system[1] = GUI::Printf(10, 65, trans->system.button_assign);
        
        if (button_assign) {
            pic_button_assign = vlfGuiAddPictureResource("system_plugin_fg.rco", "tex_cross", 4, -2);
        }
        else {
            pic_button_assign = vlfGuiAddPictureResource("system_plugin_fg.rco", "tex_circle", 4, -2);
        }
        
        vlfGuiSetPictureXY(pic_button_assign, 131, 68);
        
        char unicode_username[26];
        Utils::UTF8ToUnicode(reinterpret_cast<wchar_t *>(unicode_username), reinterpret_cast<char *>(Utils::GetRegistryValue("/CONFIG/SYSTEM", "owner_name", &username, sizeof(username), 0)));
        
        text_system[2] = GUI::Printf(237, 45, trans->system.username);
        text_system[3] = vlfGuiAddTextW(language == PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN ? 337 : 327, 45, reinterpret_cast<u16 *>(unicode_username));
        text_system[4] = GUI::Printf(237, 65, trans->system.password, Utils::GetRegistryValue("/CONFIG/SYSTEM/LOCK", "password", &password, sizeof(password), 0));
        text_system[5] = GUI::Printf(10, 120, "version.txt:");
        
        if (menu_item.vertxt != nullptr) {
            text_system[6] = vlfGuiAddTextF(10, 143, menu_item.vertxt);
        }
        else {
            text_system[6] = GUI::Printf(10, 143, trans->system.vertxterr);
        }
        
        vlfGuiSetTextFontSize(text_system[6], 0.75f);
        GUI::SetBottomDialog(false, true, Menus::SystemInfoHandler, true);
        GUI::SetFade();
    }
    
    void ConsoleIdInfo(void) {
        GUI::SetTitle("Console ID Information");

        text_consoleId[0] = GUI::Printf(10, 40, "PSID: %s", ConsoleIdInfo::GetPSID(&menu_item.psid));
        
        text_consoleId[1] = GUI::Printf(10, 60, "Company Code: %d", menu_item.pscode.companyCode);
        
        text_consoleId[2] = GUI::Printf(10, 90, "Factory Code: %d", menu_item.pscode.factoryCode);
        text_consoleId[3] = GUI::Printf(10, 110, ConsoleIdInfo::GetFactoryCodeInfo(menu_item.pscode.factoryCode));
        
        text_consoleId[4] = GUI::Printf(10, 140, "Product Code: 0x%04X", menu_item.pscode.productCode);
        text_consoleId[5] = GUI::Printf(10, 160, ConsoleIdInfo::GetProductCodeInfo(menu_item.pscode.productCode));
        
        text_consoleId[6] = GUI::Printf(10, 190, "Product Sub Code: 0x%04X", menu_item.pscode.productSubCode);
        text_consoleId[7] = GUI::Printf(10, 210, ConsoleIdInfo::GetProductSubCodeInfo(menu_item.pscode.productSubCode));
        
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
                for(int i = 0; i < NUM_DEL_ITEMS_MISC; i++) {
                    if (text_misc[i] != nullptr) {
                        vlfGuiRemoveText(text_misc[i]);
                        text_misc[i] = nullptr;
                    }
                }
                
                text_misc[0] = GUI::Printf(10, 40, "Headphone status: %s", MiscInfo::GetHeadphoneStatus());
            }

            if (!update) {
                GUI::SetBottomDialog(false, true, Menus::MiscInfoHandler, false);
                GUI::SetFade();
            }
            
            vlfGuiDrawFrame();
            
            if (clear) {
                for(int i = 0; i < NUM_DEL_ITEMS_MISC; i++) {
                    if (text_misc[i] != nullptr) {
                        vlfGuiRemoveText(text_misc[i]);
                        text_misc[i] = nullptr;
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
        GUI::SetTitle("PSP EVEREST 2 Rev 9");
        
        const char *main_menu_items[] = {
            "Hardware Information",
            "Battery Information",
            "Software Information",
            "Console ID Information",
            "Miscellaneous Information",
            "Exit"
        };
        
        vlfGuiCentralMenu(NUM_DEL_ITEMS_MAIN, main_menu_items, select, Menus::MainMenuHandler, 0, 0);
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
        
    *(u32 *)menu_item.kirk = pspGetKirkVersion();
    *(u32 *)menu_item.spock = pspGetSpockVersion();
    menu_item.tachyon = pspGetTachyonVersion();
    menu_item.fuseid = pspGetFuseId();
    menu_item.fusecfg = pspGetFuseConfig();
    menu_item.scramble = pspNandGetScramble();
    pspGetBaryonVersion(&menu_item.baryon);
    pspGetPommelVersion(&menu_item.pommel);
    pspGetPolestarVersion(&menu_item.polestar);
    menu_item.devkit = sceKernelDevkitVersion();
    pspGetInitialFW(menu_item.initial_fw);
    pspChkregGetPsCode(&menu_item.pscode);
    sceOpenPSIDGetOpenPSID(&menu_item.psid);
    Utils::GetRegistryValue("/CONFIG/SYSTEM/XMB", "button_assign", &button_assign, 4, 1);
    menu_item.vertxt = SystemInfo::GetVersionTxt();
    
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
