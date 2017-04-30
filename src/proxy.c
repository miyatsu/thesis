#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/util.h>

#include "log.h"
#include "proxy.h"
#include "global.h"

void read_cb(struct bufferevent * bev, void * ptr)
{
	;
}

void write_cb(struct bufferevent * bev, void * prt)
{
	;
}

void event_cb(struct bufferevent * bev, short events, void * ptr)
{
	;
}

void accept_cb(struct evconnlistener * listener, evutil_socket_t sock, struct sockaddr * addr, int len, void * ptr)
{
	struct bufferevent * bev = NULL;
	bev = bufferevent_socket_new(g_base, sock, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
	if ( !bev )
	{
		LOG(LOG_LEVEL_FATAL, "Create bufferevent failed.");
		exit(-1);
	}
	bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);
	return ;
}

void proxy_init()
{
	g_listener = evconnlistener_new_bind(g_base, accept_cb, NULL, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE, -1, (struct sockaddr*)&g_addr, sizeof(struct sockaddr_in));
	if ( !g_listener )
	{
		LOG(LOG_LEVEL_FATAL, "Initial g_listener failed.");
	}
	return ;
}
