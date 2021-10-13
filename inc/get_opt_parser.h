/*
 * get_opt_parser.h
 *
 *      Author: Anton Mukhin
 * 
 */

#ifndef INC_GET_OPT_PARSER_H_
#define INC_GET_OPT_PARSER_H_
#include "MAX30003.h"

/**
* \brief parsing options that passed to command line execution
* and pass them into spi_t structure
*
* \param argc - number of CL arguments
* \param argv - buffer with CL arguments present
* \param spi - structure with spidev params
* \retval spi_dev_name "/dev/spidev0.0 for example
*/
ret_code_t parse_opts(int               argc,
                      char *            argv[],
                      spi_t *const      spi,
                      ecg_data_t *const ecg_data);

#endif /* INC_GET_OPT_PARSER_H_ */
