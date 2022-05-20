TARGET = EVEREST
OBJS = source/crt0.o source/main.o source/utils.o source/hardware_utils.o source/system_utils.o \
    source/translate.o source/imports.o everest_kernel/everest_kernel.o kumdman/pspUmdMan_driver.o

INCDIR = libs/include include
CFLAGS = -O2 -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LDFLAGS = -nostdlib -nodefaultlibs

LIBDIR  = libs/lib
STDLIBS = -lpspmodinfo -lpsprtc -lvlfgui -lvlfgu -lvlfutils -lvlflibc -lpsppower -lpspkubridge
LIBS = $(STDLIBS) -lpspreg

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSP EVEREST 2 Rev6
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.png

BUILD_PRX = 1
PSP_LARGE_MEMORY = 1

PSPSDK = $(shell psp-config --pspsdk-path)
include ./build.mak
