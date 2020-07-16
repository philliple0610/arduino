// D1 Mini NodeMCU Based WIFI Controlled Car//

#define STEPPER_PIN_1 16              // GPIO16(D0)
#define STEPPER_PIN_2 5               // GPIO5(D1)
#define STEPPER_PIN_3 4               // GPIO4(D2)              
#define STEPPER_PIN_4 0               // GPIO0(D3)

int step_number = 0;

//#include <SoftwareSerial.h> 

// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <Ticker.h>

// Replace with your network credentials
const char* ssid     = "orange";
const char* password = "87511222333";

// Set web server port number to 80
WiFiServer server(80); //Creates a server that listens for incoming connections on the specified port.

// Variable to store the HTTP request
String header;

// Watchdog
Ticker secondTick;
volatile int watchdogCount = 0; //A variable, which its value can be changed by code outside the 
                                //scope of current code at any time. In this case, watchdogCount
                                //is changed by ISRwatchdog and the loop function.

const int MIN_SPEED  = 400;
const int MAX_SPEED  = 1400;
const int STEP_SPEED = 50;
const int TURN_SPEED = 1000;
const int BACK_SPEED = 600;
const int STOP_MULTIPLY = 2;
const int WATCHDOG_COUNT_MAX = 5;

int curSpeed = MIN_SPEED;

void ISRwatchdog() { //Interupt Service Routine Watchdog
  watchdogCount++;
  Serial.println(watchdogCount);
  if(watchdogCount >= WATCHDOG_COUNT_MAX) { //If the loop has not been invoked for a while then you reboot the board
    Serial.println();
    Serial.println("the watchdog licks!!!!!!!!!!!");
    ESP.reset(); //Restart the ESP board
  }
}

void setup() {
  Serial.begin(115200); //To connect to the serial moniter
  secondTick.attach(1, ISRwatchdog); // Registers the watchdog: 1st argument in seconds after which the ISRwathdog() executes
                                       // 1 (seconds) is the time interval between the invocations                                   

  // Initialize the output variables as outputs
  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);

  // Set outputs to LOW
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
  
  // Connect to Wi-Fi network with SSID and password
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); //Initializes the WiFi library's network settings
                              //and checks the current status
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Delay half second
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); //Gets and prints the WiFi shield's IP address
  server.begin(); //Tells the server to begin listening for incoming connections

}

void loop() {
  watchdogCount = 0;
  WiFiClient client = server.available();   // Get and listen for incoming clients

  if (client != 0) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // Process header/command
            if (header.indexOf("GET /F") >= 0) { // Forward
              Serial.println("F");
              if (curSpeed < MAX_SPEED) {
                curSpeed = curSpeed + STEP_SPEED;
              }
              Serial.println(curSpeed);
//              MotorB_Run(curSpeed);
            } else if (header.indexOf("GET /S") >= 0) {
              Serial.println("S");
              if (curSpeed > MIN_SPEED) {
                curSpeed = curSpeed - STOP_MULTIPLY * STEP_SPEED;
                Serial.println(curSpeed);
//                MotorB_Run(curSpeed);
              } else {
//                MotorB_Run(0);
              }
            } else if (header.indexOf("GET /B") >= 0) { // Reverse
              Serial.println("B");
//              MotorB_Run(-BACK_SPEED);
            } else if (header.indexOf("GET /L") >= 0) { // Left
              Serial.println("L");
//              MotorF_Run(TURN_SPEED);
                for(int a= 0; a < 100; a++){
                  OneStep(false);
                  delay(2);
                }
            } else if (header.indexOf("GET /M") >= 0) {
              Serial.println("M");
//              MotorF_Run(0);
            } else if (header.indexOf("GET /R") >= 0) {
              Serial.println("R");
//              MotorF_Run(-TURN_SPEED);
                for(int a = 0; a < 100; a++){
                  OneStep(true);
                  delay(2);
                } 
            } else {

              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();

              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              // CSS to style the on/off buttons
              // Feel free to change the background-color and font-size attributes to fit your preferences
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 12px; margin: 1px; cursor: pointer;}");
              client.println(".button2 { background-color: #195B6A; border: none; color: white; padding: 60px 80px; text-decoration: none; font-size: 12px; margin: 1px; cursor: pointer;}");
              client.println("</style>");
              client.println("</head>");

              // Web Page Heading
              client.println("<body>");

              client.println("<h2>Phillip's ESP8266 Controller using JavaScript and Ajax</h2>");
//              client.println("<button class=\"button\" type=\"button\" onclick=\"run('F')\">F</button>");
//              client.println("<button class=\"button\" type=\"button\" onclick=\"run('S')\">S</button>");
//              client.println("<button class=\"button\" type=\"button\" onclick=\"run('B')\">B</button>");
              client.println("<br>");
              client.println("<br>");
              client.println("<br>");
              client.println("<br>");
              client.println("<button class=\"button2\" type=\"button\" ontouchstart=\"turn('L')\" ontouchend=\"turn('M')\"></button>");
              client.println("<button class=\"button2\" type=\"button\" ontouchstart=\"turn('R')\" ontouchend=\"turn('M')\"></button>");

              client.println("<script>");
              client.println("function run(direction) {");
              client.println("  var xhttp = new XMLHttpRequest();");
              client.println("  xhttp.open('GET', direction, true);");
              client.println("  xhttp.send();");
              client.println("  return false;");
              client.println("}");

              client.println("function turn(direction) {");
              client.println("  var xhttp = new XMLHttpRequest();");
              client.println("  xhttp.open('GET', direction, true);");
              client.println("  xhttp.send();");

              client.println("  return false;");
              client.println("}");

              client.println("function sleepFor( sleepDuration ){");
              client.println("  var now = new Date().getTime();");
              client.println("  while(new Date().getTime() < now + sleepDuration){ /* do nothing */ } ");
              client.println("}");

              client.println("</script>");
              client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            }
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

}

void OneStep(bool motor_direction){
  if(motor_direction){
    switch(step_number){
      case 0:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
    } 
  }
  else{
    switch(step_number){
      case 0:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
     
    } 
  }
  step_number++;
  if(step_number > 3){
    step_number = 0;
  }
}
