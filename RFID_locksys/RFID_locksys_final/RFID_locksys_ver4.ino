#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
//-----------------------------------------
#define RST_PIN  D3
#define SS_PIN   D4
#define RELAY D0 //connect the relay to number DO pin and connect wires to NO connection
#define BUZZER D1 // connect the buzzer to D! pin to positive and synchronize the gnd
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000
//-----------------------------------------
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
//-----------------------------------------
/* Be aware of Sector Trailer Blocks */
int blockNum = 2;
/* Create another array to read data from Block */
/* Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[18];
//-----------------------------------------
String response;
String card_holder_name;
String card_holder_name_auth;
const String sheet_url_send = "https://script.google.com/macros/s/AKfycbym2CoJVgJ1kB8195Gd2F3yUFmPgMQMxLRLMtZza47oldXJ7OdHv7YVoEvUu3weqkjr/exec?name=";//add ?name= at last of the url
const String sheet_url_auth = "https://script.google.com/macros/s/AKfycbzJcZn5bFNTLIueeTTVihalRiii8fpfu4LFRebg_IzTzpOIsPoZgM9gyWcibozYEHqERA/exec?cardId=";//add ?cardId= at last of the url
//-----------------------------------------

#define WIFI_SSID "THINK AGAIN LAB_EXT"
#define WIFI_PASSWORD "Tal@12345"
//-----------------------------------------




/****************************************************************************************************
   setup() function
 ****************************************************************************************************/
void setup()
{
  //--------------------------------------------------
  /* Initialize serial communications with the PC */
  Serial.begin(9600);
  //Serial.setDebugOutput(true);
  //--------------------------------------------------
  //WiFi Connectivity
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //--------------------------------------------------

  //--------------------------------------------------
  /* Initialize SPI bus */
  SPI.begin();
  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  digitalWrite(RELAY, LOW);
  Serial.println("Put your card to the reader for scanning …");
  Serial.println();
  //--------------------------------------------------
}




/****************************************************************************************************
   loop() function
 ****************************************************************************************************/
void loop()
{
  //--------------------------------------------------
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  /* Select one of the cards */
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  /* Read data from the same block */
  //--------------------------------------------------
  Serial.println();
  //Serial.println(F("Reading last data from RFID..."));
  /* If you want to print the full memory dump, uncomment the next line */
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  //--------------------------------------------------
  String CardUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    CardUID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    CardUID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  CardUID.toUpperCase(); // Shows the card hex id
  tone(BUZZER, 300);
  delay(100);
  noTone(BUZZER);
  ReadDataFromBlock(blockNum, readBlockData); //read the name on the block
  checkifAuthorised();
  //delay(1000);
  if (response == "true") {
    Serial.println("Data is present in Google Sheets.");
    delay(500);
    digitalWrite(RELAY, HIGH);// solenoid open
    delay(ACCESS_DELAY);
    digitalWrite(RELAY, LOW);
    sendDataToSheet();
    tone(BUZZER, 300); // buzzer beep to inform that attendance registered
    delay(100);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 300);
    delay(100);
    noTone(BUZZER);
  }
  if (response == "false") {
    Serial.println("Data is not present in Google Sheets.");
    Serial.println("Unauthorised access: Not Registered card");
    tone(BUZZER, 300); //unauthorised then long buzz
    delay(DENIED_DELAY);
    noTone(BUZZER);
  }
}








//Sending Data to the Google Sheet function
void sendDataToSheet() {
  if (WiFi.status() == WL_CONNECTED) {
    //-------------------------------------------------------------------------------
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    //-------------------------------------------------------------------------------
    client->setInsecure();
    //-----------------------------------------------------------------
    card_holder_name = sheet_url_send + String((char*)readBlockData);
    card_holder_name.trim();
    Serial.println(card_holder_name);
    //-----------------------------------------------------------------
    HTTPClient https;
    Serial.print(F("[HTTPS] begin...\n"));
    //-----------------------------------------------------------------
    https.end();
    if (https.begin(*client, (String)card_holder_name)) {
      //-----------------------------------------------------------------
      https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      // HTTP
      Serial.print(F("[HTTPS] GET...\n"));
      // start connection and send HTTP header
      int httpCode = https.GET();
      //-----------------------------------------------------------------
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
      }
      //-----------------------------------------------------------------
      else
      {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      //-----------------------------------------------------------------
      https.end();
      delay(1000);
    }

    else {
      Serial.printf("[HTTPS} Unable to connect\n");
    }

  }
}
void checkifAuthorised() {
  Serial.println("Authorisation in process");
  if (WiFi.status() == WL_CONNECTED) {
    //-------------------------------------------------------------------------------
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    //-------------------------------------------------------------------------------
    client->setInsecure();
    //-----------------------------------------------------------------
    card_holder_name_auth = sheet_url_auth + String((char*)readBlockData);
    card_holder_name_auth.trim();
    Serial.println(card_holder_name_auth);
    //-----------------------------------------------------------------
    HTTPClient https;
    Serial.print(F("[HTTPS] begin...\n"));
    //-----------------------------------------------------------------

    if (https.begin(*client, (String)card_holder_name_auth)) {
      //-----------------------------------------------------------------
      https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      // HTTP
      Serial.print(F("[HTTPS] GET...\n"));
      // start connection and send HTTP header
      int httpCode = https.GET();
      //-----------------------------------------------------------------

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        response = https.getString();
        response.trim();
        Serial.println(response);
        // Check if the response contains 'true' (data present in Google Sheets)

      }
      //-----------------------------------------------------------------
      else
      {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      //-----------------------------------------------------------------
      https.end();
      //delay(1000);
    }
  }
}
/****************************************************************************************************
   ReadDataFromBlock() function
 ****************************************************************************************************/
void ReadDataFromBlock(int blockNum, byte readBlockData[])
{
  //----------------------------------------------------------------------------
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  //----------------------------------------------------------------------------
  /* Authenticating the desired data block for Read access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  //----------------------------------------------------------------------------s
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Read: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //----------------------------------------------------------------------------
  else {
    Serial.println("Read success");
  }
  //----------------------------------------------------------------------------
  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //----------------------------------------------------------------------------
  else {
    Serial.println("Block was read successfully");
  }
  //----------------------------------------------------------------------------
}
