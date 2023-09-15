
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Airtel_Uname";
const char* password = "Virgin@123";
int ledPin = 25;
int sensorPin = 34;
int ldrValue = 0;
WiFiServer server(80);

String header;

int ledStatus = 0;



unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}


void loop(){
  
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // turns the GPIOs on and off
            if (header.indexOf("GET /turnOn") >= 0) {
              ledStatus = 1;
            } else if (header.indexOf("GET /turnOff") >= 0) {
              ledStatus = 0;
            } else if (header.indexOf("GET /getSensorData") >= 0) {
              client.println(analogRead(sensorPin));
              client.println(ledStatus);
            }
            
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
      
    }
    
    header = "";
   
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  if(ledStatus) {
        if(analogRead(sensorPin) > 100) {
          digitalWrite(ledPin, LOW);
        } else {
          digitalWrite(ledPin, HIGH);
        }
      }
}

// HTTP get request
// void sendRequest() {
//   String URL = "http://192.168.1.18:80/";
//    HTTPClient http;

//    http.begin(URL.c_str());
      
//       // If you need Node-RED/server authentication, insert user and password below
//       //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
//       // Send HTTP GET request
//       int httpResponseCode = http.GET();
      
//       if (httpResponseCode>0) {
//         Serial.print("HTTP Response code: ");
//         Serial.println(httpResponseCode);
//         String payload = http.getString();
//         Serial.println(payload);
//       }
//       else {
//         Serial.print("Error code: ");
//         Serial.println(httpResponseCode);
//       }
//       // Free resources
//       http.end();
// }