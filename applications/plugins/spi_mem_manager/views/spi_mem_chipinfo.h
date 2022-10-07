#ifndef _SPI_MEM_CHIPINFO_H_
#define _SPI_MEM_CHIPINFO_H_

typedef struct {
    View* view;
} SPIMemChipinfo;

View* spi_mem_chipinfo_get_view(SPIMemChipinfo* instance);
SPIMemChipinfo* spi_mem_chipinfo_alloc(void);
void spi_mem_chipinfo_free(SPIMemChipinfo* instance);

#endif // _SPI_MEM_CHIPINFO_H_
