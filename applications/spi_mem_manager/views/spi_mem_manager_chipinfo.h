#ifndef _SPI_MEM_MANAGER_CHIPINFO_H_
#define _SPI_MEM_MANAGER_CHIPINFO_H_

typedef struct {
    View* view;
} SPIMemManagerChipinfo;

View* spi_mem_manager_chipinfo_get_view(SPIMemManagerChipinfo* instance);
SPIMemManagerChipinfo* spi_mem_manager_chipinfo_alloc(void);
void spi_mem_manager_chipinfo_free(SPIMemManagerChipinfo* instance);

#endif // _SPI_MEM_MANAGER_CHIPINFO_H_
