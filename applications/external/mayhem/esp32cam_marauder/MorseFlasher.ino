// Created by ChatGPT, and symbols added by Bing Chat
const int LED = 4;
const int dotDuration = 350;
const int dashDuration = 3 * dotDuration;
const int interLetterPause = 2 * dotDuration;
const int interWordPause = 4 * dotDuration;
const int bufferSize = 50;  // max length of the input buffer

char* letters[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // A-I
  ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R
  "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." // S-Z
};

//For Numbers
char* numbers[] = {
  "-----", ".----", "..---", "...--", "....-", ".....",
  "-....", "--...", "---..", "----."
};

char* symbols[] = {
  ".-.-.-", "--..--", "..--..", ".----.", "-.-.--", "-..-.", // . , ? , etc.
  "-.--.", "-.--.-", ".-...", "---...", "-.-.-.", // ( ), :, etc.
  "-...-", ".-.-.", "--...-", ".-..-.", ".--.-.", // = , + , / , @ , etc.
  "..--.-", "-....-", // _ , -
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
    Serial.print("Message: ");

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
    if (Serial.available()) {
      Serial.println();
      break;
    }

    char input = inputBuffer[i];
    if (input == ' ' || (input >= 'a' && input <= 'z') || (input >= '0' && input <= '9')) {
      Serial.println();
      Serial.print(input);
      Serial.print(": ");

      const char* pattern;
      if (input >= 'a' && input <= 'z')
        pattern = letters[input - 'a'];
      else if (input >= '0' && input <= '9')
        pattern = numbers[input - '0'];
      else
        pattern = symbols[input - '.' + 6];

      if (pattern) {
        for (int j = 0; pattern[j]; j++) {
          if (pattern[j] == '.') dot();
          else if (pattern[j] == '-') dash();
        }
      } else {
        space();
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
