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
