int LEDpin = LED_BUILTIN;
void setup() {
  pinMode(LEDpin, OUTPUT);     // Initialize the LEDpin pin as an output
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LEDpin, LOW);// Turns the blue LED ON  (see video why)
  delay(300);// Wait for 300 milliseconds
  digitalWrite(LEDpin, HIGH);// Turns the blue LED OFF  (see video why)
  delay(300);// Wait for 300 milliseconds  
  

  digitalWrite(LEDpin, LOW);
  delay(300);
  digitalWrite(LEDpin, HIGH);
  delay(300); 

  digitalWrite(LEDpin, LOW);
  delay(1000);// Wait for a second (keep it ON)
  digitalWrite(LEDpin, HIGH);
  delay(1000); // Wait for a second (keep it OFF)

} 
