#ifdef DEBUG
#define DEBUG_LOG(text) Serial.print(text)
#define DEBUG_LOG_LN(text) Serial.println(text)
#else
#define DEBUG_LOG(text) 
#define DEBUG_LOG_LN(text) 
#endif
