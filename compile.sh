#!/bin/sh

cd everest_kernel
make clean && make
psp-build-exports -s exports.exp
bin2c everest_kernel.prx ../everest_kernel_prx.h everest_kernel_prx
rm *.o *.elf *.prx

cd ../kumdman
make clean && make
psp-build-exports -s exports.exp
bin2c kumdman.prx ../kumdman_prx.h kumdman_prx
rm *.o *.elf *.prx

cd ../PRXs
bin2c intraFont.prx ../intraFont_prx.h intraFont_prx
bin2c vlf.prx ../vlf_prx.h vlf_prx

cd ..
rm *.PBP
bin2c backgrounds.bmp backgrounds_bmp.h backgrounds_bmp
make clean && make
rm *.o *.prx *.elf *.SFO backgrounds_bmp.h everest_kernel_prx.h kumdman_prx.h intraFont_prx.h vlf_prx.h
rm everest_kernel/everest_kernel.S everest_kernel/everest_kernel.o kumdman/pspUmdMan_driver.S kumdman/pspUmdMan_driver.o
