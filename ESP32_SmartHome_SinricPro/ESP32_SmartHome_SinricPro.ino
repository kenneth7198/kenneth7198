/*
 * 專案名稱: ESP32_SmartHome_SinricPro
 * 功能: 透過 Google Home / Alexa / Siri Shortcuts 語音控制 LED
 * 
 * === 使用教學 ===
 * 1. 安裝程式庫:
 *    在 Arduino IDE 中，前往 [草稿碼] -> [匯入程式庫] -> [管理程式庫]
 *    搜尋 "SinricPro" 並安裝 (作者: Sinric Pro)。
 *    
 * 2. 取得金鑰 (免費):
 *    前往 https://sinric.pro/ 註冊帳號。
 *    登入後，點擊 "Devices" -> "Add Device"。
 *    - Name: "Living Room Light" (這會是你語音指令的名字)
 *    - Description: 隨意
 *    - Device Type: "Switch"
 *    點擊 Next/Save。
 *    
 *    接著點擊左側 "Credentials"，你會看到:
 *    - App Key
 *    - App Secret
 *    回到 "Devices" 頁面，你會看到剛剛建立裝置的:
 *    - Device ID
 *    
 *    將這三個資訊填入下方程式碼對應位置。
 *    
 * 3. 連結 Google Home:
 *    打開手機 "Google Home" App。
 *    點擊左上角 "+" -> "設定裝置" -> "與 Google 服務連結"。
 *    搜尋 "Sinric Pro"，登入你的帳號。
 *    完成後，你就可以對手機說: "Hey Google, turn on Living Room Light"。
 *    
 * 4. 連結 Siri (iPhone):
 *    下載 "Sinric Pro" App。
 *    在 App 中建立 "Scenes" (場景)，例如 "Light On"。
 *    點擊 "Add to Siri"，設定語音指令 "開燈"。
 *    之後就可以對手機說: "Hey Siri, 開燈"。
 */

#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

// ================= 使用者設定區 =================
const char* WIFI_SSID = "Techart_AIoT";
const char* WIFI_PASS = "0908906870@a";

// 請將 Sinric Pro 網站上的資訊填入這裡
#define APP_KEY           "7387151c-26b3-4b62-9686-1b2003d76aba"      
#define APP_SECRET        "f7f0ac09-65be-45a4-9ae5-7ba5da12dcfd-e38f9656-5e32-426b-b897-a43c69fa2342"   
#define SWITCH_ID         "692ef5bc00f870dd77c6356e"    

// 控制的 LED 腳位
const int LED_PIN = 23;
// ==============================================

// 當收到開關指令時會執行此函式
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("接收到指令 - 裝置 ID: %s, 狀態: %s\r\n", deviceId.c_str(), state ? "開啟 (ON)" : "關閉 (OFF)");
  
  // 控制 LED
  digitalWrite(LED_PIN, state ? HIGH : LOW);
  
  return true; // 回傳 true 告訴伺服器執行成功
}

void setup() {
  Serial.begin(9600);
  
  // 設定 GPIO
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 連接 WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("正在連線 WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi 已連線");
  Serial.print("IP 位址: ");
  Serial.println(WiFi.localIP());

  // 設定 Sinric Pro 裝置
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];
  mySwitch.onPowerState(onPowerState);

  // 建立與 Sinric Pro 伺服器的連線
  SinricPro.begin(APP_KEY, APP_SECRET);
  Serial.println("Sinric Pro 服務已啟動，等待指令...");
}

void loop() {
  // 處理 Sinric Pro 的連線與指令
  SinricPro.handle();
}
