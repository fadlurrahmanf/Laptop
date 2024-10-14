#define PUL 12
#define DIR 13
#define ENA 14

void setup() {
  Serial.begin(115200);
  pinMode(PUL, OUTPUT);  // set PUL as output
  pinMode(DIR, OUTPUT);  // set DIR as output
  pinMode(ENA, OUTPUT);  // set ENA as output
  digitalWrite(ENA, HIGH);
  delay(3000);
  digitalWrite(ENA, LOW);
  Serial.println("Start");
}

void loop() {
  if (Serial.available()) {
    String incomingCmd = Serial.readStringUntil('\n');  // Read incoming command until newline
    int steps = incomingCmd.toInt();                    // Convert the string to integer (assuming you send number of steps)

    if (steps > 0) {                                     // Check if steps is valid
      Serial.println("Incoming cmd: " + String(steps));  // Print the number of steps received
      digitalWrite(DIR, HIGH);                           // Set direction
      for (int i = 0; i < steps; i++)                    // Move for the number of steps
      {
        digitalWrite(PUL, HIGH);
        delayMicroseconds(500);  // Pulse width HIGH
        digitalWrite(PUL, LOW);
        delayMicroseconds(500);  // Pulse width LOW
      }
    } else {
      Serial.println("Invalid input!");
    }
  }
}
