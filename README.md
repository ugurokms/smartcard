# Smartcard Demo OS Project

A demonstration operating system for smartcard applications running on STM32F051 microcontroller, featuring AES encryption with various countermeasures against side-channel attacks.

## Hardware

- **Target MCU**: STM32F051K8B6 (ARM Cortex-M0)
- **Hardware Version**: SMARTCARD_HW_V3
- **Memory**: 64KB Flash, 8KB RAM
- **Architecture**: ARM Cortex-M0 with soft-float

## Features

- **AES Encryption Implementations**:
  - Standard AES inverse operations (`aes_inv.c`)
  - Masked AES implementation for power analysis protection (`aes_inv_masked.c`)
  - Dummy operations for timing attack mitigation (`aes_inv_dummy.c`)
  - Shuffled operations for execution order randomization (`aes_inv_shuffle.c`)
  - Combined countermeasures (`aes_inv_dummy_shuffle_mask.c`)

- **System Components**:
  - Smartcard communication protocol
  - ADC functionality for random number generation
  - Hardware abstraction layer
  - Debug and programming scripts

## Project Structure

```
├── src/                   # Source code
│   ├── main.c             # Main application entry point
│   ├── sc_system.c        # Smartcard system functions
│   ├── adc.c              # ADC driver implementation
│   └── aes_*.c            # AES implementations with countermeasures
├── include/               # Header files
│   ├── sc_system.h        # System function declarations
│   └── adc.h              # ADC function declarations
├── scripts/               # Build and debug scripts
│   ├── gdb.sh             # GDB debugging script
│   ├── gdb_flash.sh       # GDB flash programming script
│   ├── openocd.sh         # OpenOCD startup script
│   └── openocd_flash.sh   # OpenOCD flash programming script
├── libopencm3/            # LibOpenCM3 library (submodule)
├── docs/                  # Documentation and analysis images
├── Makefile               # Build configuration
├── rules.mk               # Make rules for LibOpenCM3
└── DEVELOPMENT.md         # Development guide
```

## Getting Started

### Prerequisites

- ARM GCC Toolchain (`arm-none-eabi-gcc`)
- OpenOCD for programming and debugging
- ST-Link programmer
- Make build system

### Installation

1. **Clone the repository with submodules**:
   ```bash
   git clone --recursive https://github.com/ugurokms/smartcard.git
   cd smartcard
   ```

2. **If you already cloned without `--recursive`, initialize submodules**:
   ```bash
   git submodule update --init --recursive
   ```

3. **Build LibOpenCM3 library**:
   ```bash
   cd libopencm3
   make
   cd ..
   ```

### Building

1. **Build the project**:
   ```bash
   make
   ```

2. **Clean build files**:
   ```bash
   make clean
   ```

### Programming and Debugging

1. **Flash the firmware**:
   ```bash
   ./scripts/openocd_flash.sh
   ```

2. **Start debugging session**:
   ```bash
   ./scripts/gdb.sh
   ```

3. **Flash and debug in one step**:
   ```bash
   ./scripts/gdb_flash.sh
   ```

## Security Features

This project implements several countermeasures against side-channel attacks:

### Power Analysis Protection
- **Masking**: Random masks applied to sensitive operations
- **Dummy Operations**: Constant execution time regardless of data
- **Operation Shuffling**: Randomized execution order

### Timing Attack Mitigation
- Constant-time implementations
- Balanced branching structures
- Uniform execution patterns

## Analysis Tools

The project includes various analysis screenshots showing:
- AES encryption speed comparisons
- Power consumption patterns
- Communication protocol analysis
- Attack resistance measurements

## Memory Usage
Current build statistics:
- **Code Size**: ~4.2KB
- **Data (BSS)**: 8 bytes
- **Available Flash**: 64KB
- **Available RAM**: 8KB

**Note**: This is a research and educational project focused on secure video streaming using AES with smartcard-based key management. It includes DPA-resistant AES implementations (masking, shuffling) to protect against side-channel attacks and enables on-the-fly decryption of encrypted video data only for authorized users. When implementing cryptographic functions in production environments, ensure proper security review and testing.
