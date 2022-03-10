/*
  Serial Event example

  When new serial data arrives, this sketch adds it to a String.
  When a newline is received, the loop prints the string and clears it.

  A good test for this is to try it with a GPS receiver that sends out
  NMEA 0183 sentences.

  NOTE: The serialEvent() feature is not available on the Leonardo, Micro, or
  other ATmega32U4 based boards.

  created 9 May 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/SerialEvent
*/

String inputString = "";         // a String to hold incoming data
String outputString = "";
bool stringComplete = false;  // whether the string is complete
long valuesArray1[100] = {0};
bool acquiring = false;

void setup() {
  // Initialize serial:
  Serial.begin(9600);
  // Reserve 200 bytes for the inputString:
  inputString.reserve(200);
  pinMode(13, OUTPUT);
}

void loop() {
  
  // Serial.available() only needed in ESP, micro and nano.
  /*if(Serial.available()){
    serialEvent();
  }*/ 
  
  if(inputString.startsWith("START_OK")){
    acquiring = true;
    inputString = "";
  }
  
  if(acquiring){   
   for(int i=0; i<99; i++){
    digitalWrite(13,!digitalRead(13));
    outputString += analogRead(A0)/4;
    outputString += ";";
    delay(55);
   }
   outputString += analogRead(A0);
   Serial.println(outputString);
   outputString = "";
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      Serial.println(inputString);
      inputString = "";

    }
  }
}
