// Created by ChatGPT
const int LED = 4;
const int dotDuration = 350;
const int dashDuration = 3 * dotDuration;
const int interLetterPause = 2 * dotDuration;
const int interWordPause = 7 * dotDuration;
const int bufferSize = 50;  // max length of the input buffer

const char* morseAlphabet[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..",
  "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-",
  "-.--", "--.."
};

char inputBuffer[bufferSize + 1]; // input buffer
int bufferIndex = 0;           // current index in the input buffer

void morse_setup() {
  pinMode(LED, OUTPUT);
  //Serial.begin(9600);
}

void morse_loop() {
  if (Serial.available()) {
    bufferIndex = 0;
    Serial.print("Received:");

    while (Serial.available()) {
      char input = Serial.read();
      Serial.print(input);

      // add the input to the buffer
      inputBuffer[bufferIndex] = tolower(input);
      bufferIndex = (bufferIndex + 1) % bufferSize;
      if (input == 10 || input == 13) {
        inputBuffer[bufferIndex++] = ' ';
        Serial.flush();
        break;
      }
    }
  }

  // repeat the last input forever
  for (int i = 0; i < bufferIndex; i++) {
    char input = inputBuffer[i];
    if (input == ' ' || (input >= 'a' && input <= 'z') || (input >= '0' && input <= '9')) {
      Serial.print(input);
      if (input >= 'a' && input <= 'z') {
        const char* pattern = morseAlphabet[input - 'a'];
        for (int j = 0; pattern[j]; j++) {
          if (pattern[j] == '.') {
            dot();
          } else if (pattern[j] == '-') {
            dash();
          }
        }
      } else if (input >= '0' && input <= '9') {
        int num = input - '0';
        for (int j = 0; j < num; j++) {
          dot();
          delay(dotDuration);
        }
      } else {
        space();
        //delay(interWordPause);
      }
      shortspace();
    }
  }
}

void dot() {
  Serial.print(".");
  digitalWrite(LED, HIGH);
  delay(dotDuration);
  digitalWrite(LED, LOW);
  delay(dotDuration);
}

void dash() {
  Serial.print("-");
  digitalWrite(LED, HIGH);
  delay(dashDuration);
  digitalWrite(LED, LOW);
  delay(dotDuration);
}

void shortspace() {
  delay(interLetterPause);
}

void space() {
  digitalWrite(LED, LOW);
  delay(interWordPause);
}
