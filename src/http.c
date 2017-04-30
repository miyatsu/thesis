#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>

#include "proxy.h"
#include "http.h"
#include "log.h"
#include "global.h"

void http_init(char * buff, int len)
{
	memset(&g_addr, 0, sizeof(struct sockaddr_in));
	g_addr.sin_family = AF_INET;
	g_addr.sin_addr.s_addr = htonl(0);
	g_addr.sin_port = htons(8080);
	proxy_init();
	return ;
}

void parse(void)
{
	printf("123");
	return ;
}
