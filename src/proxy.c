#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/util.h>

#include "log.h"
#include "proxy.h"
#include "cache.h"
#include "global.h"

void read_client_cb(struct bufferevent *, void *);
void event_client_cb(struct bufferevent *, short, void *);

void cache_check(struct bufferevent * bev, char * str)
{
	struct evbuffer * evbuff = bufferevent_get_input(bev);
	size_t request_line_len;
	char * request_line = evbuffer_readln(evbuff, &request_line_len, EVBUFFER_EOL_CRLF_STRICT);

	/* When use evbuffer_readln, the first line will be remove from evbuffer, so we just simplly add it back. */
	evbuffer_prepend(evbuff, "\r\n", 2);
	evbuffer_prepend(evbuff, request_line, request_line_len);

	if ( !request_line )
	{
		LOG(LOG_LEVEL_FATAL, "CRLF error!");
		exit(-1);
	}
	get_request_line_md5((unsigned char *)request_line, str);
	free(request_line);
}


void read_client_cb(struct bufferevent * bev, void * ptr)
{
	if ( !bev )
	{
		LOG(LOG_LEVEL_FATAL, "bev == NULL!");
		exit(-1);
	}

	struct evbuffer * evbuff = bufferevent_get_input(bev);
	struct evbuffer_ptr pos = evbuffer_search(evbuff, "\r\n\r\n", 4, NULL);
	if ( pos.pos == -1 )
	{
		/* Not finished yet. */
		return ;
	}

	char str[128] = {0};
	strcpy(str, g_cache_dir);
	int len = strlen(str);
	int fd = 0;
	cache_check(bev, &str[len]);
	if ( ( fd = open(str, O_RDONLY) ) > 0 )
	{
		cache_hit(bev, fd);
	}
	else
	{
		/* Can not open file, create new file. */
		fd = open(str, O_WRONLY | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if ( fd <= 0 )
		{
			LOG(LOG_LEVEL_FATAL, "Create cache file error.");
			exit(-1);
		}
		cache_miss(bev, fd);
	}
}

void event_client_cb(struct bufferevent * bev, short events, void * ptr)
{
	if ( !ptr )
	{
		LOG(LOG_LEVEL_FATAL, "ptr == NULL!");
		exit(-1);
	}
	if ( events | ( BEV_EVENT_EOF | BEV_EVENT_ERROR ) )
	{
		if ( events | BEV_EVENT_EOF )
		{
			LOG(LOG_LEVEL_INFO, "%s called.\n", __func__);
			//bufferevent_free(bev);
			//bufferevent_enable(bev, 0);
		}
		/* No implamation here. */
	}
	return ;
}


void accept_cb(struct evconnlistener * listener, evutil_socket_t sock, struct sockaddr * addr, int len, void * ptr)
{
	struct bufferevent * bev_in = NULL;

	bev_in = bufferevent_socket_new(g_base, sock, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
	if ( !bev_in )
	{
		LOG(LOG_LEVEL_FATAL, "Create bufferevent failed.");
		exit(-1);
	}

	bufferevent_setcb(bev_in, read_client_cb, NULL, event_client_cb, NULL);

	bufferevent_enable(bev_in, EV_READ);

	return ;
}

void proxy_init()
{
	g_listener = evconnlistener_new_bind(g_base, accept_cb, NULL, LEV_OPT_REUSEABLE, -1, (struct sockaddr*)&g_local_addr, sizeof(struct sockaddr_in));
	if ( !g_listener )
	{
		LOG(LOG_LEVEL_FATAL, "Initial g_listener failed.");
	}
	return ;
}
