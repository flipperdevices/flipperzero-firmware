#include "devices.h"

#include "registry.h"
#include "items.h"

struct SubGhzDevices {
    const SubGhzDevice* device;
};

const SubGhzDevice* subghz_devices_get_by_name(const char* device_name) {
    const SubGhzDevice* device =
        subghz_device_registry_get_by_name(&subghz_device_registry, device_name);
    return device;
}

bool subghz_devices_begin(const SubGhzDevice* device) {
    bool ret = false;
    if(device && device->interconnect->begin) {
        ret = device->interconnect->begin();
    }
    return ret;
}

void subghz_devices_end(const SubGhzDevice* device) {
    if(device && device->interconnect->end) {
        device->interconnect->end();
    }
}

void subghz_devices_reset(const SubGhzDevice* device) {
    if(device && device->interconnect->reset) {
        device->interconnect->reset();
    }
}

void subghz_devices_sleep(const SubGhzDevice* device) {
    if(device && device->interconnect->sleep) {
        device->interconnect->sleep();
    }
}

void subghz_devices_idle(const SubGhzDevice* device) {
    if(device && device->interconnect->idle) {
        device->interconnect->idle();
    }
}

void subghz_devices_load_preset(const SubGhzDevice* device, FuriHalSubGhzPreset preset) {
    if(device && device->interconnect->load_preset) {
        device->interconnect->load_preset(preset);
    }
}

uint32_t subghz_devices_set_frequency(const SubGhzDevice* device, uint32_t frequency) {
    uint32_t ret = 0;
    if(device && device->interconnect->set_frequency) {
        ret = device->interconnect->set_frequency(frequency);
    }
    return ret;
}

void subghz_devices_set_async_mirror_pin(const SubGhzDevice* device, const GpioPin* gpio) {
    if(device && device->interconnect->set_async_mirror_pin) {
        device->interconnect->set_async_mirror_pin(gpio);
    }
}

bool subghz_devices_set_tx(const SubGhzDevice* device) {
    bool ret = 0;
    if(device && device->interconnect->set_tx) {
        ret = device->interconnect->set_tx();
    }
    return ret;
}

bool subghz_devices_start_async_tx(const SubGhzDevice* device, void* callback, void* context) {
    bool ret = false;
    if(device && device->interconnect->start_async_tx) {
        ret = device->interconnect->start_async_tx(callback, context);
    }
    return ret;
}

bool subghz_devices_is_async_complete_tx(const SubGhzDevice* device) {
    bool ret = false;
    if(device && device->interconnect->is_async_complete_tx) {
        ret = device->interconnect->is_async_complete_tx();
    }
    return ret;
}

void subghz_devices_stop_async_tx(const SubGhzDevice* device) {
    if(device && device->interconnect->stop_async_tx) {
        device->interconnect->stop_async_tx();
    }
}

void subghz_devices_set_rx(const SubGhzDevice* device) {
    if(device && device->interconnect->set_rx) {
        device->interconnect->set_rx();
    }
}

void subghz_devices_start_async_rx(const SubGhzDevice* device, void* callback, void* context) {
    if(device && device->interconnect->start_async_rx) {
        device->interconnect->start_async_rx(callback, context);
    }
}

void subghz_devices_stop_async_rx(const SubGhzDevice* device) {
    if(device && device->interconnect->stop_async_rx) {
        device->interconnect->stop_async_rx();
    }
}
