#include <Adafruit_MCP4725.h>

int incomingByte = 0;

struct KeyState {
  bool pressed;
  int note;
  int velocity;
  int index; //index 0 is most recently pressed
};

int pin_outputs[] = {3,4,5};
KeyState key_states[] = {{false, 0, 0, 2}, {false, 0, 0, 2}, {false, 0, 0, 2}};

Adafruit_MCP4725 voice1;
Adafruit_MCP4725 voice2;
Adafruit_MCP4725 voice3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(31250);

  voice1.begin(0x61);
  voice2.begin(0x60);
  voice3.begin(0x62);

  for (int i = 0; i < 3; i++) {
    pinMode(pin_outputs[i], OUTPUT);
  }
}

void loop() {
  if (Serial.available() > 0) {
      // read the incoming byte:

      incomingByte = Serial.read();
      // say what you got:

      if (incomingByte == 144) { //144 in binary corresponds to the "note on" message
        Serial.println("Channel 1 note on");

        while (Serial.available() == 0) {}
        int note = int(Serial.read());
        while (Serial.available() == 0) {}
        int velocity = int(Serial.read());
        key_pressed(note, velocity);
      }
      else if (incomingByte == 128) { //128 in binary corresponds to the "note off" message
        Serial.println("Channel 1 note off");

        while (Serial.available() == 0) {}
        key_let_go(int(Serial.read()));
      }

      update_trigger_outputs();

    }
}

void key_pressed(int note, int velocity) {
  for (int i = 0; i < 3; i++) {
    if (key_states[i].pressed == false) {
      key_states[i].pressed = true;
      key_states[i].note = note;
      key_states[i].velocity = velocity;

      switch (i) {
        case 0:
          voice1.setVoltage(note * 30,false);
          Serial.println("voice 1");
          break;
        case 1:
          voice2.setVoltage(note * 30,false);
          Serial.println("voice 2");
          break;
        case 2:
          voice3.setVoltage(note * 30,false);
          Serial.println("voice 3");
          break;
      }

      Serial.println(note * 30);

      for (int j = 0; j < 3; j++) {
        if (key_states[j].pressed) {
          key_states[j].index += 1;
        }
      }

      key_states[i].index = 0;

      return;
    }
  }

  // if we arrive at this line, all gates are high and we need to replace the longest held note with the new note

  for (int i = 0; i < 3; i++) {
    if (key_states[i].index == 2) { 
      key_states[i].note = note;
      key_states[i].velocity = velocity;
    }
  }

  for (int j = 0; j < 3; j++) {
    Serial.println(key_states[j].note);
  }
}

void key_let_go(int note) {
  for (int i = 0; i < 3; i++) {
    if (key_states[i].note == note) {
      key_states[i].pressed = false;
      key_states[i].note = 0;
      key_states[i].velocity = 0;
      return;
    }
  }
}

void update_trigger_outputs() {
  for (int i = 0; i < 3; i++) {
    KeyState current_keystate = key_states[i];

    if (current_keystate.pressed) {
      digitalWrite(pin_outputs[i], HIGH);
    }
    else {
      digitalWrite(pin_outputs[i], LOW);
    }
  }
}