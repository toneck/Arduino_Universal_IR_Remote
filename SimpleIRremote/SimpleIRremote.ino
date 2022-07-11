
//#define OUTPUT_INVERTED

#define OUTPUT_PIN 5
#define INPUT_PIN 8
#define GND_PIN 7
#define VCC_PIN 6
#define VCC_PULL_UP_PIN 4


#if defined(ESP8266)
#define OUTPUT_PIN D4
#define INPUT_PIN D6
#define GND_PIN D7
#define VCC_PIN D8
#define VCC_PULL_UP_PIN D2
#endif

#include <Arduino.h>


#include "SimpleIRremote.h"

SimpleIRdata d;
SimpleIRremote remote;

char readChar();

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nBegin");

  pinMode(OUTPUT_PIN,OUTPUT);
  pinMode(INPUT_PIN,INPUT_PULLUP);
  pinMode(GND_PIN,OUTPUT);
  pinMode(VCC_PIN,OUTPUT);
  pinMode(VCC_PULL_UP_PIN,INPUT_PULLUP);
//  
//  
  #ifdef OUTPUT_INVERTED
  digitalWrite(OUTPUT_PIN,HIGH);
  #else
  digitalWrite(OUTPUT_PIN,LOW);
  #endif  
  digitalWrite(INPUT_PIN,HIGH);
  digitalWrite(GND_PIN,LOW);
  digitalWrite(VCC_PIN,HIGH);

  remote.setReceivingPin(INPUT_PIN);
  remote.setSendingPin(OUTPUT_PIN);
  #ifdef OUTPUT_INVERTED
  remote.setPinInverted(true);
  #endif
  remote.setMaximumCompressionLevel(1);
  
//  d=*((typeof(d)*)malloc(sizeof(SimpleIRdata)));
  d.frequency=38;
  d.number_of_repeats=2;
  
//    uint8_t data[]={0,33,17,33,33,17,17,33,17,17,33,17,49,36,33,20,33,33,49,33,33,33,33,33,33,49,33,49,49,49,49,49,49,33,33,49,49,17,17,17,17,17,17,33,33,33,33,33,33,81,0,33,17,33,33,17,17,33,17,17,33,17,17,33,33,17,33,33,17,33,33,33,33,33,33,17,33,17,17,17,17,17,17,33,33,17,17,17,17,17,17,17,17,33,33,33,33,33,33,1};
    uint8_t data[]={0,33,17,33,33,17,17,33,17,17,33,17,17,33,33,17,33,33,49,33,33,33,33,33,33,49,33,17,17,17,17,17,17,33,33,17,17,17,17,17,17,17,17,33,33,33,33,33,33,81};
    uint16_t vals[]={4386,531,1589,462,639,5223};
    d.compressed_data=data;
    d.symbols_values=vals;
    d.number_of_symbols=6;
    
    d.compression_level=1;
    d.number_of_uncompressed_values=100;
    d.number_of_compressed_values=50;
    remote.setData(d);
//    remote.send();
//    delay(1000);

  
pinMode(D3,OUTPUT);
//digitalWrite(D3,HIGH);
//
//pinMode(D5,OUTPUT);
//digitalWrite(D5,LOW);
//
//delay(5000);
//pinMode(D5,INPUT_PULLUP);
//Serial.println(1);
////digitalWrite(D3,HIGH);
//remote.receive();
//pinMode(D5,OUTPUT);
//digitalWrite(D5,LOW);
//
//Serial.println(2);
//delay(10000);
//
//Serial.println(3);
//
//pinMode(D5,INPUT_PULLUP);
//remote.send();
//
//Serial.println(4);
digitalWrite(D3,LOW);

}

void loop() {
  char c=readChar();
//SimpleIRdata d2=d;
//    uint8_t data[]={0,33,17,33,33,17,17,33,17,17,33,17,17,33,33,17,33,33,17,33,33,35,35,35,33,65,33,65,65,65,65,17,17,33,33,17,17,17,17,17,17,17,17,33,33,33,33,33,33,81,0,33,17,33,33,17,17,33,17,17,33,17,17,33,33,17,33,33,17,33,33,33,33,33,33,17,33,17,17,17,17,17,17,33,33,17,17,17,17,17,17,17,17,33,33,33,33,33,33,1};
//    uint16_t vals[]={4386,531,1589,637,460,5203};
//    d2.compressed_data=data;
//    d2.symbols_values=vals;
//    d2.number_of_symbols=6;
//    
//    d2.compression_level=1;
//    d2.number_of_uncompressed_values=199;
//    d2.number_of_compressed_values=100;
//    remote.setData(d2);
//    remote.send();
//    delay(1000);
//char    c=0;
//
//SimpleIRdata d2=d;
//    uint8_t data[]={16,18,34,34,18,35,34,34,34,34,34,34,34,34,34,34,34,33,18,34,2};
//    uint16_t vals[]={2742,869,432,1009};
//    d2.compressed_data=data;
//    d2.symbols_values=vals;
//    d2.number_of_symbols=4;
//    
//    d2.compression_level=1;
//    d2.number_of_uncompressed_values=41;
//    d2.number_of_compressed_values=21;
//    remote.setData(d2);


//pinMode(D5,OUTPUT);
//digitalWrite(D5,LOW);
//    remote.send();
//    
//    Serial.println(d2.number_of_compressed_values);
//    Serial.println(d2.number_of_uncompressed_values);
//    delay(1000);
    
//pinMode(D5,INPUT_PULLUP);

  if(c=='1'){
    Serial.println();
    Serial.println(d.frequency);
    Serial.println(d.compression_level);
    Serial.println(d.number_of_compressed_values);
    Serial.println(d.number_of_uncompressed_values);
    Serial.println(d.number_of_symbols);
    Serial.println(d.number_of_repeats);
    for(uint16_t i=0;i<d.number_of_compressed_values;i++)
    	Serial.printf("%x,",*(d.compressed_data+i));
    Serial.println();
    for(uint16_t i=0;i<d.number_of_symbols;i++)
     Serial.printf("%d,",*(d.symbols_values+i));
    Serial.println();
  }
  if(c=='2'){
    
    SimpleIRdata d2=d;
    uint8_t data[]={0,33,17,33,33,17,17,33,17,17,33,17,17,33,33,17,33,17,33,33,33,33,17,33,33,33,17,17,17,17,33,17,17,33,33,33,17,17,17,17,17,17,17,17,33,33,33,33,33,49};
    uint16_t vals[]={4338,562,1606,5190};
    d2.compressed_data=data;
    d2.symbols_values=vals;
    d2.number_of_symbols=4;
    
    d2.compression_level=1;
    d2.number_of_compressed_values=50;
    d2.number_of_uncompressed_values=200;
    remote.setData(d2);
  }
  if(c=='3'){
    d=remote.getData();
  }
  if(c=='4'){
    remote.send();
  }
  if(c=='5'){
    Serial.println();
    Serial.println("receive:");
    Serial.println(remote.receive());
  }
  if(c=='6'){
    remote.setMaximumCompressionLevel((d.compression_level+1)%3);
  }
  delay(200);
}




char readChar(){
  while(!Serial.available());
  char c=tolower(Serial.read());
  Serial.print(c);
  return c;
}
