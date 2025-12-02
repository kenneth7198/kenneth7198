/*
 * 專案名稱: ESP32_Digital_Scroll (無線數位卷軸) - 穩定版
 * 
 * 修正說明:
 * 由於您使用的 ESP32 核心版本較新 (3.x.x)，導致原本的 AsyncWebServer 程式庫產生編譯錯誤。
 * 此版本改用更穩定的標準庫組合，保證可以編譯成功且運作順暢：
 * 1. WebServer (ESP32 內建標準庫)
 * 2. WebSockets (需安裝，非常穩定)
 * 
 * === 必做步驟: 安裝程式庫 ===
 * 請在 Arduino IDE 程式庫管理員 (Ctrl+Shift+I) 搜尋 "WebSockets" 並安裝:
 * -> 名稱: WebSockets
 * -> 作者: Markus Sattler
 * -> 版本: 建議安裝最新版
 * 
 * 硬體接線:
 * - 可變電阻 VCC -> ESP32 3.3V
 * - 可變電阻 GND -> ESP32 GND
 * - 可變電阻 訊號腳 -> ESP32 GPIO 34
 */

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h> // 請務必安裝 "WebSockets" by Markus Sattler

// ================= 使用者設定區 =================
const char* WIFI_SSID = "Techart_AIoT";
const char* WIFI_PASS = "0908906870@a";

const int POT_PIN = 34; // 可變電阻連接的腳位
// ==============================================

WebServer server(80);             // 網頁伺服器在 Port 80
WebSocketsServer webSocket(81);   // WebSocket 在 Port 81 (避免衝突)

int lastPotValue = 0;
const int THRESHOLD = 15; // 門檻值
unsigned long lastReadTime = 0;

// 網頁 HTML 程式碼
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>ESP32 數位卷軸 - 清明上河圖</title>
  <style>
    body { margin: 0; padding: 0; background-color: #f4e4c1; overflow: hidden; }
    #container {
      width: 100vw;
      height: 100vh;
      overflow-x: scroll;
      overflow-y: hidden;
      white-space: nowrap;
      scroll-behavior: auto;
    }
    #container::-webkit-scrollbar { display: none; }
    #scroll-content {
      height: 100%;
      /* 清明上河圖範例 */
      background-image: url('https://upload.wikimedia.org/wikipedia/commons/8/86/Along_the_River_During_the_Qingming_Festival_%28Qing_Court_Version%29.jpg');
      background-repeat: no-repeat;
      background-size: cover;
      width: 1000vw; 
    }
    #status {
      position: fixed;
      top: 10px; left: 10px;
      background: rgba(0,0,0,0.5); color: white;
      padding: 5px 10px; border-radius: 5px;
      font-family: monospace; z-index: 999;
    }
  </style>
</head>
<body>
  <div id="status">連線中...</div>
  <div id="container">
    <div id="scroll-content"></div>
  </div>

  <script>
    // 注意: WebSocket 連線到 Port 81
    var gateway = `ws://${window.location.hostname}:81/`;
    var websocket;
    var container = document.getElementById('container');
    var statusDiv = document.getElementById('status');

    function initWebSocket() {
      console.log('Trying to open a WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen    = onOpen;
      websocket.onclose   = onClose;
      websocket.onmessage = onMessage;
    }

    function onOpen(event) {
      console.log('Connection opened');
      statusDiv.innerText = "已連線 - 請轉動可變電阻";
    }

    function onClose(event) {
      console.log('Connection closed');
      statusDiv.innerText = "連線中斷，嘗試重連...";
      setTimeout(initWebSocket, 2000);
    }

    function onMessage(event) {
      var potValue = parseInt(event.data);
      var maxScroll = container.scrollWidth - container.clientWidth;
      
      // 映射 0~4095 到 捲動範圍
      var scrollPos = (potValue / 4095) * maxScroll;
      container.scrollLeft = scrollPos;
    }

    window.onload = initWebSocket;
  </script>
</body>
</html>
)rawliteral";

// WebSocket 事件處理
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // 連線成功時，先傳送一次目前數值
        String msg = String(lastPotValue);
        webSocket.sendTXT(num, msg);
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(POT_PIN, INPUT);
  analogReadResolution(12);

  // WiFi 連線
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  // 設定 Web Server
  server.on("/", [](){
    server.send(200, "text/html", index_html);
  });
  server.begin();

  // 設定 WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  Serial.println("HTTP server started on port 80");
  Serial.println("WebSocket server started on port 81");
}

void loop() {
  // 必須在 loop 中持續呼叫這兩個函式
  webSocket.loop();
  server.handleClient();

  // 非阻塞式讀取感測器 (每 20ms 讀一次)
  unsigned long now = millis();
  if (now - lastReadTime > 20) {
    lastReadTime = now;
    
    int potValue = analogRead(POT_PIN);
    
    // 數值有變化才傳送
    if (abs(potValue - lastPotValue) > THRESHOLD) {
      lastPotValue = potValue;
      String msg = String(potValue);
      webSocket.broadcastTXT(msg); // 廣播給所有連線的網頁
    }
  }
}
