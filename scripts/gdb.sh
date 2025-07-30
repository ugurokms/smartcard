#!/bin/sh

cd ..

GDB=gdb-multiarch
# GDB=arm-none-eabi-gdb

make

if [ $? == 0 ]; then
    $GDB -tui demo_os_smartcard.elf -x ./scripts/gdb_commands_dbg
fi


