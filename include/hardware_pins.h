#pragma once

// Hardware Pin Definitions for Elecrow CrowPanel 1.28" HMI ESP32-S3 Rotary Display

// Touch Panel I2C Interface
#define TP_I2C_SDA_PIN  6
#define TP_I2C_SCL_PIN  7

// Display I2C Interface
#define I2C_SDA_PIN     38
#define I2C_SCL_PIN     39

// Rotary Encoder
#define ENCODER_A_PIN   45
#define ENCODER_B_PIN   42
#define SWITCH_PIN      41

// Display Specifications
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   240

// Display Type
#define DISPLAY_TYPE_ILI9341  // Verify this from hardware docs

// Touch Controller
#define TOUCH_CONTROLLER_GT911  // Capacitive touch controller

// I2C Addresses
#define TP_I2C_ADDRESS  0x14  // Touch panel I2C address (verify from datasheet)
#define DISPLAY_I2C_ADDRESS 0x3C

// Rotary Encoder Configuration
#define ENCODER_STEPS_PER_REV 20  // Detents per revolution
#define ENCODER_DEBOUNCE_MS   50  // Debounce delay in milliseconds

// Button Configuration
#define BUTTON_DEBOUNCE_MS    50  // Button debounce delay
#define BUTTON_LONG_PRESS_MS  1000 // Long press threshold

// Memory Configuration
#define PSRAM_AVAILABLE       1     // PSRAM is available on this board
#define LVGL_BUFFER_SIZE      (SCREEN_WIDTH * SCREEN_HEIGHT / 10)  // LVGL draw buffer
