#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

// Include Firebase helper libraries
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Wi-Fi and Firebase configuration
#define WIFI_SSID "Xiomi"
#define WIFI_PASSWORD "ddddddddd"
#define API_KEY "AIzaSyDOsrJBUY4BbpWhnlPX0yStTV1EgmFDICA"
#define DATABASE_URL "https://multi-plug-default-rtdb.firebaseio.com/"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup()
{
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Firebase configuration
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Firebase authentication
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Firebase signup successful");
    signupOK = true;
  }
  else
  {
    Serial.printf("Firebase signup error: %s\n", config.signer.signupError.message.c_str());
  }

  // Set token status callback
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // Read data from './switch' in Firebase
    if (Firebase.RTDB.getString(&fbdo, "/switch"))
    {
      String switchState = fbdo.stringData();
      Serial.print("Switch state: ");
      Serial.println(switchState);
    }
    else
    {
      Serial.println("Failed to read data from /switch");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  }
}
