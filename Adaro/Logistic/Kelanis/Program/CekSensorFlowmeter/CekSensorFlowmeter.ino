#define SENSOR 14
int counter = 0;
bool sta = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(SENSOR,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Sensor : ");
  Serial.print(digitalRead(SENSOR));
  Serial.print(" - ");
  Serial.print("Counter : ");
  Serial.println(counter);
  if(digitalRead(SENSOR)){
    sta = true;
  }else{
    if(sta){
      sta = false;
      counter++;
    }
  }
}
