/*
BSD 3-Clause License

Copyright (c) 2026, Atte Nieminen

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Change these only if different pins are used (changing these may also require changes to the main code)
const byte GREEN_LEDS[] = {2, 3, 4};
const byte RGB_LED_RED = 5;
const byte RGB_LED_GREEN = 6;
const byte BUTTONS[] = {8, 9, 10, 11};
const byte BUZZER = 12;

// Free to change these constants based on your preferences
const byte ROUND_BLINKS_AMOUNT = 8; // Maximum amount for leds to blink per. round
const int LED_ON_TIME = 200; // In milliseconds
const int LED_DELAY_WAIT_TIME = 100; // In milliseconds
const int BUTTONS_PRESSES_TIME_LIMIT = 3200; // In milliseconds
const int BUTTON_DEBOUNCE_TIME = 170; // In milliseconds
const int BUZZER_FREQUENCY = 400; // In hertz
const int BUZZER_ON_TIME = 350; // In milliseconds

void blink_leds(byte blinked_leds[]);
void press_buttons(byte pressed_buttons[]);
bool check_results(const byte blinked_leds[], const byte pressed_buttons[]);


// Initialization
void setup() {
  Serial.begin(9600);

  for (byte i = 0;  i < 3; i++) {
    pinMode (GREEN_LEDS[i], OUTPUT); // Set green LEDs
  }

  pinMode(RGB_LED_RED, OUTPUT);
  pinMode(RGB_LED_GREEN, OUTPUT);

  for (byte i = 0; i < 4; i++) { 
    pinMode (BUTTONS[i], INPUT); // Set buttons
  }

  pinMode(BUZZER, OUTPUT);

  randomSeed(analogRead(0)); // A0 pin should be empty (no cable connected) for this
}

// For blinking LEDs and saving blinked LEDs to array
void blink_leds(byte blinked_leds[]) {
  byte led_index = 0;

  for (byte i = 0; i < ROUND_BLINKS_AMOUNT; i++) {
    byte led_number = random(0, 4); // Random LED number (0, 1, 2 or 3)
    byte led_randomness = random(0, 2); // 0 or 1

    if ((led_number < 3) && (led_randomness == 1)) { // For some green LED to be ON
      digitalWrite(GREEN_LEDS[led_number], HIGH);
      delay(LED_ON_TIME);
      digitalWrite(GREEN_LEDS[led_number], LOW);
      delay(LED_DELAY_WAIT_TIME);

      Serial.print("Green LED ON, order number: "); Serial.println(led_number + 1);
      blinked_leds[led_index] = (led_number + 1); // Save to index which green LED was ON
      led_index++;
    }
    else if ((led_number == 3) && (led_randomness == 1)) { // For RGB LED to show green light
      digitalWrite(RGB_LED_GREEN, HIGH);
      delay(LED_ON_TIME);
      digitalWrite(RGB_LED_GREEN, LOW);
      delay(LED_DELAY_WAIT_TIME);

      Serial.print("RGB LED ON green, order number: "); Serial.println(led_number + 1);
      blinked_leds[led_index] = (led_number + 1); // Save to index also
      led_index++;
    }
    else if ((led_number == 3) && (led_randomness == 0)) { // For RGB LED to show red light (just blink the LED)
      digitalWrite(RGB_LED_RED, HIGH);
      delay(LED_ON_TIME);
      digitalWrite(RGB_LED_RED, LOW);
      delay(LED_DELAY_WAIT_TIME);

      Serial.print("RGB LED ON red, order number: "); Serial.print(led_number + 1); Serial.println( " (not saved to array)");
    }
  }
}

// For button presses and saving pressed buttons to array
void press_buttons(byte pressed_buttons[]) {
  byte buttons_last_state[4] = {LOW}; // For checking buttons' last press state
  byte button_index = 0;
  unsigned long start_time = millis();
  unsigned long last_button_press_time = 0;

  while ((millis() - start_time) < BUTTONS_PRESSES_TIME_LIMIT) { // Time limit for pressing buttons
    for (byte i = 0; i < 4; i++) {
      byte button_current_state = digitalRead(BUTTONS[i]);

      if ((button_current_state == HIGH) && (buttons_last_state[i] == LOW) && ((millis() - last_button_press_time) >= BUTTON_DEBOUNCE_TIME)) {

        if (button_index < ROUND_BLINKS_AMOUNT) {
          Serial.print("Pressed button: "); Serial.println(i + 1);
          pressed_buttons[button_index] = (i + 1);
          button_index++;

          last_button_press_time = millis();
        } 
        else {
          Serial.print("Button "); Serial.print(i + 1); Serial.println( " press ignored (preventing buffer overflow)"); // Ignore button press
        }
      }

      buttons_last_state[i] = button_current_state;
    }
  }
}

// For checking and returning the results
bool check_results(const byte blinked_leds[], const byte pressed_buttons[]) {
  if (memcmp(blinked_leds, pressed_buttons, sizeof(blinked_leds)) == 0) {
    return true;
  } 
  else {
    return false;
  }
}

// Main program starts here
void loop() {
  // Initialize arrays
  byte blinked_leds[ROUND_BLINKS_AMOUNT] = {0};
  byte pressed_buttons[ROUND_BLINKS_AMOUNT] = {0};

  // Call functions
  Serial.print("----------"); Serial.print("NEW ROUND"); Serial.println("----------");
  blink_leds(blinked_leds);
  press_buttons(pressed_buttons);
  bool results = check_results(blinked_leds, pressed_buttons);

  if (results) {
    Serial.println("Round OK, buzzer will NOT play fail sound"); Serial.println();
  } 
  else {
    Serial.println("Round FAILED, buzzer will play fail sound"); Serial.println();
    tone(BUZZER, BUZZER_FREQUENCY);
    delay(BUZZER_ON_TIME);
    noTone(BUZZER);
  }
}
