/**
 * \file spi.c
 *
 * \brief This is linux wrappper to controll spidev interface from userspace
 * \author Anton Mukhin
 */
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "spi.h"
#include "common_types.h"
#include "common_check.h"

#include "Log_dbg_en.h"
#define DEBUG_ENABLE SPI_PRINT_EN
#define DBG_TAG      "spi.c"
#include "Log_dbg.h"

ret_code_t spi_init(spi_t *const self)
{
    ret_code_t ret = RET_CODE_SUCCESS;
    if (PTR_INVALID(self)) {
        ret = RET_CODE_NULL_PTR;
        goto exit;
    }

    memset(&self->xfer, 0, sizeof(self->xfer));

    LOG_INFO("Open the spi_dev\n");
    self->fd = open(self->dev_name, O_RDWR);
    if (self->fd < 0) {
        LOG_ERR("failed to open the spidev %s", self->dev_name);
        ret = RET_CODE_SPI_OPEN_FAIL;
        goto exit;
    }

    LOG_INFO("Setting the mode for spidev\n");
    if (ioctl(self->fd, SPI_IOC_WR_MODE, &self->mode) < 0) {
        LOG_ERR(" can't set bus mode. mode is %d\n", self->mode);
        ret = RET_CODE_SPI_ERROR;
        goto exit;
    }

    LOG_INFO("Get SPI mode\n");
    if (ioctl(self->fd, SPI_IOC_RD_MODE, &self->mode) < 0) {
        LOG_ERR("can't get bus mode\n");
        ret = RET_CODE_SPI_ERROR;
        goto exit;
    }

    LOG_INFO("Get SPI_IOC_RD_LSB_FIRST\n");
    if (ioctl(self->fd, SPI_IOC_RD_LSB_FIRST, &self->lsb) < 0) {
        LOG_ERR("can't get 'LSB first' spi struct lsb value is %d\n",
                self->lsb);
        ret = RET_CODE_SPI_ERROR;
        goto exit;
    }

    LOG_INFO("Set bits per word\n");

    if (ioctl(self->fd, SPI_IOC_WR_BITS_PER_WORD, &self->bits) < 0) {
        LOG_ERR(" can't set bits per word. spi struct bpw value is %d \n",
                self->bits);
        ret = RET_CODE_SPI_ERROR;
        goto exit;
    }

    LOG_INFO("get bits per word\n");
    if (ioctl(self->fd, SPI_IOC_RD_BITS_PER_WORD, &self->bits) < 0) {
        LOG_ERR("can't get bits per word\n");
        ret = RET_CODE_SPI_ERROR;
        goto exit;
    }

    LOG_INFO(" set max speed [Hz]\n");
    if (ioctl(self->fd, SPI_IOC_WR_MAX_SPEED_HZ, &self->speed) < 0) {
        LOG_ERR("can't set max speed [Hz] spi struct speed val is %d\n",
                self->speed);
        ret = RET_CODE_SPI_ERROR;
        goto exit;
    }
    LOG_INFO("get max speed [Hz]\n");
    if (ioctl(self->fd, SPI_IOC_RD_MAX_SPEED_HZ, &self->speed) < 0) {
        LOG_ERR("can't get max speed [Hz] \n");
        ret = RET_CODE_SPI_ERROR;
        goto exit;
    }

    LOG_INFO("open device='%s' mode=%d bits=%d lsb=%d max_speed=%d [Hz]\n",
             self->dev_name,
             (int)self->mode,
             (int)self->bits,
             (int)self->lsb,
             (int)self->speed);

    self->xfer[0].len                   = 4; /* Length of  command to write*/
    self->xfer[0].cs_change             = 0; /* Keep CS activated */
    self->xfer[0].delay_usecs           = 0, /* delay in us */
            self->xfer[0].speed_hz      = 1000000, /* speed */
            self->xfer[0].bits_per_word = 8,       /* bites per word 8 */
            self->xfer[1].len           = 4;       /* Length of Data to read */
    self->xfer[1].cs_change             = 0;       /* Keep CS activated */

exit:
    return ret;
}

ret_code_t spi_free(spi_t *self)
{
    ret_code_t ret = RET_CODE_SUCCESS;

    if (!self) {
        LOG_ERR("Null pointer spt_t \n");
        ret = RET_CODE_NULL_PTR;
        goto exit;
    }

    if (close(self->fd) < 0) {
        LOG_ERR("Faild to close SPI \n");
        ret = RET_CODE_ERROR;
        goto exit;
    }

    LOG_INFO("SPI dev closed\n");
exit:
    return ret;
}

ret_code_t spi_read(spi_t *const self, char *const rx_buf, const size_t len)
{
    ret_code_t ret = RET_CODE_SUCCESS;
    if (!self || !rx_buf) {
        ret = RET_CODE_NULL_PTR;
        goto exit;
    }

    self->xfer[0].tx_buf = (__u64)self->tx_buf;    /* output buffer */
    self->xfer[1].rx_buf = (__u64)self->rx_buf;    /* input buffer */
    self->xfer[0].len    = (__u64)SPI_COMMAND_LEN; /* input buffer */
    self->xfer[1].len    = (__u32)len;             /* length of data to read */

    if ((ioctl(self->fd, SPI_IOC_MESSAGE(2), self->xfer)) < 0) {
        LOG_ERR("error in spi_read(): ioctl(SPI_IOC_MESSAGE(2))\n");
        ret = RET_CODE_SPI_READ_ERR;
        goto exit;
    } else {
        memcpy(rx_buf, self->rx_buf, len);
    }

exit:
    return ret;
}

ret_code_t
spi_write(spi_t *const self, const char *const tx_buf, const size_t len)
{
    ret_code_t ret = RET_CODE_SUCCESS;
    if (!self || !tx_buf) {
        LOG_ERR("Null ptr in spi_write parameters\n");
        ret = RET_CODE_NULL_PTR;
        goto exit;
    }
    memcpy(self->tx_buf, tx_buf, len);

    self->xfer[0].tx_buf = (__u64)self->tx_buf;
    self->xfer[0].len    = (__u64)len;

    if ((ioctl(self->fd, SPI_IOC_MESSAGE(1), self->xfer)) < 0) {
        LOG_ERR(" ioctl(SPI_IOC_MESSAGE(1)) failed \n ");
        ret = RET_CODE_SPI_WRITE_ERR;
        goto exit;
    }

exit:
    return ret;
}
