#include <string.h>
#include "sha1.h"

union _buffer {
  uint8_t b[BLOCK_LENGTH];
  uint32_t w[BLOCK_LENGTH/4];
} buffer;
union _state {
  uint8_t b[HASH_LENGTH];
  uint32_t w[HASH_LENGTH/4];
} state;

uint8_t bufferOffset;
uint32_t byteCount;
uint8_t keyBuffer[BLOCK_LENGTH];
uint8_t innerHash[HASH_LENGTH];

#define SHA1_K0 0x5a827999
#define SHA1_K20 0x6ed9eba1
#define SHA1_K40 0x8f1bbcdc
#define SHA1_K60 0xca62c1d6

uint8_t sha1InitState[] = {
  0x01,0x23,0x45,0x67, // H0
  0x89,0xab,0xcd,0xef, // H1
  0xfe,0xdc,0xba,0x98, // H2
  0x76,0x54,0x32,0x10, // H3
  0xf0,0xe1,0xd2,0xc3  // H4
};

void sha1_init(void) {
  memcpy(state.b,sha1InitState,HASH_LENGTH);
  byteCount = 0;
  bufferOffset = 0;
}

uint32_t rol32(uint32_t number, uint8_t bits) {
  return ((number << bits) | (uint32_t)(number >> (32-bits)));
}

void hashBlock() {
  uint8_t i;
  uint32_t a,b,c,d,e,t;

  a=state.w[0];
  b=state.w[1];
  c=state.w[2];
  d=state.w[3];
  e=state.w[4];
  for (i=0; i<80; i++) {
    if (i>=16) {
      t = buffer.w[(i+13)&15] ^ buffer.w[(i+8)&15] ^ buffer.w[(i+2)&15] ^ buffer.w[i&15];
      buffer.w[i&15] = rol32(t,1);
    }
    if (i<20) {
      t = (d ^ (b & (c ^ d))) + SHA1_K0;
    } else if (i<40) {
      t = (b ^ c ^ d) + SHA1_K20;
    } else if (i<60) {
      t = ((b & c) | (d & (b | c))) + SHA1_K40;
    } else {
      t = (b ^ c ^ d) + SHA1_K60;
    }
    t+=rol32(a,5) + e + buffer.w[i&15];
    e=d;
    d=c;
    c=rol32(b,30);
    b=a;
    a=t;
  }
  state.w[0] += a;
  state.w[1] += b;
  state.w[2] += c;
  state.w[3] += d;
  state.w[4] += e;
}

void addUncounted(uint8_t data) {
  buffer.b[bufferOffset ^ 3] = data;
  bufferOffset++;
  if (bufferOffset == BLOCK_LENGTH) {
    hashBlock();
    bufferOffset = 0;
  }
}

void sha1_write(uint8_t data) {
  ++byteCount;
  addUncounted(data);

  return;
}

void sha1_write_array(uint8_t *buffer, uint8_t size){
    while (size--) {
        sha1_write(*buffer++);
    }
}

void pad() {
  // Implement SHA-1 padding (fips180-2 ��5.1.1)

  // Pad with 0x80 followed by 0x00 until the end of the block
  addUncounted(0x80);
  while (bufferOffset != 56) addUncounted(0x00);

  // Append length in the last 8 bytes
  addUncounted(0); // We're only using 32 bit lengths
  addUncounted(0); // But SHA-1 supports 64 bit lengths
  addUncounted(0); // So zero pad the top bits
  addUncounted(byteCount >> 29); // Shifting to multiply by 8
  addUncounted(byteCount >> 21); // as SHA-1 supports bitstreams as well as
  addUncounted(byteCount >> 13); // byte.
  addUncounted(byteCount >> 5);
  addUncounted(byteCount << 3);
}

uint8_t* sha1_result(void) {
  // Pad to complete the last block
  pad();

  // Swap byte order back
  uint8_t i;
  for (i=0; i<5; i++) {
    uint32_t a,b;
    a=state.w[i];
    b=a<<24;
    b|=(a<<8) & 0x00ff0000;
    b|=(a>>8) & 0x0000ff00;
    b|=a>>24;
    state.w[i]=b;
  }

  // Return pointer to hash (20 characters)
  return state.b;
}

#define HMAC_IPAD 0x36
#define HMAC_OPAD 0x5c

void sha1_init_hmac(const uint8_t* key, uint8_t keyLength) {
  uint8_t i;
  memset(keyBuffer,0,BLOCK_LENGTH);
  if (keyLength > BLOCK_LENGTH) {
    // Hash long keys
    sha1_init();
    for (;keyLength--;) sha1_write(*key++);
    memcpy(keyBuffer,sha1_result(),HASH_LENGTH);
  } else {
    // Block length keys are used as is
    memcpy(keyBuffer,key,keyLength);
  }
  // Start inner hash
  sha1_init();
  for (i=0; i<BLOCK_LENGTH; i++) {
    sha1_write(keyBuffer[i] ^ HMAC_IPAD);
  }
}

uint8_t* sha1_result_hmac(void) {
  uint8_t i;
  // Complete inner hash
  memcpy(innerHash,sha1_result(),HASH_LENGTH);
  // Calculate outer hash
  sha1_init();
  for (i=0; i<BLOCK_LENGTH; i++) sha1_write(keyBuffer[i] ^ HMAC_OPAD);
  for (i=0; i<HASH_LENGTH; i++) sha1_write(innerHash[i]);
  return sha1_result();
}
