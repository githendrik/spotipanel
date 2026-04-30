# Spotify Integration Guide

## Overview

This document describes how to integrate with the Spotify Web API to control playback and browse playlists.

## Authentication Flow

We use **OAuth 2.0 PKCE** (Proof Key for Code Exchange) flow, which is designed for public clients like embedded devices where storing a client secret is not secure.

### PKCE Flow Steps

```
1. Generate Code Verifier & Challenge
2. Redirect User to Spotify Authorization
3. User Authorizes Application
4. Spotify Redirects Back with Authorization Code
5. Exchange Code for Access Token
6. Use Access Token for API Calls
7. Refresh Token When Expired
```

## Setup Instructions

### 1. Create Spotify App

1. Go to [Spotify Developer Dashboard](https://developer.spotify.com/dashboard)
2. Click "Create App"
3. Fill in:
   - **App Name**: Spotipanel
   - **App Description**: Physical playlist launcher
   - **Redirect URI**: `http://localhost:8888/callback`
   - **Website**: (optional)
   - Check the box for "Web Playback API"
4. Note your **Client ID** (no client secret needed for PKCE)

### 2. First-Time Authentication

Since the device has limited input capabilities, the initial authentication should be done via:

**Option A: Web Setup Portal**
1. Device creates a WiFi access point
2. User connects to the AP
3. User visits `http://192.168.4.1`
4. User clicks "Connect to Spotify"
5. OAuth flow completes in browser
6. Tokens are sent to the device

**Option B: QR Code Display**
1. Device displays QR code on screen
2. User scans QR code with phone
3. OAuth flow completes on phone
4. Tokens are sent to device via WebSocket

**Option C: Manual Code Entry** (Simplest for v1)
1. User visits authorization URL on computer/phone
2. User copies authorization code
3. User enters code via serial monitor or web interface
4. Device exchanges code for tokens

## API Endpoints

### Player Control

```cpp
// Get Playback State
GET https://api.spotify.com/v1/me/player

// Start Playback (Playlist)
PUT https://api.spotify.com/v1/me/player/play
{
  "context_uri": "spotify:playlist:37i9dQZF1DXcBWIGoYBM5M"
}

// Pause Playback
PUT https://api.spotify.com/v1/me/player/pause

// Skip to Next Track
POST https://api.spotify.com/v1/me/player/next

// Skip to Previous Track
POST https://api.spotify.com/v1/me/player/previous

// Set Volume
PUT https://api.spotify.com/v1/me/player/volume?volume_percent=50

// Seek To Position
PUT https://api.spotify.com/v1/me/player/seek?position_ms=30000
```

### Playlist Management

```cpp
// Get Current User's Playlists
GET https://api.spotify.com/v1/me/playlists?limit=50

// Get Playlist Items
GET https://api.spotify.com/v1/playlists/{playlist_id}/tracks?limit=100

// Get User's Top Playlists
GET https://api.spotify.com/v1/me/playlists?limit=50&offset=0
```

### User Info

```cpp
// Get Current User Profile
GET https://api.spotify.com/v1/me
```

## Token Management

### Token Structure

```cpp
struct SpotifyToken {
    String access_token;      // Valid for 1 hour
    String refresh_token;     // Valid for 1 year
    uint32_t expires_at;      // Unix timestamp when access token expires
    String token_type;        // "Bearer"
    String scope;             // Granted scopes
};
```

### Storage

Store tokens in **NVS (Non-Volatile Storage)**:

```cpp
#include <nvs.h>

// Save tokens
nvs_handle_t nvs;
nvs_open("spotify", NVS_READWRITE, &nvs);
nvs_set_str(nvs, "access_token", token.access_token.c_str());
nvs_set_str(nvs, "refresh_token", token.refresh_token.c_str());
nvs_set_u32(nvs, "expires_at", token.expires_at);
nvs_commit(nvs);
nvs_close(nvs);

// Load tokens
nvs_get_str(nvs, "access_token", buffer, &length);
```

### Token Refresh

```cpp
POST https://accounts.spotify.com/api/token
Content-Type: application/x-www-form-urlencoded

grant_type=refresh_token
&refresh_token={refresh_token}
&client_id={client_id}
```

## Implementation Plan

### Phase 1: Basic Authentication

```cpp
// spotify_auth.h
class SpotifyAuth {
public:
    void begin(const String& clientId);
    String getAuthorizationUrl(const String& state);
    bool handleCallback(const String& code);
    bool isAuthenticated();
    String getAccessToken();
    void refreshToken();
    
private:
    String generateCodeVerifier();
    String generateCodeChallenge(const String& verifier);
    String exchangeCodeForToken(const String& code);
    String refreshAccessToken(const String& refreshToken);
};
```

### Phase 2: API Client

```cpp
// spotify_client.h
class SpotifyClient {
public:
    void begin(SpotifyAuth* auth);
    
    // Player control
    bool playPlaylist(const String& playlistUri);
    bool pause();
    bool nextTrack();
    bool previousTrack();
    bool setVolume(uint8_t volume);
    
    // Get data
    PlaylistList getPlaylists();
    PlayerState getPlayerState();
    
private:
    HTTPClient http;
    SpotifyAuth* auth;
    
    bool makeRequest(const String& endpoint, const String& method = "GET");
    String getAuthorizationHeader();
};
```

### Phase 3: Data Models

```cpp
// spotify_types.h
struct Playlist {
    String id;
    String name;
    String description;
    String imageUrl;
    int trackCount;
};

struct PlayerState {
    bool isPlaying;
    Track currentTrack;
    uint32_t progressMs;
    uint8_t volume;
    String deviceId;
};

struct Track {
    String id;
    String name;
    String artist;
    String album;
    int durationMs;
    String albumArtUrl;
};
```

## Required Scopes

Request these scopes during authorization:

```
user-read-private
user-read-email
playlist-read-private
playlist-read-collaborative
user-modify-playback-state
user-read-playback-state
user-read-currently-playing
streaming
```

## Error Handling

### Common Errors

```cpp
// 401 Unauthorized - Token expired
if (statusCode == 401) {
    refreshToken();
    retryRequest();
}

// 403 Forbidden - No active device
if (statusCode == 403) {
    showError("No active Spotify device");
    // User needs to open Spotify on another device first
}

// 429 Too Many Requests - Rate limited
if (statusCode == 429) {
    int retryAfter = response.header("Retry-After").toInt();
    delay(retryAfter * 1000);
    retryRequest();
}
```

## Testing

### Postman Collection

Create a Postman collection to test endpoints:
1. Authorization URL
2. Token exchange
3. Token refresh
4. API endpoints

### Unit Tests

```cpp
void test_spotify_auth() {
    // Test code verifier generation
    // Test code challenge generation
    // Test token parsing
}

void test_spotify_client() {
    // Test API request formatting
    // Test JSON parsing
    // Test error handling
}
```

## Security Considerations

1. **Never store client secret** - PKCE doesn't require it
2. **Use HTTPS** for all API calls
3. **Store tokens securely** in NVS with encryption if possible
4. **Validate state parameter** to prevent CSRF attacks
5. **Clear tokens** on factory reset

## References

- [Spotify Web API Documentation](https://developer.spotify.com/documentation/web-api)
- [OAuth 2.0 PKCE RFC](https://datatracker.ietf.org/doc/html/rfc7636)
- [Spotify Authorization Guide](https://developer.spotify.com/documentation/general/guides/authorization)
- [Spotify API Console](https://developer.spotify.com/console/)
