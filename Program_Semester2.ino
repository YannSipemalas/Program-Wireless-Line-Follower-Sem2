#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// --- Definisi Pin ---
const int S1_sens = 14; // Sensor paling kiri
const int S2_sens = 12; // Sensor kiri
const int S3_sens = 13; // Sensor tengah
const int S4_sens = 15; // Sensor kanan
const int S5_sens = 10; // Sensor paling kanan

const int pwmL = 5;   // Pin PWM motor kiri
const int pwmR = 4;   // Pin PWM motor kanan
const int rotL = 0;   // Pin arah motor kiri
const int rotR = 2;   // Pin arah motor kanan

const int servoPin = 16;   // Pin untuk servo lengan/kepala
const int gripperPin = 3;  // Pin untuk servo gripper (DIPERBAIKI dari GPIO3)

// --- Kredensial WiFi (Mode AP) ---
const char* ssid_ap = "ESP";
const char* password_ap = "12345678";

// --- Objek Global ---
ESP8266WebServer server(80);
Servo myServo;
Servo gripperServo;

// --- Variabel Kontrol ---
int leftSpeed = 0;
int rightSpeed = 0;
bool lineFollowerActive = false;
bool manualControl = false;
bool robotOn = false; // Status global robot

// --- Konstanta Motor ---
const int FORWARD_SPEED_LF = 150;
const int TURN_SPEED_LF = 150;

// HTML Page dengan JavaScript yang diperbaiki
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Kontrol Robot ESP8266</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  <style>
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      text-align: center;
      background-color: #021f3f;
      color: white;
      margin: 0;
      padding: 20px;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      min-height: 100vh;
    }
    h1 {
      color: #FFD700;
      margin-bottom: 20px;
    }
    .section-title {
      color: #ADD8E6;
      margin-top: 20px;
      margin-bottom: 10px;
      font-size: 1.2em;
    }
    .button-group {
      display: flex;
      flex-wrap: wrap;
      justify-content: center;
      gap: 10px;
      margin-bottom: 15px;
    }
    button {
      width: 100px;
      height: 50px;
      margin: 5px;
      border: none;
      border-radius: 8px;
      font-size: 1em;
      font-weight: bold;
      cursor: pointer;
      transition: all 0.3s ease;
      box-shadow: 2px 2px 5px rgba(0,0,0,0.3);
      color: black;
    }
    button:active {
      transform: translateY(1px);
      box-shadow: 1px 1px 3px rgba(0,0,0,0.3);
    }
    button.control-btn { background-color: #FFA500; }
    button.stop-btn { background-color: #DC143C; color: white; }
    button.toggle-btn { background-color: #28a745; color: white; }
    button.toggle-btn.off { background-color: #dc3545; }
    button.mode-btn { background-color: #FFD700; }
    button.servo-btn { background-color: #007bff; color: white; }
    button.gripper-btn { background-color: #9370DB; color: white; }
    button:disabled {
      background-color: #6c757d;
      cursor: not-allowed;
      box-shadow: none;
      opacity: 0.6;
    }
    .speed-slider {
      width: 80%;
      max-width: 300px;
      margin: 15px auto;
      -webkit-appearance: none;
      height: 10px;
      border-radius: 5px;
      background: #555;
      outline: none;
    }
    .speed-slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 20px;
      height: 20px;
      border-radius: 50%;
      background: #4CAF50;
      cursor: pointer;
    }
    #statusLabel {
      margin-top: 20px;
      font-size: 1.1em;
      font-weight: bold;
      min-height: 25px;
      padding: 10px;
      border-radius: 5px;
      background-color: rgba(255,255,255,0.1);
    }
    .ip-display {
      margin-top: 10px;
      font-size: 0.9em;
      color: #bbb;
    }
    .connection-status {
      margin: 10px 0;
      padding: 5px 10px;
      border-radius: 15px;
      font-size: 0.9em;
    }
    .connected { background-color: #28a745; color: white; }
    .disconnected { background-color: #dc3545; color: white; }
  </style>
</head>
<body>
  <h1>🤖 Kontrol Robot ESP8266</h1>
  <div class="ip-display" id="ipAddress">IP Robot: 192.168.4.1</div>
  <div class="connection-status disconnected" id="connectionStatus">Mengecek koneksi...</div>

  <input type="range" min="50" max="255" value="150" class="speed-slider" id="kecepatanSlider">
  <label for="kecepatanSlider" id="kecepatanLabel">Kecepatan: 150</label>

  <button class="toggle-btn" id="robotOnOffButton" onclick="toggleRobot()">🔴 OFF</button>

  <p class="section-title">🎮 Kontrol Gerak</p>
  <div class="button-group">
    <button class="control-btn" id="btnMaju" ontouchstart="maju()" ontouchend="berhentiRobot()" onmousedown="maju()" onmouseup="berhentiRobot()">⬆ Maju</button>
  </div>
  <div class="button-group">
    <button class="control-btn" id="btnKiri" ontouchstart="kiri()" ontouchend="berhentiRobot()" onmousedown="kiri()" onmouseup="berhentiRobot()">⬅ Kiri</button>
    <button class="stop-btn" id="btnBerhenti" onclick="berhentiRobot()">⏹ Stop</button>
    <button class="control-btn" id="btnKanan" ontouchstart="kanan()" ontouchend="berhentiRobot()" onmousedown="kanan()" onmouseup="berhentiRobot()">➡ Kanan</button>
  </div>
  <div class="button-group">
    <button class="control-btn" id="btnMundur" ontouchstart="mundur()" ontouchend="berhentiRobot()" onmousedown="mundur()" onmouseup="berhentiRobot()">⬇ Mundur</button>
  </div>

  <button class="mode-btn" id="lineFollowerButton" onclick="toggleLineFollower()">🛤 Mulai Line Follower</button>

  <p class="section-title">🦾 Kontrol Servo</p>
  <div class="button-group">
    <button class="servo-btn" id="btnServoNaik" onclick="kirimServo(150)">⬆ Naik (180°)</button>
    <button class="servo-btn" id="btnServoTengah" onclick="kirimServo(90)">➡ Tengah (90°)</button>
    <button class="servo-btn" id="btnServoTurun" onclick="kirimServo(0)">⬇ Turun (0°)</button>
  </div>

  <p class="section-title">🤏 Kontrol Gripper</p>
  <div class="button-group">
    <button class="gripper-btn" id="btnGripperTengah" onclick="kirimGripper(90)">✊TUTUP</button>
    <button class="gripper-btn" id="btnGripperTutup" onclick="kirimGripper(180)">🤏BUKA</button>
  </div>

  <div id="statusLabel">Robot Tidak Aktif</div>

  <script>
    const ROBOT_IP = "192.168.4.1";
    let robotActive = false;
    let lineFollowerActive = false;
    let isConnected = false;

    // Elemen UI
    const kecepatanSlider = document.getElementById('kecepatanSlider');
    const kecepatanLabel = document.getElementById('kecepatanLabel');
    const robotOnOffButton = document.getElementById('robotOnOffButton');
    const lineFollowerButton = document.getElementById('lineFollowerButton');
    const statusLabel = document.getElementById('statusLabel');
    const connectionStatus = document.getElementById('connectionStatus');

    const controlButtons = [
      document.getElementById('btnMaju'),
      document.getElementById('btnMundur'),
      document.getElementById('btnKiri'),
      document.getElementById('btnKanan'),
      document.getElementById('btnBerhenti')
    ];
    const servoButtons = [
      document.getElementById('btnServoNaik'),
      document.getElementById('btnServoTengah'),
      document.getElementById('btnServoTurun')
    ];
    const gripperButtons = [
      document.getElementById('btnGripperBuka'),
      document.getElementById('btnGripperTutup'),
      document.getElementById('btnGripperTengah')
    ];

    // Event listener untuk slider
    kecepatanSlider.oninput = function() {
      kecepatanLabel.innerHTML = "Kecepatan: " + this.value;
    }

    // Fungsi pengiriman perintah yang diperbaiki
    function sendCommand(path, callback, errorCallback) {
      const url = `http://${ROBOT_IP}${path}`;
      console.log("Mengirim:", url);
      
      const timeoutId = setTimeout(() => {
        updateConnectionStatus(false);
        if (errorCallback) errorCallback();
      }, 5000);

      fetch(url, {
        method: 'GET',
        cache: 'no-cache'
      })
      .then(response => {
        clearTimeout(timeoutId);
        if (!response.ok) {
          throw new Error(`HTTP ${response.status}`);
        }
        updateConnectionStatus(true);
        return response.text();
      })
      .then(data => {
        console.log("Response:", data);
        if (callback) callback(data);
      })
      .catch(error => {
        clearTimeout(timeoutId);
        console.error("Error:", error);
        updateConnectionStatus(false);
        updateStatusLabel("❌ Koneksi Error!", "red");
        if (errorCallback) errorCallback();
      });
    }

    function updateConnectionStatus(connected) {
      isConnected = connected;
      if (connected) {
        connectionStatus.textContent = "🟢 Terhubung BRE";
        connectionStatus.className = "connection-status connected";
      } else {
        connectionStatus.textContent = "🔴 Hubungkan BRE";
        connectionStatus.className = "connection-status disconnected";
      }
    }

    function updateStatusLabel(text, color = "white") {
      statusLabel.textContent = text;
      statusLabel.style.color = color;
    }

    function toggleRobot() {
      if (!isConnected) {
        updateStatusLabel("❌ Tidak terhubung ke robot!", "red");
        return;
      }

      const newState = !robotActive;
      const state = newState ? 'on' : 'off';
      
      updateStatusLabel("🔄 Mengubah status robot...", "orange");
      
      sendCommand(`/toggleRobot?status=${state}`, (response) => {
        robotActive = newState;
        const buttonText = robotActive ? '🟢 ON' : '🔴 OFF';
        const statusText = robotActive ? '✅ Robot Aktif' : '❌ Robot Tidak Aktif';
        const statusColor = robotActive ? 'green' : 'red';

        // Update tombol kontrol
        controlButtons.forEach(btn => btn.disabled = !robotActive);
        servoButtons.forEach(btn => btn.disabled = !robotActive);
        gripperButtons.forEach(btn => btn.disabled = !robotActive);
        lineFollowerButton.disabled = !robotActive;

        robotOnOffButton.textContent = buttonText;
        robotOnOffButton.className = `toggle-btn ${robotActive ? '' : 'off'}`;
        updateStatusLabel(statusText, statusColor);

        // Reset line follower jika robot dimatikan
        if (!robotActive && lineFollowerActive) {
          lineFollowerActive = false;
          lineFollowerButton.textContent = '🛤 Mulai Line Follower';
        }
      }, () => {
        updateStatusLabel("❌ Gagal mengubah status robot", "red");
      });
    }

    function toggleLineFollower() {
      if (!robotActive) {
        updateStatusLabel("⚠ Aktifkan robot terlebih dahulu!", "orange");
        return;
      }

      lineFollowerActive = !lineFollowerActive;
      const status = lineFollowerActive ? 'on' : 'off';
      const buttonText = lineFollowerActive ? '⏹ Stop Line Follower' : '🛤 Mulai Line Follower';
      const statusText = lineFollowerActive ? '🛤 Line Follower Aktif' : '⏹ Line Follower Tidak Aktif';
      const statusColor = lineFollowerActive ? 'green' : 'orange';

      sendCommand(`/linefollower?status=${status}`, () => {
        lineFollowerButton.textContent = buttonText;
        updateStatusLabel(statusText, statusColor);
        // Disable kontrol manual saat line follower aktif
        controlButtons.slice(0, -1).forEach(btn => btn.disabled = lineFollowerActive || !robotActive);
        // Stop button tetap aktif
        document.getElementById('btnBerhenti').disabled = !robotActive;
      });
    }

    function getSpeed() {
      return parseInt(kecepatanSlider.value);
    }

    function maju() {
      if (robotActive && !lineFollowerActive) {
        sendCommand(`/control?left=${getSpeed()}&right=${getSpeed()}`);
      }
    }
    function mundur() {
      if (robotActive && !lineFollowerActive) {
        sendCommand(`/control?left=${-getSpeed()}&right=${-getSpeed()}`);
      }
    }
    function kiri() {
      if (robotActive && !lineFollowerActive) {
        sendCommand(`/control?left=${-getSpeed()}&right=${getSpeed()}`);
      }
    }
    function kanan() {
      if (robotActive && !lineFollowerActive) {
        sendCommand(`/control?left=${getSpeed()}&right=${-getSpeed()}`);
      }
    }
    function berhentiRobot() {
      if (robotActive) {
        sendCommand(`/control?left=0&right=0`);
      }
    }

    function kirimServo(angle) {
      if (robotActive) {
        sendCommand(`/servo?angle=${angle}`, () => {
          updateStatusLabel(`🦾 Servo diatur ke ${angle}°`, "green");
        });
      }
    }

    function kirimGripper(position) {
      if (robotActive) {
        const actionText = position === 20 ? "dibuka" : position === 180 ? "ditutup" : "ke tengah";
        sendCommand(`/gripper?position=${position}`, () => {
          updateStatusLabel(`🤏 Gripper ${actionText}`, "green");
        });
      }
    }

    // Test koneksi
    function testConnection() {
      updateStatusLabel("🔄 Mengecek koneksi...", "orange");
      sendCommand('/', () => {
        updateStatusLabel("✅ Koneksi berhasil - Robot siap!", "green");
        setTimeout(() => {
          if (!robotActive) {
            updateStatusLabel("❌ Robot Tidak Aktif", "red");
          }
        }, 2000);
      }, () => {
        updateStatusLabel("❌ Koneksi gagal - Periksa WiFi ESP-Robot", "red");
      });
    }

    // Inisialisasi saat halaman dimuat
    window.onload = function() {
      // Set status awal
      robotActive = false;
      robotOnOffButton.textContent = '🔴 OFF';
      robotOnOffButton.className = 'toggle-btn off';
      updateStatusLabel("❌ Robot Tidak Aktif", "red");
      
      // Disable semua tombol kontrol
      controlButtons.forEach(btn => btn.disabled = true);
      servoButtons.forEach(btn => btn.disabled = true);
      gripperButtons.forEach(btn => btn.disabled = true);
      lineFollowerButton.disabled = true;
      
      // Test koneksi
      testConnection();
      
      // Auto reconnect setiap 10 detik jika terputus
      setInterval(() => {
        if (!isConnected) {
          testConnection();
        }
      }, 10000);
    }

    // Prevent context menu on buttons (untuk mobile)
    document.addEventListener('contextmenu', function(e) {
      if (e.target.tagName === 'BUTTON') {
        e.preventDefault();
      }
    });
  </script>
</body>
</html>
)rawliteral";

// --- Fungsi Kontrol Motor ---
void controlMotors(int left, int right) {
  if (!robotOn && !lineFollowerActive) {
    analogWrite(pwmL, 0);
    analogWrite(pwmR, 0);
    digitalWrite(rotL, LOW);
    digitalWrite(rotR, LOW);
    return;
  }
  
  // Motor Kiri
  if (left > 0) {
    digitalWrite(rotL, HIGH);
    analogWrite(pwmL, constrain(left, 0, 255));
  } else if (left < 0) {
    digitalWrite(rotL, LOW);
    analogWrite(pwmL, constrain(-left, 0, 255));
  } else {
    analogWrite(pwmL, 0);
  }

  // Motor Kanan
  if (right > 0) {
    digitalWrite(rotR, HIGH);
    analogWrite(pwmR, constrain(right, 0, 255));
  } else if (right < 0) {
    digitalWrite(rotR, LOW);
    analogWrite(pwmR, constrain(-right, 0, 255));
  } else {
    analogWrite(pwmR, 0);
  }

  Serial.printf("Motor - Kiri: %d, Kanan: %d\n", left, right);
}

// --- Handler Web Server ---
void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
  Serial.println("Halaman utama diminta");
}

void handleControl() {
  if (!robotOn) {
    server.send(403, "text/plain", "Robot tidak aktif");
    return;
  }
  
  if (server.hasArg("left") && server.hasArg("right")) {
    leftSpeed = constrain(server.arg("left").toInt(), -255, 255);
    rightSpeed = constrain(server.arg("right").toInt(), -255, 255);
    manualControl = true;
    lineFollowerActive = false;
    
    Serial.printf("Kontrol Manual: Kiri=%d, Kanan=%d\n", leftSpeed, rightSpeed);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Parameter hilang");
  }
}

void handleLineFollower() {
  if (!robotOn) {
    server.send(403, "text/plain", "Kamu Nanya Kenapa Robot tidak aktif");
    return;
  }
  
  if (server.hasArg("status")) {
    String status = server.arg("status");
    if (status == "on") {
      lineFollowerActive = true;
      manualControl = false;
      Serial.println("Line Follower: ON");
      server.send(200, "text/plain", "Line follower aktif");
    } else if (status == "off") {
      lineFollowerActive = false;
      manualControl = false;
      controlMotors(0, 0);
      Serial.println("Line Follower: OFF");
      server.send(200, "text/plain", "Line follower nonaktif");
    } else {
      server.send(400, "text/plain", "Status tidak valid");
    }
  } else {
    server.send(400, "text/plain", "Parameter status hilang");
  }
}

void handleServo() {
  if (!robotOn) {
    server.send(403, "text/plain", "Robot tidak aktif");
    return;
  }
  
  if (server.hasArg("angle")) {
    int angle = constrain(server.arg("angle").toInt(), 0, 180);
    myServo.write(angle);
    Serial.printf("Servo: %d derajat\n", angle);
    server.send(200, "text/plain", "Servo diatur");
  } else {
    server.send(400, "text/plain", "Parameter angle hilang");
  }
}

void handleGripper() {
  if (!robotOn) {
    server.send(403, "text/plain", "Robot tidak aktif");
    return;
  }
  
  if (server.hasArg("position")) {
    int position = constrain(server.arg("position").toInt(), 0, 180);
    gripperServo.write(position);
    Serial.printf("Gripper: %d derajat\n", position);
    server.send(200, "text/plain", "Gripper diatur");
  } else {
    server.send(400, "text/plain", "Parameter position hilang");
  }
}

void handleToggleRobot() {
  if (server.hasArg("status")) {
    String status = server.arg("status");
    if (status == "on") {
      robotOn = true;
      Serial.println("🟢 ROBOT ON");
      server.send(200, "text/plain", "Robot diaktifkan");
    } else if (status == "off") {
      robotOn = false;
      manualControl = false;
      lineFollowerActive = false;
      controlMotors(0, 0);
      myServo.write(90);
      gripperServo.write(90);
      Serial.println("🔴 ROBOT OFF");
      server.send(200, "text/plain", "Robot dinonaktifkan");
    } else {
      server.send(400, "text/plain", "Status tidak valid");
    }
  } else {
    server.send(400, "text/plain", "Parameter status hilang");
  }
}

void handleNotFound() {
  String message = "Halaman tidak ditemukan\n\nURI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, "text/plain", message);
  Serial.println("404 - " + server.uri());
}

// --- Line Follower Logic ---
void followLine() {
  if (!lineFollowerActive || !robotOn) {
    controlMotors(0, 0);
    return;
  }
  
  bool s1 = digitalRead(S1_sens);
  bool s2 = digitalRead(S2_sens);
  bool s3 = digitalRead(S3_sens);
  bool s4 = digitalRead(S4_sens);
  bool s5 = digitalRead(S5_sens);
  
  Serial.printf("Sensor: %d %d %d %d %d\n", s1, s2, s3, s4, s5);
  
  if (s3) {
    // Tengah - maju lurus
    controlMotors(FORWARD_SPEED_LF, FORWARD_SPEED_LF);
  } else if (s2 || s1) {
    // Kiri - belok kiri
    controlMotors(0, TURN_SPEED_LF);
  } else if (s4 || s5) {
    // Kanan - belok kanan
    controlMotors(TURN_SPEED_LF, 0);
  } else {
    // Tidak ada garis - berhenti
    controlMotors(0, 0);
  }
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n========================================");
  Serial.println("🤖 ESP8266 Robot Controller v2.0");
  Serial.println("========================================");

  // Inisialisasi pin sensor
  pinMode(S1_sens, INPUT);
  pinMode(S2_sens, INPUT);
  pinMode(S3_sens, INPUT);
  pinMode(S4_sens, INPUT);
  pinMode(S5_sens, INPUT);

  // Inisialisasi pin motor
  pinMode(pwmL, OUTPUT);
  pinMode(pwmR, OUTPUT);
  pinMode(rotL, OUTPUT);
  pinMode(rotR, OUTPUT);

  // Inisialisasi servo
  myServo.attach(servoPin);
  gripperServo.attach(gripperPin);
  
  // Set posisi awal servo
  myServo.write(90);
  gripperServo.write(90);
  delay(1000);

  // Inisialisasi status
  robotOn = false;
  manualControl = false;
  lineFollowerActive = false;
  
  // Pastikan motor mati di awal
  controlMotors(0, 0);

  // Setup WiFi Access Point
  Serial.println("📡 Mengatur WiFi Access Point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid_ap, password_ap);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("📶 AP IP Address: ");
  Serial.println(IP);
  Serial.printf("📶 SSID: %s\n", ssid_ap);
  Serial.printf("🔐 Password: %s\n", password_ap);

  // Setup web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/control", HTTP_GET, handleControl);
  server.on("/linefollower", HTTP_GET, handleLineFollower);
  server.on("/servo", HTTP_GET, handleServo);
  server.on("/gripper", HTTP_GET, handleGripper);
  server.on("/toggleRobot", HTTP_GET, handleToggleRobot);
  server.onNotFound(handleNotFound);

  // Start server
  server.begin();
  Serial.println("🌐 Web server dimulai!");
  Serial.println("========================================");
  Serial.println("✅ Robot siap digunakan!");
  Serial.printf("🔗 Buka browser: http://%s\n", IP.toString().c_str());
  Serial.println("========================================");
}

// --- Loop Utama ---
void loop() {
  server.handleClient();

  if (robotOn) {
    if (manualControl) {
      controlMotors(leftSpeed, rightSpeed);
    } else if (lineFollowerActive) {
      followLine();
    } else {
      controlMotors(0, 0);
    }
  } else {
    controlMotors(0, 0);
  }
  
  delay(10);
}