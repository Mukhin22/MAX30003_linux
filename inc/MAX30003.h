/**
 * \file MAX30003.h
 *
 * \brief This file is a linux userspace driver header for MAX300003.c
 *
 */
#ifndef INC_MAX30003_H_
#define INC_MAX30003_H_

#include <stdint.h>
#include "../inc/common_types.h"
#include "../inc/spi.h"

#ifndef DEF_ECG_DATA_LEN
#define DEF_ECG_DATA_LEN 1024 /* default number of points to be read */
#endif

#define BYTES_NUM_IN_REG    4
#define WREG                0x00
#define RREG                0x01
#define TWO_LSB_BITS_MASK   3
#define THREE_LSB_BITS_MASK 7
/**
 * \brief Structure to store ecg data and register settings for MAX30003
 */
typedef struct {
    uint32_t data_ID;
    int32_t *data_arr;
    int32_t  data_len;
    int32_t  timeout_val;
    /* Registers settings */
    /*CNFG_ECG settings*/
    uint32_t cnfg_ecg;
    uint32_t gain;
    uint32_t sample_rate;
    uint32_t hpf_cutoff; /* High-Pass Filter Cutoff Frequency */
    uint32_t lpf_cutoff; /* Low-Pass Filter Cutoff Frequency */
    /*MNGR_INT register settings*/
    uint32_t mngr_int;
    uint32_t efit;
    uint32_t clear_print;
    uint32_t samp_freq;
    /*CNFG_GEN settings*/
    uint32_t cnfg_gen;
    uint32_t ulp_enable;      /*Ultra-Low Power Lead-On Detection Enable*/
    uint32_t ecg_chan_enable; /*ECG Channel Enable*/
    uint32_t leadoff_enable;  /*DC Lead-Off Detection Enable*/
    uint32_t dc_leadoff_mag;  /*DC Lead-Off Current Magnitude*/
    uint32_t dc_leadoff_vol;  /*DC Lead-Off Current Voltage Threshold*/
    uint32_t rbias_val;       /*Resistive Bias Mode Value*/
    uint32_t rbias_input_con; /*R Bias on Positive/Negative Input enable*/
    /*CNFG_CAL settings*/
    uint32_t cnfg_cal;
    uint32_t enable_calib; /*Enable calibration*/
    uint32_t cal_freq;
    /*CNFG_EMUX settings*/
    uint32_t cnfg_emux;
    uint32_t input_switch; /*Open the ECGP/N Input Switch */
    uint32_t calp_sel;     /* ECGP Calibration Selection */
    uint32_t caln_sel;     /* ECGN Calibration Selection */

    uint32_t cnfg_rtor1;
} __attribute__((packed)) ecg_data_t;

/* MAX30003 registers addresses */
#define NO_OP          0x00
#define STATUS         0x01
#define EN_INT         0x02
#define EN_INT2        0x03
#define MNGR_INT       0x04
#define MNGR_DYN       0x05
#define SW_RST         0x08
#define SYNCH          0x09
#define FIFO_RST       0x0A
#define INFO           0x0F
#define CNFG_GEN       0x10
#define CNFG_CAL       0x12
#define CNFG_EMUX      0x14
#define CNFG_ECG       0x15
#define CNFG_RTOR1     0x1D
#define CNFG_RTOR2     0x1E
#define ECG_FIFO_BURST 0x20
#define ECG_FIFO       0x21
#define RTOR           0x25

#define ECG_128_INTERVAL 8 /* interval for 128sps */
#define ECG_256_INTERVAL 4 /* interval for 256sps */
#define ECG_512_INTERVAL 2 /* interval for 512sps */

#define N_SEC_TO_M_SEC 1000000 /*Nano seconds to milli seconds*/

/**
 * @brief All the settings can be used  with clear and set bitmask operations
 */
/* MAX30003 CNFG_ECG settings */

#define CNFG_ECG_DEFAULT (ECG_GAIN_80 | ECG_RATE_128 | DHPF_05_HZ | DLPF_150_HZ)
/* MAX30003 gain settings */
#define ECG_GAIN_20_RESET (TWO_LSB_BITS_MASK << 16)
#define ECG_GAIN_40       0x00010000
#define ECG_GAIN_80       0x00020000
#define ECG_GAIN_160      0x00030000

/* MAX30003 sample rate settings */
#define ECG_GAIN_512_RESET (TWO_LSB_BITS_MASK << 22)
#define ECG_RATE_128       0x00800000
#define ECG_RATE_256       0x00400000
/* MAX30003 Digital High-Pass Filter Cutoff Frequency */
#define DHPF_BYPASS_RESET (1 << 14)
#define DHPF_05_HZ        0x00004000
/* MAX30003 ECG Channel Digital Low-Pass Filter Cutoff Frequency */
#define DLPF_BYPASS 0x00000000 /* Decimation only, no FIR filter applied */
#define DLPF_40_HZ  0x00001000 /* for all except 125sps and 128sps */
#define DLPF_100_HZ 0x00002000 /* only for 512, 256, 200, 250sps */
#define DLPF_150_HZ 0x00003000 /* only of 500, 512sps */
/* MAX30003 CNFG_ECG settings end */

/* MAX30003 STATUS register flags*/
#define EINT      0x800000 /*ECG FIFO Interrupt. EFIT threshold reached */
#define EOVF      0x400000 /*ECG FIFO Overflow*/
#define FSTINT    0x200000 /*ECG Fast Recovery Mode*/
#define DCLOFFINT 0x100000 /*DC Lead-Off Detection Interrupt.*/
#define LONINT    0x000800 /*Ultra-Low Power (ULP) Leads-On Detection Int.*/
#define RRINT     0x000400 /*ECG R to R Detector R Event Interrupt*/
#define SAMP      0x000200 /*Sample Synchronization Pulse*/
#define PLLINT    0x000100 /*PLL Unlocked Interrupt*/
/*DC Lead Off Detection Detailed Status in STATUS reg*/
#define LDOFF_PH 0x000008
#define LDOFF_PL 0x000004
#define LDOFF_NH 0x000002
#define LDOFF_NL 0x000001
/* MAX30003 STATUS register flags end*/

/* MAX30003 MNGR_INT register settings*/
#define MNGR_INT_DEFAULT 0x000004 /*sets ECG FIFO threshold*/
/*ECG FIFO Interrupt Threshold - EFIT*/
#define EFIT_16      0x780000
#define EFIT_2       0x080000
#define EFIT_1_RESET ((0b11111) << 19)
/*Setting RTOR R Detect Interrupt (RRINT) Clear Behavior*/
#define CLEAR_PRINT_ON_STATUS_RESET (TWO_LSB_BITS_MASK << 4)
#define CLEAR_PRINT_ON_RTOR         0x000010
#define CLEAR_PRINT_ON_READ_CYCLE   0x000020
/*Sample Synchronization Pulse (SAMP) Clear Behavior setting*/
#define CLR_SAMP_ON_STATUS_RESET (1 << 2)
#define CLR_SAMP_AUTO            0x000004
/*Sample Synchronization Pulse (SAMP) Frequency setting*/
#define SAMP_EVERY_SAMPLE_RESET 3
#define SAMP_EVERY_SECOND       0x000001
#define SAMP_EVERY_FOURTH       0x000002
#define SAMP_EVERY_SIXTEENTH    0x000003
/* MAX30003 MNGR_INT register settings end*/

/*MAX3003 CNFG_GEN settings*/
#define CNFG_GEN_DEFAULT 0x081007
/*Ultra-Low Power Lead-On Detection Enable*/
#define ULP_DIS_RESET (TWO_LSB_BITS_MASK << 22)
#define ULP_ENABLE    0x400000
/*ECG Channel Enable*/
#define ECG_CHAN_EN        0x080000
#define ECG_CHAN_DIS_RESET (1 << 19)
/*DC Lead-Off Detection Enable*/
#define LEADOFF_DIS_RESET (TWO_LSB_BITS_MASK << 12)
#define LEADOFF_EN        0x001000
/*DC Lead-Off Current Polarity (if current sources are enabled/connected)*/
#define DCLOFFP_PULL_UP         0x000800
#define DCLOFFP_PULL_DOWN_RESET (1 << 11)
/*DC Lead-Off Current Magnitude Selection*/
#define DCLOFF_CUR_DISABLE_RESET \
    (THREE_LSB_BITS_MASK << 8)      /*Disable Current Sources*/
#define DCLOFF_CUR_5_N_A   0x000100 /*5 nano Ampers*/
#define DCLOFF_CUR_10_N_A  0x000200
#define DCLOFF_CUR_20_N_A  0x000300
#define DCLOFF_CUR_50_N_A  0x000500
#define DCLOFF_CUR_100_N_A 0x000700
/*DC Lead-Off Voltage Threshold Selection */
#define DCLOFF_VTH_300_M_V_RESET \
    (TWO_LSB_BITS_MASK << 6) /* Threshold level 300mV*/
#define DCLOFF_VTH_400_M_V 0x000040
#define DCLOFF_VTH_450_M_V 0x000080
#define DCLOFF_VTH_500_M_V 0x0000C0
/*Enable and Select Resistive Lead Bias Mode */
#define RBIAS_DISABLED_RESET (TWO_LSB_BITS_MASK << 4)
#define RBIAS_ENABLED        0x000010
/*Resistive Bias Mode Value Selection */
#define RBIAS_SEL_50_M_OHM_RESET (TWO_LSB_BITS_MASK << 2)
#define RBIAS_SEL_100_M_OHM      0x000004
#define RBIAS_SEL_200_M_OHM      0x000008
/*Enable codes for  Resistive Bias on Positive Input*/
#define RBIASP_CON \
    0x000002 /*connected to VMID through a resistor (selected by RBIASV).*/
/*Enables Resistive Bias on Negative Input*/
#define RBIASN_NC_RESET TWO_LSB_BITS_MASK /*not resistively connected to Vmid*/
#define RBIASN_CON \
    0x000001 /*connected to VMID through a resistor (selected by RBIASV).*/
/*Enables Resistive Bias on Negative Input*/
/*MAX3003 CNFG_GEN settings end*/

/*CNFG_CAL register settings*/
#define CNFG_CAL_DEFAULT 0x720000
/*Calibration Source(VCALP & VCALN) Enable */
#define CALIB_SRCS_ENABLE    0x400000 /*Calibration Source(VCALP & VCALN) Enable */
#define CALIB_SRCS_DIS_RESET (1 << 22)
/*Calibration Source Mode Selection */
#define VMODE_UNIPOL_RESET (1 << 21)
#define VMODE_BIPOL        0x200000
/*Calibration Source Magnitude Selection (VMAG)*/
#define VMAG_0_25_M_V_RESET (1 << 20) /*0.25mV*/
#define VMAG_0_50_M_V       0x100000
/*Calibration Source Frequency Selection (FCAL)*/
#define FCAL_256_HZ_RESET (THREE_LSB_BITS_MASK << 12) /*256 Hz*/
#define FCAL_64_HZ        0x001000
#define FCAL_16_HZ        0x002000
#define FCAL_4_HZ         0x003000
#define FCAL_1_HZ         0x004000
#define FCAL_250_M_HZ     0x050000 /*250 mHz*/
#define FCAL_62_M_HZ      0x060000
#define FCAL_16_M_HZ      0x070000
/*Calibration Source Time High Selection*/
#define THIGH_50_PROC 0x000800 /*THIGH = 50% (CAL_THIGH[10:0] are ignored)*/
/*CNFG_CAL register settings end*/

/*CNFG_EMUX register settings*/
#define CNFG_EMUX_DEFAULT 0x0B0000
/*ECG Input Polarity Selection*/
#define POL_NON_INV_RESET (1 << 23)
#define POL_INV           0x800000
/*Open the ECGP/N Input Switch (most often used for testing)*/
#define OPENP_CONNECTED_RESET (1 << 21)
#define OPENN_CONNECTED_RESET (1 << 20)

#define OPENP_ISOLATED 0x200000
#define OPENN_ISOLATED 0x100000
/*ECGP/N Calibration Selection*/
#define CALP_SEL_NC_RESET (TWO_LSB_BITS_MASK << 18)
#define CALP_SEL_V_MID    0x040000 /*Input is connected to Vmid*/
#define CALP_SEL_V_CALP   0x080000
#define CALP_SEL_V_CALN   0x0C0000

#define CALN_SEL_NC_RESET (TWO_LSB_BITS_MASK << 16)
#define CALN_SEL_V_MID    0x010000 /*Input is connected to Vmid*/
#define CALN_SEL_V_CALP   0x020000
#define CALN_SEL_V_CALN   0x030000
/*CNFG_EMUX register settings end*/

/*CNFG_RTOR1 register settings*/
#define CNFG_RTOR1_DEFAULT 0x3fc600

/*CNFG_RTOR1 register settings end*/

/* Codes to be written */
#define ZERO_SEQUENCE (uint32_t)0x000000

/**
* \brief set timeout for ECG measurements
* \param ecg_data - structure with ecg measurement parameters and registers
* \param timeout_val - timeout value in seconds
* \retval ret_code_t RET_CODE_SUCCESS - no errors.
*/
ret_code_t ecg_set_timeout(ecg_data_t *const ecg_data,
                           const int32_t     timeout_val);
/**
* \brief set data len for ECG measurements
* \param ecg_data - structure with ecg measurement parameters and registers
* \param data_len - lenght of data in samples to store
* \retval ret_code_t RET_CODE_SUCCESS - no errors.
*/
ret_code_t ecg_set_data_len(ecg_data_t *const ecg_data,
                            const uint32_t    data_len);
/**
* \brief creates a handler struct for ecg measurements and settings
* \retval ecg_data - structure with ecg measurement parameters and registers
*/
ecg_data_t *ecg_create_handle(void);

/**
* \brief deletes ECG handler
* \param ecg_data - structure with ecg measurement parameters and registers
* \retval ret_code_t RET_CODE_SUCCESS - no errors.
*/
ret_code_t ecg_delete_handle(ecg_data_t **ecg_data);

/**
* \brief init ECG handler with default values
* \param ecg_data - structure with ecg measurement parameters and registers
* \retval ret_code_t RET_CODE_SUCCESS - no errors.
*/
ret_code_t ecg_init_handle(ecg_data_t *const ecg_data);

/**
* \brief prints data stored after measurements of ECG
* \param ecg_data - structure with ecg measurement parameters and registers
* \retval ret_code_t RET_CODE_SUCCESS - no errors.
*/
ret_code_t ecg_print_data(const ecg_data_t *const ecg_data);

/**
* \brief writes data to register of MAX30003
* \param ecg_data - structure with ecg measurement parameters and registers
* \param write_addr - register address to write
* \param data - data to write into the register
* \retval ret_code_t RET_CODE_SUCCESS - no errors.
*/
int max30003_write_reg(spi_t *const  self,
                       const uint8_t write_addr,
                       uint32_t      data);

/**
* \brief reads data to register of MAX30003
* \param ecg_data - structure with ecg measurement parameters and registers
* \param read_addr - register address to write
* \param read_buff - buffer to store data
* \retval ret_code_t RET_CODE_SUCCESS - no errors.
*/
int max30003_read_reg(spi_t *const   self,
                      const uint8_t  read_addr,
                      uint8_t *const read_buff);

/**
* \brief software reset of max30003
* \retval ret_code_t RET_CODE_SUCCESS - no errors.
*/
ret_code_t max30003_sw_reset(void);

/**
* \brief synch operation of max30003
* \retval ret_code_t RET_CODE_SUCCESS - no errors.
*/
ret_code_t max30003_synch(void);

/**
* \brief initialization of MAX30003 according to parameters stored in
* ecg_data_t structure
* \param ecg_data - structure with ECG measurement parameters and registers
* \retval ret_code_t RET_CODE_SUCCESS - no errors.
*/
ret_code_t max30003_init(const ecg_data_t *ecg_data);

/**
 * \brief Get single ECG point
 * \return ECG point int32_t value
 */
int32_t max30003_get_ecg_point(void);

/**
 * \brief Fills ecg_data_t->data_arr field with ecg data
 * \param[out] ecg_data_t pointer to med_data_t structure
 * \return ret_code_t
 */
ret_code_t ecg_get_data(ecg_data_t *const ecg_data);

#endif /* INC_MAX30003_H_  */
