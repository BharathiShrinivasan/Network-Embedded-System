/**************COORDINATOR ARDUINO****************************************************************************/
/**************Receive frame -> Serial monitor && Fire Buzzer*************************************************/

#include <SoftwareSerial.h>
SoftwareSerial xbeeSerial(2,3); //RX, TX

int FireBuzzerPin=13;

char LEDCommand[5]={0}; // LED_ON, LED_OFF

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 xbeeSerial.begin(9600);
 pinMode(FireBuzzerPin,OUTPUT); // Fire detection Buzzer

 Serial.println("Ready radio ON");
 
}


void loop() {

  // put your main code here, to run repeatedly:
 if(xbeeSerial.available() > 0){
    byte StartDelimiter = xbeeSerial.read();
    if(StartDelimiter==0x7E){
      ReadFrame();
      }
 }

}

void ReadFrame()
{
  byte ReadData[100]={0};
  byte ReadLength[4]; // xx,xx,<frame_1byte>
  xbeeSerial.readBytes(ReadLength,3);

  unsigned int ReadDataLength=ReadLength[0]*256+ReadLength[1];

  if(ReadLength[2]==0x90){ // receive frame
    
      xbeeSerial.readBytes(ReadData,ReadDataLength);
    
      for(int i=0;i<ReadDataLength-12;i++)
      {
        Serial.print((char)ReadData[i+11]); // print Message received
      }
      Serial.println();
      
      for(int i=0;i<5;i++)
      {
        LEDCommand[i]=(char)ReadData[i+11]; //slice 5 char for LED commanding
      }
      
      LEDCommandRoutine();
    
    }

  /*if(ReadLength[2]==0x8B) { // Transmit status
    
    xbeeSerial.readBytes(ReadData,ReadDataLength);
    
    switch(ReadData[4]){ // Tx Status
      case(0x00): Serial.println("Transmit Success");break;
      default: Serial.println("Transmit FAILURE");break;
    }
    switch(ReadData[5]){ // node disc overhead
      case(0x00): Serial.println("No discovery");break;
      default: Serial.println("Discovery route");break;
    }
    
  }*/
    
  if(ReadLength[2]==0x8A) { // modum status
    
    xbeeSerial.readBytes(ReadData,ReadDataLength);

    switch(ReadData[0]){
      case 0x02: Serial.println("Assosiated with network");break;
      case 0x03: Serial.println("Disassosiated from network");break;
      case 0x06: Serial.println("Coord started");break;
    }
  }

}

void LEDCommandRoutine()
{
  if(LEDCommand[0]=='L' && LEDCommand[1]=='E' && LEDCommand[2]=='D' && LEDCommand[3]=='_')
  {
    digitalWrite(FireBuzzerPin,LEDCommand[4]=='1'); // Fire detection LED
    if(LEDCommand[4]=='1')Serial.println("FIRE DETECTED!");
  }
}
