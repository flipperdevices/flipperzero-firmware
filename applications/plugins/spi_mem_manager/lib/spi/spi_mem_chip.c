#include "spi_mem_chip_i.h"

const SPIMemChipVendorName spi_mem_chip_vendor_names[] = {
    {"Cypress", SPIMemChipVendorCypress},
    {"Fujitsu", SPIMemChipVendorFujitsu},
    {"EON", SPIMemChipVendorEon},
    {"Atmel", SPIMemChipVendorAtmel},
    {"Micron", SPIMemChipVendorMicron1},
    {"Micron", SPIMemChipVendorMicron2},
    {"AMIC", SPIMemChipVendorAmic},
    {"Nor-Mem", SPIMemChipVendorNormem},
    {"Sanyo", SPIMemChipVendorSanyo},
    {"Intel", SPIMemChipVendorIntel},
    {"ESMT", SPIMemChipVendorEsmt},
    {"Fudan", SPIMemChipVendorFudan1},
    {"Fudan", SPIMemChipVendorFudan2},
    {"Hyundai", SPIMemChipVendorHyundai},
    {"SST", SPIMemChipVendorSst},
    {"Micronix", SPIMemChipVendorMicronix},
    {"GigaDevice", SPIMemChipVendorGigadevice1},
    {"GigaDevice", SPIMemChipVendorGigadevice2},
    {"ISSI", SPIMemChipVendorIssi},
    {"Winbond", SPIMemChipVendorWinbond},
    {"BOYA", SPIMemChipVendorBoya},
    {"AP Memory", SPIMemChipVendorAPMemory},
    {"PMC-Sierra", SPIMemChipVendorPMCSierra1},
    {"PMC-Sierra", SPIMemChipVendorPMCSierra2},
    {"Excel Semicond.", SPIMemChipVendorExcelSemi},
    {"Zbit Semicond.", SPIMemChipVendorZbitSemi},
    {"Berg Micro.", SPIMemChipVendorBergMicro},
    {"ATO-Solution", SPIMemChipVendorATO},
    {"DOUQI", SPIMemChipVendorDOUQI},
    {"Fremont", SPIMemChipVendorFremont},
    {"Unknown", SPIMemChipVendorUnknown}};

static const char* spi_mem_chip_search_vendor_name(SPIMemChipVendor vendor_id) {
    const SPIMemChipVendorName* vendor = spi_mem_chip_vendor_names;
    while(vendor->vendor_id != SPIMemChipVendorUnknown && vendor->vendor_id != vendor_id) vendor++;
    return vendor->vendor_name;
}

static void spi_mem_chip_copy_info(SPIMemChip* dest, const SPIMemChip* src) {
    dest->vendor_id = src->vendor_id;
    dest->model_name = src->model_name;
    dest->vendor_name = src->vendor_name;
    dest->size = src->size;
    dest->write_mode = src->write_mode;
    dest->type_id = src->type_id;
    dest->capacity_id = src->capacity_id;
    dest->page_size = src->page_size;
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

uint8_t spi_mem_chip_get_vendor_id(SPIMemChip* chip) {
    return ((uint8_t)chip->vendor_id);
}

uint8_t spi_mem_chip_get_type_id(SPIMemChip* chip) {
    return (chip->type_id);
}

uint8_t spi_mem_chip_get_capacity_id(SPIMemChip* chip) {
    return (chip->capacity_id);
}

SPIMemChipWriteMode spi_mem_chip_get_write_mode(SPIMemChip* chip) {
    return (chip->write_mode);
}

size_t spi_mem_chip_get_page_size(SPIMemChip* chip) {
    return (chip->page_size);
}
