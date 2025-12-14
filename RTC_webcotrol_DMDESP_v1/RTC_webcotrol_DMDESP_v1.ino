// REFERENSI CODING DARI
// YOUTUBE BONNY USEFUL
// www.rokhmad.com
// dan sumber2 yang lain
// Update 24-01-2022
// https://github.com/multikejayan/Running-Text-ESP8266

/*
 * small chamges by Nicu FLORICA (niq_ro)
 * https://nicuflorica.blogspot.com/
 * https://arduinotehniq.blogspot.com/
 * https://www.arduinotehniq.com/
 * https://www.youtube.com/user/dj06ntm
 * 
 * v.1 - small changes (translated comments and commands on web page)
 */

#include <Wire.h>
#include <DMDESP.h>
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);
#include <ArduinoJson.h>
#include <SPI.h>
#include <FS.h>

#include <fonts/ElektronMart6x8.h>
#include <fonts/ElektronMart6x16.h>
#include <fonts/Fontku.h>
#include <fonts/Mono5x7.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/EMSansSP8x16.h>
#include <fonts/EMSans6x16.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Webpage.h"

#define DISPLAYS_WIDE 2 
#define DISPLAYS_HIGH 1
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);

struct ConfigDisp{
  int cerah;
  };

  struct ConfigInfo {
  char info1[512];
  
  };


//SETUP RTC
//year, month, date, hour, min, sec and week-day(Senin 0 sampai Ahad 6)
//DateTime dt(2019, 11, 7, 21, 15, 0, 3);
RtcDateTime now;
//char weekDay[][7] = {"AHAD", "SENIN", "SELASA", "RABU", "KAMIS", "JUM'AT", "SABTU", "AHAD"}; // array hari, dihitung mulai dari senin, hari senin angka nya =0,
//char monthYear[][10] = { "DESEMBER", "JANUARI", "PEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOPEMBER", "DESEMBER" };

char weekDay[][10] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY"}; // array hari, dihitung mulai dari senin, hari senin angka nya =0,
char monthYear[][10] = {"DECEMBER", "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"};

char jam[9];
byte tmpinfo;
static uint32_t pM,pMJam,cM;

// WIFI
// LED Internal
uint8_t pin_led = 2;

//WEB Server
ESP8266WebServer server(80);

// Sebagai Station 
 const char* wifissid = "bbk2"; 
 const char* wifipassword = "internet2";

// Sebagai AP
const char* ssid = "bbk4"; //kalau gagal konek
const char* password = "internet4";

IPAddress local_ip(192, 168, 22, 22);
IPAddress gateway(192, 168, 22, 22);
IPAddress netmask(255, 255, 255, 0);

void wifiConnect() {

  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  delay(1000);

  Serial.println("Wifi Sation Mode");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  
  unsigned long startTime = millis();

  WiFi.begin(wifissid, wifipassword);    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(pin_led, !digitalRead(pin_led));
    if (millis() - startTime > 15000) break;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(pin_led, HIGH);
    
  } else {
    
    Serial.println("Wifi AP Mode");
   WiFi.mode(WIFI_AP_STA);
  // WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, netmask);
    WiFi.softAP(ssid, password);
    digitalWrite(pin_led, LOW);
  }

  //Serial.println("");
  WiFi.printDiag(Serial);
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

}

const char *fileconfiginfo = "/configinfo.json";
ConfigInfo configinfo;
const char *fileconfigdisp = "/configdisp.json";
ConfigDisp configdisp;

//-------------------------------------------
//SETUP

 void setup() {
  Serial.begin(115200);
 
  //RTC D3231

  int rtn = I2C_ClearBus(); // clear the I2C bus first before calling Wire.begin()
  if (rtn != 0) {
    Serial.println(F("I2C bus error. Could not clear"));
    if (rtn == 1) {
      Serial.println(F("SCL clock line held low"));
    } else if (rtn == 2) {
      Serial.println(F("SCL clock line held low by slave clock stretch"));
    } else if (rtn == 3) {
      Serial.println(F("SDA data line held low"));
    }
  } else { // bus clear, re-enable Wire, now can start Wire Arduino master
    Wire.begin();
    Serial.println(F("RTC = ok !"));
  }
  
  Rtc.Begin();
  if (!Rtc.GetIsRunning()) {  
    Rtc.SetIsRunning(true); 
  }
  
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
    
 //Rtc.SetDateTime(RtcDateTime(2025,12,8,22,39,0)); // Setting Jam Manual
    RtcDateTime now = Rtc.GetDateTime();
    
 //  Serial.println("Setup RTC selesai");
   Serial.println("RTC setup complete");

  pinMode(pin_led, OUTPUT);
  
  SPIFFS.begin();

  loadDispConfig(fileconfigdisp, configdisp);
  loadInfoConfig(fileconfiginfo, configinfo);
  
  LoadDataAwal();

  wifiConnect();
  
 server.on("/", []() {
    server.send_P(200, "text/html", setwaktu);

    // Kalau ada perubahan tanggal
    // If there is a change in the date
    if (server.hasArg("date")) {
      uint16_t jam;
      uint8_t menit;
      uint8_t detik;      
      String sd=server.arg("date");
      String lastSd;
      
      jam = ((sd[0]-'0')*1000)+((sd[1]-'0')*100)+((sd[2]-'0')*10)+(sd[3]-'0');
      menit = ((sd[5]-'0')*10)+(sd[6]-'0');
      detik = ((sd[8]-'0')*10)+(sd[9]-'0');
      Serial.print(sd);
      Serial.print("-> ");
      Serial.print(jam);
      Serial.print(".");
      Serial.print(menit);     
      Serial.print(".");
      Serial.println(detik);        
      
      if (sd != lastSd){
        RtcDateTime now = Rtc.GetDateTime();
        uint8_t hour = now.Hour();
        uint8_t minute = now.Minute();
        Rtc.SetDateTime(RtcDateTime(jam, menit, detik, hour, minute, 0));
        lastSd=sd;
        Serial.println("date was set !"); 
      }
     server.send ( 404 ,"text", message );
    }
    
      // Kalau ada perubahaan jam
      // If there is a time change
    if (server.hasArg("time")) {   
      String st=server.arg("time");
      String lastSt;
      uint8_t jam = ((st[0]-'0')*10)+(st[1]-'0');
      uint8_t menit = ((st[3]-'0')*10)+(st[4]-'0');
      Serial.print(st);
      Serial.print("-> ");
      Serial.print(jam);
      Serial.print(":");
      Serial.println(menit);      
      
      if (st != lastSt){
         RtcDateTime now = Rtc.GetDateTime();
         uint16_t year = now.Year();
         uint8_t month = now.Month();
         uint8_t day = now.Day();
         Rtc.SetDateTime(RtcDateTime(year, month, day, jam, menit, 0));
         Serial.println("clock was set !"); 
         lastSt=st;}
      server.send ( 404 ,"text", message );
    }
  });
  
 server.on("/setdisplay", []() {
    server.send_P(200, "text/html", setdisplay);
  });  

  server.on("/settingdisp", HTTP_POST, handleSettingDispUpdate);

 server.on("/setinfo", []() {
    server.send_P(200, "text/html", setinfo);
  });
  
  server.on("/settinginfo", HTTP_POST, handleSettingInfoUpdate);
  
  //server.on ( "/xml", handleXML) ;  

 
  server.begin();
  Serial.println("HTTP server started");
    
  //DMD
  Disp.start();
  Disp.setBrightness(configdisp.cerah);

 }
 
//--------------------------------------------------------
// LOOP

void loop() {   
  server.handleClient();
  
  switch(tmpinfo){
    case 0:
    JamJatuhPulse();
    break;

    case 1:
    TampilHariTanggal();
    break;

    case 2:
    TeksBerjalan1();
    break;
        
    case 3:
    tmpinfo=0;
    break;
  }
  Disp.loop();
 }  // end main loop

//--------------------------------------------------
// TAMPIL JAM ANIMASI
// DISPLAY ANIMATED CLOCK 

void JamJatuhPulse() {
    
  static uint8_t y;
  static uint8_t d;           
  uint32_t cM = millis();

  static uint32_t pMPulse;
  static uint8_t pulse;
  
  if (cM - pMPulse >= 100) {
    pMPulse = cM;
    pulse++;
  }
  
  if (pulse > 8) {
    pulse=0;
  }

  if(cM - pM > 25) { 
    if(d == 0 and y < 16) {
      pM=cM;
      y++;
    }
    if(d  == 1 and y > 0) { // sebelumya 0
      pM=cM;
      y--;
    }    
  }
   
  if(cM - pM > 10000 and y == 16) {
    d=1;
  }
 
  TampilJamDinamis(y - 16);

  if(y == 0 and d == 1) {
  TampilJamDinamis(y - 16);
  delay(30);
  d=0;
    Disp.clear();
    tmpinfo++;
  }    
}

//---------------------------------------------------------
// JAM BESAR
// BIG CLOCK

void TampilJamDinamis(uint32_t y){
  RtcDateTime now = Rtc.GetDateTime();
  char jam[3];
  char menit[3];
  char detik[3];
    
  sprintf(jam,"%02d", now.Hour());
  sprintf(menit,"%02d", now.Minute());
  sprintf(detik,"%02d", now.Second());

//  Serial.print(now.Hour());
//  Serial.print(':');
//  Serial.println(now.Minute());
  //JAM
  Disp.setFont(EMSansSP8x16);
  Disp.drawText(1, y, jam);

  //MENIT          
  Disp.setFont(EMSansSP8x16);
  Disp.drawText(23, y, menit);

  //DETIK          
  Disp.setFont(EMSansSP8x16);
  Disp.drawText(45, y, detik);

  // Tampil titik 2
    Disp.drawRect(20, 4, 21, 5, 1, 1); //koordinat titik dua
    Disp.drawRect(20, 10, 21, 11, 1, 1);
    Disp.drawRect(42, 4, 43, 5, 1, 1); //koordinat titik dua
    Disp.drawRect(42, 10, 43, 11, 1, 1);

  }
   
//-----------------------------------------------------
// TAMPILAN JAM KECIL ATAS
// TOP SMALL CLOCK DISPLAY

void JamKecil() {
  uint32_t cM = millis();
  static uint32_t pMKedip;
  static boolean kedip;

  RtcDateTime now = Rtc.GetDateTime();
  char jam[3];
  char menit[3];
  char detik[3];

  if (cM - pMJam >= 1000) {
    pMJam = cM ;

   //JAM
   sprintf(jam,"%02d", now.Hour());
   Disp.setFont(SystemFont5x7);
   Disp.drawText(7,0,jam);

   //MENIT
   sprintf(menit,"%02d", now.Minute());
   Disp.setFont(SystemFont5x7);
   Disp.drawText(26,0,menit);

   //DETIK
   sprintf(detik,"%02d", now.Second());
   Disp.setFont(SystemFont5x7);
   Disp.drawText(46,0,detik);
   
     }
     
// KEDIP TITIK 2
// DOT FLASHING

  if (millis() - pMKedip >= 500) {
    pMKedip = millis();
    kedip = !kedip;
  }

  if (kedip) {
    Disp.drawRect(21, 1, 22, 2, 1, 1); //koordinat titik dua
    Disp.drawRect(21, 5, 22, 6, 1, 1);
    Disp.drawRect(41, 1, 42, 2, 1, 1); //koordinat titik dua
    Disp.drawRect(41, 5, 42, 6, 1, 1);
  }
  else {
   Disp.drawRect(21, 1, 22, 2, 0, 0); //koordinat titik dua
   Disp.drawRect(21, 5, 22, 6, 0, 0);
   Disp.drawRect(41, 1, 42, 2, 0, 0); //koordinat titik dua
   Disp.drawRect(41, 5, 42, 6, 0, 0);
  }
  
}

//-----------------------------------------------------
// TAMPILAN JAM KECIL BAWAH
// LOWER SMALL CLOCK DISPLAY

void JamKecilBwh() {

  uint32_t cM = millis();
  static uint32_t pMKedip;
  static boolean kedip;

  RtcDateTime now = Rtc.GetDateTime();
  char jam[3];
  char menit[3];
  char detik[3];

  if (cM - pMJam >= 1000) {
    pMJam = cM ;

   //JAM
   sprintf(jam,"%02d", now.Hour());
   Disp.setFont(SystemFont5x7);
   Disp.drawText(7,8,jam);

   //MENIT
   sprintf(menit,"%02d", now.Minute());
   Disp.setFont(SystemFont5x7);
   Disp.drawText(26,8,menit);

   //DETIK
   sprintf(detik,"%02d", now.Second());
   Disp.setFont(SystemFont5x7);
   Disp.drawText(46,8,detik);
   
     }
     
// KEDIP TITIK 2
// DOT FLASHING

  if (millis() - pMKedip >= 500) {
    pMKedip = millis();
    kedip = !kedip;
  }

  if (kedip) {
    Disp.drawRect(21, 9, 22, 10, 1, 1); //koordinat titik dua
    Disp.drawRect(21, 13, 22, 14, 1, 1);
    Disp.drawRect(41, 9, 42, 10, 1, 1); //koordinat titik dua
    Disp.drawRect(41, 13, 42, 14, 1, 1);
  }
  else {
   Disp.drawRect(21, 9, 22, 10, 0, 0); //koordinat titik dua
   Disp.drawRect(21, 13, 22, 14, 0, 0);
   Disp.drawRect(41, 9, 42, 10, 0, 0); //koordinat titik dua
   Disp.drawRect(41, 13, 42, 14, 0, 0);
  }
  
}
  
//----------------------------------------------------
// TAMPILAN HARI DAN TANGGAL
// DAY AND DATE DISPLAY

void TampilHariTanggal(){
  
  JamKecil();
  
  RtcDateTime now = Rtc.GetDateTime ();
  uint32_t cM = millis();
  static uint32_t kecepatan = 70;
  static uint32_t x;
  char tanggal[21]; // SABTU 10 NOP 2019

//  sprintf(tanggal, "%s %02d %s %02d ", weekDay[now.DayOfWeek()], now.Day(), monthYear[now.Month()], now.Year());
  sprintf(tanggal, "%s %02d-%s-%02d ", weekDay[now.DayOfWeek()], now.Day(), monthYear[now.Month()], now.Year());
 
  Disp.setFont(SystemFont5x7);
  int width = Disp.width();
  int fullScroll = Disp.textWidth(tanggal) + width;

   if(cM - pM > kecepatan){
    pM = cM ;
    if(x < fullScroll) {
      ++x;
    } else {
      x = 0 ;
      Disp.clear();
      tmpinfo++;
    }
    Disp.drawText(width - x, 8, tanggal);
   }
  
}

//---------------------------------------------------------------
// TAMPILAN TEXT BERJALAN
// TEXT DISPLAY 

static char *info1[] = {configinfo.info1};

void TeksBerjalan1(){

  static uint32_t x;
  static uint32_t kecepatan = 70 ;
  uint32_t cM = millis();
  
  Disp.setFont(EMSansSP8x16);
  int width = Disp.width();
  int fullScroll = Disp.textWidth(info1 [0]) + width;
  
  if(cM - pM > kecepatan){
    pM = cM ;
    if(x < fullScroll) {
      ++x;
    } else {
     x=0;
     Disp.clear();
     tmpinfo++;
    }
    Disp.drawText(width - x, 0, info1[0]);
   }
  
}

void loadDispConfig(const char *fileconfigdisp, ConfigDisp &configdisp) {

  File configFileDisp = SPIFFS.open(fileconfigdisp, "r");

  if (!configFileDisp) {
    Serial.println("Gagal membuka fileconfigdisp untuk dibaca");
    return;
  }

  size_t size = configFileDisp.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileDisp.readBytes(buf.get(), size);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    Serial.println("Gagal parse fileconfigdisp");
    return;
  }
  
  configdisp.cerah = doc["cerah"];
  
  configFileDisp.close();

}

void handleSettingDispUpdate() {

  timer0_detachInterrupt();
  
  String datadisp = server.arg("plain");
  
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, datadisp);

  File configFileDisp = SPIFFS.open(fileconfigdisp, "w");
  
  if (!configFileDisp) {
    Serial.println("Gagal membuka Display configFile untuk ditulis");
    return;
  }
  
  serializeJson(doc, configFileDisp);

  if (error) {
    
    Serial.print(F("deserializeJson() gagal kode sebagai berikut: "));
    Serial.println(error.c_str());
    return;
    
  } else {
    
    configFileDisp.close();
    Serial.println("Successfully changed configFileDisp");

    server.send(200, "application/json", "{\"status\":\"ok\"}");

    loadDispConfig(fileconfigdisp, configdisp);
    
    delay(500);
    
    Disp.setBrightness(configdisp.cerah);
  
  }  

}

void loadInfoConfig(const char *fileconfiginfo, ConfigInfo &configinfo) {

  File configFileInfo = SPIFFS.open(fileconfiginfo, "r");
  
  if (!configFileInfo) {
    Serial.println("Gagal membuka fileconfiginfo untuk dibaca");
    return;
  }

  size_t size = configFileInfo.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileInfo.readBytes(buf.get(), size);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    Serial.println("Gagal parse fileconfiginfo");
    return;
  }

  strlcpy(configinfo.info1, doc["info1"] | "SELAMAT DATANG", sizeof(configinfo.info1));  // Set awal Info1 
  configFileInfo.close();

}

void handleSettingInfoUpdate() {

  timer0_detachInterrupt();

  String datainfo = server.arg("plain");
  
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, datainfo);

  File configFileInfo = SPIFFS.open(fileconfiginfo, "w");
  
  if (!configFileInfo) {
    Serial.println("Gagal membuka Info configFile untuk ditulis");
    return;
  }
  
  serializeJson(doc, configFileInfo);

  if (error) {
    
    Serial.print(F("deserializeJson() gagal kode sebagai berikut: "));
    Serial.println(error.c_str());
    return;
    
  } else {
    
    configFileInfo.close();
    Serial.println("Successfully changed configFileInfo");
    
    server.send(200, "application/json", "{\"status\":\"ok\"}");    
    
    loadInfoConfig(fileconfiginfo, configinfo);
    
    delay(500);
     
  }  

}

//------------------------------------------------------------------------
// Load Data Awal
// Load Initial Data

void LoadDataAwal(){
  
  if (configdisp.cerah == 0) {
    configdisp.cerah = 150;    
  }
  if (strlen(configinfo.info1) == 0) {
  //  strlcpy(configinfo.info1, "SELAMAT DATANG DI TOKO MULTI KEJAYAN", sizeof(configinfo.info1));
  strlcpy(configinfo.info1, "arduinotehniq by niq_ro", sizeof(configinfo.info1));
  }
 }

//----------------------------------------------------------------------
// FORMAT TEKS
// TEXT FORMAT

void textCenter(int y,String Msg) {
  
  int center = int((Disp.width()-Disp.textWidth(Msg)) / 2);
  Disp.drawText(center,y,Msg);
  
}

//----------------------------------------------------------------------
// I2C_ClearBus menghindari gagal baca RTC (nilai 00 atau 165)
// I2C_ClearBus avoids RTC read failures (values ​​00 or 165)

int I2C_ClearBus() {
  
#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

  pinMode(SDA, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(SCL, INPUT_PULLUP);

  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
  // up of the DS3231 module to allow it to initialize properly,
  // but is also assists in reliable programming of FioV3 boards as it gives the
  // IDE a chance to start uploaded the program
  // before existing sketch confuses the IDE by sending Serial data.

  boolean SCL_LOW = (digitalRead(SCL) == LOW); // Check is SCL is Low.
  if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master. 
    return 1; //I2C bus error. Could not clear SCL clock line held low
  }

  boolean SDA_LOW = (digitalRead(SDA) == LOW);  // vi. Check SDA input.
  int clockCount = 20; // > 2x9 clock

  while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
    clockCount--;
  // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(SCL, INPUT); // release SCL pullup so that when made output it will be LOW
    pinMode(SCL, OUTPUT); // then clock SCL Low
    delayMicroseconds(10); //  for >5uS
    pinMode(SCL, INPUT); // release SCL LOW
    pinMode(SCL, INPUT_PULLUP); // turn on pullup resistors again
    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS
    // The >5uS is so that even the slowest I2C devices are handled.
    SCL_LOW = (digitalRead(SCL) == LOW); // Check if SCL is Low.
    int counter = 20;
    while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(SCL) == LOW);
    }
    if (SCL_LOW) { // still low after 2 sec error
      return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    }
    SDA_LOW = (digitalRead(SDA) == LOW); //   and check SDA input again and loop
  }
  if (SDA_LOW) { // still low
    return 3; // I2C bus error. Could not clear. SDA data line held low
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(SDA, INPUT); // remove pullup.
  pinMode(SDA, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(SDA, INPUT); // remove output low
  pinMode(SDA, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10); // x. wait >5uS
  pinMode(SDA, INPUT); // and reset pins as tri-state inputs which is the default state on reset
  pinMode(SCL, INPUT);
  return 0; // all ok
}
