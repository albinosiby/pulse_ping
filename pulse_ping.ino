#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// WiFi
#define WIFI_SSID     "tester"
#define WIFI_PASSWORD "12345678"

// Firebase REST endpoint
#define FIREBASE_URL "https://ping-pulse-default-rtdb.firebaseio.com/ping_urls.json"

// LED
#define LED_PIN 2

unsigned long lastPing = 0;
unsigned long lastFetch = 0;

std::vector<String> urlList;

// -------------------------------
// LED Blink
// -------------------------------
void blinkPattern(int on, int off, int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, LOW); delay(on);
    digitalWrite(LED_PIN, HIGH); delay(off);
  }
}

void idleBlink()    { blinkPattern(50, 950, 1); }
void fetchBlink()   { blinkPattern(100, 100, 3); }
void pingBlink()    { blinkPattern(200, 200, 1); }
void successBlink() { blinkPattern(80, 80, 5); }
void errorBlink()   { blinkPattern(300, 300, 3); }


// -------------------------------
// FETCH URLS FROM FIREBASE REST API
// -------------------------------
void fetchUrls() {
  Serial.println("\nFetching URL List...");
  fetchBlink();

  urlList.clear();

  WiFiClientSecure client;
  client.setInsecure();  // allow HTTPS without certificate

  HTTPClient http;

  if (http.begin(client, FIREBASE_URL)) {
    int code = http.GET();

    if (code == 200) {
      String payload = http.getString();
      Serial.println("Firebase Response:");
      Serial.println(payload);

      // Parse JSON
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, payload);

      for (JsonPair kv : doc.as<JsonObject>()) {
        String url = kv.value().as<String>();
        urlList.push_back(url);

        Serial.println("Loaded URL: " + url);
      }

      successBlink();
    } 
    else {
      Serial.println("HTTP Error: " + String(code));
      errorBlink();
    }
    http.end();
  } 
  else {
    Serial.println("Failed to connect to Firebase!");
    errorBlink();
  }

  Serial.println("Total URLs: " + String(urlList.size()));
}


// -------------------------------
// PING ALL URLS
// -------------------------------
void pingUrls() {
  if (urlList.size() == 0) {
    Serial.println("URL list empty! Reloading...");
    fetchUrls();
    return;
  }

  Serial.println("\n--- Starting Pings ---");

  WiFiClientSecure client;
  client.setInsecure();

  for (String url : urlList) {
    Serial.println("Pinging: " + url);
    pingBlink();

    HTTPClient http;

    if (http.begin(client, url)) {
      int code = http.GET();
      Serial.println("Response: " + String(code));

      if (code > 0) successBlink();
      else errorBlink();

      http.end();
    } 
    else {
      Serial.println("Failed to begin HTTP request!");
      errorBlink();
    }

    delay(300);
  }
  Serial.println("--- Ping Finished ---\n");
}


// -------------------------------
// SETUP
// -------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    idleBlink();
  }

  Serial.println("\nWiFi Connected!");
  Serial.println(WiFi.localIP());

  successBlink();

  fetchUrls();
}


// -------------------------------
// LOOP
// -------------------------------
void loop() {
  if (millis() - lastPing > 30000) {
    lastPing = millis();
    pingUrls();
  }

  if (millis() - lastFetch > 120000) {
    lastFetch = millis();
    fetchUrls();
  }

  idleBlink();
}
