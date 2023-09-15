#include "Buffer.h"
#include "lang_var.h"

Buffer::Buffer(){
  bufA = (uint8_t*)malloc(BUF_SIZE);
  bufB = (uint8_t*)malloc(BUF_SIZE);
}

void Buffer::createPcapFile(fs::FS* fs, String fn, bool log){
  int i=0;
  if (!log) {
    do{
      fileName = "/"+fn+"_"+(String)i+".pcap";
      i++;
    } while(fs->exists(fileName));
  }
  else {
    do{
      fileName = "/"+fn+"_"+(String)i+".log";
      i++;
    } while(fs->exists(fileName));
  }

  Serial.println(fileName);
  
  file = fs->open(fileName, FILE_WRITE);
  file.close();
}

void Buffer::open(bool log){
  bufSizeA = 0;
  bufSizeB = 0;

  bufSizeB = 0;

  writing = true;

  if (!log) {
    write(uint32_t(0xa1b2c3d4)); // magic number
    write(uint16_t(2)); // major version number
    write(uint16_t(4)); // minor version number
    write(int32_t(0)); // GMT to local correction
    write(uint32_t(0)); // accuracy of timestamps
    write(uint32_t(SNAP_LEN)); // max length of captured packets, in octets
    write(uint32_t(105)); // data link type
  }
}

void Buffer::close(fs::FS* fs){
  if(!writing) return;
  forceSave(fs);
  writing = false;
  Serial.println(text01);
}

void Buffer::addPacket(uint8_t* buf, uint32_t len, bool log){
  // buffer is full -> drop packet
  if((useA && bufSizeA + len >= BUF_SIZE && bufSizeB > 0) || (!useA && bufSizeB + len >= BUF_SIZE && bufSizeA > 0)){
    //Serial.print(";"); 
    return;
  }
  
  if(useA && bufSizeA + len + 16 >= BUF_SIZE && bufSizeB == 0){
    useA = false;
    //Serial.println("\nswitched to buffer B");
  }
  else if(!useA && bufSizeB + len + 16 >= BUF_SIZE && bufSizeA == 0){
    useA = true;
    //Serial.println("\nswitched to buffer A");
  }

  uint32_t microSeconds = micros(); // e.g. 45200400 => 45s 200ms 400us
  uint32_t seconds = (microSeconds/1000)/1000; // e.g. 45200400/1000/1000 = 45200 / 1000 = 45s

  microSeconds -= seconds*1000*1000; // e.g. 45200400 - 45*1000*1000 = 45200400 - 45000000 = 400us (because we only need the offset)
  
  if (!log) {
    write(seconds); // ts_sec
    write(microSeconds); // ts_usec
    write(len); // incl_len
    write(len); // orig_len
  }
  
  write(buf, len); // packet payload
}

void Buffer::write(int32_t n){
  uint8_t buf[4];
  buf[0] = n;
  buf[1] = n >> 8;
  buf[2] = n >> 16;
  buf[3] = n >> 24;
  write(buf,4);
}

void Buffer::write(uint32_t n){
  uint8_t buf[4];
  buf[0] = n;
  buf[1] = n >> 8;
  buf[2] = n >> 16;
  buf[3] = n >> 24;
  write(buf,4);
}

void Buffer::write(uint16_t n){
  uint8_t buf[2];
  buf[0] = n;
  buf[1] = n >> 8;
  write(buf,2);
}

void Buffer::write(uint8_t* buf, uint32_t len){
  if(!writing) return;
  
  if(useA){
    memcpy(&bufA[bufSizeA], buf, len);
    bufSizeA += len;
  }else{
    memcpy(&bufB[bufSizeB], buf, len);
    bufSizeB += len;
  }
}

void Buffer::save(fs::FS* fs){
  if(saving) return; // makes sure the function isn't called simultaneously on different cores

  // buffers are already emptied, therefor saving is unecessary
  if((useA && bufSizeB == 0) || (!useA && bufSizeA == 0)){
    //Serial.printf("useA: %s, bufA %u, bufB %u\n",useA ? "true" : "false",bufSizeA,bufSizeB); // for debug porpuses
    return;
  }
  
  //Serial.println("saving file");
  
  uint32_t startTime = millis();
  uint32_t finishTime;

  file = fs->open(fileName, FILE_APPEND);
  if (!file) {
    Serial.println(text02 + fileName+"'");
    //useSD = false;
    return;
  }
  
  saving = true;
  
  uint32_t len;
  
  if(useA){
    file.write(bufB, bufSizeB);
    len = bufSizeB;
    bufSizeB = 0;
  }
  else{
    file.write(bufA, bufSizeA);
    len = bufSizeA;
    bufSizeA = 0;
  }

  file.close();
  
  finishTime = millis() - startTime;

  //Serial.printf("\n%u bytes written for %u ms\n", len, finishTime);
  
  saving = false;
  
}

void Buffer::forceSave(fs::FS* fs){
  uint32_t len = bufSizeA + bufSizeB;
  if(len == 0) return;
  
  file = fs->open(fileName, FILE_APPEND);
  if (!file) {
    Serial.println(text02+fileName+"'");
    //useSD = false;
    return;
  }

  saving = true;
  writing = false;
  
  if(useA){

    if(bufSizeB > 0){
      file.write(bufB, bufSizeB);
      bufSizeB = 0;
    }

    if(bufSizeA > 0){
      file.write(bufA, bufSizeA);
      bufSizeA = 0;
    }
    
  } else {

    if(bufSizeA > 0){
      file.write(bufA, bufSizeA);
      bufSizeA = 0;
    }
    
    if(bufSizeB > 0){
      file.write(bufB, bufSizeB);
      bufSizeB = 0;
    }
    
  }

  file.close();

  //Serial.printf("saved %u bytes\n",len);

  saving = false;
  writing = true;
}

void Buffer::forceSaveSerial() {
  uint32_t len = bufSizeA + bufSizeB;
  if(len == 0) return;

  saving = true;
  writing = false;

  if(useA){
    if(bufSizeB > 0){
      Serial1.write(bufB, bufSizeB);
      bufSizeB = 0;
    }
    if(bufSizeA > 0){
      Serial1.write(bufA, bufSizeA);
      bufSizeA = 0;
    }
  } else {
    if(bufSizeA > 0){
      Serial1.write(bufA, bufSizeA);
      bufSizeA = 0;
    }
    if(bufSizeB > 0){
      Serial1.write(bufB, bufSizeB);
      bufSizeB = 0;
    }
  }

  saving = false;
  writing = true;
}
