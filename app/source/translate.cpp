
#include <psputility_sysparam.h>
#include "translate.h"
#include "main.h"

/* English Translate*/
EverestTranslate ENtrans =
{
	.yes = "Yes",
	.no = "No",
	.hardware_title = "Hardware Information",
	.hardware =
	{
		.model = "Model: %s%d0%s%d (%s)",
		.no_model = "Model: -",
		.mobo = "Motherboard: %s",
		.region = "Region: %s",
		.gen = "Generation: %s%dg",
		.eeprom = "EEPROM access: %s",
		.pandora = "Pandora access: %s",
		.initialfw = "Initial Firmware: %s",
		.umdfw = "UMD� Firmware: %s",
		.nandsize = "NAND Size: %i MB",
		.regname =
		{
			"Japan",
			"America",
			"Australia",
			"United Kingdom",
			"Europe",
			"Korea",
			"Hong-Kong",
			"Taiwan",
			"Russia",
			"China",
			"Mexico"
		}
	},
	.battery_title = "Battery Information",
	.battery =
	{
		.ex_power = "External Power: %s",
		.batt_stat = "Battery Status: %s",
		.batt_stat_present = "Present",
		.batt_stat_absent = "Absent",
		.charging = "Charging: %s",
		.charging_cpl = "Complete",
		.no_charge_lvl = "Charge Level: -",
		.charge_lvl = "Charge Level: %d%%",
		.charge_stat = "Charge Status: %s",
		.charge_stat_low = "Low",
		.charge_stat_normal = "Normal",
		.charge_stat_strong = "Strong",
		.no_left_time = "Left Time: -",
		.left_time = "Left Time: %d:%02d'",
		.no_voltage = "Voltage: -",
		.voltage = "Voltage: %0.2fV",
		.no_temperature = "Temperature: -",
		.temperature = "Temperature: %d'C",
		.no_remain_capacity = "Remain Capacity: -",
		.remain_capacity = "Remain Capacity: %i mAh",
		.no_total_capacity = "Total Capacity: -",
		.total_capacity = "Total Capacity: %i mAh",
		.serial = "Serial: 0x%04X%04X",
		.no_serial = "Serial: -",
		.mode = "Mode: %s",
		.mode_service = "Service",
		.mode_autoboot = "Autoboot",
		.mode_default = "Default"
	},
	.system_title = "System Information",
	.system =
	{
		.fw = "Firmware: %s",
		.button_assign = "Button Assign:",
		.username = "Username:",
		.password = "Password: %s",
		.vertxterr = "Error in open version.txt: 0x%08X."
	},
	.exit = "Exit"
};

/* Russian Translate */
EverestTranslate RUtrans =
{
	.yes = "��",
	.no = "���",
	.hardware_title = "���������� � ������",
	.hardware =
	{
		.model = "������: %s%d0%s%d (%s)",
		.no_model = "������: -",
		.mobo = "���. �����: %s",
		.region = "������: %s",
		.gen = "������: %s%dg",
		.eeprom = "������ �  EEPROM: %s",
		.pandora = "������ �������: %s",
		.initialfw = "��������� ��������: %s",
		.umdfw = "�������� UMD�: %s",
		.nandsize = "������ NAND: %i ��",
		.regname =
		{
			"������",
			"�������",
			"���������",
			"������",
			"������",
			"�����",
			"����-����",
			"�������",
			"������",
			"�����",
			"�������"
		}
	},
	.battery_title = "���������� � �������",
	.battery =
	{
		.ex_power = "������� �������: %s",
		.batt_stat = "������� �������: %s",
		.batt_stat_present = "��",
		.batt_stat_absent = "���",
		.charging = "�������: %s",
		.charging_cpl = "���������",
		.no_charge_lvl = "������� ������: -",
		.charge_lvl = "������� ������: %d%%",
		.charge_stat = "������ ������: %s",
		.charge_stat_low = "������",
		.charge_stat_normal = "�������",
		.charge_stat_strong = "�������",
		.no_left_time = "���������� �����: -",
		.left_time = "���������� �����: %d:%02d'",
		.no_voltage = "����������: -",
		.voltage = "����������: %0.2fV",
		.no_temperature = "�����������: -",
		.temperature = "�����������: %d'C",
		.no_remain_capacity = "������� �������: -",
		.remain_capacity = "������� �������: %i ���",
		.no_total_capacity = "����� �������: -",
		.total_capacity = "����� �������: %i ���",
		.serial = "��������: 0x%04X%04X",
		.no_serial = "��������: -",
		.mode = "�����: %s",
		.mode_service = "���������",
		.mode_autoboot = "����������",
		.mode_default = "�������"
	},
	.system_title = "���������� � �������",
	.system =
	{
		.fw = "��������: %s",
		.button_assign = "������ �����:",
		.username = "���������:",
		.password = "������: %s",
		.vertxterr = "������ ��� �������� version.txt: 0x%08X."
	},
	.exit = "�����"
};

EverestTranslate *trans;

void SetupTranslate()
{
	if(language == PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN)
		trans = &RUtrans;
	else
		trans = &ENtrans;
}
