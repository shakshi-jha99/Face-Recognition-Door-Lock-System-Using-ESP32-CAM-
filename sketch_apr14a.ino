 #include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// =========================
// WiFi Credentials (EDIT)
// =========================
const char* ssid = "Shakshi";
const char* password = "Shakshi12234";

// =========================
// Web Server
// =========================
WebServer server(80);

// =========================
// Camera Init
// =========================
void startCamera() {
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;

  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;

  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;

  config.pin_pwdn = 32;
  config.pin_reset = -1;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // ✅ Stable settings (no fb ovf)
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("❌ Camera init failed: 0x%x\n", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);

  Serial.println("✅ Camera initialized");
}

// =========================
// Stream Handler
// =========================
void handleJPGStream() {
  WiFiClient client = server.client();

  String response =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (client.connected()) {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) continue;

    server.sendContent("--frame\r\n");
    server.sendContent("Content-Type: image/jpeg\r\n");
    server.sendContent("Content-Length: " + String(fb->len) + "\r\n\r\n");

    client.write(fb->buf, fb->len);
    server.sendContent("\r\n");

    esp_camera_fb_return(fb);

    if (!client.connected()) break;

    delay(60); // prevent overflow
  }
}

// =========================
// Web Page
// =========================
void handleRoot() {
  String html = R"rawliteral(
    <html>
      <head>
        <title>ESP32-CAM</title>
      </head>
      <body style="text-align:center;">
        <h2>ESP32-CAM Live Stream</h2>
        <img src="/stream" style="width:90%;max-width:640px;">
      </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// =========================
// Start Server
// =========================
void startCameraServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/stream", HTTP_GET, handleJPGStream);
  server.begin();
  Serial.println("✅ Server started");
}

// =========================
// Setup (FIXED ORDER)
// =========================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n🚀 START");

  // ✅ CONNECT WIFI FIRST
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retry++;

    if (retry > 20) {
      Serial.println("\n❌ WiFi FAILED");
      return;
    }
  }

  Serial.println("\n✅ WiFi connected");
  Serial.print("🌐 IP: ");
  Serial.println(WiFi.localIP());

  // ✅ START CAMERA AFTER WIFI
  Serial.println("Starting Camera...");
  startCamera();
  delay(1000);

  // Start server
  startCameraServer();
}

// =========================
// Loop
// =========================
void loop() {
  server.handleClient();
}
