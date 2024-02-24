#include "arduino.h"

#define DEC 10
#define HEX 16
#define OCT 8

class Print
{
public:
  virtual ~Print() {};
  virtual size_t write(uint8_t) = 0;
  virtual size_t write(const char *buffer, size_t size) = 0;
  virtual size_t print(const char[]) = 0;
  virtual size_t print(int, int = DEC) = 0;
  virtual size_t println(void) = 0;
  virtual size_t println(const char[]) = 0;
  virtual size_t println(int, int = DEC) = 0;
  virtual int read() = 0;
  bool available() { return true; }
};

class Stream : public Print
{
public:
  Stream(App* app);
  virtual ~Stream() {};
  size_t write(uint8_t);
  size_t write(const char *buffer, size_t size);
  size_t print(const char[]);
  size_t print(int, int = DEC);
  size_t println(void);
  size_t println(const char[]);
  size_t println(int, int = DEC);
  void set_callback(DmcommCallback cb);
  int read();
private:
  App* app;
  DmcommCallback cb;
};