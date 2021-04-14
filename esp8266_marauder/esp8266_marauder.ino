// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);

  delay(100);
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
}

// the loop function runs over and over again forever
void loop() {
  //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);                       // wait for a second
  //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  //delay(1000);                       // wait for a second

  if (Serial.available()) {
    String input = Serial.readString();

    input.trim();

    if (input == "PING") {
      Serial.println("ESP8266 Pong");

      digitalWrite(LED_BUILTIN, LOW);
      delay(1);
      digitalWrite(LED_BUILTIN, HIGH);
    }

    //Serial.println(input);
  }
  else
    delay(1);
}
