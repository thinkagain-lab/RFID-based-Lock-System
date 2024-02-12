#include <SPI.h>
#include <TFT_eSPI.h>
#include <MFRC522.h>
#include <WiFi.h>
 #include <HTTPClient.h>



#define SS_PIN 21 //D21
#define RST_PIN 5 //D5
#define buz_pin 13 //D13

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

#define TFT_GREY 0xBDF7

const char * ssid = "THINK AGAIN LAB";
const char * password = "Tal@12345";

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;        


int blockNum = 2;  

byte bufferLen = 18;
byte readBlockData[18];
String url;
const String data1 = "https://script.google.com/macros/s/AKfycbwfk4j3VtrmEeb0bzCAhcE3nl25UlEzJJWdMJC5OETu56gDtVPp9JXotp2HhtR-_BZ7mQ/exec?name=";
MFRC522::StatusCode status;


void setup() 
{
  /* Initialize serial communications with the PC */
  pinMode(buz_pin,OUTPUT);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_GREY);
  tft.setTextColor(TFT_GREEN, TFT_GREY); 

  Serial.begin(115200);
  /* Initialize SPI bus */
  SPI.begin();
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

Serial.println("Started");
Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

Serial.println("Ready to go");

  Serial.println("Scan the card");
}

void loop()
{
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  digitalWrite(buz_pin, LOW);
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  
  /* Select one of the cards */
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  Serial.print("\n");
  Serial.println("*Card Detected*");
  /* Print UID of the Card */
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print("\n");
  /* Print type of card (for example, MIFARE 1K) */
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
         
   /* Call 'WriteDataToBlock' function, which will write data to the block */
   Serial.print("\n");
   Serial.println("Writing to Data Block...");
   //WriteDataToBlock(blockNum, blockData);
   
   /* Read data from the same block */
   Serial.print("\n");
   Serial.println("Reading from Data Block...");
   ReadDataFromBlock(blockNum, readBlockData);
  
   
   /* Print the data read from block */
   Serial.print("\n");
   Serial.print("Data in Block:");
   Serial.print(blockNum);
        tone (buz_pin, 1000,500);
        delay(1000);
        noTone (buz_pin);
        delay(1000);

   Serial.print("\n");

   
   //HTTPClient http;
    url = data1 + String((char*)readBlockData);
    url.trim();
    Serial.println(url);
    HTTPClient http;
  
    http.begin(url); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
  
    if (httpCode > 0) { //Check for the returning code
  
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
        tone (buz_pin, 10,500);
        delay(1000);
        noTone (buz_pin);
        delay(1000);
      }
  
    else {
      Serial.println("Error on HTTP request");
      
    }
  
    http.end(); //Free the resources
  
  
  delay(1000);

    
}


////reading block data//////
void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{
  /* Authenticating the desired data block for Read access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK)
  {
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Block was read successfully");  
  }
}
