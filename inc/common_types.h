/**
 * \file common_types.h
 *
 * \brief Types for error checking
 *
 * \author Anton Mukhin

 */

#ifndef COMMON_COMMON_TYPES_H_
#define COMMON_COMMON_TYPES_H_

typedef enum {
    RET_CODE_SUCCESS = 0,
    RET_CODE_ERROR,
    RET_CODE_BUSY,
    RET_CODE_TIMEOUT,
    RET_CODE_INVALID_PARAMS,
    RET_CODE_DIVISION_BY_ZERO,
    RET_CODE_CRC_MISMATCH,
    RET_CODE_NULL_PTR,
    RET_CODE_ALLOC_FAIL,

    RET_CODE_UART_ERROR = 50,
    RET_CODE_UART_BUSY,
    RET_CODE_UART_TIMEOUT,

    RET_CODE_I2C_ERROR = 100,
    RET_CODE_I2C_BUSY,
    RET_CODE_I2C_TIMEOUT,

    RET_CODE_SPI_ERROR = 200,
    RET_CODE_SPI_BUSY,
    RET_CODE_SPI_TIMEOUT,
    RET_CODE_SPI_OPEN_FAIL,
    RET_CODE_SPI_WRITE_ERR,
    RET_CODE_SPI_READ_ERR,
    RET_CODE_SPI_EXCHANGE_ERR
} ret_code_t;

#endif /* COMMON_COMMON_TYPES_H_ */
