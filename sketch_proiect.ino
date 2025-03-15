#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <DHT.h>

// Configurare WiFi
const char* ssid = "iPhone - Teodora"; 
const char* parola = "teodoraa"; 

// Definire pini
#define PIN_DHT 4        
#define TIP_DHT DHT11   
#define PIN_SENS_LUMINA 26  
#define PIN_SENS_IR 32  
#define PIN_LED 2        
#define PIN_BUTON 4      

// Obiecte pentru senzori și LCD
DHT dht(PIN_DHT, TIP_DHT);
LiquidCrystal_PCF8574 lcd(0x27);  // Adresa I2C 0x27 pentru LCD
AsyncWebServer server(80);

// Variabile
int stare_led = LOW;
int stare_buton, ultima_stare_buton = LOW;
unsigned long ultima_timp_debounce = 0, timp_debounce = 50;

// HTML pentru interfața web
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; text-align: center;}
    h2 {font-size: 2.0rem;}
    p {font-size: 1.5rem;}
    .switch {position: relative; display: inline-block; width: 60px; height: 34px}
    .switch input {display: none}
    .slider {position: absolute; background-color: #ccc; border-radius: 34px; width: 100%; height: 100%}
    .slider:before {content: ""; position: absolute; background: white; width: 26px; height: 26px; transition: 0.4s; left: 4px; bottom: 4px; border-radius: 50%}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {transform: translateX(26px)}
  </style>
</head>
<body>
  <h2>ESP Date Senzori</h2>
  <h3>Stare LED: <span id="stare_led">%STARE_LED%</span></h3>
  <label class="switch">
    <input type="checkbox" onchange="comutaLED(this)" %LED_CHECKED%>
    <span class="slider"></span>
  </label>
  <h3>Temperatura: <span id="temperatura">%TEMPERATURA%</span>°C</h3>
  <h3>Umiditatea: <span id="umiditate">%UMIDITATE%</span>%</h3>
  <h3>Nivel Lumină: <span id="lumina">%LUMINA%</span></h3>
  <h3>Sens IR: <span id="ir">%STARE_IR%</span></h3>

  <script>
    function comutaLED(element) {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/update?state=" + (element.checked ? "1" : "0"), true);
      xhr.send();
    }
    setInterval(function() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var data = JSON.parse(this.responseText);
          document.getElementById("temperatura").innerText = data.temperatura;
          document.getElementById("umiditate").innerText = data.umiditate;
          document.getElementById("lumina").innerText = data.lumina;
          document.getElementById("ir").innerText = data.ir ? "Detectat" : "Necunoscut";
          document.getElementById("stare_led").innerText = data.led ? "Pornit" : "Oprit";
        }
      };
      xhttp.open("GET", "/data", true);
      xhttp.send();
    }, 2000);
  </script>
</body>
</html>
)rawliteral";

// Funcție pentru a înlocui placeholder-ele din HTML
String procesor(const String& var) {
  if (var == "STARE_LED") return stare_led ? "Pornit" : "Oprit";
  if (var == "LED_CHECKED") return stare_led ? "checked" : "";
  return String();
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin(21, 22);  // Conexiune I2C pentru ESP32
  lcd.begin(16, 2);     // LCD de 16x2 caractere
  lcd.setBacklight(255);
  
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTON, INPUT);
  pinMode(PIN_SENS_IR, INPUT);
  digitalWrite(PIN_LED, stare_led);

  WiFi.begin(ssid, parola);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectare la WiFi...");
  }
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, procesor);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("state")) {
      stare_led = request->getParam("state")->value().toInt();
      digitalWrite(PIN_LED, stare_led);
    }
    request->send(200, "text/plain", "OK");
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    int lumina = analogRead(PIN_SENS_LUMINA);
    int ir = digitalRead(PIN_SENS_IR);
    
    String json = "{";
    json += "\"temperatura\":" + String(temp) + ","; 
    json += "\"umiditate\":" + String(hum) + ","; 
    json += "\"lumina\":" + String(lumina) + ","; 
    json += "\"ir\":" + String(ir) + ","; 
    json += "\"led\":" + String(stare_led);
    json += "}";
    
    request->send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
  int citire = digitalRead(PIN_BUTON);  // Citește starea butonului

  // Verifică schimbarea stării butonului (debounce)
  if (citire != ultima_stare_buton) {
    ultima_timp_debounce = millis();
  }

  // Verifică dacă a trecut timpul de debounce
  if ((millis() - ultima_timp_debounce) > timp_debounce) {
    if (citire != stare_buton) {
      stare_buton = citire;
      if (stare_buton == HIGH) {
        stare_led = !stare_led;  // Schimbă starea LED-ului
        digitalWrite(PIN_LED, stare_led);  // Actualizează LED-ul
      }
    }
  }

  ultima_stare_buton = citire;  // Salvează starea butonului pentru următoarea iterație

  // Citește valorile de temperatură și umiditate
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Actualizează LCD-ul
  lcd.clear(); // Șterge ecranul pentru a evita suprapunerea textului
  lcd.setCursor(0, 0);
  lcd.print("Temp: " + String(temp) + "C");
  lcd.setCursor(0, 1);
  lcd.print("Hum: " + String(hum) + "%");

  delay(2000); // Așteaptă 2 secunde înainte de a actualiza din nou
}