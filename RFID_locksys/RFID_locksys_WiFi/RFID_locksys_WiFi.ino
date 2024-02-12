#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#define RST_PIN     D3     // Configurable, see typical pin layout above
#define SS_PIN      D4     // Configurable, see typical pin layout above
#define RELAY_PIN   D0     // Pin connected to the relay

#define WIFI_SSID "THINK AGAIN LAB_EXT"
#define WIFI_PASSWORD "Tal@12345"

#define GOOGLE_SHEET_ID "1c3oEHJ8Joe6YCKIdzu3GOpNiESiDPDKa9RR90Ts1ZDs"
#define ACCESS_TOKEN "AIzaSyCZa7nuUoSW-rFrFCIFc6uFwWPR98xSANM"

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
WiFiClientSecure client;

const char* host = "sheets.googleapis.com";
const int httpsPort = 443;

void setup() {
  Serial.begin(9600); // Initialize serial communications
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  pinMode(RELAY_PIN, OUTPUT); // Set relay pin as output

  Serial.println("Scan a RFID card to check access...");
  connectToWiFi();
}

void loop() {
  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String cardUID = getCardUID(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println("Card UID: " + cardUID);
    if (checkAccess(cardUID)) {
      Serial.println("Access granted!");
      digitalWrite(RELAY_PIN, HIGH); // Turn on the relay
      delay(2000); // Keep the relay on for 2 seconds
      digitalWrite(RELAY_PIN, LOW); // Turn off the relay
    } else {
      Serial.println("Access denied!");
    }
    delay(1000);
  }
}

String getCardUID(byte* uidBytes, byte size) {
  Serial.print("UID tag :");
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

bool checkAccess(String cardUID) {
  String url = "/v4/spreadsheets/" + String(GOOGLE_SHEET_ID) + "/values/A2:A100?key=" + String(ACCESS_TOKEN);
  if (client.connect(host, httpsPort)) {
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Connection: close");
    client.println();

    String response = "";
    while (client.connected() || client.available()) {
      if (client.available()) {
        char c = client.read();
        response += c;
      }
    }
    client.stop();

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);
    JsonArray values = doc["values"];
    for (JsonVariant value : values) {
      if (value.as<String>() == cardUID) {
        return true;
      }
    }
  }
  return false;
}

void connectToWiFi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
