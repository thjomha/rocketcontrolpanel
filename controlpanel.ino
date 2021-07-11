#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
/*
 * Model Rocket Launch Pad v0.1
 * by Taha Jomha
 * 
 */

#define SS_PIN 10
#define RST_PIN 5
// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

//CONSTANTS
const int blueLED = 7;
const int blueButton = 3;
const int redLED = 8;
const int redButton = 4;
const int relay = 6;
const int buzzerPin = 9;

boolean launching;
byte readCard[4];
String UserTag[] = {"5A8A56AD", "CF15849"};//replace with tag ID's
String UserName[] = {"Welcome USER1", "Welcome USER2"};
int numOfUsers = 2;
String tagID = "";
int user = -1;
boolean proceed = true;

void setup() {
  Serial.begin(9600);
  SPI.begin(); // SPI bus
  mfrc522.PCD_Init(); // MFRC522

  //setup pins
  pinMode(redButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(relay, LOW);
  digitalWrite(blueLED, HIGH);
  digitalWrite(redLED, LOW);
  launching = false;
  
  //the blue button is associated with an interrupt, it can be pressed at any time, the blueButtonActions method is envoked regardless of where the program is at.
  attachInterrupt(digitalPinToInterrupt(blueButton), blueButtonActions, LOW);
  
  u8g2.begin();
  u8g2.clearBuffer();
  displaySmall("MISSION CONTROL","Please log in...");
}

void loop() {
    /*
     * This loop waits for a RFID tag to be scanned, if it matches
     * a known user, it will proceed
     */
    
    while (getID()) 
    {
      //once a known tag is scanned, figure out which user it is
      //user is was declared as -1, so if nothing matches it will remain -1 which is not logged in
      for(int i =0; i<numOfUsers; i++){
        if( tagID == UserTag[i] ){
          user = i;
          break;
        }
      }
      if (user>-1) 
      {
        buzzerSong();
        
        u8g2.clearBuffer();          // clear the internal memory
        u8g2.setFont(u8g2_font_t0_12_te);
        u8g2.drawStr(8,12,UserName[user].c_str());
        u8g2.drawStr(3,25,"Ready to launch...");  // write something to the internal memory
        u8g2.sendBuffer();
        proceed = true;
        loggedIn();
      }
      else
      {
        //no user matches the scanned RFID, goes back to scanning mode.
        user = -1;
        buzzer(4000,1500);
        displayBig("Access Denied");  
        delay(1000);
        displaySmall("MISSION CONTROL","Please log in...");
      }  

    }

    
 }
  
 

//Read new tag if available
boolean getID() 
{
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
  return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
  return false;
  }
  tagID = "";
  for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
  //readCard[i] = mfrc522.uid.uidByte[i];
  tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  return true;
}
void blueButtonActions(){
    if(launching){
      launching = false;
    }
    else{
      proceed = false;
      //user = -1;
    }
}
void displayBig(const char* text){
    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFont(u8g2_font_logisoso16_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.drawStr(8,29,text);  // write something to the internal memory
    u8g2.sendBuffer();         // transfer internal memory to the display
}
void displaySmall(const char* line1, const char* line2){
    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFont(u8g2_font_t0_12_te);
    u8g2.drawStr(3,12,line1);  // write something to the internal memory
    u8g2.drawStr(3,25,line2);
    u8g2.sendBuffer();         // transfer internal memory to the display
}
  
void loggedIn(){
  while(user>-1){//If user is >-1 then logged in
      digitalWrite(redLED, HIGH);
      if(digitalRead(redButton)==LOW){
       //launch sequence
       launching = true;
       buzzerSong();
      
       displayBig("READY...");
       delay(500);

         for(int i = 10; i>=0 ;i--){
          displayCount(i);
          delay(1000);
          //every 1second check if launching has changed to false.  Launching changes from true to false when the blue button (interrupt pin) is pressed.
          if(!launching){
            displayBig("ABORTED");
            delay(1500);
            displaySmall("Armed.","ready to launch");
            break;
           }
       
         }   
       }
       if(!proceed){
            digitalWrite(redLED,LOW);
            //user = -1;
           }
  }
  buzzerSong();
  displaySmall("Mission Control","Please log in...");
  tagID = "xxxxxx";
  //return to log in
  
}
void displayCount(int count){
       
   switch (count) {
      case 10:
        displayBig("10   10   10");
        buzzer(5000,250);
        break;
      case 9:
        displayBig("9   9   9");
        buzzer(5000,250);
        break;
      case 8:
        displayBig("8   8   8");
        buzzer(5000,250);
        break;
      case 7:
        displayBig("7   7   7");
        buzzer(5000,250);
        break;
      case 6:
        displayBig("6   6   6");
        buzzer(5000,250);
        break;
      case 5:
        displayBig("5   5   5");
        buzzer(5000,250);
        break;
      case 4:
        displayBig("4   4   4");
        buzzer(5000,250);
        break;
      case 3:
        displayBig("3   3   3");
        buzzer(5000,250);
        break;
      case 2:
        displayBig("2   2   2");
        buzzer(5000,250);
        break;
      case 1:
        displayBig("1   1   1");
        buzzer(5000,250);
        break;
      case 0:
        displayBig("BLAST OFF!!!");
        digitalWrite(relay,HIGH);
        buzzer(5000,2500);
        delay(5000);
        digitalWrite(relay,LOW);
        launching = false;
        break;
      default:
        // statements
        break;
  }
}
void buzzer(int freq, int duration){
  
  tone(buzzerPin, freq, duration);
}
void buzzerSong(){
  buzzer(1000,100);
        delay(50);
        buzzer(1200,100);
        delay(50);
        buzzer(800,100);
        delay(50);
        buzzer(1000,100);
}
