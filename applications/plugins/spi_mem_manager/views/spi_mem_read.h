#ifndef _SPI_MEM_READ_H_
#define _SPI_MEM_READ_H_

typedef struct {
    View* view;
} SPIMemRead;

View* spi_mem_read_get_view(SPIMemRead* instance);
SPIMemRead* spi_mem_read_alloc(void);
void spi_mem_read_free(SPIMemRead* instance);

#endif // _SPI_MEM_READ_H_
