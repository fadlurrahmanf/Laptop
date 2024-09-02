#define OUT1OFF PB6  //Output1
#define OUT1ON PB7   //Output1
#define OUT2OFF PB8  //Output2
#define OUT2ON PB9   //Output2
#define LED PC13

void blinkLed(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(OUT1OFF, OUTPUT);
  pinMode(OUT1ON, OUTPUT);
  pinMode(OUT2OFF, OUTPUT);
  pinMode(OUT2ON, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(OUT1OFF, HIGH);
  blinkLed(1);
  delay(3000);
  digitalWrite(OUT1OFF, LOW);

  digitalWrite(OUT1ON, HIGH);
  blinkLed(2);
  delay(3000);
  digitalWrite(OUT1ON, LOW);

  digitalWrite(OUT2OFF, HIGH);
  blinkLed(3);
  delay(3000);
  digitalWrite(OUT2OFF, LOW);

  digitalWrite(OUT2ON, HIGH);
  blinkLed(4);
  delay(3000);
  digitalWrite(OUT2ON, LOW);
}
