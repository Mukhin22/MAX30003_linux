/*
 * get_opt_parser.c
 *
 *      Author: Anton Mukhin
 */

#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include "spi.h"
#include "get_opt_parser.h"
#include "common_check.h"
#include "Log_dbg_en.h"
#include "ctype.h"
#define DEBUG_ENABLE OPT_PARSER_EN
#define DBG_TAG      "opt_parser.c"
#include "Log_dbg.h"

#define GET_OPT_FAIL (-1)

#define P_PULL_UP_VAL   1
#define P_PULL_DOWN_VAL 2
/* MAX30003 Digital High-Pass Filter Cutoff Frequency values*/
#define DHP_BYPASS_VAL   1
#define DHP_0_5_FREQ_VAL 2

/*Sample Synchronization Pulse (SAMP) Clear Behavior value*/
#define SAMP_CLR_AUTO_VAL      1
#define SAMP_CLR_ON_STATUS_VAL 2

/*ECG Channel Enable values*/
#define EN_VAL  1
#define DIS_VAL 2

#define EFIT_16_VAL 16
#define EFIT_2_VAL  2
#define EFIT_1_VAL  1

#define CLEAR_PRINT_ON_STATUS_VAL 0
#define CLEAR_PRINT_ON_RTOR_VAL   1
#define CLEAR_PRINT_ON_READ_VAL   2

/*Sample Synchronization Pulse  values*/
#define SYNCH_EVERY_SAMPLE_VAL    1
#define SYNCH_EVERY_SEC_VAL       2
#define SYNCH_EVERY_FOURTH_VAL    4
#define SYNCH_EVERY_SIXTEENTH_VAL 16

/*ECG GAIN POSSIBLE VALUES*/
#define GAIN_20  20
#define GAIN_40  40
#define GAIN_80  80
#define GAIN_160 160
/*ECG SAMPLE RATE POSSIBLE VALUES*/

#define S_RATE_128 128
#define S_RATE_256 256
#define S_RATE_512 512

/*DC Lead-Off Current Magnitude Selection values*/
#define LEADOFF_MAG_DIS 0
#define LEADOFF_MAG_5   5
#define LEADOFF_MAG_10  10
#define LEADOFF_MAG_20  20
#define LEADOFF_MAG_50  50
#define LEADOFF_MAG_100 100

/*DC Lead-Off Current Voltage Threshold values*/
#define LEADOFF_THR_300 300
#define LEADOFF_THR_400 400
#define LEADOFF_THR_450 450
#define LEADOFF_THR_500 500

/*Resistive Bias Mode Value  */
#define RBIAS_VAL_50  50
#define RBIAS_VAL_100 100
#define RBIAS_VAL_200 200

/*Enables Resistive Bias on selected input*/
#define RBIAS_NC_BOTH 0
#define RBIAS_NEG_IN  1
#define RBIAS_POS_IN  2

/*Calibration Source Frequency values  (FCAL)*/
#define FCAL_256_HZ_VAL   256000
#define FCAL_64_HZ_VAL    64000
#define FCAL_16_HZ_VAL    16000
#define FCAL_4_HZ_VAL     4000
#define FCAL_1_HZ_VAL     1000
#define FCAL_250_M_HZ_VAL 250
#define FCAL_62_M_HZ_VAL  62
#define FCAL_16_M_HZ_VAL  16

/*Open the ECGP/N Input Switch values*/
#define OPENP_ISOLATED_VAL 1
#define OPENN_ISOLATED_VAL 2
#define OPENN_CONNECTED    3
#define OPENP_CONNECTED    4

/*ECGP/N Calibration Selection*/
#define CAL_SEL_V_M_VAL    1
#define CAL_SEL_V_CALP_VAL 2
#define CAL_SEL_V_CALN_VAL 3
#define CAL_NO_CAL_SIG     4

static void print_usage(const char *prog)
{
    LOG_INFO(
            "\n-h --help 	print help for commands\n\n"

            "-D --device    device to use (default /dev/spidev0.0)\n\n"

            "-s --speed    max speed (Hz) (default 1 000 000)\n\n"

            "-b --bpw      bits per word (default 8)\n\n"

            "-L --lsb      least significant bit first\n\n"

            "-g --gain     MAX30003 gain(possible (20, 40, 80, 160) default 20\n\n"

            "-S --s_rate   MAX30003 sample rate (128, 256, 512) default 512\n\n"

            "-H --hp_freq  Digital High-pass cutoff frequency\n"
            "Possible values: 1 - 0.5 Hz, 2 - bypass\n"
            "Default bypass \n\n"

            "-e --efit     ECG FIFO Interrupt Threshold - EFIT. "
            "Interrupt triggered when EFIT threshold reached"
            "Possible values: 1, 2, 16 samples.\n"
            "Default 1 sample.\n\n"

            "-p --print_it Setting RTOR R Detect Interrupt (RRINT) "
            "Clear Behavior. \n"
            "Possible values:\n"
            "0 - CLEAR_PRINT_ON_STATUS\n"
            "1 - CLEAR_PRINT_ON_RTOR \n"
            "2 - CLEAR_PRINT_ON_READ_CYCLE\n"
            "Default: CLEAR_PRINT_ON_STATUS\n\n"

            "-a --samp_clr_auto Sample Synchronization Pulse (SAMP) Clear Behavior\n"
            "Possible values:\n"
            "1 - auto clear behavior set\n"
            "2 - clear on status\n"
            "Default: clear on status\n\n"

            "-f --ssp_freq. Sample Synchronization Pulse (SAMP)"
            " Frequency setting\n"
            "Possible values:1 - every sample, 2 - every 2nd, 4 - every 4th,"
            "16 - every sixteen\n"
            "Default: every sample\n\n"

            "-E --ecg_en ECG Channel Enable\n"
            "Possible values: 1 - enabled, 2 disabled\n"
            "Default: enabled\n\n"

            "-u --ulp_en Ultra-Low Power Lead-On Detection Enable\n"
            "Possible values: 1 - enabled, 2 disabled\n"
            "Default: enabled\n\n"

            "-l --leadoff_en DC Lead-Off Detection Enable\n"
            "Possible values: 1 - enabled, 2 disabled\n"
            "Default: enabled\n\n"

            "-P dc_pol DC Lead-Off Current Polarity\n "
            "(if current sources are enabled/connected)\n"
            "Possible values:\n"
            "1 - Positive Pullup\n"
            "2 - Positive Pulldown\n"
            "Default Positive Pullup\n\n"

            "-m --leadoff_mag - DC Lead-Off Current Magnitude Selection\n"
            "Possible values in nanoampers: 0 - disabled, 5, 10, 20, 50, 100\n"
            "If option is not set - Disable and disconnect current sources\n\n"

            "-v -leadoff_vol - DC Lead-Off Voltage Threshold Selection\n"
            "Possible values in millivolts: 300, 400, 450, 500\n"
            "If option is not set - 300 \n\n"

            "-B --rbias_en Enable and Select Resistive Lead Bias Mode\n"
            "Possible values: 1 - enabled, 2 disabled\n"
            "Default: enabled\n\n"

            "-r --rbias_val Resistive Bias Mode Value Selection\n"
            "Possible values (MegaOhm): 50, 100, 200\n"
            "If option is not set 50\n\n"

            "-i rbias_in Enables Resistive Bias on selected input"
            "Possible values:0 - not connected,  1 - Negative, 2 - Positive\n"
            "Default: 0 - Not connected both\n\n"

            "-c --cal_en Calibration Source(VCALP & VCALN) Enable\n"
            "Possible values: 1 - enabled, 2 disabled\n"
            "Default: enabled\n\n"

            "-o --cal_bipol enable Calibration Source Mode bipolar\n"
            "Possible values: 1 - bipolar, 2 unipolar\n"
            "Default: bipolar\n\n"

            "-C --cal_mag enable Calibration Source Magnitude \n"
            "Possible values: 1 - 0.50 mV, 2 - 0.25mV\n"
            "Default: 0.50 mV\n\n"

            "-F --cal_freq Calibration Source Frequency Selection (FCAL)\n"
            "Possible values milliHz:256000, 64000, 16000, 4000, 1000, 250, 62, 16\n"
            "If not set - 256000 value used \n\n"

            "-I --inv_pol Inverted input polarity enable \n"
            "Possible values: 1 - inverted, 2 non inverted\n"
            "Default: inverted\n\n"

            "-N --inp_swt Open the ECGP/N Input Switch "
            "(most often used for testing)\n"
            "Possible values:\n"
            "1 - Positive isolated\n"
            "2 - Negative isolated\n"
            "3 - Negative connected to ECG AFE\n"
            "4 - Positive connected to ECG AFE\n"
            "Default: OPENP and OPENN connected (3 + 4)\n\n"

            "-t --calp_sel ECG positive input Calibration connected to ..."
            "Possible values:"
            "1 - connected to Vmid \n"
            "2 - connected to VCALP\n"
            "3 - connected to VCALN\n"
            "4 - NO calibration signal applied\n"
            "Default: No calibration signal applied\n\n"

            "-T --caln_sel ECG negative input Calibration connected to ..."
            "Possible values:"
            "1 - connected to Vmid \n"
            "2 - connected to VCALP\n"
            "3 - connected to VCALN\n"
            "4 - NO calibration signal applied\n"
            "Default: No calibration signal applied\n\n"

    );
    LOG_INFO("Help option used - ending program\n");
    exit(EXIT_FAILURE);
}

/**
 * @brief Check option string foclear_printr digit parameters
 */
ret_code_t __check_digit_opt(const char *const opt);

ret_code_t
parse_opts(int argc, char *argv[], spi_t *const spi, ecg_data_t *const ecg_data)
{
    ret_code_t ret = RET_CODE_SUCCESS;
    /*Parameters checking section*/
    if (argc < 1) {
        LOG_ERR("Argc value is wrong \n");
        ret = RET_CODE_INVALID_PARAMS;
        goto exit;
    }
    if (PTR_INVALID(argv)) {
        LOG_ERR("argv invalid pointer\n");
        ret = RET_CODE_NULL_PTR;
        goto exit;
    }
    if (PTR_INVALID(spi)) {
        LOG_ERR("SPI handler is NULL ptr\n");
        ret = RET_CODE_NULL_PTR;
        goto exit;
    }
    if (PTR_INVALID(ecg_data)) {
        LOG_ERR("ecg_data handler is NULL ptr\n");
        ret = RET_CODE_NULL_PTR;
        goto exit;
    }

    spi->dev_name = SPI_DEVICE_NAME;
    spi->speed    = SPI_MAX_SPEED;
    int      c; /*Get opt return var*/
    uint32_t temp_val = 0;
    while (1) {
        static const struct option lopts[] = {
            { "help", 0, 0, 'h' },          { "device", 1, 0, 'D' },
            { "speed", 1, 0, 's' },         { "bpw", 1, 0, 'b' },
            { "lsb", 0, 0, 'L' },           { "gain", 1, 0, 'g' },
            { "s_rate", 1, 0, 'S' },        { "hp_freq", 1, 0, 'H' },
            { "efit", 1, 0, 'e' },          { "print_it", 1, 0, 'p' },
            { "samp_clr_auto", 1, 0, 'a' }, { "ssp_freq", 1, 0, 'f' },
            { "ecg_en", 1, 0, 'E' },        { "ulp_en", 1, 0, 'u' },
            { "leadoff_en", 1, 0, 'l' },    { "dc_pol", 1, 0, 'P' },
            { "leadoff_mag", 1, 0, 'm' },   { "leadoff_vol", 1, 0, 'v' },
            { "rbias_en", 1, 0, 'B' },      { "rbias_val", 1, 0, 'r' },
            { "rbias_in", 1, 0, 'i' },      { "cal_en", 1, 0, 'c' },
            { "cal_bipol", 1, 0, 'o' },     { "cal_mag", 1, 0, 'C' },
            { "cal_freq", 1, 0, 'F' },      { "inv_pol", 1, 0, 'I' },
            { "inp_swt", 1, 0, 'N' },       { "calp_sel", 1, 0, 't' },
            { "caln_sel", 1, 0, 'T' },      { NULL, 0, 0, 0 },
        };

        c = getopt_long(
                argc,
                argv,
                "D:s:b:i:Lg:S:H:e:p:a:f:e:u:l:P:m:v:B:r:i:c:o:C:F:I:N:t:T:",
                lopts,
                NULL);
        if (c == GET_OPT_FAIL) {
            break;
        }

        switch (c) {
        case 'h':
            print_usage(argv[0]);
            goto exit;
            break;
        case 'D':
            if (PTR_INVALID(optarg)) {
                LOG_ERR("Dev name string is wrong \n");
                ret = RET_CODE_NULL_PTR;
                goto exit;
            }
            spi->dev_name = optarg;
            break;

        case 's':
            CHECK_CODE_ERR(__check_digit_opt("speed"));
            spi->speed = atoi(optarg);
            break;

        case 'b':
            CHECK_CODE_ERR(__check_digit_opt("bits_per_word"));
            spi->bits = atoi(optarg);
            break;

        case 'L':
            BITMASK_SET(spi->lsb, SPI_LSB_FIRST);
            break;

        case 'g':
            CHECK_CODE_ERR(__check_digit_opt("bits_per_word"));
            ecg_data->gain = atoi(optarg);
            switch (ecg_data->gain) {
            case GAIN_20:
                BITMASK_CLEAR(ecg_data->cnfg_ecg, ECG_GAIN_20_RESET);
                break;
            case GAIN_40:
                BITMASK_SET(ecg_data->cnfg_ecg, ECG_GAIN_40);
                break;
            case GAIN_80:
                BITMASK_SET(ecg_data->cnfg_ecg, ECG_GAIN_80);
                break;
            case GAIN_160:
                BITMASK_SET(ecg_data->cnfg_ecg, ECG_GAIN_160);
                break;
            default:
                BITMASK_CLEAR(ecg_data->cnfg_ecg, ECG_GAIN_20_RESET);
                LOG_ERR("Wrong ECG gain value, GAIN_20 will be set\n");
                break;
            }
            break;

        case 'S': {
            CHECK_CODE_ERR(__check_digit_opt("sample_rate"));
            ecg_data->sample_rate = atoi(optarg);
            switch (ecg_data->sample_rate) {
            case S_RATE_512:
                BITMASK_CLEAR(ecg_data->cnfg_ecg, ECG_GAIN_512_RESET);
                break;
            case S_RATE_128:
                BITMASK_SET(ecg_data->cnfg_ecg, ECG_RATE_128);
                break;
            case S_RATE_256:
                BITMASK_SET(ecg_data->cnfg_ecg, ECG_RATE_256);
                break;
            default:
                LOG_ERR("Wrong ECG sample rate value, ECG_RATE_512 will be set\n");
                BITMASK_CLEAR(ecg_data->cnfg_ecg, ECG_GAIN_512_RESET);
                break;
            }
            break;
        }

        case 'H': {
            CHECK_CODE_ERR(__check_digit_opt("hp_freq"));
            ecg_data->hpf_cutoff = atoi(optarg);
            switch (ecg_data->hpf_cutoff) {
            case DHP_0_5_FREQ_VAL:
                BITMASK_SET(ecg_data->cnfg_ecg, DHPF_05_HZ);
                break;
            case DHP_BYPASS_VAL:
                BITMASK_CLEAR(ecg_data->cnfg_ecg, DHPF_BYPASS_RESET);
                break;
            default:
                LOG_ERR("Wrong ECG Channel Digital High-Pass Filter"
                        " Cutoff Frequency.\n"
                        " DHPF_BYPASS will be set\n");
                BITMASK_CLEAR(ecg_data->cnfg_ecg, DHPF_BYPASS_RESET);
                break;
            }
            break;
        }

        case 'e': {
            CHECK_CODE_ERR(__check_digit_opt("efit"));
            ecg_data->efit = atoi(optarg);
            switch (ecg_data->efit) {
            case EFIT_16_VAL:
                BITMASK_SET(ecg_data->mngr_int, EFIT_16);
                break;
            case EFIT_2_VAL:
                BITMASK_SET(ecg_data->mngr_int, EFIT_2);
                break;
            case EFIT_1_VAL:
                BITMASK_CLEAR(ecg_data->mngr_int, EFIT_1_RESET);
                break;
            default:
                LOG_ERR("Wrong EFIT threshold used. EFIT_1 will be set\n");
                BITMASK_CLEAR(ecg_data->mngr_int, EFIT_1_RESET);
                break;
            }
            break;
        }

        case 'p': {
            CHECK_CODE_ERR(__check_digit_opt("print_it"));
            ecg_data->clear_print = atoi(optarg);
            switch (ecg_data->clear_print) {
            case CLEAR_PRINT_ON_STATUS_VAL:
                BITMASK_CLEAR(ecg_data->mngr_int, CLEAR_PRINT_ON_STATUS_RESET);
                break;
            case CLEAR_PRINT_ON_RTOR_VAL:
                BITMASK_SET(ecg_data->mngr_int, CLEAR_PRINT_ON_RTOR);
                break;
            case CLEAR_PRINT_ON_READ_VAL:
                BITMASK_SET(ecg_data->mngr_int, CLEAR_PRINT_ON_READ_CYCLE);
                break;
            default:
                LOG_ERR("Wrong clear print behavior value used."
                        " CLEAR_PRINT_ON_STATUS will be set\n");
                BITMASK_CLEAR(ecg_data->mngr_int, CLEAR_PRINT_ON_STATUS_RESET);
                break;
            }
            break;
        }

        case 'a': {
            CHECK_CODE_ERR(__check_digit_opt("ssp_freq"));
            temp_val = atoi(optarg);
            switch (temp_val) {
            case SAMP_CLR_AUTO_VAL:
                BITMASK_SET(ecg_data->mngr_int, CLR_SAMP_AUTO);
                break;
            case SAMP_CLR_ON_STATUS_VAL:
                BITMASK_CLEAR(ecg_data->mngr_int, CLR_SAMP_ON_STATUS_RESET);
                break;
            default:
                LOG_ERR("Sample Synchronization Pulse (SAMP) Clear Behavior"
                        " wrong value\n"
                        " CLR_SAMP_ON_STATUS will be set\n");
                BITMASK_CLEAR(ecg_data->mngr_int, CLR_SAMP_ON_STATUS_RESET);
                break;
            }
            break;
        }

        case 'f': {
            CHECK_CODE_ERR(__check_digit_opt("ssp_freq"));
            temp_val = atoi(optarg);
            switch (temp_val) {
            case SYNCH_EVERY_SAMPLE_VAL:
                BITMASK_CLEAR(ecg_data->mngr_int, SAMP_EVERY_SAMPLE_RESET);
                break;
            case SYNCH_EVERY_SEC_VAL:
                BITMASK_SET(ecg_data->mngr_int, SAMP_EVERY_SECOND);
                break;
            case SYNCH_EVERY_FOURTH_VAL:
                BITMASK_SET(ecg_data->mngr_int, SAMP_EVERY_FOURTH);
                break;
            case SYNCH_EVERY_SIXTEENTH_VAL:
                BITMASK_SET(ecg_data->mngr_int, SAMP_EVERY_SIXTEENTH);
                break;
            default:
                LOG_ERR("Wrong Sample Synchronization Pulse (SAMP) Frequency"
                        " value used.\n"
                        " SAMP_EVERY_SAMPLE will be set\n");
                BITMASK_CLEAR(ecg_data->mngr_int, SAMP_EVERY_SAMPLE_RESET);
                break;
            }
            break;
        }

        case 'E': {
            CHECK_CODE_ERR(__check_digit_opt("ecg_en"));
            temp_val = atoi(optarg);
            switch (temp_val) {
            case EN_VAL:
                BITMASK_SET(ecg_data->cnfg_gen, ECG_CHAN_EN);
                break;
            case DIS_VAL:
                BITMASK_CLEAR(ecg_data->cnfg_gen, ECG_CHAN_DIS_RESET);
                break;
            default:
                LOG_ERR("Wrong ECG channel enable"
                        " value used.\n"
                        "Channel will be enabled\n");
                BITMASK_SET(ecg_data->cnfg_gen, ECG_CHAN_EN);
                break;
            }
            break;
        }

        case 'u': {
            CHECK_CODE_ERR(__check_digit_opt("ulp_en"));
            temp_val = atoi(optarg);
            switch (temp_val) {
            case EN_VAL:
                BITMASK_SET(ecg_data->cnfg_gen, ULP_ENABLE);
                break;
            case DIS_VAL:
                BITMASK_CLEAR(ecg_data->cnfg_gen, ULP_DIS_RESET);
                break;
            default:
                LOG_ERR("Wrong Ultra-Low Power Lead-On Detection Enable"
                        " value used.\n"
                        "Channel will be enabled\n");
                BITMASK_SET(ecg_data->cnfg_gen, ULP_ENABLE);
                break;
            }
            break;
        }

        case 'l': {
            CHECK_CODE_ERR(__check_digit_opt("leadoff_en"));
            temp_val = atoi(optarg);
            switch (temp_val) {
            case EN_VAL:
                BITMASK_SET(ecg_data->cnfg_gen, LEADOFF_EN);
                break;
            case DIS_VAL:
                BITMASK_CLEAR(ecg_data->cnfg_gen, LEADOFF_DIS_RESET);
                break;
            default:
                LOG_ERR("Wrong DC Lead-Off Detection Enable"
                        " value used.\n"
                        "Channel will be enabled\n");
                BITMASK_SET(ecg_data->cnfg_gen, LEADOFF_EN);
                break;
            }
            break;
        }

        case 'P': {
            CHECK_CODE_ERR(__check_digit_opt("dc_pol"));
            temp_val = atoi(optarg);
            switch (temp_val) {
            case P_PULL_UP_VAL:
                BITMASK_SET(ecg_data->cnfg_gen, DCLOFFP_PULL_UP);
                break;
            case P_PULL_DOWN_VAL:
                BITMASK_CLEAR(ecg_data->cnfg_gen, DCLOFFP_PULL_DOWN_RESET);
                break;
            default:
                LOG_ERR("Wrong DC Lead-Off Current Polarity"
                        " value used.\n"
                        "Positive pull up will be used\n");
                BITMASK_SET(ecg_data->cnfg_gen, DCLOFFP_PULL_UP);
                break;
            }
            break;
        }

        case 'm': {
            CHECK_CODE_ERR(__check_digit_opt("leadoff_mag"));
            ecg_data->dc_leadoff_mag = atoi(optarg);
            switch (ecg_data->dc_leadoff_mag) {
            case LEADOFF_MAG_DIS:
                BITMASK_CLEAR(ecg_data->cnfg_gen, DCLOFF_CUR_DISABLE_RESET);
                break;
            case LEADOFF_MAG_5:
                BITMASK_SET(ecg_data->cnfg_gen, DCLOFF_CUR_5_N_A);
                break;
            case LEADOFF_MAG_10:
                BITMASK_SET(ecg_data->cnfg_gen, DCLOFF_CUR_10_N_A);
                break;
            case LEADOFF_MAG_20:
                BITMASK_SET(ecg_data->cnfg_gen, DCLOFF_CUR_20_N_A);
                break;
            case LEADOFF_MAG_50:
                BITMASK_SET(ecg_data->cnfg_gen, DCLOFF_CUR_50_N_A);
                break;
            case LEADOFF_MAG_100:
                BITMASK_SET(ecg_data->cnfg_gen, DCLOFF_CUR_100_N_A);
                break;
            default:
                LOG_ERR("Wrong DC Lead-Off Current Magnitude "
                        " value used.\n"
                        " DCLOFF_CUR_DISABLED will be set\n");
                BITMASK_CLEAR(ecg_data->cnfg_gen, DCLOFF_CUR_DISABLE_RESET);
                break;
            }
            break;
        }

        case 'v': {
            CHECK_CODE_ERR(__check_digit_opt("leadoff_vol"));
            ecg_data->dc_leadoff_vol = atoi(optarg);
            switch (ecg_data->dc_leadoff_vol) {
            case LEADOFF_THR_300:
                BITMASK_CLEAR(ecg_data->cnfg_gen, DCLOFF_VTH_300_M_V_RESET);
                break;
            case LEADOFF_THR_400:
                BITMASK_SET(ecg_data->cnfg_gen, DCLOFF_VTH_400_M_V);
                break;
            case LEADOFF_THR_450:
                BITMASK_SET(ecg_data->cnfg_gen, DCLOFF_VTH_450_M_V);
                break;
            case LEADOFF_THR_500:
                BITMASK_SET(ecg_data->cnfg_gen, DCLOFF_VTH_500_M_V);
                break;
            default:
                LOG_ERR("Wrong DC Lead-Off Voltage Threshold "
                        " value used.\n"
                        " DCLOFF_VTH_300_M_V will be set\n");
                BITMASK_CLEAR(ecg_data->cnfg_gen, DCLOFF_VTH_300_M_V_RESET);
                break;
            }
            break;
        }

        case 'B': {
            CHECK_CODE_ERR(__check_digit_opt("rbias_en"));
            temp_val = atoi(optarg);
            switch (temp_val) {
            case EN_VAL:
                BITMASK_SET(ecg_data->cnfg_gen, RBIAS_ENABLED);
                break;
            case DIS_VAL:
                BITMASK_CLEAR(ecg_data->cnfg_gen, RBIAS_DISABLED_RESET);
                break;
            default:
                LOG_ERR("Enable and Select Resistive Lead Bias Mode "
                        " value used.\n"
                        "Channel will be enabled\n");
                BITMASK_SET(ecg_data->cnfg_gen, RBIAS_ENABLED);
                break;
            }
            break;
        }

        case 'r': {
            CHECK_CODE_ERR(__check_digit_opt("rbias_val"));
            ecg_data->rbias_val = atoi(optarg);
            switch (ecg_data->rbias_val) {
            case RBIAS_VAL_50:
                BITMASK_CLEAR(ecg_data->cnfg_gen, RBIAS_SEL_50_M_OHM_RESET);
                break;
            case RBIAS_VAL_100:
                BITMASK_SET(ecg_data->cnfg_gen, RBIAS_SEL_100_M_OHM);
                break;
            case RBIAS_VAL_200:
                BITMASK_SET(ecg_data->cnfg_gen, RBIAS_SEL_200_M_OHM);
                break;
            default:
                LOG_ERR("Resistive Bias Mode"
                        " value used.\n"
                        " RBIAS_SEL_50_M_OHM will be set\n");
                break;
            }
            break;
        }

        case 'i': {
            CHECK_CODE_ERR(__check_digit_opt("rbias_in"));
            ecg_data->rbias_input_con = atoi(optarg);
            switch (ecg_data->rbias_input_con) {
            case RBIAS_NC_BOTH:
                BITMASK_CLEAR(ecg_data->cnfg_gen, RBIASN_NC_RESET);
                break;
            case RBIAS_NEG_IN:
                BITMASK_SET(ecg_data->cnfg_gen, RBIASN_CON);
                break;
            case RBIAS_POS_IN:
                BITMASK_SET(ecg_data->cnfg_gen, RBIASP_CON);
                break;
            default:
                LOG_ERR("R Bias on Positive/Negative Input enable wrong"
                        " value used.\n"
                        " Not Connected for negative end positive"
                        " inputs will be set\n");
                BITMASK_CLEAR(ecg_data->cnfg_gen, RBIASN_NC_RESET);
                break;
            }
            break;
        }

        case 'F': {
            CHECK_CODE_ERR(__check_digit_opt("cal_freq"));
            ecg_data->cal_freq = atoi(optarg);
            switch (ecg_data->cal_freq) {
            case FCAL_256_HZ_VAL:
                BITMASK_CLEAR(ecg_data->cnfg_cal, FCAL_256_HZ_RESET);
                break;
            case FCAL_64_HZ_VAL:
                BITMASK_SET(ecg_data->cnfg_cal, FCAL_64_HZ);
                break;
            case FCAL_16_HZ_VAL:
                BITMASK_SET(ecg_data->cnfg_cal, FCAL_16_HZ);
                break;
            case FCAL_4_HZ_VAL:
                BITMASK_SET(ecg_data->cnfg_cal, FCAL_4_HZ);
                break;
            case FCAL_1_HZ_VAL:
                BITMASK_SET(ecg_data->cnfg_cal, FCAL_1_HZ);
                break;
            case FCAL_250_M_HZ_VAL:
                BITMASK_SET(ecg_data->cnfg_cal, FCAL_250_M_HZ);
                break;
            case FCAL_62_M_HZ_VAL:
                BITMASK_SET(ecg_data->cnfg_cal, FCAL_62_M_HZ);
                break;
            case FCAL_16_M_HZ_VAL:
                BITMASK_SET(ecg_data->cnfg_cal, FCAL_16_M_HZ);
                break;
            default:
                LOG_ERR("Calibration Source Frequency wrong"
                        " value used.\n"
                        "FCAL_256_HZ will be set\n");
                BITMASK_CLEAR(ecg_data->cnfg_cal, FCAL_256_HZ_RESET);
                break;
            }
            break;
        }

        case 'c': {
            CHECK_CODE_ERR(__check_digit_opt("cal_en"));
            temp_val = atoi(optarg);
            switch (temp_val) {
            case EN_VAL:
                BITMASK_SET(ecg_data->cnfg_cal, CALIB_SRCS_ENABLE);
                break;
            case DIS_VAL:
                BITMASK_CLEAR(ecg_data->cnfg_cal, CALIB_SRCS_DIS_RESET);
                break;
            default:
                LOG_ERR("Wrong Calibration Source (VCALP and VCALN) Enable "
                        " value used.\n"
                        "Calibration sources and modes enabled\n");
                BITMASK_SET(ecg_data->cnfg_cal, CALIB_SRCS_ENABLE);
                break;
            }
            break;
        }

        case 'o': {
            CHECK_CODE_ERR(__check_digit_opt("cal_bipol"));
            temp_val = atoi(optarg);
            switch (temp_val) {
            case EN_VAL:
                BITMASK_SET(ecg_data->cnfg_cal, VMODE_BIPOL);
                break;
            case DIS_VAL:
                BITMASK_CLEAR(ecg_data->cnfg_cal, VMODE_UNIPOL_RESET);
                break;
            default:
                LOG_ERR("Wrong calibration Source Mode Selection  "
                        " value used.\n"
                        "Bipolar, sources swing between "
                        "VMID + VMAG and VMID - VMAG\n\n");
                BITMASK_SET(ecg_data->cnfg_cal, VMODE_BIPOL);
                break;
            }
            break;
        }

        case 'C': {
            CHECK_CODE_ERR(__check_digit_opt("cal_mag"));
            temp_val = atoi(optarg);
            switch (temp_val) {
            case EN_VAL:
                BITMASK_SET(ecg_data->cnfg_cal, VMAG_0_50_M_V);
                break;
            case DIS_VAL:
                BITMASK_CLEAR(ecg_data->cnfg_cal, VMAG_0_25_M_V_RESET);
                break;
            default:
                LOG_ERR("Wrong Calibration Source Magnitude Selection (VMAG) "
                        " value used.\n"
                        "0.50mV will be used\n");
                BITMASK_SET(ecg_data->cnfg_cal, VMAG_0_50_M_V);
                break;
            }
            break;
        }

        case 'I': {
            CHECK_CODE_ERR(__check_digit_opt("inv_pol"));
            temp_val = atoi(optarg);
            switch (temp_val) {
            case EN_VAL:
                BITMASK_SET(ecg_data->cnfg_emux, POL_INV);
                break;
            case DIS_VAL:
                BITMASK_CLEAR(ecg_data->cnfg_emux, POL_NON_INV_RESET);
                break;
            default:
                LOG_ERR("ECG Input Polarity Selection (VMAG) "
                        " value used.\n"
                        "inverted polarity will be set\n");
                BITMASK_SET(ecg_data->cnfg_emux, POL_INV);
                break;
            }
            break;
        }

        case 'N': {
            CHECK_CODE_ERR(__check_digit_opt("inp_swt"));
            ecg_data->input_switch = atoi(optarg);
            switch (ecg_data->input_switch) {
            case OPENP_CONNECTED:
                BITMASK_CLEAR(ecg_data->cnfg_emux, OPENP_CONNECTED_RESET);
                break;
            case OPENN_CONNECTED:
                BITMASK_CLEAR(ecg_data->cnfg_emux, OPENN_CONNECTED_RESET);
                break;
            case OPENP_ISOLATED_VAL:
                BITMASK_SET(ecg_data->cnfg_emux, OPENN_CONNECTED_RESET);
                break;
            case OPENN_ISOLATED_VAL:
                BITMASK_SET(ecg_data->cnfg_emux, OPENN_ISOLATED);
                break;
            default:
                LOG_ERR("Open the ECGP/N Input Switch wrong"
                        " value used.\n"
                        "Negative and positive inputs will be  "
                        "internally connected to the ECG AFE Channel\n");
                BITMASK_CLEAR(ecg_data->cnfg_emux, OPENN_CONNECTED_RESET);
                BITMASK_CLEAR(ecg_data->cnfg_emux, OPENP_CONNECTED_RESET);
                break;
            }
            break;
        }

        case 't': {
            CHECK_CODE_ERR(__check_digit_opt("calp_sel"));
            ecg_data->calp_sel = atoi(optarg);
            switch (ecg_data->calp_sel) {
            case CAL_NO_CAL_SIG:
                BITMASK_SET(ecg_data->cnfg_emux, CALP_SEL_NC_RESET);
                break;
            case CAL_SEL_V_M_VAL:
                BITMASK_SET(ecg_data->cnfg_emux, CALP_SEL_V_MID);
                break;
            case CAL_SEL_V_CALP_VAL:
                BITMASK_SET(ecg_data->cnfg_emux, CALP_SEL_V_CALP);
                break;
            case CAL_SEL_V_CALN_VAL:
                BITMASK_SET(ecg_data->cnfg_emux, CALP_SEL_V_CALN);
                break;
            default:
                LOG_ERR("Positive  Calibration input Selection wrong"
                        " value used.\n"
                        "Not connected will be used\n");
                BITMASK_SET(ecg_data->cnfg_emux, CALP_SEL_NC_RESET);
                break;
            }
            break;
        }

        case 'T': {
            CHECK_CODE_ERR(__check_digit_opt("caln_sel"));
            ecg_data->caln_sel = atoi(optarg);
            switch (ecg_data->caln_sel) {
            case CAL_NO_CAL_SIG:
                BITMASK_SET(ecg_data->cnfg_emux, CALN_SEL_NC_RESET);
                break;
            case CAL_SEL_V_M_VAL:
                BITMASK_SET(ecg_data->cnfg_emux, CALN_SEL_V_MID);
                break;
            case CAL_SEL_V_CALP_VAL:
                BITMASK_SET(ecg_data->cnfg_emux, CALN_SEL_V_CALP);
                break;
            case CAL_SEL_V_CALN_VAL:
                BITMASK_SET(ecg_data->cnfg_emux, CALN_SEL_V_CALN);
                break;
            default:
                LOG_ERR("Negative Calibration input Selection wrong"
                        " value used.\n"
                        "Not connected will be used\n");
                BITMASK_SET(ecg_data->cnfg_emux, CALN_SEL_NC_RESET);
                break;
            }
            break;
        }

        default:
            print_usage(argv[0]);
        }
    }
exit:
    return ret;
}

static uint8_t __isNumber(char s[])
{
    for (int i = 0; s[i] != '\0'; i++) {
        if (isdigit(s[i]) == 0)
            return 0;
    }
    return 1;
}

ret_code_t __check_digit_opt(const char *const opt)
{
    ret_code_t ret = RET_CODE_SUCCESS;
    if (PTR_INVALID(optarg)) {
        ret = RET_CODE_NULL_PTR;
        goto exit_check;
    }
    if (!__isNumber(optarg)) {
        LOG_ERR("Argument %s must be digit.\n "
                "Make sure you've entered run arguments right \n",
                opt);
        ret = RET_CODE_INVALID_PARAMS;
        goto exit_check;
    }
exit_check:
    return ret;
}
