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

