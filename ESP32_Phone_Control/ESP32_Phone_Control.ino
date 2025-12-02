#include <WiFi.h>
#include <WebServer.h>

// ================= 使用者設定區 =================
// 設定熱點 (AP) 的名稱與密碼
// 手機請搜尋此 WiFi 名稱並輸入密碼連線
const char* AP_SSID = "ESP32_LED_Control-01";   //請改成自己喜歡的編號或名稱，不然會跟其他人相同
const char* AP_PASS = "12345678";   //密碼也是可以自己修改

// 設定 LED 腳位 
// 如果您接了外部 LED，請改為對應的 GPIO 23 腳位
const int LED_PIN = 23;
// ==============================================

// 建立 WebServer 物件，開啟 Port 80 (HTTP 預設埠)
WebServer server(80);

// 處理根目錄 '/' 的請求，回傳 HTML 控制頁面
void handleRoot() {
  // 簡單的 HTML 介面，包含 CSS 樣式讓按鈕好按一點
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset=\"utf-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"; // 適合手機瀏覽
  html += "<title>ESP32 Control</title>";
  html += "<style>";
  html += "body { font-family: Helvetica, Arial, sans-serif; text-align: center; margin-top: 50px; background-color: #f0f0f0;}";
  html += "h1 { color: #333; }";
  html += ".button { display: inline-block; padding: 20px 40px; font-size: 24px; cursor: pointer; text-align: center; text-decoration: none; outline: none; color: #fff; background-color: #4CAF50; border: none; border-radius: 15px; box-shadow: 0 5px #999; margin: 20px; -webkit-user-select: none; }";
  html += ".button:active { background-color: #3e8e41; box-shadow: 0 2px #666; transform: translateY(4px); }";
  html += ".off { background-color: #f44336; }";
  html += ".off:active { background-color: #d32f2f; }";
  html += "</style></head><body>";
  
  html += "<h1>ESP32 LED 控制</h1>";
  html += "<p>請點擊下方按鈕控制 LED 開關</p>";
  
  // 按鈕連結到 /on 和 /off
  html += "<a href=\"/on\"><button class=\"button\">開啟 LED (ON)</button></a><br>";
  html += "<a href=\"/off\"><button class=\"button off\">關閉 LED (OFF)</button></a>";
  
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// 處理 '/on' 請求
void handleLedOn() {
  digitalWrite(LED_PIN, HIGH); // 點亮 LED
  Serial.println("接收到指令: LED ON");
  
  // 執行完動作後，重新導回首頁，這樣網址列才不會停在 /on
  server.sendHeader("Location", "/");
  server.send(303);
}

// 處理 '/off' 請求
void handleLedOff() {
  digitalWrite(LED_PIN, LOW); // 熄滅 LED
  Serial.println("接收到指令: LED OFF");
  
  // 執行完動作後，重新導回首頁
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("\n=== ESP32 Phone Control (AP Mode) ===");

  // 1. 設定 GPIO
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // 預設關閉

  // 2. 啟動 SoftAP 模式 (讓 ESP32 變成基地台)
  Serial.print("正在啟動 AP 模式... ");
  // 如果不需要密碼，第二個參數可以填 NULL
  WiFi.softAP(AP_SSID, AP_PASS);

  IPAddress IP = WiFi.softAPIP();
  Serial.println("完成!");
  
  // 3. 顯示連線資訊
  Serial.println("------------------------------------------------");
  Serial.println("請使用手機連接以下 WiFi:");
  Serial.print("  SSID: "); Serial.println(AP_SSID);
  Serial.print("  Pass: "); Serial.println(AP_PASS);
  Serial.println("");
  Serial.println("連線後，請打開手機瀏覽器 (Chrome/Safari) 輸入網址:");
  Serial.print("  http://"); Serial.println(IP);
  Serial.println("------------------------------------------------");

  // 4. 設定 Web Server 路由
  server.on("/", handleRoot);      // 首頁
  server.on("/on", handleLedOn);   // 開燈
  server.on("/off", handleLedOff); // 關燈
  
  // 處理 404 找不到頁面
  server.onNotFound([]() {
    server.send(404, "text/plain", "404: Not Found");
  });

  // 5. 啟動伺服器
  server.begin();
  Serial.println("HTTP 伺服器已啟動");
}

void loop() {
  // 處理客戶端請求
  server.handleClient();
}
