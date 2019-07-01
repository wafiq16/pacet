// Include library yang diperlukan
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <SimpleDHT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
 
// Gunakan serial sebagai monitor
#define USE_SERIAL Serial
 
// Buat object Wifi
ESP8266WiFiMulti WiFiMulti;
 
// Buat object http
HTTPClient http;

//definisi string payload untuk menampung hhtp.getstring(); 
String payload;

//definisi url pembawa variabel ke ip web server
String url = "http://192.168.43.231/wsbasisdata/Simpandata.php?suhu=";

//definisi pin led,sensor,lcd dan kondisi awal pompa air
int pompa = 2;
int pinDHT11 = 2; //d4
int led1 = 0;   //d3
SimpleDHT11 dht11(pinDHT11);
LiquidCrystal_I2C lcd(0x3F,16,2); 

//fungsi pendefinisian kondisi awal
void setup() {
  
  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  lcd.init();                      // initialize the lcd 
  lcd.backlight();

    USE_SERIAL.begin(115200);
    USE_SERIAL.setDebugOutput(false);
    
 //supaya pada serial monitor terlihat posisi loading
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] Tunggu %d...\n", t);
        USE_SERIAL.flush();
    }
    //definisi wifi yang digunakan sebagai server
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("A1601", "12345678"); // Sesuaikan SSID dan password ini
}

void loop() {

    // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  //pernyataan apakah sensor berhasil membaca atau tidak
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  //menampilkan suhu dan kelembapan pada serial monitor
  Serial.print("Sample OK: ");
  Serial.print((float)temperature); Serial.print(" *C, "); 
  Serial.print((float)humidity); Serial.println("%");
  //isialisasi kondisi pompa air berdasarkan kondisi suhu dan kelembapan
  if((float)temperature<25){
    digitalWrite(led1, LOW);
    pompa = 0;
    }
  else if((float)temperature>25){
    digitalWrite(led1, HIGH);
    pompa = 1;
    }
  else if((float)humidity<25){
    digitalWrite(led1, HIGH);
    pompa = 1;
    }
  else if((float)humidity>25){
    digitalWrite(led1, LOW);
    pompa = 0;
    }
  //pernyataan untuk menampilkan suhu dan kelembapan pada lcd
  lcd.init();
  lcd.setCursor(1,0);
  lcd.print("T = ");
  lcd.print((float)temperature); lcd.print(" *C "); 
  lcd.setCursor(1,1);
  lcd.print("H = ");
  lcd.print((float)humidity); lcd.println("%");
  //
  delay(1000);
  
  //definisi variabel penyimpan data yang akan dikirim ke web server
  String suhu = (String)temperature;
  String kelembapan = (String)humidity;
  String sanyo = (String)pompa;
  
 //checking data sudah benar apa belum
  Serial.println(suhu);
  Serial.println(kelembapan);
  Serial.println(pompa);
  
 //delay
    delayMicroseconds(280);
    delayMicroseconds(40);
    delayMicroseconds(9680);

 //kondisi jika wifi sudah tersambung
    if((WiFiMulti.run() == WL_CONNECTED)) {
  
        // Tambahkan nilai suhu, kelembapan dan kondisi pompa air pada URL yang sudah kita buat
        USE_SERIAL.print("[HTTP] Memulai...\n");
        http.begin( url + (String) suhu + "&kelembapan="+ (String) kelembapan + "&sanyo=" + (String) sanyo );
        
        // Mulai koneksi dengan metode GET
        USE_SERIAL.print("[HTTP] Melakukan GET ke server...\n");
        int httpCode = http.GET();
 
        // Periksa httpCode, akan bernilai negatif kalau error
        if(httpCode > 0) {
            
            // Tampilkan response http
            USE_SERIAL.printf("[HTTP] kode response GET: %d\n", httpCode);
 
            // Bila koneksi berhasil, baca data response dari server
            if(httpCode == HTTP_CODE_OK) {
                payload = http.getString();
                USE_SERIAL.println(payload);
            }
 
        } else {
 
            USE_SERIAL.printf("[HTTP] GET gagal, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    }
     //jika tidak berhasil mengirim data maka akan muncul "gagal bambank"
     else{ 
     Serial.println("gagal bambank");
     }
    delay(5000);
}
