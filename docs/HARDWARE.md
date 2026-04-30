# Hardware Setup Guide

## Elecrow CrowPanel 1.28" HMI ESP32-S3 Rotary Display

### Product Links
- [GitHub Repository](https://github.com/Elecrow-RD/CrowPanel-1.28inch-HMI-ESP32-Rotary-Display-240-240-IPS-Round-Touch-Knob-Screen)
- [Elecrow Product Page](https://www.elecrow.com/)

## Hardware Specifications

### Main Chip: ESP32-S3R8
- **Processor**: Xtensa 32-bit LX7 dual-core
- **Frequency**: Up to 240MHz
- **SRAM**: 512KB
- **PSRAM**: 8MB (Octal SPI)
- **Flash**: 16MB

### Display
- **Size**: 1.28 inches
- **Type**: IPS LCD
- **Resolution**: 240x240 pixels
- **Touch**: Capacitive (GT911 controller)
- **Shape**: Circular

### Wireless
- **WiFi**: 802.11a/b/g/n (2.4GHz)
- **Bluetooth**: 5.0 + BLE

### Physical
- **Dimensions**: 48x48x33mm
- **Weight**: 50g
- **Power**: 5V/1A via FPC connector
- **Operating Temp**: -20°C to 65°C

## Pin Definitions

### I2C Interfaces

#### Touch Panel I2C
| Pin | GPIO | Function |
|-----|------|----------|
| SDA | 6    | Touch I2C Data |
| SCL | 7    | Touch I2C Clock |

#### Display I2C
| Pin | GPIO | Function |
|-----|------|----------|
| SDA | 38   | Display I2C Data |
| SCL | 39   | Display I2C Clock |

### Rotary Encoder
| Pin | GPIO | Function |
|-----|------|----------|
| A   | 45   | Encoder A Phase |
| B   | 42   | Encoder B Phase |
| SW  | 41   | Encoder Push Switch |

**Note**: All encoder pins use internal pull-up resistors

### Additional Interfaces
- **UART**: 2x UART available via 4P 1.25mm connector
- **I2C**: 4P 1.25mm connector for external I2C devices
- **FPC**: 12P connector for power and programming

## Getting Started

### 1. Install USB Drivers

The device uses a CH343 or CP210x USB-to-UART converter. Download drivers from:
- [CH343 Drivers](https://www.wch.cn/downloads/CH343SER_ZIP.html)
- [CP210x Drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)

### 2. Connect to Computer

1. Use a USB-C cable to connect the device to your computer
2. The device should appear as a serial port (e.g., `/dev/ttyUSB0` on Linux, `COM3` on Windows)

### 3. Enter Bootloader Mode

If the device is not detected:
1. Hold the **BOOT** button
2. Press and release the **RESET** button
3. Release the **BOOT** button
4. The device is now in bootloader mode

### 4. Verify Connection

```bash
# List available serial ports
pio device list

# Expected output:
# /dev/ttyUSB0  Elecrow CrowPanel
```

## Wiring Diagram

```
┌─────────────────────────────┐
│     CrowPanel 1.28"         │
│                             │
│    ┌───────────────┐        │
│    │   Display     │        │
│    │   240x240     │        │
│    └───────────────┘        │
│         Rotary Knob         │
│                             │
│  [BOOT]  [RESET]  [LED]    │
│                             │
│       USB-C Port            │
└─────────────────────────────┘

Internal Connections:
- Display: I2C (GPIO 38, 39)
- Touch: I2C (GPIO 6, 7)
- Encoder: GPIO 45, 42, 41
```

## Display Controller

The display controller is likely one of the following:
- **ILI9341** - Common SPI display controller
- **ST7789** - Alternative SPI display controller
- **GC9A01** - Circular display controller

**TODO**: Verify the exact controller from the hardware documentation or by probing the I2C/SPI bus.

## Touch Controller

The capacitive touch controller is:
- **GT911** - I2C touch controller
- **I2C Address**: 0x14 (or 0x5D)
- **Interface**: I2C (GPIO 6, 7)

## Memory Layout

### Flash Partition (16MB total)
```
0x0000 - 0x9000   : Empty
0x9000 - 0xF000   : NVS (Non-Volatile Storage)
0xF000 - 0x10000  : PHY Initialization
0x10000 - 0x310000: Factory App
0x310000 - 0x610000: OTA App
0x610000 - 0x1000000: SPIFFS (File System)
```

### PSRAM Usage
- **Total**: 8MB
- **LVGL Buffers**: ~500KB (dual buffers)
- **WiFi/BT**: ~200KB
- **Application**: Remaining available

## Troubleshooting

### Device Not Detected
1. Try a different USB cable
2. Install/Reinstall USB drivers
3. Enter bootloader mode manually
4. Check Device Manager (Windows) or `ls /dev/tty*` (Linux/Mac)

### Display Not Working
1. Verify I2C connections (GPIO 38, 39)
2. Check if display controller is receiving power
3. Verify the correct display driver is initialized

### Touch Not Responding
1. Check I2C connections (GPIO 6, 7)
2. Verify touch controller I2C address
3. Ensure GT911 driver is properly initialized

### Rotary Encoder Issues
1. Verify GPIO pins (45, 42, 41)
2. Check for proper pull-up resistors
3. Debounce in software if needed

## Next Steps

1. Test basic functionality with example code
2. Verify display initialization
3. Test touch input
4. Test rotary encoder
5. Connect to WiFi
6. Implement Spotify integration

## References

- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [GT911 Datasheet](https://www.displaytech-us.com/content/datasheets/DT-Touch/GT911%20Datasheet%20V1.0.pdf)
- [LVGL Documentation](https://docs.lvgl.io/8.3/)
