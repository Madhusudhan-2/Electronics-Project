#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

// =====================================
// CAMERA MODEL
// =====================================
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// =====================================
// WIFI SETTINGS
// =====================================
const char* ssid = "TP-Link_81E9";
const char* password = "66223252";

// Flask server
const char* serverUrl = "http://192.168.1.103:5000/predict";

// =====================================
// SAFE GPIO CONFIGURATION
// =====================================
// PIR  -> GPIO13
// TRIG -> GPIO14
// ECHO -> GPIO15
// NMOS -> GPIO2

#define PIR_PIN         13
#define TRIG_PIN        14
#define ECHO_PIN        15
#define NMOS_GATE_PIN   2

// =====================================
// VARIABLES
// =====================================
long duration;
float distance;
bool motionState = LOW;

// Camera timer
unsigned long lastCaptureTime = 0;
const unsigned long captureInterval = 7000;

// =====================================
// FENCE STATE MACHINE
// =====================================
enum FenceState {
  FENCE_IDLE,
  FENCE_HIGH,
  FENCE_LOW
};

FenceState fenceState = FENCE_IDLE;

unsigned long fenceStateStart = 0;
int fenceCycle = 0;

// =====================================
// CAMERA INITIALIZATION
// =====================================
void startCamera() {

  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;

  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Stable settings
  config.frame_size = FRAMESIZE_QQVGA;
  config.jpeg_quality = 40;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.print("Camera Init Failed: ");
    Serial.println(err);

    delay(3000);
    ESP.restart();
  }

  Serial.println("Camera initialized");
}

// =====================================
// DISTANCE FUNCTION
// =====================================
float getDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  distance = duration * 0.034 / 2.0;

  return distance;
}

// =====================================
// SEND IMAGE TO FLASK
// =====================================
void captureAndSendImage() {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi lost reconnecting");

    WiFi.reconnect();
    return;
  }

  camera_fb_t * fb = esp_camera_fb_get();

  // SAFETY CHECK
  if (!fb || fb->len == 0) {

    Serial.println("Camera capture failed");

    if (fb) {
      esp_camera_fb_return(fb);
    }

    esp_camera_deinit();

    delay(2000);

    startCamera();

    return;
  }

  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverUrl);

  http.addHeader("Content-Type", "image/jpeg");

  http.setTimeout(8000);

  Serial.println("Sending image...");

  int response = http.POST(fb->buf, fb->len);

  Serial.print("Server Response: ");
  Serial.println(response);

  if (response == 200) {
    Serial.println("Image sent successfully");
  } else {
    Serial.println("Send failed");
  }

  http.end();

  esp_camera_fb_return(fb);
}

// =====================================
// HANDLE PIR + ULTRASONIC + NMOS
// =====================================
void handleFenceSystem() {

  int motion = digitalRead(PIR_PIN);

  // NEW MOTION
  if (motion == HIGH && motionState == LOW) {

    motionState = HIGH;

    Serial.println("Motion Detected");

    float dist = getDistance();

    if (dist > 0) {

      Serial.print("Distance: ");
      Serial.print(dist);
      Serial.println(" cm");

    } else {

      Serial.println("No Echo Received");
    }

    // START CYCLE
    fenceCycle = 1;

    fenceState = FENCE_HIGH;

    fenceStateStart = millis();

    Serial.print("Cycle ");
    Serial.println(fenceCycle);

    Serial.println("MOS HIGH");

    digitalWrite(NMOS_GATE_PIN, HIGH);
  }

  // MOTION ENDED
  if (motion == LOW && motionState == HIGH) {

    motionState = LOW;

    Serial.println("Motion Ended");
  }

  // STATE MACHINE
  switch (fenceState) {

    case FENCE_IDLE:

      digitalWrite(NMOS_GATE_PIN, LOW);

      break;

    case FENCE_HIGH:

      // HIGH FOR 500ms
      if (millis() - fenceStateStart >= 500) {

        Serial.println("MOS LOW");

        digitalWrite(NMOS_GATE_PIN, LOW);

        fenceState = FENCE_LOW;

        fenceStateStart = millis();
      }

      break;

    case FENCE_LOW:

      // LOW FOR 1000ms
      if (millis() - fenceStateStart >= 1000) {

        if (fenceCycle < 3) {

          fenceCycle++;

          Serial.print("Cycle ");
          Serial.println(fenceCycle);

          Serial.println("MOS HIGH");

          digitalWrite(NMOS_GATE_PIN, HIGH);

          fenceState = FENCE_HIGH;

          fenceStateStart = millis();

        } else {

          Serial.println("Fence cycle completed");

          fenceState = FENCE_IDLE;

          digitalWrite(NMOS_GATE_PIN, LOW);
        }
      }

      break;
  }
}

// =====================================
// SETUP
// =====================================
void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("System Ready");

  // PIN MODES
  pinMode(PIR_PIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);

  pinMode(ECHO_PIN, INPUT);

  pinMode(NMOS_GATE_PIN, OUTPUT);

  // INITIAL STATES
  digitalWrite(TRIG_PIN, LOW);

  digitalWrite(NMOS_GATE_PIN, LOW);

  // =====================================
  // WIFI CONNECT
  // =====================================
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  WiFi.setSleep(false);

  // =====================================
  // CAMERA START
  // =====================================
  startCamera();

  // START TIMER
  lastCaptureTime = millis();
}

// =====================================
// MAIN LOOP
// =====================================
void loop() {

  // Fence system runs continuously
  handleFenceSystem();

  // Capture image every 7 seconds
  if (millis() - lastCaptureTime >= captureInterval) {

    lastCaptureTime = millis();

    captureAndSendImage();
  }

  delay(20);
}