An implementation of a web interface for an Arduino Nano 33 IoT based on SSE

This web interface communicates with the Arduino using GET/HEAD/POST requests in one direction, and SSE in the other direction.

Uses https://github.com/adamvr/arduino-base64 for base64 decoding.

There is no documentation yet, but the least you need to do to get it to work is this:

- Edit the wifi_secrets.h to use your own WiFi SSID and password;
- Edit line 19 of webserver.ino to the IP adress you want your Arduino to have.

To start things up, point your web browser at the IP address you chose; the Arduino will serve up the interface.

Note that interface3.html (with a g-meter gauge) is included in webserver.ino as a base64-encoded gzip of said interface3.html; the interface*.html files have only been included so you can have a look at them.

Almost forgot: if you want to see something happen, put a LED on pin 12 8-).

If you feel like donating for this, you can do so here: http://ocrdu.nl/donations .