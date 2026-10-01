   #include <WiFi.h>

#define FLASH_LED 4   // ESP32-CAM flash LED

const char* ap_ssid = "ESP32_CAM_TEST";
const char* ap_password = "12345678";   // minimum 8 characters

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32-CAM WiFi AP Debug Starting");
  Serial.println("=================================");

  pinMode(FLASH_LED, OUTPUT);
  digitalWrite(FLASH_LED, LOW);

  Serial.println("[1] LED test...");
  digitalWrite(FLASH_LED, HIGH);
  delay(500);
  digitalWrite(FLASH_LED, LOW);
  Serial.println("[OK] LED pin working");

  Serial.println("[2] Setting WiFi mode to AP...");
  WiFi.disconnect(true);
  delay(1000);

  bool modeSet = WiFi.mode(WIFI_AP);
  if (modeSet) {
    Serial.println("[OK] WiFi mode set to AP");
  } else {
    Serial.println("[ERROR] Failed to set WiFi mode");
  }

  delay(1000);

  Serial.println("[3] Starting Access Point...");
  bool apStarted = WiFi.softAP(ap_ssid, ap_password);

  if (apStarted) {
    Serial.println("[OK] Access Point started successfully");
    Serial.print("SSID: ");
    Serial.println(ap_ssid);
    Serial.print("Password: ");
    Serial.println(ap_password);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("[ERROR] Access Point start failed");
  }

  Serial.println("[4] WiFi status info:");
  Serial.print("WiFi mode: ");
  Serial.println(WiFi.getMode());

  Serial.print("AP SSID: ");
  Serial.println(WiFi.SSID());

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  Serial.println("Setup finished.");
}

void loop() {
  static unsigned long lastPrint = 0;

  if (millis() - lastPrint > 3000) {
    lastPrint = millis();

    Serial.println("----- Runtime Status -----");
    Serial.print("Connected stations: ");
    Serial.println(WiFi.softAPgetStationNum());

    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    digitalWrite(FLASH_LED, !digitalRead(FLASH_LED)); // blink flash LED
  }
}
