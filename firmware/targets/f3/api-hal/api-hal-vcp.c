#include <api-hal-vcp.h>
#include <usbd_cdc_if.h>
#include <flipper_v2.h>
#include <ring.h>

#define API_HAL_VCP_RX_BUFFER_SIZE 600

static osSemaphoreId_t _api_hal_vcp_tx_semaphore;
static osSemaphoreId_t _api_hal_vcp_rx_semaphore;
static Ring* _api_hal_vcp_rx_ring;
static volatile bool _api_hal_vcp_alive = false;
static volatile bool _api_hal_vcp_underrun = false;

void api_hal_vcp_init() {
    _api_hal_vcp_tx_semaphore = osSemaphoreNew(1, 1, NULL);
    _api_hal_vcp_rx_semaphore = osSemaphoreNew(1, 0, NULL);
    _api_hal_vcp_rx_ring = ring_alloc(API_HAL_VCP_RX_BUFFER_SIZE);
}

void _api_hal_vcp_init() {
    _api_hal_vcp_alive = true;
}

void _api_hal_vcp_deinit() {
    _api_hal_vcp_alive = false;
    osSemaphoreRelease(_api_hal_vcp_tx_semaphore);
}

void _api_hal_vcp_rx_callback(uint8_t* buffer, size_t size) {
    // TODO: loop and sempahore
    size_t ret = ring_push(_api_hal_vcp_rx_ring, buffer, size);
    if (ret != size) {
        _api_hal_vcp_underrun = true;
    }
    osSemaphoreRelease(_api_hal_vcp_rx_semaphore);
}

void _api_hal_vcp_tx_complete(size_t size) {
    furi_check(osSemaphoreRelease(_api_hal_vcp_tx_semaphore) == osOK);
}

size_t api_hal_vcp_rx(uint8_t* buffer, size_t size) {
    size_t ret;
    while(1) {
        ret = ring_pull(_api_hal_vcp_rx_ring, buffer, size);
        if (ret==0) {
            furi_check(osSemaphoreAcquire(_api_hal_vcp_rx_semaphore, osWaitForever) == osOK);
        } else {
            break;
        }
    }
    return ret;
}

void api_hal_vcp_tx(uint8_t* buffer, size_t size) {
    while (size > 0 && _api_hal_vcp_alive) {
        furi_check(osSemaphoreAcquire(_api_hal_vcp_tx_semaphore, osWaitForever) == osOK);

        size_t batch_size = size;
        if (batch_size > APP_TX_DATA_SIZE) {
            batch_size = APP_TX_DATA_SIZE;
        }

        if (CDC_Transmit_FS(buffer, batch_size) == USBD_OK) {
            size -= batch_size;
            buffer += batch_size;
        } else {
            // Shouldn't be there 
            osDelay(100);
        }
    }
}
