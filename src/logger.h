#ifndef __LOGGER_H__
#define __LOGGER_H__

typedef enum log_level {
    LOG_QUIET   = 0,
    LOG_NORMAL  = 1,
    LOG_VERBOSE = 2,
    LOG_DEBUG   = 3
} LogLevel;

void logger_set_level(int level);

void logger_log(LogLevel level, char *format, ...);
    
#endif // __LOGGER_H__
