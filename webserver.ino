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
  while (WiFi.status() != WL_CONNECTED) {
    Sprint("Connecting to ");
    Sprintln(ssid);
    WiFi.setHostname("nano-webserver");
    WiFi.begin(ssid, pass);
    delay(5000);
  }
  WiFi.config(ip);
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
  IPAddress ip = WiFi.localIP();
  Sprint("IP Address: ");
  Sprintln(ip);
  long rssi = WiFi.RSSI();
  Sprint("Signal strength (RSSI): ");
  Sprint(rssi);
  Sprintln(" dBm");
  Sprint("Server is at http://");
  Sprint(ip);
  Sprintln("/");
}
#endif
