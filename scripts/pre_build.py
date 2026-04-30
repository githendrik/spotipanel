# PlatformIO Pre-Build Script
# Auto-generates lv_conf.h and other configuration files

Import("env")
import os

def generate_lvgl_config():
    """Generate lv_conf.h for LVGL configuration"""
    
    lv_conf_content = """
#pragma once

#define LV_CONF_H
#define LV_CONF_SKIP

#include <stdint.h>

// ====================
// GRAPHICS SETTINGS
// ====================

#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00FF00)

#define LV_ANTIALIAS 1
#define LV_GAMMA_CORRECTION 0

// ====================
// MEMORY SETTINGS
// ====================

#define LV_MEM_CUSTOM 1
#define LV_MEM_CUSTOM_ALLOC malloc
#define LV_MEM_CUSTOM_FREE free
#define LV_MEM_CUSTOM_REALLOC realloc

#define LV_MEM_SIZE (4U * 1024U * 1024U)  // 4MB for PSRAM

#define LV_MEM_ATTR 
#define LV_MEM_BUF_MAX_NUM 16
#define LV_MEM_POOL_INCLUDE <stdlib.h>
#define LV_MEM_POOL_EXPORT

// ====================
// INPUT DEVICE SETTINGS
// ====================

#define LV_INDEV_DEF_READ_PERIOD 30
#define LV_INDEV_DEF_LONG_PRESS_TIME 400
#define LV_INDEV_DEF_LONG_PRESS_REP_TIME 100
#define LV_INDEV_DEF_GESTURE_LIMIT 50
#define LV_INDEV_DEF_GESTURE_MIN_VELOCITY 3
#define LV_INDEV_DEF_THROW_LATENCY 80

// ====================
// DISPLAY SETTINGS
// ====================

#define LV_DISP_DEF_REFR_PERIOD 30
#define LV_DISP_MAX_REFR_CNT 64
#define LV_DISP_DOUBLE_REFR 0
#define LV_DISP_FLUSH_BY_ISR 0

#define LV_DPI_DEF 130

// ====================
// DRAW SETTINGS
// ====================

#define LV_DRAW_COMPLEX 1
#define LV_DRAW_BUF_SIZE (60 * 1024)
#define LV_DRAW_BUF_STRIDE_ALIGN 1
#define LV_DRAW_BUF_DMA_ALIGN 1

#define LV_USE_DRAW_SW 1
#define LV_USE_DRAW_SW_ASM 0
#define LV_USE_DRAW_SW_COMPLEX 1

#if LV_USE_DRAW_SW_COMPLEX
    #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS 1
    #define LV_USE_DRAW_SW_COMPLEX_ARC 1
    #define LV_USE_DRAW_SW_COMPLEX_MASK 1
#endif

#define LV_USE_DRAW_SW_SHADOW 1
#define LV_USE_DRAW_SW_LINE_DASH 1
#define LV_USE_DRAW_SW_VECTOR_GRAPHIC 0
#define LV_USE_DRAW_SW_SNAPSHOT 0

#define LV_USE_DRAW_SW_TRANSFORM 0

// ====================
// FONT SETTINGS
// ====================

#define LV_FONT_MONTSERRAT_8 0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 1

#define LV_FONT_MONTSERRAT_12_SUBPX 0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0
#define LV_FONT_SIMSUMI_14_CJK 0
#define LV_FONT_SIMSUMI_16_CJK 0
#define LV_FONT_SIMSUMI_14_PERSIAN_HEBREW 0
#define LV_FONT_SIMSUMI_16_PERSIAN_HEBREW 0
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0
#define LV_FONT_UNSCII_8 0
#define LV_FONT_UNSCII_16 0

#define LV_FONT_DEFAULT &lv_font_montserrat_14
#define LV_FONT_FMT_TXT_LARGE 0
#define LV_USE_FONT_COMPRESSED 0
#define LV_USE_FONT_SUBPX 0
#define LV_FONT_SUBPX_BGR 0

// ====================
// TEXT SETTINGS
// ====================

#define LV_TXT_ENC LV_TXT_ENC_UTF8
#define LV_TXT_BREAK_CHARS " ,.;:-_\\n"
#define LV_TXT_LINE_BREAK_LONG_LEN 0
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3
#define LV_TXT_COLOR_CMD "#"
#define LV_USE_BIDI 0
#define LV_USE_ARABIC_PERSIAN_CHARS 0

// ====================
// WIDGET USAGE
// ====================

#define LV_USE_ARC 1
#define LV_USE_ANIMIMG 1
#define LV_USE_BAR 1
#define LV_USE_BTN 1
#define LV_USE_BTNMATRIX 1
#define LV_USE_CALENDAR 1
#define LV_USE_CANVAS 1
#define LV_USE_CHART 1
#define LV_USE_CHECKBOX 1
#define LV_USE_DROPDOWN 1
#define LV_USE_IMG 1
#define LV_USE_LABEL 1
#define LV_USE_LINE 1
#define LV_USE_ROLLER 1
#define LV_USE_SLIDER 1
#define LV_USE_SWITCH 1
#define LV_USE_TEXTAREA 1
#define LV_USE_TABLE 1
#define LV_USE_WIN 1

// ====================
// EXTRA COMPONENTS
// ====================

#define LV_USE_CALENDAR_HEADER_ARROW 1
#define LV_USE_CALENDAR_CHINESE 0
#define LV_USE_LIST 1
#define LV_USE_MENU 1
#define LV_USE_MSGBOX 1
#define LV_USE_SPINBOX 1
#define LV_USE_SPINNER 1
#define LV_USE_TABVIEW 1
#define LV_USE_TILEVIEW 1
#define LV_USE_WIN 1

// ====================
// THEMES
// ====================

#define LV_USE_THEME_DEFAULT 1
#define LV_USE_THEME_MONO 1

// ====================
// LAYOUTS
// ====================

#define LV_USE_FLEX 1
#define LV_USE_GRID 1

// ====================
// LOG SETTINGS
// ====================

#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 1
#define LV_LOG_TRACE_MEM 1
#define LV_LOG_TRACE_TIMER 1
#define LV_LOG_TRACE_INDEV 1
#define LV_LOG_TRACE_DISP_REFR 1
#define LV_LOG_TRACE_EVENT 1
#define LV_LOG_TRACE_OBJ_CREATE 1
#define LV_LOG_TRACE_LAYOUT 1
#define LV_LOG_TRACE_ANIM 1

// ====================
// ASSERTS
// ====================

#define LV_USE_ASSERT_NULL 1
#define LV_USE_ASSERT_MALLOC 1
#define LV_USE_ASSERT_STYLE 0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ 0

// ====================
// OTHERS
// ====================

#define LV_USE_SNAPSHOT 0
#define LV_USE_MONKEY 0
#define LV_USE_GRIDNAV 0
#define LV_USE_FRAGMENT 0
#define LV_USE_IMGFONT 0
#define LV_USE_IME_PINYIN 0
#define LV_USE_OBSERVER 1
#define LV_USE_LODEPNG 0
#define LV_USE_LIBPNG 0
#define LV_USE_BMP 0
#define LV_USE_RLE 0
#define LV_USE_LIBIMAGEQUANT 0
#define LV_USE_FS_STDIO 0
#define LV_USE_FS_POSIX 0
#define LV_USE_FS_WIN32 0
#define LV_USE_FS_FATFS 0
#define LV_USE_FS_LITTLEFS 0
#define LV_USE_FS_ARDUINO 0
#define LV_USE_FS_MEMFS 0
#define LV_USE_FS_CUSTOM 0
#define LV_USE_LODEPNG 0
#define LV_USE_LIBPNG 0
#define LV_USE_BMP 0
#define LV_USE_GIF 0
#define LV_USE_QRCODE 0
#define LV_USE_BARCODE 0
#define LV_USE_TFT_ESPI 0
#define LV_USE_TFT_eSPI 0

// ====================
// SYSTEM SETTINGS
// ====================

#define LV_ATTRIBUTE_CUSTOM_H <Arduino.h>
#define LV_ATTRIBUTE_TICK_INC 
#define LV_ATTRIBUTE_TIMER_HANDLER 
#define LV_ATTRIBUTE_FLUSH_READY 
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1
#define LV_ATTRIBUTE_MEM_ALIGN 
#define LV_ATTRIBUTE_LARGE_CONST 
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY 
#define LV_ATTRIBUTE_DMA 
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY 
#define LV_USE_PERF_MONITOR 0
#define LV_USE_REFR_MONITOR 0
#define LV_USE_PERF_TEST 0
#define LV_USE_SYSMON 0
#define LV_USE_SNAPSHOT 0
#define LV_USE_BUILTIN_MALLOC 0
#define LV_MEMCPY_MEMSET_STD 0
#define LV_ENABLE_GC_CUSTOM 0

"""
    
    # Write lv_conf.h to include directory
    with open('include/lv_conf.h', 'w') as f:
        f.write(lv_conf_content)
    
    print("Generated lv_conf.h")

# Run LVGL config generation
generate_lvgl_config()
