/**
 * \file MAX30003.c
 *
 * \brief This file is a linux userspace driver for MAX300003.c
 */
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "common_check.h"
#include "string.h"
#include "MAX30003.h"

#include "Log_dbg_en.h"
#define DEBUG_ENABLE MAX30003_PRINT_EN
#define DBG_TAG      "MAX30003.c"
#include "Log_dbg.h"

#define BYTES_NUM_TO_READ    4
#define BURST_BYTES_NUM      100
#define COMMAND_BYTE_NUM     0
#define ECG_MEAS_TIMEOUT_S   20
#define ECG_PRINT_COL_SIZE   10
#define FIFO_LAST_DATA_MASK  0xC0
#define FIFO_ADDITIONAL_BITS 6

#define EINT_TO_LAST_BYTE 16

#define ECG_DEFAULT_DATA_LEN 1024

extern spi_t spi;

char SPI_temp_32b[BYTES_NUM_IN_REG];
char SPI_temp_Burst[BURST_BYTES_NUM];

/**
 * \brief Check in status register EINT interrupt is present
 * or FIFO overflow detected
 */
static uint8_t __check_fifo_present()
{
    uint8_t status[4] = { 0 };
    uint8_t ret_flag  = 0;
    max30003_read_reg(&spi, STATUS, status);

    if (status[0] & (EOVF >> EINT_TO_LAST_BYTE)) {
        LOG_ERR("FIFO OVERFLOW");
    }
    if (status[0] & (EINT >> EINT_TO_LAST_BYTE)) {
        ret_flag = 1;
    }
    return ret_flag;
}

ret_code_t max30003_sw_reset(void)
{
    return max30003_write_reg(&spi, SW_RST, ZERO_SEQUENCE);
}

ret_code_t max30003_synch(void)
{
    return max30003_write_reg(&spi, SYNCH, ZERO_SEQUENCE);
}

int max30003_read_reg(spi_t *const   self,
                      const uint8_t  read_addr,
                      uint8_t *const read_buff)
{
    memset(self->tx_buf, 0, sizeof self->tx_buf);
    self->tx_buf[0] = ((read_addr << 1) | RREG);

    return spi_read(self, read_buff, BYTES_NUM_TO_READ);
}

int max30003_write_reg(spi_t *const   self,
                       const uint8_t  write_addr,
                       const uint32_t data)
{
    char command_buff[BYTES_NUM_IN_REG] = { 0 };
    /* Specify address to write */
    command_buff[COMMAND_BYTE_NUM] = ((write_addr << 1) | WREG);

    /* Write data to buffer for writing*/
    command_buff[1] = data >> 16;
    command_buff[2] = data >> 8;
    command_buff[3] = data;

    return spi_write(self, command_buff, BYTES_NUM_IN_REG);
}

ret_code_t max30003_init(const ecg_data_t *ecg_data)
{
    ret_code_t ret = RET_CODE_SUCCESS;
    CONTINUE_ON_SUCCESS(max30003_sw_reset());
    CONTINUE_ON_SUCCESS(max30003_write_reg(&spi, CNFG_GEN, ecg_data->cnfg_gen));
    CONTINUE_ON_SUCCESS(max30003_write_reg(&spi, CNFG_CAL, ecg_data->cnfg_cal));
    CONTINUE_ON_SUCCESS(
            max30003_write_reg(&spi, CNFG_EMUX, ecg_data->cnfg_emux));
    CONTINUE_ON_SUCCESS(max30003_write_reg(&spi, CNFG_ECG, ecg_data->cnfg_ecg));
    CONTINUE_ON_SUCCESS(
            max30003_write_reg(&spi, CNFG_RTOR1, ecg_data->cnfg_rtor1));
    CONTINUE_ON_SUCCESS(max30003_write_reg(&spi, MNGR_INT, ecg_data->mngr_int));
    CONTINUE_ON_SUCCESS(max30003_synch());
exit:
    return ret;
}

ret_code_t ecg_delete_handle(ecg_data_t **ecg_data)
{
    ret_code_t ret = RET_CODE_SUCCESS;
    if (PTR_INVALID(ecg_data)) {
        LOG_ERR("ECG handle is NULL\n");
        ret = RET_CODE_NULL_PTR;
        goto exit;
    }

    free(*ecg_data);
    ecg_data = NULL;
exit:
    return ret;
}

int32_t max30003_get_ecg_point(void)
{
    uint8_t fifo_data[3] = { 0 };
    int32_t ecg_point    = 0;

    if (max30003_read_reg(&spi, ECG_FIFO, fifo_data)) {
        LOG_ERR("Failed to read a point");
        ecg_point = 0;
        return ecg_point;
    }

    uint32_t data0 = (int32_t)(fifo_data[0]);
    data0          = data0 << 24;
    int32_t data1  = (int32_t)(fifo_data[1]);
    data1          = data1 << 16;
    int32_t data2  = (int32_t)(fifo_data[2]);

    /*Delete the PTAG and ETAG values from last FIFO byte*/
    data2 = data2 >> FIFO_ADDITIONAL_BITS;
    data2 = data2 & FIFO_LAST_DATA_MASK;

    ecg_point = (int32_t)(data0 | data1 | data2);

    return ecg_point;
}

ret_code_t ecg_set_timeout(ecg_data_t *const ecg_data,
                           const int32_t     timeout_val)
{
    if (!ecg_data) {
        return RET_CODE_INVALID_PARAMS;
    }

    ecg_data->timeout_val = timeout_val;
    return RET_CODE_SUCCESS;
}

ecg_data_t *ecg_create_handle(void)
{
    LOG_DBG("Creating a handle for ECG data\n");
    return (ecg_data_t *)malloc(sizeof(ecg_data_t));
}

ret_code_t ecg_init_handle(ecg_data_t *const ecg_data)
{
    ret_code_t ret = RET_CODE_SUCCESS;
    if (PTR_INVALID(ecg_data)) {
        LOG_ERR("ecg handle is NULL\n");
        ret = RET_CODE_NULL_PTR;
        goto exit;
    }
    memset(ecg_data, 0, sizeof(ecg_data));

    ecg_data->data_len   = DEF_ECG_DATA_LEN;
    ecg_data->cnfg_ecg   = CNFG_ECG_DEFAULT;
    ecg_data->cnfg_cal   = CNFG_CAL_DEFAULT;
    ecg_data->cnfg_emux  = CNFG_EMUX_DEFAULT;
    ecg_data->cnfg_gen   = CNFG_GEN_DEFAULT;
    ecg_data->cnfg_rtor1 = CNFG_RTOR1_DEFAULT;
    ecg_data->mngr_int   = MNGR_INT_DEFAULT;

    CONTINUE_ON_SUCCESS(ecg_set_data_len(ecg_data, DEF_ECG_DATA_LEN));
    LOG_DBG("Ecg handler inited\n");

exit:
    return ret;
}

ret_code_t ecg_set_data_len(ecg_data_t *const ecg_data, const uint32_t data_len)
{
    if (PTR_INVALID(ecg_data)) {
        return RET_CODE_INVALID_PARAMS;
    }

    ecg_data->data_len = data_len;

    ecg_data->data_arr = (int32_t *)malloc(sizeof(int32_t) * data_len);

    if (PTR_INVALID(ecg_data->data_arr)) {
        return RET_CODE_ALLOC_FAIL;
    }
    return RET_CODE_SUCCESS;
}

ret_code_t ecg_get_data(ecg_data_t *const ecg_data)
{
    ret_code_t ret = RET_CODE_SUCCESS;
    if (!ecg_data) {
        ret = RET_CODE_INVALID_PARAMS;
        goto exit;
    }

    int32_t data      = 0;
    ecg_data->data_ID = 0;
    struct timespec ts;

    /* Set timeout value if it's not specified*/
    if (!ecg_data->timeout_val) {
        ecg_data->timeout_val = ECG_MEAS_TIMEOUT_S;
    }

    uint32_t first_time_point_s = 0; /* Start point of the timer in seconds*/
    uint32_t current_time_s     = 0;
    uint32_t current_time_ns    = 0;
    uint32_t prev_time_point_ns = 0;

    //    max30003_synch(); /*Clear FIFO before start*/
    /*Get the first time point*/
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        LOG_ERR("clock_gettime failure in %s", __func__);
        ret = RET_CODE_ERROR;
        goto exit;
    }
    first_time_point_s = (uint32_t)ts.tv_sec;

    uint8_t sample_ready = 0;
    /* Measurement loop */
    while (ecg_data->data_ID < DEF_ECG_DATA_LEN) {
        /* Check timeout */
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
            LOG_ERR("clock_gettime failure in %s", __func__);
            ret = RET_CODE_ERROR;
            goto exit;
        }
        current_time_s  = (uint32_t)ts.tv_sec;
        current_time_ns = (uint32_t)ts.tv_nsec;
        if ((current_time_s - first_time_point_s) > (ecg_data->timeout_val)) {
            LOG_ERR("Timeout for measurement in  %s", __func__);
            ret = RET_CODE_ERROR;
            goto exit;
        }
        sample_ready = __check_fifo_present();
        data         = max30003_get_ecg_point();
        if (data) {
            ecg_data->data_arr[ecg_data->data_ID++] = data;
        }
#ifdef CHECK_DATA_ON_TIME_INTERVAL
        if ((current_time_ns - prev_time_point_ns) >=
            (ECG_128_INTERVAL * N_SEC_TO_M_SEC)) {
            prev_time_point_ns = current_time_ns;
            data               = max30003_get_ecg_point();
            if (data) {
                ecg_data->data_arr[ecg_data->data_ID++] = data;
            }
        }
    }
#endif
}
exit : return ret;
}

ret_code_t ecg_print_data(const ecg_data_t *const ecg_data)
{
    ret_code_t ret = RET_CODE_SUCCESS;
    if (!ecg_data) {
        ret = RET_CODE_INVALID_PARAMS;
        goto exit;
    }
#ifdef DEBUG
    /*Print array as a col */
    for (int i = 0; i < ecg_data->data_len; ++i) {
        printf("%d\n", ecg_data->data_arr[i]);
    }
#endif

exit:
    return ret;
}
