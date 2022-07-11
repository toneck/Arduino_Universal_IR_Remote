
#define INPUT_BUFFER_SIZE 500
#define INPUT_MAX_SYMBOLS 16
#define NUMBER_OF_VALUES_FOR_TREND_DETECTION 10
#define APROXIMATION_ERROR 15//%
#define IMPULSE_TIMEOUT 16 //7ms for fan-remote ~14ms for AC remote
#define PULSE_REPETITION_DELAY 0 //ms
//#define DEBUG

#ifdef DEBUG
#define DEBUG_GETDATA_print  Serial.print
#define DEBUG_RECEIVE_print  Serial.print
#define DEBUG_WAIT_print  Serial.print

#define DEBUG_GETDATA_println  Serial.println
#define DEBUG_RECEIVE_println  Serial.println
#define DEBUG_WAIT_println  Serial.println

#define DEBUG_GETDATA_printf  Serial.printf
#define DEBUG_RECEIVE_printf  Serial.printf
#define DEBUG_WAIT_printf  Serial.printf
#else
#define nopln(...) nop(...)
#define nopf(...) nop(...)
#define nop(...) 
#define DEBUG_GETDATA_print  nop
#define DEBUG_RECEIVE_print  nop
#define DEBUG_WAIT_print  nop
#define DEBUG_GETDATA_println  nop
#define DEBUG_RECEIVE_println  nop
#define DEBUG_WAIT_println  nop
#define DEBUG_GETDATA_printf  nop
#define DEBUG_RECEIVE_printf  nop
#define DEBUG_WAIT_printf  nop
#endif
//#define DEBUG_PRINT Serial.println
//#define DEBUG_PRINT Serial.println
//#define DEBUG_PRINT Serial.println

//#define I

#define WATCH_DOG_FEED 
#if defined(ESP8266)
#define WATCH_DOG_FEED ESP.wdtFeed();
#endif

struct SimpleIRdata{
    byte frequency;//in khz
    byte compression_level;
    uint8_t *compressed_data; //the compressed data is stored in bytes (8bits)
    uint16_t *symbols_values; //uncompressed values can be as high as a few thousands => 16bits
    uint16_t number_of_compressed_values; //the number of compressend values (some ACs can have 500 symbols in one trasmission => uint16_t);
    uint16_t number_of_uncompressed_values; //the number of values, if not compressed
    byte number_of_symbols; //the raw individual values shouldn't be more than 16
    byte number_of_repeats;//how many times does the message repeat
};

class SimpleIRremote{

    public:
    
    
    SimpleIRremote();

    void send();
    bool receive();


    SimpleIRdata getData();
    void setData(SimpleIRdata);

    void setReceivingPin(byte pin_number);
    void setSendingPin(byte pin_number);

    void setPinInverted(bool inverted);
    
    void setMaximumCompressionLevel(byte compression_level);

    private:
    SimpleIRdata remote_data;
    byte input_pin;
    byte output_pin;
    byte maximum_compression;
    byte transmission_level;
    byte no_transmission_level;

    void space(uint16_t duration_in_us);
    void mark(uint16_t duretion_in_us);
    bool waitForSignal(uint32_t timeout_ms);

    uint16_t decompress(uint16_t index_of_value);
	  bool aprox(uint16_t value1, uint16_t value2, uint16_t error);
//	bool alocateSpace(uint16_t size_in_bytes);
    

    
};

SimpleIRremote::SimpleIRremote(){
    maximum_compression = 2;
    transmission_level = HIGH;
    no_transmission_level = LOW;
	
}


//void SimpleIRremote::send() {
////  enableIROut(khz);
//  uint16_t val;
//  for (uint8_t r = 0; r < remote_data.number_of_repeats; r++){
//    for (uint16_t i = 0; i < remote_data.number_of_uncompressed_values; i++) {
//      
//      val = decompress(i);
//      // Serial.println(val);
//      if (i & 1) {
//        //Serial.print("-");
//        //Serial.print(code->values[idx - 1]);
//        //Serial.print(", ");
//        space(val);
//      } 
//      else {
//        //Serial.print("+");
//        //Serial.print(code->values[idx - 1]);
//        //Serial.print(", ");
//        mark(val);
//      }
//    }
////    mark(1000);
//    space(PULSE_REPETITION_DELAY*1000); // Just to be sure
//    //Serial.println();
//  }
//}


void SimpleIRremote::send() {
//  enableIROut(khz);
  uint16_t val_space,val_mark;
  uint32_t runtime_offset;

  runtime_offset=micros();
  for (uint8_t r = 0; r < remote_data.number_of_repeats; r++){
    for (uint16_t i = 0; i < remote_data.number_of_uncompressed_values-1; i+=2) {
      
      val_mark = decompress(i);
      val_space = decompress(i+1);
      runtime_offset=micros()-runtime_offset;
      break;
    }
    break;
  }
  
  for (uint8_t r = 0; r < remote_data.number_of_repeats; r++){
    for (uint16_t i = 0; i < remote_data.number_of_uncompressed_values-1; i+=2) {
      
      val_mark = decompress(i);
      val_space = decompress(i+1);
      mark(val_mark);
      space(val_space-runtime_offset);
    }
    space(PULSE_REPETITION_DELAY*1000); // Just to be sure
  }
}


bool SimpleIRremote::receive() {
  uint16_t symbols_values[INPUT_MAX_SYMBOLS];
  uint8_t symbols_values_index;
  uint8_t compressed_values[INPUT_BUFFER_SIZE];
  uint16_t compressed_values_index;
  uint16_t uncompressed_values_index;
  byte compression_level=maximum_compression;

  uint8_t symbols_values_counter[INPUT_MAX_SYMBOLS];
  
  uint32_t previous_timestamp, new_timestamp;
  uint16_t new_value;
//  uint16_t raw_values[NUMBER_OF_VALUES_FOR_TREND_DETECTION];
//  uint8_t index_raw_values;

  for(uint16_t i=0;i<INPUT_MAX_SYMBOLS;i++){
    symbols_values[i]=0;
    symbols_values_counter[i]=0;
  }
  for(uint16_t i=0;i<INPUT_BUFFER_SIZE;i++)
    compressed_values[i]=0;
  
  bool start_compression;
  bool signal_timeout;
  bool compression_decrease;
  uint16_t compression_decrease_idx;

  symbols_values_index=0;
  compressed_values_index=0;
  uncompressed_values_index=0;
//  index_raw_values=0;
  start_compression=true;
  compression_decrease=false;

  DEBUG_RECEIVE_println("initialized");
  signal_timeout=!waitForSignal(5000);
  DEBUG_RECEIVE_println(signal_timeout);
  previous_timestamp=micros();
  while(!signal_timeout){
    WATCH_DOG_FEED;
    DEBUG_RECEIVE_println("start 0");
    signal_timeout=!waitForSignal(IMPULSE_TIMEOUT);
    
      DEBUG_RECEIVE_println("start ");
    if(signal_timeout)
      break;
    new_timestamp=micros();
    new_value=new_timestamp-previous_timestamp;
//    uncompressed_values_index++;
//    raw_values[index_raw_values]=new_value;
    previous_timestamp=new_timestamp;
//    index_raw_values=(index_raw_values+1)%NUMBER_OF_VALUES_FOR_TREND_DETECTION;
//    if(index_raw_values)
//      start_compression=true;

    if(start_compression){
      DEBUG_RECEIVE_println("start compression");

      uint8_t index_of_existing_uncompressed_value;
      //check if the values was encountered before
      index_of_existing_uncompressed_value=255;
      for(uint8_t i=0;i<symbols_values_index;i++){
        if(aprox(symbols_values[i],new_value,APROXIMATION_ERROR)){
          index_of_existing_uncompressed_value=i;
          break;
        }
      }

      //if the values wasn't encountered before, add it to the list
      if(index_of_existing_uncompressed_value==255){
        symbols_values[symbols_values_index]=new_value;//add the new value to the list
        symbols_values_counter[symbols_values_index]=0;//add a counter for it
        index_of_existing_uncompressed_value=symbols_values_index;//get the position for adding later to the compression
        symbols_values_index++;//increase index
      }
      
      //if the number of uncompressed values is to high we must decrease the compression level
      //changing the compression level requires updateing the previous stored values
      if((symbols_values_index>4 && compression_level==2) || (symbols_values_index>16 && compression_level==1)){
        if(compressed_values_index*2>INPUT_MAX_SYMBOLS)
          return false;
        
        compression_decrease=true;
        compression_decrease_idx=uncompressed_values_index;
        compression_level--;
          
      }
      

      //update saved value
      uint8_t counter = symbols_values_counter[index_of_existing_uncompressed_value];
      symbols_values[index_of_existing_uncompressed_value]= ((symbols_values[index_of_existing_uncompressed_value] * counter) + new_value) / (counter + 1);
      symbols_values_counter[index_of_existing_uncompressed_value]++;


      //add the index to the compressed list
      byte compression_factor= 1 <<compression_level;
      uint16_t in_array_idx_compressed_values_index= uncompressed_values_index / compression_factor;
      byte in_symbol_idx_compressed_values_index = uncompressed_values_index % compression_factor;
      uint16_t value;
      
      value=compressed_values[in_array_idx_compressed_values_index];
      value= value | (index_of_existing_uncompressed_value << (in_symbol_idx_compressed_values_index * (8 / compression_factor)));

      compressed_values[in_array_idx_compressed_values_index] = value;
      uncompressed_values_index++;
    }

    
    
  }

  if(compression_decrease){
        if(compressed_values_index+compression_decrease_idx>INPUT_MAX_SYMBOLS)
          return false;
      
      byte old_compression_factor=1 <<(compression_level+1);
      byte new_compression_factor=1 <<compression_level;
      byte old_compression_shift_step = 8 / old_compression_factor;
      byte new_compression_shift_step = 8 / new_compression_factor;

//        for(uint16_t i=0;i<compressed_values_index;i++)
//    DEBUG_GETDATA_printf("%x, ",compressed_values[i]);
//    DEBUG_GETDATA_println("av");
//        for(uint16_t i=0;i<symbols_values_index;i++)
//    DEBUG_GETDATA_printf("%d, ",symbols_values[i]);
        
      for(uint16_t idx=compression_decrease_idx;idx>=0;idx--){
        WATCH_DOG_FEED;
//        if(idx>=comptession_decrease_idx){
//          uint16_t new_idx = idx + comptession_decrease_idx;
//          
//          uint16_t old_array_index = idx / new_compression_factor;
//          byte old_symbol_index = idx % new_compression_factor;
//          
//          uint16_t new_array_index = new_idx / new_compression_factor;
//          byte new_symbol_index = new_idx % new_compression_factor;
//        }
//        else{
          uint16_t old_array_index = idx / old_compression_factor;
          byte old_symbol_index = idx % old_compression_factor;
          
          uint16_t new_array_index = idx / new_compression_factor;
          byte new_symbol_index = idx % new_compression_factor;
  
          byte value_old = compressed_values[old_array_index];
          byte value_to_transfer = (value_old >> (old_symbol_index * old_compression_shift_step)) & ((1 << old_compression_shift_step) - 1);
          byte value_new = compressed_values[new_array_index];
          //value_new = value_new & mask; mask = not(ones shifted to position); ones = ((1 << (8 / new_compression_factor)) - 1);
          //value_new = value_new & (~(((1 << (8 / new_compression_factor)) - 1) << (new_symbol_index * (8 / new_compression_factor))));
          byte bits_to_shift = new_symbol_index * new_compression_shift_step;
          value_new = value_new & (~(((1 << new_compression_shift_step) - 1) << bits_to_shift));//clear the space before using it
          value_new = value_new | (value_to_transfer << bits_to_shift);
          compressed_values[new_array_index] = value_new;
  //          Serial.println(old_array_index);Serial.println(new_array_index);
          if(idx==0) //we exit after hitting 0 since this is the last element
            break;//due to using unsigned type, idx>=0 will always hold true, so we must exit manually
//        }
      }
  }
  
//  if(signal_timeout){
//    return false;
//  }
  remote_data=*((SimpleIRdata*)malloc(sizeof(remote_data)));
  remote_data.frequency=42;
  remote_data.compression_level=compression_level;
  remote_data.number_of_compressed_values=(uncompressed_values_index/((1<<compression_level)))+((uncompressed_values_index%((1<<compression_level)))>0);
  remote_data.number_of_uncompressed_values=uncompressed_values_index;
  remote_data.number_of_symbols=symbols_values_index;
  remote_data.number_of_repeats=1;
  Serial.println(remote_data.frequency);
  Serial.println(remote_data.compression_level );
  Serial.println(remote_data.number_of_compressed_values);
  Serial.println(remote_data.number_of_uncompressed_values);
  Serial.println(remote_data.number_of_symbols);
  Serial.println(remote_data.number_of_repeats);
  remote_data.compressed_data=(typeof(remote_data.compressed_data))malloc(remote_data.number_of_compressed_values*sizeof(uint8_t));
  remote_data.symbols_values=(typeof(remote_data.symbols_values))malloc(remote_data.number_of_symbols*sizeof(uint16_t));
  Serial.println(remote_data.frequency);
  for(uint16_t i=0;i<remote_data.number_of_compressed_values;i++)
    *(remote_data.compressed_data+i)=(compressed_values[i]);
  for(uint16_t i=0;i<remote_data.number_of_symbols;i++)
    *(remote_data.symbols_values+i)=symbols_values[i];
  return true;

  
}



SimpleIRdata SimpleIRremote::getData(){
  SimpleIRdata *p=(SimpleIRdata*)malloc(sizeof(remote_data));
  (*p).frequency=remote_data.frequency;
  (*p).compression_level=remote_data.compression_level;
  (*p).number_of_compressed_values=remote_data.number_of_compressed_values;
  (*p).number_of_uncompressed_values=remote_data.number_of_uncompressed_values;
  (*p).number_of_symbols=remote_data.number_of_symbols;
  (*p).number_of_repeats=remote_data.number_of_repeats;
  (*p).compressed_data=(typeof(remote_data.compressed_data))malloc(remote_data.number_of_compressed_values*sizeof(uint8_t));
  (*p).symbols_values=(typeof(remote_data.symbols_values))malloc(remote_data.number_of_symbols*sizeof(uint16_t));
  DEBUG_GETDATA_println("vctrs");
  for(uint16_t i=0;i<remote_data.number_of_compressed_values;i++)
    *((*p).compressed_data+i)=*(remote_data.compressed_data+i);
  DEBUG_GETDATA_println("vctrs");
  for(uint16_t i=0;i<remote_data.number_of_symbols;i++)
    *((*p).symbols_values+i)=*(remote_data.symbols_values+i);
  DEBUG_GETDATA_println("vctrs");
  for(uint16_t i=0;i<(*p).number_of_uncompressed_values;i++)
    DEBUG_GETDATA_printf("%x, ",*((*p).compressed_data+i));
  DEBUG_GETDATA_println("av");
  for(uint16_t i=0;i<(*p).number_of_symbols;i++)
    DEBUG_GETDATA_printf("%d, ",*((*p).symbols_values+i));
  DEBUG_GETDATA_println("av2");
  return *p;
}

void SimpleIRremote::setData(SimpleIRdata p){
  SimpleIRdata *d=(SimpleIRdata*)malloc(sizeof(p));
  (*d).frequency=p.frequency;
  (*d).compression_level=p.compression_level;
  (*d).number_of_compressed_values=p.number_of_compressed_values;
  (*d).number_of_uncompressed_values=p.number_of_uncompressed_values;
  (*d).number_of_symbols=p.number_of_symbols;
  (*d).number_of_repeats=p.number_of_repeats;
  (*d).compressed_data=(typeof((*d).compressed_data))malloc(p.number_of_compressed_values*sizeof(uint8_t));
  (*d).symbols_values=(typeof((*d).symbols_values))malloc(p.number_of_symbols*sizeof(uint16_t));
  for(uint16_t i=0;i<p.number_of_compressed_values;i++)
    *((*d).compressed_data+i)=*(p.compressed_data+i);
  for(uint16_t i=0;i<p.number_of_symbols;i++)
    *((*d).symbols_values+i)=*(p.symbols_values+i);
  remote_data=*d;
}

void SimpleIRremote::setReceivingPin(byte pin_number){
  input_pin = pin_number;
  pinMode(input_pin,INPUT_PULLUP);
}

void SimpleIRremote::setSendingPin(byte pin_number){
  output_pin = pin_number;
  pinMode(output_pin,OUTPUT);
}

void SimpleIRremote::setPinInverted(bool inverted) {
  if(inverted){
    transmission_level = LOW;
    no_transmission_level = HIGH;
  }
  else {
    transmission_level = HIGH;
    no_transmission_level = LOW;
  }
}

void SimpleIRremote::setMaximumCompressionLevel(byte compression_level){
  maximum_compression=compression_level;
}


void SimpleIRremote::space(uint16_t width){
  uint32_t start_period,start_space;
  start_space=micros();
  do{
    start_period=micros();
    WATCH_DOG_FEED;
    while((micros()-start_period)<26 - 2);//-1 compensating for micros-start_period
                                          //-1 compensating for micros-start_space
  }while((micros()-start_space)<width);
}

void SimpleIRremote::mark(uint16_t width){
  uint32_t start_period,start_mark;
  start_mark=micros();
  do{
    digitalWrite(output_pin,transmission_level);
    start_period=micros();
    while((micros()-start_period)<5 - 1); //-1 compensating for micros-start_period
    digitalWrite(output_pin,no_transmission_level);
    WATCH_DOG_FEED;
    while((micros()-start_period)<26 - 2); //-1 compensating for micros-start_period
                                           //-1 compensating for micros-start_mark
  }while((micros()-start_mark)<width);
//  Serial.println(start_mark);
//  Serial.println(micros());
}

bool SimpleIRremote::waitForSignal(uint32_t timeout_ms=10) {
  bool original_state=digitalRead(input_pin);
  bool new_state=digitalRead(input_pin);
  
  uint32_t previous_timestamp=millis();
  while(original_state==new_state){
    new_state=digitalRead(input_pin);
    WATCH_DOG_FEED;
    if(millis()-previous_timestamp>timeout_ms)
      return false;
  }
  DEBUG_WAIT_println(original_state);
  DEBUG_WAIT_println(new_state);
  
  return true;
}


uint16_t SimpleIRremote::decompress(uint16_t index_of_value){
  byte compression = remote_data.compression_level;
  byte compression_multiplier = 1 <<compression;
  uint8_t coded_symbol = remote_data.compressed_data[ index_of_value / compression_multiplier];
  uint8_t idx = 0;
  switch(compression){
    case 0:
      idx = coded_symbol;
      break;
    case 1:
      idx = (coded_symbol >> (4 * (index_of_value % compression_multiplier))) & 0x0F;
//      if(index_of_value%compression_multiplier==0)
//        idx = coded_symbol & 0x0F;
//      else
//        idx = coded_symbol >> 4;
      break;
    case 2:
      idx = (coded_symbol >> (2 * (index_of_value % compression_multiplier))) & 0x03;
//      if(index_of_value%compression_multiplier==0)
//        idx = coded_symbol & 0x03;
//      else if(index_of_value%compression_multiplier==1)
//        idx = (coded_symbol >> 2) & 0x03;
//      else if(index_of_value%compression_multiplier==2)
//        idx = (coded_symbol >> 4) & 0x03;
//      else if(index_of_value%compression_multiplier==3)
//        idx = (coded_symbol >> 6) & 0x03;
      break;
    default:
    break;
  }
  return remote_data.symbols_values[idx];
  
}

bool SimpleIRremote::aprox(uint16_t value1, uint16_t value2, uint16_t error=APROXIMATION_ERROR){
  uint16_t diff=(value1>value2?(value1-value2):(value2-value1));
  diff = diff * 100 / value1;
  return diff<error; 
}
