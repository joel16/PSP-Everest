#include <pspsdk.h>
#include <stdarg.h>
#include <stdio.h>

#include "gui.h"
#include "utils.h"

bool battery_fade_ctrl = false;
unsigned int button_assign = 0;
VlfPicture pic_button_assign;

extern unsigned char backgrounds_bmp_start[];
extern unsigned int backgrounds_bmp_size;

namespace GUI {
    static int background_number = 0, max_background_number = 0;
    static VlfPicture title_pic;
    static VlfText title_text;

    void SetBottomDialog(int enter, int back, int (* handler)(int enter), int delete_bd) {
        if (delete_bd) {
            if (button_assign) {
                vlfGuiBottomDialog(back ? VLF_DI_BACK : -1, enter ? VLF_DI_ENTER : -1, 1, 0, 0, nullptr);
            }
            
            vlfGuiCancelBottomDialog();
        }
        
        vlfGuiBottomDialog(back ? VLF_DI_BACK : -1, enter ? VLF_DI_ENTER : -1, 1, 0, VLF_DEFAULT, handler);
    }
    
    void SetTitle(const char *text) {
        if (title_text != nullptr) {
            vlfGuiRemoveText(title_text);
        }

        if (title_pic != nullptr) {
            vlfGuiRemovePicture(title_pic);
        }
        
        title_text = GUI::Printf(30, 1, text);
        title_pic = vlfGuiAddPictureResource("ps3scan_plugin.rco", "tex_infobar_icon", 4, -2);
        vlfGuiSetTitleBarEx(title_text, title_pic, 1, 0, background_number);
    }
    
    void SetFade(void) {
        if (pic_button_assign != nullptr) {
            vlfGuiSetPictureFade(pic_button_assign, VLF_FADE_MODE_IN, VLF_FADE_SPEED_FAST, 0);
        }
        
        vlfGuiSetRectangleFade(0, VLF_TITLEBAR_HEIGHT, 480, 272 - VLF_TITLEBAR_HEIGHT, VLF_FADE_MODE_IN, VLF_FADE_SPEED_FAST, 0, nullptr, nullptr, 0);
    }

    void GenRandomBackground(void) {
        max_background_number = backgrounds_bmp_size / 6176 - 1;
        background_number = Utils::Rand(0, max_background_number);
    }
    
    void SetBackground(void) {
        if (background_number < 0) {
            background_number = max_background_number;
        }
        else if (background_number > max_background_number) {
            background_number = 0;
        }
        
        vlfGuiSetBackgroundFileBuffer(backgrounds_bmp_start + background_number * 6176, 6176, 1);
        GUI::SetFade();
    }
    
    int OnBackgroundPlus(void *param) {
        background_number++;
        battery_fade_ctrl = true;
        GUI::SetBackground();
        return VLF_EV_RET_NOTHING;
    }
    
    int OnBackgroundMinus(void *param) {
        background_number--;
        battery_fade_ctrl = true;
        GUI::SetBackground();
        return VLF_EV_RET_NOTHING;
    }
    
    VlfText Printf(int x, int y, const char *text, ...) {
        char ascii[256], unicode[256];
        va_list list;
        va_start(list, text);
        vsnprintf(ascii, 256, text, list);
        va_end(list);
        Utils::ASCIIToUnicode(unicode, ascii);
        return vlfGuiAddTextW(x, y, reinterpret_cast<u16 *>(unicode));
    }
}
