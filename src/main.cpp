#include <WiFi.h>
#include <HTTPClient.h>
#include <MFRC522.h>
#include <Wire.h>
#include <FS.h>

#define SS_PIN 21
#define RST_PIN 18
#define SCK_PIN 22
#define MISO_PIN 19
#define MOSI_PIN 23
#define WIFI_SSID "HUAWEI P40 Pro"
#define WIFI_PASSWORD "88888888"
#define SERVER_URL "http://192.168.43.223:8000/tags/add/"

MFRC522 mfrc522(SS_PIN, RST_PIN);

void sendUidToServer(String uid) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST("uid=" + uid);
    if (httpResponseCode>0) {
      String response = http.getString();
        Serial.println(httpResponseCode);  // 打印HTTP响应码
        Serial.println(response);  // 打印HTTP响应内容
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  mfrc522.PCD_Init();
  bool selfTestResult = mfrc522.PCD_PerformSelfTest();
  if (selfTestResult) {
    Serial.println("Self test passed, RC522 is connected.");
  } else {
    Serial.println("Self test failed, RC522 is not connected.");
  }
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += mfrc522.uid.uidByte[i] < 0x10 ? "0" : "";
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  Serial.println("RFID card detected. UID: " + uid);
  sendUidToServer(uid);
  delay(1000);
}