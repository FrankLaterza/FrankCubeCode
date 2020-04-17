
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX




/// variables ///

#define ZBUTTON 4
#define ABUTTON 1
#define BBUTTON 0
#define XBUTTON 5
#define YBUTTON 6
#define TLBUTTON 7
#define TRBUTTON 8
#define STARTBUTTON 9
#define DUBUTTON 10
#define DRBUTTON 11
#define DDBUTTON 12
#define DLBUTTON 13
#define X1AXIS A0
#define Y1AXIS A1
#define X2AXIS A2
#define Y2AXIS A3

// Vars for buttons
uint32_t btnState = 0;
int x1 = 0;
int y1 = 0;
int x2 = 0;
int y2 = 0;
int length = 0;
bool Z, A, B, X, Y, TL, TR, START, DU, DR, DD, DL;

// vars for smoothing
const int numReadings = 8;
int readings1[numReadings], readings2[numReadings], readings3[numReadings], readings4[numReadings];                    // the readings from the analog input
int readIndex1 = 0,readIndex2 = 0,readIndex3 = 0,readIndex4 = 0;  // the index of the current reading
int total1 = 0,total2 = 0,total3 = 0,total4 = 0;                  // the running total
int average1 = 0,average2 = 0,average3 = 0,average4 = 0;                // the average



void setup() {
  //Serial.begin(9600);     // If you want to see the button inputs then uncomment this along with "printData();" in the void loop
  mySerial.begin(115200);

  // set up for buttons and analog 
  pinMode(ZBUTTON, INPUT_PULLUP);
  pinMode(ABUTTON, INPUT_PULLUP);
  pinMode(BBUTTON, INPUT_PULLUP);
  pinMode(XBUTTON, INPUT_PULLUP);
  pinMode(YBUTTON, INPUT_PULLUP);
  pinMode(TLBUTTON, INPUT_PULLUP);
  pinMode(TRBUTTON, INPUT_PULLUP);
  pinMode(STARTBUTTON, INPUT_PULLUP);
  pinMode(DUBUTTON, INPUT_PULLUP);
  pinMode(DRBUTTON, INPUT_PULLUP);
  pinMode(DDBUTTON, INPUT_PULLUP);
  pinMode(DLBUTTON, INPUT_PULLUP);
  pinMode(X1AXIS, INPUT);
  pinMode(Y1AXIS, INPUT);
  pinMode(X2AXIS, INPUT);
  pinMode(Y2AXIS, INPUT);

  for (int thisReading1 = 0; thisReading1 < numReadings; thisReading1++) {readings1[thisReading1] = 0;}
  for (int thisReading2 = 0; thisReading2 < numReadings; thisReading2++) {readings2[thisReading2] = 0;}
  for (int thisReading3 = 0; thisReading3 < numReadings; thisReading3++) {readings3[thisReading3] = 0;}
  for (int thisReading4 = 0; thisReading4 < numReadings; thisReading4++) {readings4[thisReading4] = 0;}
}



void loop() {
  btnState = 0;   
  delay(16.2);     // this delay is for timing
  getData();
  smoothing();
  pressCheck();
  transmitData();
  printData();       // this wont work unless you uncomment the Serial.begin(9600); in void setup //
}




///// VOIDS ///////


void getData(){
  // axis daya
  x1 = analogRead(X1AXIS);
  y1 = analogRead(Y1AXIS);
  x2 = analogRead(X2AXIS);
  y2 = analogRead(Y2AXIS);

  // button data //
  Z = digitalRead(ZBUTTON);
  A = digitalRead(ABUTTON);
  B = digitalRead(BBUTTON);
  X = digitalRead(XBUTTON);
  Y = digitalRead(YBUTTON);
  TL = digitalRead(TLBUTTON);
  TR = digitalRead(TRBUTTON);
  START = digitalRead(STARTBUTTON);
  DU = digitalRead(DUBUTTON);
  DR = digitalRead(DRBUTTON);
  DD = digitalRead(DDBUTTON);
  DL = digitalRead(DLBUTTON);
}

void smoothing(){
  // mapping
  x1 = (map(x1, 0, 800, 0, 254)) - 127;
  if(x1>=127){x1=127;}
  y1 = (map(y1, 0, 800, 0, 254)) - 127;
  if(y1>=127){y1=127;}
  x2 = (map(x2, 0, 800, 0, 254)) - 127;
  if(x2>=127){x2=127;}
  y2 = (map(y2, 0, 800, 0, 254)) - 127;
  if(y2>=127){y2=127;}

  // subtract the last reading
  total1 = total1 - readings1[readIndex1];
  total2 = total2 - readings2[readIndex2];
  total3 = total3 - readings3[readIndex3];
  total4 = total4 - readings4[readIndex4];
  // read from the sensor:
  readings1[readIndex1] = x1;
  readings2[readIndex2] = y1;
  readings3[readIndex3] = x2;
  readings4[readIndex4] = y2;
  // add the reading to the total
  total1 = total1 + readings1[readIndex1];
  total2 = total2 + readings2[readIndex2];
  total3 = total3 + readings3[readIndex3];
  total4 = total4 + readings4[readIndex4];
  // advance to the next position in the array
  readIndex1 = readIndex1 + 1;
  readIndex2 = readIndex2 + 1;
  readIndex3 = readIndex3 + 1;
  readIndex4 = readIndex4 + 1;

  // if we're at the end of the array
  if (readIndex1 >= numReadings) {readIndex1 = 0;}
  if (readIndex2 >= numReadings) {readIndex2 = 0;}
  if (readIndex3 >= numReadings) {readIndex3 = 0;}
  if (readIndex4 >= numReadings) {readIndex4 = 0;}
  // cacluate average
  x1 = total1 / numReadings;
  y1 = total2 / numReadings;
  x2 = total3 / numReadings;
  y2 = total4 / numReadings;
}


void pressCheck(){
  if (A == 0) {
    length  = 1;
    btnState |= (1 << 0);     // a button is set to bit 1 
  }
  if (B == 0) {
    length  = 1;
    btnState |= (1 << 1);   // b button is set to bit 2 ect.
  }
  if (X == 0) {
    length  = 1;
    btnState |= (1 << 3);
  }
  if (Y == 0) {
    length  = 1;
    btnState |= (1 << 4);
  }
  if (Z == 0) {                     
    length  = 1;
    btnState |= (1 << 5);
  }
  if (TL == 0) {
    length  = 1;
    btnState |= (1 << 8);
  }
  if (TR == 0) {
    length  = 1;
    btnState |= (1 << 9);
  }
  if (START == 0) {                        
    length  = 1;
    btnState |= (1 << 11);
  }
  if (DU == 0) {
    length  = 1;
    btnState |= (1 << 12);
  }
  if (DR == 0) {
    length  = 1;
    btnState |= (1 << 13);
  }
  if (DD == 0) {
    length  = 1;
    btnState |= (1 << 14);
  }
  if (DL == 0) {
    length  = 1;
    btnState |= (1 << 15);
  }
}

void transmitData(){
    if (length >= 0) {
    mySerial.write((uint8_t)0xFD);
    mySerial.write((uint8_t)0x06);
    //  1. X1/Y1 - Axis
    mySerial.write((uint8_t)-y1);
    mySerial.write((uint8_t)x1);
    //  2. X2/Y2 - Axis
    mySerial.write((uint8_t)x2);
    mySerial.write((uint8_t)-y2);
    uint8_t btnState1 = btnState & 0xFF;
    uint8_t btnState2 = (btnState >> 8) & 0xFF;
    // Buttons
    mySerial.write((uint8_t)btnState1);
    mySerial.write((uint8_t)btnState2);
  }

  else {
    mySerial.write((uint8_t)0xFD);
    mySerial.write((uint8_t)0x06);
    //  1. X1/Y1 - Axis
    //  1. X1/Y1 - Axis
    mySerial.write((uint8_t)-y1);
    mySerial.write((uint8_t)x1);
    //  2. X2/Y2 - Axis
    mySerial.write((uint8_t)x2);
    mySerial.write((uint8_t)-y2);
    // Buttons
    mySerial.write((uint8_t)0x00);
    mySerial.write((uint8_t)0x00);
  }
}

void printData(){
  Serial.print(Z);
  Serial.print("   ");
  Serial.print(A );
  Serial.print("   ");
  Serial.print(B);
  Serial.print("   ");
  Serial.print(X);
  Serial.print("   ");
  Serial.print(Y );
  Serial.print("   ");
  Serial.print(TL);
  Serial.print("   ");
  Serial.print(TR );
  Serial.print("   ");
  Serial.print(START);
  Serial.print("   "); 
  Serial.print(DU); 
  Serial.print("   ");
  Serial.print(DR);
  Serial.print("   ");
  Serial.print(DD);
  Serial.print("   ");
  Serial.println(DL );

  Serial.print(x1);
  Serial.print("   ");
  Serial.print(y1);
  Serial.print("   ");
  Serial.print(x2);
  Serial.print("   ");
  Serial.print(y2);
  Serial.println("   ");
}
