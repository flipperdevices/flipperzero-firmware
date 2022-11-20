#include "spi_mem_chip_i.h"

const SPIMemChipVendorName spi_mem_chip_vendor_names[] = {
    {"Cypress", SPIMemChipVendorCypress},
    {"Fujitsu", SPIMemChipVendorFujitsu},
    {"EON", SPIMemChipVendorEon},
    {"Atmel", SPIMemChipVendorAtmel},
    {"Micron", SPIMemChipVendorMicron},
    {"AMIC", SPIMemChipVendorAmic},
    {"Nor-Mem", SPIMemChipVendorNormem},
    {"Sanyo", SPIMemChipVendorSanyo},
    {"Intel", SPIMemChipVendorIntel},
    {"ESMT", SPIMemChipVendorEsmt},
    {"Fudan", SPIMemChipVendorFudan},
    {"Hyundai", SPIMemChipVendorHyundai},
    {"SST", SPIMemChipVendorSst},
    {"Micronix", SPIMemChipVendorMicronix},
    {"GigaDevice", SPIMemChipVendorGigadevice},
    {"ISSI", SPIMemChipVendorIssi},
    {"Winbond", SPIMemChipVendorWinbond},
    {"BOYA", SPIMemChipVendorBoya},
    {"AP Memory", SPIMemChipVendorAPMemory},
    {"Unknown", SPIMemChipVendorUnknown}};

static const char* spi_mem_chip_search_vendor_name(SPIMemChipVendor vendor_id) {
    const SPIMemChipVendorName* vendor = spi_mem_chip_vendor_names;
    while(vendor->vendor_id != SPIMemChipVendorUnknown && vendor->vendor_id != vendor_id) vendor++;
    return vendor->vendor_name;
}

// vendor_id, model_name, vendor_name, size, write_mode,
// type_id, capacity_id, address_type
static const SPIMemChip SPIMemChips[] = {
    {SPIMemChipVendorWinbond,
     "W25Q32BV",
     NULL,
     4L * 1024L * 1024L,
     SPIMemChipWriteModePage256Bytes,
     0x40,
     0x16,
     SPIMemChipAddressType3byte},
    {SPIMemChipVendorWinbond,
     "W25Q64FV",
     NULL,
     8L * 1024L * 1024L,
     SPIMemChipWriteModePage256Bytes,
     0x40,
     0x17,
     SPIMemChipAddressType3byte},
    {SPIMemChipVendorUnknown,
     NULL,
     NULL,
     0,
     SPIMemChipWriteModeUnknown,
     0,
     0,
     SPIMemChipAddressTypeUnknown}};

static void spi_mem_chip_copy_info(SPIMemChip* dest, const SPIMemChip* src) {
    dest->vendor_id = src->vendor_id;
    dest->model_name = src->model_name;
    dest->vendor_name = src->vendor_name;
    dest->size = src->size;
    dest->write_mode = src->write_mode;
    dest->type_id = src->type_id;
    dest->capacity_id = src->capacity_id;
    dest->address_type = src->address_type;
}

bool spi_mem_chip_complete_info(SPIMemChip* chip_info) {
    const SPIMemChip* chip_info_arr;
    for(chip_info_arr = SPIMemChips; chip_info_arr->model_name != NULL; chip_info_arr++) {
        if(chip_info->vendor_id != chip_info_arr->vendor_id) continue;
        if(chip_info->type_id != chip_info_arr->type_id) continue;
        if(chip_info->capacity_id != chip_info_arr->capacity_id) continue;
        spi_mem_chip_copy_info(chip_info, chip_info_arr);
        chip_info->vendor_name = spi_mem_chip_search_vendor_name(chip_info->vendor_id);
        return true;
    }
    return false;
}

void spi_mem_chip_print_chip_models(FuriString* str) {
    const SPIMemChip* chip_info_arr;
    for(chip_info_arr = SPIMemChips; chip_info_arr->model_name != NULL; chip_info_arr++) {
        furi_string_cat_printf(str, "%s\n", chip_info_arr->model_name);
    }
}

size_t spi_mem_chip_get_size(SPIMemChip* chip) {
    return (chip->size);
}

const char* spi_mem_chip_get_vendor_name(SPIMemChip* chip) {
    return (chip->vendor_name);
}

const char* spi_mem_chip_get_model_name(SPIMemChip* chip) {
    return (chip->model_name);
}
