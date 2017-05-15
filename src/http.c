#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "proxy.h"
#include "http.h"
#include "log.h"
#include "global.h"

void http_init(char * buff, int len)
{
	char addr[16] = {'\0'};
	for ( int i = 0; i < len; ++i )
	{
		addr[i] = buff[i];
	}
	if ( inet_pton(AF_INET, (const char*)addr, &g_remote_addr.sin_addr) != 1 )
	{
		perror("Wrong ip address!\n");
		exit(-1);
	}

	memset(&g_local_addr, 0, sizeof(struct sockaddr_in));
	g_local_addr.sin_family = AF_INET;
	g_local_addr.sin_addr.s_addr = htonl(0);
	g_local_addr.sin_port = htons(8080);

	memset(&g_remote_addr, 0, sizeof(struct sockaddr_in));
	g_remote_addr.sin_family = AF_INET;
	g_remote_addr.sin_port = htons(80);

	proxy_init();
	return ;
}

void parse(void)
{
	printf("123");
	return ;
}
