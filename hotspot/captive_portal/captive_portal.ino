#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>


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

String SendHTML(){
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

  ptr +="<a class=\"button button-on\" href=\"/handle_0_0_0\">Stop</a>\n";

  ptr +="<a class=\"button button-on\" href=\"/handle_0_0_1\">Ülesse</a>\n";
 
  ptr +="<a class=\"button button-on\" href=\"/handle_0_1_0\">Alla</a>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}


/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
 
    Serial.println("Request redirected to captive portal");
    server.send(200, "text/html", SendHTML()); 
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

}

void setBinary(int a, int b, int c){
  b0 = a;
  b1 = b;
  b2 = c;
  //Serial.print(b0);
  //Serial.print(b1);
  //Serial.println(b2);

}


boolean handle_0_0_0(){
  setBinary(0,0,0);   
  server.send(200, "text/html", SendHTML()); 
  return true;
}

boolean handle_0_0_1(){
  setBinary(0,0,1);   
  server.send(200, "text/html", SendHTML()); 
  return true;
}

boolean handle_0_1_0(){
  setBinary(0,1,0);  
  server.send(200, "text/html", SendHTML()); 
  return true;
}

boolean handle_0_1_1(){
  setBinary(0,1,1);   
  server.send(200, "text/html", SendHTML()); 
  return true;
}

boolean handle_1_0_0(){
  setBinary(1,0,0);   
  server.send(200, "text/html", SendHTML()); 
  return true;
}

boolean handle_1_0_1(){
  setBinary(1,0,1);   
  server.send(200, "text/html", SendHTML()); 
  return true;
}

boolean handle_1_1_0(){
  setBinary(1,1,0);   
  server.send(200, "text/html", SendHTML()); 
  return true;
}


boolean handle_1_1_1(){
  setBinary(1,1,1);   
  server.send(200, "text/html", SendHTML()); 
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
