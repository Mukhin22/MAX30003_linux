/** @mainpage ECG_Linux 
 *
 * @author Anton_mukhin 
**/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <inttypes.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include "common_types.h"
#include "spi.h"
#include "get_opt_parser.h"
#include "MAX30003.h"
#include "common_check.h"

#include "Log_dbg_en.h"
#define DEBUG_ENABLE MAIN_PRINT_EN
#define DBG_TAG      "main.c"
#include "Log_dbg.h"

spi_t spi;

int main(int argc, char **argv)
{

    ecg_data_t *ecg_data = ecg_create_handle();
    EXIT_ON_NULL(ecg_data);

    CHECK_CODE_ERR(ecg_init_handle(ecg_data));

    CHECK_CODE_ERR(parse_opts(argc, argv, &spi, ecg_data));

    CHECK_CODE_ERR(spi_init(&spi));
    CHECK_CODE_ERR(max30003_init(ecg_data));

#ifdef TEST
    uint8_t test_buff[BYTES_NUM_IN_REG] = { 0 };
    max30003_read_reg(&spi, CNFG_ECG, test_buff);
    max30003_write_reg(&spi, CNFG_ECG, 0xc350ff);
    max30003_read_reg(&spi, CNFG_ECG, test_buff);
#endif

    CHECK_CODE_ERR(ecg_get_data(ecg_data));
    CHECK_CODE_ERR(ecg_print_data(ecg_data));

    CHECK_CODE_ERR(ecg_delete_handle(&ecg_data));
    CHECK_CODE_ERR(spi_free(&spi));
    LOG_INFO("Exiting ECG runner program\n");
    return 0;
}
