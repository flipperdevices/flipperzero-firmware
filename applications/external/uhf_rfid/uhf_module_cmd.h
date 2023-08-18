#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct {
    const uint8_t* cmd;
    size_t length;
} Command;

// Define the command data arrays
static const uint8_t CMD_HW_VERSION_DATA[] = {0xBB, 0x00, 0x03, 0x00, 0x01, 0x00, 0x04, 0x7E};
static const uint8_t CMD_SW_VERSION_DATA[] = {0xBB, 0x00, 0x03, 0x00, 0x01, 0x01, 0x05, 0x7E};
static const uint8_t CMD_MANUFACTURERS_DATA[] = {0xBB, 0x00, 0x03, 0x00, 0x01, 0x02, 0x06, 0x7E};
static const uint8_t CMD_SINGLE_POLLING_DATA[] = {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E};
static const uint8_t CMD_MULTIPLE_POLLING_DATA[] =
    {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x27, 0x10, 0x83, 0x7E};
static const uint8_t CMD_STOP_MULTIPLE_POLLING_DATA[] = {0xBB, 0x00, 0x28, 0x00, 0x00, 0x28, 0x7E};
static const uint8_t CMD_SET_SELECT_PARAMETER_DATA[] = {0xBB, 0x00, 0x0C, 0x00, 0x13, 0x01, 0x00,
                                                        0x00, 0x00, 0x20, 0x60, 0x00, 0x30, 0x75,
                                                        0x1F, 0xEB, 0x70, 0x5C, 0x59, 0x04, 0xE3,
                                                        0xD5, 0x0D, 0x70, 0xAD, 0x7E};
static const uint8_t CMD_GET_SELECT_PARAMETER_DATA[] = {0xBB, 0x00, 0x0B, 0x00, 0x00, 0x0B, 0x7E};
static const uint8_t CMD_SET_SELECT_MODE_DATA[] = {0xBB, 0x00, 0x12, 0x00, 0x01, 0x01, 0x14, 0x7E};
static const uint8_t CMD_READ_LABEL_DATA_STORAGE_AREA_DATA[] =
    {0xBB, 0x00, 0x39, 0x00, 0x09, 0x00, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x02, 0x45, 0x7E};
static const uint8_t CMD_WRITE_LABEL_DATA_STORE_DATA[] = {0xBB, 0x00, 0x49, 0x00, 0x0D, 0x00, 0x00,
                                                          0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x02,
                                                          0x12, 0x34, 0x56, 0x78, 0x6D, 0x7E};
static const uint8_t CMD_LOCK_LABEL_DATA_STORE_DATA[] =
    {0xBB, 0x00, 0x82, 0x00, 0x07, 0x00, 0x00, 0xFF, 0xFF, 0x02, 0x00, 0x80, 0x09, 0x7E};
static const uint8_t CMD_INACTIVATE_KILL_TAG_DATA[] =
    {0xBB, 0x00, 0x65, 0x00, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x67, 0x7E};
static const uint8_t CMD_SET_COMMUNICATION_BAUD_RATE_DATA[] =
    {0xBB, 0x00, 0x11, 0x00, 0x02, 0x00, 0xC0, 0xD3, 0x7E};
static const uint8_t CMD_GET_QUERY_PARAMETERS_DATA[] = {0xBB, 0x00, 0x0D, 0x00, 0x00, 0x0D, 0x7E};
static const uint8_t CMD_SET_QUERY_PARAMETER_DATA[] =
    {0xBB, 0x00, 0x0E, 0x00, 0x02, 0x10, 0x20, 0x40, 0x7E};
static const uint8_t CMD_SET_WORK_AREA_DATA[] = {0xBB, 0x00, 0x07, 0x00, 0x01, 0x01, 0x09, 0x7E};

// Define the Command structs
static const Command CMD_HW_VERSION = {CMD_HW_VERSION_DATA, sizeof(CMD_HW_VERSION_DATA)};
static const Command CMD_SW_VERSION = {CMD_SW_VERSION_DATA, sizeof(CMD_SW_VERSION_DATA)};
static const Command CMD_MANUFACTURERS = {CMD_MANUFACTURERS_DATA, sizeof(CMD_MANUFACTURERS_DATA)};
static const Command CMD_SINGLE_POLLING = {
    CMD_SINGLE_POLLING_DATA,
    sizeof(CMD_SINGLE_POLLING_DATA)};
static const Command CMD_MULTIPLE_POLLING = {
    CMD_MULTIPLE_POLLING_DATA,
    sizeof(CMD_MULTIPLE_POLLING_DATA)};
static const Command CMD_STOP_MULTIPLE_POLLING = {
    CMD_STOP_MULTIPLE_POLLING_DATA,
    sizeof(CMD_STOP_MULTIPLE_POLLING_DATA)};
static const Command CMD_SET_SELECT_PARAMETER = {
    CMD_SET_SELECT_PARAMETER_DATA,
    sizeof(CMD_SET_SELECT_PARAMETER_DATA)};
static const Command CMD_GET_SELECT_PARAMETER = {
    CMD_GET_SELECT_PARAMETER_DATA,
    sizeof(CMD_GET_SELECT_PARAMETER_DATA)};
static const Command CMD_SET_SELECT_MODE = {
    CMD_SET_SELECT_MODE_DATA,
    sizeof(CMD_SET_SELECT_MODE_DATA)};
static const Command CMD_READ_LABEL_DATA_STORAGE_AREA = {
    CMD_READ_LABEL_DATA_STORAGE_AREA_DATA,
    sizeof(CMD_READ_LABEL_DATA_STORAGE_AREA_DATA)};
static const Command CMD_WRITE_LABEL_DATA_STORE = {
    CMD_WRITE_LABEL_DATA_STORE_DATA,
    sizeof(CMD_WRITE_LABEL_DATA_STORE_DATA)};
static const Command CMD_LOCK_LABEL_DATA_STORE = {
    CMD_LOCK_LABEL_DATA_STORE_DATA,
    sizeof(CMD_LOCK_LABEL_DATA_STORE_DATA)};
static const Command CMD_INACTIVATE_KILL_TAG = {
    CMD_INACTIVATE_KILL_TAG_DATA,
    sizeof(CMD_INACTIVATE_KILL_TAG_DATA)};
static const Command CMD_SET_COMMUNICATION_BAUD_RATE = {
    CMD_SET_COMMUNICATION_BAUD_RATE_DATA,
    sizeof(CMD_SET_COMMUNICATION_BAUD_RATE_DATA)};
static const Command CMD_GET_QUERY_PARAMETERS = {
    CMD_GET_QUERY_PARAMETERS_DATA,
    sizeof(CMD_GET_QUERY_PARAMETERS_DATA)};
static const Command CMD_SET_QUERY_PARAMETER = {
    CMD_SET_QUERY_PARAMETER_DATA,
    sizeof(CMD_SET_QUERY_PARAMETER_DATA)};
static const Command CMD_SET_WORK_AREA = {CMD_SET_WORK_AREA_DATA, sizeof(CMD_SET_WORK_AREA_DATA)};
