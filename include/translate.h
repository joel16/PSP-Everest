#ifndef __TRANSLATE_H__
#define __TRANSLATE_H__

struct HardwareInformation {
    const char *model;
    const char *no_model;
    const char *mobo;
    const char *region;
    const char *gen;
    const char *eeprom;
    const char *pandora;
    const char *initialfw;
    const char *umdfw;
    const char *nandsize;
    const char *regname[11];
};

struct BatteryInformation {
    const char *ex_power;
    const char *batt_stat;
    const char *batt_stat_present;
    const char *batt_stat_absent;
    const char *charging;
    const char *charging_cpl;
    const char *no_charge_lvl;
    const char *charge_lvl;
    const char *charge_stat;
    const char *charge_stat_low;
    const char *charge_stat_normal;
    const char *charge_stat_strong;
    const char *no_left_time;
    const char *left_time;
    const char *no_voltage;
    const char *voltage;
    const char *no_temperature;
    const char *temperature;
    const char *no_remain_capacity;
    const char *remain_capacity;
    const char *no_total_capacity;
    const char *total_capacity;
    const char *serial;
    const char *no_serial;
    const char *mode;
    const char *mode_service;
    const char *mode_autoboot;
    const char *mode_default;
};

struct SystemInformation {
    const char *fw;
    const char *button_assign;
    const char *username;
    const char *password;
    const char *vertxterr;
};

typedef struct _EverestTranslate {
    const char *yes;
    const char *no;
    
    const char *hardware_title;
    struct HardwareInformation hardware;
    
    const char *battery_title;
    struct BatteryInformation battery;
    
    const char *system_title;
    struct SystemInformation system;
    
    const char *exit;
} EverestTranslate;

extern EverestTranslate *trans;

void SetupTranslate(void);

#endif
