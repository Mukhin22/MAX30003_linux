/**
 * \file common.h
 *
 * \brief This file contains common check methods
 *
 * \author Anton Mukhin
 */

#ifndef INC_COMMON_CHECK_H_
#define INC_COMMON_CHECK_H_

//#define TEST
/**
 * \brief get array size for one-dimensional array
 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
/**
 * \brief reg = target variable or register, bit = bit to set or clear
 */
#define CLR_BIT(reg, bit) ((reg) &= ~((1) << (bit)))
#define SET_BIT(reg, bit) ((reg) |= ((1) << (bit)))

#define MULTIBIT_SET(addr, mask, value) \
    (((addr) & ((0xff) ^ (mask))) | ((value) & (mask)))

/**
 * \brief reg = target variable or register, mask = mask to set or clear
 */
#define BITMASK_SET(reg, mask)       ((reg) |= (mask))
#define BITMASK_CLEAR(reg, mask)     ((reg) &= (~(mask)))
#define BITMASK_FLIP(reg, mask)      ((reg) ^= (mask))
#define BITMASK_CHECK_ALL(reg, mask) (!(~(reg) & (mask)))
#define BITMASK_CHECK_ANY(reg, mask) ((reg) & (mask))

/**
* \brief macro to check  errors of ret_code_t type
*/
#define CHECK_CODE_ERR(ret)                                               \
    do {                                                                  \
        if (ret != RET_CODE_SUCCESS) {                                    \
            LOG_ERR("%s function failed with code %d \n", __func__, ret); \
            exit(ret);                                                    \
        }                                                                 \
    } while (0)

#define errExit(msg)        \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)
/**
 * \brief Pointer check macro
 */
#define CHECK_PTR(_PTR, _RET_VAR, _RET_CODE) \
    do {                                     \
        if (_PTR == NULL) {                  \
            _RET_VAR = _RET_CODE;            \
            goto exit;                       \
        }                                    \
    } while (0)
/**
 * \brief Pointer check macro
 */
#define PTR_INVALID(_ptr) (_ptr == NULL)

#define EXIT_ON_NULL(_ptr)                                         \
    do {                                                           \
        if (_ptr == NULL) {                                        \
            LOG_ERR("Failed on null ptr in func %s \n", __func__); \
            exit(EXIT_FAILURE);                                    \
        }                                                          \
    } while (0)

/**
 * \brief Check ptr and return fail code on failure
 */
#define RET_ERR_ON_NULL(_ptr)                                      \
    do {                                                           \
        if (_ptr == NULL) {                                        \
            LOG_ERR("Failed on null ptr in func %s \n", __func__); \
            return RET_CODE_NULL_PTR;                              \
        }                                                          \
    } while (0)
/**
 * \brief True when return code not successful
 */
#define RET_UNSUCCESS(_ret) (_ret != RET_CODE_SUCCESS)

/**
 * \brief Continue executing code if return code successful
 * Otherwise go to exit label
 */
#define CONTINUE_ON_SUCCESS(_ret)       \
    do {                                \
        if (_ret != RET_CODE_SUCCESS) { \
            ret = _ret;                 \
            goto exit;                  \
        }                               \
    } while (0)

#endif /* INC_COMMON_CHECK_H_ */
