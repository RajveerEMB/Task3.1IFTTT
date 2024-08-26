#include <Wire.h>
#include <WiFiNINA.h>
#include <BH1750FVI.h>

char ssid[] = "Rajveer";
char pass[] = "12345678";
const char* webhookUrl = "/trigger/sunlight_alert/with/key/inE9LYrR33VIVO4bHklquzgYacVVtpPMaozwNRMB0Tn";

WiFiClient client;

// Create an instance of the BH1750 object
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

// Define the light threshold in Lux
const int lightThreshold = 400;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize I2C communication for the light sensor
  Wire.begin();
  
  // Initialize BH1750 sensor
  LightSensor.begin();
  
  // Connect to WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Test connection to the IFTTT server
  if (client.connect("maker.ifttt.com", 80)) {
    Serial.println("Connected to IFTTT server");
    client.stop();  // Close the test connection
  } else {
    Serial.println("Connection to IFTTT server failed");
  }
}

void loop() {
  // Read the light sensor value in Lux
  int lightValue = LightSensor.GetLightIntensity();
  Serial.print("Light Sensor Value (Lux): ");
  Serial.println(lightValue);

  // Check if the light value exceeds the threshold
  if (lightValue > lightThreshold) {
    sendIFTTTNotification("Sunlight detected on the terrarium!");
  } else {
    sendIFTTTNotification("Sunlight stopped.");
  }

  // Wait before the next check
  delay(60000);  // 60 seconds delay
}

void sendIFTTTNotification(String message) {
  String queryString = "?value1=" + message;

  // Make an HTTP GET request to the IFTTT webhook URL
  client.println("GET " + String(webhookUrl) + queryString + " HTTP/1.1");
  client.println("Host: maker.ifttt.com");
  client.println("Connection: close");
  client.println(); // End of HTTP header

  // Read the server response
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
  }

  // Disconnect from the server
  client.stop();
  Serial.println();
  Serial.println("Disconnected from server");
}