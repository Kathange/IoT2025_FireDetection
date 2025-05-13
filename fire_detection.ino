#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Fire Detection Notification"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <FirebaseESP32.h>
#include <time.h>

// -----------------------------
// 感測器腳位定義
// -----------------------------
#define MQ2_AO_PIN     33   // 模擬輸出33
#define MQ2_DO_PIN     26   // 數位輸出26
#define FLAME_DO_PIN   27
#define BUZZER_PIN     14
#define ONE_WIRE_BUS   4    // DS18B20 DQ

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

#define FIREBASE_HOST "YOUR_FIREBASE_HOST"
#define FIREBASE_AUTH "YOUR_AUTH_TOKEN"            // 或使用 Firebase Auth token

FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

// -----------------------------
// DS18B20 初始化
// -----------------------------
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


void initTime() {
  configTime(28800, 0, "pool.ntp.org");  // 台灣時區 UTC+8
  Serial.print("正在同步時間");
  while (time(nullptr) < 100000) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n時間同步完成");
}


String getTimestampPath() {
  time_t now = time(nullptr);
  struct tm* t = localtime(&now);

  char dateStr[9];  // YYYYMMDD
  snprintf(dateStr, sizeof(dateStr), "%04d%02d%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);

  char timeStr[7];  // HHMMSS
  snprintf(timeStr, sizeof(timeStr), "%02d%02d%02d", t->tm_hour, t->tm_min, t->tm_sec);

  return String("/sensors/") + dateStr + "/" + timeStr;
}


// -----------------------------
void setup() {
  Serial.begin(115200);
  sensors.begin();

  pinMode(MQ2_DO_PIN, INPUT);
  pinMode(FLAME_DO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  WiFi.begin(ssid, pass);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // 初始化 Blynk 並連接 Wi-Fi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // 設定 Firebase Config 和 Auth
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // 检查 Firebase 是否准备好
  if (Firebase.ready()) {
    Serial.println("Firebase 連接成功！");
  } else {
    Serial.println("Firebase 連接失敗！");
    Serial.println("錯誤訊息: " + firebaseData.errorReason());
  }

  initTime();
}


void loop() {
  Blynk.run();  // 必須在 loop 中調用，以保持與 Blynk 伺服器的連接

  // 讀取 MQ2 感測器
  int mq2_analog = analogRead(MQ2_AO_PIN);
  int mq2_digital = digitalRead(MQ2_DO_PIN);

  // 讀取火焰感測器
  int flame = digitalRead(FLAME_DO_PIN);

  // 讀取 DS18B20 溫度
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);

  // 讀 MQ2 類比（分壓後）
  float voltage = mq2_analog * (3.3 / 4095.0);  // ESP32 ADC 最大 4095
  float aoEstimate = voltage * 2;  // 分壓回推原始 5V 電壓

  // 顯示在 Serial Monitor
  Serial.println("========== 感測器資料 ==========");
  Serial.print("MQ2 類比值："); Serial.println(mq2_analog);
  Serial.print("MQ2 類比輸出（分壓後）: "); Serial.print(voltage, 3); Serial.println(" V");
  Serial.print("原始 AO 電壓估算："); Serial.print(aoEstimate, 3); Serial.println(" V");
  Serial.print("MQ2 數位警報："); Serial.println(mq2_digital == LOW ? "濃度高！" : "正常");
  Serial.print("火焰偵測："); Serial.println(flame == LOW ? "有火" : "無火");
  Serial.print("溫度："); Serial.print(temperatureC); Serial.println(" °C");
  Serial.println("================================\n");

  // 將數據傳送至 Blynk
  Blynk.setProperty(V0, "color", (mq2_digital == LOW) ? "#F75000" : "#00DB00");
  Blynk.setProperty(V1, "color", (flame == LOW) ? "#F75000" : "#00DB00");
  Blynk.setProperty(V2, "color", "#00DB00");
  Blynk.setProperty(V3, "color", "#00DB00");
  Blynk.setProperty(V4, "color", "#00DB00");
  Blynk.setProperty(V5, "color", "#00DB00");

  Blynk.virtualWrite(V0, (mq2_digital == LOW) ? 255 : mq2_digital);
  Blynk.virtualWrite(V1, (flame == LOW) ? 255 : flame);
  Blynk.virtualWrite(V2, mq2_analog);
  Blynk.virtualWrite(V3, temperatureC);
  Blynk.virtualWrite(V4, voltage);
  Blynk.virtualWrite(V5, aoEstimate);

  // 偵測火災條件（任何一項成立都觸發）
  bool danger =
      flame == LOW &&
      mq2_digital == LOW &&
      aoEstimate > 0.8 &&       // 可依實測調整
      temperatureC > 50;               // 高溫警戒線

  // 當任一感測器偵測到火災狀況，啟動蜂鳴器
  if (danger) {
    digitalWrite(BUZZER_PIN, HIGH);  // 開啟蜂鳴器
    Serial.println("警報！火災或危險氣體偵測中！");
    Blynk.logEvent("fire_alert", "發生火災！！！");
    Serial.println("火災警報已送出！");
  } else {
    digitalWrite(BUZZER_PIN, LOW);   // 關閉蜂鳴器
  }

  String basePath = getTimestampPath();
  Firebase.setFloat(firebaseData, basePath + "/temperature", temperatureC);
  Firebase.setInt(firebaseData, basePath + "/mq2_analog", mq2_analog);
  Firebase.setFloat(firebaseData, basePath + "/aoEstimate", aoEstimate);
  Firebase.setBool(firebaseData, basePath + "/flame", flame == LOW);
  Firebase.setBool(firebaseData, basePath + "/gas_alert", mq2_digital == LOW);

  delay(10000);
}
