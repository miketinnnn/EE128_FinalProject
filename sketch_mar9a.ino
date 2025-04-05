#include <SPI.h>
char buff [255];
volatile byte indx;
volatile boolean process;
int length;
int nums[10] = {0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011, 0b1011011, 0b1011111, 0b1110000, 0b1111111, 0b1111011};
char a = ' ';
char b = ' ';

void outNum(int num) {
  digitalWrite(2, bitRead(nums[num], 6)); // A
  digitalWrite(3, bitRead(nums[num], 5)); // B
  digitalWrite(4, bitRead(nums[num], 4)); // C
  digitalWrite(5, bitRead(nums[num], 3)); // D
  digitalWrite(6, bitRead(nums[num], 2)); // E
  digitalWrite(7, bitRead(nums[num], 1)); // F
  digitalWrite(8, bitRead(nums[num], 0)); // G
}

void writeData(char a, char b) {
  if (a == ' ') {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, LOW);
    digitalWrite(A3, HIGH);
    outNum(0);
  }
  else {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, LOW);
    digitalWrite(A3, HIGH);
    outNum(a - '0');
  }
  delay(5);
  if (b == ' ') {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, HIGH);
    digitalWrite(A3, LOW);
    outNum(0);
  }
  else {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, HIGH);
    digitalWrite(A3, LOW);
    outNum(b - '0');
  }
  delay(5);
}
 
void setup (void) {
  Serial.begin (115200);
  pinMode(MISO, OUTPUT); // have to send on master in so it set as output
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A5, OUTPUT); // for testing
  SPCR |= _BV(SPE); // turn on SPI in slave mode
  indx = 0; // buffer empty
  process = false;
  SPI.attachInterrupt(); // turn on interrupt
}
 
ISR (SPI_STC_vect) // SPI interrupt routine 
{ 
  byte c = SPDR; // read byte from SPI Data Register

  if (indx < sizeof(buff)) {
    buff[indx++] = c; // save data in the next index in the array buff
    if (c == '\n') {
      digitalWrite(A5, HIGH); 
      buff[indx - 1] = 0; // replace newline ('\n') with end of string (0)
      delay(1000);
      digitalWrite(A5, LOW);
      process = true;
    }
  }   
}
 
void loop (void) {
  if (process) {
    process = false; //reset the process
    length = strlen(buff);
    Serial.println (buff); //print the array on serial monitor
    Serial.print(buff[length - 2]);
    Serial.println(buff[length - 1]);
    a = buff[length - 2];
    b = buff[length - 1];
    indx= 0; //reset button to zero
  }
  writeData(a, b);
}