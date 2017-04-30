#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "config.h"
#include "log.h"
#include "cache.h"
#include "http.h"
#include "global.h"

#define CONFIG_BUFF_SIZE 128

void config_init(const char * file)
{
	int config_file_fd, n, pos, start;
	char buff[CONFIG_BUFF_SIZE];
	if ( !file )
	{
		perror("Can NOT find the config file!\n");
		exit(-1);
	}
	config_file_fd = open(file, O_RDONLY);
	if ( config_file_fd <= 0 )
	{
		perror("Open config file error!\n");
		exit(-1);
	}

	while ( ( n = read(config_file_fd, buff, CONFIG_BUFF_SIZE-1) ) > 0 )
	{
		buff[n] = '\0';
		for ( pos = 0; pos < n && buff[pos] != '\0'; ++pos )
		{
			if ( buff[pos] == '#' )	/* Conment in one line. */
			{
				/* Skip current line. */
				while ( pos < n && buff[pos] != '\0' )
				{
					if ( buff[pos] == '\n' )
					{
						break;
					}
					++pos;
				}
			}
			if ( strncmp(&buff[pos], "log : ", 6) == 0 )
			{
				pos += 6;
				start = pos;
				while ( pos < n && buff[pos] != '\0' && buff[pos] != '\n' )
				{
					++pos;
				}
				log_init(&buff[start], pos-start);
			}
			else if ( strncmp(&buff[pos], "cache : ", 8) == 0 )
			{
				pos += 8;
				start = pos;
				while ( pos < n && buff[pos] != '\0' && buff[pos] != '\n' )
				{
					++pos;
				}
				cache_init(&buff[start], pos-start);
			}
			else if ( strncmp(&buff[pos], "upstream : ", 11) == 0 )
			{
				pos += 11;
				start = pos;
				while ( pos < n && buff[pos] != '\0' && buff[pos] != '\n' )
				{
					++pos;
				}
				http_init(&buff[start], pos-start);
			}
		}/* End of for. */
	}/* End of while. */
	return ;
}
