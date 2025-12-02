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
      background-color: #333;
      display: flex;            /* 使用 Flexbox 排版 */
      align-items: center;      /* 垂直置中 */
      justify-content: flex-start; /* 靠左對齊 */
      overflow-x: hidden;       /* 隱藏水平捲軸 (由程式控制捲動) */
      overflow-y: hidden;
    }
    #container::-webkit-scrollbar { display: none; }
    
    /* 圖片設定: 高度固定為視窗的一半 (1/2) */
    #scroll-img {
      height: 50vh;     /* 高度佔螢幕 50% */
      width: auto;      /* 寬度自動調整，保持比例 */
      flex-shrink: 0;   /* 關鍵: 禁止圖片縮小，確保能超出畫面 */
      box-shadow: 0 0 20px rgba(0,0,0,0.5);
    }

    #status {
      position: fixed;
      top: 10px; left: 10px;
      background: rgba(0,0,0,0.7); color: white;
      padding: 5px 10px; border-radius: 5px;
      font-family: monospace; z-index: 999;
    }
  </style>
</head>
<body>
  <div id="status">連線中...</div>
  <div id="container">
    <!-- 圖片來源設定 (已改為較小的 5000px 版本，載入速度較快) -->
    <!-- 
      原始大圖 (58MB): https://upload.wikimedia.org/wikipedia/commons/2/2c/Along_the_River_During_the_Qingming_Festival_%28Qing_Court_Version%29.jpg
      中解析度 (5000px): https://upload.wikimedia.org/wikipedia/commons/thumb/2/2c/Along_the_River_During_the_Qingming_Festival_%28Qing_Court_Version%29.jpg/5000px-Along_the_River_During_the_Qingming_Festival_%28Qing_Court_Version%29.jpg
    -->
    <img id="scroll-img" 
         src="https://upload.wikimedia.org/wikipedia/commons/thumb/2/2c/Along_the_River_During_the_Qingming_Festival_%28Qing_Court_Version%29.jpg/5000px-Along_the_River_During_the_Qingming_Festival_%28Qing_Court_Version%29.jpg" 
         alt="圖片載入失敗，請確認您的裝置可以連上網際網路 (外網)"
         onload="document.getElementById('status').innerText += ' (圖片已載入)'"
         onerror="document.getElementById('status').innerText = '錯誤: 無法載入圖片，請檢查網路連線'"
    >
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
      statusDiv.innerText = "WebSocket 已連線";
    }

    function onClose(event) {
      console.log('Connection closed');
      statusDiv.innerText = "連線中斷，嘗試重連...";
      setTimeout(initWebSocket, 2000);
    }

    function onMessage(event) {
      var potValue = parseInt(event.data);
      
      // 計算最大可捲動範圍 (圖片總寬 - 視窗寬)
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
    
    // 讀取原始數值
    int rawValue = analogRead(POT_PIN);
    
    // === 平滑濾波處理 (解決數值跳動問題) ===
    // 使用 EMA (指數移動平均) 演算法
    // 0.9 = 舊數值權重 (越高越平滑，但反應變慢)
    // 0.1 = 新數值權重
    static float smoothedValue = 0;
    if (smoothedValue == 0) smoothedValue = rawValue; // 初始化
    smoothedValue = 0.9 * smoothedValue + 0.1 * rawValue;
    
    int currentPotValue = (int)smoothedValue;
    
    // 數值有變化才傳送 (門檻值可依需求微調，例如改為 5 或 10)
    if (abs(currentPotValue - lastPotValue) > 5) {
      lastPotValue = currentPotValue;
      String msg = String(currentPotValue);
      webSocket.broadcastTXT(msg); // 廣播給所有連線的網頁
    }
  }
}
