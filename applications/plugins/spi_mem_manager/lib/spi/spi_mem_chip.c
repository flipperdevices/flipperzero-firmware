#include "spi_mem_chip.h"

const SPIMemChipVendorName SPIMemChipVendorNames[] = {
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
    {"Unknown", SPIMemChipVendorUnknown}};

static const char* spi_mem_get_chip_vendor_name(SPIMemChipVendor vendor_id) {
    const SPIMemChipVendorName* vendor = SPIMemChipVendorNames;
    while(vendor->vendor_id != SPIMemChipVendorUnknown && vendor->vendor_id != vendor_id) vendor++;
    return vendor->vendor_name;
}

static const SPIMemChip SPIMemChips[] = {
    {SPIMemChipVendorWinbond,
     "W25Q32BV",
     NULL,
     4L * 1024L * 1024L,
     SPIMemChipWriteModePage256Bytes,
     0x40,
     0x16,
     4096,
     0x20},
    {SPIMemChipVendorUnknown, NULL, NULL, 0, SPIMemChipWriteModeUnknown, 0, 0, 0, 0}};

static bool spi_mem_read_chip_info(SPIMemChip* chip) {
    // mock
    furi_delay_ms(1000);
    chip->vendor_id = SPIMemChipVendorWinbond;
    chip->type_id = 0x40;
    chip->capacity_id = 0x16;
    return true;
    // mock
}

static void spi_mem_copy_chip_info(SPIMemChip* dest, const SPIMemChip* src) {
    dest->vendor_id = src->vendor_id;
    dest->model_name = src->model_name;
    dest->vendor_name = src->vendor_name;
    dest->size = src->size;
    dest->write_mode = src->write_mode;
    dest->type_id = src->type_id;
    dest->capacity_id = src->capacity_id;
    dest->erase_gran = src->erase_gran;
    dest->erase_gran_cmd = src->erase_gran_cmd;
}

static bool spi_mem_complete_chip_info(SPIMemChip* chip_info) {
    const SPIMemChip* chip_info_arr;
    for(chip_info_arr = SPIMemChips; chip_info_arr->model_name != NULL; chip_info_arr++) {
        if(chip_info->vendor_id != chip_info_arr->vendor_id) continue;
        if(chip_info->type_id != chip_info_arr->type_id) continue;
        if(chip_info->capacity_id != chip_info_arr->capacity_id) continue;
        spi_mem_copy_chip_info(chip_info, chip_info_arr);
        chip_info->vendor_name = spi_mem_get_chip_vendor_name(chip_info->vendor_id);
        return true;
    }
    return false;
}

bool spi_mem_get_chip_info(SPIMemChip* chip) {
    if(!spi_mem_read_chip_info(chip)) return false;
    if(!spi_mem_complete_chip_info(chip)) return false;
    return true;
}
