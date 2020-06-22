#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

ESP8266WebServer server;

char* ssid = "ssid";
char* password = "password";

#define pinLed 10

char webpage[] PROGMEM = R"====(
<html>
<head> 
</head>
<body>
<p> LED State: <span id= "led-state">_</span></p>
<button onclick="myFunction()"> TOGGLE </button>
</body>
<script>
 function myFunction(){
   console.log("button was clicked!");
   var xhr = new XMLHttpRequest(); 
   var url = "/ledstate";
   
   xhr.onreadystatechange = function(){
      if(this.readyState == 4 && this.status == 200){
        document.getElementById("led-state").innerHTML = this.responseText;  
      }
   };
   xhr.open("GET", url, true);
   xhr.send();
 };
 document.addEventListener('DOMContentLoaded', myFunction, false);
</script>
</html>
)====";


void setup()
{
    // put your setup code here, to run once:
    SPIFFS.begin();
    Serial.begin(115200);
    pinMode(pinLed, OUTPUT);
    digitalWrite(pinLed, LOW);
    Serial.println("");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(200);
    }
    Serial.println("");
    Serial.print("IP Address:");
    Serial.print(WiFi.localIP());

    server.on("/", []() {
        //server.send(200,"text/html","Hello From Server!");
        server.send_P(200, "text/html", webpage);
    });
    server.on("/toggle", toggleLed);
    server.on("/ledstate", getLedState);
    server.begin();
}

void loop()
{
    // put your main code here, to run repeatedly:
    server.handleClient();
}

void getLedState()
{
    toggleLed();
    Serial.println("inside getLedState");
    String led_state = digitalRead(pinLed) ? "ON" : "OFF";
    server.send(200, "text/plain", led_state);
}

void toggleLed(void)
{
    static uint8_t count = 1;
    Serial.print("toggleLed: ");
    Serial.println(count);
    count++;
    digitalWrite(pinLed, !digitalRead(pinLed));
}
