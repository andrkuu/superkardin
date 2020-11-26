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


String SendHTML(uint8_t led1stat,uint8_t led2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
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
  ptr +="<h1>Mingisugune kaardin</h1>\n";
  
   if(led1stat)
  {ptr +="<a class=\"button button-off\" href=\"/curtain_down\">Ülesse</a>\n";}
  else
  {ptr +="<a class=\"button button-on\" href=\"/curtain_up\">Alla</a>\n";}

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
  digitalWrite(writeBit0, b0);
  digitalWrite(writeBit1, b1);
  digitalWrite(writeBit2, b2);
  Serial.print(b0);
  Serial.print(b1);
  Serial.println(b2);
}

void setBinary(int a, int b, int c){
  b0 = a;
  b1 = b;
  b2 = c;
  Serial.print(b0);
  Serial.print(b1);
  Serial.println(b2);

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
  
  sendBinary();
  
  //Serial.println(analogRead(AnalogPin));
  
  //DNS
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();


  

}
