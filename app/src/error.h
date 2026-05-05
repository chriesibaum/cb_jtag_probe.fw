// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023 Chriesibaum GmbH

#ifndef _ERROR_H
#define _ERROR_H

#ifdef __cplusplus
extern "C" {
#endif


// INCLUDE FILES
#include <zephyr/kernel.h>


// GLOBAL DEFINITIONS
// Definition of global defines and typedefs (visible by all modules).

// general errors defines
#define NO_ERR										0			// no error
#define ALL_GOOD									NO_ERR		// no error on exit
#define GEN_ERR										-1			// general error
#define INIT_ERR									-2			// initialization error
#define PARAM_ERR									-3			// parameter error
#define MEM_ERR										-4			// memory error
#define COMM_ERR									-5			// communication error

/**
 ** @brief Error check and return macro
 **
 ** The macro checks the contract value and if != 0/True then it returns the parameter
 ** error_code and logs or printk()'s the error to std.
 **
 ** @param[in] contract contract value to be checked
 ** @param[in] error_code error code to be returned
 ** @param[in] message_format `printf`-like format string to log or print the message
 ** @return error_code return value if contract
 **
 ** Example:
 ** \code{.c}
 **     int32_t ret = 0;
 **
 **     // return value from some_function) = 0 if all good, neg. value if error
 **     ret = some_function()
 **     ERR_CHECK(ret, GEN_ERR, "just an error to test (err %d)", ret);
 **
 **     // return value from some_other_function() = 0 or pos value if all good,
 **     // neg. value if an error occurred
 **     ret = some_other_function()
 **     ERR_CHECK(ret < 0, GEN_ERR, "just an error to test (err %d)", ret);
 **
 **     // or return the error value from some_other_function() instead of GEN_ERR
 **     ERR_CHECK(ret < 0, ret, "just an error to test (err %d)", ret);
 ** \endcode
 **/

#if defined CONFIG_LOG || defined APP_DOXYGEN	// use LOG_ERR() if logging is enabled

#define ERR_CHECK(contract, error_code, message_format, ...) \
if ((contract)) { \
	LOG_ERR(message_format, ##__VA_ARGS__); \
	return error_code; \
}

#else					// use printk if no logging is available

#define ERR_CHECK(contract, error_code, message_format, ...) \
if ((contract)) { \
	printk("<err> "); \
	printk(message_format, ##__VA_ARGS__); \
	printk("\n"); \
	return error_code; \
}
#endif


/**
 ** @brief Error check and halt macro
 **
 ** The macro checks the contract value and if true, it logs the error or prints it to std
 ** and halts the kernel by software triggering a fatal error.
 **
 ** @param[in] contract contract value to be checked
 ** @param[in] message_format `printf`-like format string to log or print the message
 **/

#if defined CONFIG_LOG || defined APP_DOXYGEN	// use LOG_ERR() if logging is enabled

#define ERR_CHECK_HALT(contract, message_format, ...) \
if ((contract)) { \
	LOG_ERR(message_format, ##__VA_ARGS__); \
	LOG_ERR("Fatal ERROR - halting system!"); \
	k_oops(); \
}

#else					// use printk if no logging is available

#define ERR_CHECK_HALT(contract, message_format, ...) \
if ((contract)) { \
	printk("<err> "); \
	printk(message_format, ##__VA_ARGS__); \
	LOG_ERR("\nFatal ERROR - halting system!\n"); \
	k_oops(); \
}
#endif


#ifdef __cplusplus
}
#endif

#endif // _ERROR_H

/** @} */












