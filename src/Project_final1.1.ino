/*#define BLYNK_TEMPLATE_ID "TMPL6M8li-hQg" 
#define BLYNK_TEMPLATE_NAME "Green House Automation" */

#include <SFE_BMP180.h>
#include <Wire.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// Blynk Auth Token
/*char blynkAuth[] = "wPRo0YvPpZZy0dqjNgaXNo0AHywGVf-v";  */

/* Wi-Fi credentials */
const char* ssid1 = "Athula";
const char* password1 = "1978At1430Hu4213La#@";

const char* ssid2 = "GreenHouse";
const char* password2 = "12345678";

/* Firebase API key and database URL */
#define API_KEY "AIzaSyDZZ_XNEHq688SoblWPd0dmnW19ih-46dM"
#define DATABASE_URL "https://esp32-test1-95778-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long dataMillis = 0;
bool signupOK = false;

SFE_BMP180 pressure;
#define ALTITUDE 30.0 // Altitude in meters

// Pin definitions for fan (Motor 1)
const int IN1_Fan = 23;
const int IN2_Fan = 18;
const int ENA_Fan = 19;     

// Pin definitions for water pump (Motor 2)
const int IN3_WaterPump = 25;
const int IN4_WaterPump = 26;
const int ENA_WaterPump = 33; 

// Define GPIO pins
#define FAN_LED_PIN 2 
#define WATER_PUMP_LED_PIN 4
#define DO_PIN 13  // ESP32's pin GPIO13 connected to DO pin of the LDR module
#define MQ135_SENSOR_PIN 32 // Pin for MQ135 sensor
#define DHTPIN 16 // GPIO pin for DHT11
#define DHTTYPE DHT11 // DHT 11 sensor type

#define FAN_BUTTON_PIN 14    // GPIO pin for fan control button
#define WATER_PUMP_BUTTON_PIN 17  // GPIO pin for water pump control button

bool fanStatus = false;
bool waterPumpStatus = false;

int sensitivity = 200;  // Adjust this value based on your calibration

// Soil Moisture Sensor pins and values
const int soil_Sensor_Data_Pin = 34;  // GPIO pin for Soil Moisture Sensor
const int AirValue = 2300;   // Dry soil value
const int WaterValue = 1215; // Wet soil value
int soilMoistureValue = 0;
int soilmoisturepercent = 0;

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void connectToWiFi(const char* ssid, const char* password) {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect");
  }
}

void setup()
{
    Serial.begin(115200);

    // Initialize BMP180 sensor
    if (pressure.begin())
        Serial.println("BMP180 init success");
    else
    {
        Serial.println("BMP180 init fail");
        while (1); // Pause forever if sensor initialization fails.
    }

     // Setup LED pins
    pinMode(FAN_LED_PIN, OUTPUT);
    pinMode(WATER_PUMP_LED_PIN, OUTPUT);
    digitalWrite(FAN_LED_PIN, LOW);
    digitalWrite(WATER_PUMP_LED_PIN, LOW);

    // Set motor pins as outputs
    pinMode(IN1_Fan, OUTPUT);
    pinMode(IN2_Fan, OUTPUT);
    pinMode(ENA_Fan, OUTPUT);
    pinMode(IN3_WaterPump, OUTPUT);
    pinMode(IN4_WaterPump, OUTPUT);
    pinMode(ENA_WaterPump, OUTPUT);

    // Setup LDR and MQ135 pins
    pinMode(DO_PIN, INPUT);

    // Setup button pins
    pinMode(FAN_BUTTON_PIN, INPUT_PULLDOWN);
    pinMode(WATER_PUMP_BUTTON_PIN, INPUT_PULLDOWN);

    // Initialize DHT sensor
    dht.begin();

    // Connect to Wi-Fi
    // Attempt to connect to the first Wi-Fi network
    connectToWiFi(ssid1, password1);

    // If connection fails, try the second Wi-Fi network
    if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Trying to connect to the second network...");
    connectToWiFi(ssid2, password2);
    }

    // Initialize Firebase
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    Firebase.reconnectWiFi(true);
    fbdo.setBSSLBufferSize(4096, 1024);

    Serial.print("Sign up new user... ");
    if (Firebase.signUp(&config, &auth, "", ""))
    {
        Serial.println("ok");
        signupOK = true;
    }
    else
    {
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

    config.token_status_callback = tokenStatusCallback;
    Firebase.begin(&config, &auth);

     /*Blynk.begin(blynkAuth, ssid1, password1);
     Serial.println("Smart Home System Initialized");*/
}

String interpret_air_quality(int sensor_value) {
    if (sensor_value < 58) {
        return "Excellent";
    } else if (sensor_value < 90) {
        return "Good";
    } else if (sensor_value < 120) {
        return "Moderate";
    } else if (sensor_value < 200) {
        return "Poor";
    } else {
        return "Dangerous";
    }
}

int calculate_air_quality_percentage(int sensor_value) {
    int max_value = 200;  // The maximum value for "Dangerous" level
    int percentage = 100 - (sensor_value * 100) / max_value;  // Reverse the percentage scale
  
    // Ensure the percentage is within 0-100%
    if (percentage < 0) {
        percentage = 0;
    } else if (percentage > 100) {
        percentage = 100;
    }
  
    return percentage;
}

void controlFan(int IN1_Fan, int IN2_Fan, bool state) {
    if (state) {
        // Turn on motor
        digitalWrite(IN1_Fan, HIGH);
        digitalWrite(IN2_Fan, LOW);
        digitalWrite(ENA_Fan, HIGH); 
    } else {
        // Turn off motor
        digitalWrite(IN1_Fan, LOW);
        digitalWrite(IN2_Fan, LOW);
        digitalWrite(ENA_Fan, LOW); 
    }
}

void controlMotor(int IN3_WaterPump, int IN4_WaterPump, bool state) {
    if (state) {
        // Turn on motor
        digitalWrite(IN3_WaterPump, HIGH);
        digitalWrite(IN4_WaterPump, LOW);
        digitalWrite(ENA_WaterPump, HIGH); 
    } else {
        // Turn off motor
        digitalWrite(IN3_WaterPump, LOW);
        digitalWrite(IN4_WaterPump, LOW);
        digitalWrite(ENA_WaterPump, LOW); 
    }
}

// Define a mode variable
enum Mode { AUTOMATIC, MANUAL };
Mode currentMode = AUTOMATIC;

void loop()
{

   /* Blynk.run();  // Keep Blynk connected
    delay(300);*/

    char status;
    double P, p0;
    double T; // Change float to double

    // Check button presses for fan and water pump control
    if (digitalRead(FAN_BUTTON_PIN) == HIGH) {  // HIGH for INPUT_PULLDOWN or LOW for INPUT_PULLUP
        Serial.println("Fan button pressed");
        delay(200); // Debouncing
        fanStatus = !fanStatus; // Toggle the fan status
        digitalWrite(FAN_LED_PIN, fanStatus ? HIGH : LOW);

        // Control the fan motor based on the LED state
        controlFan(IN1_Fan, IN2_Fan, fanStatus);

        // Switch to manual mode
        currentMode = MANUAL;

       // Update Firebase with the new fan status
        if (Firebase.ready() && signupOK) {
            if (!Firebase.RTDB.setBool(&fbdo, "/sensorOutputs/fanStatus", fanStatus)) {
                Serial.println("Failed to update fan status in Firebase");
            } else {
                Serial.println("Fan status updated in Firebase");
            }
        }
    }

    if (digitalRead(WATER_PUMP_BUTTON_PIN) == HIGH) {  // HIGH for INPUT_PULLDOWN or LOW for INPUT_PULLUP
        Serial.println("Water pump button pressed");
        delay(200); // Debouncing
        waterPumpStatus = !waterPumpStatus; // Toggle the water pump status
        digitalWrite(WATER_PUMP_LED_PIN, waterPumpStatus ? HIGH : LOW);

        // Control the water pump motor based on the LED state
        controlMotor(IN3_WaterPump, IN4_WaterPump, waterPumpStatus);

        // Switch to manual mode
        currentMode = MANUAL;

        // Update Firebase with the new water pump status
        if (Firebase.ready() && signupOK) {
            if (!Firebase.RTDB.setBool(&fbdo, "/sensorOutputs/waterPumpStatus", waterPumpStatus)) {
                Serial.println("Failed to update water pump status in Firebase");
            } else {
                Serial.println("Water pump status updated in Firebase");
            }
        }
    }

    // Retrieve fan status from Firebase to reflect any remote changes
    if (Firebase.RTDB.getBool(&fbdo, "/sensorOutputs/fanStatus")) {
        bool newFanStatus = fbdo.boolData();
        if (newFanStatus != fanStatus) {  // Only change if there's a difference
            fanStatus = newFanStatus;
            digitalWrite(FAN_LED_PIN, fanStatus ? HIGH : LOW);

            // Control the fan motor based on the LED state
            controlFan(IN1_Fan, IN2_Fan, fanStatus);
            Serial.println("Fan status updated based on Firebase data");
        }
    }

    // Retrieve water pump status from Firebase to reflect any remote changes
    if (Firebase.RTDB.getBool(&fbdo, "/sensorOutputs/waterPumpStatus")) {
        bool newWaterPumpStatus = fbdo.boolData();
        if (newWaterPumpStatus != waterPumpStatus) {  // Only change if there's a difference
            waterPumpStatus = newWaterPumpStatus;
            digitalWrite(WATER_PUMP_LED_PIN, waterPumpStatus ? HIGH : LOW);

            // Control the water pump motor based on the LED state
            controlMotor(IN3_WaterPump, IN4_WaterPump, waterPumpStatus);
            Serial.println("Water pump status updated based on Firebase data");
        }
    }


    // Read LDR value
    int lightState = digitalRead(DO_PIN);
    String lightCondition = lightState == HIGH ? "Dark" : "Light";

    // Read MQ135 sensor value for air quality
    int sensor_value = analogRead(MQ135_SENSOR_PIN);
    int air_quality = sensor_value * sensitivity / 1023;
    String air_quality_label = interpret_air_quality(air_quality);
    int air_quality_percentage = calculate_air_quality_percentage(air_quality);

    // Read DHT11 sensor values
    float h = dht.readHumidity();
    float tempC = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(tempC)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Compute heat index in Celsius
    float hic = dht.computeHeatIndex(tempC, h, false);

    // Read the analog value from the Soil Moisture Sensor
    soilMoistureValue = analogRead(soil_Sensor_Data_Pin);
    soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

     // Ensure the percentage is within 0-100%
    soilmoisturepercent = constrain(soilmoisturepercent, 0, 100);

    
    // Only run automatic logic if in automatic mode
    if (currentMode == AUTOMATIC) {

    if (T > 29) {
        if (!fanStatus) {
            Serial.println("Temperature is above 29°C. Turning fan ON.");
            fanStatus = true;
            digitalWrite(FAN_LED_PIN, HIGH);
            controlFan(IN1_Fan, IN2_Fan, true);

            // Update Firebase with the new fan status
            if (Firebase.ready() && signupOK) {
                if (!Firebase.RTDB.setBool(&fbdo, "/sensorOutputs/fanStatus", fanStatus)) {
                    Serial.println("Failed to update fan status in Firebase");
                } else {
                    Serial.println("Fan status updated in Firebase");
                }
            }
        }
    } else {
        if (fanStatus) {
            Serial.println("Temperature is below or equal to 29°C. Turning fan OFF.");
            fanStatus = false;
            digitalWrite(FAN_LED_PIN, LOW);
            controlFan(IN1_Fan, IN2_Fan, false);

            // Update Firebase with the new fan status
            if (Firebase.ready() && signupOK) {
                if (!Firebase.RTDB.setBool(&fbdo, "/sensorOutputs/fanStatus", fanStatus)) {
                    Serial.println("Failed to update fan status in Firebase");
                } else {
                    Serial.println("Fan status updated in Firebase");
                }
            }
        }
    }

    // Automation logic for water pump control
    if (soilmoisturepercent < 50) {
        if (!waterPumpStatus) {
            Serial.println("Soil moisture is below 50%. Turning water pump ON.");
            waterPumpStatus = true;
            digitalWrite(WATER_PUMP_LED_PIN, HIGH);
            controlMotor(IN3_WaterPump, IN4_WaterPump, true);

            // Update Firebase with the new water pump status
            if (Firebase.ready() && signupOK) {
                if (!Firebase.RTDB.setBool(&fbdo, "/sensorOutputs/waterPumpStatus", waterPumpStatus)) {
                    Serial.println("Failed to update water pump status in Firebase");
                } else {
                    Serial.println("Water pump status updated in Firebase");
                }
            }
        }
    } else {
        if (waterPumpStatus) {
            Serial.println("Soil moisture is above or equal to 50%. Turning water pump OFF.");
            waterPumpStatus = false;
            digitalWrite(WATER_PUMP_LED_PIN, LOW);
            controlMotor(IN3_WaterPump, IN4_WaterPump, false);

            // Update Firebase with the new water pump status
            if (Firebase.ready() && signupOK) {
                if (!Firebase.RTDB.setBool(&fbdo, "/sensorOutputs/waterPumpStatus", waterPumpStatus)) {
                    Serial.println("Failed to update water pump status in Firebase");
                } else {
                    Serial.println("Water pump status updated in Firebase");
                }
            }
        }
    }
    }

    // Send data to Firebase and print sensor values (rest of your logic)
    // This section will handle the sending of the sensor data to Firebase
    // and display the values on the Serial Monitor (temperature, soil moisture, etc.)




    if (millis() - dataMillis > 5000 && signupOK && Firebase.ready())
    {
        dataMillis = millis();

        /*// Print the soil moisture value
        Serial.print("Soil Moisture Raw Value: ");
        Serial.println(soilMoistureValue);

        
        // Print the soil moisture percentage
        Serial.print("Soil Moisture Percentage: ");
        if (soilmoisturepercent >= 100) {
        Serial.println("100%");
        } else if (soilmoisturepercent <= 0) {
        Serial.println("0%");
        } else {
        Serial.print(soilmoisturepercent);
        Serial.println("%");
        }*/

        // Start temperature measurement
        status = pressure.startTemperature();
        if (status != 0)
        {
            delay(status);
            status = pressure.getTemperature(T); // T should be double
            if (status != 0)
            {
                // Start pressure measurement
                status = pressure.startPressure(3);
                if (status != 0)
                {
                    delay(status);
                    status = pressure.getPressure(P, T); // P and T should be double
                    if (status != 0)
                    {
                        // Calculate sea-level pressure
                        p0 = pressure.sealevel(P, ALTITUDE);

                        // Print the readings
                        Serial.print("Temperature (BMP180): ");
                        Serial.print(T, 2);
                        Serial.print(" °C, Pressure: ");
                        Serial.print(P, 2);
                        Serial.print(" mb, Sea-level Pressure: ");
                        Serial.print(p0, 2);
                        Serial.println(" mb");
                        Serial.println();


                        // Print the light condition
                        Serial.print("Light Condition: ");
                        Serial.println(lightCondition);
                        Serial.println();

                        // Print air quality readings
                        Serial.print("Air Quality Index (Calibrated): ");
                        Serial.println(air_quality);
                        Serial.print("Air Quality: ");
                        Serial.println(air_quality_label);
                        Serial.print("Air Quality Percentage: ");
                        Serial.print(air_quality_percentage);
                        Serial.println("%");
                        Serial.println();

                        // Print DHT11 sensor readings
                        Serial.print("Humidity: ");
                        Serial.print(h);
                        Serial.print("%, Temperature (DHT11): ");
                        Serial.print(tempC);
                        Serial.print("°C, Heat Index: ");
                        Serial.print(hic);
                        Serial.println("°C");
                        Serial.println();

                        // Print soil moisture readings
                        Serial.print("Soil Moisture Percentage: ");
                        Serial.println(soilmoisturepercent);
                        Serial.println();

                        // Send the readings to Firebase
                        if (Firebase.RTDB.setFloat(&fbdo, "/sensorData/temperature", T))
                        {
                            Serial.println("Temperature (BMP180) sent successfully.");
                        }
                        else
                        {
                            Serial.println("Failed to send temperature.");
                            Serial.println("REASON: " + fbdo.errorReason());
                        }

                        if (Firebase.RTDB.setFloat(&fbdo, "/sensorData/airPressure", P))
                        {
                            Serial.println("Pressure sent successfully.");
                        }
                        else
                        {
                            Serial.println("Failed to send pressure.");
                            Serial.println("REASON: " + fbdo.errorReason());
                        }

                        if (Firebase.RTDB.setString(&fbdo, "/sensorData/lightCondition", lightCondition))
                        {
                            Serial.println("Light condition sent successfully.");
                        }
                        else
                        {
                            Serial.println("Failed to send light condition.");
                            Serial.println("REASON: " + fbdo.errorReason());
                        }

                        if (Firebase.RTDB.setFloat(&fbdo, "/sensorData/airQuality", air_quality_percentage))
                        {
                            Serial.println("Air Quality Percentage sent successfully.");
                        }
                        else
                        {
                            Serial.println("Failed to send air quality percentage.");
                            Serial.println("REASON: " + fbdo.errorReason());
                        }

                        if (Firebase.RTDB.setFloat(&fbdo, "/sensorData/humidity", h))
                        {
                            Serial.println("Humidity sent successfully.");
                        }
                        else
                        {
                            Serial.println("Failed to send humidity.");
                            Serial.println("REASON: " + fbdo.errorReason());
                        }

                        /*if (Firebase.RTDB.setFloat(&fbdo, "/sensorData/temperatureDHT11", tempC))
                        {
                            Serial.println("Temperature (DHT11) sent successfully.");
                        }
                        else
                        {
                            Serial.println("Failed to send temperature (DHT11).");
                            Serial.println("REASON: " + fbdo.errorReason());
                        }*/

                        if (Firebase.RTDB.setFloat(&fbdo, "/sensorData/soilMoisture", soilmoisturepercent))
                        {
                            Serial.println("Soil Moisture sent successfully.");
                            Serial.println();
                        }
                        else
                        {
                            Serial.println("Failed to send soil moisture.");
                            Serial.println("REASON: " + fbdo.errorReason());
                        }
                        
                        // Retrieve fan status from Firebase
                        if (Firebase.RTDB.getBool(&fbdo, "/sensorOutputs/fanStatus"))
                        {
                            bool fanStatus = fbdo.boolData();
                            digitalWrite(FAN_LED_PIN, fanStatus ? HIGH : LOW);
                        }

                        // Retrieve water pump status from Firebase
                        if (Firebase.RTDB.getBool(&fbdo, "/sensorOutputs/waterPumpStatus"))
                        {
                            bool waterPumpStatus = fbdo.boolData();
                            digitalWrite(WATER_PUMP_LED_PIN, waterPumpStatus ? HIGH : LOW);
                        }
                    }
                    else
                    {
                        Serial.println("Error retrieving pressure measurement.");
                    }
                }
                else
                {
                    Serial.println("Error starting pressure measurement.");
                }
            }
            else
            {
                Serial.println("Error retrieving temperature measurement.");
            }
        }
        else
        {
            Serial.println("Error starting temperature measurement.");
        }
    }
}


// Greenhouse Project by Kavi-ya (Atronox)