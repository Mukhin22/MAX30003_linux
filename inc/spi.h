/**
 * \file spi.h
 *
 * \author Anton Mukhin
 *
 */
#ifndef INC_SPI_H_
#define INC_SPI_H_
//-----------------------------------------------------------------------------
#include <linux/spi/spidev.h>
#include "../inc/common_types.h"

/* SPI default settings */
#define SPI_MAX_SPEED          1000000
#define SPI_MODE_DEFAULT       SPI_MODE_0
#define SPI_DEVICE_NAME        "/dev/spidev0.0"
#define SPI_DELAY_USEC_DEFAULT 0
#define SPI_BUFF_SIZE          4
#define SPI_COMMAND_LEN        1

/**
* \brief Struct contains spidev parameters to set
*/
typedef struct spi_ {
    __u8 *dev_name;
    int   fd;    /* file descriptor: fd = open(filename, O_RDWR); */
    __u32 speed; /* speed [Hz] */
    __u8  mode;  /* SPI mode */
    __u8  lsb;   /* LSB first */
    __u8  bits;  /* bits per word */
    __u8  rx_buf[SPI_BUFF_SIZE];
    __u8  tx_buf[SPI_BUFF_SIZE];
    struct spi_ioc_transfer xfer[2];
} __attribute__((packed)) spi_t;

#ifdef __cplusplus
extern "C" {

#endif /*__cplusplus */

/**
* \brief open and init SPIdev
* \param self - structure with spidev params
* \retval ret_code_t RET_CODE_SUCCESS - no errors.
*/
ret_code_t spi_init(spi_t *const self);

/**
* \brief close spi_dev and free memory
* \param self - structure with spidev params
* \retval ret_code_t
*/
ret_code_t spi_free(spi_t *self);

/**
* \brief used to read a chunk of data from spi
* \param self - structure with spidev params
* \param rx_buf - buffer to read data
* \param len - lenght of data to read
* \retval ret_code_t
*/
ret_code_t spi_read(spi_t *const self, char *const rx_buf, const size_t len);

/**
* \brief used to write a chunk of data to spi
* \param self - structure with spidev params
* \param tx_buf - buffer with data
* \param len - lenght of data to write
* \retval ret_code_t
*/
ret_code_t
spi_write(spi_t *const self, const char *const tx_buf, const size_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // INC_SPI_H_
