
#ifndef LOG_DBG
#define LOG_DBG

#if defined __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdio.h"

#define TEXT_COLOR_NONE    ""
#define TEXT_COLOR_DEFAULT "" //\033[0m
#define TEXT_COLOR_RED     "\033[31m"
#define TEXT_COLOR_GREEN   "\033[32m"
#define TEXT_COLOR_YELLOW  "\033[33m"
#define TEXT_COLOR_BLUE    "\033[34m"
#define TEXT_COLOR_MAGENTA "\033[35m"
#define TEXT_COLOR_CYAN    "\033[36m"
#define TEXT_COLOR_GRAY    "\033[37m"

#define SYS_LOG_TAG_ERR "[ERR]"
#define SYS_LOG_TAG_WRN "[WRN]"
#define SYS_LOG_TAG_INF "[INF]"
#define SYS_LOG_TAG_DBG "[DBG]"

#define SYS_LOG_TAG_TRC "[TRC]"

#define HEXDUMP_COLUMN_NUM 8
#define TRUE               1

#undef LOG_DBG
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERR
#undef LOG_TRACE
#undef APP_HEXDUPM_DEBUG

#ifndef DEBUG_ENABLE
#define DEBUG_ENABLE SYS_LOG_LEVEL_ERROR
#endif

#ifndef DBG_TAG
#define DBG_TAG "Default"
#endif

#ifndef DBG_TEXT_COLOR
#define DBG_TEXT_COLOR TEXT_COLOR_GREEN
#endif
/*
#if (DEBUG_ENABLE >= SYS_LOG_LEVEL_DEBUG)
#define APP_HEXDUPM_DEBUG(FORMAT, p_data, len)                         \
  vPrintf(SYS_LOG_TAG_DBG DBG_TAG ": %s: " FORMAT "\r\n",__func__);              \
  vPrintf(DBG_TEXT_COLOR);                                       \
  hexdump_debug((p_data), (len));                                \
  vPrintf(TEXT_COLOR_DEFAULT);
#endif
  */
void hexdump_debug(const uint8_t *p_data, uint16_t length);

#if (DEBUG_ENABLE >= SYS_LOG_LEVEL_DEBUG)
#define APP_HEXDUPM_DEBUG(FORMAT, p_data, len)                 \
    do {                                                       \
        printf(TRUE,                                           \
               SYS_LOG_TAG_DBG DBG_TAG ": %s: " FORMAT "\r\n", \
               __func__);                                      \
        printf(TRUE, DBG_TEXT_COLOR);                          \
        hexdump_debug((p_data), (len));                        \
        printf(TRUE, TEXT_COLOR_DEFAULT);                      \
    } while (0)
#endif

#define LOG_ERR(FORMAT, ...)                                  \
    printf(TEXT_COLOR_RED SYS_LOG_TAG_ERR DBG_TAG             \
           ": %s: " FORMAT                TEXT_COLOR_DEFAULT, \
           __func__,                                          \
           ##__VA_ARGS__)

#if (DEBUG_ENABLE >= SYS_LOG_LEVEL_WARNING)
#define LOG_WARN(FORMAT, ...)                                                  \
    do {                                                                       \
        printf(TRUE, SYS_LOG_TAG_WRN DBG_TAG ": %s: ", __func__);              \
        printf(TRUE, DBG_TEXT_COLOR FORMAT TEXT_COLOR_DEFAULT, ##__VA_ARGS__); \
    } while (0)
#endif
/*
#if (DEBUG_ENABLE >= SYS_LOG_LEVEL_INFO)
#define LOG_INFO(FORMAT,...) vPrintf(SYS_LOG_TAG_INF DBG_TAG  ": %s: ",__func__); \
      vPrintf(DBG_TEXT_COLOR FORMAT TEXT_COLOR_DEFAULT, ##__VA_ARGS__)
#endif
      */

#if (DEBUG_ENABLE >= SYS_LOG_LEVEL_INFO)
#define LOG_INFO(FORMAT, ...)                                            \
    do {                                                                 \
        printf(SYS_LOG_TAG_INF DBG_TAG ": %s: ", __func__);              \
        printf(DBG_TEXT_COLOR FORMAT TEXT_COLOR_DEFAULT, ##__VA_ARGS__); \
    } while (0)
#endif
/*
#if (DEBUG_ENABLE >= SYS_LOG_LEVEL_INFO)
#define LOG_INFO(FORMAT,...) \
      do{ \
        printf(SYS_LOG_TAG_INF DBG_TAG  ": %s: ","hello"); \
      }while(0)
#endif
        */
#if (DEBUG_ENABLE == SYS_LOG_LEVEL_DEBUG)
#define LOG_DBG(FORMAT, ...)                                                \
    do {                                                                    \
        printf(SYS_LOG_TAG_DBG DBG_TAG ": %s: ", __func__);                 \
        printf(TEXT_COLOR_YELLOW FORMAT TEXT_COLOR_DEFAULT, ##__VA_ARGS__); \
    } while (0)
#endif

#if (DEBUG_ENABLE == SYS_LOG_LEVEL_TRACE)
#define LOG_TRACE(FORMAT, ...)                                           \
    do {                                                                 \
        printf(SYS_LOG_TAG_TRC DBG_TAG ": %s: ", __func__);              \
        printf(DBG_TEXT_COLOR FORMAT TEXT_COLOR_DEFAULT, ##__VA_ARGS__); \
    } while (0)
#endif

#if !defined(LOG_WARN)
#define LOG_WARN(FORMAT, ...) \
    do {                      \
        ;                     \
    } while (0)
#endif

#if !defined(LOG_INFO)
#define LOG_INFO(FORMAT, ...) \
    do {                      \
        ;                     \
    } while (0)
#endif

#if !defined(LOG_DBG)
#define LOG_DBG(FORMAT, ...) \
    do {                     \
        ;                    \
    } while (0)
#endif

#if !defined(LOG_TRACE)
#define LOG_TRACE(FORMAT, ...) \
    do {                       \
        ;                      \
    } while (0)
#endif

#if !defined(APP_HEXDUPM_DEBUG)
#define APP_HEXDUPM_DEBUG(FORMAT, p_data, len) \
    do {                                       \
        ;                                      \
    } while (0)
#endif

#if defined __cplusplus
}
#endif

#endif /* LOG_DBG */
