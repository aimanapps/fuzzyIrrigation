#include <Wire.h>
#include <NewPing.h>

#define RELAY         4
#define TRIGGER_PIN   12  
#define ECHO_PIN      11  
#define MAX_DISTANCE  200 
#define SLAVE_ADDRESS 0x04

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
double number = 0;
double number1 = 0;
double number2 = 0;
int array[100];

long pertama;
long kedua;
int jarak;
int i=0;
 
void setup() {
  Serial.begin(115200);
  pinMode(RELAY, OUTPUT);

  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  }
 
void loop() {
  delay(5000);                   
  Serial.print("Ping: ");
  Serial.println(sonar.ping_cm());
   
  digitalWrite(RELAY,LOW); //anomali - high
  delay(1000);
  jarak = sonar.ping_cm();
  
  Serial.print("Number2: ");
  Serial.println(number2); 

  if (number2!=0){
    digitalWrite(RELAY,HIGH); //anomali -low
    delay(number2);
    digitalWrite(RELAY,LOW); //anomali - high
    number2 = 0;
    i=0;
  }
}

void receiveData(int byteCount){
  number = Wire.read();
  array[i++] = number;
  hitung();
}

void hitung(){
  pertama = array[0]*255;
  Serial.println(pertama);
  kedua = array[1];
  Serial.println(kedua);
  number2 = (pertama + kedua) * 1000;
}

void sendData(){
  Wire.write(jarak);
}
