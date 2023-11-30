#define fsrpin 34
#define fsrpin2 32
#define Switch1 33
#define Switch2 25
int fsrreading; //Variable to store FSR value
int fsrreading2; //Variable to store FSR value
bool Detector1;
bool Detector2;

void setup() {
  Serial.begin(115200);
  pinMode(Switch1, INPUT);
  pinMode(Switch2, INPUT);
   
}

void loop() {
  Detector1 = digitalRead(Switch1);
  Detector2 = digitalRead(Switch2);
  Serial.print("Detector 1 = ");
  Serial.println(!Detector1);
  Serial.print("Detector 2 = ");
  Serial.println(!Detector2);
  fsrreading = analogRead(fsrpin);
  fsrreading2 = analogRead(fsrpin2);
  Serial.print("Objeto Peso 1 = ");
  Serial.print(fsrreading);
  if (fsrreading < 10) {
    Serial.println(" - Sin objeto");
  } else if (fsrreading < 200) {
    Serial.println(" - 25%");
  } else if (fsrreading < 600) {
    Serial.println(" - 50%");
  } else if (fsrreading < 900) {
    Serial.println(" - 75%");
  } else if (fsrreading < 1200) {
    Serial.println(" - 100%");
  } 
  else {
    Serial.println(" - Objeto demasiado pesado, recalibra");
  }
  Serial.print("Objeto Peso 2 = ");
  Serial.print(fsrreading2);
  if (fsrreading2 < 10) {
    Serial.println(" - Sin objeto");
  } else if (fsrreading2 < 200) {
    Serial.println(" - 25%");
  } else if (fsrreading2 < 600) {
    Serial.println(" - 50%");
  } else if (fsrreading2 < 900) {
    Serial.println(" - 75%");
  } else if (fsrreading2 < 1200) {
    Serial.println(" - 100%");
  } 
  else {
    Serial.println(" - Objeto demasiado pesado, recalibra");
  }
  delay(500); //Delay 500 ms.
}
