#include <Wire.h>
#include "I2Cdev.h"
#include <MPU6050.h>
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define Sentidos 5
#define Direcaos 6

RF24 radio(9, 10);

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

Servo servo1;
Servo servo2;

//variaveis
int val1;
int val2;
int prevVal1;
int prevVal2;

const byte address[6] = "00001";
unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;
struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j2PotX;
  byte j2PotY;
  byte L3Button;
  byte L1Button;
  byte L2Button;
  byte R3Button;
  byte R1Button;
  byte R2Button;
};
Data_Package data; //Create a variable with the above structure

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Initialize MPU");
  mpu.initialize();
  Serial.println(mpu.testConnection () ? "Connected" : "Connection failed");

  servo1.attach(Sentidos);
  servo2.attach(Direcaos);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening(); //  Set the module as receiver
  resetData();
}

void loop() {
  // Check whether there is data to be received
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
    lastReceiveTime = millis(); // At this moment we have received the data
  }
  // Check whether we keep receving data, or we have a connection between the two modules
  currentTime = millis();
  if ( currentTime - lastReceiveTime > 1000 ) { // If current time is more then 1 second since we have recived the last data, that means we have lost connection
    resetData(); // If connection is lost, reset the data. It prevents unwanted behavior, for example if a drone has a throttle up and we lose connection, it can keep flying unless we reset the values
  }
  ////Teste 1///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /*Servo 1*/
  data.j2PotX = map(data.j2PotX, 127, 0, val1, 0);
  servo1.write(data.j2PotX);
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  val1 = map(ax, -17000, 17000, 0, 180);
  if (val1 != prevVal1)
  {
    servo1.write(val1);
    prevVal1 = val1;
  }
  /*Servo 2*/
  data.j2PotY = map(data.j2PotY, 127, 0, val2, 0);
  servo2.write(data.j2PotY);
  val2 = map(ay, -17000, 17000, 0, 199);
  if (val2 != prevVal2)
  {
    servo2.write(val2);
    prevVal2 = val2;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Print the data in the Serial Monitor
  Serial.print("j1PotX: ");
  Serial.print(data.j1PotX);
  Serial.print("; j1PotY: ");
  Serial.print(data.j1PotY);
  Serial.print("; L3: ");
  Serial.print(data.L3Button);
  Serial.print("; L1: ");
  Serial.print(data.L1Button);
  Serial.print("; L2: ");
  Serial.print(data.L2Button);
  Serial.print("; R3: ");
  Serial.print(data.R3Button);
  Serial.print("; R1: ");
  Serial.print(data.R1Button);
  Serial.print("; R2: ");
  Serial.print(data.R2Button);    
  Serial.print("; j2PotY: ");
  Serial.print(data.j2PotY);
  Serial.print("; j2PotX: ");
  Serial.println(data.j2PotX); 
}
void resetData() {
  // Reset the values when there is no radio connection - Set initial default values
  data.j1PotX = 127; // Values from 0 to 255. When Joystick is in resting position, the value is in the middle, or 127. We actually map the pot value from 0 to 1023 to 0 to 255 because that's one BYTE value
  data.j1PotY = 127;
  data.j2PotX = 127;
  data.j2PotY = 127;
  data.L3Button = 1; // data.j1Button = 1;
  data.L1Button = 1; // data.j2Button = 1;
  data.L2Button = 1; //data.button1 = 1;
  data.R3Button = 1; //data.button2 = 1;
  data.R1Button = 1; //data.button3 = 1;
  data.R2Button = 1; //data.button4 = 1;
}
