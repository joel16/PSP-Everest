TARGET = kernel
OBJS = kernel.o imports.o exports.o

PRX_EXPORTS = exports.exp

# Use the kernel's small inbuilt libc
USE_KERNEL_LIBC = 1
# Use only kernel libraries
USE_KERNEL_LIBS = 1

INCDIR = ../../app/include ../../libs/include
CFLAGS = -Os -G0 -Wall -fno-builtin-printf
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LDFLAGS = -nostartfiles
LIBS = -lpspkernel -lpspnand_driver -lpspwlan

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

all:
	psp-build-exports -s $(PRX_EXPORTS)
	mkdir "../../app/data/"; mv kernel.prx "../../app/data/"
	mkdir "../../app/drivers/"; mv kernel.S "../../app/drivers/"
