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

char webpage_base64[] = "H4sICEMZgl0CAGludGVyZmFjZTMuaHRtbADtHGtz4zTwOzP8B2PmoOVqx5bfvaaM0zRtgYPm6PEo8MGxFcfUsYPtJG0Z/ju7khzn0Zam9JhhpoHG2tXuane1q1Ukw8En3e+OLn4+P5ZG1Tg9/PijA3xKaZDFbZlmMmAk6WBEg4i1oF0lVUoPz7KKFsMgpActjhC9Y1oFUjgKipJWbXlaDRVXrvvSJLuSCpq25XKUF1U4raQkzDNZGhV02JajoAr2k3EQ09a1gh1vBkFJbXPP9/2O7x/7x/CNzzO/k2OzF8PXUYzIPvwBvu5f+nTefgPfF0dzf+vP5fxM74z7p2+/8b73O+OO/5TP6c/V9323gU/e+2+LXuZ3L1qXvj+P++v0ftrvfAUmlIOLuf/+dEnv7/v+28vMz/pnvfnJ+9MfB37utsj86/Nzb+x3z1uXsX8r4PLo99sW9t8NL+nTe++fdvULv/PHKdirnZDQ//bHakke6DvzqqOLWy248a3yB9P/+mgU+0fz0SC+et376Sztx2/XjOhc+V3/+rJzfH181ic/daO80x3ql33/pkV+9t+2vNTvftMaxPPXr43+1bvR8eP92QuPzvx3cSe5hfnxdS5P2O/71y2jf/muSHpvdfPk0TIxnm5/PkP9PC6vsZ/DmX/xztx26jP9td8/uTr5tjUCe0+FvEY+jhfHj5c3GY/Pjs5C/aeyk/jft4S8xp9B/73RmvROBqdXjxd64ob9TuePbvd3k83Ppv3db8/Hnbe9n3/awp/f+Z335z/O/dQ9if1vh8vyAJ55l/7Rqf+u7592trB/+Pu7i65xfNmfQXxt+vMa9L8+n2+bn36/3/eKzm0xv8v+xH/KpwPzcXyO8twTSNpzPj9M/hjg09B/yifwfb1LrrrfzlLt5HhwM+jnrnuC+TzRT96HP28rT+STf3o9YutDcpEmvbH74/nNDwwe3V4Oj7YQ1bp1Z7Oh2coHRuyf2Z7TaZ3MzuPWqDubn3ux4566vdnQbyWOEQ/nrz3z1KvOv+u0Jl0jbnmlZ563vj6np62oa4xahNrO0L2a9dvtRQEpqxtWaQZ5dCP9ichBEF7FRT7NIiXM07zYlz49to6tXu/Nxx/99fFHkz2oZAOacuJhnlXKMBgn6c2+5BdJkO5JZZCVSkmLZMg5VKg7GQ2rJM+kchZzxhktqiQMUiVIkzjblwZ5VeVjzrAuf06TeFQhTRq9WWDL5JbuS7pqz+bNOFWRpyVnnSdRNQICTXv1RhoJEUTTJtdMRpSUkzQArYcp5RimiZJUdFzuSyHFaiwEl/OkCke0vM9B8xFwvVke1Foa0wEFGF9eRBSoyzxNIkkHPQRKKYIomcKgFuLGQREnmZLSIfAqjGwSRFGSxUCwlfKI/31aVsnwRkHfAHalL58EYVKBFE01hKV5lg+H3NwVL9qNExkgyGvKFZ2yPKMr/c18NkSDNA+vUAshlU8UIoQDQCthbDgtyrxAuRVES5rPafRmbR4KOqFBhSSiuU4wycsEA7C2f61fBBM6KUiyVeNeS+tmLPHh3goYp0W68/nSZgvC/PX1OH0DmzV375VxBPArokHMl6DDK6P7ihBd1eEbsMzDAqdpHMd9soacJXTeya85FnCI13TxLWhg0KzkFKOqmrwy/FekB//O53N1bqh5EQMAnBo8QClkMo5BQVSvKiBxh3kx5vwlZCd9RVwVyB0T+lkDBvEWXJOgGkFHxBnGxLJUy0J1DFXTzFDRDVPVdNRUISZRbZ2p6qq20cCLhsYYDdVza6qmd6OxTKstsAoX3sANMVUQrXAmZUWJ1cftWLdU3TZRiE5U4o4UA/KDODNFh0kzRhwCMs1WdSRjCDNENWxb9QxTsUzV1LCLcXC+BYQ0qDVHI61h28oyJXZammoz6wxdtXSieIZqm9jlWKqrm4qu66qjz8Aci7ihQhxV85jrLdWxDMW0VNNFdtNUbctpYMdV9dpplgaGekikgUx0k6epnuPVDbfBcGKHz5PA2grja0D24LaBXqZtKbqrmoQoNoGHUMJWmAozRahugv0aag604L/aQjTV1ln0NAhu9C0P9WGSpopYwngAqhbvKaEKXFFlKbF4dvR41BZQizBV6vQC15mcUchxQV1tMzuJByO4mwlqEEM1vEYpxCJgDDVtWaFN/KaiOgYOabTdzDFwIpsq0OUlwV4S7AMn2CJmDc3RA+/OcNbuD2drOZKhES9a5Qzbn++u1Nr9cETDKxo9Y81NROJwcXnGaAH+35XjcVAVyfVyPRZjirqMDQvm2BQN+8FCbag8JJD0ZR15WUc+9DryLIXaVK3VQu14qqeRf1+oNS10DfIBavVSUL8k2UuSfeAk+8+KdZpEtFg9ZjG1D3vicc+hy3OefXCzHjg8srRXz3b6ggPO8nQ6pnuSGubzAU1hn3P/cUejoDjlWtHTtV6t0LBt06olNUWSTabVL9XNhLaLIIvpb5xMmdPBVQIjTiY0gI6Q1gc5jQxn+VSLrJ3WEDiukXT4alrc0dMqTTIh7R4V9vfr4bn6SjHNsmCQUqUqYN4fp+HmfLmoQhOGGIAiFD/VdPxnIxzJguNaKUdBlM85l/gDPvbZk9BEbYGM8J+1MIVjU56Gj7W5Gk3Hgy1MNSbXjamG+YCt7LNhq/GcthbxINghlrUnNV/67lKAKFU+geRGre93yDi/VRBYzPv/dmobS8S0/v9mDleTOJjGWxx7kw9cBJ5j4d1Y+psVN6LDYJpWy7Y/+tS6CdNnLhJxrcfmoH+JO5xWfYmD7cV7BQd4pXMoSKJkJiVRW26uYmQpTIOyXEUxQW1ZjGPiGi4fHrSAuxY0KjZEiluXWmCDYJQrtOIipaEVCEG7Rr10GSEYVnGCR3CxNJRYGsrsHAHSQ2ZyahFZMIa+NYECxPnCihNJeXY0gswFyipIr3aQaU8SBxO7a2PyOjvMi0ZOMyDrbMZpcOhS1lgS1ji5gTbcJzZeQmYD15SbtPoa7QM+Y8sV159vTITDBLQmSEyyLI2TrC1r8Ayu27KuQWsWpFOKuId9yunu9eg94woH/sD6/sGNm+4gT3KH2J4Jf9Tgf+8QMfDdHjninU+MLL7QC8kLUNBtUuqrlGsql7OYk/JOQVqDfKVsyza4RqxoAhAHcuA1FQYQ52vgN0mTLE+HP8DiiRrIwtO0/VarOUdr4SkangFCcsUS/3HTlrESAiJMijClUgjCDM2eXINKN9g0sQmOJY4FLVEYoEe1ZCFCEcpifwskb6LXxWuNeG1VPP5Ea8ti6yC3HsFnaQ2fuIRHPtxPS9c6IwWPENG4AYyFT4JPjeu7OEWEiM6roKI7ePwiAQP+7crbSGvFd0s0t5ZnPSjPdtRnluhpzytP18lzq6g8/7woT5+Y+u8RQUqsjSBdzx5NZE9NZbOPvPT2CIhsXhuRmxc82rLJBqrodaUEWTjCdXCcRFFKwSLESrjK6qA/auXZ8qF20EJ800s8D3qRypAP9fVe0zMbXsJ7ueUFDbl0j6i6xYhMzVBdkFYgWoPnDX8KO4luqbbXrGoOUV2EhdnD4VBeWZrW/aRzLzVrZ5QEqXzHzBor8TzJ05s4z5b4MkrRQ9IkT7KqBMXcBYNkEXgS1l5o5mr4z8PKEZzER8bE5nr3sGyDy0bfcytYKZQlPgB3veWqFrGI/riwse4Pm7ieZCwXjyqU9+yAJ4fvaDnJs5JKwyIfS9WISt/BVyF9D4V5/6A1EZTMsKCgQb0FpQXUujEty4BV3SRax4miuYku8nnJYy7MU2g5xmLXzn+BWEu/wQi0gYUGUZ6lN2BwrUet16A4ZH+NTWVYJJMKoFlQSHxwqS3V9VaWXkOsZ1E+V9M8DPBHgzrKywrMwh+CIfiigqArKuBZp0M00CRDaYdRfAJi5V3+w0YM9BpQ+zgGEIifYUs9LRlQH38U5SFs/bJKjWl1nFJsdm7Oop3lHzK7agLt4vTi7TegyecHk9pH/Jfrp72eD2sn/rYpZ/WqhAcsq3uQJq/NOqexvbY1ISb8+7iNCd4PSBFEok4k3VFNwwoVTfWIq0uaoqu2K2mqY5kub+OXDj0SktiE9ekeoh3HZr2izQgcEygRqTA6k7drAoXJMFgfsRUumj1E+3aMp8mmqTiqZ2kkVIjqGcRUDNV0TQuwRCeuYiHkKIzUqCFTtQwNx3BVEORJRAWLCScyJUGkqYbraSaoatqGYyFMHNcCLXUwGNC6YWsOgJZpOAASDS1ip+w2MlnEY2jLttARGiFuAxq6iVTQsolp2wpK07HDNVzDZmO7oJalWzra5cGU2aruEtTd0Rzwo6ZqtltDsMa7ugHyHNWyQDAes1sGJ3IkJDIJDq5zTW1Nh4dFGNLwdJsPbDYQMT0Qh2Z4HpquuabHrDCIwo3gfuGaekIazIzGPQNqc8fcjmvfg9803QthfohhofNtlcCEga4msTzFBUEOAUs8cO+q810EDXDFsu9huGXf24a37HvbJsu+tzxz2feaJVwPGnHXmyuud3jM2hA34HvDBQ/qMIRiAKlrw0w4IEJAOEswNEQN+Ab5dNN0kUTzbEmQoOddo9HAIjACH0h4voEIxPKq53XCbLDdZc+bm553PIQwjoTvFYhrz4Rv04WHBb7THEU4GN1tGwK6HWuSCQjihmixaYKu0OU4ro76WUCP5hkYkobpuSvTo+krqaETEQA2mx4WaUQzbBvRkMorqeGYIqgEtJYZmHZ8rlENk2h6Y7VrmA6Cnuc6mMCWa6LZtmeA1w3LqKESs8hx0U/gQ9ZHLIn3hZsTA2nXDGESDoh5WWSEzeaFkOWMcLg3zHszAtvoD3gQy8LQMsHTigEErq0Ix/IoEtAt1tRkohRTLAR0RrM8iupdiqgIHFwjOTwI8LWRikpBVRXJYFrRb1kZX77YlpvOC3Z+8NPbb2QpmsK+A+yBws3fSz3KpxkUESiNdJhkSUXF6QDW9Dd8741FQuxNpCNR0rJYUlUVNxSfYw0UhZbOqu/zaRFSqHEZnUvHoLDA7PC6ucuoF3T4eu+EZkA+nGasUu6MRQV+jqqqab5pdV6q6ktVfamqL1X1paq+VFVeEe6sqq21GkcjXt4eLEbikmVXrQ/SsZQFaUkfZBM3CVuzifP2rfnqSy1gZAVSFXsE4JZ1+UFecab/JFZxaL8958rdVM3OL0WRmx0i0UJ+jI+fzC58/TD/X+u7GVoUSLS5nXmeDU3PsSzDetnQvGxoXjY0Lxualw3Ny4aGV4SHNzTdpAw39jTwaQpXmOYl3dll+C13O1UxpYzvH0vx9nyiBj/EuP2GB2aGl/4tNj1bsYuNz1O5xV3T2l5Ajv+Jb+WmDcemlV+fAO2woyF5T1rcrP3jXDf3fRuyFtd/ILC5ANyTDI1/7QrpT97gLb1pLD82tp4upAm0x0hZ2/IFUcROub5JyorCdKFp8t7jj7OanBJvdMGgO2MV/zMyqQ3jY7SDOx8nIs9CfImHZ/Jf8PWwqsJ326jbuJsFKSjLdX3UVnx7BcW8bKWh4NlSRcH1BB3jIphBdt+lI15Zim5QYxIUJe2leVCJCQbZgognWxeS7R2kE8rAMKhZv5DY6x27cA+Jb6IwLry1vIPrsC3pFtBIn312l9CDtkQ0JurPZ1lMDHLUNYiIz78kmpb0Ps0OJLMZ9i57of95VrheT4PPo5Q6lAz9Ya2A4AOq9Tyz0Ot1HZRcr0/Pt6rj1femVwApr6/229YzEdtqlfeSaxrtkF2U+tmn1xCfwzexfFfu5Zl47WCb+4K1FxZW1Hhdrw049q8ZDLqVoDIs8jS9yCdSe2uuU/azvD5CqK2RNl6+bBaS61EhrlXgJue0qibv6B9TWlZ824bdKt6l7Minx34X5ocPjJZ9idOIQhFoA8Bl7/GjI85eFTd8KC6ppFm0k03TtA7XMIASI+3QRbJsnHpAH6cVNonXLTYXzAGFKKPTDFbCCPRke08/TRt1GnfUfTti2M0d69beOf/u+4uFd/7RBTIbJ5LvcMNfy++71y+pICBecz9osf/b3t8dfPeyfU8AAA==";

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
