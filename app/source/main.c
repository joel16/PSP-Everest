//>>> PSP_EVEREST 2
//Copyright(C) 2022, frostegater, Joel16

#include <pspctrl.h>
#include <pspidstorage.h>
#include <pspkernel.h>
#include <psppower.h>
#include <pspsdk.h>
#include <psputility_sysparam.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "consoleid_utils.h"
#include "everest_kernel.h"
#include "hardware_utils.h"
#include "main.h"
#include "system_utils.h"
#include "translate.h"
#include "utils.h"
#include "vlf.h"

PSP_MODULE_INFO("PSP EVEREST 2 Rev6", 0, 2, 0);
PSP_MAIN_THREAD_ATTR(0);

#define NUM_DEL_ITEMS_MAIN      5
#define NUM_DEL_ITEMS_HARDWARE  19
#define NUM_DEL_ITEMS_BATTERY   14
#define NUM_DEL_ITEMS_SYSTEM    7
#define NUM_DEL_ITEMS_CONSOLEID 8

#define EVE_ENTER_EN "Enter"
#define EVE_BACK_EN "Back"

extern unsigned char backgrounds_bmp_start[];
extern unsigned int backgrounds_bmp_size;

VlfText main_menu[NUM_DEL_ITEMS_MAIN], text_hardware[NUM_DEL_ITEMS_HARDWARE], text_battery[NUM_DEL_ITEMS_BATTERY],
    text_system[NUM_DEL_ITEMS_SYSTEM], text_consoleId[NUM_DEL_ITEMS_CONSOLEID], title_text;
VlfPicture title_pic, pic_button_assign;

static int background_number = 0, max_background_number = 0;
static bool battery_break = false, battery_fade_ctrl = false;
static unsigned int button_assign = 0;
static char initial_fw[8], kirk[4], spock[4];
static u32 fusecfg = 0, scramble = 0;
static u16 bserialdata[2], serialdata[2];
static u64 fuseid = 0;
static ScePsCode pscode = { 0 };
static char *vertxt = NULL;
static PspOpenPSID psid = { 0 };

int psp_model = 0, devkit = 0, language = 0;
s32 tachyon = 0, baryon = 0, pommel = 0, polestar = 0;

void MainMenu(int select);

void SetBottomDialog(int enter, int back, int (* handler)(int enter), int delete_bd) {
    if (delete_bd) {
        if (button_assign)
            vlfGuiBottomDialog(back ? VLF_DI_BACK : -1, enter ? VLF_DI_ENTER : -1, 1, 0, 0, NULL);
            
        vlfGuiCancelBottomDialog();
    }
    
    vlfGuiBottomDialog(back ? VLF_DI_BACK : -1, enter ? VLF_DI_ENTER : -1, 1, 0, VLF_DEFAULT, handler);
}

void SetTitle(const char *text) {
    if (title_text != NULL)
        vlfGuiRemoveText(title_text);
        
    if (title_pic != NULL)
        vlfGuiRemovePicture(title_pic);
        
    title_text = pspEverestPrintf(30, 1, text);
    title_pic = vlfGuiAddPictureResource("ps3scan_plugin.rco", "tex_infobar_icon", 4, -2);
    vlfGuiSetTitleBarEx(title_text, title_pic, 1, 0, background_number);
}

void SetFade(void) {
    if (pic_button_assign != NULL)
        vlfGuiSetPictureFade(pic_button_assign, VLF_FADE_MODE_IN, VLF_FADE_SPEED_FAST, 0);
        
    vlfGuiSetRectangleFade(0, VLF_TITLEBAR_HEIGHT, 480, 272 - VLF_TITLEBAR_HEIGHT, VLF_FADE_MODE_IN, VLF_FADE_SPEED_FAST, 0, NULL, NULL, 0);
}

void SetBackground(void) {
    if (background_number < 0)
        background_number = max_background_number;
    else if (background_number > max_background_number)
        background_number = 0;
        
    vlfGuiSetBackgroundFileBuffer(backgrounds_bmp_start + background_number * 6176, 6176, 1);
    SetFade();
}

int ExitInMainMenuHardwareInfo(int enter) {
    if (!enter) {
        for(int i = 0; i < NUM_DEL_ITEMS_HARDWARE; i++)
            vlfGuiRemoveText(text_hardware[i]);
            
        if (!button_assign)
            vlfGuiBottomDialog(-1, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, NULL);
            
        vlfGuiCancelBottomDialog();
        MainMenu(0);
    }
    
    return VLF_EV_RET_NOTHING;
}

int ExitInMainMenuBatteryInfo(int enter) {
    if (!enter) {
        battery_break = true;

        if (!button_assign)
            vlfGuiBottomDialog(-1, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, NULL);
            
        vlfGuiCancelBottomDialog();
        MainMenu(1);
    }
    
    return VLF_EV_RET_NOTHING;
}

int ExitInMainMenuSystemInfo(int enter) {
    if (!enter) {
        for(int i = 0; i < NUM_DEL_ITEMS_SYSTEM; i++) {
            if (text_system[i] != NULL) {
                vlfGuiRemoveText(text_system[i]);
                text_system[i] = NULL;
            }
        }
        
        if (pic_button_assign != NULL) {
            vlfGuiRemovePicture(pic_button_assign);
            pic_button_assign = NULL;
        }
        
        if (!button_assign)
            vlfGuiBottomDialog(-1, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, NULL);
            
        vlfGuiCancelBottomDialog();
        MainMenu(2);
    }
    
    return VLF_EV_RET_NOTHING;
}

int ExitInMainMenuConsoleIdInfo(int enter) {
    if (!enter) {
        for(int i = 0; i < NUM_DEL_ITEMS_CONSOLEID; i++)
            vlfGuiRemoveText(text_consoleId[i]);
            
        if (!button_assign)
            vlfGuiBottomDialog(-1, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, NULL);
            
        vlfGuiCancelBottomDialog();
        MainMenu(3);
    }
    
    return VLF_EV_RET_NOTHING;
}

void HardwareInfo(void) {
    SetTitle(trans->hardware_title);
    
    text_hardware[0] = pspEverestPrintf(10, 40, "Tachyon: 0x%08X", tachyon);
    text_hardware[1] = pspEverestPrintf(10, 60, "Baryon: 0x%08X", baryon);
    text_hardware[2] = pspEverestPrintf(10, 80, "Pommel: 0x%08X", pommel);
    text_hardware[3] = pspEverestPrintf(10, 100, "Polestar: 0x%08X", polestar);
    text_hardware[4] = pspEverestPrintf(10, 120, "FuseID: 0x%llX", fuseid);
    text_hardware[5] = pspEverestPrintf(10, 140, "FuseCFG: 0x%08X", fusecfg);
    text_hardware[6] = pspEverestPrintf(10, 160, "IDScramble: 0x%08X", scramble);
    text_hardware[7] = pspEverestPrintf(10, 180, "Kirk: %c%c%c%c", kirk[3], kirk[2], kirk[1], kirk[0]);
    text_hardware[8] = pspEverestPrintf(10, 200, psp_model == 4 ? "Spock: -" : "Spock: %c%c%c%c", spock[3], spock[2], spock[1], spock[0]);
    text_hardware[9] = pspEverestPrintf(10, 220, pspGetFirstSymbolOfModel() != -1 ? trans->hardware.model : trans->hardware.no_model, psp_model == 4 ? "N" : psp_model == 10 ? "E" : "", pspGetFirstSymbolOfModel(), pspGetRegion() < 10 ? "0" : "", pspGetRegion(), pspGetModelName());
    
    text_hardware[10] = pspEverestPrintf(250, 40, trans->hardware.mobo, pspGetMoBoName());
    text_hardware[11] = pspEverestPrintf(250, 60, trans->hardware.region, pspGetRegionName());
    text_hardware[12] = pspEverestPrintf(250, 80, trans->hardware.gen, psp_model < 10 ? "0" : "", psp_model + 1);
    text_hardware[13] = pspEverestPrintf(250, 100, trans->hardware.eeprom, tachyon <= 0x00500000 && tachyon != 0x00100000 && baryon <= 0x0022B200 ? trans->yes : trans->no);
    text_hardware[14] = pspEverestPrintf(250, 120, trans->hardware.pandora, tachyon <= 0x00500000 ? trans->yes : trans->no);
    text_hardware[15] = pspEverestPrintf(250, 140, "MAC: %s", pspGetMacAddressText());
    text_hardware[16] = pspEverestPrintf(250, 160, trans->hardware.initialfw, initial_fw);
    text_hardware[17] = pspEverestPrintf(250, 180, trans->hardware.umdfw, psp_model == 4 ? "-" : pspGetUMDFWText());
    text_hardware[18] = pspEverestPrintf(250, 200, trans->hardware.nandsize, (pspNandGetPageSize() * pspNandGetPagesPerBlock() * pspNandGetTotalBlocks()) / 1024 / 1024);
    
    SetBottomDialog(0, 1, ExitInMainMenuHardwareInfo, 1);
    SetFade();
}

void BatteryInfo(void) {
    bool swbt = true, checkbt = false;
    int elec = 0, total_elec = 0;
    
    if (button_assign)
        vlfGuiBottomDialog(VLF_DI_BACK, -1, 1, 0, 0, NULL);
        
    vlfGuiCancelBottomDialog();
    SetTitle(trans->battery_title);
    
    battery_fade_ctrl = false;
    
    bool first_cycle = true;
    for(int update = 0;; update++) {
        if (update == 25 || battery_fade_ctrl) {
            update = 1;
            first_cycle = battery_fade_ctrl;
            battery_fade_ctrl = false;
        }
        
        vlfGuiDrawFrame();
        
        if (!first_cycle || update == 0) {
            for(int i = 0; i < NUM_DEL_ITEMS_BATTERY; i++) {
                if (text_battery[i] != NULL) {
                    vlfGuiRemoveText(text_battery[i]);
                    text_battery[i] = NULL;
                }
            }
            
            int battery_percent = scePowerGetBatteryLifePercent();
            int battery_life_time = scePowerGetBatteryLifeTime();
            
            if (update != 0 && scePowerIsBatteryExist() && swbt && (psp_model == 0 || (tachyon <= 0x00500000 && baryon == 0x0022B200))) {
                pspReadSerial(bserialdata);
                
                u16 wrbuffer[0x80];
                wrbuffer[0] = 0x5053;
                
                if (bserialdata[0] != 0x5058)
                    wrbuffer[1] = 0x5058;
                else
                    wrbuffer[1] = 0x4456;
                    
                pspWriteSerial(wrbuffer);
                
                checkbt = false;
                pspReadSerial(serialdata);
                
                if (serialdata[0] == wrbuffer[0] && serialdata[1] == wrbuffer[1]) {
                    checkbt = true;
                    pspWriteSerial(bserialdata);
                }
                
                swbt = false;
            }
            
            if (!swbt && !scePowerIsBatteryExist())
                swbt = true;
                
            text_battery[0] = pspEverestPrintf(15, 70, trans->battery.ex_power, psp_model == 4 ? "-" : scePowerIsPowerOnline() ? trans->yes : trans->no);
            text_battery[1] = pspEverestPrintf(15, 90, trans->battery.batt_stat,  scePowerIsBatteryExist() ? trans->battery.batt_stat_present : trans->battery.batt_stat_absent);
            text_battery[2] = pspEverestPrintf(15, 110, trans->battery.charging, scePowerIsBatteryCharging() ? scePowerIsBatteryExist() ? battery_percent == 100 ? trans->battery.charging_cpl : trans->yes : trans->no : trans->no);
            text_battery[3] = pspEverestPrintf(15, 130, battery_percent > 100 || battery_percent < 0 ? trans->battery.no_charge_lvl : trans->battery.charge_lvl, battery_percent);
            text_battery[4] = pspEverestPrintf(15, 150, trans->battery.charge_stat, battery_percent >= 0 && battery_percent <= 20 ? trans->battery.charge_stat_low : battery_percent > 20 && battery_percent < 70 ? trans->battery.charge_stat_normal : battery_percent >= 70 && battery_percent <= 100 ? trans->battery.charge_stat_strong : "-");
            text_battery[5] = pspEverestPrintf(15, 170, battery_life_time < 0 || battery_life_time / 60 > 100 || (battery_life_time / 60 == 0 && battery_life_time - (battery_life_time / 60 * 60) == 0) ? trans->battery.no_left_time : trans->battery.left_time, battery_life_time / 60, battery_life_time - (battery_life_time / 60 * 60));
            text_battery[6] = pspEverestPrintf(15, 190, pspSysconBatteryGetElec(&elec) < 0? "Elec Charge: -" : "Elec Charge: %d", elec);
            
            text_battery[7] = pspEverestPrintf(240, 70, scePowerGetBatteryVolt() <= 0 ? trans->battery.no_voltage : trans->battery.voltage, (float)scePowerGetBatteryVolt() / 1000.0);
            text_battery[8] = pspEverestPrintf(240, 90, scePowerGetBatteryTemp() <= 0 ? trans->battery.no_temperature : trans->battery.temperature, scePowerGetBatteryTemp());
            text_battery[9] = pspEverestPrintf(240, 110, scePowerGetBatteryRemainCapacity() <= 0 ? trans->battery.no_remain_capacity : trans->battery.remain_capacity, scePowerGetBatteryRemainCapacity());
            text_battery[10] = pspEverestPrintf(240, 130, scePowerGetBatteryFullCapacity() <= 0 ? trans->battery.no_total_capacity : trans->battery.total_capacity, scePowerGetBatteryFullCapacity());
            text_battery[11] = pspEverestPrintf(240, 150, scePowerIsBatteryExist() && (psp_model == 0 || (tachyon == 0x00500000 && baryon == 0x0022B200)) && checkbt ? trans->battery.serial : trans->battery.no_serial, bserialdata[0], bserialdata[1]);	
            text_battery[12] = pspEverestPrintf(240, 170, trans->battery.mode, 
                checkbt && scePowerIsBatteryExist() && bserialdata[0] == 0xFFFF && bserialdata[1] == 0xFFFF && (psp_model == 0 || (tachyon <= 0x00500000 && baryon == 0x0022B200)) ? trans->battery.mode_service :
                checkbt && scePowerIsBatteryExist() && bserialdata[0] == 0x0000 && bserialdata[1] == 0x0000 && (psp_model == 0 || (tachyon <= 0x00500000 && baryon == 0x0022B200)) ? trans->battery.mode_autoboot :
                checkbt && scePowerIsBatteryExist() && (psp_model == 0 || (tachyon <= 0x00500000 && baryon == 0x0022B200)) ? trans->battery.mode_default : "-");
            text_battery[13] = pspEverestPrintf(240, 190, pspSysconBatteryGetTotalElec(&total_elec) < 0? "Total Elec Charge: -" : "Total Elec Charge: %d", total_elec);
        }
        
        if (!update) {
            SetBottomDialog(0, 1, ExitInMainMenuBatteryInfo, 0);
            SetFade();
        }
        
        vlfGuiDrawFrame();

        if (battery_break) {
            for(int i = 0; i < NUM_DEL_ITEMS_BATTERY; i++) {
                if (text_battery[i] != NULL) {
                    vlfGuiRemoveText(text_battery[i]);
                    text_battery[i] = NULL;
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
    
    SetTitle(trans->system_title);
    
    text_system[0] = pspEverestPrintf(10, 45, trans->system.fw, pspGetFirmwareName());
    text_system[1] = pspEverestPrintf(10, 65, trans->system.button_assign);
    
    if (button_assign)
        pic_button_assign = vlfGuiAddPictureResource("system_plugin_fg.rco", "tex_cross", 4, -2);
    else
        pic_button_assign = vlfGuiAddPictureResource("system_plugin_fg.rco", "tex_circle", 4, -2);
        
    vlfGuiSetPictureXY(pic_button_assign, 131, 68);
    
    char unicode_username[26];
    utf82unicode((wchar_t *)unicode_username, (char *)GetRegistryValue("/CONFIG/SYSTEM", "owner_name", &username, sizeof(username), 0));
    
    text_system[2] = pspEverestPrintf(237, 45, trans->system.username);
    text_system[3] = vlfGuiAddTextW(language == PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN ? 337 : 327, 45, (u16 *)unicode_username);
    text_system[4] = pspEverestPrintf(237, 65, trans->system.password, GetRegistryValue("/CONFIG/SYSTEM/LOCK", "password", &password, sizeof(password), 0));
    text_system[5] = pspEverestPrintf(10, 120, "version.txt:");

    if (vertxt != NULL)
        text_system[6] = vlfGuiAddTextF(10, 143, pspGetVersionTxt());
    else
        text_system[6] = pspEverestPrintf(10, 143, trans->system.vertxterr);
        
    vlfGuiSetTextFontSize(text_system[6], 0.75f);
    SetBottomDialog(0, 1, ExitInMainMenuSystemInfo, 1);
    SetFade();
}

void ConsoleIdInfo(void) {
    text_consoleId[0] = pspEverestPrintf(10, 40, "PSID: %s", pspGetPSID(&psid));

    text_consoleId[1] = pspEverestPrintf(10, 60, "Company Code: %d", pscode.companyCode);
    
    text_consoleId[2] = pspEverestPrintf(10, 90, "Factory Code: %d", pscode.factoryCode);
    text_consoleId[3] = pspEverestPrintf(10, 110, pspGetFactoryCodeInfo(pscode.factoryCode));

    text_consoleId[4] = pspEverestPrintf(10, 140, "Product Code: 0x%04X", pscode.productCode);
    text_consoleId[5] = pspEverestPrintf(10, 160, pspGetProductCodeInfo(pscode.productCode));

    text_consoleId[6] = pspEverestPrintf(10, 190, "Product Sub Code: 0x%04X", pscode.productSubCode);
    text_consoleId[7] = pspEverestPrintf(10, 210, pspGetProductSubCodeInfo(pscode.productSubCode));

    SetBottomDialog(0, 1, ExitInMainMenuConsoleIdInfo, 1);
    SetFade();
}

int OnMainMenu(int select) {
    switch(select) {
        case 0:
            vlfGuiCancelCentralMenu();
            HardwareInfo();
            break;
        
        case 1:
            vlfGuiCancelCentralMenu();
            battery_break = false;
            BatteryInfo();
            break;

        case 2:
            vlfGuiCancelCentralMenu();
            SystemInfo();
            break;

        case 3:
            vlfGuiCancelCentralMenu();
            ConsoleIdInfo();
            break;

        case 4:
            sceKernelExitGame();
            break;
    }

    return VLF_EV_RET_NOTHING;
}

void MainMenu(int select) {
    SetTitle("PSP EVEREST 2 Rev 6");
    
    char *main_menu_items[] = {
        "Hardware Information",
        "Battery Information",
        "Software Information",
        "Console ID Information",
        "Exit"
    };

    vlfGuiCentralMenu(NUM_DEL_ITEMS_MAIN, main_menu_items, select, OnMainMenu, 0, 0);
    SetBottomDialog(1, 0, OnMainMenu, 0);
}

int app_main(int argc, char *argv[]) {
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &language);
    SetupTranslate();
    
    if (language == PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN)
        vlfGuiSetLanguage(PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN);
    else
        vlfGuiSetLanguage(PSP_SYSTEMPARAM_LANGUAGE_ENGLISH);
        
    *(u32 *)kirk = pspGetKirkVersion();
    *(u32 *)spock = pspGetSpockVersion();
    tachyon = pspGetTachyonVersion();
    fuseid = pspGetFuseId();
    fusecfg = pspGetFuseConfig();
    scramble = pspNandGetScramble();
    pspGetBaryonVersion(&baryon);
    pspGetPommelVersion(&pommel);
    pspGetPolestarVersion(&polestar);
    devkit = sceKernelDevkitVersion();
    pspGetInitialFW(initial_fw);
    pspChkregGetPsCode(&pscode);
    sceOpenPSIDGetOpenPSID(&psid);
    GetRegistryValue("/CONFIG/SYSTEM/XMB", "button_assign", &button_assign, 4, 1);
    vertxt = pspGetVersionTxt();
    
    vlfGuiSystemSetup(1, 1, 1);
    
    int OnBackgroundPlus(void *param) {
        background_number++;
        battery_fade_ctrl = true;
        SetBackground();
        return VLF_EV_RET_NOTHING;
    }
    
    int OnBackgroundMinus(void *param) {
        background_number--;
        battery_fade_ctrl = true;
        SetBackground();
        return VLF_EV_RET_NOTHING;
    }
    
    vlfGuiAddEventHandler(PSP_CTRL_RTRIGGER, -1, OnBackgroundPlus, NULL);
    vlfGuiAddEventHandler(PSP_CTRL_LTRIGGER, -1, OnBackgroundMinus, NULL);
    
    max_background_number = backgrounds_bmp_size / 6176 - 1;
    background_number = Random(0, max_background_number);
    
    SetBackground();
    MainMenu(0);
    
    while(1)
        vlfGuiDrawFrame();
        
    return 0;
}
