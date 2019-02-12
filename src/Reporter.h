/*
  Reporter.h
  Based on:
  SlackWebhook.h - Library for connecting Adafruit Huzzah to Slack Webhooks
  Created by Scott Linenberger, August 19, 2016
*/
#ifndef REPORTER_H
#define REPORTER_H

#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

class Reporter
{
  const char *host;
  String url;
  const char *fingerprint;

public:
  Reporter(const char *, String, const char *);
  boolean postMessageToServer(String);
};

#endif