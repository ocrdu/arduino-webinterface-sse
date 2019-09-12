#include <WiFiNINA.h>
#include <Base64.h>
#include "wifi_secrets.h"
#include <Arduino_LSM6DS3.h>

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
boolean goodSettings = false;
float volume = 0;
int cowbell = 1000;
int ledState = 0;
unsigned long previousMillis = 0;
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
    while (client.connected()) {            // loop while the client is connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte
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
            client.println();
            client.stop();
            header.remove(0, header.indexOf("?")+1);
            header.remove(header.indexOf(" "));
            elname = header.substring(0, header.indexOf("="));
            value = header.substring(header.indexOf("=") + 1);
            goodSettings = true;
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
            sse.stop();
            client.println("HTTP/1.1 200 OK");
            client.println("Connection: Keep-Alive");
            #ifdef DEBUG
            client.println("Access-Control-Allow-Origin: *");
            #endif
            client.println("Content-Type: text/event-stream");
            client.println("Cache-Control: no-cache");
            client.println("");
            sse = client;
            sse.println("event: sw");
            sse.print("data: ");
            if (on) {
              sse.println("true");
            }
            else {
              sse.println("false");
            }
            sse.println("");
            sse.println("event: volume");
            sse.print("data: ");
            sse.println(round((volume*100)/255));
            sse.println("");
            sse.println("event: cowbell");
            sse.print("data: ");
            sse.println((1000-cowbell)/10);
            sse.println("");
            Sprintln("--SSE client established");
            break;
          } else if (header.indexOf("POST / HTTP") > -1) {
            client.println("HTTP/1.1 200 OK");
            client.println("");
            client.stop();
            sse.stop();
            Sprintln("--Interface page was closed; all clients disconnected");
          } else if (header.indexOf("GET / HTTP") > -1) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Content-Encoding: gzip");
            client.println("");
            const int webpage_base64_length = sizeof(webpage_base64);
            const int webpage_gz_length = base64_dec_len(webpage_base64, webpage_base64_length);
            char webpage_gz[webpage_gz_length];
            base64_decode(webpage_gz, webpage_base64, webpage_base64_length);
            client.write(webpage_gz, webpage_gz_length);
            client.stop();
            Sprintln("--Interface webpage sent; client disconnected");
          } else if (header.indexOf("GET /author HTTP") > -1) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain; charset=utf-8");
            client.println("");
            client.println("Oscar den Uijl, oscar@den-uijl.nl");
            client.stop();
            Sprintln("--Email address sent; client disconnected");
          } else {
            client.println("HTTP/1.1 404 Not Found");
            client.println("Content-Type: text/plain; charset=utf-8");
            client.println("");
            client.println("404 Not Found");
            client.stop();
            Sprintln("--Page not found; client disconnected");
          }
        }
      }
    }
  }
  
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) > cowbell) {
    previousMillis = currentMillis;   
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

  float x, y, z;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    if (x > 0.2 || x < -0.2 || y > 0.2 || y < -0.2 || z > 1.2 || z < 0.8) {
      analogWrite(ledPin, 0);
      Sprintln("\nTILT!!");
      if (on) {
        on = false;
        sse.println("event: sw");
        sse.print("data: ");
        sse.println("false");
        sse.println("");
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
