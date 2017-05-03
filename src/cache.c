#include "cache.h"
#include "global.h"

char g_cache_dir[128] = {0};

void cache_init(char * buff, int len)
{
	for ( int i = 0; i < len; ++i )
	{
		g_cache_dir[i] = buff[i];
	}
	return ;
}
