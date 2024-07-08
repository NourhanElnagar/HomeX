#include <WiFi.h>
#include <Firebase_ESP_Client.h>


/* 1. Define the WiFi credentials */
#define WIFI_SSID "Galaxy A51 2D76"
#define WIFI_PASSWORD "epvt5792"

/* 2. Define the API Key */
#define API_KEY "AIzaSyBgjLyqpF-Z-S59aPKfbRg7EVGdeNS3TGU"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://homex-fd825-default-rtdb.firebaseio.com/" 

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "nour.gamal3200@gmail.com"
#define USER_PASSWORD "ASHNHomeX"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;


/// Reception ///
#define led_1 23
#define fan 27

/// Kitchen ///
#define led_5 5

#define lm 32
#define buzzer_2 4
int temp;

/// Room ///
#define led_2 18

/// Bathroom ///
#define led_4 19

#define gas 34
#define buzzer_1 13
int Sensor;

/// Office ///
#define led_3 21


/// The Garden /// 
// Ultrasonic ///
#define Echo 22
#define Trigger 15
float Distance;
float Duration;

//ldr with led
const int ledPin = 12;   //the number of the LED pin
const int ldrPin = 33;  //the number of the LDR pin

unsigned long lastTime;
unsigned long lastTime1;
//////////////////////////

void setup()
{
  
/// Reception ///
     ///led///
  pinMode(led_1, OUTPUT);
  digitalWrite(led_1, LOW);

   /// fan ///
  pinMode(fan,OUTPUT);
  digitalWrite(fan, LOW);

/// Room ///
  pinMode(led_2, OUTPUT);
  digitalWrite(led_2, LOW);

/// Office ///
  pinMode(led_3, OUTPUT);
  digitalWrite(led_3, LOW);

/// Bathroom ///
  pinMode(led_4, OUTPUT);
  digitalWrite(led_4, LOW);


  pinMode(buzzer_1, OUTPUT);

/// Kitchen ///
  pinMode(led_5, OUTPUT);
  digitalWrite(led_5, LOW);

  pinMode(buzzer_2, OUTPUT);
////////////////////////////

 /// The Garden ///

 // Ultrasonic ///
  pinMode(Echo, INPUT);
  pinMode(Trigger, OUTPUT);

 // ldr
  pinMode(ldrPin, INPUT);   //initialize the LDR pin as an input
  pinMode(ledPin, OUTPUT);  //initialize the LED pin as an output

  lastTime = millis();
  lastTime1 = millis();
  
 

///wifi////
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096, 1024/* Rx buffer size in bytes from 512 - 16384 /, 1024 / Tx buffer size in bytes from 512 - 16384 */);

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;
  while (!Serial);
}

void loop()
{
  // Firebase.ready() should be called repeatedly to handle authentication tasks.
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    

/////start the code////

/// Reception ///
///////////led//////////
   if(Firebase.RTDB.getString(&fbdo, "/esp1/leds/reception/state")){
    digitalWrite(led_1, atoi(fbdo.to<const char *>()));
   }else{
    Serial.println(fbdo.errorReason().c_str());
   }

   ///////////////////fan///////
  if(Firebase.RTDB.getString(&fbdo, "/esp1/fan/state")){
    digitalWrite(fan, atoi(fbdo.to<const char *>()));
   }else{
    Serial.println(fbdo.errorReason().c_str());
   }

/// Room ///

 if(Firebase.RTDB.getString(&fbdo, "/esp1/leds/room/state")){
    digitalWrite(led_2 , atoi(fbdo.to<const char *>()));
   }else{
    Serial.println(fbdo.errorReason().c_str());
   }


/// Office ///
if(Firebase.RTDB.getString(&fbdo, "/esp1/leds/office/state")){
    digitalWrite(led_3 , atoi(fbdo.to<const char *>()));
   }else{
    Serial.println(fbdo.errorReason().c_str());
   }


/// Bathroom leds///
if(Firebase.RTDB.getString(&fbdo, "/esp1/leds/bath/state")){
    digitalWrite(led_4, atoi(fbdo.to<const char *>()));
   }else{
    Serial.println(fbdo.errorReason().c_str());
   }

 /// Bathroom gas system ///
  Sensor=analogRead(gas);
  Serial.print("Gas Reading = ");
  Serial.print(Sensor);
  Serial.println();

  int buzzer_1_value;

  if(Sensor>3000){
    digitalWrite(buzzer_1,HIGH);
    buzzer_1_value = 1;
  
  }
  else{
    digitalWrite(buzzer_1,LOW);
    buzzer_1_value = 0;
    
  }
  if (Firebase.RTDB.setInt(&fbdo, "/esp1/sensors/gasBuzzer/state", buzzer_1_value)) {
      Serial.println("Data uploaded successfully");
      
    } 
    else {
      Serial.println("Failed to upload data");
      Serial.println("Error: " + fbdo.errorReason());
    }
    
    Serial.print("Buzzer_1_value: ");
    Serial.println(buzzer_1_value);
    delay(1000);

 String gasReading = (String) map(analogRead(gas),0,5000,0,100);
  Serial.print("The gas reading precentage : ");
  Serial.println(gasReading);
  Serial.println();
  delay(1000);
  Firebase.RTDB.setString(&fbdo,"/esp1/sensors/gas",gasReading);


  ////////////////////////////////

/// Kitchen ///
////led//////////////
if(Firebase.RTDB.getString(&fbdo, "/esp1/leds/kitchen/state")){
    digitalWrite(led_5 , atoi(fbdo.to<const char *>()));
}
else{
    Serial.println(fbdo.errorReason().c_str());
}


/////////////lm system//////////
  float result;
  temp = analogRead(lm);
  temp = temp * 4.88;
  result = temp / 10;
  // Serial.print("Temp = ");
  // Serial.print(result);
  // Serial.print(" C");
  // Serial.println();

  int buzzer_2_value;

  if(result>700){
    digitalWrite(buzzer_2,HIGH);
    buzzer_2_value = 1;
   
  }
  else{
    digitalWrite(buzzer_2,LOW);
    buzzer_2_value = 0;
    
  }
  if (Firebase.RTDB.setInt(&fbdo, "/esp1/sensors/TemBuzzer/state", buzzer_2_value)) {
      Serial.println("Data uploaded successfully");
      
    } 
    else {
      Serial.println("Failed to upload data");
      Serial.println("Error: " + fbdo.errorReason());
    }

  Serial.print("Buzzer_2_value: ");
  Serial.println(buzzer_2_value);
  delay(1000);

  String lmReading = (String) map(result,0,1400,0,100);
  Serial.print("The lm reading precentage : ");
  Serial.println(lmReading);
  Serial.println();
  delay(1000);
  Firebase.RTDB.setString(&fbdo,"/esp1/sensors/lm",lmReading);


 ////////////////////
  /// The Garden /// 

/////// Ultrasonic ////////
  digitalWrite(Trigger,LOW);
  delayMicroseconds(2);
  digitalWrite(Trigger,HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger,LOW);

  Duration=pulseIn(Echo,HIGH);

  Distance=0.017*Duration;

  Serial.print("Distance: ");
  Serial.println(Distance);
  
  int value = (Distance < 50) ? 1 : 0;

  if (value == 1) {
      Serial.println("There is someone near the camera !!!!!!");
  } 
  else {
      Serial.println("There is no one ------");
  }
  
  if (Firebase.RTDB.setInt(&fbdo, "/esp1/ultrasonic/state", value)) {
      Serial.println(" ultrasonic Data uploaded successfully");
      
  } 
  else {
      Serial.println("Failed to upload data");
      Serial.println("Error: " + fbdo.errorReason());
  }

  Serial.print("Value of ultrasonic in firebase : ");
  Serial.println(value);
  delay(1000);

//////ldr with led//////

int notify_value;
if(Firebase.RTDB.getInt(&fbdo, "/esp1/onoffsystem/ldr/state")){
  int ldrSystemState = fbdo.intData();
  Serial.print("LDR System State: ");
  Serial.println(ldrSystemState);

  int ldrStatus = analogRead(ldrPin);   //read the status of the LDR value
   Serial.print("Reading ldr: ");
  Serial.println(ldrStatus);

  if (ldrSystemState) {// Check if the LDR system is enabled
    if (ldrStatus <=300) {
      digitalWrite(ledPin, HIGH);  //turn LED on
      notify_value = 1;
    }
    else{
      digitalWrite(ledPin, LOW);  //turn LED off
      notify_value = 0;
      }
      
  }

else{
  Serial.println("LDR system is Off...");
  digitalWrite(ledPin, LOW);
  notify_value = 0;
}
if  (Firebase.RTDB.setInt(&fbdo, "/esp1/sensors/ldrnotify/state", notify_value )) {
      Serial.println("Ldr notify  Data uploaded successfully");
      } 
      else {
      Serial.println("Failed to upload data");
      Serial.println("Error: " + fbdo.errorReason());
      }
delay(100);
}


else {
    Serial.println("Failed to get LDR system state");
    Serial.println("Error: " + fbdo.errorReason());
  }


 /////////////////// 
}
}



  
  

