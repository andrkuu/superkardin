#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

/*
   This example serves a "hello world" on a WLAN and a SoftAP at the same time.
   The SoftAP allow you to configure WLAN parameters at run time. They are not setup in the sketch but saved on EEPROM.
   Connect your computer or cell phone to wifi network ESP_ap with password 12345678. A popup may appear and it allow you to go to WLAN config. If it does not then navigate to http://192.168.4.1/wifi and config it there.
   Then wait for the module to connect to your wifi and take note of the WLAN IP it got. Then you can disconnect from ESP_ap and return to your regular WLAN.
   Now the ESP8266 is in your network. You can reach it through http://192.168.x.x/ (the IP you took note of) or maybe at http://esp8266.local too.
   This is a captive portal because through the softAP it will redirect any http request to http://192.168.4.1/
*/

/* Set these to your desired softAP credentials. They are not configurable at runtime */
#ifndef APSSID
#define APSSID "Kaardin"
#define APPSK  ""
#endif

const char *softAP_ssid = APSSID;
const char *softAP_password = APPSK;

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "esp8266";

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[33] = "";
char password[65] = "";

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

/* Soft AP network parameters */
IPAddress apIP(172, 217, 28, 1);
IPAddress netMsk(255, 255, 255, 0);


/** Should I connect to WLAN asap? */
boolean connect;

/** Last time I tried to connect to WLAN */
unsigned long lastConnectTry = 0;

/** Current WLAN status */
unsigned int status = WL_IDLE_STATUS;

bool LED1status = LOW;

uint8_t LED1pin = 2;



 int writeBit0 = D0;
 int writeBit1 = D1;
 int writeBit2 = D2;

int b0;
int b1; 
int b2;

int analogVAL = 0;

int delayVal = 0;

String picture1 = "";

String SendHTML(uint8_t led1stat,uint8_t led2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  //ptr +="<title>Superuloo</title><style>#title{width: 100%;height: 10%;background-color: rgb(161, 161, 161);position: absolute;bottom: 0;left: 0;top: 0;color: white;font-size:10vw;text-align:center;line-height:10vh;border-radius:5px;}#stop{width:80%;height:10%;background-color:brown;position:absolute;bottom:5%;left:10%;color:white;font-size:10vw;text-align:center;line-height:10vh;border-radius:15px;}.modes{margin-top:25%;width:50%;height:80%;float:left;}#light_img{width:80%;margin-left:5%;margin-bottom:10%;background-color:rgb(228,228,228);border-radius:15px;}#arrow{width:80%;margin-left:5%;margin-bottom:10%;border-radius:15px;}.move{margin-top:50%;width:50%;height:80%;float:right;}@mediaonlyscreenand(min-width:700px){#light_img{width:50%;margin-left:25%;}#arrow{width:50%;margin-left:25%;}@mediaonlyscreenand(min-width:1000px){#light_img{width:20%;margin-left:5%;}#arrow{width:20%;margin-left:5%;}#title{font-size:4vw;}.modes{margin-top:12%;}.move{margin-top:12%;}#stop{font-size:4vw;}}</style>";

  ptr +="<title>Kaardin</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"title\">SupeRuloo</div>";

  /*
  ptr +="<div class=\"modes\">";
  ptr +="<a href=\"curtain_up\"> <img id=\"light_img\"";
  ptr +="<a href=\"curtain_middle\"> <img id=\"light_img\"";
  ptr +="<a href=\"curtain_down\"> <img id=\"light_img\"";
  ptr +="</div>";
  
  //ptr +="<div class=\"move\">";
  //ptr +="<a href=\"arrow_up\"> <img id=\"arrow\" src=\"data:image/png;base64,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\" alt=\"Light sensor off\"/></a>";
  //ptr +="<a href=\"arrow_down\"> <img id=\"arrow\" src=\"data:image/png;base64,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\" alt=\"Light sensor off\"/></a>";
  //ptr +="</div>";
  */


  ptr +="<a class=\"button button-on\" href=\"/handle_0_0_0\">Stop</a>\n";

  ptr +="<a class=\"button button-on\" href=\"/handle_0_0_1\">Ülesse</a>\n";
 
  ptr +="<a class=\"button button-on\" href=\"/handle_0_1_0\">Alla</a>\n";

   /*

  ptr +="<a class=\"button button-on\" href=\"/handle_0_1_1\">0 1 1</a>\n";

  ptr +="<a class=\"button button-on\" href=\"/handle_1_0_0\">1 0 0</a>\n";

  ptr +="<a class=\"button button-on\" href=\"/handle_1_0_1\">1 0 1</a>\n";

  ptr +="<a class=\"button button-on\" href=\"/handle_1_1_0\">1 1 0</a>\n";

  ptr +="<a class=\"button button-on\" href=\"/handle_1_1_1\">1 1 1</a>\n";*/

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}


/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
 
    Serial.println("Request redirected to captive portal");
    server.send(200, "text/html", SendHTML(false,LED1status)); 
    return true;
}

void sendBinary(){

  Serial.print(b0);
  Serial.print(b1);
  Serial.println(b2);
  delay(500);
  digitalWrite(writeBit0, b0);
  digitalWrite(writeBit1, b1);
  digitalWrite(writeBit2, b2); 
  /*
  if (delayVal <= 1) {
    
    Serial.print(b0);
    Serial.print(b1);
    Serial.println(b2);
    delayVal++;
  }
  else
  {
    b0 = 0;
    b1 = 0;
    b2 = 0;
    delayVal=0;
  }*/
  
}

void setBinary(int a, int b, int c){
  b0 = a;
  b1 = b;
  b2 = c;
  //Serial.print(b0);
  //Serial.print(b1);
  //Serial.println(b2);

}



void handle_curtain_up(){
  LED1status = HIGH;
  Serial.println("GPIO7 Status: ON");
  analogVAL = 0;
  //analogWrite(AnalogPin, 0);
  //Serial.println(analogRead(AnalogPin));
  setBinary(0,0,1);  
  server.send(200, "text/html", SendHTML(true,LED1status)); 
}

void handle_curtain_down(){
  LED1status = LOW;
  Serial.println("GPIO7 Status: OFF");
  analogVAL = 200;

  setBinary(1,1,1);  
  //analogWrite(AnalogPin, analogVAL);
  
  
  server.send(200, "text/html", SendHTML(false,LED1status)); 
}

boolean handle_0_0_0(){
  setBinary(0,0,0);   
  server.send(200, "text/html", SendHTML(false,LED1status)); 
  return true;
}

boolean handle_0_0_1(){
  setBinary(0,0,1);   
  server.send(200, "text/html", SendHTML(false,LED1status)); 
  return true;
}

boolean handle_0_1_0(){
  setBinary(0,1,0);  
  server.send(200, "text/html", SendHTML(false,LED1status)); 
  return true;
}

boolean handle_0_1_1(){
  setBinary(0,1,1);   
  server.send(200, "text/html", SendHTML(false,LED1status)); 
  return true;
}

boolean handle_1_0_0(){
  setBinary(1,0,0);   
  server.send(200, "text/html", SendHTML(false,LED1status)); 
  return true;
}

boolean handle_1_0_1(){
  setBinary(1,0,1);   
  server.send(200, "text/html", SendHTML(false,LED1status)); 
  return true;
}

boolean handle_1_1_0(){
  setBinary(1,1,0);   
  server.send(200, "text/html", SendHTML(false,LED1status)); 
  return true;
}


boolean handle_1_1_1(){
  setBinary(1,1,1);   
  server.send(200, "text/html", SendHTML(false,LED1status)); 
  return true;
}


void setup() {
  delay(1000);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid, softAP_password);
  delay(500); // Without delay I've seen the IP address blank
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */

  server.on("/handle_0_0_0", handle_0_0_0);
  server.on("/handle_0_0_1", handle_0_0_1);
  server.on("/handle_0_1_0", handle_0_1_0);
  server.on("/handle_0_1_1", handle_0_1_1);
  server.on("/handle_1_0_0", handle_1_0_0);
  server.on("/handle_1_0_1", handle_1_0_1);
  server.on("/handle_1_1_0", handle_1_1_0);
  server.on("/handle_1_1_1", handle_1_1_1);


  server.on("/", captivePortal);
  server.on("/curtain_up", handle_curtain_up);
  server.on("/curtain_down", handle_curtain_down);
  server.on("/generate_204", captivePortal);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", captivePortal);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.

  server.begin(); // Web server start
  Serial.println("HTTP server started");
  connect = strlen(ssid) > 0; // Request WLAN connect if there is a SSID
}

void connectWifi() {
  Serial.println("Connecting as wifi client...");
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  int connRes = WiFi.waitForConnectResult();
  Serial.print("connRes: ");
  Serial.println(connRes);
}

void loop() {
  
  if (connect) {
    Serial.println("Connect requested");
    connect = false;
    connectWifi();
    lastConnectTry = millis();
  }
  {
    unsigned int s = WiFi.status();
    if (s == 0 && millis() > (lastConnectTry + 60000)) {
      /* If WLAN disconnected and idle try to connect */
      /* Don't set retry time too low as retry interfere the softAP operation */
      connect = true;
    }
    if (status != s) { // WLAN status change
      Serial.print("Status: ");
      Serial.println(s);
      status = s;
      if (s == WL_CONNECTED) {
        /* Just connected to WLAN */
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        // Setup MDNS responder
        if (!MDNS.begin(myHostname)) {
          Serial.println("Error setting up MDNS responder!");
        } else {
          Serial.println("mDNS responder started");
          // Add service to MDNS-SD
          MDNS.addService("http", "tcp", 80);
        }
      } else if (s == WL_NO_SSID_AVAIL) {
        WiFi.disconnect();
      }
    }
    if (s == WL_CONNECTED) {
      MDNS.update();
    }
  }
  // Do work:
  //delay(1000);
  //analogVAL = 200;
  //Serial.println("Analog val : "+ analogVAL);
   //DNS
  dnsServer.processNextRequest();
  
  sendBinary();

  
    //DNS
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();
 
  
  //Serial.println(analogRead(AnalogPin));
  

}
