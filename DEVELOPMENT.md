# Development Guide

This guide will help you set up the development environment and build the smartcard project.

## Prerequisites

### Required Tools
1. **ARM GCC Toolchain**
   ```bash
   # On Windows, download from ARM Developer website
   # On Linux (Ubuntu/Debian):
   sudo apt-get install gcc-arm-none-eabi
   
   # On macOS:
   brew install arm-none-eabi-gcc
   ```

2. **OpenOCD** (for programming and debugging)
   ```bash
   # On Linux (Ubuntu/Debian):
   sudo apt-get install openocd
   
   # On Windows: Download from OpenOCD website
   # On macOS:
   brew install openocd
   ```

3. **Make Build System**
   ```bash
   # Usually pre-installed on Linux/macOS
   # On Windows: Install with MinGW or WSL
   ```

### Hardware Requirements
- STM32F051 development board or compatible hardware
- ST-Link programmer/debugger
- USB cables for programming and power

## Setup Instructions

### 1. Clone and Initialize
```bash
# Clone with submodules
git clone --recursive https://github.com/ugurokms/smartcard.git
cd smartcard

# If already cloned, initialize submodules
git submodule update --init --recursive
```

### 2. Build LibOpenCM3
```bash
cd libopencm3
make -j4  # Use number of CPU cores for faster build
cd ..
```

### 3. Verify Toolchain
```bash
arm-none-eabi-gcc --version
arm-none-eabi-gdb --version
openocd --version
```

## Building the Project

### Using Make
```bash
# Build the main project
make

# Clean build files
make clean

# Build with verbose output
make V=1
```

### Using VS Code
1. Open the project folder in VS Code
2. Press `Ctrl+Shift+P` and run "Tasks: Run Task"
3. Select "Build Smartcard Project"

Available VS Code tasks:
- **Build Smartcard Project** - Main build task
- **Clean Build** - Clean build artifacts
- **Build LibOpenCM3** - Build the library dependency
- **Flash Firmware** - Build and flash to hardware

## Programming and Debugging

### Flash Firmware
```bash
# Using scripts
./scripts/openocd_flash.sh

# Manual OpenOCD
openocd -f interface/stlink-v2.cfg -f target/stm32f0x.cfg -c "program demo_os_smartcard.bin 0x08000000 verify reset exit"
```

### Debugging
```bash
# Start debugging session
./scripts/gdb.sh

# Or manually
arm-none-eabi-gdb demo_os_smartcard.elf
```

### GDB Commands
```gdb
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) load
(gdb) continue
```

## Project Structure Understanding

### Source Files
- `src/main.c` - Application entry point
- `src/sc_system.c` - Smartcard system functions
- `src/adc.c` - ADC driver for random number generation
- `src/aes_*.c` - Various AES implementations with security countermeasures

### Headers
- `include/sc_system.h` - System function declarations
- `include/adc.h` - ADC function declarations

### Build System
- `Makefile` - Main build configuration
- `rules.mk` - LibOpenCM3 build rules
- `generated.stm32f051k8b6.ld` - Linker script (auto-generated)

### Scripts
- `scripts/gdb.sh` - GDB debugging helper
- `scripts/openocd.sh` - OpenOCD startup
- `scripts/openocd_flash.sh` - Flash programming script

## Troubleshooting

### Common Build Issues

1. **LibOpenCM3 not found**
   ```
   Error: libopencm3 not found
   Solution: Build libopencm3 first: cd libopencm3 && make
   ```

2. **ARM toolchain not found**
   ```
   Error: arm-none-eabi-gcc: command not found
   Solution: Install ARM GCC toolchain and add to PATH
   ```

3. **Permission denied on scripts**
   ```bash
   # Make scripts executable
   chmod +x scripts/*.sh
   ```

### Programming Issues

1. **ST-Link not detected**
   - Check USB connection
   - Install ST-Link drivers
   - Try different USB port

2. **OpenOCD connection failed**
   - Check if another instance is running
   - Verify target configuration
   - Check hardware connections

### Code Size Issues

Current memory usage:
- Flash: ~4.2KB of 64KB available
- RAM: 8 bytes of 8KB available

If you exceed memory limits:
- Enable higher optimization (`-Os` instead of `-Og`)
- Remove unused functions
- Optimize data structures

## Contributing Guidelines

1. **Code Style**
   - Follow existing code formatting
   - Use descriptive variable names
   - Add comments for complex algorithms

2. **Security Considerations**
   - Test timing attack resistance
   - Verify power analysis countermeasures
   - Document security assumptions

3. **Testing**
   - Test on actual hardware
   - Verify against reference implementations
   - Check for side-channel leakage

## Advanced Development

### Custom AES Implementations
To add new AES variants:
1. Create new source file in `src/`
2. Follow existing naming convention
3. Update Makefile `CFILES` if needed
4. Test thoroughly for correctness

### Hardware Modifications
For different STM32 variants:
1. Update `DEVICE` in Makefile
2. Verify pin configurations
3. Update linker script if needed
4. Test memory constraints

### Debugging Tips
- Use `printf` debugging via semihosting
- Monitor power consumption during execution
- Use logic analyzer for protocol verification
- Profile execution time with timer peripherals
