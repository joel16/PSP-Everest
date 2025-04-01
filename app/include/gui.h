#pragma once

#include "vlf.h"

extern bool fadeCtrl;
extern unsigned int button_assign;
extern VlfPicture pic_button_assign;

namespace GUI {
    void SetBottomDialog(bool enter, bool back, int (* handler)(int enter), bool clear);
    void SetTitle(const char *text);
    void SetFade(void);
    void GenRandomBackground(void);
    void SetBackground(void);
    int OnBackgroundPlus(void *param);
    int OnBackgroundMinus(void *param);
    VlfText Printf(int x, int y, const char *text, ...);
    void ClearText(VlfText* textList, int size);
}
