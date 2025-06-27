#!/bin/sh

# In case of communication problems, try reducing the adapter_khz to 1000

./openocd_standalone/bin/openocd -f "interface/stlink.cfg" -f "target/stm32f0x.cfg" -c "adapter_khz 8000" -c "reset_config srst_only"
