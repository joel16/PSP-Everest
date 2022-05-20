
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
		.umdfw = "UMD™ Firmware: %s",
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
	.yes = "Да",
	.no = "Нет",
	.hardware_title = "Информация о железе",
	.hardware =
	{
		.model = "Модель: %s%d0%s%d (%s)",
		.no_model = "Модель: -",
		.mobo = "Мат. плата: %s",
		.region = "Регион: %s",
		.gen = "Модуль: %s%dg",
		.eeprom = "Доступ к  EEPROM: %s",
		.pandora = "Работа пандоры: %s",
		.initialfw = "Заводская прошивка: %s",
		.umdfw = "Прошивка UMD™: %s",
		.nandsize = "Размер NAND: %i МБ",
		.regname =
		{
			"Япония",
			"Америка",
			"Австралия",
			"Англия",
			"Европа",
			"Корея",
			"Гонг-Конг",
			"Тайвань",
			"Россия",
			"Китай",
			"Мексика"
		}
	},
	.battery_title = "Информация о батарее",
	.battery =
	{
		.ex_power = "Внешнее питание: %s",
		.batt_stat = "Наличие батареи: %s",
		.batt_stat_present = "Да",
		.batt_stat_absent = "Нет",
		.charging = "Зарядка: %s",
		.charging_cpl = "Закончена",
		.no_charge_lvl = "Уровень заряда: -",
		.charge_lvl = "Уровень заряда: %d%%",
		.charge_stat = "Статус заряда: %s",
		.charge_stat_low = "Низкий",
		.charge_stat_normal = "Средний",
		.charge_stat_strong = "Высокий",
		.no_left_time = "Оставшееся время: -",
		.left_time = "Оставшееся время: %d:%02d'",
		.no_voltage = "Напряжение: -",
		.voltage = "Напряжение: %0.2fV",
		.no_temperature = "Температура: -",
		.temperature = "Температура: %d'C",
		.no_remain_capacity = "Текущая ёмкость: -",
		.remain_capacity = "Текущая ёмкость: %i мАч",
		.no_total_capacity = "Общая ёмкость: -",
		.total_capacity = "Общая ёмкость: %i мАч",
		.serial = "Серийник: 0x%04X%04X",
		.no_serial = "Серийник: -",
		.mode = "Режим: %s",
		.mode_service = "Сервисный",
		.mode_autoboot = "Автозапуск",
		.mode_default = "Обычный"
	},
	.system_title = "Информация о системе",
	.system =
	{
		.fw = "Прошивка: %s",
		.button_assign = "Кнопка ввода:",
		.username = "Псевдоним:",
		.password = "Пароль: %s",
		.vertxterr = "Ошибка при открытии version.txt: 0x%08X."
	},
	.exit = "Выход"
};

EverestTranslate *trans;

void SetupTranslate()
{
	if(language == PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN)
		trans = &RUtrans;
	else
		trans = &ENtrans;
}
