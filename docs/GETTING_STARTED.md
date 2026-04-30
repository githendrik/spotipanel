# Getting Started Guide

## Quick Start

### Option 1: PlatformIO (Recommended)

#### Install PlatformIO Core
```bash
# Install using pip
pip install platformio

# Or using Homebrew (macOS)
brew install platformio

# Verify installation
pio --version
```

#### Build and Upload
```bash
# Install dependencies
pio lib install

# Build project
pio run

# Upload to device
pio run -t upload

# Open serial monitor
pio device monitor
```

### Option 2: Arduino IDE

#### 1. Install Arduino IDE
Download from: https://www.arduino.cc/en/software

#### 2. Add ESP32 Board Support

1. Open Arduino IDE
2. Go to **File > Preferences**
3. Add this URL to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools > Board > Boards Manager**
5. Search for "ESP32"
6. Install **ESP32 Arduino** by Espressif Systems (version 2.0.14 or later)

#### 3. Configure Board Settings

1. Select board: **Tools > Board > ESP32S3 Dev Module**
2. Configure these settings:
   - **USB CDC On Boot**: Enabled
   - **CPU Frequency**: 240MHz
   - **Flash Size**: 16MB
   - **PSRAM**: OPI PSRAM
   - **Upload Speed**: 921600
   - **Port**: (select your COM port)

#### 4. Install Libraries

Install these libraries via **Sketch > Include Library > Manage Libraries**:
- `LovyanGFX` by lovyan03 (version 1.1.5 or later)
- `lvgl` by lvgl (version 8.3.11)
- `ArduinoJson` by Benoit Blanchon (version 7.0.0 or later)

#### 5. Upload Code

1. Open `src/main.cpp` in Arduino IDE
2. Click **Upload** button (→)
3. Open **Serial Monitor** (magnifying glass icon)
4. Set baud rate to 115200

## Troubleshooting

### Device Not Detected

**Windows:**
- Install CH343 or CP210x USB drivers
- Check Device Manager for COM port
- Try different USB cable

**macOS:**
```bash
# List available ports
ls /dev/tty.*

# May need to add user to dialout group
sudo usermod -a -G dialout $USER
```

**Linux:**
```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER

# Reload udev rules
sudo udevadm control --reload-rules
```

### Upload Failed

1. **Enter Boot Mode Manually:**
   - Hold the **BOOT** button
   - Press and release **RESET**
   - Release **BOOT**
   - Try upload again

2. **Check Board Settings:**
   - Ensure correct board is selected (ESP32S3)
   - Verify PSRAM is enabled
   - Check upload speed (try 115200 if 921600 fails)

### Display Not Working

- Verify all libraries are installed
- Check if display controller is GC9A01
- Try different rotation settings

### Serial Monitor Shows Garbage

- Ensure baud rate is set to 115200
- Check board configuration
- Try resetting the device

## Next Steps

After successfully uploading the test code:

1. ✅ Verify display shows test pattern
2. ✅ Test rotary encoder (counter should change)
3. ✅ Test button press (counter should reset)
4. ✅ Test touch input (coordinates shown on screen)
5. ✅ Check serial output for debug info

## Serial Monitor Commands

```bash
# PlatformIO
pio device monitor

# Arduino IDE
# Tools > Serial Monitor (set to 115200 baud)

# Direct connection (Linux/macOS)
screen /dev/ttyUSB0 115200
# Exit with: Ctrl+A, then K, then Y
```

## Useful Commands

### PlatformIO

```bash
# Clean build files
pio run -t clean

# Upload via USB
pio run -t upload

# Monitor serial output
pio device monitor

# List connected devices
pio device list

# Update platforms/libraries
pio update
```

### Get Help

```bash
pio -h
pio run -h
```

## Resources

- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [LovyanGFX Library](https://github.com/lovyan03/LovyanGFX)
- [LVGL Documentation](https://docs.lvgl.io/)
- [Arduino IDE Guide](https://docs.arduino.cc/software/ide-v2)
