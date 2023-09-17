#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
  #include <Firebase_ESP_Client.h>

//Token generation process info.
#include "addons/TokenHelper.h"

//The RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#include "ws2812.h"
#include "twilio.hpp"

// Values from Twilio (find them on the dashboard)
static const char *account_sid = "AC1b17407ef48e2cc7ef17a71dd53bbe63";
static const char *auth_token = "7d9dc49fea6f6e898f62cc4ad2fd8c19";
// Phone number should start with "+<countrycode>"
static const char *from_number = "+18556435472";

// You choose!
// Phone number should start with "+<countrycode>"
static const char *to_number = "+12673618594";
static const char *message = "Potential harm to Mr. X with possible life threatening encounters with temporarily unknown individuals. Please provide Emergency services to the location: 3408 Powelton Avenue APT #1A, Philadelphia, PA 19104. Note: Mr. X is a visually impaired individual with special care and in need of proper professional care in regards to aid the situation.";

Twilio *twilio;

//Network credentials.
#define WIFI_SSID "HUNOSWTHAPENS"
#define WIFI_PASSWORD "annafyh3732"

// Defines the Digital Pin of the "On Board LED".
#define On_Board_LED 2

//Firebase project API Key
#define API_KEY "AIzaSyANO-45Sq1N09OjxfPKESvdll1bE287f_g"

//RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://ewb-voltage-remote-sensing-default-rtdb.firebaseio.com/"

char *CaptureTrigger = "Harm";
//Firebase Data object.
FirebaseData fbdo;

//Firebase authentication.
FirebaseAuth auth;

//Firebase configuration.
FirebaseConfig config;

int LED = 7;
int GND = 15;
int THR = 40000;
int frontSONAR = 11;
int leftSONAR = 12;
int rightSONAR = 13;
int backSONAR = 14;
String status = "No walls";
//int status = 0;

unsigned long sendDataPrevMillis = 300;
const long sendDataIntervalMillis = 500;
int count = 0;
bool signupOK = false;

String response;
int Message = 0;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  Serial.println();
  pinMode(LED, OUTPUT);
  pinMode(GND, OUTPUT);
  ws2812Init();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wifi");
  Serial.print("Connecting to : ");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(30);
  }
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(WIFI_SSID);
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Assign the api key (required).
  config.api_key = API_KEY;

  // Assign the RTDB URL (required).
  config.database_url = DATABASE_URL;

  // Sign up.
  Serial.println();
  Serial.println("Sign up");
  Serial.print("Sign up ISee AI user... ");
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Serial.println("---------------");
 
  //Callback function for the long running token generation task.
  config.token_status_callback = tokenStatusCallback; //See addons/TokenHelper.h
 
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  twilio = new Twilio(account_sid, auth_token);

  String response;
  bool success = twilio->send_message(to_number, from_number, message, response);
}


void loop() {
  // put your main code here, to run repeatedly:
  if (touchRead(frontSONAR) > THR){
    digitalWrite(LED, HIGH);
    digitalWrite(GND, LOW);
    status = "Front";
    //status = 1;
    }

  else if (touchRead(leftSONAR) > THR){
    digitalWrite(LED, HIGH);
    digitalWrite(GND, LOW);
    status = "Left";
    //status = 2;
    }

  else if (touchRead(rightSONAR) > THR){
    digitalWrite(LED, HIGH);
    digitalWrite(GND, LOW);
    status = "Right";
    //status = 3;
    }

  else if (touchRead(backSONAR) > THR){
    digitalWrite(LED, HIGH);
    digitalWrite(GND, LOW);
    status = "Back";
    //status = 4;
    }

  else if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    if (Firebase.RTDB.setString(&fbdo, "Active Sensor", status)){
      Serial.println("PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
      digitalWrite(LED, LOW);
      digitalWrite(GND, LOW);
    }
  }
  else{
      digitalWrite(LED, LOW);
      digitalWrite(GND, LOW);
      status = "No walls";
      //status = 0;
  }
}