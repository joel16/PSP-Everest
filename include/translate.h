#ifndef __TRANSLATE_H__
#define __TRANSLATE_H__

struct HardwareInformation {
    char *model;
    char *no_model;
    char *mobo;
    char *region;
    char *gen;
    char *eeprom;
    char *pandora;
    char *initialfw;
    char *umdfw;
    char *nandsize;
    char *regname[11];
};

struct BatteryInformation {
    char *ex_power;
    char *batt_stat;
    char *batt_stat_present;
    char *batt_stat_absent;
    char *charging;
    char *charging_cpl;
    char *no_charge_lvl;
    char *charge_lvl;
    char *charge_stat;
    char *charge_stat_low;
    char *charge_stat_normal;
    char *charge_stat_strong;
    char *no_left_time;
    char *left_time;
    char *no_voltage;
    char *voltage;
    char *no_temperature;
    char *temperature;
    char *no_remain_capacity;
    char *remain_capacity;
    char *no_total_capacity;
    char *total_capacity;
    char *serial;
    char *no_serial;
    char *mode;
    char *mode_service;
    char *mode_autoboot;
    char *mode_default;
};

struct SystemInformation {
    char *fw;
    char *button_assign;
    char *username;
    char *password;
    char *vertxterr;
};

typedef struct _EverestTranslate {
    char *yes;
    char *no;
    
    char *hardware_title;
    struct HardwareInformation hardware;
    
    char *battery_title;
    struct BatteryInformation battery;
    
    char *system_title;
    struct SystemInformation system;
    
    char *exit;
} EverestTranslate;

extern EverestTranslate *trans;

void SetupTranslate(void);

#endif
