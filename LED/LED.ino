#include "DigiKeyboardIT.h"

void setup() {
  pinMode(1, OUTPUT);
}

void loop() {
  digitalWrite(1, HIGH);
  DigiKeyboard.delay(1000);
  digitalWrite(1, LOW);
  DigiKeyboard.delay(1000);
  digitalWrite(1, HIGH);
  DigiKeyboard.delay(1000);
  DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT); // WIN + R (apro l'esegui di windows) (Run)
  DigiKeyboard.delay(500);
  DigiKeyboard.print("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!\"£$%&/()=\\'ì|?^,.-;:_òàùç°§è+é*[]@#{}");

  for (;;) { }
}
