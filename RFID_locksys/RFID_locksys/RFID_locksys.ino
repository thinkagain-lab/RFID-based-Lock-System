#include <ESP8266WiFi.h>
#include <Wire.h>
#include <MFRC522.h>
#include <SPI.h>
#include <WiFiClientSecure.h>

#define SS_PIN D4  // Define the SS_PIN for MFRC522
#define RST_PIN D3  // Define the RST_PIN for MFRC522

const char *ssid = "THINK AGAIN LAB_EXT";
const char *password = "Tal@12345";

MFRC522 mfrc522(SS_PIN, RST_PIN);

void sendDataToGoogleScript(String uid);
bool isAuthorized(String uid);
String getColumnBValue(String uid);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // Set up the MFRC522 RFID reader
  mfrc522.PCD_Init();
}

void loop() {
  // Look for new RFID cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String uid = getCardUID(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println("Card UID: " + uid);
    if (isAuthorized(uid)) {
      String columnBValue = getColumnBValue(uid);
      Serial.println("Access granted. Value in column B: " + String(columnBValue));
      sendDataToGoogleScript(uid);
      // You can perform the necessary actions for access granted here
    } else {
      Serial.println("Access denied. UID not authorized.");
      // You can perform the necessary actions for access denied here
    }
    delay(500);  // Add a delay to avoid multiple reads for the same card
  }
}

String getCardUID(byte* uidBytes, byte size) {
String CardUID= "";
//byte letter;
for (byte i = 0; i < mfrc522.uid.size; i++)
{
Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
Serial.print(mfrc522.uid.uidByte[i], HEX);
CardUID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
CardUID.concat(String(mfrc522.uid.uidByte[i], HEX));
}
Serial.println();
CardUID.toUpperCase();
  return CardUID;
}
void sendDataToGoogleScript(String uid) {
  String name = getColumnBValue(uid);
  if (name != "") {
    String dateTime = getDateTime();
    String url = "https://script.google.com/macros/s/AKfycbzHP3NZ9GDfxpOoAx7Wm3r4tMyYvVuBwBpUz_ZqPCI9ytOZltrsNqIiqp41lmI2TOjn/exec" + name + "&uid=" + uid + "&datetime=" + dateTime;

    WiFiClient client;

    if (client.connect("script.google.com", 443)) { // Replace with your Google Script host and port
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: script.google.com\r\n" +
                   "Connection: close\r\n\r\n");
      delay(10);

      // Read response
      while (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }

      Serial.println();
      Serial.println("Request sent");
      client.stop();
    } else {
      Serial.println("Unable to connect to Google Script.");
    }
  } else {
    Serial.println("Name not found for UID: " + uid);
  }
}
String getDateTime() {
  String dateTime;
  // Implement your logic to get the current date and time
  // For simplicity, let's use the millis() function as an example
  unsigned long currentMillis = millis();
  dateTime = String(currentMillis);
  return dateTime;
}

bool isAuthorized(String uid) {
  String url = "https://script.google.com/macros/s/AKfycbyFBpUwOuPCapA1it8VhjNN1JVe1IeXfwU7xL9kcynpjyhv0WYGpi00dZPz4Zs8WxdcgA/exec" + uid;  // Replace with your Google Script Web App URL

  WiFiClient client;

  if (client.connect("script.google.com", 443)) { // Replace with your Google Script host and port
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: script.google.com\r\n" +
                 "Connection: close\r\n\r\n");
    delay(10);

    // Read response
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

    Serial.println();
    Serial.println("Request sent");
    client.stop();
  } else {
    Serial.println("Unable to connect to Google Script.");
  }

  return false;  // Return false by default
}

String getColumnBValue(String uid) {
  String url = "http://your_google_script_web_app_url/getColumnBValue?uid=" + uid;  // Replace with your Google Script Web App URL

  WiFiClient client;

  if (client.connect("script.google.com", 443)) { // Replace with your Google Script host and port
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: script.google.com\r\n" +
                 "Connection: close\r\n\r\n");
    delay(10);

    // Read response
    String response = "";
    while (client.available()) {
      response += client.readStringUntil('\r');
    }

    Serial.println();
    Serial.println("Request sent");
    client.stop();

    return response;
  } else {
    Serial.println("Unable to connect to Google Script.");
    return "";  // Return an empty string if there's an issue
  }
}
