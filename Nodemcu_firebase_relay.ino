//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "config.h"

/* You must create a file <config.h> which will contain following sensitive data.
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define FIREBASE_HOST "" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH ""
*/

#define TRIGGER 2

//Define Firebase Data object
FirebaseData firebaseData;

void setup()
{
  Serial.begin(115200);

  pinMode(TRIGGER, OUTPUT); // Sets the trigPin as an Output

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

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  Serial.println("Checking database states");
  checkDatabaseStates();
  delay(1000);
}

void checkDatabaseStates() {
  DynamicJsonDocument jsonDoc(1024); // create json buffer
  if (Firebase.getJSON(firebaseData, "/iot/states")) // database path
  {
    /* state is a child of nodemcu object in database. Here we initialize routine */
    /// https://arduinojson.org/v6/api/jsonobject/begin_end/
    String json = firebaseData.jsonData(); // read data as string

    deserializeJson(jsonDoc, json); // deserialize as json
    JsonObject root = jsonDoc.as<JsonObject>(); // create root object to operate with

    Serial.println("PASSED");

    // Iterate through root object (json)
    for (JsonPair kv : root) {
      const char* key = kv.key().c_str();
      Serial.println(key);

      if (root[key] == false) {
        Serial.print(" .State: false. Switching off.");
        digitalWrite(TRIGGER, LOW);
      } else {
        Serial.print(" .State: true. Switching on.");
        digitalWrite(TRIGGER, HIGH);
      }

      Serial.println();
    }
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

}
