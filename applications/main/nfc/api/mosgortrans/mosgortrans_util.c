#include "mosgortrans_util.h"

void from_days_to_datetime(uint32_t days, DateTime* datetime, uint16_t start_year) {
    uint32_t timestamp = days * 24 * 60 * 60;
    DateTime start_datetime = {0};
    start_datetime.year = start_year - 1;
    start_datetime.month = 12;
    start_datetime.day = 31;
    timestamp += datetime_datetime_to_timestamp(&start_datetime);
    datetime_timestamp_to_datetime(timestamp, datetime);
}

void from_minutes_to_datetime(uint32_t minutes, DateTime* datetime, uint16_t start_year) {
    uint32_t timestamp = minutes * 60;
    DateTime start_datetime = {0};
    start_datetime.year = start_year - 1;
    start_datetime.month = 12;
    start_datetime.day = 31;
    timestamp += datetime_datetime_to_timestamp(&start_datetime);
    datetime_timestamp_to_datetime(timestamp, datetime);
}

void from_seconds_to_datetime(uint32_t seconds, DateTime* datetime, uint16_t start_year) {
    uint32_t timestamp = seconds;
    DateTime start_datetime = {0};
    start_datetime.year = start_year - 1;
    start_datetime.month = 12;
    start_datetime.day = 31;
    timestamp += datetime_datetime_to_timestamp(&start_datetime);
    datetime_timestamp_to_datetime(timestamp, datetime);
}

typedef struct {
    uint16_t view; //101
    uint16_t type; //102
    uint8_t layout; //111
    uint8_t layout2; //112
    uint16_t blank_type; //121
    uint16_t type_of_extended; //122
    uint8_t extended; //123
    uint8_t benefit_code; //124
    uint32_t number; //201
    uint16_t use_before_date; //202
    uint16_t use_with_date; //205
    uint8_t requires_activation; //301
    uint16_t activate_during; //302
    uint16_t extension_counter; //304
    uint8_t blocked; //303
    uint32_t valid_from_date; //311
    uint16_t valid_to_date; //312
    uint32_t valid_for_minutes; //314
    uint32_t valid_to_time; //317
    uint16_t remaining_trips; //321
    uint32_t remaining_funds; //322
    uint16_t total_trips; //331
    uint16_t start_trip_date; //402
    uint16_t start_trip_time; //403
    uint32_t start_trip_neg_minutes; //404
    uint32_t start_trip_minutes; //405
    uint8_t start_trip_seconds; //406
    uint8_t minutes_pass; //412
    uint8_t metro_ride_with; //414
    uint8_t transport_type; //421
    uint8_t transport_type_flag; //421.0
    uint8_t transport_type1; //421.1
    uint8_t transport_type2; //421.2
    uint8_t transport_type3; //421.3
    uint8_t transport_type4; //421.4
    uint16_t validator; //422
    uint8_t validator1; //422.1
    uint16_t validator2; //422.2
    uint16_t route; //424
    uint8_t passage_in_metro; //431
    uint8_t transfer_in_metro; //432
    uint8_t passages_ground_transport; //433
    uint8_t fare_trip; //441
    uint16_t crc16; //501.1
    uint16_t crc16_2; //501.2
    uint32_t hash; //502
    uint16_t hash1; //502.1
    uint32_t hash2; //502.2
    uint8_t geozone_a; //GeoZoneA
    uint8_t geozone_b; //GeoZoneB
    uint8_t company; //Company
    uint8_t units; //Units
    uint16_t rfu1; //rfu1
    uint8_t rfu2; //rfu2
    uint8_t write_enabled; //write_enabled
    uint32_t tech_code; //TechCode
    uint8_t interval; //Interval
    uint16_t app_code1; //AppCode1
    uint16_t app_code2; //AppCode2
    uint16_t app_code3; //AppCode3
    uint16_t app_code4; //AppCode4
    uint16_t type1; //Type1
    uint16_t type2; //Type2
    uint16_t type3; //Type3
    uint16_t type4; //Type4

} BlockData;

void parse_layout_2(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0x00, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 0x0A, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 0x14, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 0x34, 4); //111
    data_block->use_before_date = bit_lib_get_bits_16(block->data, 0x38, 16); //202
    data_block->benefit_code = bit_lib_get_bits(block->data, 0x48, 8); //124
    data_block->rfu1 = bit_lib_get_bits_32(block->data, 0x50, 32); //rfu1
    data_block->crc16 = bit_lib_get_bits_16(block->data, 0x70, 16); //501.1
    data_block->blocked = bit_lib_get_bits(block->data, 0x80, 1); //303
    data_block->start_trip_time = bit_lib_get_bits_16(block->data, 0x81, 12); //403
    data_block->start_trip_date = bit_lib_get_bits_16(block->data, 0x8D, 16); //402
    data_block->valid_from_date = bit_lib_get_bits_16(block->data, 0x9D, 16); //311
    data_block->valid_to_date = bit_lib_get_bits_16(block->data, 0xAD, 16); //312
    data_block->start_trip_seconds = bit_lib_get_bits(block->data, 0xDB, 6); //406
    data_block->transport_type1 = bit_lib_get_bits(block->data, 0xC3, 2); //421.1
    data_block->transport_type2 = bit_lib_get_bits(block->data, 0xC5, 2); //421.2
    data_block->transport_type3 = bit_lib_get_bits(block->data, 0xC7, 2); //421.3
    data_block->transport_type4 = bit_lib_get_bits(block->data, 0xC9, 2); //421.4
    data_block->use_with_date = bit_lib_get_bits_16(block->data, 0xBD, 16); //205
    data_block->route = bit_lib_get_bits(block->data, 0xCD, 1); //424
    data_block->validator1 = bit_lib_get_bits_16(block->data, 0xCE, 15); //422.1
    data_block->validator = bit_lib_get_bits_16(block->data, 0xCD, 16);
    data_block->total_trips = bit_lib_get_bits_16(block->data, 0xDD, 16); //331
    data_block->write_enabled = bit_lib_get_bits(block->data, 0xED, 1); //write_enabled
    data_block->rfu2 = bit_lib_get_bits(block->data, 0xEE, 2); //rfu2
    data_block->crc16_2 = bit_lib_get_bits_16(block->data, 0xF0, 16); //501.2
}

void parse_layout_6(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0x00, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 0x0A, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 0x14, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 0x34, 4); //111
    data_block->use_before_date = bit_lib_get_bits_16(block->data, 0x38, 16); //202
    data_block->geozone_a = bit_lib_get_bits(block->data, 0x48, 4); //GeoZoneA
    data_block->geozone_b = bit_lib_get_bits(block->data, 0x4C, 4); //GeoZoneB
    data_block->blank_type = bit_lib_get_bits_16(block->data, 0x50, 10); //121
    data_block->type_of_extended = bit_lib_get_bits_16(block->data, 0x5A, 10); //122
    data_block->rfu1 = bit_lib_get_bits_16(block->data, 0x64, 12); //rfu1
    data_block->crc16 = bit_lib_get_bits_16(block->data, 0x70, 16); //501.1
    data_block->blocked = bit_lib_get_bits(block->data, 0x80, 1); //303
    data_block->start_trip_time = bit_lib_get_bits_16(block->data, 0x81, 12); //403
    data_block->start_trip_date = bit_lib_get_bits_16(block->data, 0x8D, 16); //402
    data_block->valid_from_date = bit_lib_get_bits_16(block->data, 0x9D, 16); //311
    data_block->valid_to_date = bit_lib_get_bits_16(block->data, 0xAD, 16); //312
    data_block->company = bit_lib_get_bits(block->data, 0xBD, 4); //Company
    data_block->validator1 = bit_lib_get_bits(block->data, 0xC1, 4); //422.1
    data_block->remaining_trips = bit_lib_get_bits_16(block->data, 0xC5, 10); //321
    data_block->units = bit_lib_get_bits(block->data, 0xCF, 6); //Units
    data_block->validator2 = bit_lib_get_bits_16(block->data, 0xD5, 10); //422.2
    data_block->total_trips = bit_lib_get_bits_16(block->data, 0xDF, 16); //331
    data_block->extended = bit_lib_get_bits(block->data, 0xEF, 1); //123
    data_block->crc16_2 = bit_lib_get_bits_16(block->data, 0xF0, 16); //501.2
}

void parse_layout_8(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 10, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 20, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 52, 4); //111
    data_block->use_before_date = bit_lib_get_bits_16(block->data, 56, 16); //202
    data_block->hash = bit_lib_get_bits_32(block->data, 192, 32);
}

void parse_layout_A(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 10, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 20, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 52, 4); //111
    data_block->start_trip_minutes = bit_lib_get_bits_32(block->data, 96, 19);
    data_block->minutes_pass = bit_lib_get_bits(block->data, 119, 7);
    data_block->hash = bit_lib_get_bits_32(block->data, 192, 32);
}

void parse_layout_C(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 10, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 20, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 52, 4); //111
    data_block->use_before_date = bit_lib_get_bits_16(block->data, 56, 16); //202
    data_block->hash = bit_lib_get_bits_32(block->data, 192, 32);
}

void parse_layout_D(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 10, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 20, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 52, 4); //111
    data_block->use_before_date = bit_lib_get_bits_16(block->data, 64, 16); //202
    data_block->hash = bit_lib_get_bits_32(block->data, 192, 32);
}

void parse_layout_E1(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0x00, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 0x0A, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 0x14, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 0x34, 4); //111
    data_block->layout2 = bit_lib_get_bits(block->data, 0x38, 5); //112
    data_block->use_before_date = bit_lib_get_bits_16(block->data, 0x3D, 16); //202
    data_block->blank_type = bit_lib_get_bits_16(block->data, 0x4D, 10); //121
    data_block->validator = bit_lib_get_bits_16(block->data, 0x80, 16); //422
    data_block->minutes_pass = bit_lib_get_bits(block->data, 0xB9, 8); //412
    data_block->remaining_funds = bit_lib_get_bits_32(block->data, 0x4C, 19); //322
    data_block->blocked = bit_lib_get_bits(block->data, 0x9D, 1); //303
    data_block->hash = bit_lib_get_bits_32(block->data, 0xE0, 32); //502
}

void parse_layout_E2(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0x00, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 0x0A, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 0x14, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 0x34, 4); //111
    data_block->layout2 = bit_lib_get_bits(block->data, 0x38, 5); //112
    data_block->type_of_extended = bit_lib_get_bits_16(block->data, 0x3D, 10); //122
    data_block->use_before_date = bit_lib_get_bits_16(block->data, 0x47, 16); //202
    data_block->blank_type = bit_lib_get_bits_16(block->data, 0x57, 10); //121
    data_block->valid_from_date = bit_lib_get_bits_16(block->data, 0x61, 16); //311
    data_block->activate_during = bit_lib_get_bits_16(block->data, 0x71, 9); //302
    data_block->valid_for_minutes = bit_lib_get_bits_32(block->data, 0x83, 20); //314
    data_block->minutes_pass = bit_lib_get_bits(block->data, 0x9A, 8); //412
    data_block->transport_type = bit_lib_get_bits(block->data, 0xA3, 2); //421
    data_block->passage_in_metro = bit_lib_get_bits(block->data, 0xA5, 1); //431
    data_block->transfer_in_metro = bit_lib_get_bits(block->data, 0xA6, 1); //432
    data_block->remaining_trips = bit_lib_get_bits_16(block->data, 0xA7, 10); //321
    data_block->validator = bit_lib_get_bits_16(block->data, 0xB1, 16); //422
    data_block->start_trip_neg_minutes = bit_lib_get_bits_32(block->data, 0xC4, 20); //404
    data_block->requires_activation = bit_lib_get_bits(block->data, 0xD8, 1); //301
    data_block->blocked = bit_lib_get_bits(block->data, 0xD9, 1); //303
    data_block->extended = bit_lib_get_bits(block->data, 0xDA, 1); //123
    data_block->hash = bit_lib_get_bits_32(block->data, 0xE0, 32); //502
}

void parse_layout_E3(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0x00, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 0x0A, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 0x14, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 0x34, 4); //111
    data_block->layout2 = bit_lib_get_bits(block->data, 0x38, 5); //112
    data_block->use_before_date = bit_lib_get_bits_16(block->data, 61, 16); //202
    data_block->blank_type = bit_lib_get_bits_16(block->data, 0x4D, 10); //121
    data_block->remaining_funds = bit_lib_get_bits_32(block->data, 0xBC, 22); //322
    data_block->hash = bit_lib_get_bits_32(block->data, 224, 32); //502
    data_block->validator = bit_lib_get_bits_16(block->data, 0x80, 16); //422
    data_block->start_trip_minutes = bit_lib_get_bits_32(block->data, 0x90, 23); //405
    data_block->fare_trip = bit_lib_get_bits(block->data, 0xD2, 2); //441
    data_block->minutes_pass = bit_lib_get_bits(block->data, 0xAB, 7); //412
    data_block->transport_type_flag = bit_lib_get_bits(block->data, 0xB2, 2); //421.0
    data_block->transport_type1 = bit_lib_get_bits(block->data, 0xB4, 2); //421.1
    data_block->transport_type2 = bit_lib_get_bits(block->data, 0xB6, 2); //421.2
    data_block->transport_type3 = bit_lib_get_bits(block->data, 0xB8, 2); //421.3
    data_block->transport_type4 = bit_lib_get_bits(block->data, 0xBA, 2); //421.4
    data_block->blocked = bit_lib_get_bits(block->data, 0xD4, 1); //303
}

void parse_layout_E4(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0x00, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 0x0A, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 0x14, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 0x34, 4); //111
    data_block->layout2 = bit_lib_get_bits(block->data, 0x38, 5); //112
    data_block->type_of_extended = bit_lib_get_bits_16(block->data, 0x3D, 10); //122
    data_block->use_before_date = bit_lib_get_bits_16(block->data, 0x47, 13); //202
    data_block->blank_type = bit_lib_get_bits_16(block->data, 0x54, 10); //121
    data_block->valid_from_date = bit_lib_get_bits_16(block->data, 0x5E, 13); //311
    data_block->activate_during = bit_lib_get_bits_16(block->data, 0x6B, 9); //302
    data_block->extension_counter = bit_lib_get_bits_16(block->data, 0x74, 10); //304
    data_block->valid_for_minutes = bit_lib_get_bits_32(block->data, 0x80, 20); //314
    data_block->minutes_pass = bit_lib_get_bits(block->data, 0x98, 7); //412
    data_block->transport_type_flag = bit_lib_get_bits(block->data, 0x9F, 2); //421.0
    data_block->transport_type1 = bit_lib_get_bits(block->data, 0xA1, 2); //421.1
    data_block->transport_type2 = bit_lib_get_bits(block->data, 0xA3, 2); //421.2
    data_block->transport_type3 = bit_lib_get_bits(block->data, 0xA5, 2); //421.3
    data_block->transport_type4 = bit_lib_get_bits(block->data, 0xA7, 2); //421.4
    data_block->remaining_trips = bit_lib_get_bits_16(block->data, 0xA9, 10); //321
    data_block->validator = bit_lib_get_bits_16(block->data, 0xB3, 16); //422
    data_block->start_trip_neg_minutes = bit_lib_get_bits_32(block->data, 0xC3, 20); //404
    data_block->requires_activation = bit_lib_get_bits(block->data, 0xD7, 1); //301
    data_block->blocked = bit_lib_get_bits(block->data, 0xD8, 1); //303
    data_block->extended = bit_lib_get_bits(block->data, 0xD9, 1); //123
    data_block->hash = bit_lib_get_bits_32(block->data, 0xE0, 32); //502
}

void parse_layout_E5(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0x00, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 0x0A, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 0x14, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 0x34, 4); //111
    data_block->layout2 = bit_lib_get_bits(block->data, 0x38, 5); //112
    data_block->use_before_date = bit_lib_get_bits_16(block->data, 0x3D, 13); //202
    data_block->blank_type = bit_lib_get_bits_16(block->data, 0x4A, 10); //121
    data_block->valid_to_time = bit_lib_get_bits_32(block->data, 0x54, 23); //317
    data_block->extension_counter = bit_lib_get_bits_16(block->data, 0x6B, 10); //304
    data_block->start_trip_minutes = bit_lib_get_bits_32(block->data, 0x80, 23); //405
    data_block->metro_ride_with = bit_lib_get_bits(block->data, 0x97, 7); //414
    data_block->minutes_pass = bit_lib_get_bits(block->data, 0x9E, 7); //412
    data_block->remaining_funds = bit_lib_get_bits_32(block->data, 0xA7, 19); //322
    data_block->validator = bit_lib_get_bits_16(block->data, 0xBA, 16); //422
    data_block->blocked = bit_lib_get_bits(block->data, 0xCA, 1); //303
    data_block->route = bit_lib_get_bits_16(block->data, 0xCC, 12); //424
    data_block->passages_ground_transport = bit_lib_get_bits(block->data, 0xD8, 7); //433
    data_block->hash = bit_lib_get_bits_32(block->data, 0xE0, 32); //502
}

void parse_layout_E6(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0x00, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 0x0A, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 0x14, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 0x34, 4); //111
    data_block->layout2 = bit_lib_get_bits(block->data, 0x38, 5); //112
    data_block->type_of_extended = bit_lib_get_bits_16(block->data, 0x3D, 10); //122
    data_block->use_before_date = bit_lib_get_bits_16(block->data, 0x47, 13); //202
    data_block->blank_type = bit_lib_get_bits_16(block->data, 0x54, 10); //121
    data_block->valid_from_date = bit_lib_get_bits_32(block->data, 0x5E, 23); //311
    data_block->extension_counter = bit_lib_get_bits_16(block->data, 0x75, 10); //304
    data_block->valid_for_minutes = bit_lib_get_bits_32(block->data, 0x80, 20); //314
    data_block->start_trip_neg_minutes = bit_lib_get_bits_32(block->data, 0x94, 20); //404
    data_block->metro_ride_with = bit_lib_get_bits(block->data, 0xA8, 7); //414
    data_block->minutes_pass = bit_lib_get_bits(block->data, 0xAF, 7); //412
    data_block->remaining_trips = bit_lib_get_bits(block->data, 0xB6, 7); //321
    data_block->validator = bit_lib_get_bits_16(block->data, 0xBD, 16); //422
    data_block->blocked = bit_lib_get_bits(block->data, 0xCD, 1); //303
    data_block->extended = bit_lib_get_bits(block->data, 0xCE, 1); //123
    data_block->route = bit_lib_get_bits_16(block->data, 0xD4, 12); //424
    data_block->hash = bit_lib_get_bits_32(block->data, 0xE0, 32); //502
}

void parse_layout_FCB(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0x00, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 0x0A, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 0x14, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 0x34, 4); //111
    data_block->tech_code = bit_lib_get_bits_32(block->data, 0x38, 10); //tech_code
    data_block->valid_from_date = bit_lib_get_bits_16(block->data, 0x42, 16); //311
    data_block->valid_to_date = bit_lib_get_bits_16(block->data, 0x52, 16); //312
    data_block->interval = bit_lib_get_bits(block->data, 0x62, 4); //interval
    data_block->app_code1 = bit_lib_get_bits_16(block->data, 0x66, 10); //app_code1
    data_block->hash1 = bit_lib_get_bits_16(block->data, 0x70, 16); //502.1
    data_block->type1 = bit_lib_get_bits_16(block->data, 0x80, 10); //type1
    data_block->app_code2 = bit_lib_get_bits_16(block->data, 0x8A, 10); //app_code2
    data_block->type2 = bit_lib_get_bits_16(block->data, 0x94, 10); //type2
    data_block->app_code3 = bit_lib_get_bits_16(block->data, 0x9E, 10); //app_code3
    data_block->type3 = bit_lib_get_bits_16(block->data, 0xA8, 10); //type3
    data_block->app_code4 = bit_lib_get_bits_16(block->data, 0xB2, 10); //app_code4
    data_block->type4 = bit_lib_get_bits_16(block->data, 0xBC, 10); //type4
    data_block->hash2 = bit_lib_get_bits_32(block->data, 0xE0, 32); //502.2
}

void parse_layout_F0B(BlockData* data_block, const MfClassicBlock* block) {
    data_block->view = bit_lib_get_bits_16(block->data, 0x00, 10); //101
    data_block->type = bit_lib_get_bits_16(block->data, 0x0A, 10); //102
    data_block->number = bit_lib_get_bits_32(block->data, 0x14, 32); //201
    data_block->layout = bit_lib_get_bits(block->data, 0x34, 4); //111
    data_block->tech_code = bit_lib_get_bits_32(block->data, 0x38, 10); //tech_code
    data_block->valid_from_date = bit_lib_get_bits_16(block->data, 0x42, 16); //311
    data_block->valid_to_date = bit_lib_get_bits_16(block->data, 0x52, 16); //312
    data_block->hash1 = bit_lib_get_bits_32(block->data, 0x70, 16); //502.1
}

void parse_transport_type(BlockData* data_block, FuriString* transport) {
    switch(data_block->transport_type_flag) {
    case 0:
        furi_string_cat(transport, "");
        break;
    case 1:
        uint8_t transport_type =
            (data_block->transport_type1 || data_block->transport_type2 ||
             data_block->transport_type3 || data_block->transport_type4);
        switch(transport_type) {
        case 0:
            furi_string_cat(transport, "");
            break;
        case 1:
            furi_string_cat(transport, "Metro");
            break;
        case 2:
            furi_string_cat(transport, "Monorail");
            break;
        case 3:
            furi_string_cat(transport, "MCC");
            break;
        default:
            break;
        }
        break;
    case 2:
        furi_string_cat(transport, "Ground");
        break;
    default:
        furi_string_cat(transport, "Unknown");
        break;
    }
}

bool mosgortrans_parse_transport_block(const MfClassicBlock* block, FuriString* result) {
    BlockData data_block = {};
    const uint16_t valid_departments[] = {0x106, 0x108, 0x10A, 0x10E, 0x110, 0x117};
    uint16_t transport_departament = bit_lib_get_bits_16(block->data, 0, 10);
    bool departament_valid = false;
    for(uint8_t i = 0; i < 6; i++) {
        if(transport_departament == valid_departments[i]) {
            departament_valid = true;
            break;
        }
    }
    if(!departament_valid) {
        return false;
    }
    FURI_LOG_I(TAG2, "Transport departament: %x", transport_departament);

    uint16_t layout_type = bit_lib_get_bits_16(block->data, 52, 4);
    if(layout_type == 0xE) {
        layout_type = bit_lib_get_bits_16(block->data, 52, 9);
    } else if(layout_type == 0xF) {
        layout_type = bit_lib_get_bits_16(block->data, 52, 14);
    }

    FURI_LOG_I(TAG2, "Layout type %x", layout_type);

    uint16_t card_view = 0;
    uint16_t card_type = 0;
    uint32_t card_number = 0;
    uint8_t card_layout = 0;
    uint8_t card_layout2 = 0;
    uint16_t card_use_before_date = 0;
    uint16_t card_blank_type = 0;
    uint32_t card_start_trip_minutes = 0;
    uint8_t card_minutes_pass = 0;
    uint32_t card_remaining_funds = 0;
    uint16_t card_validator = 0;
    uint8_t card_blocked = 0;
    uint32_t card_hash = 0;

    switch(layout_type) {
    case 0x02: {
        parse_layout_2(&data_block, block);

        //number
        furi_string_cat_printf(result, "Number: %010lu\n", data_block.number);
        //use_before_date
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(data_block.use_before_date, &card_use_before_date_s, 1992);
        furi_string_cat_printf(
            result,
            "Use before: %02d.%02d.%04d\n",
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year);

        if(data_block.valid_from_date == 0 || data_block.valid_to_date == 0) {
            furi_string_cat(result, "\e#No ticket\n");
            return true;
        }
        //remaining_trips
        furi_string_cat_printf(result, "Remaining trips: %d\n", data_block.total_trips);
        //valid_from_date
        DateTime card_valid_from_date_s = {0};
        from_days_to_datetime(data_block.valid_from_date, &card_valid_from_date_s, 1992);
        furi_string_cat_printf(
            result,
            "Valid from: %02d.%02d.%04d\n",
            card_valid_from_date_s.day,
            card_valid_from_date_s.month,
            card_valid_from_date_s.year);
        //valid_to_date
        DateTime card_valid_to_date_s = {0};
        from_days_to_datetime(data_block.valid_to_date, &card_valid_to_date_s, 1992);
        furi_string_cat_printf(
            result,
            "Valid to: %02d.%02d.%04d\n",
            card_valid_to_date_s.day,
            card_valid_to_date_s.month,
            card_valid_to_date_s.year);
        //trip_number
        furi_string_cat_printf(result, "Trip number: %d\n", data_block.total_trips);
        //trip_from
        DateTime card_start_trip_minutes_s = {0};
        from_seconds_to_datetime(
            data_block.start_trip_date * 24 * 60 * 60 + data_block.start_trip_time * 60 +
                data_block.start_trip_seconds,
            &card_start_trip_minutes_s,
            1992);
        furi_string_cat_printf(
            result,
            "Trip from: %02d.%02d.%04d %02d:%02d",
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute);
        break;
    }
    case 0x06: {
        parse_layout_6(&data_block, block);
        //number
        furi_string_cat_printf(result, "Number: %010lu\n", data_block.number);
        //use_before_date
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(data_block.use_before_date, &card_use_before_date_s, 1992);
        furi_string_cat_printf(
            result,
            "Use before: %02d.%02d.%04d\n",
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year);
        //remaining_trips
        furi_string_cat_printf(result, "Remaining trips: %d\n", data_block.remaining_trips);
        //valid_from_date
        DateTime card_valid_from_date_s = {0};
        from_days_to_datetime(data_block.valid_from_date, &card_valid_from_date_s, 1992);
        furi_string_cat_printf(
            result,
            "Valid from: %02d.%02d.%04d\n",
            card_valid_from_date_s.day,
            card_valid_from_date_s.month,
            card_valid_from_date_s.year);
        //valid_to_date
        DateTime card_valid_to_date_s = {0};
        from_days_to_datetime(data_block.valid_to_date, &card_valid_to_date_s, 1992);
        furi_string_cat_printf(
            result,
            "Valid to: %02d.%02d.%04d\n",
            card_valid_to_date_s.day,
            card_valid_to_date_s.month,
            card_valid_to_date_s.year);
        //trip_number
        furi_string_cat_printf(result, "Trip number: %d\n", data_block.total_trips);
        //trip_from
        DateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            (data_block.start_trip_date) * 24 * 60 + data_block.start_trip_time,
            &card_start_trip_minutes_s,
            1992);
        furi_string_cat_printf(
            result,
            "Trip from: %02d.%02d.%04d %02d:%02d\n",
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute);
        //validator
        furi_string_cat_printf(
            result, "Validator: %05d", data_block.validator1 * 1024 + data_block.validator2);
        break;
    }
    case 0x08: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_use_before_date = bit_lib_get_bits_16(block->data, 56, 16); //202
        card_hash = bit_lib_get_bits_32(block->data, 192, 32); //502
        uint64_t card_rfu1 = bit_lib_get_bits_64(block->data, 72, 56); //rfu1
        uint16_t card_valid_from_date = bit_lib_get_bits_16(block->data, 128, 16); //311
        uint8_t card_valid_for_days = bit_lib_get_bits(block->data, 144, 8); //313
        uint8_t card_requires_activation = bit_lib_get_bits(block->data, 152, 1); //301
        uint8_t card_rfu2 = bit_lib_get_bits(block->data, 153, 7); //rfu2
        uint8_t card_remaining_trips1 = bit_lib_get_bits(block->data, 160, 8); //321.1
        uint8_t card_remaining_trips = bit_lib_get_bits(block->data, 168, 8); //321
        uint8_t card_validator1 = bit_lib_get_bits(block->data, 193, 2); //422.1
        uint16_t card_validator = bit_lib_get_bits_16(block->data, 177, 15); //422
        uint32_t card_rfu3 = bit_lib_get_bits_32(block->data, 224, 32); //rfu3

        FURI_LOG_D(
            TAG2,
            "%x %x %lx %x %llx %x %x %x %x %x %x %x %x %lx %x %lx",
            card_view,
            card_type,
            card_number,
            card_use_before_date,
            card_rfu1,
            card_valid_from_date,
            card_valid_for_days,
            card_requires_activation,
            card_rfu2,
            card_remaining_trips1,
            card_remaining_trips,
            card_validator1,
            card_validator,
            card_hash,
            card_valid_from_date,
            card_rfu3);
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date, &card_use_before_date_s, 1992);

        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrips left: %d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_remaining_trips,
            card_validator);
        break;
    }
    case 0x0A: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_start_trip_minutes = bit_lib_get_bits_32(block->data, 96, 19); //405
        card_minutes_pass = bit_lib_get_bits(block->data, 119, 7); //412
        card_hash = bit_lib_get_bits_32(block->data, 192, 32); //502
        uint16_t card_valid_from_date = bit_lib_get_bits_16(block->data, 64, 12); //311
        uint32_t card_valid_for_minutes = bit_lib_get_bits_32(block->data, 76, 19); //314
        uint8_t card_requires_activation = bit_lib_get_bits(block->data, 95, 1); //301
        uint8_t card_transport_type_flag = bit_lib_get_bits(block->data, 126, 2); //421.0
        uint8_t card_remaining_trips = bit_lib_get_bits(block->data, 128, 8); //321
        uint16_t card_validator = bit_lib_get_bits_16(block->data, 136, 16); //422
        uint8_t card_transport_type1 = bit_lib_get_bits(block->data, 152, 2); //421.1
        uint8_t card_transport_type2 = bit_lib_get_bits(block->data, 154, 2); //421.2
        uint8_t card_transport_type3 = bit_lib_get_bits(block->data, 156, 2); //421.3
        uint8_t card_transport_type4 = bit_lib_get_bits(block->data, 158, 2); //421.4

        FURI_LOG_D(
            TAG2,
            "%x %x %lx %x %x %lx %x %lx %x %x %x %x %x %x %x %x %lx",
            card_view,
            card_type,
            card_number,
            card_use_before_date,
            card_valid_from_date,
            card_valid_for_minutes,
            card_requires_activation,
            card_start_trip_minutes,
            card_minutes_pass,
            card_transport_type_flag,
            card_remaining_trips,
            card_validator,
            card_transport_type1,
            card_transport_type2,
            card_transport_type3,
            card_transport_type4,
            card_hash);
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date, &card_use_before_date_s, 2016);

        DateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(card_start_trip_minutes, &card_start_trip_minutes_s, 2016);
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nTrips left: %d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_remaining_trips,
            card_validator);
        break;
    }
    case 0x0C: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_use_before_date = bit_lib_get_bits_16(block->data, 56, 16); //202
        card_hash = bit_lib_get_bits_32(block->data, 192, 32); //502
        uint64_t card_rfu1 = bit_lib_get_bits_64(block->data, 72, 56); //rfu1
        uint16_t card_valid_from_date = bit_lib_get_bits_16(block->data, 128, 16); //311
        uint8_t card_valid_for_days = bit_lib_get_bits(block->data, 144, 8); //313
        uint8_t card_requires_activation = bit_lib_get_bits(block->data, 152, 1); //301
        uint16_t card_rfu2 = bit_lib_get_bits_16(block->data, 153, 13); //rfu2
        uint16_t card_remaining_trips = bit_lib_get_bits_16(block->data, 166, 10); //321
        uint16_t card_validator = bit_lib_get_bits_16(block->data, 176, 16); //422
        uint16_t card_start_trip_date = bit_lib_get_bits_16(block->data, 224, 16); //402
        uint16_t card_start_trip_time = bit_lib_get_bits_16(block->data, 240, 11); //403
        uint8_t card_transport_type = bit_lib_get_bits(block->data, 251, 2); //421
        uint8_t card_rfu3 = bit_lib_get_bits(block->data, 253, 2); //rfu3
        uint8_t card_transfer_in_metro = bit_lib_get_bits(block->data, 255, 1); //432

        FURI_LOG_D(
            TAG2,
            "%x %x %lx %x %llx %x %x %x %x %x %x %x %x %x %x %x",
            card_view,
            card_type,
            card_number,
            card_use_before_date,
            card_rfu1,
            card_valid_from_date,
            card_valid_for_days,
            card_requires_activation,
            card_rfu2,
            card_remaining_trips,
            card_validator,
            card_start_trip_date,
            card_start_trip_time,
            card_transport_type,
            card_rfu3,
            card_transfer_in_metro);
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date, &card_use_before_date_s, 1992);
        DateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            (card_start_trip_date) * 24 * 60 + card_start_trip_time,
            &card_start_trip_minutes_s,
            1992);
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nTrips left: %d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_remaining_trips,
            card_validator);
        break;
    }
    case 0x0D: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_use_before_date = bit_lib_get_bits_16(block->data, 64, 16); //202
        card_hash = bit_lib_get_bits_32(block->data, 192, 32); //502
        uint8_t card_rfu1 = bit_lib_get_bits(block->data, 56, 8); //rfu1
        uint16_t card_valid_for_time = bit_lib_get_bits_16(block->data, 80, 11); //316
        uint8_t card_rfu2 = bit_lib_get_bits(block->data, 91, 5); //rfu2
        uint16_t card_use_before_date2 = bit_lib_get_bits_16(block->data, 96, 16); //202.2
        uint16_t card_valid_for_time2 = bit_lib_get_bits_16(block->data, 123, 11); //316.2
        uint8_t card_rfu3 = bit_lib_get_bits(block->data, 123, 5); //rfu3
        uint16_t card_valid_from_date = bit_lib_get_bits_16(block->data, 128, 16); //311
        uint8_t card_valid_for_days = bit_lib_get_bits(block->data, 144, 8); //313
        uint8_t card_requires_activation = bit_lib_get_bits(block->data, 152, 1); //301
        uint8_t card_rfu4 = bit_lib_get_bits(block->data, 153, 2); //rfu4
        uint8_t card_passage_5_minutes = bit_lib_get_bits(block->data, 155, 5); //413
        uint8_t card_transport_type1 = bit_lib_get_bits(block->data, 160, 2); //421.1
        uint8_t card_passage_in_metro = bit_lib_get_bits(block->data, 162, 1); //431
        uint8_t card_passages_ground_transport = bit_lib_get_bits(block->data, 163, 3); //433
        uint16_t card_remaining_trips = bit_lib_get_bits_16(block->data, 166, 10); //321
        uint16_t card_validator = bit_lib_get_bits_16(block->data, 176, 16); //422
        uint16_t card_start_trip_date = bit_lib_get_bits_16(block->data, 224, 16); //402
        uint16_t card_start_trip_time = bit_lib_get_bits_16(block->data, 240, 11); //403
        uint8_t card_transport_type2 = bit_lib_get_bits(block->data, 251, 2); //421.2
        uint8_t card_rfu5 = bit_lib_get_bits(block->data, 253, 2); //rfu5
        uint8_t card_transfer_in_metro = bit_lib_get_bits(block->data, 255, 1); //432

        FURI_LOG_D(
            TAG2,
            "%x %x %lx %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_rfu1,
            card_use_before_date,
            card_valid_for_time,
            card_rfu2,
            card_use_before_date2,
            card_valid_for_time2,
            card_rfu3,
            card_valid_from_date,
            card_valid_for_days,
            card_requires_activation,
            card_rfu4,
            card_passage_5_minutes,
            card_transport_type1,
            card_passage_in_metro,
            card_passages_ground_transport,
            card_remaining_trips,
            card_validator,
            card_start_trip_date,
            card_start_trip_time,
            card_transport_type2,
            card_rfu5,
            card_transfer_in_metro);
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date, &card_use_before_date_s, 1992);
        DateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            (card_start_trip_date) * 24 * 60 + card_start_trip_time,
            &card_start_trip_minutes_s,
            1992);
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nTrips left: %d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_remaining_trips,
            card_validator);
        break;
    }
    case 0xE1:
    case 0x1C1: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_layout2 = bit_lib_get_bits(block->data, 56, 5); //112
        card_use_before_date = bit_lib_get_bits_16(block->data, 61, 16); //202.
        card_blank_type = bit_lib_get_bits_16(block->data, 77, 10); //121.
        card_validator = bit_lib_get_bits_16(block->data, 128, 16); //422
        card_minutes_pass = bit_lib_get_bits(block->data, 185, 8); //412.
        card_remaining_funds = bit_lib_get_bits_32(block->data, 196, 19) / 100; //322
        card_blocked = bit_lib_get_bits(block->data, 202, 1); //303
        card_hash = bit_lib_get_bits_32(block->data, 224, 32); //502
        uint16_t card_start_trip_date = bit_lib_get_bits_16(block->data, 144, 16); //402
        uint16_t card_start_trip_time = bit_lib_get_bits_16(block->data, 160, 11); //403
        uint8_t card_transport_type1 = bit_lib_get_bits(block->data, 171, 2); //421.1
        uint8_t card_transport_type2 = bit_lib_get_bits(block->data, 173, 2); //421.2
        uint8_t card_transfer_in_metro = bit_lib_get_bits(block->data, 177, 1); //432
        uint8_t card_passage_in_metro = bit_lib_get_bits(block->data, 178, 1); //431
        uint8_t card_passages_ground_transport = bit_lib_get_bits(block->data, 179, 3); //433
        uint8_t card_fare_trip = bit_lib_get_bits(block->data, 215, 2); //441
        uint8_t card_zoo = bit_lib_get_bits(block->data, 218, 1); //zoo

        FURI_LOG_D(
            TAG2,
            "%x %x %lx %x %x %x %x %x %x %x %x %x %x %x %x %x %lx %x %x %x %lx",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_layout2,
            card_use_before_date,
            card_blank_type,
            card_validator,
            card_start_trip_date,
            card_start_trip_time,
            card_transport_type1,
            card_transport_type2,
            card_transfer_in_metro,
            card_passage_in_metro,
            card_passages_ground_transport,
            card_minutes_pass,
            card_remaining_funds,
            card_fare_trip,
            card_blocked,
            card_zoo,
            card_hash);
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date, &card_use_before_date_s, 1992);

        DateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(card_start_trip_minutes, &card_start_trip_minutes_s, 1992);
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_validator);
        break;
    }
    case 0xE2:
    case 0x1C2: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_layout2 = bit_lib_get_bits(block->data, 56, 5); //112
        card_use_before_date = bit_lib_get_bits_16(block->data, 71, 16); //202.
        card_blank_type = bit_lib_get_bits_16(block->data, 87, 10); //121.
        card_validator = bit_lib_get_bits_16(block->data, 177, 16); //422
        card_minutes_pass = bit_lib_get_bits(block->data, 154, 8); //412.
        card_blocked = bit_lib_get_bits(block->data, 217, 1); //303
        card_hash = bit_lib_get_bits_32(block->data, 224, 32); //502
        uint16_t card_type_of_extended = bit_lib_get_bits_16(block->data, 61, 10); //122
        uint16_t card_valid_to_date = bit_lib_get_bits_16(block->data, 97, 16); //311
        uint16_t card_activate_during = bit_lib_get_bits_16(block->data, 113, 9); //302
        uint32_t card_valid_for_minutes = bit_lib_get_bits_32(block->data, 131, 20); //314
        uint8_t card_transport_type = bit_lib_get_bits(block->data, 163, 2); //421
        uint8_t card_passage_in_metro = bit_lib_get_bits(block->data, 165, 1); //431
        uint8_t card_transfer_in_metro = bit_lib_get_bits(block->data, 166, 1); //432
        uint16_t card_remaining_trips = bit_lib_get_bits_16(block->data, 167, 10); //321
        uint32_t card_start_trip_neg_minutes = bit_lib_get_bits_32(block->data, 196, 20); //404
        uint8_t card_requires_activation = bit_lib_get_bits(block->data, 216, 1); //301
        uint8_t card_extended = bit_lib_get_bits(block->data, 218, 1); //123

        FURI_LOG_D(
            TAG2,
            "%x %x %lx %x %x %x %x %x %x %x %lx %x %x %x %x %x %x %lx %x %x %x %lx",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_layout2,
            card_type_of_extended,
            card_use_before_date,
            card_blank_type,
            card_valid_to_date,
            card_activate_during,
            card_valid_for_minutes,
            card_minutes_pass,
            card_transport_type,
            card_passage_in_metro,
            card_transfer_in_metro,
            card_remaining_trips,
            card_validator,
            card_start_trip_neg_minutes,
            card_requires_activation,
            card_blocked,
            card_extended,
            card_hash);
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date, &card_use_before_date_s, 2016);

        DateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            (card_valid_to_date) * 24 * 60 + card_valid_for_minutes - card_start_trip_neg_minutes,
            &card_start_trip_minutes_s,
            2016); //-time
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_validator);
        break;
    }
    case 0xE3:
    case 0x1C3: {
        parse_layout_E3(&data_block, block);
        // number
        furi_string_cat_printf(result, "Number: %010lu\n", data_block.number);
        // use_before_date
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(data_block.use_before_date, &card_use_before_date_s, 1992);
        furi_string_cat_printf(
            result,
            "Use before: %02d.%02d.%04d\n",
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year);
        // remaining_funds
        furi_string_cat_printf(result, "Balance: %ld rub\n", data_block.remaining_funds);
        // start_trip_minutes
        DateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(data_block.start_trip_minutes, &card_start_trip_minutes_s, 2016);
        furi_string_cat_printf(
            result,
            "Trip from: %02d.%02d.%04d %02d:%02d\n",
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute);
        // transport
        FuriString* transport = furi_string_alloc();
        parse_transport_type(&data_block, transport);
        furi_string_cat_printf(result, "Transport: %s\n", furi_string_get_cstr(transport));
        // validator
        furi_string_cat_printf(result, "Validator: %05d\n", data_block.validator);
        // fare
        FuriString* fare = furi_string_alloc();
        switch(data_block.fare_trip) {
        case 0:
            furi_string_cat(fare, "");
            break;
        case 1:
            furi_string_cat(fare, "Single");
            break;
        case 2:
            furi_string_cat(fare, "90 minutes");
            break;
        default:
            furi_string_cat(fare, "Unknown");
            break;
        }
        furi_string_cat_printf(result, "Fare: %s", furi_string_get_cstr(fare));
        furi_string_free(fare);
        furi_string_free(transport);
        break;
    }
    case 0xE4:
    case 0x1C4: {
        parse_layout_E4(&data_block, block);

        // number
        furi_string_cat_printf(result, "Number: %010lu\n", data_block.number);
        // use_before_date
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(data_block.use_before_date, &card_use_before_date_s, 2016);
        furi_string_cat_printf(
            result,
            "Use before: %02d.%02d.%04d\n",
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year);
        // remaining_funds
        furi_string_cat_printf(result, "Remaining trips: %ld\n", data_block.remaining_funds);
        // valid_from_date
        DateTime card_use_from_date_s = {0};
        from_days_to_datetime(data_block.valid_from_date, &card_use_from_date_s, 2016);
        furi_string_cat_printf(
            result,
            "Valid from: %02d.%02d.%04d\n",
            card_use_from_date_s.day,
            card_use_from_date_s.month,
            card_use_from_date_s.year);
        // valid_to_date
        DateTime card_use_to_date_s = {0};
        if(data_block.requires_activation) {
            from_days_to_datetime(
                data_block.valid_from_date + data_block.activate_during,
                &card_use_to_date_s,
                2016);
        } else {
            from_minutes_to_datetime(
                data_block.valid_from_date * 24 * 60 + data_block.valid_for_minutes - 1,
                &card_use_to_date_s,
                2016);
        }

        furi_string_cat_printf(
            result,
            "Valid to: %02d.%02d.%04d\n",
            card_use_to_date_s.day,
            card_use_to_date_s.month,
            card_use_to_date_s.year);
        // trip_number
        // furi_string_cat_printf(result, "Trips left: %d", data_block.remaining_trips);
        // trip_from
        DateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            data_block.valid_from_date * 24 * 60 + data_block.valid_for_minutes -
                data_block.start_trip_neg_minutes,
            &card_start_trip_minutes_s,
            2016);
        //transport
        FuriString* transport = furi_string_alloc();
        parse_transport_type(&data_block, transport);
        furi_string_cat_printf(result, "Transport: %s\n", furi_string_get_cstr(transport));
        // validator
        furi_string_cat_printf(result, "Validator: %05d", data_block.validator);
        break;
    }
    case 0xE5:
    case 0x1C5: {
        parse_layout_E5(&data_block, block);
        //number
        furi_string_cat_printf(result, "Number: %010lu\n", data_block.number);
        //use_before_date
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(data_block.use_before_date, &card_use_before_date_s, 2019);
        furi_string_cat_printf(
            result,
            "Use before: %02d.%02d.%04d\n",
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year);
        //remaining_funds
        furi_string_cat_printf(result, "Balance: %ld rub\n", data_block.remaining_funds / 100);
        //start_trip_minutes
        if(data_block.start_trip_minutes) {
            DateTime card_start_trip_minutes_s = {0};
            from_minutes_to_datetime(
                data_block.start_trip_minutes, &card_start_trip_minutes_s, 2019);
            furi_string_cat_printf(
                result,
                "Trip from: %02d.%02d.%04d %02d:%02d\n",
                card_start_trip_minutes_s.day,
                card_start_trip_minutes_s.month,
                card_start_trip_minutes_s.year,
                card_start_trip_minutes_s.hour,
                card_start_trip_minutes_s.minute);
        }
        //start_m_trip_minutes
        if(data_block.metro_ride_with) {
            DateTime card_start_m_trip_minutes_s = {0};
            from_minutes_to_datetime(
                data_block.start_trip_minutes + data_block.metro_ride_with,
                &card_start_m_trip_minutes_s,
                2019);
            furi_string_cat_printf(
                result,
                "(M) from: %02d.%02d.%04d %02d:%02d\n",
                card_start_m_trip_minutes_s.day,
                card_start_m_trip_minutes_s.month,
                card_start_m_trip_minutes_s.year,
                card_start_m_trip_minutes_s.hour,
                card_start_m_trip_minutes_s.minute);
        }
        if(data_block.minutes_pass) {
            DateTime card_start_change_trip_minutes_s = {0};
            from_minutes_to_datetime(
                data_block.start_trip_minutes + data_block.minutes_pass,
                &card_start_change_trip_minutes_s,
                2019);
            furi_string_cat_printf(
                result,
                "Trip edit: %02d.%02d.%04d %02d:%02d\n",
                card_start_change_trip_minutes_s.day,
                card_start_change_trip_minutes_s.month,
                card_start_change_trip_minutes_s.year,
                card_start_change_trip_minutes_s.hour,
                card_start_change_trip_minutes_s.minute);
        }
        //transport
        //validator
        if(data_block.validator) {
            furi_string_cat_printf(result, "Validator: %05d", data_block.validator);
        }
        break;
    }
    case 0xE6:
    case 0x1C6: {
        parse_layout_E6(&data_block, block);
        //number
        furi_string_cat_printf(result, "Number: %010lu\n", data_block.number);
        //use_before_date
        DateTime card_use_before_date_s = {0};
        from_days_to_datetime(data_block.use_before_date, &card_use_before_date_s, 2019);
        furi_string_cat_printf(
            result,
            "Use before: %02d.%02d.%04d\n",
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year);
        //remaining_trips
        furi_string_cat_printf(result, "Trips left: %d\n", data_block.remaining_trips);
        //valid_from_date
        DateTime card_use_from_date_s = {0};
        from_minutes_to_datetime(data_block.valid_from_date, &card_use_from_date_s, 2019);
        furi_string_cat_printf(
            result,
            "Valid from: %02d.%02d.%04d\n",
            card_use_from_date_s.day,
            card_use_from_date_s.month,
            card_use_from_date_s.year);
        //valid_to_date
        DateTime card_use_to_date_s = {0};
        from_minutes_to_datetime(
            data_block.valid_from_date + data_block.valid_for_minutes - 1,
            &card_use_to_date_s,
            2019);
        furi_string_cat_printf(
            result,
            "Valid to: %02d.%02d.%04d\n",
            card_use_to_date_s.day,
            card_use_to_date_s.month,
            card_use_to_date_s.year);
        //start_trip_minutes
        if(data_block.start_trip_neg_minutes) {
            DateTime card_start_trip_minutes_s = {0};
            from_minutes_to_datetime(
                data_block.valid_from_date + data_block.valid_for_minutes -
                    data_block.start_trip_neg_minutes,
                &card_start_trip_minutes_s,
                2019); //-time
            furi_string_cat_printf(
                result,
                "Trip from: %02d.%02d.%04d %02d:%02d\n",
                card_start_trip_minutes_s.day,
                card_start_trip_minutes_s.month,
                card_start_trip_minutes_s.year,
                card_start_trip_minutes_s.hour,
                card_start_trip_minutes_s.minute);
        }
        //start_trip_m_minutes
        if(data_block.metro_ride_with) {
            DateTime card_start_trip_m_minutes_s = {0};
            from_minutes_to_datetime(
                data_block.valid_from_date + data_block.valid_for_minutes -
                    data_block.start_trip_neg_minutes + data_block.metro_ride_with,
                &card_start_trip_m_minutes_s,
                2019);
            furi_string_cat_printf(
                result,
                "(M) from: %02d.%02d.%04d %02d:%02d\n",
                card_start_trip_m_minutes_s.day,
                card_start_trip_m_minutes_s.month,
                card_start_trip_m_minutes_s.year,
                card_start_trip_m_minutes_s.hour,
                card_start_trip_m_minutes_s.minute);
        }
        //transport
        //validator
        if(data_block.validator) {
            furi_string_cat_printf(result, "Validator: %05d", data_block.validator);
        }
        break;
    }
    case 0x3CCB: {
        parse_layout_FCB(&data_block, block);
        //number
        furi_string_cat_printf(result, "Number: %010lu\n", data_block.number);
        //valid_from_date
        DateTime card_use_from_date_s = {0};
        from_days_to_datetime(data_block.valid_from_date, &card_use_from_date_s, 1992);
        furi_string_cat_printf(
            result,
            "Valid from: %02d.%02d.%04d\n",
            card_use_from_date_s.day,
            card_use_from_date_s.month,
            card_use_from_date_s.year);
        //valid_to_date
        DateTime card_use_to_date_s = {0};
        from_days_to_datetime(data_block.valid_to_date, &card_use_to_date_s, 1992);
        furi_string_cat_printf(
            result,
            "Valid to: %02d.%02d.%04d\n",
            card_use_to_date_s.day,
            card_use_to_date_s.month,
            card_use_to_date_s.year);
        break;
    }
    case 0x3C0B: {
        //number
        furi_string_cat_printf(result, "Number: %010lu\n", data_block.number);
        //valid_from_date
        DateTime card_use_from_date_s = {0};
        from_days_to_datetime(data_block.valid_from_date, &card_use_from_date_s, 1992);
        furi_string_cat_printf(
            result,
            "Valid from: %02d.%02d.%04d\n",
            card_use_from_date_s.day,
            card_use_from_date_s.month,
            card_use_from_date_s.year);
        //valid_to_date
        DateTime card_use_to_date_s = {0};
        from_days_to_datetime(data_block.valid_to_date, &card_use_to_date_s, 1992);
        furi_string_cat_printf(
            result,
            "Valid to: %02d.%02d.%04d\n",
            card_use_to_date_s.day,
            card_use_to_date_s.month,
            card_use_to_date_s.year);
        break;
    }
    default:
        result = NULL;
        return false;
    }

    return true;
}