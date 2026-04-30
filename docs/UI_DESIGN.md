# UI Design Guide

## Display Specifications

- **Resolution**: 240x240 pixels
- **Shape**: Circular
- **Color Depth**: 16-bit (RGB565)
- **Library**: LVGL 8.3.11

## Design Principles

### 1. Circular-First Design
- Design for the circular display shape
- Keep important content within the safe zone (center 200x200)
- Use circular/rounded UI elements

### 2. Touch-Friendly
- Minimum touch target: 48x48 pixels
- Adequate spacing between interactive elements
- Clear visual feedback on touch

### 3. Rotary-Optimized
- List-based navigation works best with rotary encoder
- Clear selection indicators
- Smooth scrolling animations

## Screen Layouts

### 1. Splash Screen (Boot)
```
┌─────────────────┐
│                 │
│    [Logo]       │
│                 │
│   Spotipanel    │
│                 │
│   Connecting... │
│                 │
└─────────────────┘
```

**Elements:**
- App logo/icon (80x80)
- App name (label)
- Status indicator (spinner or progress bar)

### 2. Main Menu
```
┌─────────────────┐
│   ◄ Playlists ► │
│  ┌───────────┐  │
│  │ ♫ Playlist│  │
│  │ ♫ Playlist│  │
│  │ ♫ Playlist│◄ │  <- Selected
│  │ ♫ Playlist│  │
│  │ ♫ Playlist│  │
│  └───────────┘  │
│                 │
│   [Settings] ⚙  │
└─────────────────┘
```

**Navigation:**
- Rotate: Scroll through playlists
- Press: Open selected playlist
- Long press: Context menu
- Touch: Direct selection

### 3. Playlist View
```
┌─────────────────┐
│  ← Back         │
│ ┌─────────────┐ │
│ │ [Cover Art] │ │
│ │ Playlist    │ │
│ │ 24 songs    │ │
│ └─────────────┘ │
│                 │
│ ▶ Play All      │
│ ⋯ More Options  │
└─────────────────┘
```

### 4. Now Playing
```
┌─────────────────┐
│   Now Playing   │
│ ┌─────────────┐ │
│ │ [Album Art] │ │
│ │             │ │
│ └─────────────┘ │
│  Song Title     │
│   Artist Name   │
│                 │
│ ⏮  ▶/⏸  ⏭     │
│    Volume: 50%  │
└─────────────────┘
```

**Controls:**
- Rotate: Adjust volume
- Press: Play/Pause
- Touch corners: Skip tracks

### 5. Settings
```
┌─────────────────┐
│    Settings     │
│ ┌─────────────┐ │
│ │ WiFi        │ │
│ │ Spotify     │ │
│ │ Display     │ │
│ │ About       │ │
│ │             │ │
│ └─────────────┘ │
│                 │
│   [Back]        │
└─────────────────┘
```

### 6. WiFi Setup
```
┌─────────────────┐
│  WiFi Networks  │
│ ┌─────────────┐ │
│ │ Home WiFi   │ │
│ │ Guest       │ │
│ │ Office      │ │
│ │             │ │
│ │ [Refresh]   │ │
│ └─────────────┘ │
└─────────────────┘
```

### 7. Spotify Auth
```
┌─────────────────┐
│   Connect to    │
│    Spotify      │
│                 │
│ [QR Code]       │
│                 │
│ Scan to connect │
│                 │
│ Or visit:       │
│ spoti.fi/abc123 │
└─────────────────┘
```

## Color Scheme

### Spotify Brand Colors
```cpp
#define SPOTIFY_GREEN   lv_color_hex(0x1DB954)
#define SPOTIFY_BLACK   lv_color_hex(0x191414)
#define SPOTIFY_WHITE   lv_color_hex(0xFFFFFF)
#define SPOTIFY_DARK    lv_color_hex(0x121212)
#define SPOTIFY_GRAY    lv_color_hex(0xB3B3B3)
```

### UI Theme
```cpp
// Background
#define BG_COLOR        SPOTIFY_BLACK

// Text
#define TEXT_PRIMARY    SPOTIFY_WHITE
#define TEXT_SECONDARY  SPOTIFY_GRAY

// Accent
#define ACCENT_COLOR    SPOTIFY_GREEN

// States
#define SELECTED_COLOR  lv_color_hex(0x1A1A1A)
#define DISABLED_COLOR  lv_color_hex(0x404040)
```

## Typography

### Font Hierarchy
```cpp
// LVGL Font References
#define FONT_TITLE      &lv_font_montserrat_24
#define FONT_SUBTITLE   &lv_font_montserrat_16
#define FONT_BODY       &lv_font_montserrat_14
#define FONT_SMALL      &lv_font_montserrat_12
#define FONT_ICON       &lv_font_montserrat_20  // For icons
```

### Usage
- **Title**: Screen headers (24px)
- **Subtitle**: Section headers (16px)
- **Body**: Main content (14px)
- **Small**: Secondary info (12px)
- **Icon**: Icons and symbols (20px)

## Icons

Use Unicode symbols or create custom icons:

```cpp
#define ICON_PLAY       "\xEF\x81\x8B"  // U+F04B
#define ICON_PAUSE      "\xEF\x81\x8C"  // U+F04C
#define ICON_NEXT       "\xEF\x81\x91"  // U+F051
#define ICON_PREV       "\xEF\x81\x88"  // U+F048
#define ICON_VOLUME     "\xEF\x80\xA8"  // U+F028
#define ICON_SETTINGS   "\xEF\x80\x93"  // U+F013
#define ICON_WIFI       "\xEF\x87\xAB"  // U+F1EB
#define ICON_MUSIC      "\xEF\x80\x81"  // U+F001
#define ICON_SEARCH     "\xEF\x80\x82"  // U+F002
#define ICON_HEART      "\xEF\x80\x84"  // U+F004
```

## Component Library

### Custom Widgets

#### 1. Playlist Card
```cpp
lv_obj_t* create_playlist_card(lv_obj_t* parent, const Playlist* playlist);
```

#### 2. Progress Ring (for volume/progress)
```cpp
lv_obj_t* create_progress_ring(lv_obj_t* parent, int min, int max);
```

#### 3. Circular Menu
```cpp
lv_obj_t* create_circular_menu(lv_obj_t* parent, const char** items, int count);
```

## Animations

### Transitions
```cpp
// Screen transitions (300ms)
lv_anim_t a;
lv_anim_init(&a);
lv_anim_set_time(&a, 300);
lv_anim_set_exec_cb(&a, screen_slide_anim);
```

### Feedback
```cpp
// Button press feedback (100ms)
lv_obj_set_style_bg_opa(btn, LV_OPA_50, LV_STATE_PRESSED);

// Rotary selection highlight
lv_obj_scroll_to_view(selected, LV_ANIM_ON);
```

## Input Handling

### Rotary Encoder
```cpp
// Navigation
ROTATE_CW:    Move selection down/next
ROTATE_CCW:   Move selection up/previous
PRESS:        Select/Enter
LONG_PRESS:   Context menu/options
```

### Touch Screen
```cpp
TAP:        Select item
SWIPE_UP:   Scroll down
SWIPE_DOWN: Scroll up
SWIPE_LEFT: Go back
SWIPE_RIGHT: Go forward
PINCH:      Zoom (if applicable)
```

## Memory Optimization

### Guidelines
1. Reuse objects when possible
2. Use local buffers for temporary content
3. Free resources when screens are closed
4. Limit number of simultaneous objects
5. Use LVGL's object pooling

### Budget
```
Max simultaneous objects: ~50
Max labels: ~20
Max images: ~5 (with caching)
Draw buffer: 240x240/10 = 5.5KB per buffer
```

## Implementation Checklist

### Phase 1: Basic UI
- [ ] Splash screen
- [ ] Main menu (playlist list)
- [ ] Basic navigation with rotary encoder
- [ ] Touch input handling

### Phase 2: Spotify UI
- [ ] Now playing screen
- [ ] Playlist detail view
- [ ] Playback controls
- [ ] Volume control

### Phase 3: Settings
- [ ] WiFi setup screen
- [ ] Spotify authentication
- [ ] Display settings
- [ ] About screen

### Phase 4: Polish
- [ ] Animations
- [ ] Custom icons
- [ ] Error states
- [ ] Loading indicators
- [ ] Offline mode UI

## References

- [LVGL Documentation](https://docs.lvgl.io/8.3/)
- [LVGL Examples](https://github.com/lvgl/lvgl/tree/master/examples)
- [Spotify Design Guidelines](https://design.spotify.com/)
- [Material Design](https://material.io/design/platform-guidance/android-touch.html)
