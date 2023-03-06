#include <WiFi.h>
#include <WebServer.h>

#define LEDpin 16
#define MOTORpin 17

bool LED1status = LOW;

/* Установите здесь свои SSID и пароль */
const char* ssid = "YOKO";  
const char* password = "77777777";  
/* Настройки IP адреса */
IPAddress local_ip(192,168,2,1);
IPAddress gateway(192,168,2,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

uint32_t SOS[] = {150, 400, 150, 400};
uint32_t silence = 200;
uint8_t noOfbeeps = 4;

void setup() {
  Serial.begin(115200); 
    WiFi.softAP(ssid, password);
  delay(2000);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
    server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");


  pinMode(LEDpin, OUTPUT);
  pinMode(MOTORpin, OUTPUT);
}
void loop() {
    server.handleClient();
  if(LED1status){
    morse();
  }
  else
  {
    Serial.println("Disabled");
  }
  
}

void morse() { 
  uint32_t currentMillis = millis();
  static uint8_t index = 0;
  static uint32_t lastMillis;
  static uint32_t waitTime;
  static bool toneOn;
  if (currentMillis - lastMillis >= waitTime) {
    lastMillis = currentMillis;
    if (toneOn) {
      toneOn = false;
       digitalWrite(LEDpin, LOW);
      digitalWrite(MOTORpin, LOW);
      waitTime = silence;
    } else {
      toneOn = true;
      digitalWrite(LEDpin, HIGH);
      digitalWrite(MOTORpin, HIGH);
      waitTime = SOS[index];
      index = (index + 1) % 4;
    }
  }
}


void handle_OnConnect() {
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF | GPIO16 Status: OFF | GPIO17 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status)); 
}
void handle_led1on() {
  LED1status = HIGH;
  Serial.println("GPIO4 Status: ON | GPIO5 Status: ON | GPIO16 Status: ON | GPIO17 Status: ON");
  server.send(200, "text/html", SendHTML(true)); 
}
void handle_led1off() {
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF | GPIO16 Status: OFF | GPIO17 Status: OFF");
  server.send(200, "text/html", SendHTML(false)); 
}
void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
String SendHTML(uint8_t led1stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Управление светодиодом</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Веб сервер</h1>\n";
    ptr +="<h3>Режим точка доступа WiFi (AP)</h3>\n";
   if (led1stat)
  {ptr +="<p>Состояние LED1: ВКЛ.</p><a class=\"button button-off\" href=\"/led1off\">ВЫКЛ.</a>\n";}
  else
  {ptr +="<p>Состояние LED1: ВЫКЛ.</p><a class=\"button button-on\" href=\"/led1on\">ВКЛ.</a>\n";}
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
