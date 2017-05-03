#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "md5.h"
#include "http.h"
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

/* Watch out!! This function never been tested before, be careful when call it. */
int get_request_md5(char hexstr[32], str_t uri, str_t host)
{
	MD5_CTX md5;
	unsigned char encrypt[1024] = {0};
	uint8_t decrypt[16];
	int i, j;
	for (i = 0; i < host.len; ++i)
	{
		encrypt[i] = host.ptr[i];
	}
	for ( j = 0; j < uri.len; ++j )
	{
		encrypt[i + j] = host.ptr[j];
	}

	MD5Init(&md5);
	MD5Update(&md5, encrypt, strlen(encrypt));
	MD5Final(&md5, decrypt);
	for ( i = 0; i < 16; ++i )
	{
		sprintf(&hexstr[i*2], "%02x", decrypt[i]);
	}
	return 0;
}

#ifdef DEBUG

//This example is copy from : http://www.cnblogs.com/alexthecoder/p/4990835.html

#include "md5.h"
void md5_use_example()
{
	MD5_CTX md5;
	MD5Init(&md5);
	int i;
	char encrypt[100] = {"admin"};
	unsigned char decrypt[16];
	MD5Update(&md5, encrypt, strlen(encrypt));
	MD5Final(&md5, decrypt);

	unsigned char hexstr[32];
	for ( int i = 0; i < 16; ++i )
	{
		sprintf(&hexstr[i*2], "%02x", decrypt[i]);
	}

	printf("%s\n", hexstr);
}

#endif

