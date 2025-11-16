#include <Arduino.h> 
#include <painlessMesh.h> 
 
// Konfigurasi Mesh 
#define MESH_PREFIX     "sensor-network" 
#define MESH_PASSWORD   "iotpass123" 
#define MESH_PORT       5555 
 
painlessMesh mesh; 
Scheduler userScheduler; 
const int gasPin = 35; 
const int ambangBatas = 1000; 
const String lokasi = "Gudang B";
 
// Aktuator 
const int buzzerPin = 27; 
const int ledBahaya = 26; 
const int ledStatus = 2;  // Biasanya LED on-board ESP32 ada di GPIO 2 
 
// Variabel alarm 
bool buzzerState = false; 
unsigned long previousBuzzerMillis = 0; 
const int intervalBuzzer = 150; 
 
// Cek koneksi mesh ke root 
bool isConnectedToRoot = false; 
 
// Nilai terakhir gas (agar bisa ditampilkan via Serial secara periodik) 
int lastGasValue = 0; 
String lastStatus = "AMAN"; 
 
// Task: Kirim data gas setiap 5 detik 
void kirimDataCallback() { 
  String data = lokasi + "|" + String(lastGasValue) + "|" + lastStatus; 
  mesh.sendBroadcast(data); 
  Serial.println("[KIRIM] ke root: " + data); 
} 
Task taskKirimData(5000, TASK_FOREVER, &kirimDataCallback);  // <--- Tetap 5 detik 
 
// Task: Cek koneksi ke root tiap 2 detik 
void cekKoneksiCallback() { 
  bool connected = mesh.isConnected(mesh.getNodeId()); 
  if (connected != isConnectedToRoot) { 
    isConnectedToRoot = connected; 
    digitalWrite(ledStatus, isConnectedToRoot ? HIGH : LOW); 
    Serial.println(isConnectedToRoot ? "    Terhubung ke root" : "⚠ Terputus dari root"); 
  } 
} 
Task taskCekKoneksi(1000, TASK_FOREVER, &cekKoneksiCallback); 
 
void setup() { 
  Serial.begin(115200); 
 
  pinMode(gasPin, INPUT); 
  pinMode(ledBahaya, OUTPUT); 
  pinMode(buzzerPin, OUTPUT); 
  pinMode(ledStatus, OUTPUT); 
 
  // Setup PWM buzzer 
  ledcSetup(0, 2000, 8);           // Channel 0, frekuensi 2kHz, resolusi 8-bit 
  ledcAttachPin(buzzerPin, 0);    // Hubungkan ke pin buzzer 
 
  mesh.setDebugMsgTypes(ERROR | CONNECTION); 
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT); 
 
  mesh.onReceive([](uint32_t from, String &msg) { 
    Serial.printf("[TERIMA] dari %u: %s\n", from, msg.c_str()); 
  }); 
 
  userScheduler.addTask(taskKirimData); 
  userScheduler.addTask(taskCekKoneksi); 
  taskKirimData.enable(); 
  taskCekKoneksi.enable(); 
 
  Serial.println("   ESP-B aktif (Node Mesh)"); 
} 
 
void loop() { 
  mesh.update();
  // ----------- SENSOR & ALARM FAST RESPONSE (real-time) ----------- 
  int nilaiGas = analogRead(gasPin); 
  String statusGas = (nilaiGas > ambangBatas) ? "BAHAYA" : "AMAN"; 
 
  // Simpan nilai terbaru untuk task kirim 
  lastGasValue = nilaiGas; 
  lastStatus = statusGas; 
 
  if (nilaiGas > ambangBatas) { 
    digitalWrite(ledBahaya, HIGH); 
 
    // Alarm cepat (pipipipipi) 
    if (millis() - previousBuzzerMillis >= intervalBuzzer) { 
      previousBuzzerMillis = millis(); 
      buzzerState = !buzzerState; 
      ledcWriteTone(0, buzzerState ? 2000 : 0); 
    } 
 
  } else { 
    digitalWrite(ledBahaya, LOW); 
    ledcWriteTone(0, 0);  // Matikan buzzer 
  } 
 
  // ---------------------------------------------------------------- 
}
