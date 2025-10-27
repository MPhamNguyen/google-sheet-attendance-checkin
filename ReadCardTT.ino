#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <WiFi.h>
#include <HTTPClient.h>

// Learn more about using SPI/I2C or check the pin assigment for your board: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
MFRC522DriverPinSimple ss_pin(5);

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
//MFRC522DriverI2C driver{};     // Create I2C driver
MFRC522 mfrc522{driver};         // Create MFRC522 instance

MFRC522::MIFARE_Key key;

byte firstNameAddress = 1;
// byte firstNameData[17] = {"Minh"};
byte lastNameAddress = 2;
// byte lastNameData[17] = {"Pham-Nguyen"};
byte classAddress = 4;
// byte classData[17] = {"Marshal"};
byte pointsAddress = 5;
// byte pointsData[17] = {"1000"};
byte bufferblocksize = 18;
byte blockDataRead[18];

int sda_pin = 16;
int scl_pin = 17;
LiquidCrystal_I2C lcd(0x27,20,4);

int buzzer = 15;

//Network Info - Use the connected laptop to turn into a hotspot and broadcast it
const char* ssid = "BeachThetaTau";
const char* password = "XiEpsilon";

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  while (!Serial);       // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  mfrc522.PCD_Init();    // Init MFRC522 board.
  // Prepare key - all keys are set to FFFFFFFFFFFF at chip delivery from the factory.
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Wire.setPins(sda_pin, scl_pin);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  pinMode(buzzer,OUTPUT);


  WiFi.begin(ssid, password); //Begin wifi connection; Make sure it is 2.4GHz 

  //Visual to check for connection
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  lcd.clear();
  lcd.print("Connected!");
  tone(buzzer,3000,700);
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(500);
    return;
  }

  lcd.clear();

  // FIRST NAME
  mfrc522.PCD_Authenticate(0x60, firstNameAddress, &key, &(mfrc522.uid)); // authenticate
  // mfrc522.MIFARE_Write(firstNameAddress, firstNameData, 16); // write

  bufferblocksize = sizeof(blockDataRead); 
  mfrc522.MIFARE_Read(firstNameAddress, blockDataRead, &bufferblocksize); // read into blockDataRead
  Serial.print("First Name: ");
  byte i = 0;
  while(blockDataRead[i]) {
    Serial.print((char)blockDataRead[i]);
    i++;
  }
  Serial.println();
  String name = String((char*)blockDataRead);
  String firstName = String((char*)blockDataRead);

  // LAST NAME
  mfrc522.PCD_Authenticate(0x60, lastNameAddress, &key, &(mfrc522.uid)); // authenticate
  // mfrc522.MIFARE_Write(lastNameAddress, lastNameData, 16); // write

  bufferblocksize = sizeof(blockDataRead);
  mfrc522.MIFARE_Read(lastNameAddress, blockDataRead, &bufferblocksize); // read into blockDataRead
  Serial.print("Last Name: ");
  i = 0;
  while(blockDataRead[i]) {
    Serial.print((char)blockDataRead[i]);
    i++;
  }
  Serial.println();
  name = name + " " + String((char*)blockDataRead);
  String lastName = String((char*)blockDataRead);

  lcd.setCursor(0,0);
  lcd.print(name);
  
  // class
  mfrc522.PCD_Authenticate(0x60, classAddress, &key, &(mfrc522.uid)); // authenticate
  // mfrc522.MIFARE_Write(classAddress, classData, 16); // write

  bufferblocksize = sizeof(blockDataRead); 
  mfrc522.MIFARE_Read(classAddress, blockDataRead, &bufferblocksize); // read into blockDataRead
  Serial.print("class: ");
  i = 0;
  while(blockDataRead[i]) {
    Serial.print((char)blockDataRead[i]);
    i++;
  }
  Serial.println();
  String info = String((char*)blockDataRead);

  // points
  mfrc522.PCD_Authenticate(0x60, pointsAddress, &key, &(mfrc522.uid)); // authenticate
  // mfrc522.MIFARE_Write(pointsAddress, pointsData, 16); // write

  bufferblocksize = sizeof(blockDataRead);
  mfrc522.MIFARE_Read(pointsAddress, blockDataRead, &bufferblocksize); // read into blockDataRead
  Serial.print("Points: ");
  i = 0;
  while(blockDataRead[i]) {
    Serial.print((char)blockDataRead[i]);
    i++;
  }
  Serial.println();
  info = info + " " + String((char*)blockDataRead);

  lcd.setCursor(0,1);
  lcd.print(info);

  // Halt communication with the card
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  //String manipulation for creating links based on name associated to RFID cards
  String baselink = "https://script.google.com/macros/s/AKfycbyEU5bCIcsjTHByJd-oll9IreF2NwD9X_1xAHgF-T0npipCyjGD2TC69gGUEHyPcs5a/exec?name=";
  String link = baselink + firstName + "%20" + lastName;

  //HTTP request handler
  HTTPClient http;
  http.begin(link);
  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.printf("Brother %s has been successfully checked in!", name.c_str());
  } else {
    Serial.printf("Error code: %d\n", httpCode);
  }

  http.end(); //Disconnect request

  tone(buzzer,3000,700);

  delay(2000);  // Delay for readability
}