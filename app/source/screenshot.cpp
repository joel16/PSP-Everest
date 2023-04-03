#include <cstdlib>
#include <cstring>
#include <pspdisplay.h>
#include <pspiofilemgr.h>

namespace Screenshot {
    typedef struct __attribute__((packed)) {
        char id[2];
        u32 file_size;
        u32 reserved;
        u32 offset;
        u32 head_size;
        u32 width;
        u32 height;
        u16 planes;
        u16 bpp;
        u32 comp;
        u32 bitmap_size;
        u32 hres;
        u32 vres;
        u32 colors;
        u32 impcolors;
    } BitmapHeader;

    static constexpr int SCREEN_WIDTH = 480;
    static constexpr int SCREEN_HEIGHT = 272;
    static constexpr u8 BYTES_PER_PIXEL = 3;
    
    int WriteFile(SceUID fd, void *data, u32 length) {
        u32 bytes_written = 0;
        
        while(bytes_written < length) {
            int ret = sceIoWrite(fd, static_cast<u32 *>(data) + bytes_written, length - bytes_written);
            if (ret < 0) {
                bytes_written = -1;
                break;
            }
            
            bytes_written += ret;
        }
        
        return bytes_written;
    }
    
    int WriteBitmap(void *frame_addr, void *tmp_buf, const char *file) {
        BitmapHeader *bmp = { 0 };
        void *pixel_data = static_cast<u8 *>(tmp_buf) + sizeof(BitmapHeader);
        u8 *line = nullptr, *p = nullptr;
        SceUID fd = 0;
        
        bmp = static_cast<BitmapHeader *>(tmp_buf);
        memset(bmp, 0, sizeof(BitmapHeader));
        memcpy(bmp->id, "BM", sizeof(bmp->id));
        bmp->file_size = SCREEN_WIDTH * SCREEN_HEIGHT * BYTES_PER_PIXEL + sizeof(BitmapHeader);
        bmp->offset = sizeof(BitmapHeader);
        bmp->head_size = 0x28;
        bmp->width = SCREEN_WIDTH;
        bmp->height = SCREEN_HEIGHT;
        bmp->planes = 1;
        bmp->bpp = 24;
        bmp->bitmap_size = SCREEN_WIDTH * SCREEN_HEIGHT * BYTES_PER_PIXEL;
        bmp->hres = 2834;
        bmp->vres = 2834;
        
        line = static_cast<u8 *>(pixel_data);

        for(int h = SCREEN_HEIGHT - 1; h >= 0; h--) {
            p = static_cast<u8 *>(frame_addr) + (h * 512 * 4);
            
            for(int i = 0; i < SCREEN_WIDTH; i++) {
                line[(i * BYTES_PER_PIXEL) + 2] = p[i * 4];
                line[(i * BYTES_PER_PIXEL) + 1] = p[(i * 4) + 1];
                line[(i * BYTES_PER_PIXEL) + 0] = p[(i * 4) + 2];
            }
            
            line += SCREEN_WIDTH * BYTES_PER_PIXEL;
        }

        fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

        if (fd >= 0) {
            Screenshot::WriteFile(fd, tmp_buf, bmp->file_size);
            sceIoClose(fd);
            return 0;
        }
        
        return -1;
    }

    void Save(const char *path) {
        void *framebuf = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * BYTES_PER_PIXEL + sizeof(BitmapHeader));
        if (!framebuf) {
            return;
        }

        u32 *vram32 = nullptr;
        sceDisplayWaitVblankStart();
        sceDisplayGetFrameBuf(reinterpret_cast<void **>(&vram32), nullptr, nullptr, 0);
        Screenshot::WriteBitmap(vram32, framebuf, path);
        free(framebuf);
    }

    bool DirExists(const char *path) {
        SceUID dir = sceIoDopen(path);
        if (dir >= 0) {
            sceIoDclose(dir);
            return true;
        }

        return false;
    }

    void Capture(int menu) {
        if (!Screenshot::DirExists("ms0:/PSP/PHOTO/")) {
            sceIoMkdir("ms0:/PSP/PHOTO", 0777);
        }

        if (!Screenshot::DirExists("ms0:/PSP/PHOTO/PSP-Everest/")) {
            sceIoMkdir("ms0:/PSP/PHOTO/PSP-Everest", 0777);
        }

        switch (menu) {
            case 0:
                Screenshot::Save("ms0:/PSP/PHOTO/PSP-Everest/hardware_info.bmp");
                break;

            case 1:
                Screenshot::Save("ms0:/PSP/PHOTO/PSP-Everest/battery_info.bmp");
                break;

            case 2:
                Screenshot::Save("ms0:/PSP/PHOTO/PSP-Everest/system_info.bmp");
                break;

            case 3:
                Screenshot::Save("ms0:/PSP/PHOTO/PSP-Everest/consoleid_info.bmp");
                break;
        }
    }
}
