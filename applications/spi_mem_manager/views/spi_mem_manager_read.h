#ifndef _SPI_MEM_MANAGER_READ_H_
#define _SPI_MEM_MANAGER_READ_H_

typedef struct {
    View* view;
} SPIMemManagerRead;

View* spi_mem_manager_read_get_view(SPIMemManagerRead* instance);
SPIMemManagerRead* spi_mem_manager_read_alloc(void);
void spi_mem_manager_read_free(SPIMemManagerRead* instance);

#endif // _SPI_MEM_MANAGER_READ_H_
