# Spotipanel - AI Agent Context

## Project Overview

**Spotipanel** is a physical Spotify playlist launcher built on the Elecrow CrowPanel 1.28" HMI ESP32 Rotary Display. A toddler-friendly device that lets users browse and play songs from a Spotify playlist using a rotary knob.

## Current State (Working App)

The app is fully functional with the following flow:
1. Boot → Connect WiFi → Refresh Spotify token → Fetch tracks from playlist
2. **Track list screen**: 3 large items visible, rotary to scroll, press to play
3. **Now playing screen**: play icon + track name + artist, rotary to skip tracks, press to pause and return to list
4. Playback targets a Spotify Connect speaker ("Wohnzimmer" — Yamaha MusicCast RX-V481 receiver)

### What Works
- WiFi connection (main network, not guest — devices must be on same network)
- Spotify OAuth token refresh (refresh token with proper scopes)
- Fetch playlist tracks via API (using `fields` parameter to keep response small)
- Scrollable track list UI (3 items, size 2 font, round screen safe zone)
- Play selected track on Spotify Connect speaker via `device_id`
- Pause playback and return to track list
- Skip tracks with rotary during playback
- Auto-find speaker device ID via `/me/player/devices`

### Known Limitations
- **Receiver must be powered on manually** — Spotify Connect API can send play commands but the Yamaha MusicCast receiver won't output audio unless it's already on with Spotify input selected. The Yamaha Extended Control API (port 80, `/YamahaExtendedControl/v1/`) exists but ESP32 raw socket connections to it fail (returns HTTP 0). Works fine from curl on a laptop.
- **Serial monitor doesn't work** — ESP32-S3 USB CDC issue with current config. Debug via on-screen messages only.
- **Must close serial monitor before uploading** or upload fails with "device disconnected"
- Playlist is limited to 20 tracks (MAX_TRACKS) to conserve memory
- Track names truncated to 14 chars on screen (round display constraint)

## Hardware Specifications

### Device: Elecrow CrowPanel 1.28" HMI ESP32-S3 Rotary Display
- **Main Chip**: ESP32-S3R8
- **Processor**: Xtensa 32-bit LX7 dual-core, 240MHz
- **Memory**: 512KB SRAM, 8MB PSRAM
- **Storage**: 16MB Flash
- **Display**: 1.28" IPS, 240x240 resolution, capacitive touch (GC9A01 controller)
- **Touch**: CST816D controller at I2C addr 0x15
- **Wireless**: WiFi 802.11a/b/g/n (2.4GHz), Bluetooth 5.0 + BLE
- **Input**: Rotary encoder + press switch
- **Power**: 5V/1A via FPC connector

### Pin Definitions
```cpp
#define TP_I2C_SDA_PIN  6    // Touch panel I2C
#define TP_I2C_SCL_PIN  7
#define ENCODER_A_PIN   45   // Rotary encoder
#define ENCODER_B_PIN   42
#define SWITCH_PIN      41   // Knob press button
#define SCREEN_BACKLIGHT_PIN 46  // PWM backlight (LEDC channel 0)
#define POWER_LIGHT_PIN 40

// Display SPI
// SCLK=10, MOSI=11, DC=3, CS=9, RST=14

// Power pins (must set HIGH on boot)
// GPIO 1, GPIO 2

// NeoPixel LEDs on GPIO 48 (needs Adafruit_NeoPixel library)
```

## Tech Stack

- **Framework**: PlatformIO with Arduino
- **Display Library**: LovyanGFX (NOT LVGL — matches Elecrow factory code, simpler)
- **JSON**: ArduinoJson v7 (NOT Arduino_JSON — conflicts with LovyanGFX)
- **Board**: `esp32-s3-devkitc-1` with minimal config
- **Connectivity**: WiFi, Spotify Web API, OAuth 2.0 (refresh token flow, not PKCE)

## Project Structure

```
spotipanel/
├── src/
│   ├── main.cpp              # Main application (all app logic)
│   ├── credentials.h          # WiFi + Spotify secrets (GITIGNORED)
│   └── credentials.h.example  # Template for credentials
├── include/
│   ├── hardware_pins.h        # Pin definitions (not currently used, inline in main.cpp)
│   └── config.h.example       # Old credential template
├── tools/
│   └── get_token.js           # Spotify OAuth token generator (reads from secrets.json)
├── secrets.json               # Spotify client ID/secret for tools (GITIGNORED)
├── secrets.json.example       # Template for secrets.json
├── platformio.ini             # PlatformIO config (CRITICAL: no custom PSRAM/USB flags)
├── AGENTS.md                  # This file
├── docs/                      # Documentation (mostly outdated, refers to LVGL)
└── .gitignore                 # Ignores credentials.h, secrets.json, config.h, .pio/
```

## Credentials Setup

Two gitignored files contain secrets:

1. **`src/credentials.h`** — used by the ESP32 at compile time:
   ```cpp
   const char* WIFI_SSID = "...";
   const char* WIFI_PASSWORD = "...";
   const char* SPOTIFY_CLIENT_ID = "...";
   const char* SPOTIFY_CLIENT_SECRET = "...";
   const char* SPOTIFY_REFRESH_TOKEN = "...";
   ```

2. **`secrets.json`** — used by `tools/get_token.js`:
   ```json
   {"spotify_client_id": "...", "spotify_client_secret": "...", "wifi_ssid": "...", "wifi_password": "..."}
   ```

### Generating a New Refresh Token

If the token expires or scopes need changing:
```bash
node tools/get_token.js
```
This opens a browser for Spotify authorization and prints the new refresh token.
Required scopes: `playlist-read-private playlist-read-collaborative user-read-playback-state user-modify-playback-state user-read-currently-playing streaming`

**Important**: The Spotify app's redirect URI must include `http://127.0.0.1:8888/callback` (not `localhost` — avoids HTTPS enforcement).

## Critical Lessons Learned

### Boot Loop Prevention
- **NEVER add custom `board_build` flags** for PSRAM OPI or USB CDC in `platformio.ini` — causes `rst:0x3 (RTC_SW_SYS_RST)` boot loops
- Keep `platformio.ini` minimal — the default board config works

### Spotify API on ESP32
- **Full playlist responses are too large** for ESP32 HTTPClient to buffer — always use `fields` parameter to request only needed data
- Example: `?fields=tracks.items(track(name,uri,artists(name)))&market=DE`
- **`http.getString()` blocks on 204 responses** (No Content) — don't call it after PUT requests that return 204. Just call `http.end()`.
- **Pause endpoint needs `Content-Length: 0` header** and `device_id` parameter
- The `/me/player/play` endpoint with `device_id` parameter can wake a Spotify Connect device, but the device's amplifier may not turn on (device-specific limitation)

### Network
- **ESP32 and Spotify Connect speaker must be on the same network** — guest WiFi isolates devices
- Yamaha MusicCast API exists at `http://<ip>/YamahaExtendedControl/v1/` but ESP32 can't connect to it via raw sockets (works from curl). Cause unknown.

### Display (Round Screen)
- Safe zone: ~35px from edges for content
- Track list: 3 items max with size 2 font, centered text
- Truncate track names to ~14 chars, artist to ~19 chars
- Use `fillRoundRect` + `drawRoundRect` for selection highlight

## Development Commands

```bash
# Build
pio run

# Upload (close serial monitor first!)
pio run -t upload --upload-port /dev/cu.usbmodem11101

# Serial monitor (doesn't work with current USB CDC config)
pio device monitor
```

## Configuration

### Playlist
Hardcoded in `src/main.cpp`: `PLAYLIST_ID = "1EWWknEA8IGAsvPaGg1Jge"` (private playlist, 10 tracks)

### Speaker
Hardcoded in `src/main.cpp`: `TARGET_SPEAKER = "Wohnzimmer"` (Yamaha MusicCast RX-V481 at 192.168.1.110)

## API Endpoints Used

- `POST https://accounts.spotify.com/api/token` — Refresh access token
- `GET https://api.spotify.com/v1/me/player/devices` — Find speaker device ID
- `GET https://api.spotify.com/v1/playlists/{id}?fields=...` — Fetch track list
- `PUT https://api.spotify.com/v1/me/player/play?device_id=...` — Start playback
- `PUT https://api.spotify.com/v1/me/player/pause?device_id=...` — Pause playback

## Possible Future Work

- WiFi provisioning (currently hardcoded credentials)
- Wake Yamaha receiver automatically (MusicCast API or IR blaster)
- Cover art display
- Multiple playlist support
- Touch screen interaction (currently unused)
- NeoPixel LED effects (GPIO 48)

## References

- [Hardware Repo](https://github.com/Elecrow-RD/CrowPanel-1.28inch-HMI-ESP32-Rotary-Display-240-240-IPS-Round-Touch-Knob-Screen)
- [LovyanGFX](https://github.com/lovyan03/LovyanGFX)
- [Spotify Web API](https://developer.spotify.com/documentation/web-api)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [Yamaha Extended Control API](https://github.com/rsc-dev/pyamaha)
- Reference project: `/Users/taarihe1/Documents/PlatformIO/Projects/260121-111731-freenove_esp32_s3_wroom/`

## Notes for AI Agents

1. Always read `src/main.cpp` first — it contains all app logic in a single file
2. Credentials are in `src/credentials.h` (gitignored) — never hardcode secrets in main.cpp
3. The device uses ESP32-S3, not classic ESP32 — ensure compatibility
4. **Do NOT use LVGL** — this project uses LovyanGFX directly
5. **Do NOT add PSRAM/USB board_build flags** to platformio.ini — causes boot loops
6. Debug via on-screen display text, not serial monitor
7. Always use `fields` parameter when fetching from Spotify API
8. Test on actual device — round screen clips corners
