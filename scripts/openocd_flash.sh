#!/bin/sh

cd ..

make

if [ $? == 0 ]; then
    ./scripts/openocd_standalone/bin/openocd -f "interface/stlink.cfg" -f "target/stm32f0x.cfg" -c "adapter_khz 8000" -c "reset_config srst_only" -c "program demo_os_smartcard.elf verify reset exit"
fi

