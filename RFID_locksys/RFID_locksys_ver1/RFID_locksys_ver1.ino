#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN D4
#define RST_PIN D3
#define RELAY D0 //connect the relay to number 3 pin
#define BUZZER D1 // connect the buzzer to 2 pin
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
void setup()
{
Serial.begin(9600); // Initiate a serial communication
SPI.begin(); // Initiate SPI bus
mfrc522.PCD_Init(); // Initiate MFRC522
pinMode(RELAY, OUTPUT);
pinMode(BUZZER, OUTPUT);
noTone(BUZZER);
digitalWrite(RELAY, LOW);
Serial.println("Put your card to the reader for scanning …");
Serial.println();

}
void loop()
{
// Look for new cards
if ( ! mfrc522.PICC_IsNewCardPresent())
{
return;
}
// Select one of the cards
if ( ! mfrc522.PICC_ReadCardSerial())
{
return;
}
//Show UID on serial monitor
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
Serial.print("Message : ");
CardUID.toUpperCase();
if (CardUID.substring(1) == "A1 4A D2 83" || CardUID.substring(1) == "D9 5B CA D0") // enter your own card number after copying it from serial monitor
{
Serial.println("Authorized access");
Serial.println();
delay(500);
digitalWrite(RELAY, HIGH);
delay(ACCESS_DELAY);
digitalWrite(RELAY, LOW);

}

else {
Serial.println(" Access denied");
tone(BUZZER, 300);
delay(DENIED_DELAY);
noTone(BUZZER);
}
}
