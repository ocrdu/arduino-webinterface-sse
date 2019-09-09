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
