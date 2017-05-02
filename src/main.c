#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <event2/event.h>

#include "log.h"
#include "http.h"
#include "cache.h"
#include "proxy.h"
#include "config.h"
#include "global.h"

#define GLOBAL_BUFFER_SIZE	1024

char g_buffer[GLOBAL_BUFFER_SIZE] = {'\0'};
struct event_base * g_base = NULL;
struct evconnlistener * g_listener = NULL;
struct sockaddr_in g_local_addr, g_remote_addr;

void init(void)
{
	struct event_config *cfg = event_config_new();
	if ( !cfg )
	{
		perror("event_config_new() Error!\n");
		exit(-1);
	}
	/**	Use avoid method to avoid epoll function.
	 *	Due to unsupported local file descreptor on epoll.
	 * */
	event_config_avoid_method(cfg, "epoll");
	g_base = event_base_new_with_config(cfg);
	if ( !g_base )
	{
		perror("event_base_new failed!\n");
		exit(-1);
	}
	return ;
}

int main(int argc, char *argv[])
{
	if ( 2 != argc )
	{
		perror("usage : ktrix file.conf\n");
		exit(-1);
	}
	init();
	config_init(argv[1]);
	event_base_dispatch(g_base);
	return 0;
}
