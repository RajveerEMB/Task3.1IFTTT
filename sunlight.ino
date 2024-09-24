#include <WiFiNINA.h>

// WiFi credentials
char ssid[] = "12345678";   // Your network SSID
char pass[] = "12345678";   // Your network password

// IFTTT webhook URL for notifications
const char* webhookUrl = "/trigger/sunlight_alert/with/key/inE9LYrR33VIVO4bHklquzgYacVVtpPMaozwNRMB0Tn";

// WiFi client object to connect to the IFTTT server
WiFiClient client;

// Pin connected to the LM393 digital light sensor output
const int LIGHT_SENSOR_PIN = 2;

void setup() {
  // Start the serial communication
  Serial.begin(9600);
  
  // Wait for the serial connection to be ready
  while (!Serial);
  
  // Initialize the light sensor pin as input
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  
  // Connect to the WiFi network
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  
  // Wait until the WiFi connection is established
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  
  // Test the connection to the IFTTT server
  if (client.connect("maker.ifttt.com", 80)) {
    Serial.println("Connected to IFTTT server");
    client.stop();  // Close the test connection
  } else {
    Serial.println("Connection to IFTTT server failed");
  }
}

void loop() {
  // Read the state of the light sensor (HIGH = bright, LOW = dark)
  int lightValue = digitalRead(LIGHT_SENSOR_PIN);
  Serial.print("Light Sensor State: ");
  Serial.println(lightValue == HIGH ? "Bright" : "Dark");

  // If the sensor detects bright light, send a notification
  if (lightValue == HIGH) {
    sendIFTTTNotification("Bright");
  } 
  // If the sensor detects darkness, send a notification
  else {
    sendIFTTTNotification("Dark");
  }

  // Wait for a minute before checking the light sensor again
  delay(60000);
}

void sendIFTTTNotification(String message) {
  // Create the query string to send to IFTTT (value1 parameter)
  String queryString = "?value1=" + message;

  // Connect to the IFTTT server and send an HTTP GET request
  if (client.connect("maker.ifttt.com", 80)) {
    Serial.println("Sending notification...");
    
    // Send the HTTP GET request to the server
    client.println("GET " + String(webhookUrl) + queryString + " HTTP/1.1");
    client.println("Host: maker.ifttt.com");
    client.println("Connection: close");
    client.println(); // End of the HTTP header

    // Read the server response
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();  // Read the server's response character by character
        Serial.print(c);         // Print the response to the serial monitor
      }
    }
    
    // Disconnect from the server once the response is received
    client.stop();
    Serial.println();
    Serial.println("Disconnected from server");
  } 
  // If the connection fails, print an error message
  else {
    Serial.println("Failed to connect to IFTTT server");
  }
}
