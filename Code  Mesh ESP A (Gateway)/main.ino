#include <painlessMesh.h> 
#include <WiFi.h> 
#include <WiFiClientSecure.h> 
#include <HTTPClient.h> 
#include <Preferences.h> 
#include <ArduinoJson.h> 
#define MESH_PREFIX     "sensor-network" 
#define MESH_PASSWORD   "iotpass123" 
#define MESH_PORT       5555
const char* ssid = "Bintang Rejeki 1"; 
const char* password = "suksesselalu"; 
const String BOT_TOKEN = "7541606116:AAGvpm6j-4FqlchKlc
S9NJgSG95LARMERg"; 
const String TELEGRAM_API = "https://api.telegram.org/bot" + BOT_TOKEN; 
 
enum Mode { MODE_MESH, MODE_WIFI }; 
Mode currentMode = MODE_MESH; 
 
Scheduler userScheduler; 
painlessMesh mesh; 
 
Preferences prefs; 
String pesanTertunda = ""; 
 
const int ledStatus = 2; 
unsigned long lastBlink = 0; 
bool ledState = false; 
 
unsigned long masukWifiMillis = 0; 
bool sudahKirimPesan = false; 
 
// === Daftar chat_id Telegram manual === 
const String chatIdManual[] = { 
  "5002062705" 
}; 
const int jumlahChat = sizeof(chatIdManual) / sizeof(chatIdManual[0]); 
 
// === Kirim pesan ke semua pengguna di daftar === 
void kirimTelegram(String pesan) { 
  WiFiClientSecure client; 
  client.setInsecure(); 
  HTTPClient http; 
  String url = TELEGRAM_API + "/sendMessage"; 
for (int i = 0; i < jumlahChat; i++) { 
    String chat_id = chatIdManual[i]; 
    if (!http.begin(client, url)) break; 
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
    String body = "chat_id=" + chat_id + "&text=" + pesan; 
    int code = http.POST(body); 
    Serial.printf("Kirim ke %s: %s (%d)\n", chat_id.c_str(), code == 200 ? "SUKSES" : 
"GAGAL", code); 
    http.end(); 
    delay(200); 
  } 
} 
 
// === Callback pesan mesh dari node sensor === 
void receivedCallback(uint32_t from, String &msg) { 
  if (currentMode != MODE_MESH) return; 
  Serial.printf("\n       Data dari node [%u]: %s\n", from, msg.c_str()); 
 
  int sep1 = msg.indexOf('|'); 
  int sep2 = msg.indexOf('|', sep1 + 1); 
  if (sep1 > 0 && sep2 > sep1) { 
    String lokasi = msg.substring(0, sep1); 
    int nilai = msg.substring(sep1 + 1, sep2).toInt(); 
    String status = msg.substring(sep2 + 1); 
    if (status == "BAHAYA") { 
      pesanTertunda = "       GAS BOCOR TERDETEKSI!\n    Lokasi: " + lokasi + 
                      "\n          Nilai: " + String(nilai) + "\n⚠ Status: " + status; 
      prefs.begin("notif", false); 
      prefs.putString("pesan", pesanTertunda); 
      prefs.end(); 
      Serial.println("   Bahaya terdeteksi! Pindah ke MODE_WIFI"); 
      mesh.stop();                  //    Stop mesh 
      WiFi.disconnect(true);        //    Reset WiFi 
      delay(200);                   //    Delay 
      currentMode = MODE_WIFI; 
      masukWifiMillis = millis(); 
      WiFi.mode(WIFI_STA); 
      WiFi.begin(ssid, password); 
    } 
  } 
} 
 
void newConnectionCallback(uint32_t nodeId) { 
  Serial.printf("   Node terhubung: %u\n", nodeId); 
} 
 
void initMesh() { 
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT); 
  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION); 
  mesh.setRoot(true); 
  mesh.setContainsRoot(true); 
  mesh.onReceive(&receivedCallback); 
  mesh.onNewConnection(&newConnectionCallback); 
  digitalWrite(ledStatus, HIGH); 
  Serial.println("           MODE_MESH aktif"); 
} 
void setup() { 
  Serial.begin(115200); 
  pinMode(ledStatus, OUTPUT); 
  digitalWrite(ledStatus, LOW); 
  delay(500); 
 
  sudahKirimPesan = false;  //    Reset setiap boot 
prefs.begin("notif", true); 
  pesanTertunda = prefs.getString("pesan", ""); 
  prefs.end(); 
 
  currentMode = pesanTertunda != "" ? MODE_WIFI : MODE_MESH;  
if (currentMode == MODE_MESH) { 
    initMesh(); 
  } else { 
    masukWifiMillis = millis(); 
    WiFi.disconnect(true);  //    Bersihkan 
    delay(200);             //    Stabilisasi 
    WiFi.mode(WIFI_STA); 
    WiFi.begin(ssid, password); 
  } 
} 
 
void loop() { 
  unsigned long now = millis(); 
 
  // === MODE_MESH === 
  if (currentMode == MODE_MESH) { 
    mesh.update(); 
    digitalWrite(ledStatus, HIGH); 
  } 
 
  // === MODE_WIFI === 
  else if (currentMode == MODE_WIFI) { 
    if (now - lastBlink >= 200) { 
      lastBlink = now; 
      ledState = !ledState; 
      digitalWrite(ledStatus, ledState ? HIGH : LOW); 
    } 
  if (WiFi.status() == WL_CONNECTED) { 
      if (!sudahKirimPesan) { 
        Serial.println("   WiFi terhubung"); 
        kirimTelegram(pesanTertunda); 
        prefs.begin("notif", false); 
        prefs.putString("pesan", ""); 
        prefs.end();
sudahKirimPesan = true; 
        delay(1000); 
        WiFi.disconnect(true); 
        WiFi.mode(WIFI_OFF); 
        delay(1000); 
        currentMode = MODE_MESH; 
        initMesh(); 
      } 
    } else if (now - masukWifiMillis > 10000) { 
      Serial.println("  Timeout WiFi. Kembali ke MODE_MESH."); 
      Serial.print("WiFi.status(): "); 
      Serial.println(WiFi.status()); 
      currentMode = MODE_MESH; 
      initMesh(); 
    } 
  } 
}
