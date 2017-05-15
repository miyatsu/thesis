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

void event_cb(struct bufferevent * bev, short events, void * ptr)
{
	;
}

void drained_write_cb(struct bufferevent * bev, void * ptr)
{
	;
}

void read_cb(struct bufferevent * bev, void * ptr)
{
	struct bufferevent * partner = (struct bufferevent *)ptr;
	struct evbuffer *src, *dst;
	size_t len;
	src = bufferevent_get_input(bev);
	len = evbuffer_get_length(src);
	if ( !partner )
	{
		evbuffer_drain(src, len);
		return ;
	}
	dst = bufferevent_get_output(partner);
	evbuffer_add_buffer(dst, src);

	if ( evbuffer_get_length(dst) >= 4096 )
	{
		/* We're giving the other side data faster than it can
		** pass it on.  Stop reading here until we have drained the
		** other side to MAX_OUTPUT/2 bytes. */
		bufferevent_setcb(partner, read_cb, drained_write_cb, event_cb, bev);
		bufferevent_setwatermark(partner, EV_WRITE, 2048, 4096);
		bufferevent_disable(bev, EV_READ);
	}
}

void write_cb(struct bufferevent * bev, void * prt)
{
	;
}

void accept_cb(struct evconnlistener * listener, evutil_socket_t sock, struct sockaddr * addr, int len, void * ptr)
{
	struct bufferevent * bev_in = NULL;
	struct bufferevent * bev_out = NULL;

	bev_in = bufferevent_socket_new(g_base, sock, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
	if ( !bev_in )
	{
		LOG(LOG_LEVEL_FATAL, "Create bufferevent failed.");
		exit(-1);
	}
	bev_out = bufferevent_socket_new(g_base, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
	if ( !bev_out )
	{
		LOG(LOG_LEVEL_FATAL, "Create bufferevent failed.");
		bufferevent_free(bev_in);
		exit(-1);
	}
	if ( bufferevent_socket_connect(bev_out, (struct sockaddr * )&g_remote_addr, sizeof(struct sockaddr_in)) < 0 )
	{
		/* Error */
		perror("Connect to remote server error.");
		exit(-1);
	}

	bufferevent_setcb(bev_in, read_cb, NULL, event_cb, bev_out);
	bufferevent_setcb(bev_out, read_cb, NULL, event_cb, bev_in);

	bufferevent_enable(bev_in, EV_READ | EV_WRITE);
	bufferevent_enable(bev_out, EV_READ | EV_WRITE);

	return ;
}

void proxy_init()
{
	g_listener = evconnlistener_new_bind(g_base, accept_cb, NULL, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE, -1, (struct sockaddr*)&g_local_addr, sizeof(struct sockaddr_in));
	if ( !g_listener )
	{
		LOG(LOG_LEVEL_FATAL, "Initial g_listener failed.");
	}
	return ;
}
