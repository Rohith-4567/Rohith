#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <WiFi.h>
#include <WebServer.h> // part of the ESP32-Arduino core

// ---------- SD-card SPI pins ----------
#define REASSIGN_PINS
constexpr int sck = 14;
constexpr int miso = 12;
constexpr int mosi = 13;
constexpr int cs = 15;

// ---------- Wi-Fi / web-server ----------
constexpr char AP_SSID[] = "MiteOUT-ESP32";
constexpr char AP_PASSWORD[] = "miteout123"; // 8+ chars required for WPA2

WebServer server(80);
String logMsg; // what we’ll show in the browser

// ---------- web-page handler ----------
void handleRoot()
{
  String html =
      "<!DOCTYPE html><html><head><meta charset='utf-8'>"
      "<title>ESP32 Logger</title></head><body>"
      "<h2>MiteOUT ESP32 Status</h2><pre>";
  html += logMsg;
  html += "</pre></body></html>";

  server.send(200, "text/html", html);
}

void setup()
{
  // You can keep Serial.begin for emergency debug
  Serial.begin(115200);

  // ---------- mount SD ----------
#ifdef REASSIGN_PINS
  SPI.begin(sck, miso, mosi, cs);
  if (!SD.begin(cs))
#else
  if (!SD.begin())
#endif
  {
    logMsg = "Card Mount Failed\n";
  }
  else
  {
    File f = SD.open("/MiteOut_Data.txt", FILE_WRITE);
    if (f)
    {
      f.println("Hello world");
      f.close();
      logMsg = "File written: /MiteOut_Data.txt\n";
    }
    else
    {
      logMsg = "Failed to write file\n";
    }
  }

  // ---------- start AP mode ----------
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  IPAddress ip = WiFi.softAPIP();
  logMsg += "AP started → http://" + ip.toString() + "\n";

  // ---------- start web server ----------
  server.on("/", handleRoot);
  server.begin();
}

void loop()
{
  server.handleClient(); // keep serving pages
}