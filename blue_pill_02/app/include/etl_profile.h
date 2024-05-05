#ifndef APP_INCLUDE_ETL_PROFILE_H_
#define APP_INCLUDE_ETL_PROFILE_H_

/**
 * @brief Select the features or options in the ETL by defining the appropriate macros
 * More information at https://www.etlcpp.com/macros.html
 */

#define ETL_LOG_ERRORS      // ETL_ASSERT calls the error handler then asserts.
#define ETL_VERBOSE_ERRORS  // If this is defined then error messages and ouput in their long form.
#define ETL_CHECK_PUSH_POP  // Pushes and pops to containers are checked for bounds.

#ifndef NDEBUG
#define ETL_IS_DEBUG_BUILD
#endif

#endif  // APP_INCLUDE_ETL_PROFILE_H_
