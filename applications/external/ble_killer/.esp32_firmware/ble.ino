#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "mbedtls/aes.h"

#define TX1 4  // GPIO4
#define RX1 5  // GPIO5

int scanTime = 7; //In seconds
BLEScan* pBLEScan;

uint8_t max_scan_count = 10;

BLEUUID serviceUUID("0000fee7-0000-1000-8000-00805f9b34fb");
BLEUUID charUUID_TX("000036f5-0000-1000-8000-00805f9b34fb");
BLEUUID charUUID_RX("000036f6-0000-1000-8000-00805f9b34fb");

//BLEUUID charUUID("00002a37-0000-1000-8000-00805f9b34fb");

uint8_t key[] = {0x0D, 0x4E, 0x62, 0x26, 0x5B, 0x4E, 0x0B, 0x4B, 0x10, 0x01, 0x08, 0x3D, 0x16, 0x56, 0x12, 0x5F};
uint8_t openLockEncData[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


BLERemoteCharacteristic* aer_charac = NULL;

typedef struct Weapon {
  uint8_t weapon_id;
  char weapon_name[24];
  void (*weapon_cb)(void *wname);
} Weapon;


typedef struct Weapon_List {
  Weapon weapon;
  struct Weapon_List *next;
} Weapon_List;


void print_log(char *logs);
Weapon_List* createNode(Weapon weapon);
Weapon_List* addNode(Weapon_List* head, Weapon weapon);
void printList(Weapon_List* head);
void weapon_init();

Weapon_List* createNode(Weapon weapon) {
  Weapon_List* newNode = (Weapon_List*) malloc(sizeof(Weapon_List));
  if(newNode == NULL) {
    fprintf(stderr, "Unable to allocate memory for new node\n");
    exit(-1);
  }
  newNode->weapon = weapon;
  newNode->next = NULL;
  return newNode;
}

Weapon_List* addNode(Weapon_List* head, Weapon weapon) {
  Weapon_List* newNode = createNode(weapon);
  if(head == NULL) {
    head = newNode;
  } else {
    Weapon_List* temp = head;
    while(temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = newNode;
  }

  return head;
}


void printList(Weapon_List* head) {
  Weapon_List* temp = head;
  while(temp != NULL) {
    Serial.printf("Weapon ID: %d, Weapon Name: %s\n", temp->weapon.weapon_id, temp->weapon.weapon_name);
    Serial1.printf("Weapon ID: %d, Weapon Name: %s\n", temp->weapon.weapon_id, temp->weapon.weapon_name);
    temp = temp->next;
  }
}


Weapon_List *head = NULL;

void weapon_init(){
  
  Weapon weapon0 = {0, "oklok_unlock", oklok_unlock};
  head = addNode(head, weapon0);

  printList(head);
}

void encrypt(uint8_t * input, uint8_t * key, uint8_t * output) {

  mbedtls_aes_context aes;

  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_enc( &aes, key, 128);
  mbedtls_aes_crypt_ecb( &aes, MBEDTLS_AES_ENCRYPT, input, output);
  mbedtls_aes_free( &aes );
}

void decrypt(uint8_t* input, uint8_t* output, size_t length, uint8_t* key) {
  mbedtls_aes_context aes;

  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_dec(&aes, key, 128);

  for (size_t i = 0; i < length; i += 16) {
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, input + i, output + i);
  }

  mbedtls_aes_free(&aes);
}


void notifyCallback(BLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  if (length > 0) {
    Serial.println("Received notification:");
    //        for (int i = 0; i < length; i++)
    //            Serial.print(pData[i]);
    //        Serial.println();

    // Decrypt the data.
    
    uint8_t decrypted[length];
    decrypt(pData, decrypted, length, key);

    // Print the decrypted data.
    Serial.println("Decrypted notify data:");
    Serial1.println("[Notify] Decrypted notify data:");
    for (int i = 0; i < length; i++)
      Serial.printf("%02x,", decrypted[i]);
    Serial.println();
    Serial1.println();

    if (decrypted[0] == '\x06' && decrypted[1] == '\x02' && decrypted[2] == '\x07') {
      uint8_t plainBuffer[16];
      uint8_t header[] = {0x05, 0x01, 0x06, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
      uint8_t tail[] = {0x00, 0x00, 0x00};

      for (int i = 0; i < 9; i++) {
        plainBuffer[i] = header[i];
      }
      for (int i = 0; i < 4; i++) {
        plainBuffer[i + 9] = decrypted[i + 3];
      }

      for (int i = 0; i < 3; i++) {
        plainBuffer[i + 13] = tail[i];
      }


      Serial.print("Parse token result: ");
      Serial1.print("[Notify] Parse token result: ");
      for (int i = 0; i < 16; i++) {
        Serial.printf("%02x,", plainBuffer[i]);
        Serial1.printf("%02x,", plainBuffer[i]);
      }
      Serial.println();
      Serial1.println();

      encrypt(plainBuffer, key, openLockEncData);

      Serial.print("Encrypt open lock data: ");
      Serial1.print("[Notify] Encrypt open lock data: ");
      for (int i = 0; i < 16; i++) {
        Serial.printf("%02x,", openLockEncData[i]);
        Serial1.printf("%02x,", openLockEncData[i]);
      }
      Serial.println();
      Serial1.println();
    }
  }
}

void oklok_unlock(void *wname){
  Serial1.printf("Execute payload: %s", wname);
  Serial.printf("Execute payload: %s", wname);
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found count: ");
  Serial1.print("Devices found count: ");
  Serial.println(foundDevices.getCount());
  Serial1.println(foundDevices.getCount());
  
  for (int i = 0; i < foundDevices.getCount(); i++) {
    BLEAdvertisedDevice advertisedDevice = foundDevices.getDevice(i);
    if (strstr(advertisedDevice.getName().c_str(), "Blue")) {
      Serial.print("Found lock: ");
      Serial1.print("Found lock: ");
      Serial.println(advertisedDevice.getName().c_str());
      Serial1.println(advertisedDevice.getName().c_str());
      BLEClient* pClient  = BLEDevice::createClient();

      // connect to the remote BLE Server
      pClient->connect(&advertisedDevice);

      delay(200);

      BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
      if (pRemoteService == nullptr) {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        return;
      }

      Serial.println("ServiceUUID init done.");
      Serial1.println("ServiceUUID init done.");

      delay(200);

      BLERemoteCharacteristic* pRemoteCharacteristic_tx = pRemoteService->getCharacteristic(charUUID_TX);
      BLERemoteCharacteristic* pRemoteCharacteristic_rx = pRemoteService->getCharacteristic(charUUID_RX);
      if (pRemoteCharacteristic_tx == nullptr || pRemoteCharacteristic_rx == nullptr) {
        Serial.print("Failed to find our characteristic UUID: ");
        Serial.println(charUUID_TX.toString().c_str());
        return;
      }

      Serial.println("CharUUID init done.");
      Serial1.println("CharUUID init done.");

      pRemoteCharacteristic_rx->registerForNotify(notifyCallback);



      delay(200);
      // 特殊的uuid服务，检测到写01 00数据时会发送通知
      BLEDescriptor* pDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
      uint8_t val[] = {0x01, 0x00};
      pDescriptor->setValue((uint8_t*)val, 2);
      
      // 写加密数据，固定加密的结果
      uint8_t get_token[] = {0xa2, 0x73, 0x0d, 0x1e, 0x25, 0x43, 0xdd, 0xe8, 0xb2, 0x35, 0x7f, 0xf7, 0x34, 0x60, 0xfd, 0x17};


      // 写完之后触发 notify 回调函数
      pRemoteCharacteristic_tx->writeValue(get_token, sizeof(get_token));


      delay(200);

      while (1) {
        //        for (int i = 0; i < 16; i++) {
        //          Serial.printf("%02x,", openLockEncData[i]);
        //        }
        if (openLockEncData[0] != '\x00') {
          break;
        }
        delay(500);
      };


      Serial.println("Sending open lock data...");
      Serial1.println("Sending open lock data...");

      pRemoteCharacteristic_tx->writeValue(openLockEncData, sizeof(openLockEncData));

      Serial.println("Done.");
      Serial1.println("Done.");



      Serial.println("Sleeping to restart ESP32.");
      Serial1.println("Sleeping to restart ESP32.");
        for (int i = 0; i < sizeof(openLockEncData); i++) {
          openLockEncData[i] = '\x00';
        }

      delay(2000);
      ESP.restart();

    }
  }


  Serial.println("Scan done!");
  Serial1.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  
}

void ble_device_scan(){
  char ble_scan_info[1024];
  memset(ble_scan_info, '\0', sizeof(ble_scan_info));
  
  // print_log("Ble device Scan staring!");
  Serial.println("Ble device Scan staring!");
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  //print_log("Devices found count: ");
  Serial.println("Devices found count: ");
//  Serial.print("Devices found count: ");
//  Serial1.print("Devices found count: ");
  Serial.println(foundDevices.getCount());
  int c;
  if(foundDevices.getCount() > max_scan_count){

    c = max_scan_count;
  }
  //Serial1.println(foundDevices.getCount());
  
  for (int i = 0; i < c; i++) {
    BLEAdvertisedDevice advertisedDevice = foundDevices.getDevice(i);
    const char *addr = advertisedDevice.getAddress().toString().c_str();
    Serial.printf("Device Address: %s \n", addr);
    const char *wname = advertisedDevice.getName().c_str();
    
    print_log(strdup(wname));

    strcat(ble_scan_info, "Address: ");
    strcat(ble_scan_info, addr);
    if(strlen(wname) > 1){
      strcat(ble_scan_info, " Name: ");
      strcat(ble_scan_info, wname);
    }
    strcat(ble_scan_info, ",");
    
  }
  strcat(ble_scan_info, ";");

  Serial.println(ble_scan_info);
  Serial1.println(ble_scan_info);
  pBLEScan->clearResults();
  Serial.println("Ble device Scan finish!");
}

void print_log(char *logs){
  Serial.println(logs);
  Serial1.println(logs);
  
}

void setup() {
  Serial.begin(115200);
  Serial.println("Init...");
  Serial1.println("Init...");

  Serial1.begin(9600, SERIAL_8N1, RX1, TX1);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value


  weapon_init();

}

void loop() {
  if(Serial1.available()){
    String cmd = Serial1.readString();

    if(!strncmp(cmd.c_str(), "scan", 4)){
      ble_device_scan();
    }else if(strstr(cmd.c_str(), "weapon")){
      
      char ccmd[256];
      memcpy(ccmd, cmd.c_str(), strlen(cmd.c_str()));

      char *c = strtok(ccmd, ",");

      char *idx_ptr = strtok(NULL, ",");
      uint8_t idx = atoi(idx_ptr);

      Weapon_List* temp = head;
      while(temp != NULL) {
        if(temp->weapon.weapon_id == idx){
          Serial.printf("Enter callback func, weapon index: %d.\n", idx);
          (* temp->weapon.weapon_cb)(temp->weapon.weapon_name);
        }

       temp = temp->next;
      }
    }
  
  delay(100);
}
