void setup() {
  Serial.begin(115200);

  delay(100);
  
  Serial1.begin(115200);

  delay(100);
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  delay(100);

  Serial.println("Waiting for serial data...");
}

// the loop function runs over and over again forever
void loop() {
  if (Serial1.available()) {
    String input = Serial1.readString();

    input.trim();

    if (input == "Ping") {
      Serial1.println("A32U4 Pong");
      Serial.println("A32U4 Pong");
    }
      
    Serial.println(input);
    digitalWrite(LED_BUILTIN, HIGH);                      // wait for a second
    delay(1);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1);
  }
}
