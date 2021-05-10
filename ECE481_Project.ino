#include <SPI.h>
#include <WiFi101.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status


#define SENSORPIN 11
#define PIN_LED_12 12

// Include the servo library:
#include <Servo.h>
// Create a new servo object:
Servo myservo;
// Define the servo pin:
#define servoPin 9
// Create a variable to store the servo position:
int angle = 0;


// variables will change:
int sensorState = 0, lastState=0, activationCount=0;
bool isFull = false;
bool compressed = false;

void setup() {

  Serial.begin(9600);

  //WIFI SETUP
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) 
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
    }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWiFiData();


  // initialize the LED pin as an output:
  pinMode(PIN_LED_13, OUTPUT); 
  pinMode(PIN_LED_12, OUTPUT);           
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);     
  digitalWrite(SENSORPIN, HIGH); // turn on the pullup

  // Attach the Servo variable to a pin:
  myservo.attach(servoPin);

  activationCount = 0;
  isFull = false;
  compressed = false;


}

void loop(){
  
  while(!isFull){
    // read the state of the pushbutton value:
    sensorState = digitalRead(SENSORPIN);
    // turn LED off:
    digitalWrite(PIN_LED_13, LOW);
    digitalWrite(PIN_LED_12, HIGH);

    // check if the sensor beam is broken
    // if it is, the sensorState is LOW:
    if (sensorState == LOW) { 
      activationCount += 1;
      // turn LED on:
      digitalWrite(PIN_LED_13, HIGH);
      digitalWrite(PIN_LED_12, LOW);  

      delay(1000);
      if(sensorState == LOW && !compressed){
        myservo.write(90);
        delay(1000);
        myservo.write(0);
        delay(1000);
        compressed == true;
        delay(1000);
        if(sensorState  == LOW){
          isFull = true;
        }
        else{
          compressed = false;
        }
      }
    }
    break;
  }

  while(isFull){

    digitalWrite(PIN_LED_13, HIGH);
    digitalWrite(PIN_LED_12, LOW);

    // read the state of the pushbutton value:
    sensorState = digitalRead(SENSORPIN);

    if(sensorState == HIGH){
      compressed == false;
      isFull == false;   
    }
    break;
  }

  printCurrentNet();
}

void printWiFiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);
  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
