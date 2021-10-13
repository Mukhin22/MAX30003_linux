/**
 * \file Log_dbg_en.h
 * \copyrights (C) 2021 Alnicko Development OU. All rights reserved.
 *
 *  Created on: jul 2, 2021
 */

#ifndef INC_LOG_DBG_EN_H_
#define INC_LOG_DBG_EN_H_

#define DEBUG

#define SYS_LOG_LEVEL_OFF     0
#define SYS_LOG_LEVEL_ERROR   1
#define SYS_LOG_LEVEL_WARNING 2
#define SYS_LOG_LEVEL_INFO    3
#define SYS_LOG_LEVEL_DEBUG   4
#define SYS_LOG_LEVEL_TRACE   5

#ifdef DEBUG
#define MAIN_PRINT_EN     SYS_LOG_LEVEL_DEBUG
#define SPI_PRINT_EN      SYS_LOG_LEVEL_DEBUG
#define MAX30003_PRINT_EN SYS_LOG_LEVEL_DEBUG
#define OPT_PARSER_EN     SYS_LOG_LEVEL_DEBUG

#else
#define MAIN_PRINT_EN     SYS_LOG_LEVEL_INFO
#define SPI_PRINT_EN      SYS_LOG_LEVEL_INFO
#define MAX30003_PRINT_EN SYS_LOG_LEVEL_INFO
#define OPT_PARSER_EN     SYS_LOG_LEVEL_INFO

#endif

#if defined __cplusplus
}
#endif

#endif /* INC_LOG_DBG_EN_H_ */
