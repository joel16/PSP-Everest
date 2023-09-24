TARGET = EVEREST

SOURCES   := data drivers source
CFILES    := $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES  := $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.cpp))
SFILES    := $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.S))
GFXFILES  := $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.bmp))
PRXFILES  := $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.prx))

OBJS := $(addsuffix .o,$(BINFILES)) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(SFILES:.S=.o) \
        $(GFXFILES:.bmp=.o) $(PRXFILES:.prx=.o)

INCDIR   = ../libs/ ../libs/include include
CFLAGS = -O2 -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -std=gnu++17
ASFLAGS = $(CFLAGS) -c

BUILD_PRX = 1
PSP_LARGE_MEMORY = 1

LIBDIR  = ../libs/lib
LDFLAGS = -nostdlib -nodefaultlibs
LIBS = -lpspmodinfo -lpspopenpsid -lpsprtc -lpsppower -lpspreg -lvlfgui -lvlfgu -lvlfutils -lvlflibc

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSP EVEREST 2 Rev 8
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.png

PSPSDK=$(shell psp-config --pspsdk-path)
include ./build.mak

%.o: %.bmp
	bin2o -i $< $@ $(addsuffix _bmp, $(basename $(notdir $<) ))

%.o: %.prx
	bin2o -i $< $@ $(addsuffix _prx, $(basename $(notdir $<) ))
