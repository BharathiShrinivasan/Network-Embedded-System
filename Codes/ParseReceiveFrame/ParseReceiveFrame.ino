#include <SoftwareSerial.h>
SoftwareSerial xbeeSerial(2,3); //RX, TX

char LEDCommand[5]={0}; // LED_ON, LED_OFF

byte TempSendFrame[27]={0x7E,0x00,0x16,0x10,0x01,0x00,0x13,0xA2,0x00,0x41,0x7C,0x20,0xD3,0xFF,0xFE,0x00,0x00,0x54,0x45,0x4D,0x50,0x3A,0x58,0x58,0x25,0x36,0X0D};
uint8_t frame_send[27]={0x7E,0x00,0x16,0x10,0x01,0x00,0x13,0xA2,0x00,0x41,0x7C,0x20,0xD3,0xFF,0xFE,0x00,0x00,0x52,0x4F,0x55,0x54,0x45,0x52,0x5F,0x32,0x1A,0x0D};

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 xbeeSerial.begin(9600);
 pinMode(13,OUTPUT);
 xbeeSerial.write(frame_send, 27);
 
}

unsigned long StartTime=0;

void loop() {
  StartTime=millis();
  
  while(millis()-StartTime<5000){
  // put your main code here, to run repeatedly:
 if(xbeeSerial.available() > 0){
    byte StartDelimiter = xbeeSerial.read();
    if(StartDelimiter==0x7E){
      ReadFrame();
      }
    //Serial.print(input,HEX); Serial.print('\t');
 }

  }

  SendTempReading();  
}

void ReadFrame()
{
  byte ReadData[100]={0};
  byte ReadLength[4]; // xx,xx,<frame_1byte>
  xbeeSerial.readBytes(ReadLength,3);

  if(ReadLength[2]==0x90){ // receive frame
    
      xbeeSerial.readBytes(ReadData,ReadLength[1]);
    
      for(int i=0;i<ReadLength[1]-12;i++)
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

  if(ReadLength[2]==0x8B) { // Transmit status
    
    xbeeSerial.readBytes(ReadData,ReadLength[1]);
    
    switch(ReadData[4]){ // Tx Status
      case(0x00): Serial.println("Transmit Success");break;
      default: Serial.println("Transmit FAILURE");break;
    }
    switch(ReadData[5]){ // node disc overhead
      case(0x00): Serial.println("No discovery");break;
      default: Serial.println("Discovery route");break;
    }
    
  }
    
  if(ReadLength[2]==0x8A) { // modum status
    
    xbeeSerial.readBytes(ReadData,ReadLength[1]);

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
    digitalWrite(13,LEDCommand[4]=='1'); // Fire detection LED
  }
}

void SendTempReading()
{
  int Temp_tensDigit= random(0,9);
  int Temp_onesDigit= random(0,9);

  // int x=56;
  // x%10=6; x/10=5;
  //TempSendFrame[22]='0'+(x/10);
  //TempSendFrame[22]='0'+(x%10);
  
  TempSendFrame[22]='0'+Temp_tensDigit;
  TempSendFrame[23]='0'+Temp_onesDigit;

  byte checkSum=0;
  checkSum=0xFF-(0x08+TempSendFrame[22]+TempSendFrame[23]);
  TempSendFrame[25]=checkSum;

  xbeeSerial.write(TempSendFrame, 27);
  Serial.print("Temp=");Serial.print(Temp_tensDigit);Serial.println(Temp_onesDigit);
  /*for(int i=0;i<27;i++){
    Serial.print(TempSendFrame[i],HEX); Serial.print('\t');
  }*/
  
  
  
}
