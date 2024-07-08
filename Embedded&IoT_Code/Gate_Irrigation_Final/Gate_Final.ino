#include <Keypad.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Arduino_JSON.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"

// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Replace these with your network credentials
#define WIFI_SSID "Galaxy A51 2D76"
#define WIFI_PASSWORD "epvt5792"

// Replace these with your Firebase project credentials
#define FIREBASE_PROJECT_ID "homex-fd825-default-rtdb"
#define API_KEY "AIzaSyBgjLyqpF-Z-S59aPKfbRg7EVGdeNS3TGU"

// Define the user email and password for Firebase authentication
#define USER_EMAIL "nour.gamal3200@gmail.com"
#define USER_PASSWORD "ASHNHomeX"

// Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

/* Define pins */

#define ROW_NUM 4    // four rows
#define COLUMN_NUM 4 // four columns
#define SERVO_PIN 17 // the pin which connects to the servo motor

int sensor_pin = 34; // Sensor Pin
int relay_pin = 4;   // Relay Pin

unsigned long sendDataPrevMillis = 0;

Servo servo; // servo motor

char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte pin_rows[ROW_NUM] = {14, 27, 26, 25};     // connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {33, 32, 21, 19}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

const String password_1 = "3699";
const String password_2 = "5642B";
const String password_3 = "9765";
const String password_4 = "3333";
String input_password;

String gateStatusPath = "/esp1/ultrasonic/gate/state"; // Path to store gate status (0 or 1)

unsigned long lastCheckTime = 0; // to keep track of the last Firebase check time
unsigned long checkInterval = 10000; // interval to check Firebase (10 seconds)

bool isGateOpen = false; // flag to keep track of the gate state

// Function to check the gate status from Firebase
void checkGateStatus() {
  if (Firebase.RTDB.getInt(&fbdo, gateStatusPath.c_str())) {
    int gateStatus = fbdo.intData();
    if (gateStatus == 1 && !isGateOpen) {
      Serial.println("Firebase indicates gate should be open.");
      openGate();
    } else if (gateStatus == 0 && isGateOpen) {
      Serial.println("Firebase indicates gate should be closed.");
      closeGate();
    }
  } else {
    Serial.println("Failed to read data from Firebase");
    Serial.println("Reason: " + fbdo.errorReason());
  }
}

void openGate() {
  if (!isGateOpen) {
    Serial.println("Opening gate...");
    servo.write(180); // Run the continuous servo forward (adjust as needed)
    delay(3000);      // Run the servo for 3 seconds (adjust as needed)
    // servo.write(90);  // Stop the servo (adjust to stop)
    isGateOpen = true; // Update the gate state
  } else {
    Serial.println("Gate is already open.");
  }
}

void closeGate() {
  if (isGateOpen) {
    Serial.println("Closing gate...");
    servo.write(0);  // Run the continuous servo backward (adjust as needed)
    delay(3000);     // Run the servo for 3 seconds (adjust as needed)
    // servo.write(90); // Stop the servo (adjust to stop)
    isGateOpen = false; // Update the gate state
  } else {
    Serial.println("Gate is already closed.");
  }
}

void setup() {
  Serial.begin(115200);
  input_password.reserve(32); // maximum password size is 32, change if needed

  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected.");

  // Firebase setup
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = "https://" FIREBASE_PROJECT_ID ".firebaseio.com"; // Replace with your Firebase Database URL

  // Assign the callback function for the long running token generation task.
  config.token_status_callback = tokenStatusCallback; // See addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;

  pinMode(sensor_pin, INPUT);
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, LOW);

  // Attach servo
  servo.attach(SERVO_PIN);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.println(key);

    if (key == '*') {
      input_password = ""; // reset the input password
    } else if (key == '#') {
      if (input_password == password_1 || input_password == password_2 || input_password == password_3) {
        Serial.println("The password is correct, opening gate.");
        openGate();
      } else if (input_password == password_4) {
        Serial.println("The password is correct, closing gate.");
        closeGate();
      } else {
        Serial.println("The password is incorrect, try again.");
      }

      input_password = ""; // reset the input password
    } else {
      input_password += key; // append new character to input password string
    }
  }
  // Check Firebase gate status periodically
  else if (millis() - lastCheckTime > checkInterval) {
    lastCheckTime = millis();
    checkGateStatus();
  }

  // Firebase.ready() should be called repeatedly to handle authentication tasks.
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    int sensor_data = analogRead(sensor_pin);
    Serial.print("Sensor_data:");
    Serial.print(sensor_data);
    Serial.print("\t | ");
    

    Firebase.RTDB.getString(&fbdo, "/esp1/onoffsystem/soilstate/state");
    String soilstate = fbdo.stringData();
    if(soilstate == "1"){
      if (sensor_data >= 3500) {
        Serial.println("No moisture, Soil is dry");
        digitalWrite(relay_pin, LOW);
      } else {
        Serial.println("Soil is wet");
        digitalWrite(relay_pin, HIGH);
      }
    }
    else{
        digitalWrite(relay_pin, HIGH);
    }
    
    int soilReadingPercent = map(sensor_data, 0, 4095, 0, 100); // assuming sensor range is 0-4095
    Serial.print("The soil reading percentage : ");
    Serial.println(soilReadingPercent);
    Serial.println();

    // Send soil reading percentage to Firebase
    if (Firebase.RTDB.setInt(&fbdo, "/esp1/sensors/soil", soilReadingPercent)) {
      Serial.println("Data sent to Firebase");
    } else {
      Serial.print("Failed to send data to Firebase: ");
      Serial.println(fbdo.errorReason());
    }
}
}
