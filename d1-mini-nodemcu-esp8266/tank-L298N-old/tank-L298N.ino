#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "orange";
const char* password = "87511222333";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int output5 = 5;
const int output4 = 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


int curSpeed = 200;

int ENA = 5;
int IN1 = 3;
int IN2 = 4;

int ENB = 8;
int IN3 = 6;
int IN4 = 7;


// int back_led = 13;
// int front_led = 12;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);

  // Connect to Wi-Fi network with SSID and password
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
  
  // Back motor
//  pinMode(IN1, OUTPUT);
//  pinMode(IN2, OUTPUT);
  
  // Front motor
//  pinMode(IN3, OUTPUT);
//  pinMode(IN4, OUTPUT);

  // back and front leds 
  // pinMode(back_led, OUTPUT);
  // pinMode(front_led, OUTPUT);
}

void loop() {

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
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
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 12px; margin: 1px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h2>ESP8266 Web Server</h2>");

            // Create a hidden 1x2 table
            client.println("<center><table border=1>");
            client.println("<tr>");

            // Left controller
            client.println("<td>");
            
            // Create a hidden 3x3 table
            client.println("<center><table border=1>");
            
            // Row 1
            client.println("<tr>");
            client.println("<td>&nbsp;</td>");
            client.println("<td>");
            // Display current state, and ON/OFF buttons for GPIO 5  
            //client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<a href=\"/5/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/5/off\"><button class=\"button button2\">OFF</button></a>");
            } 
            client.println("</td>");
            client.println("<td>&nbsp;</td>");
            client.println("</tr>");
            
            // Row 2
            client.println("<tr>");
            client.println("<td>");
            client.println("<a href=\"/4/on\"><button class=\"button\">ON</button></a>");
            client.println("</td>");
            client.println("<td>&nbsp;");
            client.println("</td>");
            client.println("<td>");
            client.println("<a href=\"/4/on\"><button class=\"button\">ON</button></a>");
            client.println("</td>");
            client.println("</tr>");

            // Row 3
            client.println("<tr>");
            client.println("<td>&nbsp;</td>");
            client.println("<td>");
            // Display current state, and ON/OFF buttons for GPIO 4  
            //client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<a href=\"/4/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/4/off\"><button class=\"button button2\">OFF</button></a>");
            }
            client.println("</td>");
            client.println("<td>&nbsp;</td>");
            client.println("</tr>");
            client.println("</table>");
            client.println("</td>");


            // Right controller
            client.println("<td>");
            
            // Create a hidden 3x3 table
            client.println("<center><table border=1>");
            
            // Row 1
            client.println("<tr>");
            client.println("<td>&nbsp;</td>");
            client.println("<td>");
            // Display current state, and ON/OFF buttons for GPIO 5  
            //client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<a href=\"/5/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/5/off\"><button class=\"button button2\">OFF</button></a>");
            } 
            client.println("</td>");
            client.println("<td>&nbsp;</td>");
            client.println("</tr>");
            
            // Row 2
            client.println("<tr>");
            client.println("<td>");
            client.println("<a href=\"/4/on\"><button class=\"button\">ON</button></a>");
            client.println("</td>");
            client.println("<td>&nbsp;");
            client.println("</td>");
            client.println("<td>");
            client.println("<a href=\"/4/on\"><button class=\"button\">ON</button></a>");
            client.println("</td>");
            client.println("</tr>");

            // Row 3
            client.println("<tr>");
            client.println("<td>&nbsp;</td>");
            client.println("<td>");
            // Display current state, and ON/OFF buttons for GPIO 4  
            //client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<a href=\"/4/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/4/off\"><button class=\"button button2\">OFF</button></a>");
            }
            client.println("</td>");
            client.println("<td>&nbsp;</td>");
            client.println("</tr>");
            client.println("</table>");
            client.println("</td>");
            
            client.println("</tr>");
            client.println("</table></center>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

  
// if(Serial.available() > 0){
//   char letter = Serial.read();
//   Serial.println(letter);
//
//   if(letter == 'F'){
//     Forward();
//   }
//   else if(letter == 'B'){
//     Backward();
//   }
//   else if(letter == 'R' || letter == 'I' || letter == 'J'){
//     TurnRight();
//   }
//   else if(letter == 'L' || letter == 'G' || letter == 'H'){
//     TurnLeft();
//   }
//   else if(letter == '1'){
//     ChangeSpeed(1);
//   }
//   else if(letter == '2'){
//     ChangeSpeed(2);
//   }
//
//   else if(letter == '3'){
//     ChangeSpeed(3);
//   }
//
//
//   else if(letter == '4'){
//     ChangeSpeed(4);
//   }
//
//
//   else if(letter == '5'){
//     ChangeSpeed(5);
//   }
//
//
//   else if(letter == '6'){
//     ChangeSpeed(6);
//   }
//
//
//   else if(letter == '7'){
//     ChangeSpeed(7);
//   }
//
//
//   else if(letter == '8'){
//     ChangeSpeed(8);
//   }
//
//
//   else if(letter == '9'){
//     ChangeSpeed(9);
//   }
//
//   else if(letter == 'S'){
//     Stop();
//   }
//
//   else if(letter == 'W'){
//     digitalWrite(13, HIGH);
//   }
//
//   else if(letter == 'w'){
//     digitalWrite(13, LOW);
//   }
//
//   else if(letter == 'U'){
//     digitalWrite(12, HIGH);
//   }
//
//   else if(letter == 'u'){
//     digitalWrite(12, LOW);
//   }
// }
}

void Forward()
{
 MotorB_Run(curSpeed);
}

void Backward()
{
 MotorB_Run(-curSpeed);
}

void TurnLeft()
{
 MotorF_Run(-1000);
}

void TurnRight()
{
 MotorF_Run(1000);
}

void Stop()
{
 MotorB_Run(0);
 MotorF_Run(0);
}

void ChangeSpeed(int spd)
{
 curSpeed = 100*spd;
}


void MotorB_Run(int spd){
  if (spd>0){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, spd);
  }else{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, -spd);
  }
}

void MotorF_Run(int spd){
  if (spd>0){
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, spd);
  }else{
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, -spd);
  }
}
