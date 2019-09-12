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
