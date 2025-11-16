# Sistem Alarm Kebocoran Gas Berbasis ESP32, ESP-Mesh & Telegram Bot

Sistem ini dirancang untuk mendeteksi kebocoran gas menggunakan sensor MQ-2 dan ESP32 sebagai node sensor. Komunikasi antar node menggunakan **painlessMesh**, sehingga sistem tetap berfungsi meskipun jaringan WiFi tidak stabil.

Node sensor (ESP-B) membaca nilai gas dan mengirimkan pesan **"BAHAYA"** ke ESP-A (Gateway). ESP-A kemudian beralih ke mode WiFi untuk mengirimkan notifikasi Telegram secara real-time, lalu kembali ke mode mesh.

Tujuan project ini adalah membangun sistem peringatan dini kebocoran gas yang cepat, stabil, dan tidak bergantung pada internet secara terus-menerus.

---

## Fitur Utama
- Deteksi kebocoran gas menggunakan **sensor MQ-2**
- Komunikasi antar ESP32 menggunakan **painlessMesh**
- **Mode switching otomatis:** MESH → WiFi → MESH
- Notifikasi real-time ke Telegram Bot
- Penyimpanan chat_id menggunakan **Preferences**
- Alarm lokal (LED & buzzer) pada node sensor
- Sistem tetap berjalan meski WiFi mati

---

## Komponen Sistem & Peran
| Komponen | Node | Fungsi |
|----------|------|--------|
| **ESP32-A** | Gateway | Menerima pesan dari mesh, kirim notifikasi Telegram |
| **ESP32-B** | Sensor | Membaca MQ-2, mengirim status bahaya |
| **Sensor MQ-2** | ESP-B | Mendeteksi gas LPG/CO |
| **LED** | ESP-B | Indikator bahaya |
| **Buzzer** | ESP-B | Alarm cepat |
| **Telegram Bot** | ESP-A | Media peringatan real-time |

---

## Pemetaan Pin ESP32

### **ESP32-B (Node Sensor)**
| Perangkat | Pin | Keterangan |
|-----------|-----|------------|
| MQ-2 Sensor | GPIO 34 | Input analog |
| LED Indikator | GPIO 2 | Output |
| Buzzer | GPIO 4 | Alarm |
| Mesh Network | WiFi AP | painlessMesh |

### **ESP32-A (Gateway)**
| Perangkat | Pin | Keterangan |
|-----------|-----|------------|
| WiFi STA | Internal | Terhubung ke internet untuk Telegram |
| Mesh AP | Internal | Komunikasi antar ESP32 |
| Storage NVS | Preferences | Menyimpan chat_id & status |

---

##  Langkah Percobaan / Pengujian

| No | Langkah Percobaan | Hasil yang Diharapkan |
|----|-------------------|------------------------|
| 1 | Nyalakan ESP-A & ESP-B | Kedua node terhubung melalui mesh |
| 2 | Buka Serial Monitor | Menampilkan status koneksi mesh |
| 3 | Dekatkan gas ke MQ-2 | Nilai sensor meningkat |
| 4 | Alarm lokal aktif | LED & buzzer menyala cepat |
| 5 | ESP-B mengirim “BAHAYA” | ESP-A menerima pesan |
| 6 | ESP-A switching ke WiFi | Mesh OFF → WiFi ON |
| 7 | Telegram menerima notifikasi | Pesan “🚨 GAS BOCOR TERDETEKSI!” muncul |
| 8 | ESP-A restart otomatis | Kembali ke mode mesh |
| 9 | Sistem kembali monitoring | Siap mendeteksi ulang |

---

## Format Notifikasi Telegram
🚨 GAS BOCOR TERDETEKSI!
Lokasi : Node Sensor ESP32
Status : BAHAYA
Segera lakukan pengecekan!


---

## Dokumentasi

### **Rangkaian Sistem**
![Rangkaian Sistem](images/rangkaian.jpg)

### **Contoh Notifikasi Telegram**
![Telegram Alert](images/telegram-alert.jpg)

### **Topologi Mesh**
![Mesh Topology](images/mesh-topology.png)

---

## Video Demo
https://youtu.be/93MFlIaWFAE?si=sy8nNN30ZMorD-dF

---

## Author
**Muhammad Irsyad Alkais**  
Computer Engineering Student – PENS  
IoT • ESP32 • Mesh Network • Telegram Bot • Embedded Systems
