#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LovyanGFX.hpp>
#include <Wire.h>
#include "credentials.h"

// Pins
#define TP_I2C_SDA_PIN  6
#define TP_I2C_SCL_PIN  7
#define ENCODER_A_PIN   45
#define ENCODER_B_PIN   42
#define SWITCH_PIN      41
#define SCREEN_BACKLIGHT_PIN 46
#define POWER_LIGHT_PIN 40

// Display driver
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 80000000;
      cfg.freq_read = 20000000;
      cfg.spi_3wire = true;
      cfg.use_lock = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = 10;
      cfg.pin_mosi = 11;
      cfg.pin_miso = -1;
      cfg.pin_dc = 3;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = 9;
      cfg.pin_rst = 14;
      cfg.pin_busy = -1;
      cfg.memory_width = 240;
      cfg.memory_height = 240;
      cfg.panel_width = 240;
      cfg.panel_height = 240;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;
      _panel_instance.config(cfg);
    }
    setPanel(&_panel_instance);
  }
};

LGFX display;

// ---- App state ----
enum AppState {
  STATE_BOOT,
  STATE_WIFI,
  STATE_TOKEN,
  STATE_FETCH_TRACKS,
  STATE_TRACK_LIST,
  STATE_PLAYING,
  STATE_ERROR
};

AppState appState = STATE_BOOT;

String spotifyAccessToken = "";

// Track storage - max 20 tracks to keep memory reasonable
#define MAX_TRACKS 20
struct Track {
  char name[40];   // truncated display name
  char uri[50];    // spotify:track:xxxxx
  char artist[30]; // artist name
};

Track tracks[MAX_TRACKS];
int totalTracks = 0;
int selectedTrack = 0;
int scrollOffset = 0;

// Speaker device ID (resolved at startup)
String speakerDeviceId = "";

// Encoder state
volatile int encoderPos = 0;
int lastEncoderPos = 0;
volatile unsigned long lastEncoderTime = 0;
int lastEncoderA = HIGH;

// Button state
bool lastButtonState = HIGH;
unsigned long lastButtonPress = 0;

// UI needs redraw
bool needsRedraw = true;

// Debug: last API response codes
int lastTransferCode = 0;
int lastPlayCode = 0;
int lastDeviceCount = 0;
String lastDeviceNames = "";

// ---- Drawing helpers ----

void drawCenteredText(const char* text, int y, int size, uint16_t color) {
  display.setTextSize(size);
  display.setTextColor(color);
  int textWidth = strlen(text) * (6 * size);
  int x = (240 - textWidth) / 2;
  if (x < 0) x = 0;
  display.setCursor(x, y);
  display.print(text);
}

void drawLoadingScreen(const char* message) {
  display.fillScreen(TFT_BLACK);
  drawCenteredText("Spotipanel", 90, 2, TFT_GREEN);
  drawCenteredText(message, 130, 2, TFT_WHITE);
}

void drawErrorScreen(const char* line1, const char* line2) {
  display.fillScreen(TFT_BLACK);
  drawCenteredText(line1, 90, 2, TFT_RED);
  if (line2) drawCenteredText(line2, 130, 1, TFT_GRAY);
}

// ---- Track list UI ----
// Round screen: safe zone ~30px from edges
// 3 large items, text size 2 (12px tall), centered in round area

#define LIST_X      35
#define LIST_W      170
#define LIST_Y      55
#define ITEM_H      50
#define VISIBLE_ITEMS 3

void drawTrackList() {
  display.fillScreen(TFT_BLACK);

  // Scroll position indicator (e.g. "2 / 10")
  char posText[10];
  snprintf(posText, sizeof(posText), "%d / %d", selectedTrack + 1, totalTracks);
  drawCenteredText(posText, 20, 1, 0x8410);

  // Draw visible track items
  for (int i = 0; i < VISIBLE_ITEMS; i++) {
    int trackIdx = scrollOffset + i;
    if (trackIdx >= totalTracks) break;

    int itemY = LIST_Y + i * ITEM_H;
    bool isSelected = (trackIdx == selectedTrack);

    if (isSelected) {
      display.fillRoundRect(LIST_X - 4, itemY - 2, LIST_W + 8, ITEM_H - 4, 8, 0x1082);
      display.drawRoundRect(LIST_X - 4, itemY - 2, LIST_W + 8, ITEM_H - 4, 8, TFT_GREEN);
    }

    // Track name - size 2 for readability, truncate to ~14 chars
    display.setTextSize(2);
    display.setTextColor(isSelected ? TFT_WHITE : TFT_LIGHTGREY);
    char displayName[15];
    strncpy(displayName, tracks[trackIdx].name, 14);
    displayName[14] = '\0';
    int nameW = strlen(displayName) * 12;
    int nameX = (240 - nameW) / 2;
    if (nameX < LIST_X) nameX = LIST_X;
    display.setCursor(nameX, itemY + 4);
    display.print(displayName);

    // Artist - size 1, centered
    display.setTextSize(1);
    display.setTextColor(isSelected ? TFT_CYAN : 0x8410);
    char displayArtist[20];
    strncpy(displayArtist, tracks[trackIdx].artist, 19);
    displayArtist[19] = '\0';
    int artistW = strlen(displayArtist) * 6;
    int artistX = (240 - artistW) / 2;
    if (artistX < LIST_X) artistX = LIST_X;
    display.setCursor(artistX, itemY + 24);
    display.print(displayArtist);
  }

  // Scroll arrows
  if (scrollOffset > 0) {
    drawCenteredText("^", 42, 2, 0x4208);
  }
  if (scrollOffset + VISIBLE_ITEMS < totalTracks) {
    drawCenteredText("v", 210, 2, 0x4208);
  }
}

// ---- Now Playing UI ----

char nowPlayingName[40] = "";
char nowPlayingArtist[30] = "";

void drawNowPlaying() {
  display.fillScreen(TFT_BLACK);

  // Big play icon (triangle) centered
  int cx = 120, cy = 65;
  display.fillTriangle(cx - 25, cy - 30, cx - 25, cy + 30, cx + 30, cy, TFT_GREEN);

  // Track name - size 2, centered, truncate to fit
  display.setTextSize(2);
  display.setTextColor(TFT_WHITE);
  char displayName[15];
  strncpy(displayName, nowPlayingName, 14);
  displayName[14] = '\0';
  int nameW = strlen(displayName) * 12;
  display.setCursor((240 - nameW) / 2, 120);
  display.print(displayName);

  // Artist - size 2, centered
  display.setTextSize(1);
  display.setTextColor(TFT_CYAN);
  char displayArtist[20];
  strncpy(displayArtist, nowPlayingArtist, 19);
  displayArtist[19] = '\0';
  int artistW = strlen(displayArtist) * 6;
  display.setCursor((240 - artistW) / 2, 145);
  display.print(displayArtist);

  // Navigation hint
  drawCenteredText("Turn: skip", 185, 1, 0x4208);
  drawCenteredText("Press: stop", 200, 1, 0x4208);
}

// ---- Spotify API functions ----

bool refreshToken() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  http.begin(client, "https://accounts.spotify.com/api/token");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String postData = "grant_type=refresh_token&refresh_token=" + String(SPOTIFY_REFRESH_TOKEN) +
                    "&client_id=" + String(SPOTIFY_CLIENT_ID) +
                    "&client_secret=" + String(SPOTIFY_CLIENT_SECRET);

  int httpCode = http.POST(postData);
  bool success = false;

  if (httpCode == 200) {
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (!error && doc["access_token"].is<const char*>()) {
      spotifyAccessToken = doc["access_token"].as<String>();
      success = true;
    }
  }

  http.end();
  return success;
}

bool fetchTracks() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.setTimeout(10000);

  // Fetch tracks with only the fields we need — keeps response small
  String url = "https://api.spotify.com/v1/playlists/" + String(PLAYLIST_ID) +
               "?fields=tracks.items(track(name,uri,artists(name)))&market=DE";
  http.begin(client, url.c_str());
  http.addHeader("Authorization", "Bearer " + spotifyAccessToken);

  int httpCode = http.GET();
  String payload = http.getString();
  http.end();

  if (httpCode != 200 || payload.length() < 10) return false;

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) return false;

  JsonArray items = doc["tracks"]["items"].as<JsonArray>();
  totalTracks = 0;

  for (JsonObject item : items) {
    if (totalTracks >= MAX_TRACKS) break;

    JsonObject track = item["track"].as<JsonObject>();
    if (track.isNull()) continue;

    const char* name = track["name"] | "Unknown";
    const char* uri = track["uri"] | "";
    const char* artist = "Unknown";
    JsonArray artists = track["artists"].as<JsonArray>();
    if (artists.size() > 0) {
      artist = artists[0]["name"] | "Unknown";
    }

    strncpy(tracks[totalTracks].name, name, sizeof(tracks[0].name) - 1);
    tracks[totalTracks].name[sizeof(tracks[0].name) - 1] = '\0';
    strncpy(tracks[totalTracks].uri, uri, sizeof(tracks[0].uri) - 1);
    tracks[totalTracks].uri[sizeof(tracks[0].uri) - 1] = '\0';
    strncpy(tracks[totalTracks].artist, artist, sizeof(tracks[0].artist) - 1);
    tracks[totalTracks].artist[sizeof(tracks[0].artist) - 1] = '\0';

    totalTracks++;
  }

  return totalTracks > 0;
}

bool findSpeaker() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  http.begin(client, "https://api.spotify.com/v1/me/player/devices");
  http.addHeader("Authorization", "Bearer " + spotifyAccessToken);

  int httpCode = http.GET();
  String payload = http.getString();
  http.end();

  if (httpCode != 200 || payload.length() < 10) return false;

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) return false;

  JsonArray devices = doc["devices"].as<JsonArray>();
  lastDeviceCount = devices.size();
  lastDeviceNames = "";

  for (JsonObject device : devices) {
    const char* name = device["name"] | "";
    if (lastDeviceNames.length() > 0) lastDeviceNames += ", ";
    lastDeviceNames += name;
    if (strcmp(name, TARGET_SPEAKER) == 0) {
      speakerDeviceId = device["id"].as<String>();
      return true;
    }
  }

  return false;
}

bool transferPlayback() {
  if (speakerDeviceId.length() == 0) return false;

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  http.begin(client, "https://api.spotify.com/v1/me/player");
  http.addHeader("Authorization", "Bearer " + spotifyAccessToken);
  http.addHeader("Content-Type", "application/json");

  String body = "{\"device_ids\":[\"" + speakerDeviceId + "\"],\"play\":true}";
  int httpCode = http.PUT(body);
  http.end();

  return (httpCode >= 200 && httpCode < 300);
}

bool startPlayback(const char* trackUri) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String url = "https://api.spotify.com/v1/me/player/play";
  if (speakerDeviceId.length() > 0) {
    url += "?device_id=" + speakerDeviceId;
  }

  http.begin(client, url.c_str());
  http.addHeader("Authorization", "Bearer " + spotifyAccessToken);
  http.addHeader("Content-Type", "application/json");

  String body = "{\"context_uri\":\"spotify:playlist:" + String(PLAYLIST_ID) +
                "\",\"offset\":{\"uri\":\"" + String(trackUri) + "\"}}";

  lastPlayCode = http.PUT(body);
  lastTransferCode = 0;
  http.end();

  return (lastPlayCode >= 200 && lastPlayCode < 300);
}

bool pausePlayback() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String url = "https://api.spotify.com/v1/me/player/pause";
  if (speakerDeviceId.length() > 0) {
    url += "?device_id=" + speakerDeviceId;
  }

  http.begin(client, url.c_str());
  http.addHeader("Authorization", "Bearer " + spotifyAccessToken);
  http.addHeader("Content-Length", "0");

  int httpCode = http.PUT("");
  http.end();

  return (httpCode >= 200 && httpCode < 300);
}

// ---- Input handling ----

void readEncoder() {
  int a = digitalRead(ENCODER_A_PIN);
  if (a != lastEncoderA && a == LOW) {
    unsigned long now = millis();
    if (now - lastEncoderTime > 50) { // debounce
      int b = digitalRead(ENCODER_B_PIN);
      if (b == HIGH) {
        encoderPos++;
      } else {
        encoderPos--;
      }
      lastEncoderTime = now;
    }
  }
  lastEncoderA = a;
}

bool buttonPressed() {
  bool current = digitalRead(SWITCH_PIN);
  if (current == LOW && lastButtonState == HIGH) {
    unsigned long now = millis();
    if (now - lastButtonPress > 300) { // debounce
      lastButtonPress = now;
      lastButtonState = current;
      return true;
    }
  }
  lastButtonState = current;
  return false;
}

// ---- Setup & Loop ----

void setup() {
  // Power pins
  pinMode(POWER_LIGHT_PIN, OUTPUT);
  digitalWrite(POWER_LIGHT_PIN, LOW);
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  Wire.begin(TP_I2C_SDA_PIN, TP_I2C_SCL_PIN);

  // Encoder & button
  pinMode(ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ENCODER_B_PIN, INPUT_PULLUP);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  lastEncoderA = digitalRead(ENCODER_A_PIN);

  // Display
  display.init();
  display.setRotation(0);
  display.fillScreen(TFT_BLACK);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(SCREEN_BACKLIGHT_PIN, 0);
  ledcWrite(0, 255);

  drawLoadingScreen("Starting...");
  appState = STATE_WIFI;
}

void loop() {
  readEncoder();

  switch (appState) {

    case STATE_WIFI: {
      drawLoadingScreen("Connecting WiFi...");
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        attempts++;
      }
      if (WiFi.status() == WL_CONNECTED) {
        // Show ESP32's IP for debugging
        char ipStr[30];
        snprintf(ipStr, sizeof(ipStr), "IP: %s", WiFi.localIP().toString().c_str());
        drawCenteredText(ipStr, 150, 1, TFT_CYAN);
        delay(2000);
        appState = STATE_TOKEN;
      } else {
        drawErrorScreen("WiFi Failed", "Check credentials");
        delay(3000);
        // Retry
      }
      break;
    }

    case STATE_TOKEN: {
      drawLoadingScreen("Logging in...");
      if (refreshToken()) {
        appState = STATE_FETCH_TRACKS;
      } else {
        drawErrorScreen("Login Failed", "Check Spotify creds");
        delay(3000);
      }
      break;
    }

    case STATE_FETCH_TRACKS: {
      drawLoadingScreen("Loading songs...");
      if (fetchTracks()) {
        // Try to find the target speaker
        drawLoadingScreen("Finding speaker...");
        if (!findSpeaker()) {
          // Not fatal — will retry when playing
          drawLoadingScreen("Speaker not found");
          delay(1000);
        }
        selectedTrack = 0;
        scrollOffset = 0;
        encoderPos = 0;
        lastEncoderPos = 0;
        needsRedraw = true;
        appState = STATE_TRACK_LIST;
      } else {
        drawErrorScreen("No tracks found", "Check playlist");
        delay(3000);
      }
      break;
    }

    case STATE_TRACK_LIST: {
      // Handle encoder rotation — navigate track list
      if (encoderPos != lastEncoderPos) {
        int delta = encoderPos - lastEncoderPos;
        lastEncoderPos = encoderPos;

        selectedTrack += delta;
        if (selectedTrack < 0) selectedTrack = 0;
        if (selectedTrack >= totalTracks) selectedTrack = totalTracks - 1;

        // Adjust scroll to keep selection visible
        if (selectedTrack < scrollOffset) {
          scrollOffset = selectedTrack;
        }
        if (selectedTrack >= scrollOffset + VISIBLE_ITEMS) {
          scrollOffset = selectedTrack - VISIBLE_ITEMS + 1;
        }

        needsRedraw = true;
      }

      // Handle button press — play selected track
      if (buttonPressed()) {
        strncpy(nowPlayingName, tracks[selectedTrack].name, sizeof(nowPlayingName) - 1);
        strncpy(nowPlayingArtist, tracks[selectedTrack].artist, sizeof(nowPlayingArtist) - 1);
        drawLoadingScreen("Starting...");

        bool played = startPlayback(tracks[selectedTrack].uri);
        if (!played) {
          // Retry: find speaker and try again
          drawLoadingScreen("Finding speaker...");
          played = findSpeaker() && startPlayback(tracks[selectedTrack].uri);
        }

        if (played) {
          appState = STATE_PLAYING;
          drawNowPlaying();
        } else {
          drawErrorScreen("Playback failed", "Speaker offline?");
          delay(2000);
          needsRedraw = true;
        }
      }

      if (needsRedraw) {
        drawTrackList();
        needsRedraw = false;
      }
      break;
    }

    case STATE_PLAYING: {
      if (needsRedraw) {
        drawNowPlaying();
        needsRedraw = false;
      }

      // Rotary to skip tracks
      if (encoderPos != lastEncoderPos) {
        int delta = encoderPos - lastEncoderPos;
        lastEncoderPos = encoderPos;

        selectedTrack += delta;
        if (selectedTrack < 0) selectedTrack = 0;
        if (selectedTrack >= totalTracks) selectedTrack = totalTracks - 1;

        // Update scroll position
        if (selectedTrack < scrollOffset) scrollOffset = selectedTrack;
        if (selectedTrack >= scrollOffset + VISIBLE_ITEMS) scrollOffset = selectedTrack - VISIBLE_ITEMS + 1;

        // Play the new track immediately
        strncpy(nowPlayingName, tracks[selectedTrack].name, sizeof(nowPlayingName) - 1);
        strncpy(nowPlayingArtist, tracks[selectedTrack].artist, sizeof(nowPlayingArtist) - 1);
        startPlayback(tracks[selectedTrack].uri);
        drawNowPlaying();
      }

      // Press button to pause and return to list
      if (buttonPressed()) {
        pausePlayback();
        needsRedraw = true;
        appState = STATE_TRACK_LIST;
      }
      break;
    }

    default:
      break;
  }

  delay(5); // small delay to avoid busy-looping
}
