#ifndef __LOG_H__
#define __LOG_H__

#include <event2/event.h>

#define LOG_ENABLE

#ifdef LOG_ENABLE
	#define LOG(LEVEL, FORMAT, ...)	_log(LEVEL, __FILE__, __func__, __LINE__, FORMAT, ##__VA_ARGS__)
#else
	#define LOG(LEVEL, FORMAT, ...)	_log_invalid(LEVEL, __FILE__, __func__,  __LINE__, FORMAT, ##__VA_ARGS__)
#endif

typedef struct
{
	char		*address;
	unsigned int length;
	unsigned int capacity;
}log_str_t;

typedef enum
{
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
}log_level_t;

void log_init(char * buff, int len);
void _log(log_level_t level, const char * file, const char * func, uint32_t line, const char * format, ...);
void _log_invalid(log_level_t level, const char * file, const char * func, uint32_t line, const char * format, ...);

#endif	/* End of __LOG_H__  */

