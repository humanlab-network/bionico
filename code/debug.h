#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>

#ifndef RELEASE
    #define RELEASE 0
#endif

#if RELEASE > 0
    // Undefine the log level to set it at very high level
    #ifdef LOG_LEVEL
        #undef LOG_LEVEL
    #endif

    #define LOG_LEVEL 4
#endif

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_DISABLED 4

#ifndef NO_DEBUG_HEADER
    #ifndef NO_DEBUG_COLOR
        #define DEBUG_HEADER()             printf("\x1b[32m[in %s() DEBUG] ", __FUNCTION__)
        #define INFO_HEADER()              printf("\x1b[33m[in %s() INFO] ", __FUNCTION__)
        #define WARNING_HEADER()           printf("\x1b[35m[in %s() WARNING] ", __FUNCTION__)
        #define ERROR_HEADER()             printf("\x1b[31m[in %s() ERROR] ", __FUNCTION__)
        #define NOT_IMPLEMENTED_HEADER()   printf("\x1b[36m[in %s() NOT IMPLEMENTED] ", __FUNCTION__)
    #else
        #define DEBUG_HEADER()             printf("[in %s() DEBUG] ", __FUNCTION__)
        #define INFO_HEADER()              printf("[in %s() INFO] ", __FUNCTION__)
        #define WARNING_HEADER()           printf("[in %s() WARNING] ", __FUNCTION__)
        #define ERROR_HEADER()             printf("[in %s() ERROR] ", __FUNCTION__)
        #define NOT_IMPLEMENTED_HEADER()   printf("[in %s() NOT IMPLEMENTED] ", __FUNCTION__)
    #endif
#else
    #ifndef NO_DEBUG_COLOR
        #define DEBUG_HEADER()             printf("\x1b[32m")
        #define INFO_HEADER()              printf("\x1b[33m")
        #define WARNING_HEADER()           printf("\x1b[35m")
        #define ERROR_HEADER()             printf("\x1b[31m")
        #define NOT_IMPLEMENTED_HEADER()   printf("\x1b[36m")
    #else
        #define DEBUG_HEADER()
        #define INFO_HEADER()
        #define WARNING_HEADER()
        #define ERROR_HEADER()
        #define NOT_IMPLEMENTED_HEADER()
    #endif
#endif

#ifndef NO_DEBUG_COLOR
    #define DEBUG_ENDL()               printf("\x1b[0m\n")
#else
    #define DEBUG_ENDL()               printf("\n")
#endif

#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
    #define log_debug(...) do {DEBUG_HEADER(); printf(__VA_ARGS__);DEBUG_ENDL();}while(0)
#else
    #define log_debug(...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
    #define log_info(...) do {INFO_HEADER(); printf(__VA_ARGS__);DEBUG_ENDL();}while(0)
#else
    #define log_info(...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_WARNING)
    #define log_warning(...) do {WARNING_HEADER(); printf(__VA_ARGS__);DEBUG_ENDL();}while(0)
#else
    #define log_warning(...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
    #define log_error(...) do {ERROR_HEADER(); printf(__VA_ARGS__);DEBUG_ENDL();}while(0)
    #define log_not_implemented(...) do {NOT_IMPLEMENTED_HEADER(); printf(__VA_ARGS__);DEBUG_ENDL();}while(0)
#else
    #define log_error(...)
    #define log_not_implemented(...)
#endif

#endif
