/**************END Device ARDUINO****************************************************************************/
/**************Transmit periodic TEMP reading && FireAlart pushbutton ; Coord message -> Serial monitor  *************************************************/

#include <SoftwareSerial.h>
#include <DHT.h>
SoftwareSerial xbeeSerial(2,3); //RX, TX

int FireButton=8;

#define DHTTYPE DHT11
#define DHTPIN 4
DHT dht(DHTPIN, DHTTYPE);

//byte TempSendFrame[27]={0x7E,0x00,0x16,0x10,0x01,0x00,0x13,0xA2,0x00,0x41,0x7C,0x20,0xD3,0xFF,0xFE,0x00,0x00,0x54,0x45,0x4D,0x50,0x3A,0x58,0x58,0x25,0x36,0X0D}; // TEMP:XX%
byte HumiSendFrame[27] = {0x7E,0x00,0x16,0x10,0x01,0x00,0x13,0xA2,0x00,0x41,0x7C,0x20,0xD3,0xFF,0xFE,0x00,0x00,0x48,0x55,0x4D,0x49,0x3A,0x58,0x58,0x25,0x36,0X0D}; // HUMI:XX%
byte LedOnSendFrame[24]={0x7E,0x00,0x13,0x10,0x01,0x00,0x13,0xA2,0x00,0x41,0x7C,0x20,0xD3,0xFF,0xFE,0x00,0x00,0x4C,0x45,0x44,0x5F,0x31,0x27,0x0D}; // LED_1
byte LedOffSendFrame[24]={0x7E,0x00,0x13,0x10,0x01,0x00,0x13,0xA2,0x00,0x41,0x7C,0x20,0xD3,0xFF,0xFE,0x00,0x00,0x4C,0x45,0x44,0x5F,0x30,0x28,0x0D}; // LED_0

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 xbeeSerial.begin(9600);
 dht.begin();
 pinMode(FireButton,INPUT_PULLUP); // input button
}

unsigned long StartTime=0; bool fireConditionLatched=HIGH;

void loop() {
  StartTime=millis();
  
  while(millis()-StartTime<5000){ // Keep looping until 5sec
  // put your main code here, to run repeatedly:
 if(xbeeSerial.available() > 0){
    byte StartDelimiter = xbeeSerial.read();
    if(StartDelimiter==0x7E){
      ReadFrame();
      }
    //Serial.print(input,HEX); Serial.print('\t');
 }

 if(digitalRead(FireButton)==LOW) // fire triggered/suppressed
 {
    fireConditionLatched=not(fireConditionLatched);
    if(fireConditionLatched==LOW){
      xbeeSerial.write(LedOnSendFrame, 24);
      Serial.println("LED ON");
      }
    else {
      xbeeSerial.write(LedOffSendFrame, 24);
      Serial.println("LED OFF");
    }
    delay(500);
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


void SendTempReading()
{
 // int Temp_tensDigit= random(0,9);
 // int Temp_onesDigit= random(0,9);
  int humidity = dht.readHumidity();
  // int x=56;
  // x%10=6; x/10=5;
  //TempSendFrame[22]='0'+(x/10);
  //TempSendFrame[22]='0'+(x%10);
  
//  TempSendFrame[22]='0'+(humidity/10);
//  TempSendFrame[23]='0'+(humidity%10);

  HumiSendFrame[22]='0'+(humidity/10);
  HumiSendFrame[23]='0'+(humidity%10);

  byte checkSum=0;
  //checkSum=0xFF-(0x08+HumiSendFrame[22]+HumiSendFrame[23]);
  checkSum=0xFF-(0x05+HumiSendFrame[22]+HumiSendFrame[23]); // Checksum calc
  HumiSendFrame[25]=checkSum;

  xbeeSerial.write(HumiSendFrame, 27);
  Serial.print("Humidity=");Serial.println(humidity);//Serial.println((humidity%10)); 
  
  
}
