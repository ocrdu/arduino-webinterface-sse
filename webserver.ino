#include <WiFiNINA.h>
#include <Arduino_LSM6DS3.h>
#include <Base64.h>
#include "wifi_secrets.h"

//#define DEBUG 1

#ifdef DEBUG
  #define Sprint(a) (Serial.print(a))
  #define Sprintln(a) (Serial.println(a))
#else
  #define Sprint(a)
  #define Sprintln(a)
#endif

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

IPAddress ip(192, 168, 1, 99);
WiFiServer server(80);
WiFiClient sse;
int ledPin = 12;
boolean on = false;
float volume = 0;
int cowbell = 1000;
float zdata[] = {1,1,1,1,1};
int zdatasize = sizeof(zdata) / sizeof(zdata[0]);
int ledState = 0;
unsigned long previousCowbellMillis = 0;
unsigned long previousGravityMillis = 0;

char webpage_base64[] = "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";

void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPin, OUTPUT);
  IMU.begin();
  WiFiConnect();
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    Sprintln("\nLost WiFi connection");
    sse.stop();
    WiFi.end();
    WiFiConnect();
  }
  
  WiFiClient client = server.available();

  if (client) {
    Sprintln("\n--New client");
    String header = "";
    String elname = "";
    String value = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c != '\r') {
          header += c;
        }
        if (header.substring(header.length() - 2) == "\n\n") {
          Sprint(header.substring(0, header.length() - 1));
          if (header.indexOf("HEAD /?") > -1) {
            client.println("HTTP/1.1 200 OK");
            #ifdef DEBUG
            client.println("Access-Control-Allow-Origin: *");
            #endif
            client.println("");
            client.stop();
            header.remove(0, header.indexOf("?")+1);
            header.remove(header.indexOf(" "));
            elname = header.substring(0, header.indexOf("="));
            value = header.substring(header.indexOf("=") + 1);
            boolean goodSettings = true;
            if (elname == "switch") {
              on = (value == "true");
            } else if (elname == "volume") {
              volume = (value.toFloat()*255)/100;
            } else if (elname == "cowbell") {
              cowbell = 1000 - (value.toInt()*10);
            } else {
              goodSettings = false;
            }
            if (goodSettings) {
              sse.println("data: " + elname + " set to " + value);
            } else {
              sse.println("Bad data; ignored");
            }
            sse.println("");
            Sprintln("--Client disconnected");
          } else if (header.indexOf("event-stream") > -1) {
            if (sse.connected()) {
              sse.stop();
            }
            client.println("HTTP/1.1 200 OK\nConnection: Keep-Alive");
            #ifdef DEBUG
            client.println("Access-Control-Allow-Origin: *");
            #endif
            client.println("Content-Type: text/event-stream\nCache-Control: no-cache\n");
            sse = client;
            if (on) {
              sse.println("event: sw\ndata: true\n");
            } else {
              sse.println("event: sw\ndata: false\n");
            }
            sse.println("event: volume\ndata: " + (String)round((volume*100)/255) + "\n");
            sse.println("event: cowbell\ndata: " + (String)((1000-cowbell)/10) + "\n");
            Sprintln("--SSE client established");
            break;
          } else if (header.indexOf("POST / HTTP") > -1) {
            client.println("HTTP/1.1 200 OK\n");
            client.stop();
            sse.stop();
            Sprintln("--Interface page was closed; all clients disconnected");
          } else if (header.indexOf("GET / HTTP") > -1) {
            client.println("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Encoding: gzip\n");
            const int webpage_base64_length = sizeof(webpage_base64);
            const int webpage_gz_length = base64_dec_len(webpage_base64, webpage_base64_length);
            char webpage_gz[webpage_gz_length];
            base64_decode(webpage_gz, webpage_base64, webpage_base64_length);
            int packetsize = 1024;
            int done = 0;
            while (webpage_gz_length > done) {
              if (webpage_gz_length - done < packetsize) {
                packetsize = webpage_gz_length - done;
              }
              client.write(webpage_gz + done, packetsize * sizeof(char));
              done = done + packetsize;
            }
            client.stop();
            Sprintln("--Interface webpage sent; client disconnected");
          } else if (header.indexOf("GET /author HTTP") > -1) {
            client.println("HTTP/1.1 200 OK\nContent-Type: text/plain; charset=utf-8\n\nOscar den Uijl, oscar@den-uijl.nl\n");
            client.stop();
            Sprintln("--Email address sent; client disconnected");
          } else {
            client.println("HTTP/1.1 404 Not Found\nContent-Type: text/plain; charset=utf-8\n\n404 Not Found\n");
            client.stop();
            Sprintln("--Page not found; client disconnected");
          }
        }
      }
    }
  }
  
  unsigned long currentCowbellMillis = millis();
  if ((currentCowbellMillis - previousCowbellMillis) > cowbell) {
    previousCowbellMillis = currentCowbellMillis;   
    if (on) {
      if (cowbell == 1000) {
        analogWrite(ledPin, (int) round(volume));
      } else {
        if (ledState == 0) {
          ledState = 1;
          analogWrite(ledPin, (int) round(volume));
        } else {
          ledState = 0;
          analogWrite(ledPin, 0);
        }
      }
    } else {
      analogWrite(ledPin, 0);
    }
  }

  float x, y, z, zsum = 0;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    if (x > 0.2 || x < -0.2 || y > 0.2 || y < -0.2 || z > 1.2 || z < 0.8) {
      analogWrite(ledPin, 0);
      Sprintln("\nTILT!!");
      if (on) {
        on = false;
        if (sse.connected()) {
          sse.println("event: sw\ndata: false\n");
        }
      }
    }
    for (int i=0; i<zdatasize-1; i++) {
      zdata[i] = zdata[i+1];
    }
    zdata[zdatasize-1] = z;
    for (int i=0; i<zdatasize; i++) {
      zsum += zdata[i];
    }
    if (sse.connected()) {
      unsigned long currentGravityMillis = millis();
      if ((currentGravityMillis - previousGravityMillis) > 200) {
        previousGravityMillis = currentGravityMillis;
        sse.println("event: gravity\ndata: " + String(zsum/zdatasize, 3) + "\n");
      }
    }
  }
}

void WiFiConnect() {
  WiFi.config(ip);
  WiFi.setHostname("nano-webserver");
  while (WiFi.status() != WL_CONNECTED) {
    Sprint("Connecting to ");
    Sprintln(ssid);
    WiFi.begin(ssid, pass);
    delay(5000);
  }
  Sprint("Connected to ");
  Sprintln(ssid);
  server.begin();
  #ifdef DEBUG
  printWifiStatus();
  #endif
  WiFi.lowPowerMode();
  digitalWrite(LED_BUILTIN, HIGH);
}

#ifdef DEBUG
void printWifiStatus() {
  Sprint("SSID: ");
  Sprintln(WiFi.SSID());
  Sprint("Signal strength (RSSI): ");
  Sprint(WiFi.RSSI());
  Sprintln(" dBm");
  Sprint("IP address: ");
  Sprintln(WiFi.localIP());
  Sprint("Gateway: ");
  Sprintln(WiFi.gatewayIP());
  Sprint("Netmask: ");
  Sprintln(WiFi.subnetMask());
  Sprint("Webserver is at http://");
  Sprint(ip);
  Sprintln("/");
}
#endif
