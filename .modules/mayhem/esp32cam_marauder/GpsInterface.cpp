#include "GpsInterface.h"

#ifdef HAS_GPS

char nmeaBuffer[100];

MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

HardwareSerial Serial2(GPS_SERIAL_INDEX);

void GpsInterface::begin() {

  Serial2.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);

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
  this->good_fix = nmea.isValid();
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