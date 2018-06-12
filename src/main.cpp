#include <Arduino.h>
#include <ESP8266WiFi.h>        // Include the Wi-Fi library

#define PIN_STATUS D2     // Status led on PCB
#define IR_LED D3         // IR led
#define PIN_DETECT1 D4    // Infrared receiver ( IR photo module TSOP1738, TSOP34838)
#define PIN_FEEDBACK_LED D1 // Green led to indicate goal is detected

// I should learn how to use environment variables for this stuff, but
// for now, you need to replace the ssid and password manually.
const char* ssid     = "<WIFI SSID>";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "<WIFI PASSWORD>";     // The password of the Wi-Fi network

/*
  Goals will be reported to url put together by the following constants. A
  PUT request will be sent to
    http://<host>:<port>/<path>/<team>
  Note that the slashes before and after path should not be included in the
  variables.
  If you need https, you need to use the secure client in notifyGoal instead.
*/
const char* host = "10.0.201.72";
const uint16_t port = 3000;
const char* path = "score";
const char* team = "blue";

// Minimum time in ms that the IR needs to be broken for the code to report
// is as a goal. This is to avoid goals being reported for flake variations.
const int minActiveTime = 2;

void blinkFeedback(int count) {
  for (int i = 0; i < count; i++)
  {
    digitalWrite(PIN_FEEDBACK_LED, HIGH);
    delay(100);
    digitalWrite(PIN_FEEDBACK_LED, LOW);
    delay(100);
  }
}

void notifyGoal() {
  digitalWrite(PIN_FEEDBACK_LED, HIGH);
  WiFiClient client;
  Serial.println("Registering goal");
  Serial.printf("\n[Connecting to %s ... ", host);
  if (client.connect(host, port))
  {
    Serial.println("connected]");
    client.print(
      String("PUT ") + "/" + path + "/" + team + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Connection: close\r\n" +
      "\r\n"
    );

    // Printing the response. Not necessary, but nice for debugging
    Serial.println("[Response:]");
    while (client.connected())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
    Serial.println("\n[Disconnected]");
  }
  else
  {
    Serial.println("connection failed!]");
    client.stop();
  }

  digitalWrite(PIN_FEEDBACK_LED, LOW);
  delay(100);
  blinkFeedback(3);

}

void setup() {
  Serial.begin(28800);
  delay(1000);
  Serial.println('\n');

  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  //pinMode(IR_LED,  OUTPUT);
  pinMode(PIN_STATUS, OUTPUT);
  pinMode(PIN_DETECT1, INPUT);
  pinMode(PIN_FEEDBACK_LED, OUTPUT);
  digitalWrite(PIN_FEEDBACK_LED, LOW);
  tone(IR_LED, 38000U);
  Serial.print("Setup done");
}

unsigned long highStart = 0;
bool hasNotified = true;

void loop() {
  if (digitalRead(PIN_DETECT1) == HIGH) {
    if (highStart && !hasNotified) {
      unsigned long delta = millis() - highStart;
      if (delta > minActiveTime) {
        hasNotified = true;
        notifyGoal();
      }
    } else {
      highStart = millis();
    }
    digitalWrite(PIN_STATUS, LOW);
  } else {
    digitalWrite(PIN_STATUS, HIGH);
    highStart = 0;
    hasNotified = false;
  }
}
