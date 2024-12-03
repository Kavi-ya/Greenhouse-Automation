#include <esp_now.h>
#include <WiFi.h>
#include "AudioGeneratorAAC.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourcePROGMEM.h"
#include <HardwareSerial.h>

#include "Now_the_temperature_is.h"

#include "20.h"
#include "21.h"
#include "22.h"
#include "23.h"
#include "24.h"
#include "25.h"
#include "26.h"
#include "27.h"
#include "28.h"
#include "29.h"
#include "30.h"
#include "31.h"
#include "32.h"
#include "33.h"
#include "34.h"
#include "35.h"
#include "36.h"
#include "37.h"
#include "38.h"

#include "It_s_dark.h"
#include "It_s_light.h"

#include "Humidity_level_is_100.h"
#include "Humidity_level_is_above_95.h"
#include "Humidity_level_is_above_90.h"
#include "Humidity_level_is_above_85.h"
#include "Humidity_level_is_above_80.h"
#include "Humidity_level_is_below_75.h"

#include "The_fan_is_running_smoothly.h"
#include "The_fan_is_currently_off.h"
#include "The_water_pump_is_running_perfectly.h"
#include "The_water_pump_is_turned_off_right_now.h"

#define LIGHT 2
#define Fan_Led 4
#define Waterpump_Led 18

int redPin = 34;    // Red RGB pin
int greenPin = 32;  // Green RGB pin
int bluePin = 33;   // Blue RGB pin

bool isWiFiConnected = false;    // Flag for Wi-Fi connection status
bool isEspNowConnected = false;  // Flag for ESP-NOW connection status
unsigned long lastColorChangeTime = 0;
unsigned long colorCycleInterval = 1000;  // 1 second for color cycle
int cycleIndex = 0;                       // To cycle through colors

void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

HardwareSerial VC02Serial(2);  // UART2

char ssid[] = "Athula";
char pass[] = "1978At1430Hu4213La#@";

typedef struct struct_message {
  float T;  // Temperature
  float h;  // Humidity

  char lightCondition[32];

  bool fanState;
  bool waterPumpState;

} struct_message;

struct_message receivedData;

esp_now_peer_info_t peerInfo;
uint8_t receiverMAC[] = { 0x78, 0xe3, 0x6d, 0x0b, 0x4e, 0x74 };

// Audio playback objects
AudioFileSourcePROGMEM *in;
AudioGeneratorAAC *aac;
AudioOutputI2S *out;


// Function to play audio
void playAudio(const uint8_t *audioData, size_t dataSize) {
  in = new AudioFileSourcePROGMEM(audioData, dataSize);
  aac = new AudioGeneratorAAC();
  out = new AudioOutputI2S();
  out->SetGain(1.0);

  if (!aac->begin(in, out)) {
    Serial.println("Audio initialization failed!");
    delete in;
    delete aac;
    delete out;
    return;
  }

  while (aac->isRunning()) {
    aac->loop();
  }

  aac->stop();
  delete in;
  delete aac;
}

// Callback when data is received
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (incomingData) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
  }
}

// Function to send commands (Make sure it's outside handleAudioPlayback function)
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("ESP-NOW Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// Function to handle audio playback separately
void handleAudioPlayback(bool playTemp, bool playHumidity, bool playLightCondition, bool playfanState, bool playwaterPumpState) {
  if (playTemp) {
    // Existing temperature audio playback logic
    int roundedTemp = (int)floor(receivedData.T);
    Serial.printf("Temperature: %.1f°C (Rounded: %d°C)\n", receivedData.T, roundedTemp);

    if (roundedTemp >= 20 && roundedTemp <= 38) {
      switch (roundedTemp) {
        case 20:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_20, sizeof(_20));
          break;

        case 21:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_21, sizeof(_21));
          break;

        case 22:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_22, sizeof(_22));
          break;

        case 23:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_23, sizeof(_23));
          break;

        case 24:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_24, sizeof(_24));
          break;

        case 25:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_25, sizeof(_25));
          break;

        case 26:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_26, sizeof(_26));
          break;

        case 27:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_27, sizeof(_27));
          break;

        case 28:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_28, sizeof(_28));
          break;

        case 29:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_29, sizeof(_29));
          break;

        case 30:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_30, sizeof(_30));
          break;

        case 31:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_31, sizeof(_31));
          break;

        case 32:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_32, sizeof(_32));
          break;

        case 33:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_33, sizeof(_33));
          break;

        case 34:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_34, sizeof(_34));
          break;

        case 35:
          playAudio(Now_the_temperature_is, sizeof(Now_the_temperature_is));
          playAudio(_35, sizeof(_35));
          break;

        default:
          Serial.println("Temperature out of audio range!");
      }
    }
  }

  if (playHumidity) {
    // Existing humidity audio playback logic
    Serial.printf("Humidity: %.1f%%\n", receivedData.h);
    if (receivedData.h < 75) {
      playAudio(Humidity_level_is_below_75, sizeof(Humidity_level_is_below_75));
    } else if (receivedData.h >= 75 && receivedData.h < 85) {
      playAudio(Humidity_level_is_above_80, sizeof(Humidity_level_is_above_80));
    } else if (receivedData.h >= 85 && receivedData.h < 90) {
      playAudio(Humidity_level_is_above_85, sizeof(Humidity_level_is_above_85));
    } else if (receivedData.h >= 90 && receivedData.h < 95) {
      playAudio(Humidity_level_is_above_90, sizeof(Humidity_level_is_above_90));
    } else if (receivedData.h >= 95 && receivedData.h <= 100) {
      playAudio(Humidity_level_is_above_95, sizeof(Humidity_level_is_above_95));
    } else if (receivedData.h == 100) {
      playAudio(Humidity_level_is_100, sizeof(Humidity_level_is_100));
    } else {
      Serial.println("Humidity is out of expected range!");
    }
  }

  if (playLightCondition) {
    // Existing light condition playback logic
    Serial.printf("Light Condition: %s\n", receivedData.lightCondition);

    if (strcmp(receivedData.lightCondition, "Light") == 0) {
      playAudio(It_s_light, sizeof(It_s_light));
    } else if (strcmp(receivedData.lightCondition, "Dark") == 0) {
      playAudio(It_s_dark, sizeof(It_s_dark));
    } else {
      Serial.println("Unknown light condition!");
    }
  }

  if (playfanState) {
    // Fan state audio playback logic
    Serial.printf("Fan Status: %s\n", receivedData.fanState ? "ON" : "OFF");
    if (receivedData.fanState) {
      playAudio(The_fan_is_running_smoothly, sizeof(The_fan_is_running_smoothly));
    } else {
      playAudio(The_fan_is_currently_off, sizeof(The_fan_is_currently_off));
    }
  }

  if (playwaterPumpState) {
    // Water pump state audio playback logic
    Serial.printf("Water Pump Status: %s\n", receivedData.waterPumpState ? "ON" : "OFF");
    if (receivedData.waterPumpState) {
      playAudio(The_water_pump_is_running_perfectly, sizeof(The_water_pump_is_running_perfectly));
    } else {
      playAudio(The_water_pump_is_turned_off_right_now, sizeof(The_water_pump_is_turned_off_right_now));
    }
  }
}

// Function to process VC02 command (Make sure it's outside handleAudioPlayback function)
// Updated function declaration and definition
void processVC02Command(uint8_t byte1, uint8_t byte2) {  // Pass individual bytes as parameters
  if ((byte1 == 0xAB && byte2 == 0x00) || (byte1 == 0x00 && byte2 == 0xAB)) {
    Serial.println("Playing temperature audio...");
    handleAudioPlayback(true, false, false, false, false);

  } else if ((byte1 == 0xAC && byte2 == 0x00) || (byte1 == 0x00 && byte2 == 0xAC)) {
    Serial.println("Playing humidity audio...");
    handleAudioPlayback(false, true, false, false, false);

  } else if ((byte1 == 0xAD && byte2 == 0x00) || (byte1 == 0x00 && byte2 == 0xAD)) {
    Serial.println("Playing light condition audio...");
    handleAudioPlayback(false, false, true, false, false);

  } else if ((byte1 == 0xAD && byte2 == 0x11) || (byte1 == 0x11 && byte2 == 0xAD)) {
    Serial.println("Playing fan state audio...");
    handleAudioPlayback(false, false, false, true, false);

  } else if ((byte1 == 0xAC && byte2 == 0x11) || (byte1 == 0x11 && byte2 == 0xAC)) {
    Serial.println("Playing water pump state audio...");
    handleAudioPlayback(false, false, false, false, true);

  } else if ((byte1 == 0xFA && byte2 == 0x11) || (byte1 == 0x11 && byte2 == 0xFA)) {
    Serial.println("Turn on the fan ");
    digitalWrite(Fan_Led, HIGH);

  } else if ((byte1 == 0xFA && byte2 == 0x00) || (byte1 == 0x00 && byte2 == 0xFA)) {
    Serial.println("Turn off the fan .");
    digitalWrite(Fan_Led, LOW);

  } else if ((byte1 == 0xAA && byte2 == 0x11) || (byte1 == 0x11 && byte2 == 0xAA)) {
    Serial.println("Turn on the water pump");
    digitalWrite(Waterpump_Led, HIGH);

  } else if ((byte1 == 0xAA && byte2 == 0x00) || (byte1 == 0x00 && byte2 == 0xAA)) {
    Serial.println("Turn off the water pump.");
    digitalWrite(Waterpump_Led, LOW);

  } else if ((byte1 == 0xFA && byte2 == 0xAA) || (byte1 == 0xAA && byte2 == 0xFA)) {
    digitalWrite(LIGHT, HIGH);
    Serial.println("Turning On Light...");

  } else if ((byte1 == 0xFA && byte2 == 0xBB) || (byte1 == 0xBB && byte2 == 0xFA)) {
    digitalWrite(LIGHT, LOW);
    Serial.println("Turning OFF Light...");

  } else {
    Serial.println("Unknown command received.");
  }
}

void setupESPNow() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed!");
    setColor(255, 0, 255);  // Red for failure
    return;
  }
  isEspNowConnected = true;  // ESP-NOW connection established
  setColor(255, 0, 255);     // Green when ESP-NOW is connected
  Serial.println("ESP-NOW Initialized");
}

// Function to initialize setup (Make sure it's outside handleAudioPlayback function)
void setup() {
  Serial.begin(115200);
  VC02Serial.begin(9600, SERIAL_8N1, 16, 17);

  // Initialize RGB LED pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  setupESPNow();

  // Connect to WiFi

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    setColor(0, 0, 255);  // Yellow while connecting
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");


  // Set device as a Wi-Fi Station
  //WiFi.mode(WIFI_STA);

  pinMode(LIGHT, OUTPUT);
  pinMode(Fan_Led, OUTPUT);
  pinMode(Waterpump_Led, OUTPUT);

  digitalWrite(LIGHT, LOW);

  // Register peer
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Register receive callback
  esp_now_register_recv_cb(OnDataRecv);
}

// Function to execute main loop (Make sure it's outside handleAudioPlayback function)
void loop() {
  delay(300);  // Wait between readings
  if (VC02Serial.available() >= 2) {
    uint8_t dataToSend[2];
    dataToSend[0] = VC02Serial.read();  // Read the first byte
    dataToSend[1] = VC02Serial.read();  // Read the second byte

    Serial.print("Sending bytes to 1st board: ");
    Serial.print(dataToSend[1], HEX);
    Serial.print(" ");
    Serial.println(dataToSend[0], HEX);

    /* int byte1 = VC02Serial.read();
    int byte2 = VC02Serial.read();
   */

    // Handle commands based on received bytes
    processVC02Command(dataToSend[0], dataToSend[1]);  //byte1, byte2

    // Send data via ESP-NOW
    esp_err_t result = esp_now_send(receiverMAC, dataToSend, sizeof(dataToSend));
    if (result == ESP_OK) {
      Serial.println("Bytes sent successfully");
      Serial.println("");
    } else {
      Serial.println("Error sending bytes");
    }
  }

  delay(1000);

  // If Wi-Fi is connected and ESP-NOW is initialized, run colorful mode
  if (isWiFiConnected && isEspNowConnected) {
    // If it's time to change the color, do it
    if (millis() - lastColorChangeTime > colorCycleInterval) {
      lastColorChangeTime = millis();     // Reset timer
      cycleIndex = (cycleIndex + 1) % 6;  // Cycle through 6 colors

      // Switch colors based on cycleIndex
      switch (cycleIndex) {
        case 0: setColor(255, 0, 0); break;    // Red
        case 1: setColor(255, 165, 0); break;  // Orange
        case 2: setColor(255, 255, 0); break;  // Yellow
        case 3: setColor(0, 0, 255); break;    // Blue
        case 4: setColor(0, 255, 0); break;    // Green
        case 5: setColor(75, 0, 130); break;   // Indigo
      }
    }
  } else {
    // If either Wi-Fi or ESP-NOW is not connected, show a status color
    if (!isWiFiConnected) {
      setColor(255, 255, 0);  // Yellow while Wi-Fi is still connecting
    } else if (!isEspNowConnected) {
      setColor(255, 0, 0);  // Red while ESP-NOW is not connected
    }
  }
}
