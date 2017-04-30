#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include <pthread.h>

#include <event2/event.h>

#include "log.h"
#include "global.h"

#define LOG_DEFAULT_BUFF_SIZE 64

static const struct timeval g_timeout = {5, 0};

int						g_log_fd = 0;
static pthread_mutex_t	g_log_mutex;

void log_cb(int fd, short what, void * arg)
{
	printf("log_cb called.\n");
	if ( !arg )
	{
		perror("In func log_cb, arg == NULL!\n");
		exit(-1);
	}
	if ( what & EV_WRITE )
	{
		if ( 0 != pthread_mutex_trylock(&g_log_mutex) )
		{
			event_base_once(g_base, g_log_fd, EV_WRITE, log_cb, arg, &g_timeout);
			return ;
		}
		/* At this point, mutex_trylock is successfuly locked. */
		char *str = (char *)arg;
		ssize_t left = strlen(str), n = 0;
		do
		{
			n = write(g_log_fd, str, left);
			if ( n > 0 )
			{
				left -= n;
			}
		}while (left);
		/* Unlock the log file. */
		pthread_mutex_unlock(&g_log_mutex);
		free(str);
		return ;
	}
	if ( what & EV_TIMEOUT )
	{
		/* If this event din't triger when timeout, reset current event. */
		event_base_once(g_base, g_log_fd, EV_WRITE, log_cb, arg, &g_timeout);
		return ;
	}
}

void log_init(char * buff, int len)
{
	char path[128] = {0};
	if ( !buff )
	{
		printf("No log file!\n");
		exit(-1);
	}
	for ( int i = 0; i < len; ++i )
	{
		path[i] = buff[i];
	}
	g_log_fd = open(path, O_WRONLY);
	if ( g_log_fd <= 0 )
	{
		printf("Can not open file %s!\n", path);
		exit(-1);
	}
	fcntl(g_log_fd, F_SETFL, fcntl(g_log_fd, F_GETFL) | O_NONBLOCK);
	
	if ( 0 != pthread_mutex_init(&g_log_mutex, NULL) )
	{
		printf("g_log_rwlock initial error!\n");
		exit(-1);
	}
	return ;
}

void _log(log_level_t level, const char * file, const char * func, uint32_t line, const char * restrict format, ...)
{
	char *str = NULL;
	int len;
	va_list ap;

	str = malloc(LOG_DEFAULT_BUFF_SIZE);
	if ( !str )
	{
		printf("Out of memory!\n");
		exit(-1);
	}
	len = sprintf(str, "[%s][%s][%d]\t", file, func, line);

	va_start(ap, format);
	vsnprintf( str + len, LOG_DEFAULT_BUFF_SIZE - len, format, ap );
	va_end(ap);

	event_base_once(g_base, g_log_fd, EV_WRITE, log_cb, (void*)str, &g_timeout);
	return ;
} 

void _log_invalid(log_level_t level, const char * file, const char * func, uint32_t line, const char * restrict format, ...)
{
	return ;
}
