int pins[4] = {8, 9, 10, 11};

int seq[8][4] = {
  {1,0,0,0},
  {1,1,0,0},
  {0,1,0,0},
  {0,1,1,0},
  {0,0,1,0},
  {0,0,1,1},
  {0,0,0,1},
  {1,0,0,1}
};

void setup() {
  Serial.begin(9600);

  for(int i = 0; i < 4; i++) {
    pinMode(pins[i], OUTPUT);
  }

  Serial.println("Stepper test started");
}

void loop() {
  for(int s = 0; s < 8; s++) {
    for(int i = 0; i < 4; i++) {
      digitalWrite(pins[i], seq[s][i]);
    }
    delayMicroseconds(1000);  // 1000 microseconds = 1 millisecond
  }
}