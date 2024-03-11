#pragma once

#ifndef GpsInterface_h
#define GpsInterface_h

#include <MicroNMEA.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

#include "configs.h"

//#define GPS_TEXT_MAXLINES 5 //default:5 lines in the buffer maximum
//#define GPS_TEXT_MAXCYCLES 1 //default:1

//#define GPS_NMEA_SCRNLINES TEXT_HEIGHT //default: defined TEXT_HEIGHT from configs.h
//#define GPS_NMEA_SCRNWRAP true //default:true, except on MARAUDER_MINI where false
//#define GPS_NMEA_MAXQUEUE 30 //default:30 messages max in queue

#ifdef MARAUDER_MINI
  #ifndef GPS_NMEA_SCRNWRAP
    #define GPS_NMEA_SCRNWRAP false
  #endif
#else
  #ifndef GPS_NMEA_SCRNWRAP
    #define GPS_NMEA_SCRNWRAP true
  #endif
#endif

struct nmea_sentence_t {
  bool unparsed;
  String type;
  String sentence;
};

void gps_nmea_notimp(MicroNMEA& nmea);

class GpsInterface {
  public:
    void begin();
    void main();

    int getNumSats();
    String getNumSatsString();
    bool getFixStatus();
    String getFixStatusAsString();
    bool getGpsModuleStatus();
    String getLat();
    String getLon();
    float getAlt();
    float getAccuracy();
    String getDatetime();
    String getText();
    int getTextQueueSize();
    String getTextQueue(bool flush=1);
    String getNmea();
    String getNmeaNotimp();
    String getNmeaNotparsed();

    void setType(String t);

    void enqueue(MicroNMEA& nmea);
    LinkedList<nmea_sentence_t>* get_queue();
    void flush_queue();
    void flush_text();
    void new_queue();
    void enable_queue();
    void disable_queue();
    bool queue_enabled();

    void sendSentence(const char* sentence);
    void sendSentence(Stream &s, const char* sentence);

    String generateGXgga();
    String generateGXrmc();

  private:
    enum type_t {
      GPSTYPE_NATIVE = 0,
      GPSTYPE_ALL = 1,
      GPSTYPE_GPS = 2,
      GPSTYPE_GLONASS = 3,
      GPSTYPE_GALILEO = 4,
      GPSTYPE_NAVIC = 5,
      GPSTYPE_QZSS = 6,
      GPSTYPE_BEIDOU = 7,
      GPSTYPE_BEIDOU_BD = 8
    };

    // GPS Info
    String gps_text = "";
    String nmea_sentence = "";
    String notimp_nmea_sentence = "";
    String notparsed_nmea_sentence = "";
    String lat = "";
    String lon = "";
    float altf = 0.0;
    float accuracy = 0.0;
    String datetime = "";
    
    bool gps_enabled = false;
    bool good_fix = false;
    char nav_system='\0';
    uint8_t num_sats = 0;

    type_t type_flag = GPSTYPE_NATIVE;

    bool queue_enabled_flag=0;
    LinkedList<nmea_sentence_t> *queue=NULL;

    unsigned int text_cycles=0;
    LinkedList<String> *text_in=NULL;
    LinkedList<String> *text=NULL;

    String generateType();
    void flush_queue_text();
    void flush_queue_textin();
    void flush_queue_nmea();
    String dt_string_from_gps();
    void setGPSInfo();
};

#endif
