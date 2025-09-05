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
