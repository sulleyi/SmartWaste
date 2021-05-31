#include <ArduinoJson.h>
#include <SPI.h>
#include <WiFi101.h>
#include <RBD_Timer.h>
#include <RBD_Button.h>
#include <Scheduler.h>
#include <ArduinoHttpClient.h>

#include "arduino_secrets.h"
//enter sensitive data in the arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)


// AWS API INFO
char apiKey[] = SECRET_APIKEY;
char HOST_NAME[] = "cqdup0la3c.execute-api.us-east-2.amazonaws.com";
String PATH_NAME = "/Phase3Test/";
int HTTP_PORT = 443;
String HTTP_METHOD = "POST";
String queryString = "?requestId=300&countActuations=9&state=Full";

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiSSLClient client;
//HttpClient client = HttpClient(wifi, server, port);

int status = WL_IDLE_STATUS;


// Local Sensing

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
int requestId;
int sensorState = 0, lastState = 0, activationCount = 0;
bool isFull = false;
bool compressed = false;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8, 7, 4, 2);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();
}

void loop() {
  pollState();
  delay(5000);

  String requestBody = postBody(requestId, activationCount, isFull);


  // connect to web server
  if (client.connect(HOST_NAME, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    
    client.println(F("POST /Phase3Test HTTP/1.1"));
    client.println(F("Content-Type: application/json"));
    client.println("Content-Length: " + String(requestBody.length()));
    client.println(F("Host: cqdup0la3c.execute-api.us-east-2.amazonaws.com"));
    client.println(F("x-api-key: UXEUtFmYBz1Eb3dNF8skj3iQWkdeOYlS2uYLlqnl"));
    client.println(); // end HTTP header
    // send HTTP body
    client.println(requestBody);

    Serial.println(requestBody);

    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {// if not connected:
    Serial.println("connection failed");
  }

  while(true){
    //continues forever
    }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


// Create JSON POST Body
String postBody(int requestId, int countActuations, bool state) {

  Serial.println("Generating JSON Document");
  //TODO: EDIT JSON REQUEST STRING
  const int capacity = JSON_OBJECT_SIZE(3);

  StaticJsonDocument<capacity> doc;

  doc["requestId"].set(requestId);
  doc["countActuations"].set(countActuations);
  doc["state"].set(state);

  String json;

  serializeJson(doc, json);
  return json;
}

/*

  void postRequest(String json){
  Serial.println("making POST request");
  String contentType = "application/json";
  String postData = json;

  Serial.print("POST Data: ");
  Serial.println(json);

  client.beginRequest();
  client.post(path);
  client.sendHeader("Content-Type", contentType);
  client.sendHeader("Content-Length", postData.length());
  client.sendHeader("x-api-key", apiKey);
  client.beginBody();
  client.print(postData);
  client.endRequest();


  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  Serial.println("Wait five seconds");
  delay(5000);
  }
*/

void pollState() {
  while (!isFull) {
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
      if (sensorState == LOW && !compressed) {
        myservo.write(90);
        delay(1000);
        myservo.write(0);
        delay(1000);
        compressed == true;
        delay(1000);
        if (sensorState  == LOW) {
          isFull = true;
        }
        else {
          compressed = false;
        }
      }
    }
    break;
  }

  while (isFull) {

    digitalWrite(PIN_LED_13, HIGH);
    digitalWrite(PIN_LED_12, LOW);

    // read the state of the pushbutton value:
    sensorState = digitalRead(SENSORPIN);

    if (sensorState == HIGH) {
      compressed == false;
      isFull == false;
    }
    break;
  }
}
