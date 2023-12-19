#include "GpsInterface.h"

#ifdef HAS_GPS

extern GpsInterface gps_obj;

char nmeaBuffer[100];

MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

#ifndef GPS_SOFTWARE_SERIAL
  HardwareSerial Serial2(GPS_SERIAL_INDEX);
#else
  EspSoftwareSerial::UART Serial2;
#endif

void GpsInterface::begin() {

  #ifndef GPS_SOFTWARE_SERIAL
    Serial2.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);
  #else
    Serial2.begin(9600, SWSERIAL_8N1, GPS_TX, GPS_RX);
  #endif

  MicroNMEA::sendSentence(Serial2, "$PSTMSETPAR,1201,0x00000042");
  MicroNMEA::sendSentence(Serial2, "$PSTMSAVEPAR");

  MicroNMEA::sendSentence(Serial2, "$PSTMSRR");

  delay(4000);

  if (Serial2.available()) {
    Serial.println("GPS Attached Successfully");
    this->gps_enabled = true;
    while (Serial2.available())
      Serial2.read();
  }

  this->type_flag=GPSTYPE_NATIVE; //enforce default
  this->disable_queue(); //init the queue, disabled, kill NULLs

  nmea.setUnknownSentenceHandler(gps_nmea_notimp);
}

//passthrough for other objects
void gps_nmea_notimp(MicroNMEA& nmea){
  gps_obj.enqueue(nmea);
}

void GpsInterface::enqueue(MicroNMEA& nmea){
  std::string nmea_sentence = std::string(nmea.getSentence());

  if(nmea_sentence.length()){
    this->notimp_nmea_sentence = nmea_sentence.c_str();

    bool unparsed=1;
    bool enqueue=1;

    char system=nmea.getTalkerID();
    String msg_id=nmea.getMessageID();
    int length=nmea_sentence.length();

    if(length>0&&length<256){
      if(system){
        if(msg_id=="TXT"){
          if(length>8){
            std::string content=nmea_sentence.substr(7,std::string::npos);

            int tot_brk=content.find(',');
            int num_brk=content.find(',',tot_brk+1);
            int txt_brk=content.find(',',num_brk+1);
            int chk_brk=content.rfind('*');

            if(tot_brk!=std::string::npos && num_brk!=std::string::npos && txt_brk!=std::string::npos && chk_brk!=std::string::npos
                && chk_brk>txt_brk && txt_brk>num_brk && num_brk>tot_brk && tot_brk>=0){
              std::string total_str=content.substr(0,tot_brk);
              std::string num_str=content.substr(tot_brk+1,num_brk-tot_brk-1);
              std::string type_str=content.substr(num_brk+1,txt_brk-num_brk-1);
              std::string text_str=content.substr(txt_brk+1,chk_brk-txt_brk-1);
              std::string checksum=content.substr(chk_brk+1,std::string::npos);

              int total=0;
              if(total_str.length()) total=atoi(total_str.c_str());

              int num=0;
              if(num_str.length()) num=atoi(num_str.c_str());

              int type=0;
              if(type_str.length()) type=atoi(type_str.c_str());

              if(text_str.length() && checksum.length()){
                String text=text_str.c_str();
                if(type>1){
                  char type_cstr[4];
                  snprintf(type_cstr, 4, "%02d ", type);
                  type_cstr[3]='\0';
                  text=type_cstr+text;
                }

                if((num<=1||total<=1) && this->queue_enabled_flag){
                  if(this->text){
                    if(this->text_in){
                      int size=text_in->size();
                      if(size){
                        #ifdef GPS_TEXT_MAXCOPIES
                          if(this->text_cycles>=GPS_TEXT_MAXCOPIES){
                        #else
                          if(this->text_cycles){
                        #endif
                            if(this->text->size()){
                              LinkedList<String> *delme=this->text;
                              this->text=new LinkedList<String>;
                              delete delme;
                              this->text_cycles=0;
                            }
                          }
                        
                        for(int i=0;i<size;i++){
                          this->text->add(this->text_in->get(i));
                        }
                        LinkedList<String> *delme=this->text_in;
                        this->text_in=new LinkedList<String>;
                        delete delme;
                        this->text_cycles++;

                        this->gps_text=text;
                      }
                    }
                    else
                      this->text_in=new LinkedList<String>;
                  }
                  else{
                    if(this->text_in){
                      this->text_cycles=0;
                      this->text=this->text_in;
                      if(this->text->size()){
                        if(this->gps_text=="") this->gps_text=this->text->get(0);
                        this->text_cycles++;
                      }
                      this->text_in=new LinkedList<String>;
                    }
                    else {
                      this->text_cycles=0;
                      this->text=new LinkedList<String>;
                      this->text_in=new LinkedList<String>;
                    }
                  }

                  this->text_in->add(text);
                }
                else if(this->queue_enabled_flag){
                  if(!this->text_in) this->text_in=new LinkedList<String>;
                  this->text_in->add(text);
                  int size=this->text_in->size();

                  #ifdef GPS_TEXT_MAXLINES
                    if(size>=GPS_TEXT_MAXLINES){
                  #else
                    if(size>=5){
                  #endif
                      #ifdef GPS_TEXT_MAXCOPIES
                        if(this->text_cycles>=GPS_TEXT_MAXCOPIES){
                      #else
                        if(this->text_cycles){
                      #endif
                          if(this->text->size()){
                            LinkedList<String> *delme=this->text;
                            this->text=new LinkedList<String>;
                            delete delme;
                            this->text_cycles=0;
                          }
                        }
                      
                        for(int i=0;i<size;i++)
                          this->text->add(this->text_in->get(i));

                        LinkedList<String> *delme=this->text_in;
                        this->text_in=new LinkedList<String>;
                        delete delme;
                        this->text_cycles++;
                      }
                }
                else
                  if(num<=1||total<=1) this->gps_text=text;

                if(this->gps_text=="") this->gps_text=text;
                unparsed=0;
              }
            }
          }
        }
      }
    }

    if(unparsed)
      this->notparsed_nmea_sentence = nmea_sentence.c_str();

    if(this->queue_enabled_flag){
      if(!this->queue) this->new_queue();
      if(enqueue){
        String enqueue_me=nmea_sentence.c_str();
        this->queue->add(enqueue_me);
      }
    }
    else
      this->flush_queue();
  }
  else
    if(!this->queue_enabled_flag)
      this->flush_queue();
}

void GpsInterface::enable_queue(){
  if(this->queue_enabled_flag){
    if(!this->queue)
      this->new_queue();
    if(!this->text)
      this->text=new LinkedList<String>;
    if(!this->text_in)
      this->text_in=new LinkedList<String>;
  }
  else {
    this->flush_queue();
    this->queue_enabled_flag=1;
  }
}

void GpsInterface::disable_queue(){
  this->queue_enabled_flag=0;
  this->flush_queue();
}

bool GpsInterface::queue_enabled(){
  return this->queue_enabled_flag;
}

LinkedList<String>* GpsInterface::get_queue(){
  return this->queue;
}

void GpsInterface::new_queue(){
  this->queue=new LinkedList<String>;
}

void GpsInterface::flush_queue(){
  this->flush_queue_nmea();
  this->flush_text();
}

void GpsInterface::flush_queue_nmea(){
  if(this->queue){
    if(this->queue->size()){
      LinkedList<String> *delme=this->queue;
      this->new_queue();
      delete delme;
    }
  }
  else
    this->new_queue();
}

void GpsInterface::flush_text(){
  this->flush_queue_text();
  this->flush_queue_textin();
}

void GpsInterface::flush_queue_text(){
  this->text_cycles=0;

  if(this->text){
    if(this->text->size()){
      LinkedList<String> *delme=this->text;
      this->text=new LinkedList<String>;
      delete delme;
    }
  }
  else
    this->text=new LinkedList<String>;
}

void GpsInterface::flush_queue_textin(){
  if(this->text_in){
    if(this->text_in->size()){
      LinkedList<String> *delme=this->text_in;
      this->text_in=new LinkedList<String>;
      delete delme;
    }
  }
  else
    this->text_in=new LinkedList<String>;
}

void GpsInterface::sendSentence(const char* sentence){
  MicroNMEA::sendSentence(Serial2, sentence);
}

void GpsInterface::sendSentence(Stream &s, const char* sentence){
  MicroNMEA::sendSentence(s, sentence);
}

void GpsInterface::setType(String t){
  if(t == "native")
    this->type_flag=GPSTYPE_NATIVE;
  else if(t == "gps")
    this->type_flag=GPSTYPE_GPS;
  else if(t == "glonass")
    this->type_flag=GPSTYPE_GLONASS;
  else if(t == "galileo")
    this->type_flag=GPSTYPE_GALILEO;
  else if(t == "navic")
    this->type_flag=GPSTYPE_NAVIC;
  else if(t == "qzss")
    this->type_flag=GPSTYPE_QZSS;        
  else if(t == "beidou")
    this->type_flag=GPSTYPE_BEIDOU;
  else if(t == "beidou_bd")
    this->type_flag=GPSTYPE_BEIDOU_BD;    
  else
    this->type_flag=GPSTYPE_ALL;
}

String GpsInterface::generateGXgga(){
  String msg_type="$"+this->generateType()+"GGA,";

  char timeStr[8];
  snprintf(timeStr, 8, "%02d%02d%02d,", (int)(nmea.getHour()), (int)(nmea.getMinute()), (int)(nmea.getSecond()));

  long lat = nmea.getLatitude();
  char latDir = lat < 0 ? 'S' : 'N';
  lat = abs(lat);
  char latStr[12];
  snprintf(latStr, 12, "%02ld%08.5f,", lat / 1000000, ((lat % 1000000)*60) / 1000000.0);

  long lon = nmea.getLongitude();
  char lonDir = lon < 0 ? 'W' : 'E';
  lon = abs(lon);
  char lonStr[13];
  snprintf(lonStr, 13, "%03ld%08.5f,", lon / 1000000, ((lon % 1000000)*60) / 1000000.0);

  int fixQuality = nmea.isValid() ? 1 : 0;
  char fixStr[3];
  snprintf(fixStr, 3, "%01d,", fixQuality);

  int numSatellites = nmea.getNumSatellites();
  char satStr[4];
  snprintf(satStr, 4, "%02d,", numSatellites);

  unsigned long hdop = nmea.getHDOP();
  char hdopStr[13];
  snprintf(hdopStr, 13, "%01.2f,", 2.5 * (((float)(hdop))/10));

  long altitude;
  if(!nmea.getAltitude(altitude)) altitude=0;
  char altStr[9];
  snprintf(altStr, 9, "%01.1f,", altitude/1000.0);

  String message = msg_type + timeStr + latStr + latDir + ',' + lonStr + lonDir +
                    ',' + fixStr + satStr + hdopStr + altStr + "M,,M,,";

  return message;
}

String GpsInterface::generateGXrmc(){
  String msg_type="$"+this->generateType()+"RMC,";

  char timeStr[8];
  snprintf(timeStr, 8, "%02d%02d%02d,", (int)(nmea.getHour()), (int)(nmea.getMinute()), (int)(nmea.getSecond()));

  char dateStr[8];
  snprintf(dateStr, 8, "%02d%02d%02d,", (int)(nmea.getDay()), (int)(nmea.getMonth()), (int)(nmea.getYear()%100));

  char status = nmea.isValid() ? 'A' : 'V';
  char mode = nmea.isValid() ? 'A' : 'N';

  long lat = nmea.getLatitude();
  char latDir = lat < 0 ? 'S' : 'N';
  lat = abs(lat);
  char latStr[12];
  snprintf(latStr, 12, "%02ld%08.5f,", lat / 1000000, ((lat % 1000000)*60) / 1000000.0);

  long lon = nmea.getLongitude();
  char lonDir = lon < 0 ? 'W' : 'E';
  lon = abs(lon);
  char lonStr[13];
  snprintf(lonStr, 13, "%03ld%08.5f,", lon / 1000000, ((lon % 1000000)*60) / 1000000.0);

  char speedStr[8];
  snprintf(speedStr, 8, "%01.1f,", nmea.getSpeed() / 1000.0);

  char courseStr[7];
  snprintf(courseStr, 7, "%01.1f,", nmea.getCourse() / 1000.0);

  String message = msg_type + timeStr + status + ',' + latStr + latDir + ',' +
                    lonStr + lonDir + ',' + speedStr + courseStr + dateStr + ',' + ',' + mode;
  return message;
}

String GpsInterface::generateType(){
  String msg_type="";

  if(this->type_flag<8) //8=BeiDou in BD mode
    msg_type+='G';

  if(this->type_flag == GPSTYPE_NATIVE){ //type_flag=0
    char system=this->nav_system;
    if(system)
      msg_type+=system;
    else
      msg_type+='N';
  }
  else if(this->type_flag == GPSTYPE_GPS) //type_flag=2
    msg_type+='P';
  else if(this->type_flag == GPSTYPE_GLONASS) //type_flag=3
    msg_type+='L';
  else if(this->type_flag == GPSTYPE_GALILEO) //type_flag=4
    msg_type+='A';
  else if(this->type_flag == GPSTYPE_NAVIC) //type_flag=5
    msg_type+='I';
  else if(this->type_flag == GPSTYPE_QZSS) //type_flag=6
    msg_type+='Q';
  else if(this->type_flag == GPSTYPE_BEIDOU) //type_flag=7
    msg_type+='B';
  else if(this->type_flag == GPSTYPE_BEIDOU_BD){ //type_flag=8
    msg_type+='B';
    msg_type+='D';
  }
  else{ //type_flag=1=all ... also default if unset/wrong (obj default is type_flag=0=native)
    if(this->type_flag>=8) //catch uncaught first char, assume G if not already output
      msg_type+='G';
    msg_type+='N';
  }

  return msg_type;
}

// Thanks JosephHewitt
String GpsInterface::dt_string_from_gps(){
  //Return a datetime String using GPS data only.
  String datetime = "";
  if (nmea.isValid() && nmea.getYear() > 0){
    datetime += nmea.getYear();
    datetime += "-";
    datetime += nmea.getMonth();
    datetime += "-";
    datetime += nmea.getDay();
    datetime += " ";
    datetime += nmea.getHour();
    datetime += ":";
    datetime += nmea.getMinute();
    datetime += ":";
    datetime += nmea.getSecond();
  }
  return datetime;
}

void GpsInterface::setGPSInfo() {
  String nmea_sentence = String(nmea.getSentence());
  if(nmea_sentence != "") this->nmea_sentence = nmea_sentence;

  this->good_fix = nmea.isValid();
  this->nav_system = nmea.getNavSystem();
  this->num_sats = nmea.getNumSatellites();

  this->datetime = this->dt_string_from_gps();

  this->lat = String((float)nmea.getLatitude()/1000000, 7);
  this->lon = String((float)nmea.getLongitude()/1000000, 7);
  long alt = 0;
  if (!nmea.getAltitude(alt)){
    alt = 0;
  }
  this->altf = (float)alt / 1000;

  this->accuracy = 2.5 * ((float)nmea.getHDOP()/10);

  //nmea.clear();
}

float GpsInterface::getAccuracy() {
  return this->accuracy;
}

String GpsInterface::getLat() {
  return this->lat;
}

String GpsInterface::getLon() {
  return this->lon;
}

float GpsInterface::getAlt() {
  return this->altf;
}

String GpsInterface::getDatetime() {
  return this->datetime;
}

String GpsInterface::getNumSatsString() {
  return (String)num_sats;
}

bool GpsInterface::getFixStatus() {
  return this->good_fix;
}

String GpsInterface::getFixStatusAsString() {
  if (this->getFixStatus())
    return "Yes";
  else
    return "No";
}

bool GpsInterface::getGpsModuleStatus() {
  return this->gps_enabled;
}

String GpsInterface::getText() {
  return this->gps_text;
}

int GpsInterface::getTextQueueSize() {
  if(this->queue_enabled_flag){
    bool exists=0;
    if(this->text){
      int size=this->text->size();
      if(size) return size;
      exists=1;
    }
    if(this->text_in){
      int size=this->text_in->size();
      if(size) return size;
      exists=1;
    }
    if(exists)
      return 0;
    else
      return -2;
  }
  else
    return -1;
}

String GpsInterface::getTextQueue(bool flush) {
  if(this->queue_enabled_flag){
    if(this->text){
      int size=this->text->size();
      if(size){
        String text;
        for(int i=0;i<size;i++){
          String now=this->text_in->get(i);
          if(now!=""){
            if(text!=""){
              text+='\r';
              text+='\n';
            }
            text+=now;
          }
        }
        if(flush){
          LinkedList<String> *delme=this->text;
          this->text_cycles=0;
          this->text=this->text_in;
          if(!this->text) this->text=new LinkedList<String>;
          if(this->text->size()) this->text_cycles++;
          this->text_in=new LinkedList<String>;
          delete delme;
        }
        return text;
      }
    }
    else{
      this->text=new LinkedList<String>;
      this->text_cycles=0;
    }

    if(this->text_in){
      int size=this->text_in->size();
      if(size){
        LinkedList<String> *buffer=this->text_in;
        if(flush)
          this->text_in=new LinkedList<String>;
        String text;
        for(int i=0;i<size;i++){
          String now=buffer->get(i);
          if(now!=""){
            if(text!=""){
              text+='\r';
              text+='\n';
            }
            text+=now;
          }
        }
        if(flush)
          delete buffer;
        return text;
      }
    }
    else
      this->text_in=new LinkedList<String>;

    return this->gps_text;
  }
  else
    return this->gps_text;
}

String GpsInterface::getNmea() {
  return this->nmea_sentence;
}

String GpsInterface::getNmeaNotimp() {
  return this->notimp_nmea_sentence;
}

String GpsInterface::getNmeaNotparsed() {
  return this->notparsed_nmea_sentence;
}

void GpsInterface::main() {
  while (Serial2.available()) {
    //Fetch the character one by one
    char c = Serial2.read();
    //Serial.print(c);
    //Pass the character to the library
    nmea.process(c);
  }

  uint8_t num_sat = nmea.getNumSatellites();

  if ((nmea.isValid()) && (num_sat > 0))
    this->setGPSInfo();

  else if ((!nmea.isValid()) && (num_sat <= 0)) {
    this->setGPSInfo();
  }
}
#endif
