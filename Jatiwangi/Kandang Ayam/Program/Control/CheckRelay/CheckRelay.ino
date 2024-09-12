#define K1 12
#define K2 13
#define K3 14
#define K4 16
void setup() {
  // put your setup code here, to run once:
  pinMode(K1, OUTPUT);
  pinMode(K2, OUTPUT);
  pinMode(K3, OUTPUT);
  pinMode(K4, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(K1, HIGH);
  Serial.println("Nyala -------------- 1");
  delay(3000);
  digitalWrite(K1, LOW);

  digitalWrite(K2, HIGH);
  Serial.println("Nyala -------------- 2");
  delay(3000);
  digitalWrite(K2, LOW);

  digitalWrite(K3, HIGH);
  Serial.println("Nyala -------------- 3");
  delay(3000);
  digitalWrite(K3, LOW);

  digitalWrite(K4, HIGH);
  Serial.println("Nyala -------------- 4");
  delay(3000);
  digitalWrite(K4, LOW);
  delay(10000);
}
