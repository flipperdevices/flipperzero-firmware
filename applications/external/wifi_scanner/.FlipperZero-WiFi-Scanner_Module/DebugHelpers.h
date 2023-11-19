class PerfTimer
{
  public:
    PerfTimer(const char* debugName) : m_debugName(debugName)
    {
      m_startTime = millis();
    }
  
    ~PerfTimer()
    {
      char buffer[128];
      sprintf(buffer, "[%s] %ul millis", m_debugName, millis() - m_startTime);
      Serial.println();
      Serial.println(buffer);
    }

  private:
    unsigned long m_startTime = 0;
    const char* m_debugName;
};


#ifdef DEBUG
#define DEBUG_LOG(text) Serial.print(text)
#define DEBUG_LOG_LN(text) Serial.println(text)
#define FUNCTION_PERF() PerfTimer perfTimer(__FUNCTION__)
#else
#define DEBUG_LOG(text) 
#define DEBUG_LOG_LN(text)
#define FUNCTION_PERF()
#endif
