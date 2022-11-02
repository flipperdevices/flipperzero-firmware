#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>
#include <furi_hal_uart.h>
#include <furi_hal_console.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>

#include "constants.h"

typedef struct {
    FuriHalUartId channel;
    uint32_t baudRate;

    BufferModel* bufferModel;
} UartDevice;

/**
 * Allocate a UartDevice
*/
UartDevice* uart_device_alloc();

/**
 * Deallocates and frees the model
*/
void uart_device_free(UartDevice* dev);

/**
 * Pop a command from the uart device if one is available
*/
PwnCommand* uart_device_pop(UartDevice* dev);

/**
 * Buffer model which will handle taking and processing input
*/
typedef struct {
    uint8_t* messageQueue;
    size_t queueSize;

    uint8_t* writePtr;
    uint8_t* readPtr;

} BufferModel;

/**
 * Allocate a buffer model
*/
BufferModel* buffer_model_alloc();

/**
 * Deallocates and frees the model
*/
void buffer_model_free(BufferModel* model);

/**
 * Determines if a message is available according to the current protocol
*/
bool buffer_model_has_message(BufferModel* model);

/**
 * Append data into the buffer and adjust the writePtr as needed
*/
void buffer_model_push(BufferModel* model, uint8_t data);

/**
 * Pop off the last byte of data
*/
uint8_t buffer_model_pop(BufferModel* model);