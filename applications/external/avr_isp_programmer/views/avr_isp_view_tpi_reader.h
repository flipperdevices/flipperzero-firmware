#pragma once

#include <gui/view.h>
#include "../helpers/avr_isp_types.h"
#include "../helpers/avr_isp_event.h"

typedef struct AvrIspReaderTpiView AvrIspReaderTpiView;

typedef void (*AvrIspReaderTpiViewCallback)(AvrIspCustomEvent event, void* context);

typedef enum {
    AvrIspReaderTpiViewStatusIDLE,
    AvrIspReaderTpiViewStatusReading,
    AvrIspReaderTpiViewStatusVerification,
} AvrIspReaderTpiViewStatus;

void avr_isp_tpi_reader_update_progress(AvrIspReaderTpiView* instance);

void avr_isp_tpi_reader_set_file_path(
    AvrIspReaderTpiView* instance,
    const char* file_path,
    const char* file_name);

void avr_isp_tpi_reader_view_set_callback(
    AvrIspReaderTpiView* instance,
    AvrIspReaderTpiViewCallback callback,
    void* context);

AvrIspReaderTpiView* avr_isp_tpi_reader_view_alloc();

void avr_isp_tpi_reader_view_free(AvrIspReaderTpiView* instance);

View* avr_isp_tpi_reader_view_get_view(AvrIspReaderTpiView* instance);

void avr_isp_tpi_reader_view_exit(void* context);
