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
float gdata[3] = {1,1,1};
int ledState = 0;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
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
              sse.print("data: ");
              sse.print(elname);
              sse.print(" set to ");
              sse.println(value);
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
            sse.print("event: volume\ndata: ");
            sse.println(round((volume*100)/255));
            sse.println("");
            sse.print("event: cowbell\ndata: ");
            sse.println((1000-cowbell)/10);
            sse.println("");
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
            Sprint("Bytes to send: ");
            Sprintln(webpage_gz_length);
            while (webpage_gz_length > done) {
              client.write(webpage_gz + done, packetsize * sizeof(char));
              done = done + packetsize;
              Sprint("Bytes sent: ");
              Sprintln(done);
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
  
  unsigned long currentMillis1 = millis();
  if ((currentMillis1 - previousMillis1) > cowbell) {
    previousMillis1 = currentMillis1;   
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
    for (int i=0; i<2; i++) {gdata[i] = gdata[i+1];}
    gdata[2] = z;
    for (int i=0; i<3; i++) {zsum += gdata[i];}
    if (sse.connected()) {
      unsigned long currentMillis2 = millis();
      if ((currentMillis2 - previousMillis2) > 200) {
        previousMillis2 = currentMillis2;
        sse.println("event: gravity\ndata: " + String(zsum/3, 2) + "\n");
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
