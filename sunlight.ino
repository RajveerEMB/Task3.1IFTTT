#include <WiFiNINA.h>

char ssid[] = "12345678";
char pass[] = "12345678";
const char* webhookUrl = "/trigger/sunlight_alert/with/key/inE9LYrR33VIVO4bHklquzgYacVVtpPMaozwNRMB0Tn";

WiFiClient client;

// Pin connected to the LM393 digital output
const int LIGHT_SENSOR_PIN = 2;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize the light sensor pin
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  
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
  // Read the light sensor value (HIGH for light, LOW for dark)
  int lightValue = digitalRead(LIGHT_SENSOR_PIN);
  Serial.print("Light Sensor State: ");
  Serial.println(lightValue == HIGH ? "Bright" : "Dark");

  // Check if the light value is high (light detected)
  if (lightValue == HIGH) {
    sendIFTTTNotification("Bright");
  } else {
    sendIFTTTNotification("Dark");
  }

  // Wait before the next check
  delay(60000);
}

void sendIFTTTNotification(String message) {
  String queryString = "?value1=" + message;

  // Connect to IFTTT and send an HTTP GET request
  if (client.connect("maker.ifttt.com", 80)) {
    Serial.println("Sending notification...");
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
  } else {
    Serial.println("Failed to connect to IFTTT server");
  }
}
