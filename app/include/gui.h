#pragma once

#include "vlf.h"

extern bool battery_fade_ctrl;
extern unsigned int button_assign;
extern VlfPicture pic_button_assign;

namespace GUI {
    void SetBottomDialog(int enter, int back, int (* handler)(int enter), int delete_bd);
    void SetTitle(const char *text);
    void SetFade(void);
    void GenRandomBackground(void);
    void SetBackground(void);
    int OnBackgroundPlus(void *param);
    int OnBackgroundMinus(void *param);
    VlfText Printf(int x, int y, const char *text, ...);
}
