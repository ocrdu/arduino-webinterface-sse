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
              client.write(webpage_gz + done, packetsize * sizeof(char));
              done = done + packetsize;
              if (webpage_gz_length - done < packetsize) {
                packetsize = webpage_gz_length - done;
              }
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
