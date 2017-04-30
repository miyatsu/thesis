#include "cache.h"
#include "global.h"

void cache_init(char * buff, int len)
{
	for ( int i = 0; i < len; ++i )
	{
		printf("%c", buff[i]);
	}
	printf("\n");
	return ;
}
