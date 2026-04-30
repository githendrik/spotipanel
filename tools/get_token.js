#!/usr/bin/env node
/**
 * Spotify OAuth Token Generator for Spotipanel
 * 
 * Usage: node tools/get_token.js
 * 
 * Opens browser for Spotify authorization, captures the callback,
 * and prints a new refresh token with all required scopes.
 */

const http = require('http');
const { exec } = require('child_process');
const https = require('https');
const url = require('url');
const fs = require('fs');
const path = require('path');

const secretsPath = path.join(__dirname, '..', 'secrets.json');
if (!fs.existsSync(secretsPath)) {
  console.error('Missing secrets.json — copy secrets.json.example and fill in your credentials.');
  process.exit(1);
}
const secrets = JSON.parse(fs.readFileSync(secretsPath, 'utf8'));

const CLIENT_ID = secrets.spotify_client_id;
const CLIENT_SECRET = secrets.spotify_client_secret;
const REDIRECT_URI = 'http://127.0.0.1:8888/callback';
const SCOPES = [
  'playlist-read-private',
  'playlist-read-collaborative',
  'user-read-playback-state',
  'user-modify-playback-state',
  'user-read-currently-playing',
  'streaming',
].join(' ');

const server = http.createServer((req, res) => {
  const parsed = url.parse(req.url, true);
  
  if (parsed.pathname === '/callback') {
    const code = parsed.query.code;
    if (!code) {
      res.end('Error: no code received');
      server.close();
      return;
    }
    
    // Exchange code for tokens
    const postData = new URLSearchParams({
      grant_type: 'authorization_code',
      code,
      redirect_uri: REDIRECT_URI,
      client_id: CLIENT_ID,
      client_secret: CLIENT_SECRET,
    }).toString();

    const options = {
      hostname: 'accounts.spotify.com',
      path: '/api/token',
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded',
        'Content-Length': Buffer.byteLength(postData),
      },
    };

    const tokenReq = https.request(options, (tokenRes) => {
      let body = '';
      tokenRes.on('data', d => body += d);
      tokenRes.on('end', () => {
        const data = JSON.parse(body);
        if (data.refresh_token) {
          console.log('\n=== SUCCESS ===');
          console.log('Refresh Token:', data.refresh_token);
          console.log('Access Token:', data.access_token?.substring(0, 30) + '...');
          console.log('Scopes:', data.scope);
          console.log('\nPaste this refresh token into src/credentials.h');
          res.end('Success! You can close this tab. Check your terminal for the token.');
        } else {
          console.log('Error:', body);
          res.end('Error exchanging code. Check terminal.');
        }
        server.close();
      });
    });
    tokenReq.write(postData);
    tokenReq.end();
  }
});

server.listen(8888, () => {
  const authUrl = `https://accounts.spotify.com/authorize?client_id=${CLIENT_ID}&response_type=code&redirect_uri=${encodeURIComponent(REDIRECT_URI)}&scope=${encodeURIComponent(SCOPES)}`;
  console.log('Opening browser for Spotify authorization...');
  console.log('If it doesn\'t open, visit:', authUrl);
  exec(`open "${authUrl}"`);
});
