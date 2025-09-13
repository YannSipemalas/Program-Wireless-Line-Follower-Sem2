# CODE WIRELES ROBOT LINE FOLLOWER
Pada program ini saya membuat tampilan GUI, yaitu sistem monitoring untuk mengendalikan robot line follower secara wireless.


![Gambar GUI_HTML](https://drive.google.com/uc?export=view&id=1NMsAt_PKSrkLzP0rNCTQL01n_uoUINiv)

Program ini digunakan untuk mengontrol sebuah robot berbasis ESP8266 melalui jaringan wifi. Setelah ESP8266 dinyalakan, ia akan membuat jaringan wifi (Acces Point), yang dimana pengguna bisa menghubungkan HP/Laptop ke jaringan ini dan mengakses tampilan kontrol robot melalui browser dengan alamat ini :

http://192.168.4.1.


## FITUR TAMPILAN GUI
1. Kontrol Manual Robot
   - Maju, Mundur,Kiri.Kanan. dan Stop
   - Terdapat Slider untuk mengatur kecepatan Motor (PWM)
   - Mulai Line Follower : Robot akan memasuki mode membaca garis yang dimana ke 5 sensor garis akan membaca dan mengikuti garis secara otomatis.
2. Kontrol Servo Arm dan Gripper
   - Kontrol Arm     : Naik dan Turun 
   - Kontrol Gripper : Buka dan Tutup
3. Status dan Koneksi
   - Tampilam status robot (aktif/tidak aktif) 
   - Indikator apakah robot terhubung atau tidak

## Tech

Robot ESP8266 ini dibangun menggunakan sejumlah library open source agar dapat bekerja dengan baik:

- ESP8266WiFi – library resmi untuk koneksi WiFi pada ESP8266
- ESP8266WebServer – web server ringan untuk ESP8266
- Servo.h – library untuk mengontrol servo motor
- Arduino Code untuk ESP8266 – framework utama untuk menjalankan kode pada ESP8266
- C++ – bahasa pemrograman inti yang digunakan

A. Front END
-  HTML5 – struktur halaman kontrol robot
-  CSS3 – styling antarmuka kontrol
-  JavaScript (Vanilla) – logika interaktif untuk kontrol robot via browser
-  Fetch API – komunikasi HTTP dengan web server ESP8266
-  Responsive Design – mendukung tampilan mobile dan desktop

## Requirement
Untuk menjalankan kode ini, dibutuhkan:
1. Hardware
   - Motor Shield     : L293D
   - MikroKontroler   :ESP8266
   - Motor DC         : 2 Motor DC N20
   - Batre dan Holder :2 Batre 18650 NCR dan Holder batrenya(1 batre 3.7V)
   - Servo           : 2 Servo MG90S (1 servo untuk Arm dan 1 servo untuk Gripper}
   - Kabel Jumpere Male to Female
   - Kabel Jumpere Female to Female
   - Kabel Upload
  
2. SoftWare
   - Arduino Ide :
   1. Arduino IDE dengan ESP8266 core terinstal
   2. Arduino IDE dengan ESP8266 core terinstal
   3. Driver USB to UART (CP210X) sesuai board



# Penjelasan Kode Wireless
|1. Library dan Definisi Pin | 
| -------------- |
```cpp
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

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
const int gripperPin = 3;  // Pin untuk servo gripper
```

- ESP8266WiFi → library WiFi untuk koneksi AP.
- ESP8266WebServer → menjalankan server web pada ESP8266.
- Servo.h → kontrol servo motor.
-Definisi pin untuk sensor garis, motor, dan servo.

| 2. Kredensial WIFI(AP) | 
| -------------- |

```cpp
const char* ssid_ap = "ESP";
const char* password_ap = "12345678";
```
Bagian ini mendefinisikan SSID dan password untuk Access Point yang dibuat oleh ESP8266.
Jadi HP/laptop bisa konek langsung ke WiFi ESP tanpa router.

- SSID WiFi: "ESP"
- Password: "12345678"

| 3. Definisi Objek Global | 
| -------------- |
```cpp
ESP8266WebServer server(80);
Servo myServo;
Servo gripperServo;
```
- ESP8266WebServer server(80); → Membuat server web pada port 80 (port HTTP standar). Digunakan untuk komunikasi lewat web (control GUI atau browser).
- Servo myServo; → Objek servo untuk arm.
- Servo gripperServo; → Objek servo untuk gripper.

| 4. Variabel Kontrol | 
| -------------- |
```cpp
int leftSpeed = 0;
int rightSpeed = 0;
bool lineFollowerActive = false;
bool manualControl = false;
bool robotOn = false; // Status global robot
```
- leftSpeed, rightSpeed → Menyimpan kecepatan motor kiri dan kanan (0 = berhenti).
- lineFollowerActive → true kalau mode line follower aktif.
- manualControl → true kalau mode manual (via remote/GUI) dipakai.
- robotOn → Status global robot (true = robot nyala, false = mati/standby).


| 5. Membuat Struktur Halaman Web(HTML)| 
| -------------- |

HTML di sini digunakan untuk membuat tampilan web kontrol robot. Semua tombol, slider, label, dan teks ada di dalam HTML.

```cpp
<!DOCTYPE html>
<html>
<head>
  <title>Kontrol Robot ESP8266</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  ...
</head>
<body>
  <h1>🤖 Kontrol Robot ESP8266</h1>
  <div class="ip-display" id="ipAddress">IP Robot: 192.168.4.1</div>
  <div class="connection-status disconnected" id="connectionStatus">Mengecek koneksi...</div>

  <!-- Slider kecepatan -->
  <input type="range" min="50" max="255" value="150" class="speed-slider" id="kecepatanSlider">
  <label for="kecepatanSlider" id="kecepatanLabel">Kecepatan: 150</label>

  <!-- Tombol ON/OFF Robot -->
  <button class="toggle-btn" id="robotOnOffButton" onclick="toggleRobot()">🔴 OFF</button>

  <!-- Kontrol Gerak -->
  <p class="section-title">🎮 Kontrol Gerak</p>
  <div class="button-group">
    <button class="control-btn" id="btnMaju" ...>⬆ Maju</button>
  </div>
  <div class="button-group">
    <button class="control-btn" id="btnKiri" ...>⬅ Kiri</button>
    <button class="stop-btn" id="btnBerhenti" onclick="berhentiRobot()">⏹ Stop</button>
    <button class="control-btn" id="btnKanan" ...>➡ Kanan</button>
  </div>
  <div class="button-group">
    <button class="control-btn" id="btnMundur" ...>⬇ Mundur</button>
  </div>

  <!-- Mode Line Follower -->
  <button class="mode-btn" id="lineFollowerButton" onclick="toggleLineFollower()">🛤 Mulai Line Follower</button>

  <!-- Kontrol Servo -->
  <p class="section-title">🦾 Kontrol Servo</p>
  <div class="button-group">
    <button class="servo-btn" onclick="kirimServo(150)">⬆ Naik (180°)</button>
    <button class="servo-btn" onclick="kirimServo(90)">➡ Tengah (90°)</button>
    <button class="servo-btn" onclick="kirimServo(0)">⬇ Turun (0°)</button>
  </div>

  <!-- Kontrol Gripper -->
  <p class="section-title">🤏 Kontrol Gripper</p>
  <div class="button-group">
    <button class="gripper-btn" onclick="kirimGripper(90)">✊ TUTUP</button>
    <button class="gripper-btn" onclick="kirimGripper(180)">🤏 BUKA</button>
  </div>

  <!-- Status Robot -->
  <div id="statusLabel">Robot Tidak Aktif</div>
</body>
</html>
```
- Fungsi HTML ini:
  
- (h1) → Sebagai judul halaman.
- (div) → Menampilkan IP robot & status koneksi.
- (input type="range") → Slider untuk atur kecepatan.
- (button) → Tombol untuk kontrol robot (maju, mundur, kiri, kanan, stop).
- Bagian Servo & Gripper → Tombol untuk mengatur posisi servo & gripper.
- (div id="statusLabel") → Label status robot.

| 6. Membuat Tampilan Desain(CSS) | 
| -------------- |

Kode CSS dipakai untuk mengatur gaya visual (warna, ukuran tombol, background, layout).

```cpp
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
h1 { color: #FFD700; margin-bottom: 20px; }
.section-title { color: #ADD8E6; margin-top: 20px; font-size: 1.2em; }

button {
  width: 100px; height: 50px;
  margin: 5px; border-radius: 8px;
  font-size: 1em; font-weight: bold;
  cursor: pointer; transition: all 0.3s ease;
  box-shadow: 2px 2px 5px rgba(0,0,0,0.3);
}
button.control-btn { background-color: #FFA500; }
button.stop-btn { background-color: #DC143C; color: white; }
button.toggle-btn { background-color: #28a745; color: white; }
button.toggle-btn.off { background-color: #dc3545; }
button.mode-btn { background-color: #FFD700; }
button.servo-btn { background-color: #007bff; color: white; }
button.gripper-btn { background-color: #9370DB; color: white; }

.speed-slider {
  width: 80%; max-width: 300px;
  height: 10px; border-radius: 5px;
  background: #555;
}
.speed-slider::-webkit-slider-thumb {
  width: 20px; height: 20px;
  border-radius: 50%; background: #4CAF50;
}
.connection-status.connected { background-color: #28a745; color: white; }
.connection-status.disconnected { background-color: #dc3545; color: white; }
```
 Fungsi CSS ini :

- body → Tema utama (warna gelap, teks putih, layout tengah).
- button-group → Mengatur posisi tombol agar rapi & sejajar.
- button → Styling tombol (warna, efek klik, shadow).
- control-btn, .stop-btn, .toggle-btn → Warna tombol beda sesuai fungsi.
- speed-slider → Slider kecepatan dengan desain custom.
- connection-status → Label status koneksi (hijau kalau connected, merah kalau putus).

| 7. Logika dan Fungsi Interaktif (JS) | 
| -------------- |

JavaScript mengatur fungsi kontrol robot melalui HTTP request ke ESP8266.

```cpp
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
```
 Bagian Variabel & Elemen :
- ROBOT_IP → IP default robot (mode AP).
- robotActive → Status robot (nyala/mati).
- lineFollowerActive → Mode line follower aktif/tidak.
- isConnected → Status koneksi ke robot.
- getElementById → Mengambil elemen HTML supaya bisa dimanipulasi lewat JS.


| 8. Kode JS Fungsi Utama | 
| -------------- |

Kode ini memiliki fungsi untuk mengirim perintah dari webserver ke ESP8266

```cpp
int leftSpeed = 0;
function sendCommand(path, callback, errorCallback) {
  const url = `http://${ROBOT_IP}${path}`;
  fetch(url, { method: 'GET', cache: 'no-cache' })
    .then(response => response.text())
    .then(data => { if (callback) callback(data); })
    .catch(error => { if (errorCallback) errorCallback(); });
}
```
 Kirim HTTP GET ke ESP8266, misalnya:
- /control?left=150&right=150 → maju
- /servo?angle=90 → servo tengah


| 9. Fungsi Kontrol Robot | 
| -------------- |

Fungsi ini mengatur arah gerak robot dengan mengirim kecepatan motor kiri & kanan.

```cpp
function toggleRobot() { ... }   // ON/OFF robot
function toggleLineFollower() { ... } // Aktifkan/stop line follower
function maju() { sendCommand(`/control?left=${getSpeed()}&right=${getSpeed()}`); }
function mundur() { sendCommand(`/control?left=${-getSpeed()}&right=${-getSpeed()}`); }
function kiri() { sendCommand(`/control?left=${-getSpeed()}&right=${getSpeed()}`); }
function kanan() { sendCommand(`/control?left=${getSpeed()}&right=${-getSpeed()}`); }
function berhentiRobot() { sendCommand(`/control?left=0&right=0`); }
```
| 10. Fungsi Servo dan Gripper | 
| -------------- |

Atur servo arm & gripper sesuai sudut.

```cpp
function kirimServo(angle) {
  sendCommand(`/servo?angle=${angle}`);
}
function kirimGripper(position) {
  sendCommand(`/gripper?position=${position}`);
}
```

| 11. Fungsi Status dan Koneksi | 
| -------------- |

Tugasnya kode ini untuk cek koneksi robot & update label status di halaman.

```cpp
function updateStatusLabel(text, color) { ... }
function updateConnectionStatus(connected) { ... }
function testConnection() { ... }
```

| 12. Inisialisasi Halaman web saat dibuka | 
| -------------- |

Saat halaman web dibuka, robot default OFF, lalu otomatis tes koneksi ke ESP8266, dan reconnect tiap 10 detik.

```cpp
window.onload = function() {
  robotActive = false;
  updateStatusLabel("❌ Robot Tidak Aktif", "red");
  testConnection();
  setInterval(() => { if (!isConnected) testConnection(); }, 10000);
}
```

| 13. Fungsi Kontrol Motor | 
| -------------- |

Pada kode program ini memiliki fungsi untuk mengatur kecepatan & arah motor kiri/kanan. Fungsi ini dipanggil baik dari kontrol wireless maupun line follower.

- left & right = nilai PWM (0 sampai 255).
   - Positif → maju.
   - Negatif → mundur.
   - Nol → berhenti.
- Serial.printf dipakai untuk debug (menampilkan nilai motor di Serial Monitor).

```cpp
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
```



| 14. Handle Web Server| 
| -------------- |

Apa fungsi Handle??
Handle adalah fungsi yang dijalankan saat ada request HTTP ke alamat tertentu di web server ESP8266.

Jadi: tiap handle() = merupakan penghubung antara perintah dari browser dengan pergerakan robot.

Dalam Kode program ini ada banyak handle yang saya buat untuk dipanggil saat ada request http ke ip yang sudah saya buat (192.168.4.1).
 - handleRoot()
   - Menampilkan halaman utama (HTML).
   - Kalau user buka http://IP/, halaman UI robot akan muncul.
 - handleControl()
   - Untuk kontrol manual motor (parameter left & right).
   - Kalau robot ON, motor dikontrol sesuai pengaturan kita.
 - handleLineFollower()
   - Untuk mengaktifkan / menonaktifkan mode line follower.
   - Menampilkan status=on → aktif, status=off → nonaktif.
 - handleServo/Arm()
   - Untuk mengatur servo arm posisi 0 = turun, 90 = Angkat, 180 = Arm tegak ke atas
 - handleGripper()
   - Mengatur servo gripper (posisi buka/tutup).
 - handleGripper()
   - Mengatur servo gripper (posisi buka/tutup).
 - handleToggleRobot()
   - Untuk menghidupkan atau mematikan robot.
   - Kalau OFF → motor berhenti, servo reset ke 90°.
 - handleNotFound()
   - Menangani request URL yang salah (404).   
```cpp
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
```

| 15. Fungsi Line Follower | 
| -------------- |

Kode ini berisi logika sederhana line follower.
-  Robot membaca garis hitam/putih dengan 5 sensor.
-  Jika tombol "Line Follower" di web ditekan ON, maka robot masuk ke mode mengikuti garis.
-  Dalam mode ini, robot akan:
   - Maju lurus kalau sensor 3 (tengah) aktif.
   - Belok kiri kalau sensor 1 dan 2 aktif.
   - Belok kanan kalau sensor 4 dan 5 aktif.
   - Berhenti kalau tidak ada garis yang terdeteksi.
```cpp
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

```


















